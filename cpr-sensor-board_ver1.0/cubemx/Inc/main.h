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
extern SPI_HandleTypeDef hspi3;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART1_UART_Init(void);
void MX_SPI3_Init(void);
void MX_SPI1_Init(void);
void MX_ADC1_Init(void);
void MX_USART3_UART_Init(void);
void MX_USART2_UART_Init(void);
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DEBUG_LED_Pin GPIO_PIN_0
#define DEBUG_LED_GPIO_Port GPIOC
#define SPHYGMUS_CTRL2_Pin GPIO_PIN_2
#define SPHYGMUS_CTRL2_GPIO_Port GPIOC
#define SPHYGMUS_CTRL1_Pin GPIO_PIN_3
#define SPHYGMUS_CTRL1_GPIO_Port GPIOC
#define SPI1_NSS_Pin GPIO_PIN_4
#define SPI1_NSS_GPIO_Port GPIOA
#define SPHYGMUS_KEY2_Pin GPIO_PIN_1
#define SPHYGMUS_KEY2_GPIO_Port GPIOB
#define SPHYGMUS_KEY1_Pin GPIO_PIN_2
#define SPHYGMUS_KEY1_GPIO_Port GPIOB
#define nRF24L01_IRQ_Pin GPIO_PIN_10
#define nRF24L01_IRQ_GPIO_Port GPIOB
#define nRF24L01_CE_Pin GPIO_PIN_11
#define nRF24L01_CE_GPIO_Port GPIOB
#define nRF24_IRQ_Pin GPIO_PIN_2
#define nRF24_IRQ_GPIO_Port GPIOD
#define nRF24_CSN_Pin GPIO_PIN_6
#define nRF24_CSN_GPIO_Port GPIOB
#define nRF24_CE_Pin GPIO_PIN_7
#define nRF24_CE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
