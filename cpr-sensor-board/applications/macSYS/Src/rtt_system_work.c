/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author      Notes
 * 2024-11-19     teati       the first version
 */
#include <rtt_system_work.h>





static void Timing_1ms(void)
{

}



static int motor1_ticks = 0;
static int motor2_ticks = 0;
static int motor1_print = 0;
static int motor2_print = 0;
extern void coreless_motor_ctrl(MOTOR_NAME_et name,SWITCH_et status);
static void Timing_10ms(void)
{
    // 空心杯电机1控制逻辑----------------------------------------------------
    if(coreless_motolr_read_key1() == 1)
    {
        motor1_ticks++;
        if(motor1_ticks < 50){
            coreless_motor_ctrl(Coreless_motor_1, ON);
            if(motor1_print == 6){
                motor1_print = 0;
            }
            for(; motor1_print < 2; motor1_print = 3){
                rt_kprintf("key1 pressed\r\n");
            }
        }
        else if(motor1_ticks >= 50 && motor1_ticks < 100) {
            coreless_motor_ctrl(Coreless_motor_1, OFF);
            for(; motor1_print < 5; motor1_print = 6){
                rt_kprintf("key1 release\r\n");
            }
        }
        else{
            motor1_ticks = 0;
            motor1_print = 0;
        }
    }
    else{
        motor1_ticks = 0;
        coreless_motor_ctrl(Coreless_motor_1, OFF);
    }

    // 空心杯电机2控制逻辑----------------------------------------------------
    if(coreless_motolr_read_key2() == 1)
    {
        motor2_ticks++;
        if(motor2_ticks < 50){
            coreless_motor_ctrl(Coreless_motor_2, ON);

            if(motor2_print == 6){
                motor2_print = 0;
            }
            for(; motor2_print < 2; motor2_print = 3){
                rt_kprintf("key2 pressed\r\n");
            }

        }
        else if(motor2_ticks >= 50 && motor2_ticks < 100) {
            coreless_motor_ctrl(Coreless_motor_2, OFF);

            for(; motor2_print < 5; motor2_print = 6){
                rt_kprintf("key2 release\r\n");
            }
        }
        else{
            motor2_ticks = 0;
            motor2_print = 0;
        }
    }
    else{
        motor2_ticks = 0;
        coreless_motor_ctrl(Coreless_motor_2, OFF);
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
  * @brief  sysTimer Callback Function -- 10ms entry
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
  * @brief  keyTimer initialize
  * @retval int
  */
rt_timer_t sysTimer;
int sysTimer_Init(void)
{
    /* 创建key软件定时器线程 */
    sysTimer = rt_timer_create("sysTimer_callback", sysTimer_callback, RT_NULL, 1, RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_PERIODIC);
    /* 如果keyTimer句柄创建成功,开启软件定时器 */
    if(sysTimer != RT_NULL)
    {
        rt_kprintf("PRINTF:%d. sysTimer initialize succeed!\r\n",Record.kprintf_cnt++);
        rt_timer_start(sysTimer);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(sysTimer_Init);







