/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-12-05     18452       the first version
 * 2026-04-25     coder       完善光栅板通信协议解析与指令发送
 * 2026-04-27     coder       适配光栅板纯采集架构：接收脉冲增量帧，删除成绩帧处理
 * 2026-04-29     coder       添加模式切换指令发送和三路信号联合判别
 */

#include "uart2_protocol.h"
#include "bsp_mpu6050_euler_angles.h"
#include "string.h"

/* 串口2连接光栅板，用于接收按压/吹气脉冲增量数据 */

/* ADC128S 压力通道读取函数（声明在 adc128s102cimtx.c 中） */
extern rt_err_t adc128s102_read_raw(adc128s_channel_et ch, rt_uint16_t *value);


/* ====== UART2 配置 ====== */
#define RT_SERIAL_CONFIG_USART2            \
{                                          \
    BAUD_RATE_115200, /* 115200 bits/s */  \
    DATA_BITS_8,      /* 8 databits */     \
    STOP_BITS_1,      /* 1 stopbit */      \
    PARITY_NONE,      /* No parity  */     \
    BIT_ORDER_LSB,    /* LSB first sent */ \
    NRZ_NORMAL,       /* Normal mode */    \
    RT_SERIAL_RB_BUFSZ, /* Buffer size */  \
    0                                      \
}


rt_device_t  serial2;
#define USART2_NAME "uart2"
struct serial_configure usart2Config = RT_SERIAL_CONFIG_USART2;
rt_sem_t usart2_rec_sem = RT_NULL;
xUsart_Structure Uart2Buf;


/* ====== 光栅板原始数据 ====== */
volatile int32_t  g_raster_press_cumulative = 0;   /* 按压脉冲累计值 */
volatile int32_t  g_raster_blow_cumulative = 0;    /* 吹气脉冲累计值 */
volatile int8_t   g_raster_press_dir = 0;          /* 按压方向: -1=回弹, 0=静止, 1=下压 */
volatile int8_t   g_raster_blow_dir = 0;           /* 吹气方向: -1=泄气, 0=静止, 1=充气 */
volatile uint16_t g_raster_press_depth_01mm = 0;   /* 计算后的按压深度 (0.1mm) */
volatile uint16_t g_raster_blow_depth_01mm = 0;    /* 计算后的吹气深度 (0.1mm) */


/* ====== 帧解析状态机 ====== */
#define FRAME_HEAD      0xAA
#define FRAME_TAIL      0x55
#define CMD_REALTIME    0x04   /* 实时帧的LEN值（脉冲增量帧） */

typedef enum {
    PARSE_WAIT_HEAD = 0,   /* 等待帧头 0xAA */
    PARSE_WAIT_LEN,        /* 等待LEN字节 */
    PARSE_RECV_DATA,       /* 接收数据 */
    PARSE_WAIT_TAIL        /* 等待帧尾 0x55 */
} parse_state_t;

static parse_state_t parse_state = PARSE_WAIT_HEAD;
static uint8_t  parse_buf[10];     /* 帧缓冲 (最大10字节) */
static uint8_t  parse_idx = 0;    /* 当前接收索引 */
static uint8_t  parse_len = 0;    /* LEN字段值 */
static uint32_t parse_start_tick = 0;  /* 帧解析开始时间戳 */


/* ====== 联合判别状态 ====== */
#define JOINT_DISCRIM_PRESSURE_THRESHOLD  3500   /* 压力 ADC 阈值 */
static detect_state_et s_detect_state = DETECT_IDLE;


/**
  * @brief  UART2接收中断回调
  */
rt_err_t Usart2_RX_Callback(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(usart2_rec_sem);
    return RT_EOK;
}


/**
  * @brief  UART2初始化
  */
int USART2_Init(void)
{
    static rt_size_t sendNum = 0;

    /* 创建动态信号量 */
    usart2_rec_sem = rt_sem_create("dynamic_sem2", 0, RT_IPC_FLAG_FIFO);
    if (usart2_rec_sem == RT_NULL){
        rt_kprintf("PRINTF:%d. create dynamic semaphore failed.\n",Record.kprintf_cnt++);
        return -1;
    }
    else{
        rt_kprintf("PRINTF:%d. create done. dynamic semaphore value = 0.\n",Record.kprintf_cnt++);
    }

    serial2 = rt_device_find(USART2_NAME);
    if(serial2 != RT_NULL){
        rt_kprintf("PRINTF:%d. Usart2 Device node created succeed! \r\n",Record.kprintf_cnt++);
        usart2Config.baud_rate = BAUD_RATE_115200;
        usart2Config.bufsz = 2048;
    }
    else {
        rt_kprintf("PRINTF:%d. Usart2 Device node created Failed! \r\n",Record.kprintf_cnt++);
        return RT_ERROR;
    }

    rt_device_control(serial2, RT_DEVICE_CTRL_CONFIG, &usart2Config);
    rt_device_open(serial2, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(serial2, Usart2_RX_Callback);

    /* 初始化循环队列 */
    Uart2Buf.head = 0;
    Uart2Buf.tail = 0;
    Uart2Buf.lock = rt_mutex_create("uart2_buf_lock", RT_IPC_FLAG_FIFO);

    sendNum = rt_device_write(serial2, RT_NULL, "usart2 is opened!\r\n", 19);
    rt_kprintf("PRINTF:%d. The usart2 test send size : %d\r\n",Record.kprintf_cnt++,sendNum);

    return RT_EOK;
}


/**
  * @brief  处理解析完成的实时数据帧
  *         新帧格式: 0xAA + 0x04 + TYPE + CNT_H + CNT_L + DIR + CHK + 0x55 (8字节)
  * @param  buf: 从LEN开始的数据指针
  */
static void process_realtime_frame(uint8_t *buf)
{
    uint8_t calc_chk;
    uint8_t type;
    int16_t pulse_count;
    int8_t  dir;

    /* buf[0]=LEN(0x04), buf[1]=TYPE, buf[2]=CNT_H, buf[3]=CNT_L, buf[4]=DIR, buf[5]=CHK */

    /* 校验: 0xAA + LEN + TYPE + CNT_H + CNT_L + DIR */
    calc_chk = FRAME_HEAD + buf[0] + buf[1] + buf[2] + buf[3] + buf[4];
    if (calc_chk != buf[5])
    {
        rt_kprintf("UART2 Realtime Frame CHK error: calc=0x%02X recv=0x%02X\n",
                   calc_chk, buf[5]);
        return;
    }

    type = buf[1];
    pulse_count = (int16_t)(((uint16_t)buf[2] << 8) | buf[3]);
    dir = (int8_t)buf[4];

    if (type == 0x01)
    {
        /* 按压数据 */
        g_raster_press_cumulative += pulse_count;
        g_raster_press_dir = dir;
        /* 计算深度: 每脉冲 0.5mm = 5 (0.1mm单位) */
        g_raster_press_depth_01mm = (uint16_t)(g_raster_press_cumulative * 5);
    }
    else if (type == 0x02)
    {
        /* 吹气数据 */
        g_raster_blow_cumulative += pulse_count;
        g_raster_blow_dir = dir;
        g_raster_blow_depth_01mm = (uint16_t)(g_raster_blow_cumulative * 5);
    }
}


/**
  * @brief  UART2解码线程入口
  *         使用状态机解析来自光栅板的数据帧
  *
  *         实时帧: 0xAA + 0x04 + TYPE + CNT_H + CNT_L + DIR + CHK + 0x55 (8字节)
  */
void uart2_thread_entry(void* parameter)
{
    uint8_t recDat;
    rt_size_t sizeValue;

    while(1)
    {
        /* 等待 RX 中断通知有数据 */
        rt_sem_take(usart2_rec_sem, RT_WAITING_FOREVER);

        /* 从设备读取数据 */
        sizeValue = rt_device_read(serial2, RT_NULL, &recDat, 1);
        if(sizeValue == 1)
        {
            /* 加锁保护队列操作 */
            rt_mutex_take(Uart2Buf.lock, RT_WAITING_FOREVER);
            /* 计算下一个尾指针位置 */
            uint16_t next_tail = (Uart2Buf.tail + 1) % MAX_DATA_LENGTH;
            /* 队列未满 */
            if(next_tail != Uart2Buf.head) {
                Uart2Buf.rxBuffer[Uart2Buf.tail] = recDat;
                Uart2Buf.tail = next_tail;
            }
            else {
                rt_kprintf("UART2 Queue Full! Data Lost: 0x%02X\n", recDat);
            }
            /* 释放互斥锁 */
            rt_mutex_release(Uart2Buf.lock);

            /* ====== 帧解析超时保护 ====== */
            if (parse_state != PARSE_WAIT_HEAD) {
                /* 超过 50ms 未完成解析，强制重置 */
                if ((rt_tick_get() - parse_start_tick) > (50 * RT_TICK_PER_SECOND / 1000)) {
                    parse_state = PARSE_WAIT_HEAD;
                    parse_idx = 0;
                }
            }

            /* ====== 帧解析状态机 ====== */
            switch (parse_state)
            {
                case PARSE_WAIT_HEAD:
                    /* 等待帧头 0xAA */
                    if (recDat == FRAME_HEAD)
                    {
                        parse_start_tick = rt_tick_get();
                        parse_state = PARSE_WAIT_LEN;
                        parse_idx = 0;
                    }
                    break;

                case PARSE_WAIT_LEN:
                    if (recDat == FRAME_HEAD) {
                        /* 收到新的帧头，重新同步 */
                        parse_idx = 0;
                        /* parse_state 保持 PARSE_WAIT_LEN */
                    } else if (recDat == CMD_REALTIME) {
                        parse_len = recDat;
                        parse_buf[parse_idx++] = recDat;
                        parse_state = PARSE_RECV_DATA;
                    } else {
                        /* 未知LEN，回到等待帧头 */
                        parse_state = PARSE_WAIT_HEAD;
                        parse_idx = 0;
                    }
                    break;

                case PARSE_RECV_DATA:
                    if (recDat == FRAME_HEAD) {
                        /* 数据域中出现帧头，说明前面是残帧，重新同步 */
                        parse_state = PARSE_WAIT_LEN;
                        parse_idx = 0;
                    } else {
                        parse_buf[parse_idx++] = recDat;
                        if (parse_idx >= (parse_len + 1)) {
                            parse_state = PARSE_WAIT_TAIL;
                        }
                        if (parse_idx >= sizeof(parse_buf)) {
                            parse_state = PARSE_WAIT_HEAD;
                            parse_idx = 0;
                        }
                    }
                    break;

                case PARSE_WAIT_TAIL:
                    /* 检查帧尾 0x55 */
                    if (recDat == FRAME_TAIL)
                    {
                        process_realtime_frame(parse_buf);
                    }
                    /* 无论帧尾是否正确，都回到等待帧头 */
                    parse_state = PARSE_WAIT_HEAD;
                    parse_idx = 0;
                    break;

                default:
                    parse_state = PARSE_WAIT_HEAD;
                    parse_idx = 0;
                    break;
            }
        }

        /* 三路信号联合判别（每50ms调用一次） */
        {
            static uint32_t last_discrim_tick = 0;
            uint32_t now = rt_tick_get();
            if ((now - last_discrim_tick) >= (50 * RT_TICK_PER_SECOND / 1000))
            {
                last_discrim_tick = now;
                rt_uint16_t pressure_raw = 0;
                adc128s102_read_raw(ADC128S_Channel_0, &pressure_raw);
                joint_discrimination(pressure_raw);
            }
        }

        rt_thread_mdelay(10);
    }
}


/**
  * @brief  发送开始指令到光栅板
  *         帧格式: 0xAA + 0x02 + 0x01 + 0xFF + CHK + 0x55 (6字节)
  * @param  None
  * @retval None
  */
void USART2_Send_Start_To_Raster(void)
{
    uint8_t buf[6];
    uint8_t chk;

    buf[0] = 0xAA;  /* 帧头 */
    buf[1] = 0x02;  /* LEN: CMD(1)+DATA(1) = 2 */
    buf[2] = 0x01;  /* CMD: 开始采集 */
    buf[3] = 0xFF;  /* DATA: 固定值 */

    /* 校验和: Byte[0]+Byte[1]+Byte[2]+Byte[3] */
    chk = buf[0] + buf[1] + buf[2] + buf[3];
    buf[4] = chk;
    buf[5] = 0x55;  /* 帧尾 */

    rt_device_write(serial2, 0, buf, 6);

    /* 清零 Sensor 端累计值（与 Raster 端同步） */
    g_raster_press_cumulative = 0;
    g_raster_blow_cumulative = 0;
    g_raster_press_dir = 0;
    g_raster_blow_dir = 0;
    g_raster_press_depth_01mm = 0;
    g_raster_blow_depth_01mm = 0;

    rt_kprintf("UART2 Send START to Raster (cumulative cleared)\n");
}


/**
  * @brief  发送停止指令到光栅板
  *         帧格式: 0xAA + 0x02 + 0x03 + 0xFF + CHK + 0x55 (6字节)
  * @param  None
  * @retval None
  */
void USART2_Send_Stop_To_Raster(void)
{
    uint8_t buf[6];
    uint8_t chk;

    buf[0] = 0xAA;  /* 帧头 */
    buf[1] = 0x02;  /* LEN: CMD(1)+DATA(1) = 2 */
    buf[2] = 0x03;  /* CMD: 停止采集 */
    buf[3] = 0xFF;  /* DATA: 固定值 */

    /* 校验和: Byte[0]+Byte[1]+Byte[2]+Byte[3] */
    chk = buf[0] + buf[1] + buf[2] + buf[3];
    buf[4] = chk;
    buf[5] = 0x55;  /* 帧尾 */

    rt_device_write(serial2, 0, buf, 6);
    rt_kprintf("UART2 Send STOP to Raster\n");
}


/* ====== 模式切换指令发送 ====== */

/**
  * @brief  发送进入按压模式指令到光栅板
  *         帧格式: 0xAA + 0x02 + 0x11 + 0x01 + CHK + 0x55 (6字节)
  *         CMD=0x11 表示模式切换类指令，DATA=CMD_ACTIVATE_PRESSURE(0x01)
  */
void send_activate_pressure_cmd(void)
{
    uint8_t buf[6];
    uint8_t chk;

    buf[0] = 0xAA;                         /* 帧头 */
    buf[1] = 0x02;                         /* LEN */
    buf[2] = 0x11;                         /* CMD: 模式切换指令 */
    buf[3] = CMD_ACTIVATE_PRESSURE;        /* DATA: 进入按压模式 */

    chk = buf[0] + buf[1] + buf[2] + buf[3];
    buf[4] = chk;
    buf[5] = 0x55;

    rt_device_write(serial2, 0, buf, 6);
    rt_kprintf("UART2 Send ACTIVATE_PRESSURE to Raster\n");
}


/**
  * @brief  发送退出按压模式指令到光栅板
  *         帧格式: 0xAA + 0x02 + 0x11 + 0x02 + CHK + 0x55 (6字节)
  */
void send_idle_pressure_cmd(void)
{
    uint8_t buf[6];
    uint8_t chk;

    buf[0] = 0xAA;
    buf[1] = 0x02;
    buf[2] = 0x11;                         /* CMD: 模式切换指令 */
    buf[3] = CMD_IDLE_PRESSURE;            /* DATA: 退出按压模式 */

    chk = buf[0] + buf[1] + buf[2] + buf[3];
    buf[4] = chk;
    buf[5] = 0x55;

    rt_device_write(serial2, 0, buf, 6);
    rt_kprintf("UART2 Send IDLE_PRESSURE to Raster\n");
}


/**
  * @brief  发送进入吹气模式指令到光栅板
  *         帧格式: 0xAA + 0x02 + 0x11 + 0x03 + CHK + 0x55 (6字节)
  */
void send_activate_blow_cmd(void)
{
    uint8_t buf[6];
    uint8_t chk;

    buf[0] = 0xAA;
    buf[1] = 0x02;
    buf[2] = 0x11;                         /* CMD: 模式切换指令 */
    buf[3] = CMD_ACTIVATE_BLOW;            /* DATA: 进入吹气模式 */

    chk = buf[0] + buf[1] + buf[2] + buf[3];
    buf[4] = chk;
    buf[5] = 0x55;

    rt_device_write(serial2, 0, buf, 6);
    rt_kprintf("UART2 Send ACTIVATE_BLOW to Raster\n");
}


/* ====== 三路信号联合判别 ====== */

/**
  * @brief  三路信号联合判别函数
  *
  *         输入: ADC128S 压力值 + MPU6050 pitch 上仰标志
  *         输出: 通过 UART2 发送模式切换指令到光栅板
  *
  *         判别组合表:
  *         | 压力超阈值 | pitch 上仰 | 结果            | 发送指令            |
  *         |-----------|-----------|----------------|-------------------|
  *         | 0         | 0         | DETECT_IDLE    | idle_pressure     |
  *         | 1         | 0         | DETECT_PRESS   | activate_pressure |
  *         | 0         | 1         | DETECT_BLOW    | activate_blow     |
  *         | 1         | 1         | DETECT_CONFLICT| activate_pressure (按压优先) |
  *
  * @param  pressure_raw: ADC128S 压力通道原始值
  */
void joint_discrimination(uint16_t pressure_raw)
{
    uint8_t pressure_triggered = (pressure_raw >= JOINT_DISCRIM_PRESSURE_THRESHOLD) ? 1 : 0;
    uint8_t tilt_triggered     = head_tilt_data.is_head_tilt_up;
    detect_state_et new_state  = DETECT_IDLE;

    /* 判别逻辑 */
    if (pressure_triggered && tilt_triggered) {
        /* 冲突: 两者同时触发，按压优先 */
        new_state = DETECT_PRESS_CONFIRMED;
    } else if (pressure_triggered) {
        new_state = DETECT_PRESS_CONFIRMED;
    } else if (tilt_triggered) {
        new_state = DETECT_BLOW_CONFIRMED;
    } else {
        new_state = DETECT_IDLE;
    }

    /* 状态变化时才发送指令，避免重复发送 */
    if (new_state != s_detect_state)
    {
        switch (new_state)
        {
            case DETECT_PRESS_CONFIRMED:
                send_activate_pressure_cmd();
                break;

            case DETECT_BLOW_CONFIRMED:
                send_activate_blow_cmd();
                break;

            case DETECT_IDLE:
                send_idle_pressure_cmd();
                break;

            default:
                break;
        }

        s_detect_state = new_state;

        rt_kprintf("JOINT: press=%u tilt=%u -> state=%d\n",
                   pressure_triggered, tilt_triggered, new_state);
    }
}


/* ====== 以下为原有接口，保持兼容 ====== */

rt_thread_t uart2_decodeThread_Handle;

int uart2_decodeThread_Init(void)
{
    uart2_decodeThread_Handle = rt_thread_create("uart2_thread_entry", uart2_thread_entry, RT_NULL, 4096, 10, 200);
    if(uart2_decodeThread_Handle != RT_NULL){
        rt_kprintf("PRINTF:%d. uart2 Thread is created!!\r\n",Record.kprintf_cnt++);
        USART2_Init();
        rt_thread_startup(uart2_decodeThread_Handle);
    }
    else {
        rt_kprintf("PRINTF:%d. Thread is not created!!\r\n",Record.kprintf_cnt++);
    }
    return RT_EOK;
}
