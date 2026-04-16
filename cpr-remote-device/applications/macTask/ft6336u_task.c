/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-07-01     Administrator       the first version
 */
#include "bsp_sys.h"


#define USE_EXTI_FUNC 1

#if USE_EXTI_FUNC
/**
  * @brief  FT6336U的INT引脚的中断回调函数
  * @retval void
  */
static void FT6336U_INT_Callback(void *args)
{

    rt_interrupt_enter();

    if (rt_pin_read(GET_PIN(A,6)) == PIN_LOW){
        /* 下降沿：手指按下 */
        Record.touch_down_flag = 1;
//        rt_kprintf("down\r\n");
    }
    else{
        /* 上升沿：手指离开 */
        Record.touch_down_flag = 0;
//        rt_kprintf("up\r\n");
    }

    rt_interrupt_leave();
}




/**
  * @brief
  * @retval void
  */
static int FT6336U_INT_GPIO_Config(void)
{
    rt_pin_mode(GET_PIN(A, 6), PIN_MODE_INPUT);  /* 保险起见 */
    rt_pin_attach_irq(  GET_PIN(A, 6),
                        PIN_IRQ_MODE_RISING_FALLING,    /* 与 CubeMX 极性一致 */
                        FT6336U_INT_Callback,
                        RT_NULL);                       /* 不需要参数 */
    rt_pin_irq_enable(GET_PIN(A, 6), PIN_IRQ_ENABLE);
    return 0;
}
INIT_APP_EXPORT(FT6336U_INT_GPIO_Config);

#endif /* USE_EXTI_FUNC */


/**
  * @brief  This thread entry is used for touch check
  * @retval void
  */
void FT6336U_Thread_entry(void* parameter)
{


    FT6336U_Reset();
    ft6336u_device_init();
    FT6336U_Read_Info(ft6336u_iic.i2c_bus, &ft6336u_info);

    for(;;)
    {

        Record.touch_fingers = FT6336U_Read_Finger_Number(ft6336u_iic.i2c_bus);
        /* 有中断事件才读寄存器 */
        if (Record.touch_down_flag == 1){
            FT6336U_Read_Pressed_Point_xy(ft6336u_iic.i2c_bus);
            Record.touch_down_flag = 2;
        }
        else if(Record.touch_down_flag == 0){
            Record.touch_down_flag = 0;
            Record.touch_fingers = 0;
        }

        rt_thread_mdelay(10);
    }
}



/**
  * @brief  This is a Initialization for water level Check
  * @retval int
  */
int FT6336U_Thread_Init(void)
{
    rt_thread_t FT6336U_Task_Handle = RT_NULL;
    FT6336U_Task_Handle = rt_thread_create("FT6336U_Thread_entry", FT6336U_Thread_entry, RT_NULL, 1024, 25, 300);
    /* 检查是否创建成功,成功就启动线程 */
    if(FT6336U_Task_Handle != RT_NULL)
    {
        rt_kprintf("PRINTF:%d. FT6336U_Thread_entry is Succeed!! \r\n",Record.kprintf_cnt++);
        rt_thread_startup(FT6336U_Task_Handle);
    }
    else {
        rt_kprintf("PRINTF:%d. FT6336U_Thread_entry is Failed \r\n",Record.kprintf_cnt++);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(FT6336U_Thread_Init);







