#include "stm8s_it.h"
#include "app_sys.h"



volatile int32_t depth_count = 0;     // 脉冲计数（4×）
float depth_mm = 0.0f;                // 当前深度（mm）
int8_t direction = 0;                 // 方向：1=向下，-1=向上，0=静止




/**
  * @brief  UART1 TX Interrupt routine
  * @param None
  * @retval
  * None
  */
INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
    static uint8_t last_state = 0;
    uint8_t curr_A = GPIO_ReadInputPin(GPIOD, GPIO_PIN_3);
    uint8_t curr_B = GPIO_ReadInputPin(GPIOD, GPIO_PIN_4);
    uint8_t curr_state = (curr_A << 1) | curr_B;  // 00,01,10,11

    /* 格雷码 4× 倍频状态表 */
static const int8_t quad_table[16] = {
         0,  +1,  -1,  0,   // 00 → 00,01,10,11
        -1,   0,   0, +1,   // 01 → ...
        +1,   0,   0, -1,   // 10 → ...
         0,  -1,  +1,  0    // 11 → ...
    };

    uint8_t trans = (last_state << 2) | curr_state;
    int8_t delta = quad_table[trans];  // 本次变化量

    // 判断方向
    if (delta > 0) {
        direction = 1;   // 向下压
    } else if (delta < 0) {
        direction = -1;  // 向上弹
    } else {
        direction = 0;   // 非法或静止
    }

    depth_count += delta;
    last_state = curr_state;

    /* 正确清除 PORTD 中断标志 */
    EXTI->CR1 |= 0xC0;  // 写1到 bit7 和 bit6

}




extern void TimingDelay_Decrement(void);
#pragma vector = 0x0D
/* 每1ms 进入一次中断 */
__interrupt void TIM1_IRQHandler(void)
{
    static uint32_t msCnt = 0;
    msCnt++;
    /*计数用于取余运算来判断时间事件*/
    if (++msCnt >= 60000)
    {
        msCnt = 0;
    }

    /*1s扫秒时间，取余为整数说明时间到进入执行函数*/
    if ((msCnt % 1000) == 0)	Timing_1s();



    TimingDelay_Decrement();
    /* Cleat Interrupt Pending bit */
    TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
}








#if defined(STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S103) || \
    defined(STM8S003) || defined(STM8S001) || defined(STM8AF62Ax) || defined(STM8AF52Ax) || defined(STM8S903)
/**
  * @brief  UART1 TX Interrupt routine
  * @param None
  * @retval
  * None
  */
INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
{

}

/**
  * @brief  UART1 RX Interrupt routine
  * @param None
  * @retval
  * None
  */


INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
{
    uint8_t dat = 0;
    if (UART1_GetITStatus(UART1_IT_RXNE) != RESET)
    {
        UART1_ClearITPendingBit(UART1_IT_RXNE);
        dat = (uint8_t)UART1_ReceiveData8();
        UART1_Receive(&USART1_QueueBuf, dat);
    }
}

#endif /*STM8S105*/
