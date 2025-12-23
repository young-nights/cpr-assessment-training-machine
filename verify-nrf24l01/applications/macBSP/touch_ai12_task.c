/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-12-23     18452       the first version
 */
#include "bsp_sys.h"



/* 把 4 位 BCD 值映射到键码表 */
static const AI12_Key_t key_map[16] =
{
    /* 0x00 */ AI12_KEY_1,
    /* 0x01 */ AI12_KEY_2,
    /* 0x02 */ AI12_KEY_3,
    /* 0x03 */ AI12_KEY_4,
    /* 0x04 */ AI12_KEY_5,
    /* 0x05 */ AI12_KEY_6,
    /* 0x06 */ AI12_KEY_7,
    /* 0x07 */ AI12_KEY_8,
    /* 0x08 */ AI12_KEY_9,
    /* 0x09 */ AI12_KEY_10,
    /* 0x0A */ AI12_KEY_11,
    /* 0x0B */ AI12_KEY_12,
    /* 0x0C..0x0E 未使用，返回 NONE */
    [0x0C]  = AI12_KEY_NONE,
    [0x0D]  = AI12_KEY_NONE,
    [0x0E]  = AI12_KEY_NONE,
    /* 0x0F = 1111b → 无按键 */
    [0xF]  = AI12_KEY_NONE
};



/***
 *  @brief  触摸芯片扫描函数
 *  @return 返回键值信息
 **/
rt_uint8_t bcd,bcd0,bcd1,bcd2,bcd3 = 0;
AI12_Key_t AI12_ScanKey(void)
{

    static rt_uint8_t index = 0;

    bcd0  = HAL_GPIO_ReadPin(BCD_0_GPIO_Port, BCD_0_Pin) ? 1 : 0;
    bcd1  = HAL_GPIO_ReadPin(BCD_1_GPIO_Port, BCD_1_Pin) ? 1 : 0;
    bcd2  = HAL_GPIO_ReadPin(BCD_2_GPIO_Port, BCD_2_Pin) ? 1 : 0;
    bcd3  = HAL_GPIO_ReadPin(BCD_3_GPIO_Port, BCD_3_Pin) ? 1 : 0;

    bcd = (bcd3 << 3) + (bcd2 << 2) + (bcd1 << 1) + bcd0;
//    rt_kprintf("%x\r\n",bcd);
    /* 计算索引值 */
    index = bcd & 0x0F;

    /* 返回键值 */
    return key_map[index];
}





/**
  * @brief  This thread entry is used for touch check
  * @retval void
  */
void AI12_Thread_entry(void* parameter)
{

    AI12_Key_t keyValue;

    for(;;)
    {
        keyValue = AI12_ScanKey();

        if(keyValue == AI12_KEY_NONE){

        }
        else if(keyValue == AI12_KEY_1){
            rt_kprintf("Press1\r\n");
        }
        else if(keyValue == AI12_KEY_2){
            rt_kprintf("Press2\r\n");
        }
        else if(keyValue == AI12_KEY_3){
            rt_kprintf("Press3\r\n");
        }
        else if(keyValue == AI12_KEY_4){
            rt_kprintf("Press4\r\n");
        }
        else if(keyValue == AI12_KEY_5){
            rt_kprintf("Press5\r\n");
        }
        else if(keyValue == AI12_KEY_6){
            rt_kprintf("Press6\r\n");
        }
        else if(keyValue == AI12_KEY_7){
            rt_kprintf("Press7\r\n");
        }
        else if(keyValue == AI12_KEY_8){
            rt_kprintf("Press8\r\n");
        }
        else if(keyValue == AI12_KEY_9){
            rt_kprintf("Press9\r\n");
        }
        else if(keyValue == AI12_KEY_10){
            rt_kprintf("Press10\r\n");
        }
        else if(keyValue == AI12_KEY_11){
            rt_kprintf("Press11\r\n");
        }
        else if(keyValue == AI12_KEY_12){
            rt_kprintf("Press12\r\n");
        }

        rt_thread_mdelay(50);
    }


}



/**
  * @brief  This is a Initialization for water level Check
  * @retval int
  */
int AI12_Thread_Init(void)
{
    rt_thread_t AI12_Task_Handle = RT_NULL;
    /* 创建检查一些系统状态标志的线程  -- 优先级：25 */
    AI12_Task_Handle = rt_thread_create("AI12_Thread_entry", AI12_Thread_entry, RT_NULL, 512, 25, 300);
    /* 检查是否创建成功,成功就启动线程 */
    if(AI12_Task_Handle != RT_NULL)
    {
        rt_kprintf("PRINTF:%d. AI12_Thread_entry is Succeed!! \r\n",Record.kprintf_cnt++);
        rt_thread_startup(AI12_Task_Handle);
    }
    else {
        rt_kprintf("PRINTF:%d. AI12_Thread_entry is Failed \r\n",Record.kprintf_cnt++);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(AI12_Thread_Init);

















