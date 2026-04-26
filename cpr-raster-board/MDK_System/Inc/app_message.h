/**
  ******************************************************************************
  * @file    app_message.h
  * @brief   光栅板消息发送与接收处理接口
  *
  *          发送: 原始脉冲计数帧 (每100ms, 8字节)
  *          接收: 来自Sensor板的控制指令 (开始/停止)
  ******************************************************************************
  */

#ifndef __APP_MESSAGE_H
#define __APP_MESSAGE_H

#include "app_sys.h"

/**
  * @brief  发送原始脉冲计数和方向数据帧
  * @param  pulse_count: 自上次发送以来的脉冲增量 (int16_t, 正=正向, 负=反向)
  * @param  dir: 方向 (-1=回弹/泄气, 0=静止, 1=下压/充气)
  * @param  type: 数据类型 (0x01=按压, 0x02=吹气)
  * @retval None
  *
  *         帧格式: 0xAA + 0x04 + TYPE + CNT_H + CNT_L + DIR + CHK + 0x55  (8字节)
  *         CHK = Byte[0]+Byte[1]+...+Byte[5] 累加和
  */
void USART1_SendRealtimeData(int16_t pulse_count, int8_t dir, uint8_t type);

/**
  * @brief  从UART1接收缓冲区读取数据，解析并执行指令
  * @param  None
  * @retval None
  *
  *         接收帧格式: 0xAA + LEN + CMD + DATA + CHK + 0x55
  *         开始指令: CMD=0x01, DATA=0xFF → 清零计数器，进入采集状态
  *         停止指令: CMD=0x03, DATA=0xFF → 停止采集，返回待机
  */
void USART1_ProcessRxData(void);

#endif /* __APP_MESSAGE_H */
