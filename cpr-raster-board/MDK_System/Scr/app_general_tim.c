#include "app_general_tim.h"



__IO uint32_t TimingDelay = 0;




void TIM1_Config(void)
{
    /* TIM1 寄存器初始化默认值 */
    TIM1_DeInit();

    /* 配置时钟分频系数为100，设置为向上计数模式，计数重装载值为160，重复计数0次 */
    TIM1_TimeBaseInit(100, TIM1_COUNTERMODE_UP, 160, 0);

    /* 使能自动重装载 */
    TIM1_ARRPreloadConfig(ENABLE);

    /* 使能 TIM1 中断 */
    TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);

    /* 使能TIM1时钟 */
    TIM1_Cmd(ENABLE);
}





/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in subtle.
  * @retval None
  */
void Delay_ms(__IO uint32_t nTime)
{
    TimingDelay = nTime;

    while (TimingDelay != 0);
}




/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
    {
        TimingDelay--;
    }
}






























