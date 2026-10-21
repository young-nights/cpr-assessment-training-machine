/**
 * @file    stm32f103x6.h
 * @brief   Minimal register definitions for STM32F103C6T6 (bare-metal)
 *
 * Only defines peripherals used by the raster board:
 *   GPIOA/B, RCC, AFIO, EXTI, USART1, USART2, SysTick, FLASH, NVIC
 *
 * Change Logs:
 * Date           Author      Notes
 * 2026-09-27     coder       the first version (head board)
 * 2026-10-21     coder       extended for raster board (USART2/EXTI/AFIO)
 */
#ifndef __STM32F103X6_H__
#define __STM32F103X6_H__

#include <stdint.h>

/* ====== Base Addresses ====== */
#define PERIPH_BASE         0x40000000UL
#define APB1PERIPH_BASE     PERIPH_BASE
#define APB2PERIPH_BASE     (PERIPH_BASE + 0x00010000UL)
#define AHBPERIPH_BASE      (PERIPH_BASE + 0x00020000UL)

/* APB1 */
#define USART2_BASE         (APB1PERIPH_BASE + 0x4400UL)
#define EXTI_BASE           (APB1PERIPH_BASE + 0x0400UL)

/* APB2 */
#define AFIO_BASE           (APB2PERIPH_BASE + 0x0000UL)
#define GPIOA_BASE          (APB2PERIPH_BASE + 0x0800UL)
#define GPIOB_BASE          (APB2PERIPH_BASE + 0x0C00UL)
#define GPIOC_BASE          (APB2PERIPH_BASE + 0x1000UL)
#define USART1_BASE         (APB2PERIPH_BASE + 0x3800UL)

/* AHB */
#define RCC_BASE            (AHBPERIPH_BASE + 0x1000UL)
#define FLASH_R_BASE        (AHBPERIPH_BASE + 0x2000UL)

/* Cortex-M3 System Control */
#define SYSTICK_BASE        0xE000E010UL
#define NVIC_ISER_BASE      0xE000E100UL

/* ====== GPIO Registers ====== */
typedef struct {
    volatile uint32_t CRL;
    volatile uint32_t CRH;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t BRR;
    volatile uint32_t LCKR;
} GPIO_TypeDef;

#define GPIOA               ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *)GPIOC_BASE)

/* ====== RCC Registers ====== */
typedef struct {
    volatile uint32_t CR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB1RSTR;
    volatile uint32_t AHBENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB1ENR;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
} RCC_TypeDef;

#define RCC                 ((RCC_TypeDef *)RCC_BASE)

/* RCC bits */
#define RCC_CR_HSION        (1UL << 0)
#define RCC_CR_HSIRDY       (1UL << 1)
#define RCC_CR_HSEON        (1UL << 16)
#define RCC_CR_HSERDY       (1UL << 17)
#define RCC_CR_CSSON        (1UL << 19)
#define RCC_CR_PLLON        (1UL << 24)
#define RCC_CR_PLLRDY       (1UL << 25)

#define RCC_CFGR_SW_PLL     (2UL << 0)
#define RCC_CFGR_SWS_PLL    (2UL << 2)
#define RCC_CFGR_SWS_MASK   (3UL << 2)
#define RCC_CFGR_PPRE1_DIV2 (4UL << 8)    /* APB1 = HCLK/2 = 36MHz */
#define RCC_CFGR_PLLSRC_HSE (1UL << 16)
#define RCC_CFGR_PLLMULL9   (7UL << 18)   /* 8MHz * 9 = 72MHz */

#define RCC_APB2ENR_AFIOEN  (1UL << 0)
#define RCC_APB2ENR_IOPAEN  (1UL << 2)
#define RCC_APB2ENR_IOPBEN  (1UL << 3)
#define RCC_APB2ENR_IOPCEN  (1UL << 4)
#define RCC_APB2ENR_USART1EN (1UL << 14)

#define RCC_APB1ENR_USART2EN (1UL << 17)

/* ====== AFIO Registers ====== */
typedef struct {
    volatile uint32_t EVCR;
    volatile uint32_t MAPR;
    volatile uint32_t EXTICR1;
    volatile uint32_t EXTICR2;
    volatile uint32_t EXTICR3;
    volatile uint32_t EXTICR4;
    volatile uint32_t MAPR2;
} AFIO_TypeDef;

#define AFIO                ((AFIO_TypeDef *)AFIO_BASE)

/* ====== EXTI Registers ====== */
typedef struct {
    volatile uint32_t IMR;
    volatile uint32_t EMR;
    volatile uint32_t RTSR;
    volatile uint32_t FTSR;
    volatile uint32_t SWIER;
    volatile uint32_t PR;
} EXTI_TypeDef;

#define EXTI                ((EXTI_TypeDef *)EXTI_BASE)

/* ====== USART Registers ====== */
typedef struct {
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
} USART_TypeDef;

#define USART1              ((USART_TypeDef *)USART1_BASE)
#define USART2              ((USART_TypeDef *)USART2_BASE)

/* USART bits */
#define USART_SR_RXNE       (1UL << 5)
#define USART_SR_TC         (1UL << 6)
#define USART_SR_TXE        (1UL << 7)
#define USART_CR1_UE        (1UL << 13)
#define USART_CR1_TE        (1UL << 3)
#define USART_CR1_RE        (1UL << 2)
#define USART_CR1_RXNEIE    (1UL << 5)

/* ====== SysTick Registers ====== */
typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_TypeDef;

#define SYSTICK             ((SysTick_TypeDef *)SYSTICK_BASE)

#define SYSTICK_CTRL_ENABLE     (1UL << 0)
#define SYSTICK_CTRL_TICKINT    (1UL << 1)
#define SYSTICK_CTRL_CLKSOURCE  (1UL << 2)

/* ====== FLASH Registers ====== */
typedef struct {
    volatile uint32_t ACR;
    volatile uint32_t KEYR;
    volatile uint32_t OPTKEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t AR;
} FLASH_TypeDef;

#define FLASH               ((FLASH_TypeDef *)FLASH_R_BASE)

#define FLASH_ACR_LATENCY_2 (2UL << 0)   /* 2 wait states for 72MHz */
#define FLASH_ACR_PRFTBE    (1UL << 4)   /* Prefetch buffer enable */

/* ====== NVIC ====== */
#define NVIC_ISER(n)        (*(volatile uint32_t *)(NVIC_ISER_BASE + 4UL * ((n) / 32)))

#define IRQn_EXTI9_5        23
#define IRQn_USART2         38

/* ====== Inline Helpers ====== */
static inline void __disable_irq(void)
{
    __asm volatile ("cpsid i" ::: "memory");
}

static inline void __enable_irq(void)
{
    __asm volatile ("cpsie i" ::: "memory");
}

#endif /* __STM32F103X6_H__ */
