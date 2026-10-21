/**
 * @file    main.c
 * @brief   Raster board main entry - grating pulse acquisition (STM32F103C6T6)
 *
 * Role: pure data collector between grating photoelectric switches and Sensor
 *   - USART1: debug console
 *   - USART2: Sensor link (start/stop/mode in, pulse delta out every 100ms)
 *   - OUT1~OUT4: photoelectric switch pulse capture (EXTI)
 *
 * Change Logs:
 * Date           Author      Notes
 * 2026-10-21     coder       the first version (STM32F103C6T6 refactor)
 */
#include "raster_sys.h"

int main(void)
{
    /* 1. System clock: HSE 8MHz -> 72MHz */
    SystemClock_Config();

    /* 2. SysTick 1ms tick */
    SysTick_Init();

    /* 3. Debug UART1 */
    UART1_Init();
    UART1_Print("\r\n========================================\r\n");
    UART1_Print("[RASTER] CPR Raster Board v1.1\r\n");
    UART1_Print("[RASTER] MCU: STM32F103C6T6 @ 72MHz\r\n");
    UART1_Print("[RASTER] DBG UART1: PA9(TX)/PA10(RX) 115200\r\n");
    UART1_Print("[RASTER] SENSOR UART2: PA2(TX)/PA3(RX) 115200\r\n");
    UART1_Print("[RASTER] OUT1=PA5 OUT2=PA6 OUT3=PB8 OUT4=PB9\r\n");
    UART1_Print("========================================\r\n");

    /* 4. Photoelectric switch GPIO + EXTI */
    Raster_GPIO_Init();
    UART1_Print("[RASTER] Photoelectric switches: EXTI rising edge enabled\r\n");

    /* 5. Sensor link UART2 */
    UART2_Init();
    UART1_Print("[RASTER] USART2 Sensor link initialized\r\n");

    /* 6. Acquisition state machine */
    Raster_TaskInit();
    UART1_Print("[RASTER] State: IDLE (waiting for START from Sensor)\r\n");
    UART1_Print("[RASTER] Entering main loop...\r\n");

    /* ====== Main loop: poll Sensor commands + 100ms report ====== */
    while (1) {
        Raster_TaskPoll();
    }

    /* Never reached */
    return 0;
}
