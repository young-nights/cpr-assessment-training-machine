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




char CC6201_Hall_Sensor_Dout(void)
{
    return HAL_GPIO_ReadPin(MAGNETIC_STAT_GPIO_Port, MAGNETIC_STAT_Pin);
}


void Debug_LED_Ctrl(SWITCH_et sta)
{
    if(sta == ON){
        HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, GPIO_PIN_SET);
    }
    else if(sta == OFF){
        HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, GPIO_PIN_RESET);
    }
}


/**
 * @brief  Initialize pupil state GPIO outputs (PC10/PC11/PC12, push-pull)
 */
void Pupil_GPIO_Init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin   = PUPIL_P1_PIN | PUPIL_P2_PIN | PUPIL_P3_PIN;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &gpio);

    /* Default: arrest/dilated state (all low = 0) */
    Pupil_GPIO_Output(PUPIL_STATE_ARREST);
}

/**
 * @brief  Output pupil state encoding to head board
 * @param  state: 0=arrest/dilated, 1=resuscitated/normal, 2~7=reserved
 *
 * 3-bit encoding on PC10(P1)/PC11(P2)/PC12(P3):
 *   state 0: 0 0 0
 *   state 1: 1 0 0
 *   state N: bit0=PC10, bit1=PC11, bit2=PC12
 */
void Pupil_GPIO_Output(uint8_t state)
{
    /* P1 = bit0 -> PC10 */
    if (state & 0x01)
        HAL_GPIO_WritePin(PUPIL_P1_PORT, PUPIL_P1_PIN, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(PUPIL_P1_PORT, PUPIL_P1_PIN, GPIO_PIN_RESET);

    /* P2 = bit1 -> PC11 */
    if (state & 0x02)
        HAL_GPIO_WritePin(PUPIL_P2_PORT, PUPIL_P2_PIN, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(PUPIL_P2_PORT, PUPIL_P2_PIN, GPIO_PIN_RESET);

    /* P3 = bit2 -> PC12 */
    if (state & 0x04)
        HAL_GPIO_WritePin(PUPIL_P3_PORT, PUPIL_P3_PIN, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(PUPIL_P3_PORT, PUPIL_P3_PIN, GPIO_PIN_RESET);
}







