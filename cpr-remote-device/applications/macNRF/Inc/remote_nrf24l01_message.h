/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-12     18452       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_BSP_NRF24L01_MESSAGE_H_
#define APPLICATIONS_MACBSP_INC_BSP_NRF24L01_MESSAGE_H_

#include <rtthread.h>
#include <stdint.h>

/* Forward declaration to avoid circular include with remote_nrf24l01_driver.h */
typedef struct nRF24L01_STRUCT *nrf24_t;

#define DEVICE_REMOTE_ID_H   0x00
#define DEVICE_REMOTE_ID_L   0x04

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
#define      FRAME_NRF24_CONNECT_CTRL_PANEL_CMD                  (0x01)      // 设置：连接控制面板指令
#define      FRAME_NRF24_MODE_DATA_IN_CMD                        (0x02)      // 设置：进入考核/竞赛/训练模式指令
#define      FRAME_NRF24_MODE_DATA_OUT_CMD                       (0x03)      // 设置：退出考核/竞赛/训练模式指令
#define      FRAME_NRF24_PRESS_LED_CTRL_CMD                      (0x04)      // 设置：根据接收的指令控制数据页面的按压位置圆形填充色

/* Remote ↔ Mainboard bidirectional communication commands */
#define      FRAME_NRF24_REMOTE_START_CMD                        (0x11)      // Remote → Mainboard: start CPR
#define      FRAME_NRF24_REMOTE_MODE_SWITCH_CMD                  (0x12)      // Remote → Mainboard: switch mode
#define      FRAME_NRF24_REMOTE_START_ACK                        (0x21)      // Mainboard → Remote: start confirmation
#define      FRAME_NRF24_REMOTE_STATUS_SYNC                      (0x22)      // Mainboard → Remote: status sync



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


/* ==================== 数据来源区分（与主板统一） ==================== */
typedef enum {
    SRC_UNKNOWN = 0,
    SRC_FROM_SENSOR,
    SRC_FROM_REMOTE,
    SRC_FROM_MAIN
} cpr_src_type_t;


/**
  * @brief  枚举类型,指令码
  * @param  None
  */
typedef enum
{
    Order_nRF24L01_ASK_Connect_Control_Panel = 0,

    Order_nRF24L01_ASK_Data_Mode_In = 50,
    Order_nRF24L01_ASK_Data_Mode_Out,

    Order_nRF24L01_SEND_To_Main_Start,
    Order_nRF24L01_SEND_To_Main_Mode_Switch,

}nRF24L01_Order_StructType;





uint16_t CrcCalc_Crc16Modbus(uint8_t *dat, uint8_t len);
rt_uint8_t nrf24l01_build_frame(uint8_t cmd_type, uint8_t cmd_status,uint8_t *data, uint8_t data_len,uint8_t *out_frame);
uint8_t nrf24l01_portocol_get_command(const uint8_t *cmdBuf, const uint16_t cmdLength, cpr_src_type_t *out_src);
void nrf24l01_protocol_operation(uint8_t* CmdBuf, cpr_src_type_t src);
void nrf24l01_order_to_pipe(nrf24_t nrf24, uint8_t order, uint8_t pipe_num);




#endif /* APPLICATIONS_MACBSP_INC_BSP_NRF24L01_MESSAGE_H_ */
