# CPR Assessment Training Machine — Hardware Interface Document

> **Document Version**: v1.0  
> **Date**: 2026-05-20  
> **Scope**: Three-board system (Mainboard / Remote / Sensor) hardware pinout, peripheral connections, and nRF24L01 wireless communication topology.

---

## Table of Contents

1. [System Overview](#1-system-overview)
2. [Mainboard Hardware Interface](#2-mainboard-hardware-interface)
3. [Remote Hardware Interface](#3-remote-hardware-interface)
4. [Sensor Hardware Interface](#4-sensor-hardware-interface)
5. [nRF24L01 Wireless Communication](#5-nrf24l01-wireless-communication)
6. [Inter-Board Connection Summary](#6-inter-board-connection-summary)
7. [Appendix: CubeMX Pin Macro Reference](#7-appendix-cubemx-pin-macro-reference)

---

## 1. System Overview

The CPR Assessment Training Machine consists of **three independent STM32F103-based boards** communicating over a **star-topology nRF24L01 2.4 GHz wireless network**.

### Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        MAINBOARD (Central)                      │
│  STM32F103ZE  |  RT-Thread OS  |  nRF24L01 Device ID: —        │
│  Roles: Display + Audio + Print + LED Panel + Wired Check       │
│  Targets: REMOTE_ID=0x0004, SENSOR_ID=0x0005                   │
└────────────┬────────────────────────────────┬───────────────────┘
             │  nRF24L01 (2.4 GHz)            │
     ┌───────▼────────┐              ┌────────▼──────────┐
     │    REMOTE        │              │     SENSOR         │
     │  STM32F103RF     │              │  STM32F103RC       │
     │  RT-Thread+LVGL  │              │  RT-Thread OS      │
     │  Device ID:0x0004│              │  Device ID:0x0005  │
     │  TFT Touch + Key │              │  Pressure + Breath │
     │  + Battery       │              │  + Posture + Tests │
     └──────────────────┘              └────────────────────┘
```

### Board Responsibilities

| Board | Role | Core Function |
|-------|------|---------------|
| **Mainboard** | Central Controller | LED panel display, voice prompts (WT588D), thermal printing (RD-DM32), FAL flash storage, wired connection check, RS232/RS485 wired communication |
| **Remote** | User Control Terminal | 3.2" TFT LCD (ST7789) + Capacitive Touch (FT6336U) + 3×3 Matrix Keypad, mode/parameter setting, battery-powered with charging management |
| **Sensor** | Manikin Sensor Hub | Press depth detection (raster board via UART2), breath/blow sensing (UART3), posture monitoring (MPU6050), 8-ch external ADC (ADC128S102), foreign object detection (CC6201 Hall), consciousness judgment (piezo), pulse simulation (coreless motor), WS2812B LED strip, OLED eye display |

---

## 2. Mainboard Hardware Interface

### 2.1 MCU Parameters

| Parameter | Value |
|-----------|-------|
| **Chip** | STM32F103ZET6 |
| **Core** | ARM Cortex-M3 |
| **Max Frequency** | 72 MHz |
| **Flash** | 512 KB |
| **SRAM** | 64 KB |
| **Package** | LQFP-144 |
| **HSE** | 8 MHz |
| **OS** | RT-Thread |

### 2.2 Pin Assignment Table

#### Communication Interfaces

| Peripheral | Bus | Pins | DMA / Notes |
|------------|-----|------|-------------|
| **UART1 (Debug)** | USART1 | TX=PA9, RX=PA10 | Debug console, 115200 baud |
| **UART3 (RS232)** | USART3 | TX=PB10, RX=PB11 | DMA RX/TX, connected to RS232 external device |
| **UART4 (RS485)** | UART4 | TX=PC10, RX=PC11 | DMA RX/TX (hdma_uart4_rx, hdma_uart4_tx), connected to RS485 bus |
| **SPI2 (nRF24L01)** | SPI2 | SCK,MOSI,MISO via CubeMX | nRF24L01 wireless communication |
| **nRF24L01 CE** | GPIO | PD8 | Chip Enable (defined as `nRF24L01_CE_Pin`) |
| **nRF24L01 CSN** | GPIO | PB12 | SPI Chip Select (defined as `nRF24L01_CSN_Pin`) |
| **nRF24L01 IRQ** | GPIO | PD9 | Interrupt (defined as `nRF24L01_IRQ_Pin`) |

#### LED Panel Indicators (all GPIO, CubeMX generated)

| LED Name | Function | Port | Pin Macro |
|----------|----------|------|-----------|
| Debug | Debug indicator | PE1 | `LED_DEBUG_Pin` |
| Conscious Judgment | 意识判断 indicator | PF1 | `LED_CONSCIOUS_JUDGMENT_Pin` |
| Sphygmoscopy | 脉搏检查 indicator | PF0 | `LED_SPHYGMOSCOPY_Pin` |
| Check Breath | 呼吸检查 indicator | PC15 | `LED_CHECK_BREATH_Pin` |
| Emergency Call | 急救呼叫 indicator | PC14 | `LED_EMERGENCY_CALL_Pin` |
| Remove Foreign | 清除异物 indicator | PC13 | `LED_REMOVE_FOREIGN_Pin` |
| Body1 | Press position 1 | PD7 | `LED_BODY1_Pin` |
| Body2 | Press position 2 | PG9 | `LED_BODY2_Pin` |
| Body3 | Press position 3 | PG10 | `LED_BODY3_Pin` |
| Body4 | Press position 4 | PG11 | `LED_BODY4_Pin` |
| Body5 | Press position 5 | PG12 | `LED_BODY5_Pin` |
| Body6 | Press position 6 | PG13 | `LED_BODY6_Pin` |
| Body7 | Press position 7 | PG14 | `LED_BODY7_Pin` |
| Reset | Reset indicator | PD12 | `LED_RESET_Pin` |
| Competition | 竞赛 mode indicator | PG7 | `LED_COMPETITION_Pin` |
| Assess | 考核 mode indicator | PE9 | `LED_ASSESS_Pin` |
| Train | 训练 mode indicator | PG8 | `LED_TRAIN_Pin` |
| Minus | 减号 indicator | PE12 | `LED_MINUS_Pin` |
| Plus | 加号 indicator | PG5 | `LED_PLUS_Pin` |
| Setting | 设置 indicator | PD13 | `LED_SETTING_Pin` |
| Printer | Print status indicator | PD10 | `LED_PRINTER_Pin` |
| Start | Start indicator | PE10 | `LED_START_Pin` |

> **Total LED count**: 22 (defined as `LED_NUM=30` in `bsp_led.h`, includes reserved slots)

#### WT588D Voice Module (3-wire SPI-like, GPIO)

| Signal | Port | Pin Macro |
|--------|------|-----------|
| DATA | PC7 | `WT588D_DATA_Pin` |
| CS | PC8 | `WT588D_CS_Pin` |
| CLK | PC9 | `WT588D_CLK_Pin` |
| RESET | PD3 | `WT588D_RESET_Pin` |
| BUSY | PD4 | `WT588D_BUSY_Pin` |

#### RD-DM32 Thermal Printer

| Signal | Port | Pin Macro | Notes |
|--------|------|-----------|-------|
| CTS | PC12 | `PRINTER_CTS_Pin` | Printer busy/ready flow control (active low) |

#### TM1629A Nixie Tube Drivers (2 channels)

| Channel | DIO | CLK | STB |
|---------|-----|-----|-----|
| Channel A | PE13 (`TM1629A_A_DIO_Pin`) | PE14 (`TM1629A_A_CLK_Pin`) | PE15 (`TM1629A_A_STB_Pin`) |
| Channel B | PA4 (`TM1629A_B_DIO_Pin`) | PA5 (`TM1629A_B_CLK_Pin`) | PA6 (`TM1629A_B_STB_Pin`) |

#### TM1638 LED Driver

| Signal | Port | Pin Macro |
|--------|------|-----------|
| DIO | PD0 | `TM1638_DIO_Pin` |
| CLK | PD1 | `TM1638_CLK_Pin` |
| STB | PD2 | `TM1638_STB_Pin` |

#### Touch Inputs (Capacitive touch buttons on manikin panel)

| Input | Port | Pin Macro |
|-------|------|-----------|
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

#### Wired Connection Detection

| Signal | Port | Pin Macro | Notes |
|--------|------|-----------|-------|
| WIRED_CONNECT_CHECK | PC6 | `WIRED_CONNECT_CHECK_Pin` | Detects when manikin is wired to mainboard |

### 2.3 Peripheral Summary

| Peripheral | Interface | Purpose |
|------------|-----------|---------|
| **WT588D** | 3-wire GPIO (PC7/PC8/PC9) + RST(PD3) + BUSY(PD4) | Voice/audio prompts and feedback |
| **RD-DM32** | UART? (CTS on PC12) | Thermal printer for score/results output |
| **FAL Flash** | On-chip Flash via FAL | Persistent storage of records and configuration |
| **TM1629A ×2** | 3-wire GPIO | Dual-channel nixie tube display drivers |
| **TM1638** | 3-wire GPIO (PD0/PD1/PD2) | LED + key scan driver IC |
| **RS232** | UART3 (PB10/PB11) with DMA | Wired communication to external RS232 devices |
| **RS485** | UART4 (PC10/PC11) with DMA | Wired communication to external RS485 devices |

---

## 3. Remote Hardware Interface

### 3.1 MCU Parameters

| Parameter | Value |
|-----------|-------|
| **Chip** | STM32F103RFT6 |
| **Core** | ARM Cortex-M3 |
| **Max Frequency** | 72 MHz |
| **Flash** | 768 KB |
| **SRAM** | 96 KB |
| **Package** | LQFP-64 |
| **HSE** | 8 MHz |
| **OS + GUI** | RT-Thread + LVGL |

### 3.2 Pin Assignment Table

#### Communication Interfaces

| Peripheral | Bus | Pins | Notes |
|------------|-----|------|-------|
| **UART1** | USART1 | TX=PA9, RX=PA10 | Debug/serial console |
| **SPI1 (TFT LCD)** | SPI1 | SCK,MOSI,MISO via CubeMX | ST7789 TFT display (240×320) |
| **SPI3 (nRF24L01)** | SPI3 | SCK,MOSI,MISO via CubeMX | nRF24L01 wireless module |
| **I2C1 (Touch)** | I2C1 | SCL=PB6, SDA=PB7 | FT6336U capacitive touch controller |
| **ADC1 (Battery)** | ADC1 | BAT_VOL=PA1 | Battery voltage monitoring |

#### ST7789 TFT LCD Control Pins

| Signal | Port | Pin Macro | Function |
|--------|------|-----------|----------|
| CS | PB0 | `LCD_CS_Pin` | SPI Chip Select |
| RST | PC5 | `LCD_RST_Pin` | Hardware Reset |
| BLK | PB15 | `LCD_BLK_Pin` | Backlight control (PWM-capable) |
| DC | PC4 | `LCD_DC_Pin` | Data/Command select |

#### FT6336U Capacitive Touch

| Signal | Port | Pin Macro | Function |
|--------|------|-----------|----------|
| I2C SCL | PB6 | — | Hardware I2C1 clock |
| I2C SDA | PB7 | — | Hardware I2C1 data |
| INT (IRQ) | PA6 | `TOUCH_INT_Pin` | Touch interrupt (active low) |
| RST | PA4 | `TOUCH_RST_Pin` | Touch controller reset |

#### nRF24L01 Wireless Module (SPI3)

| Signal | Port | Pin Macro | Function |
|--------|------|-----------|----------|
| CE | PB11 | `nRF24_CE_Pin` | Chip Enable |
| CSN | PA15 | `nRF24_CSN_Pin` | SPI Chip Select (NSS) |
| IRQ | PB10 | `nRF24_IRQ_Pin` | Interrupt (EXTI15_10_IRQn) |
| SPI Bus | SPI3 | SCK,MOSI,MISO | Hardware SPI3 |

#### 3×3 Matrix Keypad

| Row/Col | Port | Pin Macro |
|---------|------|-----------|
| Row 1 | PB12 | `Matrixkey_Row1_Pin` |
| Row 2 | PB13 | `Matrixkey_Row2_Pin` |
| Row 3 | PB14 | `Matrixkey_Row3_Pin` |
| Column 1 | PC6 | `Matrixkey_Column1_Pin` |
| Column 2 | PC7 | `Matrixkey_Column2_Pin` |
| Column 3 | PC8 | `Matrixkey_Column3_Pin` |

#### Battery Management

| Signal | Port | Pin Macro | Function |
|--------|------|-----------|----------|
| BAT_PROG | PA0 | `BAT_PROG_Pin` | Charge current programming |
| BAT_VOL | PA1 | `BAT_VOL_Pin` | Battery voltage ADC input |
| BAT_CHARG | PA3 | `BAT_CHARG_Pin` | Charge status detection |
| BAT_STDBY | PC2 | `BAT_STDBY_Pin` | Standby/charge complete |
| BAT_EN | PC3 | `BAT_EN_Pin` | Charge enable control |

> Reference voltage: 3.3V, ADC resolution: 12-bit, RPROG=2kΩ

#### Status LED

| LED | Port | Pin Macro | Function |
|-----|------|-----------|----------|
| Green | PC0 | `LED_GREEN_Pin` | Power/status indicator |

### 3.3 Peripheral Summary

| Peripheral | Interface | Purpose |
|------------|-----------|---------|
| **ST7789 TFT** | SPI1 + GPIO (PB0/PC5/PB15/PC4) | 240×320 color LCD display |
| **FT6336U** | I2C1 (PB6/PB7) + INT(PA6) + RST(PA4) | Capacitive touch input |
| **nRF24L01** | SPI3 + GPIO (PB11/PA15/PB10) | 2.4 GHz wireless communication |
| **3×3 Matrix Keypad** | GPIO (PB12-14, PC6-8) | Physical button input |
| **Battery Management** | ADC(PA1) + GPIO(PA0/PA3/PC2/PC3) | Li-Po charge/discharge management |

---

## 4. Sensor Hardware Interface

### 4.1 MCU Parameters

| Parameter | Value |
|-----------|-------|
| **Chip** | STM32F103RCT6 |
| **Core** | ARM Cortex-M3 |
| **Max Frequency** | 72 MHz |
| **Flash** | 256 KB |
| **SRAM** | 48 KB |
| **Package** | LQFP-64 |
| **HSE** | 8 MHz |
| **OS** | RT-Thread |

### 4.2 Pin Assignment Table

#### Communication Interfaces

| Peripheral | Bus | Pins | Notes |
|------------|-----|------|-------|
| **UART1 (Debug)** | USART1 | TX=PA9, RX=PA10 | Debug console |
| **UART2 (Raster Board)** | USART2 | TX=PA2, RX=PA3 | Raster board protocol: press depth / blow detection |
| **UART3 (Protocol)** | USART3 | TX=PB10, RX=PB11 | General protocol communication |
| **SPI1 (nRF24L01)** | SPI1 | SCK,MOSI,MISO via CubeMX | nRF24L01 wireless module |
| **SPI2 (WS2812B)** | SPI2 | SCK,MOSI via CubeMX | WS2812B LED strip (77 LEDs, SPI method) |
| **SPI3 (ADC128S102)** | SPI3 | SCK,MOSI,MISO via CubeMX | 8-channel external ADC |
| **I2C1 (MPU6050)** | I2C1 | SCL=PB0, SDA=PC5 | 6-axis IMU (accelerometer + gyroscope) |
| **ADC1** | ADC1 | On-board ADC channels | On-board analog sensing |

#### nRF24L01 Wireless Module (SPI1)

| Signal | Port | Pin Macro | Function |
|--------|------|-----------|----------|
| CE | PB7 | `nRF24_CE_Pin` | Chip Enable (rt_pin API) |
| CSN | PB6 | `nRF24_CSN_Pin` | SPI Chip Select (NSS) |
| IRQ | PD2 | `nRF24_IRQ_Pin` | Interrupt |

#### ADC128S102 External ADC (SPI3)

| Signal | Port | Pin Macro | Function |
|--------|------|-----------|----------|
| CS | PA4 | `SPI1_NSS_Pin` | SPI Chip Select |
| Bus | SPI3 | SCK,MOSI,MISO | Hardware SPI3 |

> 8-channel, 12-bit SAR ADC for external analog signal acquisition.

#### WS2812B LED Strip (SPI2)

| Signal | Port | Pin Macro | Function |
|--------|------|-----------|----------|
| NSS | PB14 | `SPI2_NSS_Pin` | SPI-based LED data output |
| Bus | SPI2 | MOSI | Hardware SPI2 (SPI method mode) |

> **77 LEDs** (`WS2812B_LED_NUMS=77`), SPI bit-bang encoding: 0-code=0xC0, 1-code=0xF0.

#### MPU6050 6-axis IMU (I2C1)

| Signal | Port | Function |
|--------|------|----------|
| I2C SCL | PB0 | Hardware I2C1 clock |
| I2C SDA | PC5 | Hardware I2C1 data |

#### OLED Eye Display (Software I2C)

| Signal | Port | Pin | Function |
|--------|------|-----|----------|
| SDA | PC10 | GPIO | Bit-bang I2C data |
| SCL | PC11 | GPIO | Bit-bang I2C clock |

> Display: ST7315 driver, 64×48 pixels, I2C address 0x3C, 6 pages.

#### Pulse Simulation (Coreless Motor) & Peripheral GPIO

| Signal | Port | Pin Macro | Function |
|--------|------|-----------|----------|
| SPHYGMUS_CTRL1 | PC2 | `SPHYGMUS_CTRL1_Pin` | Motor 1 control |
| SPHYGMUS_CTRL2 | PC3 | `SPHYGMUS_CTRL2_Pin` | Motor 2 control |
| SPHYGMUS_KEY1 | PC14 | `SPHYGMUS_KEY1_Pin` | Motor key input 1 |
| SPHYGMUS_KEY2 | PC13 | `SPHYGMUS_KEY2_Pin` | Motor key input 2 |

#### Foreign Object Detection & Consciousness Judgment

| Signal | Port | Pin Macro | Function |
|--------|------|-----------|----------|
| MAGNETIC_STAT | PC1 | `MAGNETIC_STAT_Pin` | CC6201 Hall sensor status (foreign object detection) |
| SHAKE_DOUT0 | PB8 | `SHAKE_DOUT0_Pin` | Piezoelectric ceramic data 0 (consciousness judgment) |
| SHAKE_DOUT1 | PB9 | `SHAKE_DOUT1_Pin` | Piezoelectric ceramic data 1 (consciousness judgment) |

#### Debug LED

| LED | Port | Pin Macro | Function |
|-----|------|-----------|----------|
| DEBUG_LED | PA15 | `DEBUG_LED_Pin` | General debug indicator |

### 4.3 Peripheral Summary

| Peripheral | Interface | Purpose |
|------------|-----------|---------|
| **nRF24L01** | SPI1 + GPIO (PB7/PB6/PD2) | Wireless data transmission to Mainboard |
| **Raster Board** | UART2 (PA2/PA3) | Press depth / blow detection via raster sensor protocol |
| **Protocol Device** | UART3 (PB10/PB11) | External protocol device communication |
| **MPU6050** | I2C1 (PB0/PC5) | 6-axis posture/angle monitoring |
| **ADC128S102** | SPI3 + CS(PA4) | 8-channel external analog acquisition |
| **WS2812B** | SPI2 + NSS(PB14) | 77-LED strip for visual feedback |
| **OLED Eye** | Soft I2C (PC10/PC11) | 64×48 OLED eye expression display |
| **Coreless Motor ×2** | GPIO (PC2/PC3/PC13/PC14) | Pulse simulation (sphygmoscopy) |
| **CC6201 Hall Sensor** | GPIO (PC1) | Foreign object (metal) detection |
| **Piezo Ceramic** | GPIO (PB8/PB9) | Consciousness judgment (tap/vibration detection) |
| **RS485** | Serial (Reuses UART) | Wired RS485 communication with external devices |

---

## 5. nRF24L01 Wireless Communication

### 5.1 RF Parameters

| Parameter | Value | Notes |
|-----------|-------|-------|
| **Frequency Band** | 2.400 – 2.525 GHz | ISM band, channel selectable via `RF_CH` register |
| **Data Rate** | 1 Mbps (ADR_1Mbps) | Extends range, good interference immunity |
| **TX Power** | 0 dBm (RF_POWER_0dBm) | Maximum transmit power |
| **CRC** | 2 Bytes (CRC_2_BYTE) | Enhanced error detection |
| **Address Width** | 5 Bytes | Full 5-byte pipe addresses |
| **Dynamic Payload** | Enabled (`EN_DPL`) | Variable-length payload support |
| **Auto Acknowledgment** | Pipe 0 / Pipe 1 | Configurable per pipe |
| **Auto Retransmit** | Configurable (ARC + ARD) | Hardware-level retry |

### 5.2 Device ID & Pipe Assignment

| Board | nRF24L01 Role | Device ID | Pipe Assignment |
|-------|--------------|-----------|-----------------|
| **Mainboard** | PRX (Primary Receiver) | — (Central Hub) | Pipe 0: RX from Remote; Pipe 1: RX from Sensor |
| **Remote** | PTX (Primary Transmitter) | `0x0004` | Pipe 0: TX to Mainboard |
| **Sensor** | PTX (Primary Transmitter) | `0x0005` | Pipe 0: TX to Mainboard |

> **Communication model**: Remote and Sensor initiate transmissions independently; Mainboard listens on both pipes and responds via ACK payload or separate TX. Mainboard encodes target device IDs (`REMOTE_ID=0x0004`, `SENSOR_ID=0x0005`) in the protocol frame for source/destination identification.

### 5.3 Frame Format (CPR Protocol)

The application-layer protocol (`cpr_packet_t`) is encapsulated in nRF24L01 payloads:

```
┌────────┬────────┬─────┬──────────┬─────┬────────┬──────┬─────────────────────┬──────────┐
│ head1  │ head2  │ len │ dev_type │ cmd │ status │ seq  │  payload (max 24B)  │ crc16    │
│ 1B     │ 1B     │ 1B  │ 1B       │ 1B  │ 1B     │ 2B   │  0~24 B             │ 2B       │
│ 0x55   │ 0xAA   │     │          │     │        │      │                     │ (Modbus) │
└────────┴────────┴─────┴──────────┴─────┴────────┴──────┴─────────────────────┴──────────┘
```

| Field | Size | Description |
|-------|------|-------------|
| `head1` | 1 B | Frame sync byte 1: `0x55` |
| `head2` | 1 B | Frame sync byte 2: `0xAA` |
| `len` | 1 B | Length from `dev_type` to before `crc` |
| `dev_type` | 1 B | Source device type: `0x01`=Mainboard, `0x02`=Sensor, `0x03`=Remote |
| `cmd` | 1 B | Command code (see below) |
| `status` | 1 B | Status: ASK / ACK / ERR |
| `seq` | 2 B | Sequence number (anti-replay) |
| `payload` | 0–24 B | Variable-length data (max 24) |
| `crc` | 2 B | CRC16-Modbus |

#### Command Codes

| Code | Name | Direction | Purpose |
|------|------|-----------|---------|
| `0x01` | `CMD_ASK_CONNECT` | Sensor/Remote → Mainboard | Connection request |
| `0x02` | `CMD_ACK_CONNECT` | Mainboard → Device | Connection acknowledgment |
| `0x10` | `CMD_SENSOR_DATA` | Sensor → Mainboard | Press/breath/angle data |
| `0x20` | `CMD_REMOTE_CMD` | Remote → Mainboard | Mode/parameter/start commands |
| `0x30` | `CMD_DISPLAY_FEEDBACK` | Mainboard → Remote | LED status, score feedback |
| `0x40` | `CMD_HEARTBEAT` | Bidirectional | Keep-alive heartbeat |
| `0x50` | `CMD_MODE_IN` | — | Enter training/assessment/competition |
| `0x51` | `CMD_MODE_OUT` | — | Exit mode |
| `0x60` | `CMD_PRESS_LED_CTRL` | Remote/Mainboard | Press position LED control |

### 5.4 SPI Pin Summary (per board)

| Board | SPI Bus | CE | CSN (NSS) | IRQ |
|-------|---------|-----|-----------|-----|
| Mainboard | SPI2 | PD8 | PB12 | PD9 |
| Remote | SPI3 | PB11 | PA15 | PB10 (EXTI15_10) |
| Sensor | SPI1 | PB7 | PB6 | PD2 |

---

## 6. Inter-Board Connection Summary

### 6.1 Wireless Connections

| From | To | Medium | Protocol |
|------|----|--------|----------|
| Remote (0x0004) | Mainboard | nRF24L01 @ 2.4 GHz | CPR Protocol (CMD_REMOTE_CMD, etc.) |
| Sensor (0x0005) | Mainboard | nRF24L01 @ 2.4 GHz | CPR Protocol (CMD_SENSOR_DATA, etc.) |
| Mainboard | Remote | nRF24L01 @ 2.4 GHz | CPR Protocol (CMD_DISPLAY_FEEDBACK) |
| Mainboard | Sensor | nRF24L01 @ 2.4 GHz | CPR Protocol (CMD_ACK_CONNECT, etc.) |

### 6.2 Wired Connections

| Board | Interface | Target Device | Protocol |
|-------|-----------|---------------|----------|
| Sensor → Raster Board | UART2 (PA2/PA3) | Raster sensor board | Binary raster protocol (press/blow data) |
| Sensor → External Device | UART3 (PB10/PB11) | TBD protocol device | General serial protocol |
| Mainboard → External RS232 Device | UART3 (PB10/PB11) | RS232 peripheral | DMA-based serial |
| Mainboard → External RS485 Device | UART4 (PC10/PC11) | RS485 peripheral | DMA-based serial, half-duplex |
| Mainboard → Manikin (wired check) | GPIO (PC6) | Manikin detection pin | Digital input (wired connection detect) |

### 6.3 Power Topology

| Board | Power Source | Notes |
|-------|-------------|-------|
| **Mainboard** | DC input / external PSU | Full-time powered |
| **Remote** | Li-Po battery + charging circuit | ADC voltage monitoring, charge management |
| **Sensor** | External PSU via harness | Powered through manikin cabling |

---

## 7. Appendix: CubeMX Pin Macro Reference

### 7.1 Mainboard (`cpr-display-main-board/cubemx/Inc/main.h`)

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

### 7.2 Remote (`cpr-remote-device/cubemx/Inc/main.h`)

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

### 7.3 Sensor (`cpr-sensor-board/cubemx/Inc/main.h`)

```
SPI1_NSS_Pin              → PA4         SPI2_NSS_Pin               → PB14
DEBUG_LED_Pin             → PA15        nRF24_IRQ_Pin              → PD2
nRF24_CSN_Pin             → PB6         nRF24_CE_Pin               → PB7
SPHYGMUS_KEY2_Pin         → PC13        SPHYGMUS_KEY1_Pin          → PC14
MAGNETIC_STAT_Pin         → PC1         SPHYGMUS_CTRL2_Pin         → PC3
SPHYGMUS_CTRL1_Pin        → PC2         SHAKE_DOUT1_Pin            → PB9
SHAKE_DOUT0_Pin           → PB8
```

> **Note**: OLED eye uses software (bit-bang) I2C on PC10 (SDA) / PC11 (SCL), defined in `bsp_oled_eye.c` (not CubeMX-generated).
> RS485 direction control pin (`pin` field in `rs485_inst_t`) is configured at runtime via serial device creation.

---

*Document generated from CubeMX `main.h` pin defines and application source code analysis.*
