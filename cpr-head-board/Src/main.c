/**
 * @file    main.c
 * @brief   Head board main entry - pupil state display (WS2812B + OLED)
 *
 * Clinical role:
 *   - Pupil state 0 (arrest/dilated): WS2812B 3% white + OLED black
 *   - Pupil state 1 (resuscitated):   WS2812B 100% white + OLED white+black circle
 *
 * Data source: Sensor board GPIO PC10/PC11/PC12 -> PA2/PA3/PA4 (3-bit encoding)
 *
 * Change Logs:
 * Date           Author      Notes
 * 2026-09-27     coder       the first version
 */
#include "head_sys.h"

static eye_state_et s_current_state = EYE_DYING;
static uint8_t      s_last_pupil = 0xFF;  /* Force initial update */

/**
 * @brief  Map pupil GPIO state to eye state
 */
static eye_state_et pupil_to_eye(uint8_t pupil_state)
{
    /* State 0 = arrest/dilated, State 1 = resuscitated/normal */
    /* States 2~7 = reserved, treat as dying for safety */
    return (pupil_state == PUPIL_STATE_NORMAL) ? EYE_NORMAL : EYE_DYING;
}

/**
 * @brief  Apply eye state to both WS2812B and OLED
 */
static void apply_eye_state(eye_state_et state)
{
    WS2812B_SetEyeState(state);
    OLED_SetEyeState(state);
    s_current_state = state;
}

/**
 * @brief  Main entry point
 */
int main(void)
{
    /* 1. System clock: HSE 8MHz -> 72MHz */
    SystemClock_Config();

    /* 2. SysTick 1ms tick */
    SysTick_Init();

    /* 3. GPIO (pupil input + OLED pins + LED pin + UART pins) */
    Head_GPIO_Init();

    /* 4. Debug UART */
    UART_Init();
    UART_Print("\r\n========================================\r\n");
    UART_Print("[HEAD] CPR Head Board v1.0\r\n");
    UART_Print("[HEAD] MCU: STM32F103C6T6 @ 72MHz\r\n");
    UART_Print("[HEAD] Pupil GPIO: PA2/PA3/PA4\r\n");
    UART_Print("[HEAD] WS2812B: PA11 (TIM1_CH4 PWM+DMA)\r\n");
    UART_Print("[HEAD] OLED: PC10(SCL)/PC11(SDA) soft I2C\r\n");
    UART_Print("========================================\r\n");

    /* 5. WS2812B RGB LED */
    WS2812B_Init();
    UART_Print("[HEAD] WS2812B initialized (default: dying state)\r\n");

    /* 6. OLED eye display */
    OLED_Init();
    UART_Print("[HEAD] OLED ST7315 initialized (64x48, black screen)\r\n");

    /* 7. Initial eye state: dying (pupils dilated) */
    apply_eye_state(EYE_DYING);
    UART_Print("[HEAD] Initial state: EYE_DYING (pupils dilated, 3% white + black)\r\n");
    UART_Print("[HEAD] Entering main loop...\r\n");

    /* ====== Main loop: poll pupil GPIO -> decode -> update eye display ====== */
    while (1) {
        uint8_t pupil_state = Pupil_GetState();

        if (pupil_state != s_last_pupil) {
            s_last_pupil = pupil_state;
            eye_state_et new_state = pupil_to_eye(pupil_state);

            UART_Printf("[HEAD] Pupil GPIO=0x%x -> state=%d (%s)\r\n",
                        pupil_state, (int)new_state,
                        (new_state == EYE_NORMAL) ? "NORMAL" : "DYING");

            if (new_state != s_current_state) {
                apply_eye_state(new_state);
                UART_Printf("[HEAD] Eye updated: %s\r\n",
                            (new_state == EYE_NORMAL) ?
                            "RGB 100% white + OLED white/black circle" :
                            "RGB 3% white + OLED black");
            }
        }

        /* Small delay to avoid excessive polling */
        Delay_ms(10);
    }

    /* Never reached */
    return 0;
}
