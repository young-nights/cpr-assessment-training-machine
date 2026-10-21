/**
 * @file    raster_gpio.c
 * @brief   Photoelectric switch inputs (EXTI rising edge) and direction decode
 *
 * Hardware mapping (v1.1 refactor):
 *   Press pair: OUT1=PA5 (A), OUT2=PA6 (B)
 *   Blow  pair: OUT3=PB8 (A), OUT4=PB9 (B)
 *
 * Direction decode (quadrature-style on slot sensors):
 *   On rising edge of channel A, sample channel B:
 *     B == 0 -> forward (+1, downstroke / inflate)
 *     B == 1 -> reverse (-1, rebound / deflate)
 *
 * Change Logs:
 * Date           Author      Notes
 * 2026-10-21     coder       the first version
 */
#include "raster_sys.h"

/* Pulse accumulators shared between EXTI ISR and main loop */
static volatile int32_t s_press_delta = 0;
static volatile int32_t s_blow_delta  = 0;
static volatile int8_t  s_press_dir   = 0;
static volatile int8_t  s_blow_dir    = 0;

/**
 * @brief  Initialize GPIO + EXTI for the four photoelectric switches
 */
void Raster_GPIO_Init(void)
{
    /* Enable GPIOA, GPIOB, AFIO clocks */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

    /* ---- PA5/PA6: input floating (EXTI) ---- */
    /* CRL: PA5 bits 23:20, PA6 bits 27:24; CNF=01 MODE=00 -> 0x4 */
    GPIOA->CRL &= ~(0xFFUL << 20);
    GPIOA->CRL |=  (0x44UL << 20);

    /* ---- PB8/PB9: input floating (EXTI) ---- */
    /* CRH: PB8 bits 3:0, PB9 bits 7:4 */
    GPIOB->CRH &= ~(0xFFUL << 0);
    GPIOB->CRH |=  (0x44UL << 0);

    /* ---- AFIO EXTI line mapping ---- */
    /* EXTICR2: EXTI5 (bits 7:4)=PA(0000), EXTI6 (bits 11:8)=PA(0000) */
    AFIO->EXTICR2 &= ~((0xFUL << 4) | (0xFUL << 8));
    /* EXTICR3: EXTI8 (bits 3:0)=PB(0001), EXTI9 (bits 7:4)=PB(0001) */
    AFIO->EXTICR3 &= ~((0xFUL << 0) | (0xFUL << 4));
    AFIO->EXTICR3 |=  ((0x1UL << 0) | (0x1UL << 4));

    /* ---- EXTI: rising edge only, unmask 5/6/8/9 ---- */
    EXTI->FTSR &= ~((1UL << 5) | (1UL << 6) | (1UL << 8) | (1UL << 9));
    EXTI->RTSR |=  ((1UL << 5) | (1UL << 6) | (1UL << 8) | (1UL << 9));
    EXTI->IMR  |=  ((1UL << 5) | (1UL << 6) | (1UL << 8) | (1UL << 9));
    EXTI->PR    =   (1UL << 5) | (1UL << 6) | (1UL << 8) | (1UL << 9);

    /* ---- NVIC: enable EXTI9_5 ---- */
    NVIC_ISER(IRQn_EXTI9_5 / 32) = (1UL << (IRQn_EXTI9_5 % 32));
}

/**
 * @brief  EXTI9_5 shared interrupt: OUT1/OUT2 press pair, OUT3/OUT4 blow pair
 */
void EXTI9_5_IRQHandler(void)
{
    /* ---- Press pair (OUT1=PA5 A, OUT2=PA6 B) ---- */
    if (EXTI->PR & (1UL << OUT1_PIN)) {
        EXTI->PR = (1UL << OUT1_PIN);
        int8_t dir = (GPIOA->IDR & (1UL << OUT2_PIN)) ? -1 : 1;
        s_press_dir = dir;
        s_press_delta += dir;
    }
    if (EXTI->PR & (1UL << OUT2_PIN)) {
        EXTI->PR = (1UL << OUT2_PIN);
        /* B-channel edge only updates direction reference, no count */
    }

    /* ---- Blow pair (OUT3=PB8 A, OUT4=PB9 B) ---- */
    if (EXTI->PR & (1UL << OUT3_PIN)) {
        EXTI->PR = (1UL << OUT3_PIN);
        int8_t dir = (GPIOB->IDR & (1UL << OUT4_PIN)) ? -1 : 1;
        s_blow_dir = dir;
        s_blow_delta += dir;
    }
    if (EXTI->PR & (1UL << OUT4_PIN)) {
        EXTI->PR = (1UL << OUT4_PIN);
    }
}

/**
 * @brief  Latest press pair direction (-1 / 0 / +1)
 */
int8_t Raster_GetPressDir(void)
{
    return s_press_dir;
}

/**
 * @brief  Latest blow pair direction (-1 / 0 / +1)
 */
int8_t Raster_GetBlowDir(void)
{
    return s_blow_dir;
}

/**
 * @brief  Atomically take and clear press pulse delta since last call
 */
int16_t Raster_TakePressPulse(void)
{
    int32_t v;
    __disable_irq();
    v = s_press_delta;
    s_press_delta = 0;
    __enable_irq();
    return (int16_t)v;
}

/**
 * @brief  Atomically take and clear blow pulse delta since last call
 */
int16_t Raster_TakeBlowPulse(void)
{
    int32_t v;
    __disable_irq();
    v = s_blow_delta;
    s_blow_delta = 0;
    __enable_irq();
    return (int16_t)v;
}
