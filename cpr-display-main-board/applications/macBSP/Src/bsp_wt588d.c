/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-10-16     Administrator       the first version
 */
#include "bsp_wt588d.h"



/**
  * @brief  Delay function(uint:ms)
  * @param  us 微妙数
  * @retval void
  */
void WT588D_Delay_us(uint32_t us)
{
    for(; us; us--) __NOP();
}


/**
  * @brief  WT588D Chip send one byte and select tube to write
  * @param  data 需要写入的数据
  * @retval void
  * @note   NULL
  */
/* 发送 1 字节（低位先行） */
void WT588D_Write_Byte(rt_uint8_t data)
{
    for(rt_uint8_t i = 0; i < 8; i++)
    {
        WT588D_CLK_L();
        if(data & 0x01){
            WT588D_DATA_H();
        }
        else {
            WT588D_DATA_L();
        }
        WT588D_Delay_us(150);
        WT588D_CLK_H();
        WT588D_Delay_us(150);
        data >>= 1;
    }
    WT588D_CLK_L();
}




/**
  * @brief  WT588D set command
  * @param  cmd  : 数据命令指令
  * @retval void
  * @note
  *         ---------------------------------------------------
  *-          指令      |    功能              |   说明
  *         ---------------------------------------------------
  *          0xE0~0xE7  |   音量调节                        |      在语音播放或者待机状态发此命令可以调节 8 级音量
  *                     |                      |   E0H 最小，E7H 音量最大
  *         ---------------------------------------------------
  *          0xF2       |   循环播放                        |       在语音播放过程中发送此命令可循环播放当前地址语音
  *         ---------------------------------------------------
  *          0xFE       |   停止播放                        |       停止播放语音命令
  *         ---------------------------------------------------
  *          0xF5       |   进入控制端口扩展输出   |       在常规三线串口模式下，发送此命令可进入控制端口扩展输出状态
  *         ---------------------------------------------------
  *          0xF6       |   退出控制端口扩展输出   |       在控制端口扩展输出状态下，发送此命令可进入常规三线串口控制模式
  *         ---------------------------------------------------
  *          0x00~0xDB  |   播放第 0 ~219段语音     |
  *
  */
void WT588D_Set_Cmd(rt_uint8_t cmd)
{
    WT588D_CS_L();
    rt_thread_mdelay(5);
    WT588D_Write_Byte(cmd);

    WT588D_CS_H();
}



/**
  * @brief  Check wt588d is busy or not.
  * @param  void
  * @retval 1: busy
  *         0: free
  * @note   NULL
  */
rt_uint8_t WT588D_Busy_Check(void)
{
    static rt_uint8_t status = 0;

    status = HAL_GPIO_ReadPin(WT588D_BUSY_GPIO_Port, WT588D_BUSY_Pin);

    return status;
}



/**
  * @brief  WT588D control loudspeaker loop playback
  * @param  void
  * @retval void
  * @note   循环播放指令在发送之前需要与其他指令有至少30ms的时间间隔
  */
void WT588D_Loop_Playback(void)
{
    rt_thread_mdelay(40);
    WT588D_Set_Cmd(WT588D_CMD_LOOP_PLAYBACK);
}



/**
  * @brief  WT588D control loudspeaker stop
  * @param  void
  * @retval void
  * @note   停止播放指令在发送之前需要与其他指令有至少1ms的时间间隔
  */
void WT588D_Stop(void)
{
    rt_thread_mdelay(2);
    WT588D_Set_Cmd(WT588D_CMD_LOOP_PLAYBACK);
}



/**
  * @brief  WT588D control loudspeaker stop
  * @param  cmd: 音量调节指令
  * @retval void
  * @note   在语音芯片工作状态中发送此命令可以调节音量大小，不管语音芯片是否处于语音播放还是语音停止状态。
                        如果是先触发地址语音或者其他的命令，则需要等待 90ms 才能发送音量调节命令，否则无效。
  */
void WT588D_Set_Volume(rt_uint8_t cmd)
{
    WT588D_Set_Cmd(cmd);
}









