#ifndef UI_H
#define UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef struct
{
    lv_obj_t *light_box;

    lv_obj_t *red_light;
    lv_obj_t *yellow_light;
    lv_obj_t *green_light;

    lv_obj_t *countdown_label;   // ✔ 新增

} traffic_light_t;

// 在头文件或代码顶部定义
typedef enum {
    LIGHT_ALL_OFF = 0, // 全灭
    LIGHT_RED,         // 红灯亮
    LIGHT_YELLOW,      // 黄灯亮
    LIGHT_GREEN        // 绿灯亮
} active_light_t;

extern traffic_light_t lightbox_n;
extern traffic_light_t lightbox_s;
extern traffic_light_t lightbox_e;
extern traffic_light_t lightbox_w;

/**
 * @brief Initialize LVGL core library, display porting, and create the UI.
 * Must be called once from the LVGL task before entering the timer loop.
 */
void ui_init(void);

/**
 * @brief Create the initial demo screen with a status label.
 * Called by ui_init(). Can be extended for multi-screen management.
 */
void ui_demo_screen_create(void);

void ui_verify_test(void);

void set_traffic_light_exclusive(traffic_light_t *tl, active_light_t active);

#ifdef __cplusplus
}
#endif

#endif /* UI_H */
