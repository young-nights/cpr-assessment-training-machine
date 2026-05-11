#ifndef __SYS_H
#define __SYS_H
#include "stm8s.h"

#define MM_PER_PULSE_01  5        /* 每脉冲 0.5mm = 5 (0.1mm单位) */

extern volatile int32_t depth_count_press;    // 按压脉冲计数（正=下压，负=回弹）
extern volatile int32_t depth_count_blow;     // 吹气脉冲计数（正=充气，负=泄气）
extern volatile int8_t direction_press;       // 按压方向: -1=回弹, 0=静止, 1=下压
extern volatile int8_t direction_blow;        // 吹气方向: -1=泄气, 0=静止, 1=充气
extern volatile uint32_t g_system_tick_ms;    // 全局系统滴答(ms)

/* 光栅板状态机 */
typedef enum {
    RASTER_IDLE = 0,      // 待机状态
    RASTER_ACTIVE         // 采集中
} raster_state_t;
extern volatile raster_state_t raster_state;

/*System header file******************/
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

/*Custom header file*****************/
#include "app_general_tim.h"
#include "app_usart.h"
#include "app_bsp.h"
#include "app_timming.h"
#include "app_message.h"
/*GPIO Port marco definition********/
#include "stm8s.h"
#include "stm8s_uart1.h"
#include "stm8s_gpio.h"
#include "stm8s_flash.h"


void System_Clock_Config(void);

#endif /*__SYS_H */
