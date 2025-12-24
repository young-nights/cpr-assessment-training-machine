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



RecordStruct Record;



// 与nRF24L01通讯相关的全局事件集
rt_event_t nrf24l01_events = RT_NULL;
// 与rs485通讯相关的全局事件集
rt_event_t rs485_decode_events = RT_NULL;


void all_project_event_init(void)
{
    //-------------------------------------------------------------------
    nrf24l01_events = rt_event_create("nrf24_evt", RT_IPC_FLAG_FIFO);
    if(nrf24l01_events == RT_NULL){
        LOG_E("Failed to create nrf24_event.");
    }
    else{
        LOG_I("Succeed to create nrf24_event.");
    }

}



