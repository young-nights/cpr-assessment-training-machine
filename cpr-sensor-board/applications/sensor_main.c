/*
 * Copyright (c) 2006-2025, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-27     RT-Thread    first version
 */

#include <rtthread.h>
#include "bsp_sys.h"




extern int WS2812B_Thread_Init(void);
extern int nRF24L01_Thread_Init(void);
extern int Hard_Thread_Init(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_SPI3_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  MX_USART3_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */


  uart3_decodeThread_Init();
  uart2_decodeThread_Init();

  adc128s102_spi_init();
  adc128s102_thread_init();

  BSP_MPU6050_Init();
  rt_thread_mdelay(20);
  euler_angles_Thread_Init();
  bsp_mpu6xxx_calibrate_Thread_Init();
  mpu6xxxParameter.if_start_gyro_cali_process = 1;

  ws2812b_init();
  oled_eye_init();
  Hard_Thread_Init();

  nRF24L01_Thread_Init();

  rt_kprintf("PRINTF:%d. All peripherals/threads initialized OK\n",Record.kprintf_cnt++);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

      rt_thread_mdelay(500);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
