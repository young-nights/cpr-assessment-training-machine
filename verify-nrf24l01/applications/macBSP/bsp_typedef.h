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
    rt_uint8_t   nrf_connected;
    rt_uint8_t   nRF24_tx_pending;
    rt_uint8_t   nRF24_tx_pressing;
    rt_uint8_t   mode_data_in;                   // 数据模式(0：未进行数据传输   1：考核模式   2：竞赛模式    3：训练模式)
    rt_uint8_t   pressed;
    rt_uint8_t   pressed_data;
    rt_uint8_t   tidal;
    rt_uint8_t   tidal_data;


}RecordStruct;
extern RecordStruct Record;




typedef enum
{
    AI12_KEY_NONE = 0,
    AI12_KEY_1,
    AI12_KEY_2,
    AI12_KEY_3,
    AI12_KEY_4,
    AI12_KEY_5,
    AI12_KEY_6,
    AI12_KEY_7,
    AI12_KEY_8,
    AI12_KEY_9,
    AI12_KEY_10,
    AI12_KEY_11,
    AI12_KEY_12
} AI12_Key_t;



/* nRF24L01 Event Group */
extern rt_event_t nrf24l01_events;
#define EVENT_NRF24_ACK_MODE_DATA_IN    (1 << 0)
#define EVENT_NRF24_ACK_MODE_DATA_OUT   (1 << 1)




void all_project_event_init(void);
int nRF24L01_Thread_Init(void);



#endif /* APPLICATIONS_MACSYS_INC_BSP_TYPEDEF_H_ */
