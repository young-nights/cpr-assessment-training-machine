/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-12-05     18452       the first version
 */

#include "uart2_protocol.h"





#define RT_SERIAL_CONFIG_USART2            \
{                                          \
    BAUD_RATE_115200, /* 115200 bits/s */  \
    DATA_BITS_8,      /* 8 databits */     \
    STOP_BITS_1,      /* 1 stopbit */      \
    PARITY_NONE,      /* No parity  */     \
    BIT_ORDER_LSB,    /* LSB first sent */ \
    NRZ_NORMAL,       /* Normal mode */    \
    RT_SERIAL_RB_BUFSZ, /* Buffer size */  \
    0                                      \
}


rt_device_t  serial2;
#define USART2_NAME "uart2"
struct serial_configure usart2Config = RT_SERIAL_CONFIG_USART2;
rt_sem_t usart2_rec_sem = RT_NULL;
xUsart_Structure Uart2Buf;




rt_err_t Usart2_RX_Callback(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(usart2_rec_sem);
    return RT_EOK;
}



int USART2_Init(void)
{
    static rt_size_t sendNum = 0;

    // 创建动态信号量
    usart2_rec_sem = rt_sem_create("dynamic_sem2", 0, RT_IPC_FLAG_FIFO);
    if (usart2_rec_sem == RT_NULL){
        rt_kprintf("PRINTF:%d. create dynamic semaphore failed.\n",Record.kprintf_cnt++);
        return -1;
    }
    else{
        rt_kprintf("PRINTF:%d. create done. dynamic semaphore value = 0.\n",Record.kprintf_cnt++);
    }


    serial2 = rt_device_find(USART2_NAME);
    if(serial2 != RT_NULL){
        rt_kprintf("PRINTF:%d. Usart2 Device node created succeed! \r\n",Record.kprintf_cnt++);
        usart2Config.baud_rate = BAUD_RATE_115200;
        usart2Config.bufsz = 1024;
    }
    else {
        rt_kprintf("PRINTF:%d. Usart2 Device node created Failed! \r\n",Record.kprintf_cnt++);
        return RT_ERROR;
    }

    rt_device_control(serial2, RT_DEVICE_CTRL_CONFIG, &usart2Config);
    rt_device_open(serial2, RT_DEVICE_OFLAG_RDONLY | RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(serial2, Usart2_RX_Callback);

    /* 初始化循环队列 */
    Uart2Buf.head = 0;
    Uart2Buf.tail = 0;
    Uart2Buf.lock = rt_mutex_create("uart2_buf_lock", RT_IPC_FLAG_FIFO);


    sendNum = rt_device_write(serial2, RT_NULL, "usart2 is opened!\r\n", 19);
    rt_kprintf("PRINTF:%d. The usart2 test send size : %d\r\n",Record.kprintf_cnt++,sendNum);

    return RT_EOK;
}




void uart2_thread_entry(void* parameter)
{
    char recDat = 0;
    rt_size_t sizeValue = 0;
    uint8_t decodeStatus = 0;
    while(1)
    {
        sizeValue = rt_device_read(serial2, RT_NULL, &recDat, 1);
        if(sizeValue == 1){
            rt_sem_take(usart2_rec_sem, RT_WAITING_FOREVER);
            /* 加锁保护队列操作 */
            rt_mutex_take(Uart2Buf.lock, RT_WAITING_FOREVER);
            /* 计算下一个尾指针位置 */
            uint16_t next_tail = (Uart2Buf.tail + 1) % MAX_DATA_LENGTH;
            /* 队列未满 */
            if(next_tail != Uart2Buf.head) {
                Uart2Buf.rxBuffer[Uart2Buf.tail] = recDat;
                Uart2Buf.tail = next_tail;
            }
            else {
                rt_kprintf("UART2 Queue Full! Data Lost: 0x%02X\n", recDat);
            }
            /* 释放互斥锁 */
            rt_mutex_release(Uart2Buf.lock);

            //----------------------------------------------------------------
            /* 触发协议解析 */

            if(decodeStatus == CMD_TRUE) {

            }
        }
        rt_thread_mdelay(10);
    }
}




rt_thread_t uart2_decodeThread_Handle;
int uart2_decodeThread_Init(void)
{
    uart2_decodeThread_Handle = rt_thread_create("uart2_thread_entry", uart2_thread_entry, RT_NULL, 1024, 10, 200);
    if(uart2_decodeThread_Handle != RT_NULL){
        rt_kprintf("PRINTF:%d. uart2 Thread is created!!\r\n",Record.kprintf_cnt++);
        USART2_Init();
        rt_thread_startup(uart2_decodeThread_Handle);
    }
    else {
        rt_kprintf("PRINTF:%d. Thread is not created!!\r\n",Record.kprintf_cnt++);
    }
    return RT_EOK;
}


















