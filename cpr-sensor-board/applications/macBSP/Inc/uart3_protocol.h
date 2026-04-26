/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-02-05     18452       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_UART3_PROTOCOL_H_
#define APPLICATIONS_MACBSP_INC_UART3_PROTOCOL_H_

#include "bsp_sys.h"


#if 1

#ifdef BSP_USING_UART3
#define USART3_SEND_CMD_INFO_PRINTF     1       // 串口1发送指令信息打印
#define USART3_REC_CMD_PRINTF           1       // 串口1接收指令信息打印
#endif



extern rt_device_t  serial3;




int uart3_decodeThread_Init(void);
void USART3_Send_Command_to_Principal(uint8_t DataLen, uint8_t CmdType, uint8_t CmdStatus, uint8_t* DataBuf);
void Protocol_Operation_USART3(rt_device_t dev,uint8_t* CmdBuf);
void USART3_Order_to_Andriod(uint8_t order);
int USART3_Init(void);



#endif


#endif /* APPLICATIONS_MACBSP_INC_UART3_PROTOCOL_H_ */
