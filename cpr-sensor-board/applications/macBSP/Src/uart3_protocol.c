/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-02-05     18452       the first version
 */
#include "uart3_protocol.h"


#if 0



#define RT_SERIAL_CONFIG_USART3            \
{                                          \
    BAUD_RATE_9600,   /* 9600   bits/s */  \
    DATA_BITS_8,      /* 8 databits */     \
    STOP_BITS_1,      /* 1 stopbit */      \
    PARITY_NONE,      /* No parity  */     \
    BIT_ORDER_LSB,    /* LSB first sent */ \
    NRZ_NORMAL,       /* Normal mode */    \
    RT_SERIAL_RB_BUFSZ, /* Buffer size */  \
    0                                      \
}


rt_device_t  serial3;
#define USART3_NAME "uart3"
struct serial_configure usart3Config = RT_SERIAL_CONFIG_USART3;
rt_sem_t usart3_rec_sem = RT_NULL;
xUsart_Structure Uart3Buf;



rt_err_t Usart3_RX_Callback(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(usart3_rec_sem);
    return RT_EOK;
}



int USART3_Init(void)
{
    static rt_size_t sendNum = 0;

    // 创建动态信号量
    usart3_rec_sem = rt_sem_create("dynamic_sem3", 0, RT_IPC_FLAG_FIFO);
    if (usart3_rec_sem == RT_NULL){
        rt_kprintf("PRINTF:%d. create dynamic semaphore failed.\n",Record.kprintf_cnt++);
        return -1;
    }
    else{
        rt_kprintf("PRINTF:%d. create done. dynamic semaphore value = 0.\n",Record.kprintf_cnt++);
    }


    serial3 = rt_device_find(USART3_NAME);
    if(serial3 != RT_NULL){
        rt_kprintf("PRINTF:%d. Usart3 Device node created succeed! \r\n",Record.kprintf_cnt++);
        usart3Config.baud_rate = BAUD_RATE_9600;
        usart3Config.bufsz = 1024;
    }
    else {
        rt_kprintf("PRINTF:%d. Usart3 Device node created Failed! \r\n",Record.kprintf_cnt++);
        return RT_ERROR;
    }

    rt_device_control(serial3, RT_DEVICE_CTRL_CONFIG, &usart3Config);
    rt_device_open(serial3, RT_DEVICE_OFLAG_RDONLY | RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(serial3, Usart3_RX_Callback);

    /* 初始化循环队列 */
    Uart3Buf.head = 0;
    Uart3Buf.tail = 0;
    Uart3Buf.lock = rt_mutex_create("uart3_buf_lock", RT_IPC_FLAG_FIFO);


    sendNum = rt_device_write(serial3, RT_NULL, "usart3 is opened!\r\n", 19);
    rt_kprintf("PRINTF:%d. The usart3 test send size : %d\r\n",Record.kprintf_cnt++,sendNum);

    return RT_EOK;
}




void uart3_thread_entry(void* parameter)
{
    char recDat = 0;
    rt_size_t sizeValue = 0;
    uint8_t decodeStatus = 0;
    while(1)
    {
        sizeValue = rt_device_read(serial3, RT_NULL, &recDat, 1);
        if(sizeValue == 1){
            rt_sem_take(usart3_rec_sem, RT_WAITING_FOREVER);
            /* 加锁保护队列操作 */
            rt_mutex_take(Uart3Buf.lock, RT_WAITING_FOREVER);
            /* 计算下一个尾指针位置 */
            uint16_t next_tail = (Uart3Buf.tail + 1) % MAX_DATA_LENGTH;
            /* 队列未满 */
            if(next_tail != Uart3Buf.head) {
                Uart3Buf.rxBuffer[Uart3Buf.tail] = recDat;
                Uart3Buf.tail = next_tail;
            }
            else {
                rt_kprintf("UART3 Queue Full! Data Lost: 0x%02X\n", recDat);
            }
            /* 释放互斥锁 */
            rt_mutex_release(Uart3Buf.lock);

            //----------------------------------------------------------------
            /* 触发协议解析 */

            if(decodeStatus == CMD_TRUE) {

            }
        }
        rt_thread_mdelay(10);
    }
}




rt_thread_t uart3_decodeThread_Handle;
int uart3_decodeThread_Init(void)
{
    uart3_decodeThread_Handle = rt_thread_create("uart3_thread_entry", uart3_thread_entry, RT_NULL, 1024, 10, 200);
    if(uart3_decodeThread_Handle != RT_NULL){
        rt_kprintf("PRINTF:%d. uart3 Thread is created!!\r\n",Record.kprintf_cnt++);
        USART3_Init();
        rt_thread_startup(uart3_decodeThread_Handle);
    }
    else {
        rt_kprintf("PRINTF:%d. Thread is not created!!\r\n",Record.kprintf_cnt++);
    }
    return RT_EOK;
}

#endif


