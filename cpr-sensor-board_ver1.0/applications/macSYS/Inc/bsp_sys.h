/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-15     teati       the first version
 */
#ifndef APPLICATIONS_MACSYS_INC_BSP_SYS_H_

#define APPLICATIONS_MACSYS_INC_BSP_SYS_H_

/* RTT实时操作系统的头文件 */
#include <rtthread.h>
#include <drv_common.h>
#include <board.h>
#include <rtdevice.h>
#include <rthw.h>
#include <drv_spi.h>
#include <rtdbg.h>
#include <ulog.h>
#include <stdio.h>
#include <string.h>

/* 该头文件包含了所有CubeMX自动生成的初始化引脚 */
#include "main.h"

/* macAPP 头文件 */
#include "rtt_system_work.h"
/* macBSP 头文件 */
#include "adc1115idgsr.h"
#include "adc128s102cimtx.h"
#include "bsp_hard.h"
#include "uart2_protocol.h"
#include "bsp_rs485_drv.h"
#include "bsp_rs485_dev.h"
#include "bsp_rs485_message.h"
/* macNRF 头文件 */
#include "bsp_nrf24l01_driver.h"
#include "bsp_nrf24l01_spi.h"
#include "bsp_nrf24l01_message.h"
/* macSYS 头文件 */
#include "bsp_typedef.h"






#endif /* APPLICATIONS_MACSYS_INC_BSP_SYS_H_ */
