/**
 * @file    sys_clock.c
 * @brief   System clock configuration (HSE 8MHz -> 72MHz) and SysTick delay
 *
 * Change Logs:
 * Date           Author      Notes
 * 2026-09-27     coder       the first version
 */
#include "head_sys.h"

static volatile uint32_t s_tick_ms = 0;

/**
 * @brief  Configure system clock: HSE 8MHz -> PLL x9 -> 72MHz
 */
void SystemClock_Config(void)
{
    /* Enable HSE */
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY))
        ;

    /* Flash latency: 2 wait states for 72MHz */
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2;

    /* APB1 = HCLK/2 = 36MHz (required: APB1 max 36MHz) */
    /* PLL = HSE * 9 = 72MHz, SW = PLL */
    RCC->CFGR = RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9;

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY))
        ;

    /* Switch SYSCLK to PLL */
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_PLL)
        ;
}

/**
 * @brief  Initialize SysTick for 1ms tick
 */
void SysTick_Init(void)
{
    s_tick_ms = 0;
    SYSTICK->LOAD = (SYSCLK_FREQ_HZ / 1000) - 1;   /* 1ms = 72000-1 ticks */
    SYSTICK->VAL = 0;
    SYSTICK->CTRL = SYSTICK_CTRL_CLKSOURCE | SYSTICK_CTRL_TICKINT | SYSTICK_CTRL_ENABLE;
}

/**
 * @brief  SysTick interrupt handler (called every 1ms)
 */
void SysTick_Handler(void)
{
    s_tick_ms++;
}

/**
 * @brief  Get system tick in milliseconds
 */
uint32_t SysTick_GetTick(void)
{
    return s_tick_ms;
}

/**
 * @brief  Blocking delay in milliseconds
 */
void Delay_ms(uint32_t ms)
{
    uint32_t start = s_tick_ms;
    while ((s_tick_ms - start) < ms)
        ;
}

/**
 * @brief  Blocking delay in microseconds (busy-wait approximation)
 */
void Delay_us(uint32_t us)
{
    uint32_t cycles = us * (SYSCLK_FREQ_HZ / 1000000) / 5;
    while (cycles--)
        ;
}
