/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-15     teati       the first version
 */
#ifndef APPLICATIONS_MACSYS_INC_MAINBOARD_TYPEDEF_H_

#define APPLICATIONS_MACSYS_INC_MAINBOARD_TYPEDEF_H_

#include "mainboard_sys.h"


/* nRF24L01 Event Group */
extern rt_event_t nrf24l01_events;
#define EVENT_NRF24_ACK_MODE_DATA_IN    (1 << 0)
#define EVENT_NRF24_ACK_MODE_DATA_OUT   (1 << 1)



// 以下为移植时必须需要的结构体等的初始化---------------------------------------------------------------------------------------------------------
typedef struct {
    rt_uint8_t   Empty;                          // 空值
    rt_uint16_t  kprintf_cnt;                    // 用于打印序列
    rt_uint16_t  ulog_cnt;                       // 用于日志序列
    //------------------------------------------------------------
    rt_uint8_t  mode_data_in;                    // 数据模式(0：未进行数据传输   1：考核模式   2：竞赛模式    3：训练模式)
    rt_uint8_t  touch_set_cnt;                   // 按下次数：设置按键
    rt_uint8_t  wired_connect_flag;              // 有线连接（0：无线连接   1：有线连接）

    uint8_t     sensor_connect_pending;          // Sensor 连接请求待处理
    uint8_t     remote_connect_pending;          // Remote 连接请求待处理
    uint8_t     sensor_connected;                // Sensor 是否已连接
    uint8_t     remote_connected;                // Remote 是否已连接
    uint32_t    last_sensor_heartbeat;           // Sensor 最后心跳时间戳
    uint32_t    last_remote_heartbeat;           // Remote 最后心跳时间戳
    uint8_t     last_connect_src;                // 最后一次收到连接请求的来源（0:none 1: sensor 2:remote 3:main）

}RecordStruct;
extern RecordStruct Record;





typedef enum
{
    TM1629A_SEG_1 = 1,
    TM1629A_SEG_2,
    TM1629A_SEG_3,
    TM1629A_SEG_4,
    TM1629A_SEG_5,
    TM1629A_SEG_6,
    TM1629A_SEG_7,
    TM1629A_SEG_8,
    TM1629A_SEG_9,
    TM1629A_SEG_10,
    TM1629A_SEG_11,
    TM1629A_SEG_12,
    TM1629A_SEG_13,
    TM1629A_SEG_14,
    TM1629A_SEG_15,
    TM1629A_SEG_16,
} TM1629x_SEG_SELECT;



typedef enum
{
    TOUCH_START = 1,
    TOUCH_TRAIN,
    TOUCH_ASSESS,
    TOUCH_COMPETITION,
    TOUCH_MINUS,
    TOUCH_PLUS,
    TOUCH_RESET,
    TOUCH_PRINTER,
    TOUCH_SETTING,
    TOUCH_REMOVE_FOREIGN,
    TOUCH_EMERGENCY_CALL,
    TOUCH_CHECK_BREATH,
    TOUCH_SPHYMOSCOPY,
    TOUCH_CONSCIOUS_JUDGMENT,
} Touch_Type_et;



typedef enum
{
    TM1629A_A = 0,
    TM1629A_B,
} TM16xxSelect;



/**
  * @brief  Function ON and OFF enumeration
  */
typedef enum
{
  OFF = 0u,
  ON
} FUNCTION_SWITCH;



typedef enum {
    MODE_TRAIN = 0,     // 训练模式
    MODE_ASSESS,        // 考核模式
    MODE_COMPETE,       // 竞赛模式
    MODE_MAX            // 用来循环的边界，永远放最后
} System_Mode_t;


typedef struct {
    rt_int16_t Number_CountDown;                // 倒计时
    rt_uint16_t Number_Press_Frequency;          // 按压-频率
    rt_uint16_t Number_Press_Correct;            // 按压-正确计数
    rt_uint16_t Number_Press_Error;              // 按压-错误计数

    rt_uint16_t Number_Cycle;                    // 循环数
    rt_uint16_t Number_Blow_Time;                // 潮气-时间
    rt_uint16_t Number_Blow_Correct;             // 潮气-正确计数
    rt_uint16_t Number_Blow_Error;               // 潮气-错误计数

    uint8_t  press_rate;        // 按压达标率 (%)     0~100
    uint8_t  tidal_rate;        // 潮气达标率 (%)     0~100

} Mode_Params_t;


typedef struct {
    System_Mode_t current_mode;     // 当前模式（默认训练）
    uint8_t       start_status;     // 开始状态(0：未开始       1：已开始    2：已结束 )
    uint8_t       start_press_cnt;  // 开始按键按下次数
    uint8_t       reset_press_cnt;  // 复位按键按下次数
    uint8_t       setting_mode;     // 设置状态(0：正常模式   1：设置模式 )
    uint8_t       edit_index;       // 当前正在编辑哪个参数（0=倒计时，1=按压率，2=潮气率）
    Mode_Params_t params[MODE_MAX];
} System_Config_t;
extern System_Config_t MySysCfg;



typedef enum {
    CONN_WIRED      = 0,   // 有线优先，nRF 应关闭/挂起
    CONN_WIRELESS   = 1,   // 无线模式，nRF 应运行
    CONN_UNKNOWN    = 2    // 初始/过渡状态
} conn_state_t;


// ---------------------------------------------------------------------------------------------------
extern rt_thread_t nRF24L01_Task_Handle;
extern rt_thread_t nRF24L01_Decode_Handle;
extern rt_thread_t nRF24L01_Data_Transmit_Task_Handle;





void system_params_init(void);
void system_events_init(void);



#endif /* APPLICATIONS_MACSYS_INC_MAINBOARD_TYPEDEF_H_ */
