#ifndef __SCREEN_LEARNING_H__
#define __SCREEN_LEARNING_H__

#include "stm32h7xx_hal.h"
// #include "FrameManager.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"

#include "timers.h"
#include "queue.h"

// #define LIGHT_RED    0x01
// #define LIGHT_YELLOW 0x02
// #define LIGHT_GREEN  0x04

// 状态表索引枚举
typedef enum {
    STATE_NS_GO = 0,
    STATE_NS_WARN,
    STATE_EW_GO,
    STATE_EW_WARN,
    STATE_MAX
} TrafficState_e;

typedef enum
{
    TimeoutSwitch = 0,
}TrafficEvent_t;

typedef void (*TrafficAction)(void);

// 状态机节点结构体
typedef struct {
    TrafficState_e current_state;       
    TrafficEvent_t event;    // 当前状态维持的时间
    TrafficAction action;
    TrafficState_e next_state;  // 下一个状态的索引
} StateNode_t;



void ScreenLearning_Init(void);

void Transport_Init(void);

#endif