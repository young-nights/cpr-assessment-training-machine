#include "app_bsp.h"




void GPIO_EXTIConfig(void)
{
    // 初始化 GPIOC 端口 的引脚 6 和 7 为 上拉输入 带中断模式
    GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_IN_PU_IT);   // A相
    GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_IN_PU_IT);   // B相

    // 初始化 GPIOD 端口 的引脚 3 和 4 为 上拉输入 带中断模式
    GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_IN_PU_IT);   // A相
    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_IN_PU_IT);   // B相

    // 设置 GPIOC 端口 的所有引脚为 双边沿触发（即上升沿和下降沿都会触发中断）
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOC, EXTI_SENSITIVITY_RISE_FALL);
    // 设置 GPIOD 端口 的所有引脚为 双边沿触发（即上升沿和下降沿都会触发中断）
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_RISE_FALL);

    ITC_SetSoftwarePriority(ITC_IRQ_PORTC, ITC_PRIORITYLEVEL_2); // 设置 GPIOC 中断优先级为 2
    ITC_SetSoftwarePriority(ITC_IRQ_PORTD, ITC_PRIORITYLEVEL_2); // 设置 GPIOD 中断优先级为 2
}
