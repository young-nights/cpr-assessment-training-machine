# CPR 考核培训机 — 硬件接口文档

> **文档版本**：v1.0
> **日期**：2026-05-20
> **范围**：三板系统（主板 / 遥控器 / 传感器）硬件引脚分配、外设连接及 nRF24L01 无线通信拓扑。

---

## 目录

1. [系统概述](#1-系统概述)
2. [主板硬件接口](#2-主板硬件接口)
3. [遥控器硬件接口](#3-遥控器硬件接口)
4. [传感器硬件接口](#4-传感器硬件接口)
5. [nRF24L01 无线通信](#5-nrf24l01-无线通信)
6. [板间连接汇总](#6-板间连接汇总)
7. [附录：CubeMX 引脚宏参考](#7-附录cubemx-引脚宏参考)

---

## 1. 系统概述

CPR 考核培训机由 **三块独立的 STM32F103 板卡** 组成，通过 **星型拓扑 nRF24L01 2.4 GHz 无线网络** 进行通信。

### 系统架构

```
┌─────────────────────────────────────────────────────────────────┐
│                        主板 MAINBOARD（中心节点）                   │
│  STM32F103ZE  |  RT-Thread OS  |  nRF24L01 Device ID: —         │
│  功能：显示 + 语音 + 打印 + LED 面板 + 有线检测                     │
│  目标设备：REMOTE_ID=0x0004, SENSOR_ID=0x0005                    │
└────────────┬────────────────────────────────┬───────────────────┘
             │  nRF24L01 (2.4 GHz)            │
     ┌───────▼────────┐              ┌────────▼──────────┐
     │    遥控器 REMOTE  │              │   传感器 SENSOR    │
     │  STM32F103RF      │              │  STM32F103RC       │
     │  RT-Thread+LVGL   │              │  RT-Thread OS      │
     │  Device ID:0x0004 │              │  Device ID:0x0005  │
     │  TFT 触摸 + 按键   │              │  按压 + 呼吸       │
     │  + 电池供电        │              │  + 姿态 + 检测      │
     └──────────────────┘              └────────────────────┘
```

### 板卡职责

| 板卡 | 角色 | 核心功能 |
|------|------|---------|
| **主板** | 中央控制器 | LED 面板显示、语音提示（WT588D）、热敏打印（RD-DM32）、FAL Flash 存储、有线连接检测、RS232/RS485 有线通信 |
| **遥控器** | 用户控制终端 | 3.2" TFT LCD（ST7789）+ 电容触摸（FT6336U）+ 3×3 矩阵键盘，模式/参数设置，电池供电并带充电管理 |
| **传感器** | 假人传感器中枢 | 按压深度检测（光栅板，通过 UART2）、吹气/呼吸检测（UART3）、姿态监测（MPU6050）、8 通道外部 ADC（ADC128S102）、异物检测（CC6201 霍尔）、意识判断（压电陶瓷）、脉搏模拟（空心杯电机）、WS2812B 灯带、OLED 眼睛显示 |

---

## 2. 主板硬件接口

### 2.1 MCU 参数

| 参数 | 值 |
|------|-----|
| **芯片** | STM32F103ZET6 |
| **内核** | ARM Cortex-M3 |
| **最高频率** | 72 MHz |
| **Flash** | 512 KB |
| **SRAM** | 64 KB |
| **封装** | LQFP-144 |
| **HSE** | 8 MHz |
| **操作系统** | RT-Thread |

### 2.2 引脚分配表

#### 通信接口

| 外设 | 总线 | 引脚 | DMA / 备注 |
|------|------|------|-------------|
| **UART1（调试）** | USART1 | TX=PA9, RX=PA10 | 调试控制台，115200 波特率 |
| **UART3（RS232）** | USART3 | TX=PB10, RX=PB11 | DMA RX/TX，连接 RS232 外部设备 |
| **UART4（RS485）** | UART4 | TX=PC10, RX=PC11 | DMA RX/TX（hdma_uart4_rx, hdma_uart4_tx），连接 RS485 总线 |
| **SPI2（nRF24L01）** | SPI2 | SCK,MOSI,MISO 由 CubeMX 生成 | nRF24L01 无线通信 |
| **nRF24L01 CE** | GPIO | PD8 | 芯片使能（宏定义为 `nRF24L01_CE_Pin`） |
| **nRF24L01 CSN** | GPIO | PB12 | SPI 片选（宏定义为 `nRF24L01_CSN_Pin`） |
| **nRF24L01 IRQ** | GPIO | PD9 | 中断信号（宏定义为 `nRF24L01_IRQ_Pin`） |

#### LED 面板指示灯（均为 GPIO，CubeMX 生成）

| LED 名称 | 功能 | 端口 | 引脚宏 |
|----------|------|------|--------|
| Debug | 调试指示灯 | PE1 | `LED_DEBUG_Pin` |
| Conscious Judgment | 意识判断指示灯 | PF1 | `LED_CONSCIOUS_JUDGMENT_Pin` |
| Sphygmoscopy | 脉搏检查指示灯 | PF0 | `LED_SPHYGMOSCOPY_Pin` |
| Check Breath | 呼吸检查指示灯 | PC15 | `LED_CHECK_BREATH_Pin` |
| Emergency Call | 紧急呼叫指示灯 | PC14 | `LED_EMERGENCY_CALL_Pin` |
| Remove Foreign | 清除异物指示灯 | PC13 | `LED_REMOVE_FOREIGN_Pin` |
| Body1 | 按压位置 1 | PD7 | `LED_BODY1_Pin` |
| Body2 | 按压位置 2 | PG9 | `LED_BODY2_Pin` |
| Body3 | 按压位置 3 | PG10 | `LED_BODY3_Pin` |
| Body4 | 按压位置 4 | PG11 | `LED_BODY4_Pin` |
| Body5 | 按压位置 5 | PG12 | `LED_BODY5_Pin` |
| Body6 | 按压位置 6 | PG13 | `LED_BODY6_Pin` |
| Body7 | 按压位置 7 | PG14 | `LED_BODY7_Pin` |
| Reset | 复位指示灯 | PD12 | `LED_RESET_Pin` |
| Competition | 竞赛模式指示灯 | PG7 | `LED_COMPETITION_Pin` |
| Assess | 考核模式指示灯 | PE9 | `LED_ASSESS_Pin` |
| Train | 训练模式指示灯 | PG8 | `LED_TRAIN_Pin` |
| Minus | 减号指示灯 | PE12 | `LED_MINUS_Pin` |
| Plus | 加号指示灯 | PG5 | `LED_PLUS_Pin` |
| Setting | 设置指示灯 | PD13 | `LED_SETTING_Pin` |
| Printer | 打印状态指示灯 | PD10 | `LED_PRINTER_Pin` |
| Start | 开始指示灯 | PE10 | `LED_START_Pin` |

> **LED 总数**：22 个（`bsp_led.h` 中定义为 `LED_NUM=30`，包含预留槽位）

#### WT588D 语音模块（3 线类 SPI，GPIO 控制）

| 信号 | 端口 | 引脚宏 |
|------|------|--------|
| DATA | PC7 | `WT588D_DATA_Pin` |
| CS | PC8 | `WT588D_CS_Pin` |
| CLK | PC9 | `WT588D_CLK_Pin` |
| RESET | PD3 | `WT588D_RESET_Pin` |
| BUSY | PD4 | `WT588D_BUSY_Pin` |

#### RD-DM32 热敏打印机

| 信号 | 端口 | 引脚宏 | 备注 |
|------|------|--------|------|
| CTS | PC12 | `PRINTER_CTS_Pin` | 打印机忙/就绪流控（低电平有效） |

#### TM1629A 数码管驱动（2 通道）

| 通道 | DIO | CLK | STB |
|------|-----|-----|-----|
| 通道 A | PE13（`TM1629A_A_DIO_Pin`） | PE14（`TM1629A_A_CLK_Pin`） | PE15（`TM1629A_A_STB_Pin`） |
| 通道 B | PA4（`TM1629A_B_DIO_Pin`） | PA5（`TM1629A_B_CLK_Pin`） | PA6（`TM1629A_B_STB_Pin`） |

#### TM1638 LED 驱动

| 信号 | 端口 | 引脚宏 |
|------|------|--------|
| DIO | PD0 | `TM1638_DIO_Pin` |
| CLK | PD1 | `TM1638_CLK_Pin` |
| STB | PD2 | `TM1638_STB_Pin` |

#### 触摸输入（假人面板电容触摸按键）

| 输入 | 端口 | 引脚宏 |
|------|------|--------|
| TOUCH_IN1 | PD15 | `TOUCH_IN1_Pin` |
| TOUCH_IN2 | PG1 | `TOUCH_IN2_Pin` |
| TOUCH_IN3 | PG2 | `TOUCH_IN3_Pin` |
| TOUCH_IN4 | PG0 | `TOUCH_IN4_Pin` |
| TOUCH_IN5 | PG3 | `TOUCH_IN5_Pin` |
| TOUCH_IN6 | PF15 | `TOUCH_IN6_Pin` |
| TOUCH_IN7 | PF13 | `TOUCH_IN7_Pin` |
| TOUCH_IN8 | PG4 | `TOUCH_IN8_Pin` |
| TOUCH_IN9 | PF14 | `TOUCH_IN9_Pin` |
| TOUCH_IN10 | PE2 | `TOUCH_IN10_Pin` |
| TOUCH_IN11 | PE3 | `TOUCH_IN11_Pin` |
| TOUCH_IN12 | PE4 | `TOUCH_IN12_Pin` |
| TOUCH_IN13 | PE5 | `TOUCH_IN13_Pin` |
| TOUCH_IN14 | PE6 | `TOUCH_IN14_Pin` |

#### 有线连接检测

| 信号 | 端口 | 引脚宏 | 备注 |
|------|------|--------|------|
| WIRED_CONNECT_CHECK | PC6 | `WIRED_CONNECT_CHECK_Pin` | 检测假人是否通过有线方式连接到主板 |

### 2.3 外设汇总

| 外设 | 接口 | 用途 |
|------|------|------|
| **WT588D** | 3 线 GPIO（PC7/PC8/PC9）+ RST(PD3) + BUSY(PD4) | 语音/音频提示与反馈 |
| **RD-DM32** | UART?（CTS 接 PC12） | 热敏打印机，输出成绩/结果 |
| **FAL Flash** | 片上 Flash，通过 FAL 组件访问 | 持久化存储记录与配置 |
| **TM1629A ×2** | 3 线 GPIO | 双通道数码管显示驱动 |
| **TM1638** | 3 线 GPIO（PD0/PD1/PD2） | LED + 按键扫描驱动 IC |
| **RS232** | UART3（PB10/PB11）带 DMA | 与外部 RS232 设备有线通信 |
| **RS485** | UART4（PC10/PC11）带 DMA | 与外部 RS485 设备有线通信 |

---

## 3. 遥控器硬件接口

### 3.1 MCU 参数

| 参数 | 值 |
|------|-----|
| **芯片** | STM32F103RFT6 |
| **内核** | ARM Cortex-M3 |
| **最高频率** | 72 MHz |
| **Flash** | 768 KB |
| **SRAM** | 96 KB |
| **封装** | LQFP-64 |
| **HSE** | 8 MHz |
| **操作系统 + GUI** | RT-Thread + LVGL |

### 3.2 引脚分配表

#### 通信接口

| 外设 | 总线 | 引脚 | 备注 |
|------|------|------|------|
| **UART1** | USART1 | TX=PA9, RX=PA10 | 调试/串口控制台 |
| **SPI1（TFT LCD）** | SPI1 | SCK,MOSI,MISO 由 CubeMX 生成 | ST7789 TFT 显示屏（240×320） |
| **SPI3（nRF24L01）** | SPI3 | SCK,MOSI,MISO 由 CubeMX 生成 | nRF24L01 无线模块 |
| **I2C1（触摸）** | I2C1 | SCL=PB6, SDA=PB7 | FT6336U 电容触摸控制器 |
| **ADC1（电池）** | ADC1 | BAT_VOL=PA1 | 电池电压监测 |

#### ST7789 TFT LCD 控制引脚

| 信号 | 端口 | 引脚宏 | 功能 |
|------|------|--------|------|
| CS | PB0 | `LCD_CS_Pin` | SPI 片选 |
| RST | PC5 | `LCD_RST_Pin` | 硬件复位 |
| BLK | PB15 | `LCD_BLK_Pin` | 背光控制（支持 PWM） |
| DC | PC4 | `LCD_DC_Pin` | 数据/命令选择 |

#### FT6336U 电容触摸

| 信号 | 端口 | 引脚宏 | 功能 |
|------|------|--------|------|
| I2C SCL | PB6 | — | 硬件 I2C1 时钟 |
| I2C SDA | PB7 | — | 硬件 I2C1 数据 |
| INT（IRQ） | PA6 | `TOUCH_INT_Pin` | 触摸中断（低电平有效） |
| RST | PA4 | `TOUCH_RST_Pin` | 触摸控制器复位 |

#### nRF24L01 无线模块（SPI3）

| 信号 | 端口 | 引脚宏 | 功能 |
|------|------|--------|------|
| CE | PB11 | `nRF24_CE_Pin` | 芯片使能 |
| CSN | PA15 | `nRF24_CSN_Pin` | SPI 片选（NSS） |
| IRQ | PB10 | `nRF24_IRQ_Pin` | 中断信号（EXTI15_10_IRQn） |
| SPI 总线 | SPI3 | SCK,MOSI,MISO | 硬件 SPI3 |

#### 3×3 矩阵键盘

| 行/列 | 端口 | 引脚宏 |
|--------|------|--------|
| 第 1 行 | PB12 | `Matrixkey_Row1_Pin` |
| 第 2 行 | PB13 | `Matrixkey_Row2_Pin` |
| 第 3 行 | PB14 | `Matrixkey_Row3_Pin` |
| 第 1 列 | PC6 | `Matrixkey_Column1_Pin` |
| 第 2 列 | PC7 | `Matrixkey_Column2_Pin` |
| 第 3 列 | PC8 | `Matrixkey_Column3_Pin` |

#### 电池管理

| 信号 | 端口 | 引脚宏 | 功能 |
|------|------|--------|------|
| BAT_PROG | PA0 | `BAT_PROG_Pin` | 充电电流编程 |
| BAT_VOL | PA1 | `BAT_VOL_Pin` | 电池电压 ADC 输入 |
| BAT_CHARG | PA3 | `BAT_CHARG_Pin` | 充电状态检测 |
| BAT_STDBY | PC2 | `BAT_STDBY_Pin` | 待机/充电完成 |
| BAT_EN | PC3 | `BAT_EN_Pin` | 充电使能控制 |

> 参考电压：3.3V，ADC 分辨率：12 位，RPROG=2kΩ

#### 状态 LED

| LED | 端口 | 引脚宏 | 功能 |
|-----|------|--------|------|
| 绿色 | PC0 | `LED_GREEN_Pin` | 电源/状态指示 |

### 3.3 外设汇总

| 外设 | 接口 | 用途 |
|------|------|------|
| **ST7789 TFT** | SPI1 + GPIO（PB0/PC5/PB15/PC4） | 240×320 彩色 LCD 显示屏 |
| **FT6336U** | I2C1（PB6/PB7）+ INT(PA6) + RST(PA4) | 电容触摸输入 |
| **nRF24L01** | SPI3 + GPIO（PB11/PA15/PB10） | 2.4 GHz 无线通信 |
| **3×3 矩阵键盘** | GPIO（PB12-14, PC6-8） | 物理按键输入 |
| **电池管理** | ADC(PA1) + GPIO(PA0/PA3/PC2/PC3) | 锂电池充放电管理 |

---

## 4. 传感器硬件接口

### 4.1 MCU 参数

| 参数 | 值 |
|------|-----|
| **芯片** | STM32F103RCT6 |
| **内核** | ARM Cortex-M3 |
| **最高频率** | 72 MHz |
| **Flash** | 256 KB |
| **SRAM** | 48 KB |
| **封装** | LQFP-64 |
| **HSE** | 8 MHz |
| **操作系统** | RT-Thread |

### 4.2 引脚分配表

#### 通信接口

| 外设 | 总线 | 引脚 | 备注 |
|------|------|------|------|
| **UART1（调试）** | USART1 | TX=PA9, RX=PA10 | 调试控制台 |
| **UART2（光栅板）** | USART2 | TX=PA2, RX=PA3 | 光栅板协议：按压深度 / 吹气检测 |
| **UART3（协议通信）** | USART3 | TX=PB10, RX=PB11 | 通用协议通信 |
| **SPI1（nRF24L01）** | SPI1 | SCK,MOSI,MISO 由 CubeMX 生成 | nRF24L01 无线模块 |
| **SPI2（WS2812B）** | SPI2 | SCK,MOSI 由 CubeMX 生成 | WS2812B 灯带（77 颗 LED，SPI 方式） |
| **SPI3（ADC128S102）** | SPI3 | SCK,MOSI,MISO 由 CubeMX 生成 | 8 通道外部 ADC |
| **I2C1（MPU6050）** | I2C1 | SCL=PB0, SDA=PC5 | 6 轴 IMU（加速度计 + 陀螺仪） |
| **ADC1** | ADC1 | 片上 ADC 通道 | 片上模拟量采集 |

#### nRF24L01 无线模块（SPI1）

| 信号 | 端口 | 引脚宏 | 功能 |
|------|------|--------|------|
| CE | PB7 | `nRF24_CE_Pin` | 芯片使能（rt_pin API） |
| CSN | PB6 | `nRF24_CSN_Pin` | SPI 片选（NSS） |
| IRQ | PD2 | `nRF24_IRQ_Pin` | 中断信号 |

#### ADC128S102 外部 ADC（SPI3）

| 信号 | 端口 | 引脚宏 | 功能 |
|------|------|--------|------|
| CS | PA4 | `SPI1_NSS_Pin` | SPI 片选 |
| 总线 | SPI3 | SCK,MOSI,MISO | 硬件 SPI3 |

> 8 通道、12 位 SAR ADC，用于外部模拟信号采集。

#### WS2812B 灯带（SPI2）

| 信号 | 端口 | 引脚宏 | 功能 |
|------|------|--------|------|
| NSS | PB14 | `SPI2_NSS_Pin` | 基于 SPI 的 LED 数据输出 |
| 总线 | SPI2 | MOSI | 硬件 SPI2（SPI 方式模式） |

> **77 颗 LED**（`WS2812B_LED_NUMS=77`），SPI 位脉冲编码：0 码=0xC0，1 码=0xF0。

#### MPU6050 6 轴 IMU（I2C1）

| 信号 | 端口 | 功能 |
|------|------|------|
| I2C SCL | PB0 | 硬件 I2C1 时钟 |
| I2C SDA | PC5 | 硬件 I2C1 数据 |

#### OLED 眼睛显示（软件 I2C）

| 信号 | 端口 | 引脚 | 功能 |
|------|------|------|------|
| SDA | PC10 | GPIO | 位脉冲 I2C 数据 |
| SCL | PC11 | GPIO | 位脉冲 I2C 时钟 |

> 显示屏：ST7315 驱动，64×48 像素，I2C 地址 0x3C，6 页。

#### 脉搏模拟（空心杯电机）及外设 GPIO

| 信号 | 端口 | 引脚宏 | 功能 |
|------|------|--------|------|
| SPHYGMUS_CTRL1 | PC2 | `SPHYGMUS_CTRL1_Pin` | 电机 1 控制 |
| SPHYGMUS_CTRL2 | PC3 | `SPHYGMUS_CTRL2_Pin` | 电机 2 控制 |
| SPHYGMUS_KEY1 | PC14 | `SPHYGMUS_KEY1_Pin` | 电机按键输入 1 |
| SPHYGMUS_KEY2 | PC13 | `SPHYGMUS_KEY2_Pin` | 电机按键输入 2 |

#### 异物检测与意识判断

| 信号 | 端口 | 引脚宏 | 功能 |
|------|------|--------|------|
| MAGNETIC_STAT | PC1 | `MAGNETIC_STAT_Pin` | CC6201 霍尔传感器状态（异物检测） |
| SHAKE_DOUT0 | PB8 | `SHAKE_DOUT0_Pin` | 压电陶瓷数据 0（意识判断） |
| SHAKE_DOUT1 | PB9 | `SHAKE_DOUT1_Pin` | 压电陶瓷数据 1（意识判断） |

#### 调试 LED

| LED | 端口 | 引脚宏 | 功能 |
|-----|------|--------|------|
| DEBUG_LED | PA15 | `DEBUG_LED_Pin` | 通用调试指示灯 |

### 4.3 外设汇总

| 外设 | 接口 | 用途 |
|------|------|------|
| **nRF24L01** | SPI1 + GPIO（PB7/PB6/PD2） | 无线数据传输至主板 |
| **光栅板** | UART2（PA2/PA3） | 通过光栅传感器协议获取按压深度/吹气数据 |
| **协议设备** | UART3（PB10/PB11） | 外部协议设备通信 |
| **MPU6050** | I2C1（PB0/PC5） | 6 轴姿态/角度监测 |
| **ADC128S102** | SPI3 + CS(PA4) | 8 通道外部模拟量采集 |
| **WS2812B** | SPI2 + NSS(PB14) | 77 颗 LED 灯带，用于视觉反馈 |
| **OLED 眼睛** | 软 I2C（PC10/PC11） | 64×48 OLED 眼睛表情显示 |
| **空心杯电机 ×2** | GPIO（PC2/PC3/PC13/PC14） | 脉搏模拟（脉搏检查） |
| **CC6201 霍尔传感器** | GPIO（PC1） | 异物（金属）检测 |
| **压电陶瓷** | GPIO（PB8/PB9） | 意识判断（敲击/振动检测） |
| **RS485** | 串口（复用 UART） | 与外部设备的有线 RS485 通信 |

---

## 5. nRF24L01 无线通信

### 5.1 RF 参数

| 参数 | 值 | 备注 |
|------|-----|------|
| **频段** | 2.400 – 2.525 GHz | ISM 频段，通道可通过 `RF_CH` 寄存器选择 |
| **数据速率** | 1 Mbps（ADR_1Mbps） | 扩展通信距离，抗干扰能力良好 |
| **发射功率** | 0 dBm（RF_POWER_0dBm） | 最大发射功率 |
| **CRC** | 2 字节（CRC_2_BYTE） | 增强错误检测 |
| **地址宽度** | 5 字节 | 完整 5 字节管道地址 |
| **动态负载** | 已启用（`EN_DPL`） | 支持可变长度负载 |
| **自动应答** | 管道 0 / 管道 1 | 每个管道可独立配置 |
| **自动重传** | 可配置（ARC + ARD） | 硬件级别重试 |

### 5.2 设备 ID 与管道分配

| 板卡 | nRF24L01 角色 | 设备 ID | 管道分配 |
|------|--------------|---------|----------|
| **主板** | PRX（主接收端） | —（中央枢纽） | 管道 0：接收来自遥控器的数据；管道 1：接收来自传感器的数据 |
| **遥控器** | PTX（主发送端） | `0x0004` | 管道 0：发送至主板 |
| **传感器** | PTX（主发送端） | `0x0005` | 管道 0：发送至主板 |

> **通信模型**：遥控器和传感器各自独立发起传输；主板同时在两个管道上监听，并通过 ACK 负载或独立 TX 进行回复。主板在协议帧中编码目标设备 ID（`REMOTE_ID=0x0004`、`SENSOR_ID=0x0005`），用于源/目标识别。

### 5.3 帧格式（CPR 协议）

应用层协议（`cpr_packet_t`）封装在 nRF24L01 负载中：

```
┌────────┬────────┬─────┬──────────┬─────┬────────┬──────┬─────────────────────┬──────────┐
│ head1  │ head2  │ len │ dev_type │ cmd │ status │ seq  │  payload（最大 24B） │ crc16    │
│ 1B     │ 1B     │ 1B  │ 1B       │ 1B  │ 1B     │ 2B   │  0~24 B             │ 2B       │
│ 0x55   │ 0xAA   │     │          │     │        │      │                     │ (Modbus) │
└────────┴────────┴─────┴──────────┴─────┴────────┴──────┴─────────────────────┴──────────┘
```

| 字段 | 大小 | 描述 |
|------|------|------|
| `head1` | 1 B | 帧同步字节 1：`0x55` |
| `head2` | 1 B | 帧同步字节 2：`0xAA` |
| `len` | 1 B | 从 `dev_type` 到 `crc` 之前的长度 |
| `dev_type` | 1 B | 源设备类型：`0x01`=主板，`0x02`=传感器，`0x03`=遥控器 |
| `cmd` | 1 B | 命令码（见下表） |
| `status` | 1 B | 状态：ASK / ACK / ERR |
| `seq` | 2 B | 序列号（防重放） |
| `payload` | 0–24 B | 可变长度数据（最大 24 字节） |
| `crc` | 2 B | CRC16-Modbus |

#### 命令码

| 码值 | 名称 | 方向 | 用途 |
|------|------|------|------|
| `0x01` | `CMD_ASK_CONNECT` | 传感器/遥控器 → 主板 | 连接请求 |
| `0x02` | `CMD_ACK_CONNECT` | 主板 → 设备 | 连接确认 |
| `0x10` | `CMD_SENSOR_DATA` | 传感器 → 主板 | 按压/呼吸/角度数据 |
| `0x20` | `CMD_REMOTE_CMD` | 遥控器 → 主板 | 模式/参数/启动命令 |
| `0x30` | `CMD_DISPLAY_FEEDBACK` | 主板 → 遥控器 | LED 状态、成绩反馈 |
| `0x40` | `CMD_HEARTBEAT` | 双向 | 保活心跳 |
| `0x50` | `CMD_MODE_IN` | — | 进入训练/考核/竞赛模式 |
| `0x51` | `CMD_MODE_OUT` | — | 退出模式 |
| `0x60` | `CMD_PRESS_LED_CTRL` | 遥控器/主板 | 按压位置 LED 控制 |

### 5.4 SPI 引脚汇总（按板卡）

| 板卡 | SPI 总线 | CE | CSN（NSS） | IRQ |
|------|---------|-----|-----------|-----|
| 主板 | SPI2 | PD8 | PB12 | PD9 |
| 遥控器 | SPI3 | PB11 | PA15 | PB10（EXTI15_10） |
| 传感器 | SPI1 | PB7 | PB6 | PD2 |

---

## 6. 板间连接汇总

### 6.1 无线连接

| 发送方 | 接收方 | 媒介 | 协议 |
|--------|--------|------|------|
| 遥控器（0x0004） | 主板 | nRF24L01 @ 2.4 GHz | CPR 协议（CMD_REMOTE_CMD 等） |
| 传感器（0x0005） | 主板 | nRF24L01 @ 2.4 GHz | CPR 协议（CMD_SENSOR_DATA 等） |
| 主板 | 遥控器 | nRF24L01 @ 2.4 GHz | CPR 协议（CMD_DISPLAY_FEEDBACK） |
| 主板 | 传感器 | nRF24L01 @ 2.4 GHz | CPR 协议（CMD_ACK_CONNECT 等） |

### 6.2 有线连接

| 板卡 | 接口 | 目标设备 | 协议 |
|------|------|----------|------|
| 传感器 → 光栅板 | UART2（PA2/PA3） | 光栅传感器板 | 二进制光栅协议（按压/吹气数据） |
| 传感器 → 外部设备 | UART3（PB10/PB11） | 待定协议设备 | 通用串口协议 |
| 主板 → 外部 RS232 设备 | UART3（PB10/PB11） | RS232 外设 | 基于 DMA 的串口通信 |
| 主板 → 外部 RS485 设备 | UART4（PC10/PC11） | RS485 外设 | 基于 DMA 的串口通信，半双工 |
| 主板 → 假人（有线检测） | GPIO（PC6） | 假人检测引脚 | 数字量输入（有线连接检测） |

### 6.3 供电拓扑

| 板卡 | 供电来源 | 备注 |
|------|----------|------|
| **主板** | DC 输入 / 外部电源 | 持续供电 |
| **遥控器** | 锂电池 + 充电电路 | ADC 电压监测，充电管理 |
| **传感器** | 通过线束外部供电 | 通过假人内部线缆供电 |

---

## 7. 附录：CubeMX 引脚宏参考

### 7.1 主板（`cpr-display-main-board/cubemx/Inc/main.h`）

```
nRF24L01_CSN_Pin          → PB12        LED_DEBUG_Pin             → PE1
nRF24L01_CE_Pin           → PD8         LED_CONSCIOUS_JUDGMENT_Pin → PF1
nRF24L01_IRQ_Pin           → PD9         LED_SPHYGMOSCOPY_Pin       → PF0
LED_REMOVE_FOREIGN_Pin     → PC13        LED_CHECK_BREATH_Pin       → PC15
LED_EMERGENCY_CALL_Pin     → PC14        LED_BODY1_Pin              → PD7
LED_BODY2_Pin              → PG9         LED_BODY3_Pin              → PG10
LED_BODY4_Pin              → PG11        LED_BODY5_Pin              → PG12
LED_BODY6_Pin              → PG13        LED_BODY7_Pin              → PG14
LED_RESET_Pin              → PD12        LED_COMPETITION_Pin        → PG7
LED_ASSESS_Pin             → PE9         LED_TRAIN_Pin              → PG8
LED_MINUS_Pin              → PE12        LED_PLUS_Pin               → PG5
LED_SETTING_Pin            → PD13        LED_PRINTER_Pin            → PD10
LED_START_Pin              → PE10        WIRED_CONNECT_CHECK_Pin    → PC6
WT588D_DATA_Pin            → PC7         WT588D_CS_Pin              → PC8
WT588D_CLK_Pin             → PC9         WT588D_RESET_Pin           → PD3
WT588D_BUSY_Pin            → PD4         PRINTER_CTS_Pin            → PC12
TM1629A_A_DIO_Pin          → PE13        TM1629A_A_CLK_Pin          → PE14
TM1629A_A_STB_Pin          → PE15        TM1629A_B_DIO_Pin          → PA4
TM1629A_B_CLK_Pin          → PA5         TM1629A_B_STB_Pin          → PA6
TM1638_DIO_Pin             → PD0         TM1638_CLK_Pin             → PD1
TM1638_STB_Pin             → PD2
TOUCH_IN1_Pin → PD15  TOUCH_IN2_Pin → PG1  TOUCH_IN3_Pin → PG2  TOUCH_IN4_Pin → PG0
TOUCH_IN5_Pin → PG3   TOUCH_IN6_Pin → PF15 TOUCH_IN7_Pin → PF13 TOUCH_IN8_Pin → PG4
TOUCH_IN9_Pin → PF14  TOUCH_IN10_Pin→ PE2  TOUCH_IN11_Pin→ PE3  TOUCH_IN12_Pin→ PE4
TOUCH_IN13_Pin→ PE5   TOUCH_IN14_Pin→ PE6
```

### 7.2 遥控器（`cpr-remote-device/cubemx/Inc/main.h`）

```
LED_GREEN_Pin             → PC0         BAT_STDBY_Pin              → PC2
BAT_EN_Pin                → PC3         BAT_PROG_Pin               → PA0
BAT_VOL_Pin               → PA1         BAT_CHARG_Pin              → PA3
TOUCH_RST_Pin             → PA4         TOUCH_INT_Pin              → PA6
LCD_DC_Pin                → PC4         LCD_RST_Pin                → PC5
LCD_CS_Pin                → PB0         LCD_BLK_Pin                → PB15
nRF24_IRQ_Pin             → PB10        nRF24_CE_Pin               → PB11
nRF24_CSN_Pin             → PA15
Matrixkey_Row1_Pin        → PB12        Matrixkey_Row2_Pin         → PB13
Matrixkey_Row3_Pin        → PB14        Matrixkey_Column1_Pin      → PC6
Matrixkey_Column2_Pin     → PC7         Matrixkey_Column3_Pin      → PC8
```

### 7.3 传感器（`cpr-sensor-board/cubemx/Inc/main.h`）

```
SPI1_NSS_Pin              → PA4         SPI2_NSS_Pin               → PB14
DEBUG_LED_Pin             → PA15        nRF24_IRQ_Pin              → PD2
nRF24_CSN_Pin             → PB6         nRF24_CE_Pin               → PB7
SPHYGMUS_KEY2_Pin         → PC13        SPHYGMUS_KEY1_Pin          → PC14
MAGNETIC_STAT_Pin         → PC1         SPHYGMUS_CTRL2_Pin         → PC3
SPHYGMUS_CTRL1_Pin        → PC2         SHAKE_DOUT1_Pin            → PB9
SHAKE_DOUT0_Pin           → PB8
```

> **注意**：OLED 眼睛显示使用软件（位脉冲）I2C，引脚为 PC10（SDA）/ PC11（SCL），定义在 `bsp_oled_eye.c`（非 CubeMX 生成）。
> RS485 方向控制引脚（`rs485_inst_t` 结构体中的 `pin` 字段）在运行时通过串口设备创建进行配置。

---

*文档基于 CubeMX `main.h` 引脚定义与应用源码分析生成。*
