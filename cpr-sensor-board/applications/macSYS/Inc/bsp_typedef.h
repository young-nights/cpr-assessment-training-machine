/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-15     teati       the first version
 */
#ifndef APPLICATIONS_MACSYS_INC_BSP_TYPEDEF_H_
#define APPLICATIONS_MACSYS_INC_BSP_TYPEDEF_H_

#include "bsp_sys.h"


// 以下为移植时必须需要的结构体等的初始化---------------------------------------------------------------------------------------------------------
typedef struct {
    rt_uint8_t   Empty;                          // 空值
    rt_uint16_t  kprintf_cnt;                    // 用于打印序列
    rt_uint32_t  ulog_cnt;                       // ulog打印序列
    //------------------------------------------------------------
    rt_uint8_t  touch_down_flag;                 // 触摸按下标志
    rt_uint8_t  touch_fingers;                   // 触摸报点数
    rt_uint8_t  nrf_if_connected;                // 是否建立连接(0：未建立连接  1：已建立连接)
    rt_uint8_t  nrf_rec_start_cmd;               // 接收到开始指令的标志(0：未开始    1：已开始)

    //------------------------------------------------------------
    rt_uint16_t set_work_time;                   // 需要设置的工作时间(在设置页面设置)
    rt_uint16_t set_air_rate;                    // 需要设置的潮气达标率
    rt_uint16_t set_press_rate;                  // 需要设置的按压达标率

    rt_uint8_t  setting_mode;                    // 设置模式(0：不处于设置模式   1：处于设置模式)
    rt_uint8_t  working_mode;                    // 工作模式(0：不处于工作模式   1：处于工作模式)

    rt_uint8_t  ws2812b_levle;                   // WS2812B的灯光亮度等级：0~2
    rt_uint8_t  motor_work_sta;                  // Motor的工作模式：0.关闭  1.随按压频率  2.正常心跳模式
    rt_uint8_t  body_led_type;                   // LED的类型：0.全关闭  1.上 2.下 3.左 4.右 5.中间 6.中偏下 7.中偏上
}RecordStruct;
extern RecordStruct Record;




typedef struct {

    rt_uint8_t  if_start_master_cali_process;       // 是否开启总校准流程            （0： 不开启       1：开启）
    rt_uint8_t  if_finish_master_cali_process;      // 是否完成总校准流程            （0： 未完成       1：已完成）
    rt_uint8_t  if_start_gyro_cali_process;         // 是否开启陀螺仪静态校准    （0： 不开启       1：开启）
    rt_uint8_t  if_finish_gyro_cali_process;        // 是否完成陀螺仪静态校准    （0： 未完成       1：已完成）
    rt_uint8_t  if_start_x_axis_positive_process;   // 是否开启x轴正方向校准      （0： 不开启       1：开启）
    rt_uint8_t  if_finih_x_axis_negetive_process;   // 是否完成x轴正方向校准      （0： 未完成       1：已完成）
    rt_uint8_t  if_start_y_axis_positive_process;   // 是否开启y轴正方向校准      （0： 不开启       1：开启）
    rt_uint8_t  if_finih_y_axis_negetive_process;   // 是否完成y轴正方向校准      （0： 未完成       1：已完成）
    rt_uint8_t  if_start_z_axis_positive_process;   // 是否开启z轴正方向校准      （0： 不开启       1：开启）
    rt_uint8_t  if_finih_z_axis_negetive_process;   // 是否完成z轴正方向校准      （0： 未完成       1：已完成）

}mpu6xxxStruct;
extern mpu6xxxStruct mpu6xxxParameter;




// 欧拉角数据结构体
typedef struct {
    float pitch;
    float roll;
    float yaw;
}EulerAngles;
extern EulerAngles carEulerAngles;


/**
  * @brief  枚举类型
  * @param  None
  */
typedef enum
{
    nothing = 0,
    x_axis_interchangeable_y_axis,

}transform_coordinates_StructType;




// 以下为移植时必须需要的结构体等的初始化---------------------------------------------------------------------------------------------------------
typedef struct {
    rt_uint8_t   air_rate_set;      // 潮气达标率设置标志(0：未处于设置状态   1：处于设置状态)
    rt_uint8_t   work_time_set;     // 工作时间设置标志(0：未处于设置状态   1：处于设置状态)
    rt_uint8_t   press_rate_set;    // 按压达标率设置标志(0：未处于设置状态   1：处于设置状态)
    rt_uint8_t   start;             // 开始标志（0：未开始   1：已开始）
    rt_uint8_t   shoke_ack;         // 震动标志（0：未回应   1：已回应）
    rt_uint8_t   ws2812b_ack;       // RGB标志 （0：未回应   1：已回应）
    rt_uint8_t   motor_ack;         // 电机标志（0：未回应   1：已回应）
    rt_uint8_t   cc6201_ack;        // 磁传感器（0：未回应   1：已回应）
    rt_uint8_t   last_cc6201_state; // 上次 Hall 传感器状态（0/1）
}FlagStruct;
extern FlagStruct Flag;



typedef enum
{
    ON = 1,
    OFF = 0,
}SWITCH_et;


/* 按压位置枚举（与 mainboard 对应） */
typedef enum {
    BODY_LED_OFF      = 0,   // 全关闭
    BODY_LED_UP       = 1,
    BODY_LED_DOWN     = 2,
    BODY_LED_LEFT     = 3,
    BODY_LED_RIGHT    = 4,
    BODY_LED_MID      = 5,
    BODY_LED_MID_DOWN = 6,   // 下中中间
    BODY_LED_MID_UP   = 7    // 上中中间
} body_led_type_et;


void system_param_init(void);
void Debug_LED_Ctrl(SWITCH_et sta);
void CC6201_Hall_Sensor_Ctrl(SWITCH_et sta);

extern rt_event_t nrf24l01_events;

/* Thread-safe print functions */
void print_mutex_init(void);
void print_lock(void);
void print_unlock(void);


#endif /* APPLICATIONS_MACSYS_INC_BSP_TYPEDEF_H_ */
