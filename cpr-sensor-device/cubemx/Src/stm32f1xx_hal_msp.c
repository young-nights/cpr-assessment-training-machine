/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32f1xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */
extern DMA_HandleTypeDef hdma_tim1_ch4_trig_com;

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */



/* USER CODE BEGIN 1 */

/**
  * @brief  TIM_PWM MSP Initialization
n  *         Configures DMA channel for TIM1 CH4 (WS2812B)
  * @param  htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        /* DMA1 clock enable */
        __HAL_RCC_DMA1_CLK_ENABLE();

        /* TIM1 CH4 TRIG COM uses DMA1 Channel 4 */
        hdma_tim1_ch4_trig_com.Instance                 = DMA1_Channel4;
        hdma_tim1_ch4_trig_com.Init.Direction            = DMA_MEMORY_TO_PERIPH;
        hdma_tim1_ch4_trig_com.Init.PeriphInc            = DMA_PINC_DISABLE;
        hdma_tim1_ch4_trig_com.Init.MemInc               = DMA_MINC_ENABLE;
        hdma_tim1_ch4_trig_com.Init.PeriphDataAlignment  = DMA_PDATAALIGN_HALFWORD;
        hdma_tim1_ch4_trig_com.Init.MemDataAlignment     = DMA_MDATAALIGN_HALFWORD;
        hdma_tim1_ch4_trig_com.Init.Mode                 = DMA_NORMAL;
        hdma_tim1_ch4_trig_com.Init.Priority             = DMA_PRIORITY_HIGH;
        if (HAL_DMA_Init(&hdma_tim1_ch4_trig_com) != HAL_OK)
        {
            Error_Handler();
        }

        /* Link DMA to TIM handle */
        __HAL_LINKDMA(htim, hdma[TIM_DMA_ID_CC4], hdma_tim1_ch4_trig_com);

        /* DMA1_Channel4 interrupt: same priority as SysTick (0,0) or slightly higher */
        HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    }
}

/* USER CODE END 1 */
