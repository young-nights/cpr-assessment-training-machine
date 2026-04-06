/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-04-04     Administrator       the first version
 */
#ifndef APPLICATIONS_MACNRF_INC_CPR_PROTOCOL_H_

#define APPLICATIONS_MACNRF_INC_CPR_PROTOCOL_H_

#include <macSYS/Inc/mainboard_sys.h>
#include <rtthread.h>

#define CPR_FRAME_HEAD1     0x55
#define CPR_FRAME_HEAD2     0xAA
#define CPR_MAX_PAYLOAD     24

/* 设备类型 */
typedef enum {
    DEV_MAINBOARD = 0x01,
    DEV_SENSOR    = 0x02,
    DEV_REMOTE    = 0x03,
} cpr_dev_type_t;

/* 命令类型 */
typedef enum {
    CMD_ASK_CONNECT         = 0x01,   // 传感器/遥控 → 主板：连接请求
    CMD_ACK_CONNECT         = 0x02,   // 主板 → 设备：连接应答
    CMD_SENSOR_DATA         = 0x10,   // 传感器 → 主板：按压/吹气/角度数据
    CMD_REMOTE_CMD          = 0x20,   // 遥控 → 主板：模式/参数/启动等指令
    CMD_DISPLAY_FEEDBACK    = 0x30,   // 主板 → 遥控：LED状态、成绩反馈
    CMD_HEARTBEAT           = 0x40,   // 心跳
    CMD_MODE_IN             = 0x50,   // 进入训练/考核/竞赛模式
    CMD_MODE_OUT            = 0x51,   // 退出模式
    CMD_PRESS_LED_CTRL      = 0x60,   // 按压位置LED控制（遥控/主板用）
} cpr_cmd_t;

/* 通用数据包结构（支持动态payload） */
typedef struct __attribute__((packed)) {
    uint8_t  head1;           // 0x55
    uint8_t  head2;           // 0xAA
    uint8_t  len;             // 从 dev_type 开始到 crc 前的数据长度
    uint8_t  dev_type;        // 发送方设备类型
    uint8_t  cmd;             // 命令码
    uint8_t  status;          // 状态（ASK/ACK/ERR）
    uint16_t seq;             // 序列号（防重）
    uint8_t  payload[CPR_MAX_PAYLOAD];  // 动态负载
    uint16_t crc;             // CRC16-Modbus
} cpr_packet_t;

/* 传感器数据 payload 示例 */
typedef struct __attribute__((packed)) {
    uint16_t press_depth;     // 按压深度 (0-1000)
    uint16_t press_freq;      // 按压频率
    uint16_t tidal_volume;    // 吹气量
    int16_t  angle_x;         // MPU6050 角度
    int16_t  angle_y;
    uint8_t  hall_status;     // 霍尔传感器状态
    uint8_t  position;        // 按压位置 1-7
} sensor_data_payload_t;

/* 遥控指令 payload 示例 */
typedef struct __attribute__((packed)) {
    uint8_t  mode;            // 0=训练 1=考核 2=竞赛
    uint16_t work_time;       // 工作时间 (秒)
    uint8_t  pass_rate;       // 达标率
} remote_cmd_payload_t;








/* 函数声明 */
uint16_t cpr_crc16_modbus(const uint8_t *data, uint16_t len);
uint8_t  cpr_build_packet(cpr_packet_t *pkt, uint8_t dev_type, uint8_t cmd, uint8_t status,
                         const uint8_t *payload, uint8_t payload_len, uint16_t seq);
uint8_t  cpr_parse_packet(const uint8_t *buf, uint16_t buf_len, cpr_packet_t *pkt);


#endif /* APPLICATIONS_MACNRF_INC_CPR_PROTOCOL_H_ */
