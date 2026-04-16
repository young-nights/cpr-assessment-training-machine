#include "stm8s.h"
#include "app_sys.h"



void main(void)
{

	/* Close all interrupts */
	disableInterrupts();

	/* Clock configuration */
	System_Clock_Config();

	/* TIM1 General TIM For Systick Config */
	TIM1_Config();
	/* initialize Uart1's variable */
	UART1_ReceiveValueInit(&USART1_QueueBuf, UART_DATALENGTH);
	/* UART1 Config*/
	UART1_Config();
	/* EXTI Config */
	GPIO_EXTIConfig();
	
	/* Open all interrupts -----------------------------------------------------*/
	enableInterrupts();


	/* Send ID-Deep-ResidueNum */   
	while (1)
	{
		depth_mm = depth_count * MM_PER_PULSE;
		USART1_SendDepthDir(depth_mm, direction); 
		Delay_ms(100);
	}
}












#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	   /* Infinite loop */
	while (1)
	{
	}
}
#endif

