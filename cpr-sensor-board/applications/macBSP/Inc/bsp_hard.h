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

/* Pupil state output to head board (3-bit encoding via PC10/PC11/PC12) */
#define PUPIL_P1_PORT       GPIOC   /* PC10 -> head PA2 */
#define PUPIL_P1_PIN        GPIO_PIN_10
#define PUPIL_P2_PORT       GPIOC   /* PC11 -> head PA3 */
#define PUPIL_P2_PIN        GPIO_PIN_11
#define PUPIL_P3_PORT       GPIOC   /* PC12 -> head PA4 */
#define PUPIL_P3_PIN        GPIO_PIN_12

/* Pupil state values (matches eyes_rgb_level semantics) */
#define PUPIL_STATE_ARREST      0   /* Arrest/dilated */
#define PUPIL_STATE_NORMAL      1   /* Resuscitated/normal */


void coreless_motor_ctrl(MOTOR_NAME_et name,SWITCH_et status);
char coreless_motolr_read_key1(void);
char coreless_motolr_read_key2(void);
char CC6201_Hall_Sensor_Dout(void);

void Pupil_GPIO_Init(void);
void Pupil_GPIO_Output(uint8_t state);



#endif /* APPLICATIONS_MACBSP_INC_BSP_HARD_H_ */
