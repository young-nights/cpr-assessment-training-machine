/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-15     teati       the first version
 */


#include "bsp_sys.h"


///**
// * @brief Thread-safe print mutex
// *        rt_kprintf() uses a static buffer which is not thread-safe.
// *        This mutex protects concurrent access from multiple threads.
// */
//static rt_mutex_t s_print_mutex = RT_NULL;
//
///**
// * @brief Initialize the global print mutex (call once during system init)
// */
//void print_mutex_init(void)
//{
//    if (s_print_mutex == RT_NULL) {
//        s_print_mutex = rt_mutex_create("print_mtx", RT_IPC_FLAG_FIFO);
//        RT_ASSERT(s_print_mutex != RT_NULL);
//    }
//}
//
///**
// * @brief Lock the print mutex
// */
//void print_lock(void)
//{
//    if (s_print_mutex) {
//        rt_mutex_take(s_print_mutex, RT_WAITING_FOREVER);
//    }
//}
//
///**
// * @brief Unlock the print mutex
// */
//void print_unlock(void)
//{
//    if (s_print_mutex) {
//        rt_mutex_release(s_print_mutex);
//    }
//}
//
///**
// * @brief Thread-safe rt_kprintf override
// *        The original rt_kprintf uses a static buffer which is not thread-safe.
// *        This override wraps it with a mutex to prevent garbled output.
// */
//#include <stdarg.h>
//RT_WEAK int rt_kprintf(const char *fmt, ...)
//{
//    va_list args;
//    rt_size_t length;
//    static char rt_log_buf[RT_CONSOLEBUF_SIZE];
//
//    print_lock();
//
//    va_start(args, fmt);
//    length = rt_vsnprintf(rt_log_buf, sizeof(rt_log_buf) - 1, fmt, args);
//    if (length > RT_CONSOLEBUF_SIZE - 1)
//        length = RT_CONSOLEBUF_SIZE - 1;
//#ifdef RT_USING_DEVICE
//    if (rt_console_get_device() == RT_NULL)
//    {
//        rt_hw_console_output(rt_log_buf);
//    }
//    else
//    {
//        rt_device_write(rt_console_get_device(), 0, rt_log_buf, length);
//    }
//#else
//    rt_hw_console_output(rt_log_buf);
//#endif
//    va_end(args);
//
//    print_unlock();
//    return length;
//}
