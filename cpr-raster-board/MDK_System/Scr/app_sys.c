#include "app_sys.h"










/**
 * @brief  配置 STM8S003F6P6 的系统时钟
 * @param  无
 * @retval 使用内部高速时钟源（HSI） Max：16Mhz
 * @retval 时钟分频数：CLK_PRESCALER_HSIDIV1（1分频）
 */
void System_Clock_Config(void)
{
	/*所有时钟源寄存器配置为缺省值*/
	CLK_DeInit();

	/*使用内部高速时钟源（HSI）*/
    /* Clock divider to HSI/1 - 16Mhz */
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

}


