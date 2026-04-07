/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-11-07     18452       the first version
 */
#include "bsp_sys.h"

rs232_inst_t *printer_hinst;

/**
  * @brief  This thread entry is used for printer
  * @retval void
  */
void Printer_Thread_entry(void* parameter)
{


    rt_device_t rs232_printer_dev = rt_device_find("rs232-printer");
    if (rs232_printer_dev == RT_NULL) {
        rt_kprintf("rs232-printer device not found!\n");
        goto _err;
    }
    if (rt_device_open(rs232_printer_dev, RT_DEVICE_FLAG_RDWR) != RT_EOK) {
        rt_kprintf("rs232-printer open failed!\n");
        goto _err;
    }
    rs232_dev_t *pdev = (rs232_dev_t *)rs232_printer_dev;
    printer_hinst = pdev->hinst;
    rs232_dev_tmo_param_t tmo = { .ack_tmo_ms = 500, .byte_tmo_ms = 10 };
    rt_device_control(rs232_printer_dev, RS232_CTRL_SET_TMO, &tmo);

    //------------------------------------------------------------------------
    // 初始化打印机
    rd_printer_reset_cmd();
    // 进入汉字模式
    rd_printer_enter_chinese_mode_cmd();


    for(;;)
    {
        rs232_send(printer_hinst, "Hello\n", 6);

        rt_thread_mdelay(500);
    }

_err:
    LOG_E("RS232 INIT ERROR. Please reset the mcu.");
    return;

}





/**
  * @brief  This is a Initialization for printer
  * @retval int
  */
int Printer_Thread_Init(void)
{
    rt_thread_t Printer_Task_Handle = RT_NULL;
    /* 创建检查一些系统状态标志的线程  -- 优先级：25 */
    Printer_Task_Handle = rt_thread_create("Printer_Thread_entry", Printer_Thread_entry, RT_NULL, 1024, 11, 100);
    /* 检查是否创建成功,成功就启动线程 */
    if(Printer_Task_Handle != RT_NULL)
    {
        rt_kprintf("PRINTF:%d. Printer_Thread_entry is Succeed!! \r\n",Record.kprintf_cnt++);
        rt_thread_startup(Printer_Task_Handle);
    }
    else {
        rt_kprintf("PRINTF:%d. Printer_Thread_entry is Failed \r\n",Record.kprintf_cnt++);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(Printer_Thread_Init);







static int rd_test_print(void)
{
    char ch_buf_1[] = "心肺复苏操作训练考核\n";
    rd_printer_set_alignment_cmd(1);
    rs232_send(printer_hinst, ch_buf_1, strlen(ch_buf_1));

    char ch_buf_2[] = "场次号：";
    rd_printer_set_alignment_cmd(0);
    rs232_send(printer_hinst, ch_buf_2, strlen(ch_buf_2));

    return RT_EOK;
}
MSH_CMD_EXPORT(rd_test_print,print_test);







