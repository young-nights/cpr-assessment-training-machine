/*
 * Copyright (c) 2006-2026, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-07-04     RT-Thread    first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "bsp_sys.h"

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* NOTE: HAL_Init(), SystemClock_Config() and MX_USART1_UART_Init()
   * are already called by RT-Thread during rt_hw_board_init().
   * Do NOT call them again here — re-initializing USART1 via
   * HAL_UART_Init() overwrites USART1->CR1 and clears RXNEIE,
   * which breaks terminal input. */

  /* Initialize peripherals not set up by RT-Thread */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();

  /* USER CODE BEGIN 2 */

  ws2812b_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

      ws2812b_demo_effects();
      rt_thread_mdelay(50);

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
