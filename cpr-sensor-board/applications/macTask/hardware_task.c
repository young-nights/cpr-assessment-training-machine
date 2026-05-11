/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-02-28     Administrator       the first version
 */


#include "bsp_sys.h"

#define DBG_TAG "[Test]"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>


void Hard_Thread_entry(void* parameter)
{

    for(;;)
    {
        // 如果连接成功 并且 接收到 开始指令
        if(Record.nrf_if_connected == 1 && Flag.start == 1 )
        {
            //--------------------------------------------------
            // step1：模拟人进入初始状态（双侧瞳孔涣散）
            switch(Record.ws2812b_levle)
            {
                case 0: ws2812b_set_white(0); break;
                case 1: ws2812b_set_white(1); break;
                case 2: ws2812b_set_white(2); break;
                default: break;
            }

            //--------------------------------------------------
            // step2：磁传感器进入检测状态（异物检测）
            uint8_t current = CC6201_Hall_Sensor_Dout();

            if (current != Flag.last_cc6201_state)
            {
                Flag.last_cc6201_state = current;
                Flag.cc6201_ack = 1;                 // 触发 nRF 发送

                rt_kprintf("CC6201 状态翻转！当前 = %d (将发送一次)\n", current);
            }

            //--------------------------------------------------
            // step3：空心杯电机控制（颈动脉控制）
            // 这个在 rtt_system_work.c 的 Timing_10ms()函数中处理

            //--------------------------------------------------
            // step4：当开始进行一次按压时，就需要给mainboard发送按压数据了

        }

        rt_thread_mdelay(500);
    }
}


/**
  * @brief  This is a Initialization for test func
  * @retval int
  */
rt_thread_t Hard_Task_Handle = RT_NULL;
int Hard_Thread_Init(void)
{
    Hard_Task_Handle = rt_thread_create("Hard_Thread_entry", Hard_Thread_entry, RT_NULL, 4096, 23, 100);
    /* 检查是否创建成功,成功就启动线程 */
    if(Hard_Task_Handle != RT_NULL)
    {
        rt_kprintf("PRINTF:%d. Hard_Thread_entry is Succeed.\n",Record.kprintf_cnt++);
        rt_thread_startup(Hard_Task_Handle);
    }
    else {
        LOG_E("PRINTF:%d. Hard_Thread_entry is Failed",Record.kprintf_cnt++);
    }

    return RT_EOK;
}
