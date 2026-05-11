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
#include "bsp_sys.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern ADC_HandleTypeDef hadc1;

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi3;

extern UART_HandleTypeDef huart1;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART1_UART_Init(void);
void MX_ADC1_Init(void);
void MX_SPI3_Init(void);
void MX_SPI1_Init(void);
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_GREEN_Pin GPIO_PIN_0
#define LED_GREEN_GPIO_Port GPIOC
#define BAT_STDBY_Pin GPIO_PIN_2
#define BAT_STDBY_GPIO_Port GPIOC
#define BAT_EN_Pin GPIO_PIN_3
#define BAT_EN_GPIO_Port GPIOC
#define BAT_PROG_Pin GPIO_PIN_0
#define BAT_PROG_GPIO_Port GPIOA
#define BAT_VOL_Pin GPIO_PIN_1
#define BAT_VOL_GPIO_Port GPIOA
#define BAT_CHARG_Pin GPIO_PIN_3
#define BAT_CHARG_GPIO_Port GPIOA
#define TOUCH_RST_Pin GPIO_PIN_4
#define TOUCH_RST_GPIO_Port GPIOA
#define TOUCH_INT_Pin GPIO_PIN_6
#define TOUCH_INT_GPIO_Port GPIOA
#define LCD_DC_Pin GPIO_PIN_4
#define LCD_DC_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_5
#define LCD_RST_GPIO_Port GPIOC
#define LCD_CS_Pin GPIO_PIN_0
#define LCD_CS_GPIO_Port GPIOB
#define nRF24_IRQ_Pin GPIO_PIN_10
#define nRF24_IRQ_GPIO_Port GPIOB
#define nRF24_IRQ_EXTI_IRQn EXTI15_10_IRQn
#define nRF24_CE_Pin GPIO_PIN_11
#define nRF24_CE_GPIO_Port GPIOB
#define Matrixkey_Row1_Pin GPIO_PIN_12
#define Matrixkey_Row1_GPIO_Port GPIOB
#define Matrixkey_Row2_Pin GPIO_PIN_13
#define Matrixkey_Row2_GPIO_Port GPIOB
#define Matrixkey_Row3_Pin GPIO_PIN_14
#define Matrixkey_Row3_GPIO_Port GPIOB
#define LCD_BLK_Pin GPIO_PIN_15
#define LCD_BLK_GPIO_Port GPIOB
#define Matrixkey_Column1_Pin GPIO_PIN_6
#define Matrixkey_Column1_GPIO_Port GPIOC
#define Matrixkey_Column2_Pin GPIO_PIN_7
#define Matrixkey_Column2_GPIO_Port GPIOC
#define Matrixkey_Column3_Pin GPIO_PIN_8
#define Matrixkey_Column3_GPIO_Port GPIOC
#define nRF24_CSN_Pin GPIO_PIN_15
#define nRF24_CSN_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
