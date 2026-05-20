/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-12     18452       the first version
 */
#include "bsp_sys.h"
#include <remote_nrf24l01_message.h>





uint16_t CrcCalc_Crc16Modbus(uint8_t *dat,uint8_t len)
{
    uint16_t    CRC_index = 0xffff;
    uint16_t    buffer;
    volatile    uint8_t i = 0, j = 0;
    for(i = 0; i < len; i++){
        buffer = dat[i];                            // 把数据取出来放在缓冲区
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





/****
 * @brief 构建nRF24L01的数据包
 * @param cmd_type  --> 帧类型
 *        cmd_status--> 帧状态
 *        *data     --> 数据指针
 *        data_len  --> 数据长度
 *        *out_frame--> 数据包缓冲数组指针
 * @return 总帧长
 */
rt_uint8_t nrf24l01_build_frame(uint8_t cmd_type, uint8_t cmd_status,uint8_t *data, uint8_t data_len,uint8_t *out_frame)
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



/**
 * @brief 尝试获取一条指令
 * @param command 指令存放指针
 * @return 获取的指令长度
 * @retval 0 没有获取到指令
 */
static uint8_t  Decode_Step = 0;
static uint8_t  CMD_Length = 0;
static uint8_t  CMD_buffer[30] = {0};
static uint8_t  CMD_DataCnt = 0;
static uint8_t  CRC16_H,CRC16_L = 0;
static uint16_t CRC16_Value = 0;
static uint8_t  consecutive_parse_fails = 0;

uint8_t nrf24l01_portocol_get_command(const uint8_t *cmdBuf, const uint16_t cmdLength, cpr_src_type_t *out_src)
{
    uint8_t i = 0;
    *out_src = SRC_UNKNOWN;

    if(cmdLength < CMD_MINI_LENGTH) {
        consecutive_parse_fails++;
        if(consecutive_parse_fails >= 10) {
            LOG_E("NRF24L01 protocol parse: 10 consecutive short pkt fails (timeout), resetting.");
            consecutive_parse_fails = 0;
            Decode_Step = Decode_Step_0;
        }
        return CMD_ERROR;
    }

    if(Decode_Step == Decode_Step_0)
    {
        if(*cmdBuf != 0x55) {
            consecutive_parse_fails++;
            if(consecutive_parse_fails >= 10) {
                LOG_E("NRF24L01 protocol parse: 10 consecutive 0x55 fails (timeout), resetting.");
                consecutive_parse_fails = 0;
                Decode_Step = Decode_Step_0;
            }
            return CMD_ERROR;
        }
        Decode_Step = Decode_Step_1;
    }
    if(Decode_Step == Decode_Step_1)
    {
        if(*(cmdBuf + Decode_Step_1) != 0xAA){
            Decode_Step = Decode_Step_0;
            consecutive_parse_fails++;
            if(consecutive_parse_fails >= 10) {
                LOG_E("NRF24L01 protocol parse: 10 consecutive 0xAA fails (timeout), resetting.");
                consecutive_parse_fails = 0;
                Decode_Step = Decode_Step_0;
            }
            return CMD_ERROR;
        }
        Decode_Step = Decode_Step_2;
    }
    if(Decode_Step == Decode_Step_2)
    {
        CMD_Length = *(cmdBuf + Decode_Step_2);

        // Bounds check: payload must be at least 4 bytes (ID+TYPE+STATUS),
        // and must fit within CMD_buffer (exclude CRC length from sizeof)
        if(CMD_Length < 4 || CMD_Length >= sizeof(CMD_buffer)) {
            Decode_Step = Decode_Step_0;
            return CMD_ERROR;
        }
        // Ensure remaining buffer has enough data for full frame (payload + 2 CRC)
        if(cmdLength < (uint16_t)(CMD_Length + 5)) {
            Decode_Step = Decode_Step_0;
            return CMD_ERROR;
        }

        CMD_DataCnt = 0;
        CMD_buffer[CMD_DataCnt] = CMD_Length;
        CMD_DataCnt++;
        Decode_Step = Decode_Step_3;
    }
    if(Decode_Step == Decode_Step_3)
    {
        /* Validate frame source: must be from Mainboard (DEVICE_MAINBOARD=0x0001) */
        if(*(cmdBuf + 3) != DEVICE_MAINBOARD_ID_H || *(cmdBuf + 4) != DEVICE_MAINBOARD_ID_L) {
            Decode_Step = Decode_Step_0;
            return CMD_ERROR;
        }
        *out_src = SRC_FROM_MAIN;
        CMD_buffer[CMD_DataCnt] = *(cmdBuf + Decode_Step_3);
        CMD_DataCnt++;
        Decode_Step = Decode_Step_4;
    }
    if(Decode_Step == Decode_Step_4)
    {
        CMD_buffer[CMD_DataCnt] = *(cmdBuf + Decode_Step_4);
        CMD_DataCnt++;
        Decode_Step = Decode_Step_5;
    }
    if(Decode_Step == Decode_Step_5)
    {
        for(i = 0; CMD_DataCnt < (CMD_Length + 1); CMD_DataCnt++,i++)
        {
            CMD_buffer[CMD_DataCnt] = *(cmdBuf + Decode_Step_5 + i);
        }
        CRC16_H = *(cmdBuf + Decode_Step_5 + i);
        Decode_Step = Decode_Step_6;
        i++;
    }
    if(Decode_Step == Decode_Step_6)
    {
        CRC16_L = *(cmdBuf + Decode_Step_5 + i);
        Decode_Step = Decode_Step_0;
        CRC16_Value = CrcCalc_Crc16Modbus(CMD_buffer, CMD_Length + 1);
        if(((CRC16_H << 8) | CRC16_L) == CRC16_Value)
        {
            consecutive_parse_fails = 0;
            nrf24l01_protocol_operation(CMD_buffer, *out_src);
            return CMD_TRUE;
        } else {
            consecutive_parse_fails++;
            if(consecutive_parse_fails >= 10) {
                LOG_E("NRF24L01 protocol parse: 10 CRC fails (fault/timeout), resetting state.");
                consecutive_parse_fails = 0;
                Decode_Step = Decode_Step_0;
            }
        }
    }
    return CMD_ERROR;
}



/**
 * @brief   解析数据域指令，执行响应的函数
 * @param   CmdBuf  数据域存放的指针
 * @retval  void
 */
void nrf24l01_protocol_operation(uint8_t* CmdBuf, cpr_src_type_t src)
{

    if(src != SRC_FROM_MAIN) {
        LOG_W("Remote received data not from Main!");
        return;
    }

    uint8_t cmd_type = *(CmdBuf + 3);
    uint8_t cmd      = *(CmdBuf + 5);

    LOG_I("Remote received from Main: cmd_type=0x%02X, cmd=0x%02X", cmd_type, cmd);

    switch(cmd_type)
    {
        case FRAME_TYPE_ACT:
        {
            switch(cmd)
            {
                //----------------------------------------------------------------------------------------------------
                case FRAME_NRF24_CONNECT_CTRL_PANEL_CMD:
                {
                    LOG_I("Receive: Connect succeed from Main.");
                    Record.nrf_if_connected = 1;
                }break;
                //----------------------------------------------------------------------------------------------------
                case FRAME_NRF24_MODE_DATA_IN_CMD:
                {
                    LOG_I("Receive: FRAME_NRF24_MODE_DATA_IN_CMD.");
                    Record.mode_data_in_set = 3; // 退出重发
                }break;
                //----------------------------------------------------------------------------------------------------
                case FRAME_NRF24_MODE_DATA_OUT_CMD:
                {
                    LOG_I("Receive: FRAME_NRF24_MODE_DATA_OUT_CMD.");
                    Record.mode_data_in_set = 3; // 退出重发
                }break;
                //----------------------------------------------------------------------------------------------------
                case FRAME_NRF24_PRESS_LED_CTRL_CMD:
                {
                  LOG_I("Receive: FRAME_NRF24_PRESS_LED_CTRL_CMD.");
                  Record.set_press_led = *(CmdBuf + 6);
                  Record.set_press_led_color = *(CmdBuf + 7);
                  if(nrf24l01_events != RT_NULL){
                      rt_event_send(nrf24l01_events, EVENT_NRF24_ACK_BODY_LED);
                  }
                }break;

                // --- Mainboard → Remote status sync commands -------------------------------
                case FRAME_NRF24_REMOTE_START_ACK:  // (0x21) Mainboard confirms CPR start
                {
                    LOG_I("Receive: Mainboard START ACK, status=%d", *(CmdBuf + 6));
                    Record.main_start_status = *(CmdBuf + 6);
                    if(nrf24l01_events != RT_NULL){
                        rt_event_send(nrf24l01_events, EVENT_NRF24_ACK_START_STATUS);
                    }
                }break;

                case FRAME_NRF24_REMOTE_STATUS_SYNC:  // (0x22) Mainboard syncs mode/status
                {
                    LOG_I("Receive: Mainboard STATUS SYNC, mode=%d", *(CmdBuf + 6));
                    Record.synced_mode = *(CmdBuf + 6);
                    if(nrf24l01_events != RT_NULL){
                        rt_event_send(nrf24l01_events, EVENT_NRF24_ACK_MODE_SYNC);
                    }
                }break;

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
void nrf24l01_order_to_pipe(nrf24_t nrf24, uint8_t order, uint8_t pipe_num)
{
    uint8_t emptyBuf[20] = {0};
    uint8_t frame_package[30] = { 0 };
    uint8_t package_len = 0;
    switch(order)
    {
        // 0x31指令集-----------------------------------------------------------------------------------------------------------------

        case Order_nRF24L01_ASK_Connect_Control_Panel:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_CONNECT_CTRL_PANEL_CMD;
            package_len = nrf24l01_build_frame(FRAME_TYPE_ACT,FRAME_STATE_ASK,emptyBuf,1,frame_package);
            nRF24L01_Send_Packet(nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            Record.nrf_sending = 1;
        }break;


        case Order_nRF24L01_ASK_Data_Mode_In:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_MODE_DATA_IN_CMD;
            emptyBuf[1] = Record.mode_data_in;
            package_len = nrf24l01_build_frame(FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf, 2, frame_package);
            nRF24L01_Send_Packet(nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
        }break;

        case Order_nRF24L01_ASK_Data_Mode_Out:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_MODE_DATA_OUT_CMD;
            package_len = nrf24l01_build_frame(FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf, 1, frame_package);
            nRF24L01_Send_Packet(nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
        }break;

        // --- Remote → Mainboard commands -----------------------------------------------
        case Order_nRF24L01_SEND_To_Main_Start:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_REMOTE_START_CMD;
            package_len = nrf24l01_build_frame(FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf, 1, frame_package);
            nRF24L01_Send_Packet(nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            Record.nrf_sending = 1;
        }break;

        case Order_nRF24L01_SEND_To_Main_Mode_Switch:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_REMOTE_MODE_SWITCH_CMD;
            emptyBuf[1] = Record.mode_data_in;
            package_len = nrf24l01_build_frame(FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf, 2, frame_package);
            nRF24L01_Send_Packet(nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            Record.nrf_sending = 1;
        }break;

        default: break;
    }
}


