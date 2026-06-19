#include "ui.h"
#include "lv_port_disp.h"
#include "FreeRTOS.h"
#include "task.h"

traffic_light_t lightbox_n;
traffic_light_t lightbox_s;
traffic_light_t lightbox_e;
traffic_light_t lightbox_w;
traffic_light_t test;

/* LVGL tick callback -- uses FreeRTOS 1ms tick directly */
static uint32_t lvgl_tick_get_cb(void)
{
    return xTaskGetTickCount();
}

void ui_traffic_intersection_layout(void);

void setup_vertical_light_internals(traffic_light_t *lightbox, bool reverse);
void setup_horizontal_light_internals(traffic_light_t *lightbox, bool reverse);

void ui_init(void)
{
    /* 1. Initialize LVGL core library */
    lv_init();

    /* 2. Set the tick source before any timers run */
    lv_tick_set_cb(lvgl_tick_get_cb);

    /* 3. Initialize the display porting layer */
    lv_port_disp_init();

    /* 4. Create the UI screens */
    // ui_verify_test();

    ui_traffic_intersection_layout();

    
}

void ui_demo_screen_create(void)
{
    /* Create a screen object (NULL parent = root screen) */
    lv_obj_t * scr = lv_obj_create(NULL);
    lv_scr_load(scr);

    /* Create a centered label showing LVGL is alive */
    lv_obj_t * label = lv_label_create(scr);
    lv_label_set_text(label, "LVGL Ready!\n280 x 240");
    lv_obj_center(label);
}

void ui_verify_test(void) 
{
    /* 1. 设置屏幕背景为纯蓝色 (测试整体填充和 SPI 颜色端序) */
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x0000FF), 0);

    /* 2. 创建一个显眼的大红色方块 (测试局部刷新) */
    lv_obj_t * box = lv_obj_create(lv_screen_active());
    lv_obj_set_size(box, 100, 100);
    lv_obj_align(box, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(box, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_border_width(box, 0, 0); // 去除默认边框

    /* 3. 添加简单的位移动画 (测试心跳和刷新回调) */
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, box);
    lv_anim_set_values(&a, -50, 50); // 在 Y 轴正负 50 像素内移动
    lv_anim_set_duration(&a, 1000);  // 动画时长 1000ms
    lv_anim_set_playback_time(&a, 1000); // 原路返回的时长
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE); // 无限循环
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_start(&a);
}

void ui_traffic_intersection_layout(void)
{
    lv_obj_t * scr = lv_screen_active();

    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    lv_color_t box_bg = lv_color_hex(0x4A4A4A); 

    // 1. 北侧（长边贴顶部：横向展开）
    lightbox_n.light_box = lv_obj_create(scr);
    lv_obj_set_size(lightbox_n.light_box, 70, 30);   // ✔ 改这里
    lv_obj_align(lightbox_n.light_box, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(lightbox_n.light_box, box_bg, 0);
    lv_obj_set_style_border_width(lightbox_n.light_box, 0, 0);
    setup_horizontal_light_internals(&lightbox_n, 0);   // ✔ 注意：变横向

    // 2. 南侧（长边贴底部：横向展开）
    lightbox_s.light_box = lv_obj_create(scr);
    lv_obj_set_size(lightbox_s.light_box, 70, 30);   // ✔ 改这里
    lv_obj_align(lightbox_s.light_box, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(lightbox_s.light_box, box_bg, 0);
    lv_obj_set_style_border_width(lightbox_s.light_box, 0, 0);
    setup_horizontal_light_internals(&lightbox_s, 1);   // ✔ 同样横向

    // 3. 西侧（长边贴左侧：竖向展开）✔ 不变
    lightbox_w.light_box = lv_obj_create(scr);
    lv_obj_set_size(lightbox_w.light_box, 30, 70);
    lv_obj_align(lightbox_w.light_box, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_color(lightbox_w.light_box, box_bg, 0);
    lv_obj_set_style_border_width(lightbox_w.light_box, 0, 0);
    setup_vertical_light_internals(&lightbox_w, 0);

    // 4. 东侧（长边贴右侧：竖向展开）✔ 不变
    lightbox_e.light_box = lv_obj_create(scr);
    lv_obj_set_size(lightbox_e.light_box, 30, 70);
    lv_obj_align(lightbox_e.light_box, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_color(lightbox_e.light_box, box_bg, 0);
    lv_obj_set_style_border_width(lightbox_e.light_box, 0, 0);
    setup_vertical_light_internals(&lightbox_e, 1);
}

// 底层辅助函数保持不变（仅控制单灯UI样式）
static void set_single_light(lv_obj_t *light, lv_color_t color, bool on)
{
    if (on) {
        lv_obj_set_style_bg_color(light, color, 0);
        lv_obj_set_style_shadow_width(light, 15, 0); 
        lv_obj_set_style_shadow_color(light, color, 0);
    } else {
        lv_obj_set_style_bg_color(light, lv_color_hex(0x333333), 0); 
        lv_obj_set_style_shadow_width(light, 0, 0);
    }
}

// 新增的互斥控制函数
void set_traffic_light_exclusive(traffic_light_t *tl, active_light_t active)
{
    // 第一步：将该路口的所有灯光强制设为熄灭状态
    set_single_light(tl->red_light, lv_color_hex(0xFF0000), false);
    set_single_light(tl->yellow_light, lv_color_hex(0xFFFF00), false);
    set_single_light(tl->green_light, lv_color_hex(0x00FF00), false);

    // 第二步：根据传入的 active 参数，点亮唯一的目标灯
    switch (active) {
        case LIGHT_RED:
            set_single_light(tl->red_light, lv_color_hex(0xFF0000), true);
            break;
        case LIGHT_YELLOW:
            set_single_light(tl->yellow_light, lv_color_hex(0xFFFF00), true);
            break;
        case LIGHT_GREEN:
            set_single_light(tl->green_light, lv_color_hex(0x00FF00), true);
            break;
        case LIGHT_ALL_OFF:
        default:
            // 保持全灭状态，不做任何操作
            break;
    }
}

// 垂直灯箱内部排列（南北方向）
void setup_vertical_light_internals(traffic_light_t *lightbox, bool reverse)
{
    lv_flex_flow_t flow = reverse ? LV_FLEX_FLOW_COLUMN_REVERSE : LV_FLEX_FLOW_COLUMN;

    lv_obj_set_flex_flow(lightbox->light_box, flow);
    lv_obj_set_flex_align(lightbox->light_box,
                          LV_FLEX_ALIGN_SPACE_EVENLY,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    lv_obj_set_style_pad_all(lightbox->light_box, 2, 0);

    lightbox->red_light = lv_obj_create(lightbox->light_box);
    lightbox->yellow_light = lv_obj_create(lightbox->light_box);
    lightbox->green_light = lv_obj_create(lightbox->light_box);

    lv_obj_t *lamps[] = {lightbox->red_light, lightbox->yellow_light, lightbox->green_light};

    for(int i = 0; i < 3; i++) {
        lv_obj_set_size(lamps[i], 16, 16);
        lv_obj_set_style_radius(lamps[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_border_width(lamps[i], 0, 0);
        lv_obj_set_style_bg_color(lamps[i], lv_color_hex(0x333333), 0);
    }
}

// 1. 水平灯箱内部排列（东西方向）
void setup_horizontal_light_internals(traffic_light_t *lightbox, bool mirror)
{
    lv_flex_flow_t flow = mirror ? LV_FLEX_FLOW_ROW_REVERSE : LV_FLEX_FLOW_ROW;

    lv_obj_set_flex_flow(lightbox->light_box, flow);
    lv_obj_set_flex_align(lightbox->light_box,
                          LV_FLEX_ALIGN_SPACE_EVENLY,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    lv_obj_set_style_pad_all(lightbox->light_box, 2, 0);

    lightbox->red_light = lv_obj_create(lightbox->light_box);
    lightbox->yellow_light = lv_obj_create(lightbox->light_box);
    lightbox->green_light = lv_obj_create(lightbox->light_box);

    lv_obj_t *lamps[] = {lightbox->red_light, lightbox->yellow_light, lightbox->green_light};

    for(int i = 0; i < 3; i++) {
        lv_obj_set_size(lamps[i], 16, 16);
        lv_obj_set_style_radius(lamps[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_border_width(lamps[i], 0, 0);
        lv_obj_set_style_bg_color(lamps[i], lv_color_hex(0x333333), 0);
    }
}
