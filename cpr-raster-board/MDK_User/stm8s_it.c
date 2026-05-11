#include "stm8s_it.h"
#include "app_sys.h"



volatile int32_t depth_count_press = 0;  /* 按压脉冲计数（正=下压，负=回弹） */
volatile int32_t depth_count_blow = 0;   /* 吹气脉冲计数（正=充气，负=泄气） */
volatile int8_t direction_press = 0;     /* 按压方向: -1=回弹, 0=静止, 1=下压 */
volatile int8_t direction_blow = 0;      /* 吹气方向: -1=泄气, 0=静止, 1=充气 */

volatile uint32_t g_system_tick_ms = 0;  /* 全局系统滴答(ms) */




/**
  * @brief  GPIOC 中断处理（吹气光栅编码器: Pin6=A相, Pin7=B相）
  *         使用4倍频正交解码，通过查表法判断方向
  *         direction_blow: -1=泄气, 0=静止, 1=充气
  */
INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5)
{
    static uint8_t last_state_c = 0;
    static const int8_t quad_table[16] = {
         0,  +1,  -1,  0,
        -1,   0,   0, +1,
        +1,   0,   0, -1,
         0,  -1,  +1,  0
    };
    uint8_t curr_A, curr_B, curr_state, trans;
    int8_t delta;

    curr_A = GPIO_ReadInputPin(GPIOC, GPIO_PIN_6);
    curr_B = GPIO_ReadInputPin(GPIOC, GPIO_PIN_7);
    curr_state = (curr_A << 1) | curr_B;
    trans = (last_state_c << 2) | curr_state;
    delta = quad_table[trans];

    /* 更新吹气方向: -1=泄气, 0=静止, 1=充气 */
    if (delta > 0) {
        direction_blow = 1;    /* 充气 */
    } else if (delta < 0) {
        direction_blow = -1;   /* 泄气 */
    } else {
        direction_blow = 0;
    }

    depth_count_blow += delta;

    last_state_c = curr_state;
}


/**
  * @brief  GPIOD 中断处理（按压光栅编码器: Pin3=A相, Pin4=B相）
  *         使用4倍频正交解码，通过查表法判断方向
  *         direction_press: -1=回弹, 0=静止, 1=下压
  */
INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
    static uint8_t last_state_d = 0;
    static const int8_t quad_table[16] = {
         0,  +1,  -1,  0,
        -1,   0,   0, +1,
        +1,   0,   0, -1,
         0,  -1,  +1,  0
    };
    uint8_t curr_A, curr_B, curr_state, trans;
    int8_t delta;

    curr_A = GPIO_ReadInputPin(GPIOD, GPIO_PIN_3);
    curr_B = GPIO_ReadInputPin(GPIOD, GPIO_PIN_4);
    curr_state = (curr_A << 1) | curr_B;
    trans = (last_state_d << 2) | curr_state;
    delta = quad_table[trans];

    /* 更新按压方向: -1=回弹, 0=静止, 1=下压 */
    if (delta > 0) {
        direction_press = 1;   /* 下压 */
    } else if (delta < 0) {
        direction_press = -1;  /* 回弹 */
    } else {
        direction_press = 0;
    }

    depth_count_press += delta;

    last_state_d = curr_state;
}




extern void TimingDelay_Decrement(void);
#pragma vector = 0x0D
/* TIM1中断，每1ms触发一次，用于系统滴答计时 */
__interrupt void TIM1_IRQHandler(void)
{
    static uint32_t msCnt = 0;

    g_system_tick_ms++;

    if (++msCnt >= 60000)
    {
        msCnt = 0;
    }

    /* 每1秒执行定时任务 */
    if ((msCnt % 1000) == 0)	Timing_1s();

    TimingDelay_Decrement();
    /* 清除TIM1更新中断标志 */
    TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
}








#if defined(STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S103) || \
    defined(STM8S003) || defined(STM8S001) || defined(STM8AF62Ax) || defined(STM8AF52Ax) || defined(STM8S903)
/**
  * @brief  UART1 TX 中断处理（未使用）
  */
INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
{

}

/**
  * @brief  UART1 RX 中断处理
  *         接收1字节数据并存入环形队列
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
