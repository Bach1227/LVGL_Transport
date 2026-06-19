#ifndef LV_PORT_DISP_H
#define LV_PORT_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 * INCLUDES
 *********************/
// 如果头文件中不需要使用特定的类型，这里可以保持为空
// 所有的实现细节都在 .c 文件中处理

#include "lvgl.h"

/*********************
 * GLOBAL PROTOTYPES
 *********************/
/**
 * @brief 初始化 LVGL 的显示接口
 * 该函数将被 main.c 调用，用于注册显示设备、分配显存及挂载刷新回调。
 */
void lv_port_disp_init(void);

/**
 * @brief Get the LVGL display handle.
 * Used by the DMA complete interrupt to signal flush ready.
 * @return Pointer to the LVGL display object.
 */
lv_display_t * lv_port_disp_get_display(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_PORT_DISP_H */