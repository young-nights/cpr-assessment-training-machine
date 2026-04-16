/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-11-04     Administrator       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_BSP_RS485_MESSAGE_H_
#define APPLICATIONS_MACBSP_INC_BSP_RS485_MESSAGE_H_

#include "bsp_sys.h"


#define RS485_SLAVE_ADDR        0x01
#define MODBUS_READ_HOLDING     0x03   // 读保持寄存器（最常用读 AO）
#define MODBUS_WRITE_SINGLE     0x06   // 写单个寄存器
#define MODBUS_WRITE_MULTIPLE   0x10   // 写多个寄存器（批量写 AO）


// 保持寄存器地址定义（从 0 开始）
#define REG_PRESSURE          1     // 压力值，uint16_t


int rs485_decode_thread_init(void);
void update_sensors(void);

extern rs485_inst_t *rs485_hinst;


#endif /* APPLICATIONS_MACBSP_INC_BSP_RS485_MESSAGE_H_ */
