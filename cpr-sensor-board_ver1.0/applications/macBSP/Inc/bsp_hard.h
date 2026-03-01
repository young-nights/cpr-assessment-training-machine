/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-12-05     18452       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_BSP_HARD_H_
#define APPLICATIONS_MACBSP_INC_BSP_HARD_H_

#include "bsp_sys.h"



typedef enum
{
    Coreless_motor_1 = 1,
    Coreless_motor_2,
}MOTOR_NAME_et;



char coreless_motolr_read_key1(void);
char coreless_motolr_read_key2(void);
char CC6201_Hall_Sensor_Dout(void);



#endif /* APPLICATIONS_MACBSP_INC_BSP_HARD_H_ */
