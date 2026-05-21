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


#define DBG_TAG "nRF24"
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
        rt_kprintf("ERROR\n");
        return CMD_ERROR;
    }

    /* 然后可以进行正常的数据解析流程 */
    /*--------------------------------*/
    /*****************                第一步数据解析               ****************************/
    if(Decode_Step == Decode_Step_0)
    {
        if(*cmdBuf != 0x55){
            rt_kprintf("cmdBuf != 0x55 \n");
            return CMD_ERROR;
        }
        Decode_Step = Decode_Step_1;
    }
    /*****************                第二步数据解析               ****************************/
    if(Decode_Step == Decode_Step_1)
    {
        if(*(cmdBuf + Decode_Step_1) != 0xAA){
            rt_kprintf("*(cmdBuf + Decode_Step_1) != 0xAA \n");
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
            rt_kprintf("Device ID Error\n");
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




extern char send_step_nums;
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

                case FRAME_NRF24_SEND_TO_SENSOR_START_CMD:
                {
                    rt_kprintf("Receive: 接收到sensor的开始指令的响应 \n");
                    Record.sensor_start_cmd_ack = 1;
                    send_step_nums = 1;
                }break;

                case FRAME_NRF24_ACK_SHOKE_SENSOR_CMD:
                {
                    rt_kprintf("Receive: 接收到传感器板的压电陶瓷片触发指令 \n");
                    Record.shoke_cmd_ack = 1; // 此时可以执行回应
                }break;

                case FRAME_NRF24_ASK_WS2812B_LEVEL_CMD:
                {
                    rt_kprintf("Receive：接收到传感器板的控制WS2812B亮度的指令响应\n");
                    Record.sensor_wsrgb_cmd_ack = 1;
                    send_step_nums = 2;
                }break;

                case FRAME_NRF24_ASK_MOTOR_STATUS_CMD:
                {
                    rt_kprintf("Receive：接收到传感器板的控制电机工作模式的指令响应\n");
                    Record.sensor_motor_cmd_ack = 1;
                    send_step_nums = 3;
                }break;

                case FRAME_NRF24_ACK_CC6201_CMD:
                {
                    rt_kprintf("Receive：接收到传感器板的磁传感器的指令响应\n");
                    MySysCfg.cc6201_state = *(CmdBuf + 6);
                    Record.cc6201_cmd_ack = 1;
                }break;

                // --- Remote operation commands -------------------------------------------------
                case FRAME_NRF24_REMOTE_START_CMD:  // (0x11) Remote triggers CPR start
                {
                    LOG_I("Receive START command from %s",
                          (src == SRC_FROM_REMOTE) ? "Remote" : "Unknown");
                    if(src == SRC_FROM_REMOTE) {
                        MySysCfg.start_status = 1;
                        MySysCfg.start_press_cnt = 1;
                        MySysCfg.reset_press_cnt = 0;
                        MySysCfg.setting_mode = 0;

                        WT588D_Set_Cmd(WT588D_ADDR_VOICE_2);
                        LED_On(LED_Name_Start);
                        LED_Off(LED_Name_Reset);
                        LED_Off(LED_Name_Setting);

                        Record.sensor_start_cmd_ack = 0;
                        Record.sensor_wsrgb_cmd_ack = 0;
                        Record.sensor_motor_cmd_ack = 0;
                        send_step_nums = 0;

                        LOG_I("CPR started from Remote.");
                    }
                }break;

                case FRAME_NRF24_REMOTE_MODE_SWITCH_CMD:  // (0x12) Remote switches mode
                {
                    uint8_t mode = *(CmdBuf + 6);
                    LOG_I("Receive MODE SWITCH from %s: mode=%d",
                          (src == SRC_FROM_REMOTE) ? "Remote" : "Unknown", mode);
                    if(src == SRC_FROM_REMOTE && mode < MODE_MAX) {
                        MySysCfg.current_mode = (System_Mode_t)mode;
                        if(mode == MODE_TRAIN) {
                            LED_On(LED_Name_Train);
                            LED_Off(LED_Name_Assess);
                            LED_Off(LED_Name_Competition);
                            WT588D_Set_Cmd(WT588D_ADDR_VOICE_5);
                        } else if(mode == MODE_ASSESS) {
                            LED_Off(LED_Name_Train);
                            LED_On(LED_Name_Assess);
                            LED_Off(LED_Name_Competition);
                            WT588D_Set_Cmd(WT588D_ADDR_VOICE_6);
                        } else if(mode == MODE_COMPETE) {
                            LED_Off(LED_Name_Train);
                            LED_Off(LED_Name_Assess);
                            LED_On(LED_Name_Competition);
                            WT588D_Set_Cmd(WT588D_ADDR_VOICE_7);
                        }
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
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            }
            else if(pipe_num == NRF24_PIPE_2){
                package_len = nrf24l01_build_remote_frame(FRAME_TYPE_ACT, FRAME_STATE_ACK, emptyBuf, 1, frame_package);
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            }
        }break;

        /* 发送开始指示 */
        case Order_nRF24L01_SEND_To_Sensor_Start:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_SEND_TO_SENSOR_START_CMD;
            emptyBuf[1] = MySysCfg.start_status;
            if(pipe_num == NRF24_PIPE_1){
                package_len = nrf24l01_build_sensor_frame(FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf, 2, frame_package);
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            }
            else if(pipe_num == NRF24_PIPE_2){
                package_len = nrf24l01_build_remote_frame(FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf, 2, frame_package);
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            }
        }break;

        /* 回应shoke_cmd */
        case Order_nRF24L01_ACK_Shoke_Sensor_Cmd:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_ACK_SHOKE_SENSOR_CMD;
            if(pipe_num == NRF24_PIPE_1){
                package_len = nrf24l01_build_sensor_frame(FRAME_TYPE_ACT, FRAME_STATE_ACK, emptyBuf, 1, frame_package);
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            }
            else if(pipe_num == NRF24_PIPE_2){
                package_len = nrf24l01_build_remote_frame(FRAME_TYPE_ACT, FRAME_STATE_ACK, emptyBuf, 1, frame_package);
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            }
        }break;


        /* 回应cc6201_cmd */
        case Order_nRF24L01_ACK_CC6201_State_Cmd:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_ACK_CC6201_CMD;
            if(pipe_num == NRF24_PIPE_1){
                package_len = nrf24l01_build_sensor_frame(FRAME_TYPE_ACT, FRAME_STATE_ACK, emptyBuf, 1, frame_package);
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            }
            else if(pipe_num == NRF24_PIPE_2){
                package_len = nrf24l01_build_remote_frame(FRAME_TYPE_ACT, FRAME_STATE_ACK, emptyBuf, 1, frame_package);
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            }
        }break;


        /* WS2812B的亮度等级 */
        case Order_nRF24L01_SEND_To_Sensor_WS2812_Level:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_ASK_WS2812B_LEVEL_CMD;
            emptyBuf[1] = MySysCfg.eyes_rgb_level;
            if(pipe_num == NRF24_PIPE_1){
                package_len = nrf24l01_build_sensor_frame(FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf, 2, frame_package);
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            }
            else if(pipe_num == NRF24_PIPE_2){
                package_len = nrf24l01_build_remote_frame(FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf, 2, frame_package);
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            }
        }break;


        /* 空心杯电机控制 */
        case Order_nRF24L01_SEND_To_Sensor_Motor_Status:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_ASK_MOTOR_STATUS_CMD;
            emptyBuf[1] = MySysCfg.motor_work_sta;
            if(pipe_num == NRF24_PIPE_1){
                package_len = nrf24l01_build_sensor_frame(FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf, 2, frame_package);
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            }
            else if(pipe_num == NRF24_PIPE_2){
                package_len = nrf24l01_build_remote_frame(FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf, 2, frame_package);
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            }
        }break;

        /* Send start status confirmation to Remote (Pipe2 only) */
        case Order_nRF24L01_SEND_To_Remote_Start_Status:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_REMOTE_START_ACK;
            emptyBuf[1] = MySysCfg.start_status;
            if(pipe_num == NRF24_PIPE_2){
                package_len = nrf24l01_build_remote_frame(FRAME_TYPE_ACT, FRAME_STATE_ACK, emptyBuf, 2, frame_package);
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            }
        }break;

        /* Sync mode info to Remote (Pipe2 only) */
        case Order_nRF24L01_SEND_To_Remote_Mode_Sync:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_NRF24_REMOTE_STATUS_SYNC;
            emptyBuf[1] = (uint8_t)MySysCfg.current_mode;
            if(pipe_num == NRF24_PIPE_2){
                package_len = nrf24l01_build_remote_frame(FRAME_TYPE_ACT, FRAME_STATE_ACK, emptyBuf, 2, frame_package);
                nRF24L01_Send_Packet(_nrf24, frame_package, package_len, pipe_num, nRF24_SEND_NO_ACK);
            }
        }break;

        default: break;
    }
}




rt_err_t nrf24l01_send_with_retry(nrf24_t nrf24, uint8_t order, nrf24_pipe_et pipe, uint8_t max_retry)
{
    /* Take mutex once for the entire TX sequence to prevent the main
       thread from reading/modifying STATUS registers during PTX mode */
    if(nrf24_mutex) rt_mutex_take(nrf24_mutex, RT_WAITING_FOREVER);

    rt_err_t result = -RT_ETIMEOUT;

    for(uint8_t r = 0; r < max_retry; r++)
    {
        LOG_I("TX order %d to pipe %d, retry %d", order, pipe, r+1);

        // 1. Switch to PTX mode
        _nrf24->nrf24_ops.nrf24_reset_ce();
        nRF24L01_Set_Role_Mode(nrf24, ROLE_PTX);
        nRF24L01_Flush_TX_FIFO(nrf24);

        // 2. Fill and send
        nrf24l01_order_to_pipe(order, pipe);

        // 3. Pulse CE to trigger transmission
        _nrf24->nrf24_ops.nrf24_set_ce();

        // 4. Poll for TX completion or failure (mutex held)
        rt_tick_t poll_start = rt_tick_get();
        rt_bool_t tx_ok = RT_FALSE;

        while(rt_tick_get() - poll_start < 80)
        {
            rt_uint8_t st = nRF24L01_Read_Status_Register(nrf24);

            if(st & NRF24BITMASK_TX_DS)
            {
                LOG_I("TX OK (TX_DS set)");
                tx_ok = RT_TRUE;
                break;
            }

            if(st & NRF24BITMASK_MAX_RT)
            {
                LOG_W("TX failed: MAX_RT");
                break;
            }

            rt_thread_mdelay(5);
        }

        // 5. Clear IRQ flags
        nRF24L01_Clear_IRQ_Flags(nrf24);

        if(tx_ok)
        {
            result = RT_EOK;
            break;
        }

        // Wait before retry
        rt_thread_mdelay(20);
    }

    /* Always restore PRX mode, flush RX FIFO, and clear IRQ flags */
    _nrf24->nrf24_ops.nrf24_reset_ce();
    nRF24L01_Set_Role_Mode(nrf24, ROLE_PRX);
    nRF24L01_Flush_RX_FIFO(nrf24);
    nRF24L01_Clear_IRQ_Flags(nrf24);
    _nrf24->nrf24_ops.nrf24_set_ce();

    /* Release mutex on all exit paths */
    if(nrf24_mutex) rt_mutex_release(nrf24_mutex);

    if(result != RT_EOK) {
        LOG_E("TX failed after %d retries", max_retry);
    }

    return result;
}

