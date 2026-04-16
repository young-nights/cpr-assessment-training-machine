/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-12-05     18452       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_UART2_PROTOCOL_H_
#define APPLICATIONS_MACBSP_INC_UART2_PROTOCOL_H_

#include "bsp_sys.h"


#ifdef BSP_USING_UART2
#define USART2_SEND_CMD_INFO_PRINTF     1       // 串口1发送指令信息打印
#define USART2_REC_CMD_PRINTF           1       // 串口1接收指令信息打印
#endif



extern rt_device_t  serial2;

/* 创建消息队列结构体参数 */
typedef struct{
    rt_device_t device_t;
    rt_size_t   size;
}MessageQueue;


/* 创建串口接收与发送缓冲区结构体参数 */
#define MAX_DATA_LENGTH 512
typedef struct{
    uint8_t rxBuffer[MAX_DATA_LENGTH];  // 循环队列缓冲区
    volatile rt_uint16_t rx_index;      // 数据索引
    volatile rt_uint16_t head;          // 队列头指针（读位置）
    volatile rt_uint16_t tail;          // 队列尾指针（写位置）
    rt_mutex_t lock;                    // 互斥锁
}xUsart_Structure;



int uart2_decodeThread_Init(void);
void USART2_Send_Command_to_Principal(uint8_t DataLen, uint8_t CmdType, uint8_t CmdStatus, uint8_t* DataBuf);
void Protocol_Operation_USART2(rt_device_t dev,uint8_t* CmdBuf);
void USART2_Order_to_Andriod(uint8_t order);
int USART2_Init(void);






#endif /* APPLICATIONS_MACBSP_INC_UART2_PROTOCOL_H_ */
