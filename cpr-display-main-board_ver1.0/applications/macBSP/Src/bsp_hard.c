/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-03-10     18452       the first version
 */
#include "bsp_hard.h"


// 读取是否是有线输入
rt_uint8_t Wired_Read_In(void)
{
    return HAL_GPIO_ReadPin(WIRED_CONNECT_CHECK_GPIO_Port, WIRED_CONNECT_CHECK_Pin);
}














