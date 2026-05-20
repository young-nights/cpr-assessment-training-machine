/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-12     18452       the first version
 */
#include <sensor_nrf24l01_message.h>

#define DBG_TAG "[nRF24L01]"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>



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

/**
 * @brief 统一协议解析入口（带来源识别）
 * @note  Sensor 作为 PTX，收到的数据来自主板（DEV_MAINBOARD=0x01）
 */
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
        LOG_W("Sensor received data not from Main!");
        return;
    }

    uint8_t cmd_type = *(CmdBuf + 3);
    uint8_t cmd      = *(CmdBuf + 5);

//    rt_kprintf("\n----------------------------------------------------\n");
//    LOG_I("Sensor received from Main: cmd_type=0x%02X, cmd=0x%02X", cmd_type, cmd);

    switch(cmd_type)
    {
        case FRAME_TYPE_ACT:
        {
            switch(cmd)
            {
                case FRAME_NRF24_ASK_CONNECT_PANEL_CMD:
                {
                    rt_kprintf("Set: 接收到从主板发来的连接指令的响应.\n");
                    Record.nrf_if_connected = 1;
                }break;


                case FRAME_NRF24_ACK_START_CMD:
                {
                    rt_kprintf("Receive：接收到从主板发来的开始指令.\n");
                    Record.nrf_rec_start_cmd = 1;
                    Flag.start = *(CmdBuf + 6);
                }break;

                case FRAME_NRF24_ASK_SHOKE_SENSOR_CMD:
                {
                    rt_kprintf("Receive：意识判断指令的响应.\n");
                    Flag.shoke_ack = 0;
                }break;

                case FRAME_NRF24_ACK_WS2812B_LEVEL_CMD:
                {
                    rt_kprintf("Receive：RGB灯亮度指令.\n");
                    Record.ws2812b_levle = *(CmdBuf + 6); // 这个会在 hardware_task.c的线程中处理
                    Flag.ws2812b_ack = 1;
                }break;

                case FRAME_NRF24_ACK_MOTOR_STATUS_CMD:
                {
                    rt_kprintf("Receive：电机工作模式指令.\n");
                    Record.motor_work_sta = *(CmdBuf + 6);
                    Flag.motor_ack = 1;
                }break;

                case FRAME_NRF24_ASK_CC6201_CMD:
                {
                    rt_kprintf("Receive：磁传感器状态指令的响应.\n");
                    Flag.cc6201_ack = 0;
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
/**
 * @brief   nRF24L01向指定管道发送指令
 * @param   nrf24   nRF24L01 设备句柄
 * @param   order   指令码
 * @param   pipe_num 管道编号
 * @retval  None
 */
void nrf24l01_order_to_pipe(nrf24_t nrf24, uint8_t order, uint8_t pipe_num)
{
    uint8_t emptyBuf[20] = {0};
    uint8_t frame_package[30] = { 0 };
    uint8_t package_len = 0;
    switch(order)
    {
        case Order_nRF24L01_ASK_Connect_Control_Panel:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_ASK_CONNECT_PANEL_CMD;
            package_len = nrf24l01_build_frame(FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf, 1, frame_package);
            nRF24L01_Send_Packet(nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
        }break;

        case Order_nRF24L01_ACK_Start_Cmd:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_ACK_START_CMD;
            package_len = nrf24l01_build_frame(FRAME_TYPE_ACT, FRAME_STATE_ACK, emptyBuf, 1, frame_package);
            nRF24L01_Send_Packet(nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
        }break;

        case Order_nRF24L01_ACK_WS2812_Level_Cmd:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_ACK_WS2812B_LEVEL_CMD;
            package_len = nrf24l01_build_frame(FRAME_TYPE_ACT, FRAME_STATE_ACK, emptyBuf, 1, frame_package);
            nRF24L01_Send_Packet(nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
        }break;

        case Order_nRF24L01_ACK_Motor_Status_Cmd:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_ACK_MOTOR_STATUS_CMD;
            package_len = nrf24l01_build_frame(FRAME_TYPE_ACT, FRAME_STATE_ACK, emptyBuf, 1, frame_package);
            nRF24L01_Send_Packet(nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
        }break;

        case Order_nRF24L01_ASK_Shoke_Sensor_Cmd:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_ASK_SHOKE_SENSOR_CMD;
            package_len = nrf24l01_build_frame(FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf, 1, frame_package);
            nRF24L01_Send_Packet(nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
        }break;

        case Order_nRF24L01_ASK_CC6201_Cmd:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_ASK_CC6201_CMD;
            emptyBuf[1] = Flag.last_cc6201_state;
            package_len = nrf24l01_build_frame(FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf, 2, frame_package);
            nRF24L01_Send_Packet(nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
        }break;


        default: break;
    }
}


rt_err_t nrf24l01_send_with_retry(nrf24_t nrf24, uint8_t order, nrf24_pipe_et pipe, uint8_t max_retry)
{
    for(uint8_t r = 0; r < max_retry; r++)
    {
        LOG_I("TX order %d to pipe %d, retry %d", order, pipe, r+1);

        // ==================== 关键步骤 ====================
        if(nrf24_mutex) rt_mutex_take(nrf24_mutex, RT_WAITING_FOREVER);

        // 1. 切换到发送模式 (PTX)
        _nrf24->nrf24_ops.nrf24_reset_ce();           // 先拉低 CE
        nRF24L01_Set_Role_Mode(nrf24, ROLE_PTX);      // 切换为 PTX
        nRF24L01_Flush_TX_FIFO(nrf24);                // 清空 TX FIFO

        // 2. 填充数据并发送
        nrf24l01_order_to_pipe(nrf24, order, pipe);          // 里面会调用 nRF24L01_Send_Packet

        // 3. 拉高 CE 触发发送
        _nrf24->nrf24_ops.nrf24_set_ce();

        if(nrf24_mutex) rt_mutex_release(nrf24_mutex);
        // ================================================

        // 等待发送完成或失败
        rt_tick_t poll_start = rt_tick_get();
        rt_bool_t tx_ok = RT_FALSE;

        while(rt_tick_get() - poll_start < 80)        // 建议给 50~100ms
        {
            rt_uint8_t st = nRF24L01_Read_Status_Register(nrf24);

            if(st & NRF24BITMASK_TX_DS)               // 发送成功
            {
                LOG_I("TX OK (TX_DS set)\n");
                tx_ok = RT_TRUE;
                break;
            }

            if(st & NRF24BITMASK_MAX_RT)              // 达到最大重发次数
            {
                LOG_W("TX failed: MAX_RT");
                break;
            }

            rt_thread_mdelay(5);
        }

        // 清理中断标志
        nRF24L01_Clear_IRQ_Flags(nrf24);

        if(tx_ok)
        {
            // 发送成功后，建议切回 PRX（因为你的主线程默认是接收模式）
            if(nrf24_mutex) rt_mutex_take(nrf24_mutex, RT_WAITING_FOREVER);
            _nrf24->nrf24_ops.nrf24_reset_ce();
            nRF24L01_Set_Role_Mode(nrf24, ROLE_PRX);
            _nrf24->nrf24_ops.nrf24_set_ce();
            if(nrf24_mutex) rt_mutex_release(nrf24_mutex);

            return RT_EOK;
        }

        // 重试前稍等一下
        rt_thread_mdelay(20);
    }

    LOG_E("TX failed after %d retries", max_retry);

    // 最终失败也要切回接收模式
    if(nrf24_mutex) rt_mutex_take(nrf24_mutex, RT_WAITING_FOREVER);
    _nrf24->nrf24_ops.nrf24_reset_ce();
    nRF24L01_Set_Role_Mode(nrf24, ROLE_PRX);
    _nrf24->nrf24_ops.nrf24_set_ce();
    if(nrf24_mutex) rt_mutex_release(nrf24_mutex);

    return -RT_ETIMEOUT;
}
