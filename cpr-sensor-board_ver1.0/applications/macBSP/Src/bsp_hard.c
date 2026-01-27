/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-12-05     18452       the first version
 */
#include "bsp_hard.h"



void coreless_motor_ctrl(MOTOR_NAME_et name,SWITCH_et status)
{

    if(name == Coreless_motor_1 && status == ON)
    {
        HAL_GPIO_WritePin(SPHYGMUS_CTRL1_GPIO_Port, SPHYGMUS_CTRL1_Pin, GPIO_PIN_SET);
    }
    else if(name == Coreless_motor_1 && status == OFF)
    {
        HAL_GPIO_WritePin(SPHYGMUS_CTRL1_GPIO_Port, SPHYGMUS_CTRL1_Pin, GPIO_PIN_RESET);
    }

    if(name == Coreless_motor_2 && status == ON)
    {
        HAL_GPIO_WritePin(SPHYGMUS_CTRL2_GPIO_Port, SPHYGMUS_CTRL2_Pin, GPIO_PIN_SET);
    }
    else if(name == Coreless_motor_2 && status == OFF)
    {
        HAL_GPIO_WritePin(SPHYGMUS_CTRL2_GPIO_Port, SPHYGMUS_CTRL2_Pin, GPIO_PIN_RESET);
    }
}



char coreless_motolr_read_key1(void)
{
    return HAL_GPIO_ReadPin(SPHYGMUS_KEY1_GPIO_Port, SPHYGMUS_KEY1_Pin);
}


char coreless_motolr_read_key2(void)
{
    return HAL_GPIO_ReadPin(SPHYGMUS_KEY2_GPIO_Port, SPHYGMUS_KEY2_Pin);
}







