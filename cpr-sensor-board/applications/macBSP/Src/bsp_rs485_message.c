/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-11-04     Administrator       the first version
 */
#include "bsp_rs485_message.h"

// Define maximum frame size
#define MAX_FRAME_SIZE 256

// Define holding registers (example: 100 registers, uint16_t)
#define MAX_HOLDING_REGS 100
// Initialize to 0, can be modified as needed
static uint16_t holding_regs[MAX_HOLDING_REGS] = {0};

// Modbus exception codes
#define EXC_ILLEGAL_FUNCTION 0x01   // 非法功能码
#define EXC_ILLEGAL_ADDRESS  0x02   // 非法数据地址
#define EXC_ILLEGAL_VALUE    0x03   // 非法数据值
#define EXC_SERVER_FAILURE   0x04   // 从机服务器设备故障


rs485_inst_t *rs485_hinst;
//-----------------------------------------------------------------------------------------------------

static uint16_t crc16_modbus(uint8_t *dat,uint8_t len)
{
    uint16_t    CRC_index = 0xffff;
    uint16_t    buffer;
    volatile    uint8_t i = 0, j = 0;
    for(i = 0; i < len; i++){
        buffer = dat[i];
        CRC_index ^= buffer;
        for(j = 0; j < 8; j++){
            if(CRC_index & 0x0001){
                CRC_index >>= 1;
                CRC_index ^= 0xa001;
            }else{
                CRC_index >>= 1;
            }
        }
    }
    return CRC_index;
}



static void send_exception_response(rs485_inst_t *hinst, uint8_t addr, uint8_t func, uint8_t exc_code)
{
    uint8_t resp[5];        // 异常响应帧缓冲区：5字节固定长度
    resp[0] = addr;         // 从机地址
    resp[1] = func | 0x80;  // 功能码最高位置1，表示异常响应
    resp[2] = exc_code;     // 具体的异常码

    // 计算前3个字节的 Modbus CRC16
    uint16_t crc = crc16_modbus(resp, 3);

    // CRC 低字节在前，高字节在后（Modbus RTU 规范）
    resp[3] = crc & 0xFF;
    resp[4] = (crc >> 8) & 0xFF;

    // 通过 RS485 发送完整的异常响应帧
    rs485_send(hinst, resp, 5);
}



void my_protocol_decode(uint8_t *frame, int len, int port_idx)
{
    if (len < 4) return;

    uint8_t addr = frame[0];
    uint8_t func = frame[1];
    uint16_t crc = frame[len - 2] | (frame[len - 1] << 8);

    if (crc16_modbus(frame, len - 2) != crc) {
        rt_kprintf("crc16 error!\n");
        return;
    }

    // Address filter: slave addr or broadcast (0x00)
    if (addr != RS485_SLAVE_ADDR && addr != 0x00) return;

    // Broadcast (0x00) does not require response for writes, but for reads it might (depending on protocol, here we skip response for broadcast)
    rt_bool_t need_response = (addr != 0x00);

    switch (func)
    {
        case MODBUS_READ_HOLDING:  // Read Holding Registers
        {
            // 计算寄存器的起始地址
            uint16_t start = (frame[2] << 8) | frame[3];
            // 计算要读取的连续寄存器数量
            uint16_t num = (frame[4] << 8) | frame[5];

            // 不允许读取的寄存器数为0，并且起始地址加上操作的寄存器数量不能超过实际定义的寄存器数组大小
            if (num == 0 || start + num > MAX_HOLDING_REGS) {
                if (need_response) send_exception_response(NULL, addr, func, EXC_ILLEGAL_ADDRESS);
                return;
            }

            // Build response
            uint8_t resp[MAX_FRAME_SIZE];
            resp[0] = addr;
            resp[1] = func;
            resp[2] = num * 2;  // Byte count
            for (uint16_t i = 0; i < num; i++) {
                uint16_t val = holding_regs[start + i];
                resp[3 + i * 2] = (val >> 8) & 0xFF;
                resp[3 + i * 2 + 1] = val & 0xFF;
            }
            uint16_t resp_len = 3 + num * 2;
            uint16_t crc_resp = crc16_modbus(resp, resp_len);
            resp[resp_len] = crc_resp & 0xFF;
            resp[resp_len + 1] = (crc_resp >> 8) & 0xFF;
            resp_len += 2;

            if (need_response) {
                // Send response (hinst needs to be available, will be in thread context)
//                rs485_send(bsp_rs485_hinst, resp, resp_len);
            }
        } break;

        case MODBUS_WRITE_MULTIPLE:  // Write Multiple Registers
        {
            uint16_t start = (frame[2] << 8) | frame[3];
            uint16_t num = (frame[4] << 8) | frame[5];
            uint8_t byte_count = frame[6];

            if (byte_count != num * 2 || start + num > MAX_HOLDING_REGS || len != 7 + byte_count + 2) {
                if (need_response) send_exception_response(NULL, addr, func, EXC_ILLEGAL_ADDRESS);
                return;
            }

            // Write to registers
            for (uint16_t i = 0; i < num; i++) {
                holding_regs[start + i] = (frame[7 + i * 2] << 8) | frame[7 + i * 2 + 1];
            }

            // Build response
            uint8_t resp[8];
            resp[0] = addr;
            resp[1] = func;
            resp[2] = frame[2];
            resp[3] = frame[3];
            resp[4] = frame[4];
            resp[5] = frame[5];
            uint16_t crc = crc16_modbus(resp, 6);
            resp[6] = crc & 0xFF;
            resp[7] = (crc >> 8) & 0xFF;

            if (need_response) {
                // Send response
                // rs485_send(hinst, resp, 8);
            }
        } break;

        default:
            if (need_response) send_exception_response(NULL, addr, func, EXC_ILLEGAL_FUNCTION);
            break;
    }
}






void rs485_decode_thread_entry(void *paragram)
{

    rt_device_t rs485_dev = rt_device_find("rs485-1");
    if (rs485_dev == RT_NULL) {
        rt_kprintf("RS485 device not found!\n");
    }
    if (rt_device_open(rs485_dev, RT_DEVICE_FLAG_RDWR) != RT_EOK) {
        rt_kprintf("RS485 open failed!\n");
    }
    rs485_dev_t *pdev = (rs485_dev_t *)rs485_dev;
    rs485_hinst = pdev->hinst;
    rs485_dev_tmo_param_t tmo = { .ack_tmo_ms = 500, .byte_tmo_ms = 10 };
    rt_device_control(rs485_dev, RS485_CTRL_SET_TMO, &tmo);

    while(1)
    {
        rs485_send(rs485_hinst, "OK\n", 3);
        rt_thread_mdelay(500);
    }

}




rt_thread_t rs485_decode_thread_handle;
int rs485_decode_thread_init(void)
{
    // 接收解码线程------------------------------------------------------------------------------------------
    rs485_decode_thread_handle = rt_thread_create(" rs485_decode_thread_entry",
                                                    rs485_decode_thread_entry,
                                                    RT_NULL, 1024, 10, 200);
    if(rs485_decode_thread_handle != RT_NULL){
        rt_kprintf("PRINTF:%d. rs485 decode thread is created!!\r\n",Record.kprintf_cnt++);
        rt_thread_startup(rs485_decode_thread_handle);
    }
    else {
        rt_kprintf("PRINTF:%d. rs485 decode is not created!!\r\n",Record.kprintf_cnt++);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(rs485_decode_thread_init);



