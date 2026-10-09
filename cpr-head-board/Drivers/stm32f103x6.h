/**
 * @file    stm32f103x6.h
 * @brief   Minimal register definitions for STM32F103C6T6 (bare-metal)
 *
 * Only defines peripherals used by the head board:
 *   GPIOA/B/C, RCC, TIM1, DMA1, USART1, SysTick, FLASH
 *
 * Change Logs:
 * Date           Author      Notes
 * 2026-09-27     coder       the first version
 */
#ifndef __STM32F103X6_H__
#define __STM32F103X6_H__

#include <stdint.h>

/* ====== Base Addresses ====== */
#define PERIPH_BASE         0x40000000UL
#define APB1PERIPH_BASE     PERIPH_BASE
#define APB2PERIPH_BASE     (PERIPH_BASE + 0x00010000UL)
#define AHBPERIPH_BASE      (PERIPH_BASE + 0x00020000UL)

/* APB2 */
#define GPIOA_BASE          (APB2PERIPH_BASE + 0x0800UL)
#define GPIOB_BASE          (APB2PERIPH_BASE + 0x0C00UL)
#define GPIOC_BASE          (APB2PERIPH_BASE + 0x1000UL)
#define USART1_BASE         (APB2PERIPH_BASE + 0x3800UL)
#define TIM1_BASE           (APB2PERIPH_BASE + 0x2C00UL)

/* AHB */
#define DMA1_BASE           (AHBPERIPH_BASE + 0x0000UL)
#define RCC_BASE            (AHBPERIPH_BASE + 0x1000UL)
#define FLASH_R_BASE        (AHBPERIPH_BASE + 0x2000UL)

/* Cortex-M3 System Control */
#define SYSTICK_BASE        0xE000E010UL
#define SCB_BASE            0xE000ED00UL

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
#define RCC_APB2ENR_TIM1EN  (1UL << 11)

#define RCC_AHBENR_DMA1EN   (1UL << 0)

/* ====== USART1 Registers ====== */
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

/* USART bits */
#define USART_SR_RXNE       (1UL << 5)
#define USART_SR_TC         (1UL << 6)
#define USART_SR_TXE        (1UL << 7)
#define USART_CR1_UE        (1UL << 13)
#define USART_CR1_TE        (1UL << 3)
#define USART_CR1_RE        (1UL << 2)

/* ====== TIM1 Registers ====== */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RCR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t BDTR;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
} TIM_TypeDef;

#define TIM1                ((TIM_TypeDef *)TIM1_BASE)

/* TIM1 bits */
#define TIM_CR1_CEN         (1UL << 0)
#define TIM_CR1_ARPE        (1UL << 7)
#define TIM_DIER_CC4DE      (1UL << 12)  /* DMA request on CC4 */
#define TIM_DIER_UDE        (1UL << 8)   /* DMA request on update */
#define TIM_CCMR2_OC4PE     (1UL << 12)  /* Output compare 4 preload enable */
#define TIM_CCMR2_OC4M_PWM1 (6UL << 12)  /* PWM mode 1 */
#define TIM_CCER_CC4E       (1UL << 12)  /* Capture/compare 4 output enable */
#define TIM_BDTR_MOE        (1UL << 15)  /* Main output enable */
#define TIM_BDTR_AOE        (1UL << 14)  /* Automatic output enable */
#define TIM_SR_CC4IF        (1UL << 4)
#define TIM_SR_UIF          (1UL << 0)
#define TIM_EGR_UG          (1UL << 0)

/* ====== DMA1 Registers ====== */
typedef struct {
    volatile uint32_t CCR;
    volatile uint32_t CNDTR;
    volatile uint32_t CPAR;
    volatile uint32_t CMAR;
} DMA_Channel_TypeDef;

typedef struct {
    volatile uint32_t ISR;
    volatile uint32_t IFCR;
} DMA_TypeDef;

#define DMA1                ((DMA_TypeDef *)DMA1_BASE)
#define DMA1_Channel4       ((DMA_Channel_TypeDef *)(DMA1_BASE + 0x44UL))

/* DMA bits */
#define DMA_CCR_EN          (1UL << 0)
#define DMA_CCR_TCIE        (1UL << 1)
#define DMA_CCR_HTIE        (1UL << 2)
#define DMA_CCR_TEIE        (1UL << 3)
#define DMA_CCR_DIR         (1UL << 4)   /* 1 = read from memory */
#define DMA_CCR_CIRC        (1UL << 5)   /* Circular mode */
#define DMA_CCR_PINC        (1UL << 6)
#define DMA_CCR_MINC        (1UL << 7)   /* Memory increment */
#define DMA_CCR_PSIZE_0     (1UL << 8)   /* Peripheral size 16-bit */
#define DMA_CCR_MSIZE_0     (1UL << 10)  /* Memory size 16-bit */
#define DMA_CCR_PL_HIGH     (2UL << 12)  /* Priority high */
#define DMA_ISR_TCIF4       (1UL << 13)  /* Transfer complete flag ch4 */
#define DMA_ISR_HTIF4       (1UL << 12)  /* Half transfer flag ch4 */
#define DMA_ISR_TEIF4       (1UL << 11)  /* Transfer error flag ch4 */

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

/* ====== NVIC (Cortex-M3) ====== */
#define NVIC_ISER_BASE      0xE000E100UL
#define NVIC_ICER_BASE      0xE000E180UL

#define IRQn_DMA1_Channel4  14
#define IRQn_TIM1_UP        25
#define IRQn_USART1         37

/* ====== System Control Block ====== */
typedef struct {
    volatile uint32_t CPUID;
    volatile uint32_t ICSR;
    volatile uint32_t VTOR;
    volatile uint32_t AIRCR;
    volatile uint32_t SCR;
    volatile uint32_t CCR;
    volatile uint32_t SHPR[3];
    volatile uint32_t SHCSR;
    volatile uint32_t CFSR;
    volatile uint32_t HFSR;
    volatile uint32_t DFSR;
    volatile uint32_t MMFAR;
    volatile uint32_t BFAR;
    volatile uint32_t AFSR;
} SCB_TypeDef;

#define SCB                 ((SCB_TypeDef *)SCB_BASE)

#endif /* __STM32F103X6_H__ */
