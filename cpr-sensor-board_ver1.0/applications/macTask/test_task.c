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




void Test_Thread_entry(void* parameter)
{
    CC6201_Hall_Sensor_Ctrl(OFF);

    for(;;)
    {
        if(CC6201_Hall_Sensor_Dout() == 1){
            Debug_LED_Ctrl(ON);
        }
        else if(CC6201_Hall_Sensor_Dout() == 0){
            Debug_LED_Ctrl(OFF);
        }
    }
}


/**
  * @brief  This is a Initialization for test func
  * @retval int
  */
rt_thread_t Test_Task_Handle = RT_NULL;
int Test_Thread_Init(void)
{
    Test_Task_Handle = rt_thread_create("Test_Thread_entry", Test_Thread_entry, RT_NULL, 4096, 8, 100);
    /* 检查是否创建成功,成功就启动线程 */
    if(Test_Task_Handle != RT_NULL)
    {
        LOG_I("LOG:%d. Test_Thread_entry is Succeed.",Record.ulog_cnt++);
        rt_thread_startup(Test_Task_Handle);
    }
    else {
        LOG_E("LOG:%d. Test_Thread_entry is Failed",Record.ulog_cnt++);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(Test_Thread_Init);
