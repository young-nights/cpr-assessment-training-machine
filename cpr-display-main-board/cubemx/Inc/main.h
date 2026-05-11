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


extern SPI_HandleTypeDef hspi2;

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_uart4_tx;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_DMA_Init(void);
void MX_USART1_UART_Init(void);
void MX_UART4_Init(void);
void MX_USART3_UART_Init(void);
void MX_SPI2_Init(void);
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TOUCH_IN10_Pin GPIO_PIN_2
#define TOUCH_IN10_GPIO_Port GPIOE
#define TOUCH_IN11_Pin GPIO_PIN_3
#define TOUCH_IN11_GPIO_Port GPIOE
#define TOUCH_IN12_Pin GPIO_PIN_4
#define TOUCH_IN12_GPIO_Port GPIOE
#define TOUCH_IN13_Pin GPIO_PIN_5
#define TOUCH_IN13_GPIO_Port GPIOE
#define TOUCH_IN14_Pin GPIO_PIN_6
#define TOUCH_IN14_GPIO_Port GPIOE
#define LED_REMOVE_FOREIGN_Pin GPIO_PIN_13
#define LED_REMOVE_FOREIGN_GPIO_Port GPIOC
#define LED_EMERGENCY_CALL_Pin GPIO_PIN_14
#define LED_EMERGENCY_CALL_GPIO_Port GPIOC
#define LED_CHECK_BREATH_Pin GPIO_PIN_15
#define LED_CHECK_BREATH_GPIO_Port GPIOC
#define LED_SPHYGMOSCOPY_Pin GPIO_PIN_0
#define LED_SPHYGMOSCOPY_GPIO_Port GPIOF
#define LED_CONSCIOUS_JUDGMENT_Pin GPIO_PIN_1
#define LED_CONSCIOUS_JUDGMENT_GPIO_Port GPIOF
#define TM1629A_B_DIO_Pin GPIO_PIN_4
#define TM1629A_B_DIO_GPIO_Port GPIOA
#define TM1629A_B_CLK_Pin GPIO_PIN_5
#define TM1629A_B_CLK_GPIO_Port GPIOA
#define TM1629A_B_STB_Pin GPIO_PIN_6
#define TM1629A_B_STB_GPIO_Port GPIOA
#define TOUCH_IN7_Pin GPIO_PIN_13
#define TOUCH_IN7_GPIO_Port GPIOF
#define TOUCH_IN9_Pin GPIO_PIN_14
#define TOUCH_IN9_GPIO_Port GPIOF
#define TOUCH_IN6_Pin GPIO_PIN_15
#define TOUCH_IN6_GPIO_Port GPIOF
#define TOUCH_IN4_Pin GPIO_PIN_0
#define TOUCH_IN4_GPIO_Port GPIOG
#define TOUCH_IN2_Pin GPIO_PIN_1
#define TOUCH_IN2_GPIO_Port GPIOG
#define LED_ASSESS_Pin GPIO_PIN_9
#define LED_ASSESS_GPIO_Port GPIOE
#define LED_START_Pin GPIO_PIN_10
#define LED_START_GPIO_Port GPIOE
#define LED_MINUS_Pin GPIO_PIN_12
#define LED_MINUS_GPIO_Port GPIOE
#define TM1629A_A_DIO_Pin GPIO_PIN_13
#define TM1629A_A_DIO_GPIO_Port GPIOE
#define TM1629A_A_CLK_Pin GPIO_PIN_14
#define TM1629A_A_CLK_GPIO_Port GPIOE
#define TM1629A_A_STB_Pin GPIO_PIN_15
#define TM1629A_A_STB_GPIO_Port GPIOE
#define nRF24L01_CSN_Pin GPIO_PIN_12
#define nRF24L01_CSN_GPIO_Port GPIOB
#define nRF24L01_CE_Pin GPIO_PIN_8
#define nRF24L01_CE_GPIO_Port GPIOD
#define nRF24L01_IRQ_Pin GPIO_PIN_9
#define nRF24L01_IRQ_GPIO_Port GPIOD
#define LED_PRINTER_Pin GPIO_PIN_10
#define LED_PRINTER_GPIO_Port GPIOD
#define LED_RESET_Pin GPIO_PIN_12
#define LED_RESET_GPIO_Port GPIOD
#define LED_SETTING_Pin GPIO_PIN_13
#define LED_SETTING_GPIO_Port GPIOD
#define TOUCH_IN1_Pin GPIO_PIN_15
#define TOUCH_IN1_GPIO_Port GPIOD
#define TOUCH_IN3_Pin GPIO_PIN_2
#define TOUCH_IN3_GPIO_Port GPIOG
#define TOUCH_IN5_Pin GPIO_PIN_3
#define TOUCH_IN5_GPIO_Port GPIOG
#define TOUCH_IN8_Pin GPIO_PIN_4
#define TOUCH_IN8_GPIO_Port GPIOG
#define LED_PLUS_Pin GPIO_PIN_5
#define LED_PLUS_GPIO_Port GPIOG
#define LED_COMPETITION_Pin GPIO_PIN_7
#define LED_COMPETITION_GPIO_Port GPIOG
#define LED_TRAIN_Pin GPIO_PIN_8
#define LED_TRAIN_GPIO_Port GPIOG
#define WIRED_CONNECT_CHECK_Pin GPIO_PIN_6
#define WIRED_CONNECT_CHECK_GPIO_Port GPIOC
#define WT588D_DATA_Pin GPIO_PIN_7
#define WT588D_DATA_GPIO_Port GPIOC
#define WT588D_CS_Pin GPIO_PIN_8
#define WT588D_CS_GPIO_Port GPIOC
#define WT588D_CLK_Pin GPIO_PIN_9
#define WT588D_CLK_GPIO_Port GPIOC
#define PRINTER_CTS_Pin GPIO_PIN_12
#define PRINTER_CTS_GPIO_Port GPIOC
#define TM1638_DIO_Pin GPIO_PIN_0
#define TM1638_DIO_GPIO_Port GPIOD
#define TM1638_CLK_Pin GPIO_PIN_1
#define TM1638_CLK_GPIO_Port GPIOD
#define TM1638_STB_Pin GPIO_PIN_2
#define TM1638_STB_GPIO_Port GPIOD
#define WT588D_RESET_Pin GPIO_PIN_3
#define WT588D_RESET_GPIO_Port GPIOD
#define WT588D_BUSY_Pin GPIO_PIN_4
#define WT588D_BUSY_GPIO_Port GPIOD
#define LED_BODY1_Pin GPIO_PIN_7
#define LED_BODY1_GPIO_Port GPIOD
#define LED_BODY2_Pin GPIO_PIN_9
#define LED_BODY2_GPIO_Port GPIOG
#define LED_BODY3_Pin GPIO_PIN_10
#define LED_BODY3_GPIO_Port GPIOG
#define LED_BODY4_Pin GPIO_PIN_11
#define LED_BODY4_GPIO_Port GPIOG
#define LED_BODY5_Pin GPIO_PIN_12
#define LED_BODY5_GPIO_Port GPIOG
#define LED_BODY6_Pin GPIO_PIN_13
#define LED_BODY6_GPIO_Port GPIOG
#define LED_BODY7_Pin GPIO_PIN_14
#define LED_BODY7_GPIO_Port GPIOG
#define LED_DEBUG_Pin GPIO_PIN_1
#define LED_DEBUG_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
