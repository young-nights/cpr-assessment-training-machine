/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-12-05     18452       the first version
 * 2026-04-25     coder       完善光栅板通信协议，添加帧解析与指令发送
 * 2026-04-27     coder       适配光栅板纯采集架构：接收脉冲增量帧，删除成绩帧处理
 * 2026-04-29     coder       添加模式切换指令和联合判别函数声明
 */
#ifndef APPLICATIONS_MACBSP_INC_UART2_PROTOCOL_H_
#define APPLICATIONS_MACBSP_INC_UART2_PROTOCOL_H_

#include "bsp_sys.h"


#ifdef BSP_USING_UART2
#define USART2_SEND_CMD_INFO_PRINTF     1       /* 串口2发送指令信息打印 */
#define USART2_REC_CMD_PRINTF           1       /* 串口2接收指令信息打印 */
#endif


/* ====== Sensor → 光栅板 模式切换指令码 ====== */
#define CMD_ACTIVATE_PRESSURE   0x01    /* 进入按压检测模式 */
#define CMD_IDLE_PRESSURE       0x02    /* 退出按压检测模式 */
#define CMD_ACTIVATE_BLOW       0x03    /* 进入吹气检测模式 */


extern rt_device_t  serial2;

/* 创建消息队列结构体参数 */
typedef struct{
    rt_device_t device_t;
    rt_size_t   size;
}MessageQueue;


/* 创建串口接收与发送缓冲区结构体参数 */
#define MAX_DATA_LENGTH 512
typedef struct{
    uint8_t rxBuffer[MAX_DATA_LENGTH];  /* 循环队列缓冲区 */
    volatile rt_uint16_t rx_index;      /* 数据索引 */
    volatile rt_uint16_t head;          /* 队列头指针（读位置） */
    volatile rt_uint16_t tail;          /* 队列尾指针（写位置） */
    rt_mutex_t lock;                    /* 互斥锁 */
}xUsart_Structure;


/* ====== 光栅板原始数据（成绩结构体定义移至 app_calculator.h） ====== */
extern volatile int32_t  g_raster_press_cumulative;   /* 按压脉冲累计值 */
extern volatile int32_t  g_raster_blow_cumulative;    /* 吹气脉冲累计值 */
extern volatile int8_t   g_raster_press_dir;          /* 按压方向 */
extern volatile int8_t   g_raster_blow_dir;           /* 吹气方向 */
extern volatile uint16_t g_raster_press_depth_01mm;   /* 按压深度 (0.1mm) */
extern volatile uint16_t g_raster_blow_depth_01mm;    /* 吹气深度 (0.1mm) */


/* ====== 函数声明 ====== */

/**
  * @brief  初始化UART2及解码线程
  */
int uart2_decodeThread_Init(void);

/**
  * @brief  UART2初始化
  */
int USART2_Init(void);

/**
  * @brief  发送开始指令到光栅板
  *         帧格式: 0xAA + 0x02 + 0x01 + 0xFF + CHK + 0x55 (6字节)
  */
void USART2_Send_Start_To_Raster(void);

/**
  * @brief  发送停止指令到光栅板
  *         帧格式: 0xAA + 0x02 + 0x03 + 0xFF + CHK + 0x55 (6字节)
  */
void USART2_Send_Stop_To_Raster(void);


/* ====== 模式切换指令发送 ====== */

/**
  * @brief  发送进入按压模式指令到光栅板
  *         帧格式: 0xAA + 0x02 + 0x11 + 0x01 + CHK + 0x55 (6字节)
  */
void send_activate_pressure_cmd(void);

/**
  * @brief  发送退出按压模式指令到光栅板
  *         帧格式: 0xAA + 0x02 + 0x11 + 0x02 + CHK + 0x55 (6字节)
  */
void send_idle_pressure_cmd(void);

/**
  * @brief  发送进入吹气模式指令到光栅板
  *         帧格式: 0xAA + 0x02 + 0x11 + 0x03 + CHK + 0x55 (6字节)
  */
void send_activate_blow_cmd(void);


/* ====== 三路信号联合判别 ====== */

/**
  * @brief  三路信号联合判别函数
  *         读取 ADC128S 压力值 + MPU6050 pitch 上仰标志，
  *         根据判别组合表决定发送哪种模式切换指令
  *
  *         判别规则：
  *         - 压力超阈值 → 按压模式（优先级最高）
  *         - pitch 上仰确认 → 吹气模式
  *         - 两者同时触发 → 按压优先
  *         - 都未触发 → 退出按压模式（idle）
  *
  * @param  pressure_raw: ADC128S 压力通道原始值
  */
void joint_discrimination(uint16_t pressure_raw);


#endif /* APPLICATIONS_MACBSP_INC_UART2_PROTOCOL_H_ */
