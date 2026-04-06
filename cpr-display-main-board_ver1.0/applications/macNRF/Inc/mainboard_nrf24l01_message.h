/*
 * mainboard_nrf24l01_message.h
 * 已适配 Pipe 区分 + 来源识别
 */

#ifndef APPLICATIONS_MACBSP_INC_BSP_NRF24L01_MESSAGE_H_

#define APPLICATIONS_MACBSP_INC_BSP_NRF24L01_MESSAGE_H_

#include <macSYS/Inc/mainboard_sys.h>

/* ==================== 设备ID保持不变 ==================== */
#define DEVICE_ID_H          0x00
#define DEVICE_ID_L          0x04
#define DEVICE_SENSOR_ID_H   0x00
#define DEVICE_SENSOR_ID_L   0x05

/* 函数返回宏 */
#define CMD_ERROR   0
#define CMD_TRUE    1

// 指令的最小长度为4
#define CMD_MINI_LENGTH 4

/* 帧头、帧类型、状态宏（保持你原有定义） */
#define FRAME_HEAD1          (0x55)
#define FRAME_HEAD2          (0xAA)
#define FRAME_TYPE_ACT       (0x31)
#define FRAME_TYPE_SET       (0x32)
#define FRAME_TYPE_GET       (0x33)
#define FRAME_TYPE_POST      (0x66)
#define FRAME_STATE_ASK      (0x01)
#define FRAME_STATE_ACK      (0x02)
#define FRAME_STATE_ERR      (0x00)

/* 指令宏（保持你原有定义） */
#define FRAME_NRF24_CONNECT_CTRL_PANEL_CMD   (0x01)
#define FRAME_NRF24_MODE_DATA_IN_CMD         (0x02)
#define FRAME_NRF24_MODE_DATA_OUT_CMD        (0x03)
#define FRAME_NRF24_SEND_PRESS_DATA_CMD      (0x04)
#define FRAME_NRF24_SEND_TIDAL_DATA_CMD      (0x05)

/* ==================== 数据来源区分 ==================== */
typedef enum {
    SRC_UNKNOWN = 0,
    SRC_FROM_SENSOR,      // 来自传感器板 (Pipe1)
    SRC_FROM_REMOTE,      // 来自遥控设备 (Pipe2)
    SRC_FROM_MAIN         // 来自主板自己（极少用）
} cpr_src_type_t;

/* 指令码枚举 */
typedef enum {
    Order_nRF24L01_ACK_Connect_Control_Panel = 0,
    Order_nRF24L01_ACK_Mode_Data_In,
    Order_nRF24L01_ACK_Mode_Data_Out,

    Order_nRF24L01_SEND_Press_Data = 50,
    Order_nRF24L01_SEND_Tidal_Data,
} nRF24L01_Order_StructType;



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




uint16_t CrcCalc_Crc16Modbus(uint8_t *dat, uint8_t len);

rt_uint8_t nrf24l01_build_frame(uint8_t cmd_type, uint8_t cmd_status,
                                uint8_t *data, uint8_t data_len, uint8_t *out_frame);

rt_uint8_t nrf24l01_build_sensor_frame(uint8_t cmd_type, uint8_t cmd_status,
                                       uint8_t *data, uint8_t data_len, uint8_t *out_frame);

/* 修改后的统一解析函数（新增 src 参数） */
uint8_t nrf24l01_portocol_get_command(const uint8_t *cmdBuf, const uint16_t cmdLength, cpr_src_type_t *out_src);

/* 协议处理函数（新增 src 参数） */
void nrf24l01_protocol_operation(uint8_t* CmdBuf, cpr_src_type_t src);

#endif /* APPLICATIONS_MACBSP_INC_BSP_NRF24L01_MESSAGE_H_ */
