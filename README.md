# CPR Assessment Training Machine - 心肺复苏评估训练系统

## 🚀 项目概述

**4板协同嵌入式系统**，通过 NRF24L01 无线通信实现 CPR 训练评估：

```
┌─────────────────────────────┐    NRF24L01    ┌─────────────────────────────┐
│        传感器板              │ ─────────────► │        主控显示板            │
│ STM32F103RCTx               │                │ STM32F103ZETx               │
│ MPU6050 + ADS1115 + ADC128  │ ◄────────────── │ 数码管/打印机/语音 WT588D    │
│ WS2812B LED反馈             │                │ RS485/RS232                  │
└─────────────────────────────┘                └─────────────────────────────┘
         │ NRF24L01                                        ▲
         └────────────── UART 脉冲 ────────────────────────┘
                  ┌─────────────────────────────┐
                  │        光栅板                │
                  │ STM8S 裸机 (IAR)             │
                  │ 光栅编码器 (A/B相中断)      │
                  └─────────────────────────────┘
                                                         NRF24L01
                                            ┌─────────────────────────────┐
                                            │        远程手柄              │ ◄──────
                                            │ STM32F103RGTx + LVGL 8.3.10 │
                                            │ ST7789 LCD + FT6336U 触控   │
                                            └─────────────────────────────┘
```

## 🏗️ 系统架构

### 板级分工
| 板名 | MCU | 角色 | 关键外设 |
|------|-----|------|----------|
| sensor | STM32F103RC LQFP64 | 姿态/力采集 | MPU6050(I2C PB6/7), ADS1115(I2C), ADC128(SPI), RS485(UART3 PA10/9), WS2812B(PB1 DMA) |
| raster | STM8S | 深度编码 | GPIO中断(A/B相), UART1 输出脉冲 |
| main-display | STM32F103ZE LQFP144 | 评估/显示 | 数码管/LED光条, DM32打印机, WT588D语音, RS485/RS232, NRF24L01 |
| remote | STM32F103RG LQFP64 | 交互 | ST7789 LCD(SPI), FT6336U触控(I2C), NRF24L01, ADC电池 |

### 通讯协议
1. **NRF24L01** (2.4GHz, 32B payload):
   ```
   [CMD 1B] [SRC 1B] [LEN 1B] [PAYLOAD N<=29B] [CRC 2B]
   CMD 示例: 0x01 ASK_Connect, 0x02 SEND_Press_Data
   ```
2. **RS485**：自定义消息帧 (设备层/驱动层分离)
3. **UART 脉冲**：raster → sensor/main (深度mm + 方向)

## 🔧 外设资源表 (Sensor板示例)

| 外设 | 接口 | 引脚 | 驱动文件 |
|------|------|------|----------|
| MPU6050 | I2C1 | PB6(SCL)/PB7(SDA) | macMPU/bsp_mpu6050.c (mpu6xxx-v1.1.1) |
| ADS1115 | I2C1 | PB6/7 | macBSP/adc1115idgsr.c |
| ADC128 | SPI3 | PB3(SCK)/PB4(MISO)/PB5(MOSI)/CS | macBSP/adc128s102cimtx.c |
| RS485 | UART3 | PA9(TX)/PA10(RX)/DE | macBSP/bsp_rs485_dev/drv/message.c |
| WS2812B | GPIO DMA | PB1 | macTask/ws2812b_task.c |
| NRF24L01 | SPI3 + IRQ | PB3/4/5 + PD2(IRQ) | macNRF/sensor_nrf24l01_driver/spi/message.c |

## 🧵 线程与初始化排序 (Sensor板)

**start_task.c** 4阶段编排：
```
Stage 0: RT-Thread SysTick/GPIO (board.c)
Stage 1: Bus (drv_uart/spi/i2c)
Stage 2: 外设sem (peripheral_init)
Stage 3: 反序线程 (低优先先)
  sysTimer(31) → Test(25) → NRF(20, sem阻塞ready) → hardware(15, MPU/RS485/ADC) → WS2812B(10)
```

## 📦 编译部署

### SCons (推荐)
```
scons --menuconfig  # Kconfig 配置 RT-Thread
scons               # GCC ARM 交叉编译
```

### Eclipse/RT-Thread Studio
1. 导入 `.cproject`
2. RT-Thread → Update Kconfig
3. Build → .hex/.bin

### Keil MDK
1. 导入 `.uvprojx`
2. Options → RT-Thread Settings → Update Components
3. Build → 下载

## 📚 关键文档
- [CHANGELOG.md](CHANGELOG.md)：修改历史
- [thread-timing-analysis.md](cpr-sensor-board_ver1.0/thread-timing-analysis.md)：线程时序
- [architecture-report.md](architecture-report.md)：资源统计

## 🔮 下一步
1. raster板 NRF RX 实现
2. NRF 帧 CRC/SEQ 生产化
3. Keil 一键烧录脚本

---
*2026-04-16 by 龙虾主管 @coder*
