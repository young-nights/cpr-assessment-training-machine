/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern ADC_HandleTypeDef hadc1;

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;

extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_tim1_ch1;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_DMA_Init(void);
void MX_USART1_UART_Init(void);
void MX_SPI3_Init(void);
void MX_SPI1_Init(void);
void MX_ADC1_Init(void);
void MX_USART3_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_TIM1_Init(void);
void MX_SPI2_Init(void);
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPHYGMUS_KEY2_Pin GPIO_PIN_13
#define SPHYGMUS_KEY2_GPIO_Port GPIOC
#define SPHYGMUS_KEY1_Pin GPIO_PIN_14
#define SPHYGMUS_KEY1_GPIO_Port GPIOC
#define MAGNETIC_STAT_Pin GPIO_PIN_1
#define MAGNETIC_STAT_GPIO_Port GPIOC
#define SPHYGMUS_CTRL2_Pin GPIO_PIN_2
#define SPHYGMUS_CTRL2_GPIO_Port GPIOC
#define SPHYGMUS_CTRL1_Pin GPIO_PIN_3
#define SPHYGMUS_CTRL1_GPIO_Port GPIOC
#define SPI1_NSS_Pin GPIO_PIN_4
#define SPI1_NSS_GPIO_Port GPIOA
#define SPI2_NSS_Pin GPIO_PIN_14
#define SPI2_NSS_GPIO_Port GPIOB
#define DEBUG_LED_Pin GPIO_PIN_15
#define DEBUG_LED_GPIO_Port GPIOA
#define nRF24_IRQ_Pin GPIO_PIN_2
#define nRF24_IRQ_GPIO_Port GPIOD
#define nRF24_CSN_Pin GPIO_PIN_6
#define nRF24_CSN_GPIO_Port GPIOB
#define nRF24_CE_Pin GPIO_PIN_7
#define nRF24_CE_GPIO_Port GPIOB
#define SHAKE_DOUT1_Pin GPIO_PIN_8
#define SHAKE_DOUT1_GPIO_Port GPIOB
#define SHAKE_DOUT0_Pin GPIO_PIN_9
#define SHAKE_DOUT0_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
