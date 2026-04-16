/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-12     18452       the first version
 */
#include <mainboard_nrf24l01_message.h>

uint16_t CrcCalc_Crc16Modbus(uint8_t *dat,uint8_t len)
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





rt_uint8_t nrf24l01_build_remote_frame(uint8_t cmd_type, uint8_t cmd_status,uint8_t *data, uint8_t data_len,uint8_t *out_frame)
{
    rt_uint8_t  index = 0;
    rt_uint16_t crc = 0;

    out_frame[index++] = 0x55;
    out_frame[index++] = 0xAA;
    out_frame[index++] = 4 + data_len; // 长度 = ID(2) + cmd_type + cmd_status + data
    out_frame[index++] = DEVICE_REMOTE_ID_H;
    out_frame[index++] = DEVICE_REMOTE_ID_L;
    out_frame[index++] = cmd_type;
    out_frame[index++] = cmd_status;

    for(uint8_t i = 0; i < data_len; i++){
        out_frame[index++] = data[i];
    }

    crc = CrcCalc_Crc16Modbus(&out_frame[2], index - 2);
    out_frame[index++] = (crc >> 8) & 0xFF;
    out_frame[index++] = crc & 0xFF;

    return index; // 返回总帧长
}



rt_uint8_t nrf24l01_build_sensor_frame(uint8_t cmd_type, uint8_t cmd_status,uint8_t *data, uint8_t data_len,uint8_t *out_frame)
{
    rt_uint8_t  index = 0;
    rt_uint16_t crc = 0;

    out_frame[index++] = 0x55;
    out_frame[index++] = 0xAA;
    out_frame[index++] = 4 + data_len; // 长度 = ID(2) + cmd_type + cmd_status + data
    out_frame[index++] = DEVICE_SENSOR_ID_H;
    out_frame[index++] = DEVICE_SENSOR_ID_L;
    out_frame[index++] = cmd_type;
    out_frame[index++] = cmd_status;

    for(uint8_t i = 0; i < data_len; i++){
        out_frame[index++] = data[i];
    }

    crc = CrcCalc_Crc16Modbus(&out_frame[2], index - 2);
    out_frame[index++] = (crc >> 8) & 0xFF;
    out_frame[index++] = crc & 0xFF;

    return index; // 返回总帧长
}





static uint8_t  Decode_Step = 0;
static uint8_t  CMD_Length = 0;
static uint8_t  CMD_buffer[30] = {0};
static uint8_t  CMD_DataCnt = 0;
static uint8_t  CRC16_H,CRC16_L = 0;
static uint16_t CRC16_Value = 0;
uint8_t nrf24l01_portocol_get_command(const uint8_t *cmdBuf,const uint16_t cmdLength, cpr_src_type_t *out_src)
{

    uint8_t i = 0;
    *out_src = SRC_UNKNOWN;

    /* 如果未处理的数据长度小于指令长度 则不可能有完整的指令 */
    if(cmdLength < CMD_MINI_LENGTH){
        return CMD_ERROR;
    }

    /* 然后可以进行正常的数据解析流程 */
    /*--------------------------------*/
    /*****************                第一步数据解析               ****************************/
    if(Decode_Step == Decode_Step_0)
    {
        if(*cmdBuf != 0x55)return CMD_ERROR;
        Decode_Step = Decode_Step_1;
    }
    /*****************                第二步数据解析               ****************************/
    if(Decode_Step == Decode_Step_1)
    {
        if(*(cmdBuf + Decode_Step_1) != 0xAA){
            Decode_Step = Decode_Step_0;
            return CMD_ERROR;
        }
        Decode_Step = Decode_Step_2;
    }
    /*****************                第三步数据解析               ****************************/
    if(Decode_Step == Decode_Step_2)
    {
        CMD_Length = *(cmdBuf + Decode_Step_2);
        CMD_DataCnt = 0;
        CMD_buffer[CMD_DataCnt] = CMD_Length;
        CMD_DataCnt++;
        Decode_Step = Decode_Step_3;
    }
    /*****************                第四步数据解析               ****************************/
    if(Decode_Step == Decode_Step_3)
    {
        if(*(cmdBuf + 3) == DEVICE_REMOTE_ID_H && *(cmdBuf + 4) == DEVICE_REMOTE_ID_L) {
            *out_src = SRC_FROM_REMOTE;
            CMD_buffer[CMD_DataCnt++] = *(cmdBuf + 3);
            Decode_Step = Decode_Step_4;
        }
        else if(*(cmdBuf + 3) == DEVICE_SENSOR_ID_H && *(cmdBuf + 4) == DEVICE_SENSOR_ID_L) {
            *out_src = SRC_FROM_SENSOR;
            CMD_buffer[CMD_DataCnt++] = *(cmdBuf + 3);
            Decode_Step = Decode_Step_4;
        }
        else {
            Decode_Step = Decode_Step_0;
            return CMD_ERROR;
        }
    }
    /*****************                第五步数据解析               ****************************/
    if(Decode_Step == Decode_Step_4)
    {
        CMD_buffer[CMD_DataCnt++] = *(cmdBuf + 4);
        Decode_Step = Decode_Step_5;
    }
    /*****************                第六步数据解析               ****************************/
    if(Decode_Step == Decode_Step_5)
    {
        /* 接收数据 */
        for(i = 0; CMD_DataCnt < (CMD_Length + 1); CMD_DataCnt++,i++)
        {
            CMD_buffer[CMD_DataCnt] = *(cmdBuf + Decode_Step_5 + i);
        }

        CRC16_H = *(cmdBuf + Decode_Step_5 + i);
        Decode_Step = Decode_Step_6;
        i++;
    }
    /*****************                第七步数据解析               ****************************/
    if(Decode_Step == Decode_Step_6)
    {
        CRC16_L = *(cmdBuf + Decode_Step_5 + i);
        Decode_Step = Decode_Step_0;
        CRC16_Value = CrcCalc_Crc16Modbus(CMD_buffer, CMD_Length + 1);
        if(((CRC16_H << 8) | CRC16_L) == CRC16_Value)
        {
            /* ====================== 解析成功，打印完整帧 ====================== */
            LOG_I("=== [nRF24 Protocol] Parse SUCCESS ===");
            LOG_I("Source: %s", (*out_src == SRC_FROM_SENSOR) ? "SENSOR" :
                                (*out_src == SRC_FROM_REMOTE) ? "REMOTE" : "UNKNOWN");
            LOG_I("Length: %d bytes", CMD_Length + 2 + 2);  // 头2 + ID2 + Type+Status + Data + CRC2
            LOG_I("Frame : 55 AA %02X %02X %02X %02X %02X ... (CRC %02X %02X)",
                  CMD_Length,
                  CMD_buffer[1], CMD_buffer[2], CMD_buffer[3], CMD_buffer[4],
                  CRC16_H, CRC16_L);

            /* 可选：打印整个接收到的原始数据（方便调试） */
            rt_kprintf("Raw Data: ");
            for(uint16_t j = 0; j < cmdLength; j++) {
                rt_kprintf("%02X ", cmdBuf[j]);
            }
            rt_kprintf("\n");

            /* 调用协议处理函数 */
            nrf24l01_protocol_operation(CMD_buffer, *out_src);

            return CMD_TRUE;
        }
        else
        {
            LOG_W("CRC Check FAILED! Calculated=0x%04X, Received=0x%04X",
                  CRC16_Value, (CRC16_H << 8) | CRC16_L);
        }
    }
}





void nrf24l01_protocol_operation(uint8_t* CmdBuf, cpr_src_type_t src)
{
    uint8_t cmd_type = *(CmdBuf + 3);
    uint8_t cmd      = *(CmdBuf + 5);

    LOG_I   ("Receive from %s, cmd_type=0x%02X, cmd=0x%02X",
            (src == SRC_FROM_SENSOR) ? "Sensor" :
            (src == SRC_FROM_REMOTE) ? "Remote" : "Unknown",
            cmd_type, cmd);

    /*以 06 00 61 31 02 01 01 数据域指令为例*/
    /*长度 + 设备ID_H + 设备ID_L + 指令类型 + 指令状态 + 实际指令宏 + 指令数据 */
    switch(cmd_type)
    {
        case FRAME_TYPE_ACT:
        {
            switch(cmd)
            {
                //----------------------------------------------------------------------------------------------------
                case FRAME_NRF24_CONNECT_CTRL_PANEL_CMD:
                {
                    LOG_I("Receive Connect request from %s",
                          (src == SRC_FROM_SENSOR) ? "Sensor" : "Remote");

                    if(src == SRC_FROM_SENSOR) {
                        Record.sensor_connect_pending = 1;
                        Record.sensor_connected = 1;
                        Record.last_sensor_heartbeat = rt_tick_get();
                        LOG_I("Sensor connected (mainboard confirmed).");
                    } else if(src == SRC_FROM_REMOTE) {
                        Record.remote_connect_pending = 1;
                        Record.remote_connected = 1;
                        Record.last_remote_heartbeat = rt_tick_get();
                        LOG_I("Remote connected (mainboard confirmed).");
                    }
                }break;
                //----------------------------------------------------------------------------------------------------
//                // 接收来自 remote 的 模式控制指令
//                case FRAME_NRF24_MODE_DATA_IN_CMD:
//                {
//                    LOG_I("Receive: Mode Data In.");
//                    Record.mode_data_in = *(CmdBuf + 6);
//                    if(nrf24l01_events != RT_NULL){
//                        rt_event_send(nrf24l01_events, EVENT_NRF24_ACK_MODE_DATA_IN);
//                    }
//                }break;
//                //----------------------------------------------------------------------------------------------------
//                case FRAME_NRF24_MODE_DATA_OUT_CMD:
//                {
//                    LOG_I("Receive: Mode Data Out.");
//                    Record.mode_data_in = 0;
//                    if(nrf24l01_events != RT_NULL){
//                        rt_event_send(nrf24l01_events, EVENT_NRF24_ACK_MODE_DATA_OUT);
//                    }
//
//                }break;


                default:    break;
            }
        }break;


        default:    break;
    }
}







/**
 * @brief   nRF24L01向指定管道发送指令
 * @param   order   指令码
 * @retval  None
 */
void nrf24l01_order_to_pipe(uint8_t order, nrf24_pipe_et pipe_num)
{
    uint8_t emptyBuf[20] = {0};
    uint8_t frame_package[30] = { 0 };
    uint8_t package_len = 0;
    switch(order)
    {
        /* 回复连接请求：Pipe1 → Sensor (0x05), Pipe2 → Remote (0x04) */
        case Order_nRF24L01_ACK_Connect_Control_Panel:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_CONNECT_CTRL_PANEL_CMD;
            if(pipe_num == NRF24_PIPE_1){
                package_len = nrf24l01_build_sensor_frame(FRAME_TYPE_ACT, FRAME_STATE_ACK, emptyBuf, 1, frame_package);
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NEED_ACK);
            }
            else if(pipe_num == NRF24_PIPE_2){
                package_len = nrf24l01_build_remote_frame(FRAME_TYPE_ACT, FRAME_STATE_ACK, emptyBuf, 1, frame_package);
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NEED_ACK);
            }
        }break;


        //  响应进入数据模式设置指令：55 AA 05 00 04 31 02 90 3D
        case Order_nRF24L01_ACK_Mode_Data_In:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_MODE_DATA_IN_CMD;
            package_len = nrf24l01_build_remote_frame(FRAME_TYPE_ACT,FRAME_STATE_ACK,emptyBuf,1,frame_package);
            nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NEED_ACK);
        }break;


        //  响应进入数据模式设置指令：55 AA 05 00 04 31 03 50 FC
        case Order_nRF24L01_ACK_Mode_Data_Out:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_MODE_DATA_OUT_CMD;
            package_len = nrf24l01_build_remote_frame(FRAME_TYPE_ACT,FRAME_STATE_ACK,emptyBuf,1,frame_package);
            nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NEED_ACK);
        }break;


        default: break;
    }
}


