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




void system_param_init(void)
{
    Record.menu_index = 0;
    Record.nrf_if_connected = 0;
    Record.mode_data_in_set = 3;

}



void all_project_event_init(void)
{

    //-------------------------------------------------------------------
    nrf24l01_events = rt_event_create("nrf24_evt", RT_IPC_FLAG_FIFO);
    if(nrf24l01_events == RT_NULL){
        LOG_E("Failed to create nrf24l01_events.");
    }
    else{
        LOG_I("Succeed to create nrf24l01_events.");
    }
}












