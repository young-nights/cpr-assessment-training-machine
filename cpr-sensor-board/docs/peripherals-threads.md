# CPR Sensor Board v1.0 - 外设与线程总结

**生成**: 龙虾主管 2026-04-17  
**来源**: grep + rtconfig/SConstruct

## 外设初始化

| 外设 | 类型 | 初始化函数 | 阶段 |
|------|------|------------|------|
| nRF24L01 | 无线通信 | nRF24L01_Thread_Init() | APP prio10 |
| WS2812B | RGB LED | ws2812b_init() | BSP |
| ADS1115 | ADC I2C | ads1115_device_init() | INIT_APP_EXPORT |
| ADC128S102 | ADC SPI | adc128s102_spi_init() | INIT_DEVICE_EXPORT |
| UART2 | 协议通信 | USART2_Init() + uart2_decodeThread_Init() | UART |
| UART3/RS485 | 串口 | USART3_Init() + rs485_decode_thread_init() | UART |
| MPU6050 | IMU | mpu6050_init() | Package |

## RT-Thread 列表

| 线程名 | 功能 | 优先级 | 栈大小 | 触发方式 |
|--------|------|--------|--------|----------|
| nRF24L01_Thread_entry | nRF RX/TX IRQ处理 | TBD | 4096 | Sem/IRQ |
| adc128s102_thread_entry | ADC128S102采样 | 9 | 1024 | 100 ticks |
| uart2_decode_thread | UART2协议解析 | TBD | TBD | 数据ready |
| uart3_decode_thread | RS485协议解析 | TBD | TBD | 数据ready |
| ws2812b_update_task | WS2812B LED更新 | TBD | TBD | Timer/DMA |

## 配置摘要

```
RT_TICK_PER_SECOND = 1000
FINSH tshell prio20 stack4096
启用: SERIAL / I2C / SPI / PIN / ADC
MPU6XXX pkg (ACCE / GYRO / MAG)
```

## 硬件细节

- nRF24L01: SPI3, PD2 IRQ
- WS2812B: DMA + TIM1_CH1

## 编译

```bash
scons --target=mdk5
```

---

*龙虾主管自动生成*
