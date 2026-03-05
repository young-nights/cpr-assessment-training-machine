/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-03-05     Administrator       the first version
 */

#include "bsp_sys.h"




static void ws2812b_thread_entry(void *parameter) {
    ws2812b_init();
    while (1) {
        // 示例：设置第 0 个 LED 为红色，亮度 50%
        ws2812b_set_color(0, 255, 0, 0, 128);
        // 设置其他 LED...
        ws2812b_update();  // 更新 LED
        rt_thread_mdelay(1000);  // 延时 1s
    }
}



/**
  * @brief  This is a Initialization for test func
  * @retval int
  */
rt_thread_t WS2812b_Task_Handle = RT_NULL;
int WS2812b_Thread_Init(void)
{
    WS2812b_Task_Handle = rt_thread_create("ws2812b_thread_entry", ws2812b_thread_entry, RT_NULL, 4096, 8, 100);
    /* 检查是否创建成功,成功就启动线程 */
    if(WS2812b_Task_Handle != RT_NULL)
    {
        LOG_I("LOG:%d. ws2812b_thread_entry is Succeed.",Record.ulog_cnt++);
        rt_thread_startup(WS2812b_Task_Handle);
    }
    else {
        LOG_E("LOG:%d. ws2812b_thread_entry is Failed",Record.ulog_cnt++);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(WS2812b_Thread_Init);

