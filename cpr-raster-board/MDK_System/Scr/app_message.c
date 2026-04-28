/**
  ******************************************************************************
  * @file    app_message.c
  * @brief   光栅板消息发送与接收处理
  *
  *          发送: 原始脉冲计数帧 (每100ms, 8字节)
  *          接收: 来自Sensor板的控制指令 (开始/停止/模式切换)
  ******************************************************************************
  */

#include "app_message.h"
#include "app_usart.h"

/* ====== 光栅板检测模式状态 ====== */
volatile raster_detect_mode_t g_raster_detect_mode = RASTER_DETECT_IDLE;

/* ====== 接收状态机变量 ====== */
static uint8_t rx_frame[5];      /* 接收帧数据缓冲 */
static uint8_t rx_len = 0;
static uint8_t rx_idx = 0;
static uint8_t rx_state = 0;     /* 0=等待帧头, 1=接收LEN, 2=接收数据, 3=接收CHK, 4=等待帧尾 */


/**
  * @brief  发送原始脉冲计数和方向数据帧
  * @param  pulse_count: 自上次发送以来的脉冲增量 (int16_t)
  * @param  dir: 方向 (-1=回弹/泄气, 0=静止, 1=下压/充气)
  * @param  type: 数据类型 (0x01=按压, 0x02=吹气)
  *
  *         帧格式: 0xAA + 0x04 + TYPE + CNT_H + CNT_L + DIR + CHK + 0x55  (8字节)
  */
void USART1_SendRealtimeData(int16_t pulse_count, int8_t dir, uint8_t type)
{
    uint8_t buf[8];
    uint8_t checksum;
    uint8_t i;

    buf[0] = 0xAA;                              /* 帧头 */
    buf[1] = 0x04;                              /* 数据域长度: TYPE+CNT_H+CNT_L+DIR = 4字节 */
    buf[2] = type;                              /* 类型: 0x01=按压, 0x02=吹气 */
    buf[3] = (uint8_t)((pulse_count >> 8) & 0xFF);  /* 脉冲计数高字节 */
    buf[4] = (uint8_t)(pulse_count & 0xFF);          /* 脉冲计数低字节 */
    buf[5] = (uint8_t)dir;                      /* 方向: -1/0/1 */

    /* 计算累加和校验 (Byte[0] ~ Byte[5]) */
    checksum = 0;
    for (i = 0; i < 6; i++)
    {
        checksum += buf[i];
    }
    buf[6] = checksum;
    buf[7] = 0x55;                              /* 帧尾 */

    USART1_SendData(buf, 8);
}


/**
  * @brief  根据模式码切换光栅板检测模式
  * @param  mode_code: CMD_ACTIVATE_PRESSURE(0x01) / CMD_IDLE_PRESSURE(0x02) / CMD_ACTIVATE_BLOW(0x03)
  */
static void switch_detect_mode(uint8_t mode_code)
{
    switch (mode_code)
    {
        case CMD_ACTIVATE_PRESSURE:
            g_raster_detect_mode = RASTER_DETECT_PRESSURE;
            depth_count_blow = 0;
            direction_blow = 0;
            break;

        case CMD_IDLE_PRESSURE:
            g_raster_detect_mode = RASTER_DETECT_IDLE;
            depth_count_press = 0;
            direction_press = 0;
            depth_count_blow = 0;
            direction_blow = 0;
            break;

        case CMD_ACTIVATE_BLOW:
            g_raster_detect_mode = RASTER_DETECT_BLOW;
            depth_count_press = 0;
            direction_press = 0;
            break;

        default:
            break;
    }
}


/**
  * @brief  接收并解析来自Sensor板的指令
  *
  *         支持指令:
  *         - 开始采集:  0xAA + 0x02 + 0x01 + 0xFF + CHK + 0x55
  *         - 停止采集:  0xAA + 0x02 + 0x03 + 0xFF + CHK + 0x55
  *         - 模式切换:  0xAA + 0x02 + 0x11 + MODE + CHK + 0x55
  *           MODE: 0x01=按压, 0x02=空闲, 0x03=吹气
  */
void USART1_ProcessRxData(void)
{
    uint8_t byte;
    uint8_t calc_chk;
    uint8_t i;

    while (1)
    {
        byte = UART1_GetByte(&USART1_QueueBuf);
        if (byte == 0) return;

        if (rx_state == 0)
        {
            if (byte == 0xAA)
            {
                rx_state = 1;
                rx_idx = 0;
            }
        }
        else if (rx_state == 1)
        {
            if (byte > 5) { rx_state = 0; continue; }
            rx_len = byte;
            rx_state = 2;
            rx_idx = 0;
        }
        else if (rx_state == 2)
        {
            rx_frame[rx_idx++] = byte;
            if (rx_idx >= rx_len)
            {
                rx_state = 3;
            }
        }
        else if (rx_state == 3)
        {
            calc_chk = 0xAA + rx_len;
            for (i = 0; i < rx_len; i++)
            {
                calc_chk += rx_frame[i];
            }

            if (calc_chk == byte)
            {
                rx_state = 4;
            }
            else
            {
                rx_state = 0;
                rx_idx = 0;
            }
        }
        else if (rx_state == 4)
        {
            if (byte == 0x55)
            {
                if (rx_frame[0] == 0x01 && rx_frame[1] == 0xFF)
                {
                    /* 开始采集指令: 清零计数器 */
                    depth_count_press = 0;
                    depth_count_blow = 0;
                    direction_press = 0;
                    direction_blow = 0;
                    raster_state = RASTER_ACTIVE;
                }
                else if (rx_frame[0] == 0x03 && rx_frame[1] == 0xFF)
                {
                    /* 停止采集指令 */
                    raster_state = RASTER_IDLE;
                }
                else if (rx_frame[0] == 0x11)
                {
                    /* 模式切换指令: CMD=0x11, DATA=模式码 */
                    switch_detect_mode(rx_frame[1]);
                }
            }
            rx_state = 0;
            rx_idx = 0;
        }
    }
}
