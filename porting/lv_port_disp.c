#include "lv_port_disp.h"
#include "lvgl.h"
#include "lcd.h"   // 引入你 Bsp 目录下的 LCD 驱动头文件

#include "string.h"

// 1. 定义屏幕分辨率（匹配 Bsp/lcd.h 中的横屏配置：LCD_W=280, LCD_H=240）
#define MY_DISP_HOR_RES    280
#define MY_DISP_VER_RES    240

#define BYTE_PER_PIXEL     2
#define DRAW_BUF_SIZE      (MY_DISP_HOR_RES * MY_DISP_VER_RES / 4 * BYTE_PER_PIXEL)

/* 开辟双缓冲区，使用 32 字节对齐以利于 Cache 维护 */
#if defined(__GNUC__)
    __attribute__((section(".RAM_D1"), aligned(32))) static uint8_t buf_1[DRAW_BUF_SIZE];
    __attribute__((section(".RAM_D1"), aligned(32))) static uint8_t buf_2[DRAW_BUF_SIZE];
#else
    __align(32) static uint8_t buf_1[DRAW_BUF_SIZE];
    __align(32) static uint8_t buf_2[DRAW_BUF_SIZE];
#endif

/* 文件作用域保存显示句柄，供 DMA 中断回调获取 */
static lv_display_t * disp;

// 2. 声明刷新回调函数
static void disp_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

void lv_port_disp_init(void)
{
    // A. 创建一个显示器对象 (LVGL v9 新 API)
    disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);

    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565_SWAPPED);

    memset(buf_1, 0, sizeof(buf_1));
    memset(buf_2, 0, sizeof(buf_2));

    // lv_display_set_buffers(disp, buf_1, buf_2, DRAW_BUF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);

    // B. 将显存分配给显示器
    lv_display_set_buffers(disp, buf_1, buf_2, DRAW_BUF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);

    // C. 注册刷新回调函数
    lv_display_set_flush_cb(disp, disp_flush_cb);

    // 在你的 lv_port_disp_init 或应用初始化中加入：
    // lv_display_t * disp = lv_display_get_default();
    // lv_display_set_monitor_enabled(disp, true); // 开启左下角的帧率监控
}

lv_display_t * lv_port_disp_get_display(void)
{
    return disp;
}

// 3. 实现刷新回调函数 (LVGL 画好了一块区域后，会调用这个函数交给你)
static void disp_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    /* 获取要刷新的区域坐标 */
    uint16_t x1 = area->x1;
    uint16_t y1 = area->y1;
    uint16_t x2 = area->x2;
    uint16_t y2 = area->y2;

    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* 【STM32H7 D-Cache 一致性】
     * CPU 写入 draw buffer 的数据可能还在 Cache 里没进 RAM。
     * DMA 去搬运 RAM 的数据时就会搬出乱码，因此必须先 Clean Cache。 */
    SCB_CleanDCache_by_Addr((uint32_t *)px_map, pixel_num * 2);

    /* 调用底层驱动，启动 DMA 传输 */
    LCD_Show_ColorBuffer_LVGL(x1, y1, x2, y2, (uint8_t *)px_map, pixel_num);

    /* 不在此处调用 lv_display_flush_ready(disp)
     * 因为 DMA 是异步的，刷新完成的通知在 SPI DMA 传输完成中断里处理
     * 参见 App/Src/interrupthandle.c 中的 HAL_SPI_TxCpltCallback */
}
