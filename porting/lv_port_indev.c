// #include "lv_port_indev.h"
// #include "lvgl.h"
// // #include "touch.h" // 引入你的触摸屏驱动头文件

// // 声明读取回调函数
// static void touchpad_read_cb(lv_indev_t * indev, lv_indev_data_t * data);

// void lv_port_indev_init(void)
// {
//     // A. 创建一个输入设备 (LVGL v9 新 API)
//     lv_indev_t * indev = lv_indev_create();
    
//     // B. 设置类型为指针设备 (触摸屏、鼠标均属此类)
//     lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    
//     // C. 注册读取回调函数
//     lv_indev_set_read_cb(indev, touchpad_read_cb);
// }

// 实现读取回调函数 (LVGL 会每隔十几毫秒主动调用一次这个函数)
// static void touchpad_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
// {
//     uint16_t touch_x = 0;
//     uint16_t touch_y = 0;

//     // 调用你底层的触摸读取函数。通常返回 true 表示按着，false 表示没按
//     // 这里的 Touch_GetPos 是假设函数，请替换为你 Bsp 中的实际函数
//     bool is_pressed = Touch_GetPos(&touch_x, &touch_y);

//     if(is_pressed) {
//         data->state = LV_INDEV_STATE_PRESSED;
//         data->point.x = touch_x;
//         data->point.y = touch_y;
//     } else {
//         data->state = LV_INDEV_STATE_RELEASED;
//     }
// }