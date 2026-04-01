/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-03-23     Administrator       the first version
 */
#include "bsp_sys.h"







void start_thread_entry(void* parameter)
{

    for(;;)
    {

        rt_thread_mdelay(20);
    }
}




int start_thread_Init(void)
{
    rt_thread_t start_task_handle = RT_NULL;
    /* 创建检查一些系统状态标志的线程  -- 优先级：25 */
    start_task_handle = rt_thread_create("start_thread_entry", start_thread_entry, RT_NULL, 1024, 11, 100);
    /* 检查是否创建成功,成功就启动线程 */
    if(start_task_handle != RT_NULL)
    {
        rt_kprintf("PRINTF:%d. start_thread_entry is Succeed!! \r\n",Record.kprintf_cnt++);
        rt_thread_startup(start_task_handle);
    }
    else {
        rt_kprintf("PRINTF:%d. start_thread_entry is Failed \r\n",Record.kprintf_cnt++);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(start_thread_Init);



