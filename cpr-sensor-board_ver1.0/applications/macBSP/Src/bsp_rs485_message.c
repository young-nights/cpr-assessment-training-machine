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
static uint16_t holding_regs[MAX_HOLDING_REGS] = {0};  // Initialize to 0, can be modified as needed

// Modbus exception codes
#define EXC_ILLEGAL_FUNCTION 0x01
#define EXC_ILLEGAL_ADDRESS  0x02
#define EXC_ILLEGAL_VALUE    0x03
#define EXC_SERVER_FAILURE   0x04


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

//-----------------------------------------------------------------------------------------------------

#ifndef RS485_SERIAL
#define RS485_SERIAL       "uart3"         //default test serial
#endif

#ifndef RS485_BAUDRATE
#define RS485_BAUDRATE     9600            //defalut test baudrate
#endif

#ifndef RS485_PARITY
#define RS485_PARITY       0               //defalut test parity
#endif

#ifndef RS485_PIN
#define RS485_PIN          -1              // 表示“无引脚” → 硬件自动切换
#endif

#ifndef RS485_LEVEL
#define RS485_LEVEL        -1              // 表示“无电平” → 硬件自动切换
#endif

#ifndef RS485_BUF_SIZE
#define RS485_BUF_SIZE     1024            //default test buffer size
#endif

#ifndef RS485_RECV_TMO
#define RS485_RECV_TMO     30000           //default test recicve timeout
#endif


static rs485_inst_t * bsp_rs485_hinst = RT_NULL;






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
            if (len != 8) {  // Expected: Addr(1) + Func(1) + Start(2) + Num(2) + CRC(2)
                if (need_response) send_exception_response(NULL, addr, func, EXC_ILLEGAL_VALUE);  // hinst will be passed later
                return;
            }
            uint16_t start = (frame[2] << 8) | frame[3];
            uint16_t num = (frame[4] << 8) | frame[5];

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
                // Placeholder: rs485_send(hinst, resp, resp_len);
            }
        } break;

        case 0x10:  // Write Multiple Registers
        {
            if (len < 11) {  // Min: Addr(1)+Func(1)+Start(2)+Num(2)+ByteCount(1)+Data(min2)+CRC(2)
                if (need_response) send_exception_response(NULL, addr, func, EXC_ILLEGAL_VALUE);
                return;
            }
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



    while(1)
    {
        rt_thread_mdelay(10);
    }

}




rt_thread_t rs485_decode_thread_handle;
int rs485_decode_thread_init(void)
{

    // 默认参数（可通过宏配置）
    char *serial = RS485_SERIAL;
    int baudrate = RS485_BAUDRATE;
    int parity = RS485_PARITY;
    int pin = RS485_PIN;
    int level = RS485_LEVEL;

    bsp_rs485_hinst = rs485_create(serial, baudrate, parity, pin, level);

    if (bsp_rs485_hinst != NULL)
    {
        rt_kprintf("rs485 instance create success.\n");
        rt_kprintf("rs485 serial            : %s \n", serial);
        rt_kprintf("rs485 baudrate          : %d \n", baudrate);
        rt_kprintf("rs485 parity            : %d \n", parity);
        rt_kprintf("rs485 control pin       : %d \n", pin);
        rt_kprintf("rs485 send mode level   : %d \n", level);

        // 设置默认接收超时
        rs485_set_recv_tmo(bsp_rs485_hinst, RS485_RECV_TMO);
        rt_kprintf("rs485 receive timeout   : %d \n", RS485_RECV_TMO);
    }


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








