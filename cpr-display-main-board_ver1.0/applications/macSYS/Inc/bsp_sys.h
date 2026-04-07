/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-08-05     18452       the first version
 */
#ifndef __BSP_SYS_H

#define __BSP_SYS_H



/* RTT实时操作系统的头文件 */
#include <rtthread.h>
#include <drv_common.h>
#include <board.h>
#include <rtdevice.h>
#include <rthw.h>
#include <rtdbg.h>
#include <stdio.h>
#include <drv_spi.h>
#include <rtconfig.h>
#include <string.h>

/* 该头文件包含了所有CubeMX自动生成的初始化引脚 */
#include "main.h"

/* macBSP文件 */
#include "bsp_led.h"
#include "bsp_rs232_drv.h"
#include "bsp_rs232_dev.h"
#include "bsp_rs485_drv.h"
#include "bsp_rs485_dev.h"
#include "bsp_rs485_message.h"
#include "bsp_wt588d.h"
#include "bsp_hard.h"

/* macNRF文件 */
#include <macNRF/Inc/mainboard_nrf24l01_driver.h>
#include <macNRF/Inc/mainboard_nrf24l01_message.h>
#include <macNRF/Inc/mainboard_nrf24l01_spi.h>

/* macSYS文件 */
#include "bsp_typedef.h"
#include "rtt_system_work.h"
#include "rd_dm32_printer.h"


#endif /* __BSP_SYS_H */
