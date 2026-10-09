/**
 * @file    head_ws2812b.c
 * @brief   WS2812B single LED driver via TIM1_CH4 PWM + DMA1_CH4
 *
 * Hardware: PA11 (TIM1_CH4) -> WS2812B DIN
 * Timing (72MHz, period=90 ticks = 1.25us):
 *   Logic 0: high 28 ticks (0.389us), low 62 ticks
 *   Logic 1: high 56 ticks (0.778us), low 34 ticks
 *   Reset: >=50us low (handled by frame padding)
 *
 * GRB color order for WS2812B.
 *
 * Change Logs:
 * Date           Author      Notes
 * 2026-09-27     coder       the first version
 */
#include "head_sys.h"

/* One WS2812B = 24 bits G + 24 bits R + 24 bits B = 24 bits total (GRB) */
/* PWM duty buffer: 24 data bits + 48 reset bits (pre) = 72 entries (uint16_t) */
#define DATA_BITS       24
#define RESET_BITS      48              /* >=50us reset at 1.25us/bit = 60 bits */
#define PWM_BUF_SIZE    (DATA_BITS + RESET_BITS)

static uint16_t s_pwm_buf[PWM_BUF_SIZE];
static uint8_t  s_r, s_g, s_b;
static uint8_t  s_brightness = 3;       /* Default: 3% (dying state) */

/**
 * @brief  Encode one byte into 8 PWM duty values (MSB first)
 */
static void encode_byte(uint8_t byte, uint16_t *buf)
{
    for (int i = 7; i >= 0; i--) {
        *buf++ = (byte & (1 << i)) ? WS2812B_T1H : WS2812B_T0H;
    }
}

/**
 * @brief  Fill PWM buffer with GRB data + reset padding
 */
static void fill_buffer(void)
{
    /* Reset: 48 bits of zero duty */
    for (int i = 0; i < RESET_BITS; i++) {
        s_pwm_buf[i] = 0;
    }
    /* Data: G, R, B (24 bits total, starting at index RESET_BITS) */
    uint16_t *p = &s_pwm_buf[RESET_BITS];
    encode_byte(s_g, p);  p += 8;
    encode_byte(s_r, p);  p += 8;
    encode_byte(s_b, p);
}

/**
 * @brief  Apply brightness percentage (0~100) to color values
 */
static void apply_brightness(void)
{
    /* Simple gamma-less scaling */
    uint16_t r = (uint16_t)s_r * s_brightness / 100;
    uint16_t g = (uint16_t)s_g * s_brightness / 100;
    uint16_t b = (uint16_t)s_b * s_brightness / 100;
    /* Temporarily scale for buffer fill */
    uint8_t orig_r = s_r, orig_g = s_g, orig_b = s_b;
    s_r = (uint8_t)r;
    s_g = (uint8_t)g;
    s_b = (uint8_t)b;
    fill_buffer();
    s_r = orig_r;
    s_g = orig_g;
    s_b = orig_b;
}

/**
 * @brief  Initialize TIM1_CH4 PWM + DMA1_CH4 for WS2812B
 */
void WS2812B_Init(void)
{
    /* Enable TIM1 and DMA1 clocks */
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    RCC->AHBENR  |= RCC_AHBENR_DMA1EN;

    /* TIM1 configuration: PWM mode 1 on CH4, period = 90 ticks (1.25us) */
    TIM1->PSC  = 0;                         /* No prescaler: 72MHz timer clock */
    TIM1->ARR  = WS2812B_PERIOD - 1;        /* 89 -> period = 90 ticks */
    TIM1->CCR4 = 0;                         /* Start with 0% duty (reset) */

    /* CCMR2: OC4 preload enable + PWM mode 1 */
    TIM1->CCMR2 = TIM_CCMR2_OC4PE | TIM_CCMR2_OC4M_PWM1;

    /* CCER: Enable CH4 output */
    TIM1->CCER = TIM_CCER_CC4E;

    /* BDTR: Main output enable */
    TIM1->BDTR = TIM_BDTR_MOE | TIM_BDTR_AOE;

    /* DIER: DMA request on CC4 */
    TIM1->DIER = TIM_DIER_CC4DE;

    /* DMA1 Channel 4 configuration (TIM1_CH4) */
    DMA1_Channel4->CCR   = 0;
    DMA1_Channel4->CPAR  = (uint32_t)&TIM1->CCR4;
    DMA1_Channel4->CMAR  = (uint32_t)s_pwm_buf;
    DMA1_Channel4->CNDTR = PWM_BUF_SIZE;

    /* DMA config: memory->peripheral, 16-bit, increment memory, circular, high priority */
    DMA1_Channel4->CCR = DMA_CCR_DIR | DMA_CCR_MINC | DMA_CCR_CIRC |
                         DMA_CCR_PSIZE_0 | DMA_CCR_MSIZE_0 |
                         DMA_CCR_PL_HIGH | DMA_CCR_TCIE;

    /* Enable DMA channel */
    DMA1_Channel4->CCR |= DMA_CCR_EN;

    /* Generate update to load registers */
    TIM1->EGR = TIM_EGR_UG;

    /* Clear flags */
    DMA1->IFCR = DMA_ISR_TCIF4 | DMA_ISR_HTIF4 | DMA_ISR_TEIF4;

    /* Initialize to dying state */
    WS2812B_SetEyeState(EYE_DYING);
}

/**
 * @brief  Set white color with brightness percentage
 * @param  brightness_pct: 0~100
 */
void WS2812B_SetWhite(uint8_t brightness_pct)
{
    if (brightness_pct > 100) brightness_pct = 100;
    s_brightness = brightness_pct;
    s_r = 255;
    s_g = 255;
    s_b = 255;
    apply_brightness();
}

/**
 * @brief  Start PWM+DMA transfer (non-blocking, circular mode runs continuously)
 */
void WS2812B_Show(void)
{
    /* DMA is in circular mode, so data is continuously refreshed */
    /* Just update the buffer contents */
    apply_brightness();
}

/**
 * @brief  Set eye state: dying (low brightness) or normal (full brightness)
 */
void WS2812B_SetEyeState(eye_state_et state)
{
    switch (state) {
        case EYE_DYING:
            WS2812B_SetWhite(3);     /* ~3% brightness */
            break;
        case EYE_NORMAL:
            WS2812B_SetWhite(100);   /* Full brightness */
            break;
        default:
            WS2812B_SetWhite(0);
            break;
    }
}

/**
 * @brief  DMA1 Channel 4 transfer complete ISR (for debug/monitoring)
 */
void DMA1_Channel4_IRQHandler(void)
{
    if (DMA1->ISR & DMA_ISR_TCIF4) {
        DMA1->IFCR = DMA_ISR_TCIF4;
    }
}
