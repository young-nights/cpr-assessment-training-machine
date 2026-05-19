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
    rt_uint8_t  nrf_sending;                     // 正在发送标志(0：未发送          1：已发送)
    rt_uint8_t  nrf_connect_failed;              // 连接失败标志(0: 未进行连接  1：连接失败)
    rt_uint8_t  nrf_send_start;                  // Flag to send start command via NRF (0: idle, 1: pending)
    rt_uint8_t  main_start_status;               // Mainboard start status synced from Main
    rt_uint8_t  synced_mode;                     // Mode synced from Mainboard
    //------------------------------------------------------------
    rt_uint8_t  menu_index;                      // 菜单页面索引(0：主页面  1：菜单页面  2：其他子页面)
    rt_uint8_t  mode_data_in;                    // 数据模式(0：未进行数据传输   1：考核模式   2：竞赛模式    3：训练模式)
    rt_uint8_t  mode_data_in_set;                // 数据模式设置(0：未收到对方回复继续设置   1：收到对方回复停止设置)
    rt_uint8_t  set_press_led;                   // 数据模式中的按压LED的设置(0: 不亮   1~7：映射关系详看原理图 )
    rt_uint8_t  set_press_led_color;             // 数据模式中的按压LED的颜色信息(0：透明  1：红色  2：黄色  3：绿色)
    rt_uint16_t set_work_time;                   // 需要设置的工作时间(在设置页面设置)
    rt_uint16_t set_air_rate;                    // 需要设置的潮气达标率
    rt_uint16_t set_press_rate;                  // 需要设置的按压达标率

    rt_uint8_t  setting_mode;                    // 设置模式(0：不处于设置模式   1：处于设置模式)
    rt_uint8_t  working_mode;                    // 工作模式(0：不处于工作模式   1：处于工作模式)


}RecordStruct;
extern RecordStruct Record;



// 以下为移植时必须需要的结构体等的初始化---------------------------------------------------------------------------------------------------------
typedef struct {
    rt_uint8_t   air_rate_set;      // 潮气达标率设置标志(0：未处于设置状态   1：处于设置状态)
    rt_uint8_t   work_time_set;     // 工作时间设置标志(0：未处于设置状态   1：处于设置状态)
    rt_uint8_t   press_rate_set;    // 按压达标率设置标志(0：未处于设置状态   1：处于设置状态)


}FlagStruct;
extern FlagStruct Flag;



typedef enum
{
    Circle_Name_Body1 = 1,
    Circle_Name_Body2,
    Circle_Name_Body3,
    Circle_Name_Body4,
    Circle_Name_Body5,
    Circle_Name_Body6,
    Circle_Name_Body7,
}Circle_Name_TypeDef;


void system_param_init(void);
void all_project_event_init(void);

extern rt_event_t nrf24l01_events;

#define     EVENT_NRF24_ACK_BODY_LED    (1<<0)
#define     EVENT_NRF24_ACK_START_STATUS (1<<1)
#define     EVENT_NRF24_ACK_MODE_SYNC    (1<<2)





#endif /* APPLICATIONS_MACSYS_INC_BSP_TYPEDEF_H_ */
