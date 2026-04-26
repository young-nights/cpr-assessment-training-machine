/**
  ******************************************************************************
  * @file    main.c
  * @brief   光栅板主程序（纯采集模式）
  *
  *          STM8 只负责中断驱动脉冲计数，每100ms发送原始数据给Sensor板。
  *          所有成绩计算由Sensor板完成。
  *
  *          状态机:
  *          RASTER_IDLE    - 等待Sensor板发送开始指令
  *          RASTER_ACTIVE  - 采集中，每100ms发送原始脉冲计数
  ******************************************************************************
  */

#include "stm8s.h"
#include "app_sys.h"

/* 全局状态机变量 */
volatile raster_state_t raster_state = RASTER_IDLE;


void main(void)
{
    int16_t delta_press;
    int16_t delta_blow;
    int32_t prev_press;
    int32_t prev_blow;
    uint32_t last_send_tick = 0;   /* 上次发送时间戳 */

    /* 关闭所有中断 */
    disableInterrupts();

    /* 时钟配置 (HSI 16MHz) */
    System_Clock_Config();

    /* TIM1 定时器配置 (1ms系统滴答) */
    TIM1_Config();

    /* 初始化UART1接收缓冲区 */
    UART1_ReceiveValueInit(&USART1_QueueBuf, UART_DATALENGTH);

    /* UART1 配置 (115200bps, 8N1) */
    UART1_Config();

    /* GPIO外部中断配置 (光栅编码器A/B相) */
    GPIO_EXTIConfig();

    /* 开启所有中断 */
    enableInterrupts();


    /* 主循环 */
    while (1)
    {
        /* 1. 检查并处理来自Sensor板的接收指令 */
        USART1_ProcessRxData();

        /* 2. 状态机处理 */
        switch (raster_state)
        {
            case RASTER_IDLE:
                /* 待机状态: 等待开始指令，不发送任何数据 */
                prev_press = 0;
                prev_blow = 0;
                break;

            case RASTER_ACTIVE:
                /* 每100ms发送一次（非阻塞） */
                if ((g_system_tick_ms - last_send_tick) >= 100)
                {
                    last_send_tick = g_system_tick_ms;

                    /* 计算自上次发送以来的脉冲增量 */
                    delta_press = (int16_t)(depth_count_press - prev_press);
                    delta_blow  = (int16_t)(depth_count_blow - prev_blow);
                    prev_press = depth_count_press;
                    prev_blow  = depth_count_blow;

                    /* 发送按压或吹气帧（二选一，不同时发生） */
                    if (direction_blow != 0) {
                        /* 吹气阶段 */
                        USART1_SendRealtimeData(delta_blow, direction_blow, 0x02);
                    } else {
                        /* 按压阶段（含静止） */
                        USART1_SendRealtimeData(delta_press, direction_press, 0x01);
                    }
                }
                break;

            default:
                raster_state = RASTER_IDLE;
                break;
        }

        /* 3. 短延时（非关键路径，给UART接收留时间） */
        Delay_ms(5);
    }
}


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
    while (1) {}
}
#endif
