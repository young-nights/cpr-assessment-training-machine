/**
 * @file    head_sys.h
 * @brief   Head board system configuration, pin definitions, and global types
 *
 * Board:  STM32F103C6T6 (LQFP-48), bare-metal, HSE 8MHz -> 72MHz
 * Role:   Pupil state display (WS2812B RGB + OLED), driven by Sensor GPIO
 *
 * Change Logs:
 * Date           Author      Notes
 * 2026-09-27     coder       the first version
 */
#ifndef __HEAD_SYS_H__
#define __HEAD_SYS_H__

#include <stdint.h>

/* ====== MCU Register Definitions (STM32F103C6T6) ====== */
#include "stm32f103x6.h"

/* ====== System Clock ====== */
#define SYSCLK_FREQ_HZ      72000000UL
#define APB1_FREQ_HZ        36000000UL
#define APB2_FREQ_HZ        72000000UL

/* ====== Pin Definitions ====== */

/* Pupil state GPIO input (from Sensor board PC10/PC11/PC12) */
#define PUPIL_P1_PORT       GPIOA   /* PA2 */
#define PUPIL_P1_PIN        2
#define PUPIL_P2_PORT       GPIOA   /* PA3 */
#define PUPIL_P2_PIN        3
#define PUPIL_P3_PORT       GPIOA   /* PA4 */
#define PUPIL_P3_PIN        4

/* WS2812B RGB eye LED (TIM1_CH4 PWM + DMA) */
#define RGB_LED_PORT        GPIOA   /* PA11 */
#define RGB_LED_PIN         11

/* OLED eye display (soft I2C) */
#define OLED_SCL_PORT       GPIOC   /* PC10 */
#define OLED_SCL_PIN        10
#define OLED_SDA_PORT       GPIOC   /* PC11 */
#define OLED_SDA_PIN        11

/* Debug UART */
#define DBG_UART_PORT       GPIOA   /* PA9=TX, PA10=RX */
#define DBG_UART_TX_PIN     9
#define DBG_UART_RX_PIN     10

/* ====== Pupil State Encoding ====== */
#define PUPIL_STATE_ARREST      0   /* Pupils dilated, cardiac arrest */
#define PUPIL_STATE_NORMAL      1   /* Pupils normal, resuscitated */
/* States 2~7 reserved for future use */

/* ====== Eye State Mapping ====== */
typedef enum {
    EYE_DYING = 0,      /* Arrest: min brightness white + OLED black */
    EYE_NORMAL = 1      /* Resuscitated: full white + OLED white with black circle */
} eye_state_et;

/* ====== WS2812B Timing (72MHz APB2, TIM1 PWM) ====== */
#define WS2812B_PERIOD      89      /* 89+1 = 90 ticks = 1.25us @ 72MHz */
#define WS2812B_T0H         27      /* 27+1 = 28 ticks = 0.389us (logic 0 high) */
#define WS2812B_T1H         55      /* 55+1 = 56 ticks = 0.778us (logic 1 high) */
#define WS2812B_RESET       60      /* >=50us reset = >=3600 ticks; use 60 * 1.25us = 75us */

/* ====== OLED ST7315 Parameters ====== */
#define OLED_WIDTH          64
#define OLED_HEIGHT         48
#define OLED_PAGES          8       /* 48 / 6 = 8 pages (6px per page for ST7315) */
#define OLED_ADDR           0x3C    /* I2C address (7-bit) */
#define OLED_BUF_SIZE       (OLED_WIDTH * OLED_PAGES)

/* ====== Function Prototypes ====== */

/* sys_clock.c */
void SystemClock_Config(void);
void SysTick_Init(void);
uint32_t SysTick_GetTick(void);
void Delay_ms(uint32_t ms);
void Delay_us(uint32_t us);

/* head_gpio.c */
void Head_GPIO_Init(void);
uint8_t Pupil_GetState(void);

/* head_ws2812b.c */
void WS2812B_Init(void);
void WS2812B_SetWhite(uint8_t brightness_pct);
void WS2812B_Show(void);
void WS2812B_SetEyeState(eye_state_et state);

/* head_oled.c */
void OLED_Init(void);
void OLED_Clear(void);
void OLED_FillWhite(void);
void OLED_DrawCircle(int cx, int cy, int r, uint8_t color);
void OLED_Flush(void);
void OLED_SetEyeState(eye_state_et state);

/* head_uart.c */
void UART_Init(void);
void UART_Print(const char *str);
void UART_Printf(const char *fmt, ...);

#endif /* __HEAD_SYS_H__ */
