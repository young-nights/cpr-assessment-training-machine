#include "app_message.h"




/**
  * @brief  发送按压深度数据（带校验和）
  * @param  depth_mm: 深度值，单位 mm
  * @retval None
  */
void USART1_SendDepthDir(float depth_mm, int8_t dir)
{
    uint8_t buf[7];
    uint16_t depth_01mm = (uint16_t)(depth_mm * 10.0f); // 0.1 mm 精度

    buf[0] = 0xAA;                 // 帧头
    buf[1] = 0x03;                 // 有效字节数
    buf[2] = (depth_01mm >> 8) & 0xFF;
    buf[3] = depth_01mm & 0xFF;
    buf[4] = (uint8_t)dir;         // 方向 -1/0/1

    uint8_t checksum = 0;
    for (uint8_t i = 0; i < 5; i++) checksum += buf[i];
    buf[5] = checksum;             // 校验和
    buf[6] = 0x55;                 // 帧尾

    USART1_SendData(buf, 7);
}












