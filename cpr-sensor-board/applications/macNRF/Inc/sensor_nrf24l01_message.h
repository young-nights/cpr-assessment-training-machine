/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-12     18452       the first version
 */
#ifndef APPLICATIONS_MACNRF_INC_SENSOR_NRF24L01_MESSAGE_H_
#define APPLICATIONS_MACNRF_INC_SENSOR_NRF24L01_MESSAGE_H_

#include "bsp_sys.h"

/* 前向声明，避免 circular include 导致 nrf24_t 未定义 */
typedef struct nRF24L01_STRUCT *nrf24_t;

#define DEVICE_SENSOR_ID_H     0x00
#define DEVICE_SENSOR_ID_L     0x05

#define DEVICE_MAINBOARD_ID_H   0x00
#define DEVICE_MAINBOARD_ID_L   0x01

/* 函数进行解析指令后的返回宏 */
#define CMD_ERROR   0
#define CMD_TRUE    1

// Minimum full frame: 2 headers + 1 LEN + 4 min payload (ID+TYPE+STATUS) + 2 CRC = 9
#define CMD_MINI_LENGTH 9


/* 解析指令数据的指令类型以及状态宏 */
//------------------------------------------------------------------------
#define       FRAME_HEAD1                                        (0x55)      // 帧头1
#define       FRAME_HEAD2                                        (0xAA)      // 帧头2
#define       FRAME_TYPE_ACT                                     (0x31)      // 帧类型:动作命令
#define       FRAME_TYPE_SET                                     (0x32)      // 帧类型:参数设置
#define       FRAME_TYPE_GET                                     (0x33)      // 帧类型:参数获取
#define       FRAME_TYPE_POST                                    (0x66)      // 帧类型:主动上报
#define       FRAME_STATE_ASK                                    (0x01)      // 帧状态:上位请求
#define       FRAME_STATE_ACK                                    (0x02)      // 帧状态:下位应答
#define       FRAME_STATE_ERR                                    (0x00)      // 帧状态:校验出错


// 指令宏------------------------------------------------------------
#define      FRAME_NRF24_ASK_CONNECT_PANEL_CMD                   (0x01)      // 设置：连接控制面板指令
#define      FRAME_NRF24_ACK_START_CMD                           (0x02)      // 回应：主板发送的开始指令
#define      FRAME_NRF24_ASK_SHOKE_SENSOR_CMD                    (0x03)      // 设置：压电陶瓷片
#define      FRAME_NRF24_ACK_WS2812B_LEVEL_CMD                   (0x04)      // 回应：眼灯挡位
#define      FRAME_NRF24_ACK_MOTOR_STATUS_CMD                    (0x05)      // 回应：空心杯电机工作模式（脉搏检测）
#define      FRAME_NRF24_ASK_CC6201_CMD                          (0x06)      // 设置：磁传感器工作状态（异物检测）




/**
  * @brief  枚举类型,指令解码步骤
  * @param  None
  */
typedef enum
{
    Decode_Step_0 = 0,
    Decode_Step_1,
    Decode_Step_2,
    Decode_Step_3,
    Decode_Step_4,
    Decode_Step_5,
    Decode_Step_6
}DecodeStep_et;




/**
  * @brief  枚举类型,指令码
  * @param  None
  */
typedef enum
{
    Order_nRF24L01_ASK_Connect_Control_Panel = 0,
    Order_nRF24L01_ACK_Start_Cmd,
    Order_nRF24L01_ASK_Shoke_Sensor_Cmd,
    Order_nRF24L01_ACK_WS2812_Level_Cmd,
    Order_nRF24L01_ACK_Motor_Status_Cmd,
    Order_nRF24L01_ASK_CC6201_Cmd,

}nRF24L01_Order_StructType;



/* ==================== 数据来源区分（与主板统一） ==================== */
typedef enum {
    SRC_UNKNOWN = 0,
    SRC_FROM_SENSOR,
    SRC_FROM_REMOTE,
    SRC_FROM_MAIN
} cpr_src_type_t;

uint16_t CrcCalc_Crc16Modbus(uint8_t *dat, uint8_t len);
rt_uint8_t nrf24l01_build_frame(uint8_t cmd_type, uint8_t cmd_status,uint8_t *data, uint8_t data_len,uint8_t *out_frame);
void nrf24l01_protocol_operation(uint8_t* CmdBuf, cpr_src_type_t src);
uint8_t nrf24l01_portocol_get_command(const uint8_t *cmdBuf, const uint16_t cmdLength, cpr_src_type_t *out_src);



#endif /* APPLICATIONS_MACNRF_INC_SENSOR_NRF24L01_MESSAGE_H_ */
