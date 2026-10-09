/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author      Notes
 * 2024-11-19     teati       the first version
 * 2026-09-27     coder       complete carotid pulse motor state machine
 */
#include <rtt_system_work.h>

extern void coreless_motor_ctrl(MOTOR_NAME_et name, SWITCH_et status);
extern char coreless_motolr_read_key1(void);
extern char coreless_motolr_read_key2(void);
extern volatile int8_t g_raster_press_dir;

/* ====== Motor pulse simulation states ====== */
#define MOTOR_OFF           0   /* No pulse - cardiac arrest */
#define MOTOR_PASSIVE       1   /* Passive pulse - follows compression rate */
#define MOTOR_AUTONOMOUS    2   /* Autonomous pulse (ROSC) */

/* Autonomous pulse timing (fixed ~70 bpm: 860ms period, ~100ms vibration) */
#define ROSC_PERIOD_TICKS   86  /* 86 x 10ms = 860ms (~70 bpm) */
#define ROSC_VIBRATE_TICKS  10  /* 10 x 10ms = 100ms vibration on */

/* Touch palpation timing */
#define PALPATE_MAX_TICKS   50  /* 50 x 10ms = 500ms max vibration per press */

static uint16_t rosc_ticks = 0;
static uint8_t  rosc_phase = 0;
static uint16_t motor1_palpate_ticks = 0;
static uint16_t motor2_palpate_ticks = 0;


static void Timing_1ms(void)
{
}


static void Timing_10ms(void)
{
    /* Not connected or not started: both motors OFF (no pulse) */
    if (Flag.start == 0 || Record.nrf_if_connected == 0)
    {
        coreless_motor_ctrl(Coreless_motor_1, OFF);
        coreless_motor_ctrl(Coreless_motor_2, OFF);
        rosc_ticks = 0;
        rosc_phase = 0;
        motor1_palpate_ticks = 0;
        motor2_palpate_ticks = 0;
        return;
    }

    switch (Record.motor_work_sta)
    {
        /* Mode 0: Motor OFF - cardiac arrest, no carotid pulse */
        case MOTOR_OFF:
            coreless_motor_ctrl(Coreless_motor_1, OFF);
            coreless_motor_ctrl(Coreless_motor_2, OFF);
            break;

        /* Mode 1: Passive pulse - motor vibrates at compression rate.
         * ON during press-down (dir==1), OFF during rebound/release.
         * Rhythm naturally follows rescuer's compression frequency. */
        case MOTOR_PASSIVE:
            if (g_raster_press_dir == 1)
            {
                coreless_motor_ctrl(Coreless_motor_1, ON);
                coreless_motor_ctrl(Coreless_motor_2, ON);
            }
            else
            {
                coreless_motor_ctrl(Coreless_motor_1, OFF);
                coreless_motor_ctrl(Coreless_motor_2, OFF);
            }
            break;

        /* Mode 2: Autonomous pulse (ROSC) - fixed ~70 bpm vibration */
        case MOTOR_AUTONOMOUS:
            rosc_ticks++;
            if (rosc_ticks >= ROSC_PERIOD_TICKS)
            {
                rosc_ticks = 0;
                rosc_phase ^= 1;
            }
            if (rosc_phase == 0 && rosc_ticks < ROSC_VIBRATE_TICKS)
            {
                coreless_motor_ctrl(Coreless_motor_1, ON);
                coreless_motor_ctrl(Coreless_motor_2, ON);
            }
            else
            {
                coreless_motor_ctrl(Coreless_motor_1, OFF);
                coreless_motor_ctrl(Coreless_motor_2, OFF);
            }
            break;

        default:
            coreless_motor_ctrl(Coreless_motor_1, OFF);
            coreless_motor_ctrl(Coreless_motor_2, OFF);
            break;
    }

    /* Touch palpation: KEY1/KEY2 trigger brief vibration (500ms max).
     * Active regardless of motor_work_sta for pulse check simulation. */
    if (coreless_motolr_read_key1() == 1)
    {
        if (motor1_palpate_ticks < PALPATE_MAX_TICKS)
        {
            motor1_palpate_ticks++;
            coreless_motor_ctrl(Coreless_motor_1, ON);
        }
    }
    else
    {
        motor1_palpate_ticks = 0;
    }

    if (coreless_motolr_read_key2() == 1)
    {
        if (motor2_palpate_ticks < PALPATE_MAX_TICKS)
        {
            motor2_palpate_ticks++;
            coreless_motor_ctrl(Coreless_motor_2, ON);
        }
    }
    else
    {
        motor2_palpate_ticks = 0;
    }
}


static void Timing_50ms(void)
{
}


static void Timing_500ms(void)
{
}


static void Timing_1s(void)
{
}


/*---------------------------------------------------------------------------------------------------------------*/
/* 以下是系统扫描线程的创建以及回调函数                                                                          */
/*---------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  sysTimer Callback Function -- 1ms tick, dispatches to 1/10/50/500/1000ms handlers
  * @retval void
  */
static rt_uint32_t sysTimeTick = 0;
static void sysTimer_callback(void* parameter)
{
    sysTimeTick++;

    if(sysTimeTick > 60000){
        sysTimeTick = 0;
    }

    if((sysTimeTick % 1)    == 0)   Timing_1ms();
    if((sysTimeTick % 10)   == 0)   Timing_10ms();
    if((sysTimeTick % 50)   == 0)   Timing_50ms();
    if((sysTimeTick % 500)  == 0)   Timing_500ms();
    if((sysTimeTick % 1000) == 0)   Timing_1s();
}


/**
  * @brief  sysTimer initialize
  * @retval int
  */
rt_timer_t sysTimer;
int sysTimer_Init(void)
{
    sysTimer = rt_timer_create("sysTimer_callback", sysTimer_callback, RT_NULL, 1, RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_PERIODIC);
    if(sysTimer != RT_NULL)
    {
        rt_kprintf("PRINTF:%d. sysTimer initialize succeed!\r\n",Record.kprintf_cnt++);
        rt_timer_start(sysTimer);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(sysTimer_Init);
