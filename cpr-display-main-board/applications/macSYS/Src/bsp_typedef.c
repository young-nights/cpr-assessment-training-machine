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


// 与nRF24L01通讯相关的全局事件集
rt_event_t nrf24l01_events = RT_NULL;
// 与rs485通讯相关的全局事件集
rt_event_t rs485_decode_events = RT_NULL;


RecordStruct Record;




System_Config_t MySysCfg = {
    .current_mode = MODE_TRAIN,
    .edit_index   = 0,
    .setting_mode = 0,
    .params = {
        [MODE_TRAIN]   = {  .Number_CountDown = 320,
                            .Number_Press_Frequency = 0,
                            .Number_Press_Correct = 0,
                            .Number_Press_Error = 0,
                            .Number_Cycle = 0,
                            .Number_Blow_Time = 0,
                            .Number_Blow_Correct = 0,
                            .Number_Blow_Error = 0,
                            .press_rate = 90,
                            .tidal_rate = 85
                           },


        [MODE_ASSESS]  = {  .Number_CountDown = 230,
                            .Number_Press_Frequency = 0,
                            .Number_Press_Correct = 0,
                            .Number_Press_Error = 0,
                            .Number_Cycle = 0,
                            .Number_Blow_Time = 0,
                            .Number_Blow_Correct = 0,
                            .Number_Blow_Error = 0,
                            .press_rate = 90,
                            .tidal_rate = 85
                            },

        [MODE_COMPETE] = {  .Number_CountDown = 120,
                            .Number_Press_Frequency = 0,
                            .Number_Press_Correct = 0,
                            .Number_Press_Error = 0,
                            .Number_Cycle = 0,
                            .Number_Blow_Time = 0,
                            .Number_Blow_Correct = 0,
                            .Number_Blow_Error = 0,
                            .press_rate = 90,
                            .tidal_rate = 85 },
    }
};






void system_params_init(void)
{
    Record.touch_set_cnt = 0;
    MySysCfg.start_status = 0;
    MySysCfg.start_press_cnt = 0;
    MySysCfg.reset_press_cnt = 1;

    // 训练模式数据初始化
    MySysCfg.params[MODE_TRAIN].Number_CountDown = 300;
    MySysCfg.params[MODE_TRAIN].Number_Press_Frequency = 0;
    MySysCfg.params[MODE_TRAIN].Number_Press_Correct = 0;
    MySysCfg.params[MODE_TRAIN].Number_Press_Error = 0;
    MySysCfg.params[MODE_TRAIN].Number_Blow_Time = 0;
    MySysCfg.params[MODE_TRAIN].Number_Blow_Correct = 0;
    MySysCfg.params[MODE_TRAIN].Number_Blow_Error = 0;

    // 考核模式数据初始化
    MySysCfg.params[MODE_ASSESS].Number_CountDown = 150;
    MySysCfg.params[MODE_ASSESS].Number_Press_Frequency = 0;
    MySysCfg.params[MODE_ASSESS].Number_Press_Correct = 0;
    MySysCfg.params[MODE_ASSESS].Number_Press_Error = 0;
    MySysCfg.params[MODE_ASSESS].Number_Blow_Time = 0;
    MySysCfg.params[MODE_ASSESS].Number_Blow_Correct = 0;
    MySysCfg.params[MODE_ASSESS].Number_Blow_Error = 0;

    // 竞赛模式数据初始化
    MySysCfg.params[MODE_COMPETE].Number_CountDown = 200;
    MySysCfg.params[MODE_COMPETE].Number_Press_Frequency = 0;
    MySysCfg.params[MODE_COMPETE].Number_Press_Correct = 0;
    MySysCfg.params[MODE_COMPETE].Number_Press_Error = 0;
    MySysCfg.params[MODE_COMPETE].Number_Blow_Time = 0;
    MySysCfg.params[MODE_COMPETE].Number_Blow_Correct = 0;
    MySysCfg.params[MODE_COMPETE].Number_Blow_Error = 0;
}



void system_events_init(void)
{
    // 初始处于复位状态
    if(MySysCfg.start_status == 0)
    {
        LED_Off(LED_Name_Start);
        LED_On(LED_Name_Reset);
        LED_On(LED_Name_Train);
    }

    //-------------------------------------------------------------------
    nrf24l01_events = rt_event_create("nrf24_evt", RT_IPC_FLAG_FIFO);
    if(nrf24l01_events == RT_NULL){
        LOG_E("Failed to create nrf24_event.");
    }
    else{
        LOG_I("Succeed to create nrf24_event.");
    }
}

