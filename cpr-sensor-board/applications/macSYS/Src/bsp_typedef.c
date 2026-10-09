/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-15     teati       the first version
 */

#include "bsp_typedef.h"



// 全局变量
RecordStruct Record;
FlagStruct Flag;


// 与nRF24L01通讯相关的全局事件集
rt_event_t nrf24l01_events;
// 与rs485通讯相关的全局事件集
rt_event_t rs485_decode_events;
// mpu6050的相关参数结构体
mpu6xxxStruct mpu6xxxParameter;


void system_param_init(void)
{
    Record.nrf_if_connected = 0;
    Flag.start = 0;
    Flag.shoke_ack = 0;
    Flag.ws2812b_ack = 0;
    Flag.cc6201_ack = 0;
    Flag.last_cc6201_state = 0xFF;
}



void all_project_event_init(void)
{

}












