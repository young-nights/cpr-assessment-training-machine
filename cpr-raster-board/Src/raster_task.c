/**
 * @file    raster_task.c
 * @brief   Acquisition state machine: 100ms pulse-delta report to Sensor
 *
 * Reporting rule (compatible with Sensor joint discrimination):
 *   TYPE = BLOW when mode==BLOW OR latest blow direction != 0
 *   TYPE = PRESS otherwise
 *   Frame: 0xAA 0x04 TYPE CNT_H CNT_L DIR CHK 0x55
 *   CNT is int16 signed pulse increment; DIR is the matching pair direction
 *
 * Change Logs:
 * Date           Author      Notes
 * 2026-10-21     coder       the first version
 */
#include "raster_sys.h"

static raster_state_et s_state = RASTER_STATE_IDLE;
static raster_mode_et  s_mode  = RASTER_MODE_IDLE;
static uint32_t        s_last_report_tick = 0;

/**
 * @brief  Initialize acquisition state (IDLE, mode idle)
 */
void Raster_TaskInit(void)
{
    s_state = RASTER_STATE_IDLE;
    s_mode  = RASTER_MODE_IDLE;
    s_last_report_tick = SysTick_GetTick();
}

/**
 * @brief  Poll UART2 commands; when ACTIVE, send realtime frame every 100ms
 */
void Raster_TaskPoll(void)
{
    UART2_PollCommand(&s_state, &s_mode);

    if (s_state != RASTER_STATE_ACTIVE) {
        return;
    }

    uint32_t now = SysTick_GetTick();
    if ((now - s_last_report_tick) < REPORT_PERIOD_MS) {
        return;
    }
    s_last_report_tick = now;

    /* Decide frame type: blow preferred when mode or direction says so */
    int8_t blow_dir = Raster_GetBlowDir();
    uint8_t type = (s_mode == RASTER_MODE_BLOW || blow_dir != 0)
                   ? TYPE_BLOW : TYPE_PRESS;

    int16_t pulse;
    int8_t dir;
    if (type == TYPE_BLOW) {
        pulse = Raster_TakeBlowPulse();
        /* Drain press pulses so they do not accumulate while in blow mode */
        (void)Raster_TakePressPulse();
        dir = blow_dir;
    } else {
        pulse = Raster_TakePressPulse();
        (void)Raster_TakeBlowPulse();
        dir = Raster_GetPressDir();
    }

    /* Report every 100ms while ACTIVE, zero deltas included (protocol cadence) */
    uint8_t frame[8];
    frame[0] = FRAME_HEAD;
    frame[1] = LEN_REALTIME;
    frame[2] = type;
    frame[3] = (uint8_t)((uint16_t)pulse >> 8);
    frame[4] = (uint8_t)((uint16_t)pulse & 0xFF);
    frame[5] = (uint8_t)dir;

    uint8_t chk = 0;
    for (uint8_t i = 0; i < 6U; i++) {
        chk = (uint8_t)(chk + frame[i]);
    }
    frame[6] = chk;
    frame[7] = FRAME_TAIL;

    UART2_SendFrame(frame, sizeof(frame));

    UART1_Printf("[RASTER] TX type=0x%x pulse=%d dir=%d\r\n",
                 type, (int)pulse, (int)dir);
}
