#ifndef __SYS_H
#define __SYS_H
#include "stm8s.h"

#define PITCH_MM    2.00f             // 你的齿距
#define MM_PER_PULSE (PITCH_MM / 4.0f) // 每脉冲 0.5 mm

extern volatile int32_t depth_count;       // 脉冲计数（4×）
extern float depth_mm;                     // 当前深度（mm）
extern int8_t direction;                   // 方向：1=向下，-1=向上，0=静止

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


#endif /*__SYS_H*/