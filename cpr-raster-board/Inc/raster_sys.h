/**
 * @file    raster_sys.h
 * @brief   Raster board system configuration, pin definitions, and global types
 *
 * Board:  STM32F103C6T6 (LQFP-48), bare-metal, HSE 8MHz -> 72MHz
 * Role:   Grating ruler pulse acquisition (press / blow), pure data collector
 *
 * Pin map (hardware v1.1 refactor):
 *   USART1  PA9=TX  PA10=RX   debug console (115200)
 *   USART2  PA2=TX  PA3=RX    Sensor board link (115200)
 *   OUT1    PA5               photoelectric switch (press pair A)
 *   OUT2    PA6               photoelectric switch (press pair B)
 *   OUT3    PB8               photoelectric switch (blow pair A)
 *   OUT4    PB9               photoelectric switch (blow pair B)
 *
 * Change Logs:
 * Date           Author      Notes
 * 2026-10-21     coder       the first version (STM32F103C6T6 refactor)
 */
#ifndef __RASTER_SYS_H__
#define __RASTER_SYS_H__

#include <stdint.h>

/* ====== MCU Register Definitions (STM32F103C6T6) ====== */
#include "stm32f103x6.h"

/* ====== System Clock ====== */
#define SYSCLK_FREQ_HZ      72000000UL
#define APB1_FREQ_HZ        36000000UL
#define APB2_FREQ_HZ        72000000UL

/* ====== Pin Definitions ====== */

/* Photoelectric switches (slot sensors, EXTI rising edge) */
#define OUT1_PORT           GPIOA   /* PA5, press pair channel A */
#define OUT1_PIN            5
#define OUT2_PORT           GPIOA   /* PA6, press pair channel B */
#define OUT2_PIN            6
#define OUT3_PORT           GPIOB   /* PB8, blow pair channel A */
#define OUT3_PIN            8
#define OUT4_PORT           GPIOB   /* PB9, blow pair channel B */
#define OUT4_PIN            9

/* Debug UART (USART1) */
#define DBG_UART_TX_PORT    GPIOA   /* PA9 */
#define DBG_UART_TX_PIN     9
#define DBG_UART_RX_PORT    GPIOA   /* PA10 */
#define DBG_UART_RX_PIN     10

/* Sensor board UART (USART2) */
#define SENSOR_UART_TX_PORT GPIOA   /* PA2 */
#define SENSOR_UART_TX_PIN  2
#define SENSOR_UART_RX_PORT GPIOA   /* PA3 */
#define SENSOR_UART_RX_PIN  3

/* ====== Raster Operating Mode ====== */
typedef enum {
    RASTER_MODE_IDLE   = 0x02,      /* idle / no reporting */
    RASTER_MODE_PRESS  = 0x01,      /* press detection */
    RASTER_MODE_BLOW   = 0x03       /* blow detection */
} raster_mode_et;

typedef enum {
    RASTER_STATE_IDLE   = 0,        /* not collecting */
    RASTER_STATE_ACTIVE = 1         /* collecting and reporting */
} raster_state_et;

/* ====== Protocol Constants (Sensor <-> Raster, UART2) ====== */
#define FRAME_HEAD          0xAA
#define FRAME_TAIL          0x55
#define LEN_CMD             0x02    /* Sensor -> Raster command frame length */
#define LEN_REALTIME        0x04    /* Raster -> Sensor realtime frame length */

#define CMD_START           0x01    /* start acquisition */
#define CMD_STOP            0x03    /* stop acquisition */
#define CMD_MODE            0x11    /* mode switch, DATA selects mode */

#define TYPE_PRESS          0x01
#define TYPE_BLOW           0x02

#define REPORT_PERIOD_MS    100U    /* realtime frame period */

/* ====== Function Prototypes ====== */

/* sys_clock.c */
void SystemClock_Config(void);
void SysTick_Init(void);
uint32_t SysTick_GetTick(void);
void Delay_ms(uint32_t ms);

/* raster_uart1.c (debug) */
void UART1_Init(void);
void UART1_Print(const char *str);
void UART1_Printf(const char *fmt, ...);

/* raster_gpio.c (photoelectric switches) */
void Raster_GPIO_Init(void);
int8_t Raster_GetPressDir(void);
int8_t Raster_GetBlowDir(void);
int16_t Raster_TakePressPulse(void);
int16_t Raster_TakeBlowPulse(void);

/* raster_uart2.c (Sensor link) */
void UART2_Init(void);
void UART2_SendFrame(const uint8_t *buf, uint8_t len);
int UART2_RecvByte(uint8_t *byte);
void UART2_PollCommand(raster_state_et *state, raster_mode_et *mode);

/* raster_task.c (acquisition + report state machine) */
void Raster_TaskInit(void);
void Raster_TaskPoll(void);

#endif /* __RASTER_SYS_H__ */
