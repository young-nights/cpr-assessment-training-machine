/*
#include <mainboard__sys.h>
 * Copyright (c) 2006-2025, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-27     RT-Thread    first version
 */

#include <rtthread.h>
#include "mainboard_sys.h"


#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>


extern int nRF24L01_Thread_Init(void);


// 上一次的状态
static conn_state_t prev_conn_state = CONN_UNKNOWN;
// 标记是否已第一次 startup
static rt_bool_t nrf_threads_started = RT_FALSE;


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
  MX_UART4_Init();
  MX_USART3_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */

  system_params_init();
  system_events_init();

  // 初始化一下 nRF24L01 的线程
  nRF24L01_Thread_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

      rt_bool_t is_wired = (Wired_Read_In() == 1);

      conn_state_t curr_state = is_wired ? CONN_WIRED : CONN_WIRELESS;

      if (curr_state != prev_conn_state)   // 只在切换时处理
      {
          if(curr_state == CONN_WIRED)
          {
              rt_kprintf("→ 有线连接，挂起 nRF 线程\n");
              // 按优先级从高到低挂起（建议顺序：发送 → 主 → 解码）
              if (nRF24L01_Data_Transmit_Task_Handle)
              {
                  rt_err_t ret = rt_thread_suspend(nRF24L01_Data_Transmit_Task_Handle);
                  if (ret != RT_EOK) rt_kprintf("suspend transmit failed: %d\n", ret);
              }

              if (nRF24L01_Task_Handle)
              {
                  rt_err_t ret = rt_thread_suspend(nRF24L01_Task_Handle);
                  if (ret != RT_EOK) rt_kprintf("suspend main nRF failed: %d\n", ret);
              }

              if (nRF24L01_Decode_Handle)
              {
                  rt_err_t ret = rt_thread_suspend(nRF24L01_Decode_Handle);
                  if (ret != RT_EOK) rt_kprintf("suspend decode failed: %d\n", ret);
              }

              Record.wired_connect_flag = 1;
          }
          // CONN_WIRELESS
          else
          {
              rt_kprintf("→ 无线模式，恢复 nRF 线程\n");

              if(!nrf_threads_started)
              {
                  // 第一次：startup（从 INIT → READY）
                  if (nRF24L01_Task_Handle) rt_thread_startup(nRF24L01_Task_Handle);
                  if (nRF24L01_Decode_Handle) rt_thread_startup(nRF24L01_Decode_Handle);
                  if (nRF24L01_Data_Transmit_Task_Handle) rt_thread_startup(nRF24L01_Data_Transmit_Task_Handle);

                  nrf_threads_started = RT_TRUE;
                  rt_kprintf("nRF threads first startup!\n");
              }


              // 恢复顺序建议相反：主 → 解码 → 发送（让主线程先跑起来初始化/收包）
              if (nRF24L01_Task_Handle)
              {
                  rt_err_t ret = rt_thread_resume(nRF24L01_Task_Handle);
                  if (ret != RT_EOK) rt_kprintf("resume main nRF failed: %d\n", ret);
              }

              if (nRF24L01_Decode_Handle)
              {
                  rt_err_t ret = rt_thread_resume(nRF24L01_Decode_Handle);
                  if (ret != RT_EOK) rt_kprintf("resume decode failed: %d\n", ret);
              }

              if (nRF24L01_Data_Transmit_Task_Handle)
              {
                  rt_err_t ret = rt_thread_resume(nRF24L01_Data_Transmit_Task_Handle);
                  if (ret != RT_EOK) rt_kprintf("resume transmit failed: %d\n", ret);
              }

              Record.wired_connect_flag = 0;
          }
          prev_conn_state = curr_state;
      }

      LED_Blink(LED_Name_Debug, 1, 0, 0);
      rt_thread_mdelay(1000);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

