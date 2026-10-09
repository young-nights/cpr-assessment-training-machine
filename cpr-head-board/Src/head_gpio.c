/**
 * @file    head_gpio.c
 * @brief   GPIO initialization and pupil state input reading
 *
 * Pupil state encoding (3-bit from Sensor board PC10/PC11/PC12):
 *   PA2 = P1 (LSB), PA3 = P2, PA4 = P3 (MSB)
 *   state = (PA4<<2)|(PA3<<1)|PA2
 *
 * Change Logs:
 * Date           Author      Notes
 * 2026-09-27     coder       the first version
 */
#include "head_sys.h"

/**
 * @brief  Initialize all GPIO pins
 */
void Head_GPIO_Init(void)
{
    /* Enable GPIOA and GPIOC clocks */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;
    /* Enable AFIO (needed for some GPIO configs) */
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    /* ---- PA2/PA3/PA4: Pupil state input (pull-up) ---- */
    /* CRL controls PA0~PA7, 4 bits per pin */
    /* Mode: 10 = input with pull-up/pull-down, CNF: 10 = input pull-up/down */
    /* Each pin: CNF[1:0]=10, MODE[1:0]=00 -> 0x8 */
    /* PA2 (bits 11:8), PA3 (bits 15:12), PA4 (bits 19:16) */
    GPIOA->CRL &= ~(0xFFFUL << 8);       /* Clear PA2/PA3/PA4 config */
    GPIOA->CRL |=  (0x888UL << 8);       /* Input pull-up/down */
    GPIOA->BSRR = (1UL << 2) | (1UL << 3) | (1UL << 4);  /* Set pull-up (ODR=1) */

    /* ---- PA11: WS2812B TIM1_CH4 PWM output (alternate function push-pull) ---- */
    /* CRH controls PA8~PA15 */
    /* PA11: CNF=10 (AF push-pull), MODE=11 (50MHz) -> 0xB */
    GPIOA->CRH &= ~(0xFUL << 12);        /* Clear PA11 config (bits 15:12) */
    GPIOA->CRH |=  (0xBUL << 12);        /* AF push-pull, 50MHz */

    /* ---- PA9: USART1 TX (alternate function push-pull) ---- */
    GPIOA->CRH &= ~(0xFUL << 4);         /* Clear PA9 config */
    GPIOA->CRH |=  (0xBUL << 4);         /* AF push-pull, 50MHz */

    /* ---- PA10: USART1 RX (input floating) ---- */
    GPIOA->CRH &= ~(0xFUL << 8);         /* Clear PA10 config */
    GPIOA->CRH |=  (0x4UL << 8);         /* Input floating */

    /* ---- PC10: OLED SCL (output push-pull) ---- */
    /* CRH controls PC8~PC15 */
    GPIOC->CRH &= ~(0xFUL << 8);         /* Clear PC10 config */
    GPIOC->CRH |=  (0x3UL << 8);         /* Output push-pull, 50MHz */

    /* ---- PC11: OLED SDA (output push-pull, switched to input when reading) ---- */
    GPIOC->CRH &= ~(0xFUL << 12);        /* Clear PC11 config */
    GPIOC->CRH |=  (0x3UL << 12);        /* Output push-pull, 50MHz */
}

/**
 * @brief  Read pupil state from 3 GPIO inputs
 * @retval 3-bit state value (0~7)
 */
uint8_t Pupil_GetState(void)
{
    uint8_t state = 0;
    if (GPIOA->IDR & (1UL << PUPIL_P1_PIN)) state |= 0x01;
    if (GPIOA->IDR & (1UL << PUPIL_P2_PIN)) state |= 0x02;
    if (GPIOA->IDR & (1UL << PUPIL_P3_PIN)) state |= 0x04;
    return state;
}
