#include "Transportation.h"
// #include "lvgl.h"
#include "ui.h"
#include "timers.h"

// StateNode_t Node = STATE_NS_GO;

TrafficState_e current_state = STATE_EW_WARN;
TaskHandle_t transport_handle;
QueueHandle_t EventQueue;
TimerHandle_t LightSwitch_Timer;

void NS_G2Y(void);
void NS_Y2R(void);
void EW_G2Y(void);
void EW_Y2R(void);

StateNode_t NodeTable[] = 
{
    {STATE_NS_GO, TimeoutSwitch, NS_G2Y, STATE_NS_WARN},
    {STATE_NS_WARN, TimeoutSwitch, NS_Y2R, STATE_EW_GO},
    {STATE_EW_GO, TimeoutSwitch, EW_G2Y, STATE_EW_WARN},
    {STATE_EW_WARN, TimeoutSwitch, EW_Y2R, STATE_NS_GO},
};

void Engine(TrafficEvent_t event) {
    for (int i = 0; i < 4; i++) {
        // 查找匹配的 当前状态 和 触发事件
        if (NodeTable[i].current_state == current_state && 
            NodeTable[i].event == event) {
            
            // 如果定义了动作函数，则执行
            if (NodeTable[i].action != NULL) 
            {
                NodeTable[i].action();
            }
            
            // 更新状态机当前状态
            current_state = NodeTable[i].next_state;
            break; // 处理完毕，跳出循环
        }
    }
}

void TrafficTimerCallback(TimerHandle_t xTimer)
{
    Engine(TimeoutSwitch);
}

void Traffic_Init(void)
{
    xTimerCreate("trafficTimer", 1000, pdFALSE, 0, TrafficTimerCallback);
}

//南北绿变黄
void NS_G2Y(void)
{
    // LCD_Fill(0,0,LCD_W, LCD_H, RED);
    set_traffic_light_exclusive(&lightbox_n, LIGHT_YELLOW);
    set_traffic_light_exclusive(&lightbox_s, LIGHT_YELLOW);
    xTimerChangePeriod(LightSwitch_Timer, 500, 0);
    xTimerStart(LightSwitch_Timer, 0);
}

//南北黄变红，东西红变绿
void NS_Y2R(void)
{
    set_traffic_light_exclusive(&lightbox_n, LIGHT_RED);
    set_traffic_light_exclusive(&lightbox_s, LIGHT_RED);
    set_traffic_light_exclusive(&lightbox_e, LIGHT_GREEN);
    set_traffic_light_exclusive(&lightbox_w, LIGHT_GREEN);
    xTimerChangePeriod(LightSwitch_Timer, 2000, 0);
    xTimerStart(LightSwitch_Timer, 0);
    // LCD_Fill(0,0,LCD_W, LCD_H, YELLOW);
}

//东西绿变黄
void EW_G2Y(void)
{
    set_traffic_light_exclusive(&lightbox_e, LIGHT_YELLOW);
    set_traffic_light_exclusive(&lightbox_w, LIGHT_YELLOW);
    xTimerChangePeriod(LightSwitch_Timer, 500, 0);
    xTimerStart(LightSwitch_Timer, 0);
    // LCD_Fill(0,0,LCD_W, LCD_H, GREEN);
}

//东西黄变红，南北红变绿
void EW_Y2R(void)
{
    set_traffic_light_exclusive(&lightbox_n, LIGHT_GREEN); // 修改为绿灯
    set_traffic_light_exclusive(&lightbox_s, LIGHT_GREEN); // 修改为绿灯
    set_traffic_light_exclusive(&lightbox_e, LIGHT_RED);   // 修改为红灯
    set_traffic_light_exclusive(&lightbox_w, LIGHT_RED);   // 修改为红灯
    xTimerChangePeriod(LightSwitch_Timer, 2000, 0);
    xTimerStart(LightSwitch_Timer, 0);
    // LCD_Fill(0,0,LCD_W, LCD_H, BLUE);
}



static void Transport_Task(void* arg)
{
    TrafficEvent_t event = 0;
    xTimerStart(LightSwitch_Timer, 0);
    while (1)
    {
        xQueueReceive(EventQueue, &event, portMAX_DELAY);
        Engine(event);
        // osDelay(1);
    } 
}

void TimerCallback(TimerHandle_t Timer)
{
    TrafficEvent_t event = TimeoutSwitch;
    xQueueSend(EventQueue, &event, 0);
 
    xTimerStart(LightSwitch_Timer, 0);
}

void Transport_Init(void)
{
    EventQueue = xQueueCreate(4, sizeof(TrafficEvent_t));
    LightSwitch_Timer = xTimerCreate("LightSwitch", 100, pdFALSE, 0, TimerCallback);
    xTaskCreate(Transport_Task, "transport", 256, NULL, osPriorityAboveNormal1, &transport_handle);

    // set_traffic_light_exclusive(&lightbox_n, LIGHT_GREEN);
    // set_traffic_light_exclusive(&lightbox_s, LIGHT_GREEN);
    set_traffic_light_exclusive(&lightbox_n, LIGHT_RED);
    set_traffic_light_exclusive(&lightbox_s, LIGHT_RED);
    set_traffic_light_exclusive(&lightbox_e, LIGHT_RED);
    set_traffic_light_exclusive(&lightbox_w, LIGHT_RED);
}
