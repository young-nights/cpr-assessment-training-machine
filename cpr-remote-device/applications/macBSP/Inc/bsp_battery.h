/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-07-28     18452       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_BSP_BATTERY_H_
#define APPLICATIONS_MACBSP_INC_BSP_BATTERY_H_

#include "bsp_sys.h"


#define REF_VOL_MV          3300    /* 参考电压 3.3V -> 3300 mV */
#define CONVERT_BITS        12      /* 分辨率 */
#define ADC_MAX             (1UL << CONVERT_BITS) - 1  /* 4095 */
#define SAMPLE_NUM          8       /* 采样次数 */
#define RPROG_KOHM          2       /* RPROG = 2kΩ */
#define CURRENT_FACTOR      1100    /* 电流因子 */
#define VREF_FACTOR         3       /* 电压常数因子 = R104 / R106 = 30k / 10k = 3 */

void battery_recharge_enable(void);
void battery_recharge_disable(void);
rt_uint8_t battery_check_if_recharging(void);
rt_uint8_t battery_check_if_power_full(void);
int battery_get_current_ma(rt_uint32_t adc_avg);  /* 返回 mA */
int battery_get_vol_mv(rt_uint32_t adc_avg);      /* 返回 mV */

#endif /* APPLICATIONS_MACBSP_INC_BSP_BATTERY_H_ */
