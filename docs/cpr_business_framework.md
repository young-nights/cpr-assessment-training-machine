# CPR 心肺复苏训练考核系统 - 业务框架设计文档

> **版本**:v2.1
> **日期**:2026-05-12
> **作者**:coder
> **关联文档**:[业务需求.md](./业务需求.md)(各板业务流程规范)
> **项目仓库**:`cpr-assessment-training-machine`

---

## 目录

1. [系统概述](#1-系统概述)
2. [硬件架构总览](#2-硬件架构总览)
3. [各板功能职责划分](#3-各板功能职责划分)
4. [通信拓扑与交互链路](#4-通信拓扑与交互链路)
5. [通信协议设计](#5-通信协议设计)
    - 5.1 nRF24L01 无线协议
    - 5.2 Raster 板串口协议(实时数据帧)
    - 5.3 Raster 板串口协议(成绩上报帧)
    - 5.4 RS485 有线协议
6. [核心业务流程](#6-核心业务流程)
    - 6.1 系统启动与连接建立
    - 6.2 正常训练/考核流程
    - 6.3 数据流详解
    - 6.4 考核模式操作流程检查
7. [线程架构与任务调度](#7-线程架构与任务调度)
8. [错误处理与容错机制](#8-错误处理与容错机制)
9. [有线/无线切换机制](#9-有线无线切换机制)
10. [扩展性与维护性](#10-扩展性与维护性)
11. [附录:关键数据结构](#11-附录关键数据结构)
    - 11.5 三种模式成绩单打印格式

---

## 1. 系统概述

CPR(Cardiopulmonary Resuscitation)心肺复苏训练考核系统是一套用于医学教育的嵌入式多板协同系统,模拟人体心肺复苏操作场景,支持**训练、考核、竞赛**三种工作模式。

系统由 **4 块独立 PCB 板** 组成,通过 **nRF24L01 无线** 和 **串口有线** 双通道进行数据交互,实现按压深度检测、吹气量检测、异物检测、瞳孔模拟、脉搏模拟等完整 CPR 操作流程。

### 1.1 系统工作模式

| 模式 | 说明 | 是否评分 | 是否打印 |
|------|------|----------|----------|
| **训练模式** | 自由练习,实时反馈 | 否 | 可选 |
| **考核模式** | 按标准达标率评分 | 是 | 是 |
| **竞赛模式** | 限时竞技排名 | 是 | 是 |

---

## 2. 硬件架构总览

```
┌─────────────────────────────────────────────────────────────────────┐
│                        CPR 系统硬件拓扑                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   ┌──────────────┐         nRF24L01 (Pipe1)        ┌────────────┐   │
│   │  Sensor 板   │◄──────────────────────────────►│            │   │
│   │  (STM32)     │                                 │  Mainboard │   │
│   │              │── UART2 (115200)──►┌──────────┐ │  板(STM32) │   │
│   │              │                   │ Raster 板│ │            │   │
│   │              │◄── UART2 ──────────│ (STM8)   │ │            │   │
│   └──────────────┘                   └──────────┘ │            │   │
│         │           RS485 (有线备用)               │            │   │
│         └──────────────────────────────────────────►│            │   │
│                                                    │            │   │
│   ┌──────────────┐         nRF24L01 (Pipe2)        │            │   │
│   │  Remote 板   │◄──────────────────────────────►│            │   │
│   │  (STM32)     │                                 │            │   │
│   │  LVGL GUI    │                                 │            │   │
│   └──────────────┘                                 └────────────┘   │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 2.1 芯片平台

| 板卡 | MCU | OS | 通信接口 |
|------|-----|-----|---------|
| **Sensor 板** | STM32 (Cortex-M) | RT-Thread | nRF24L01 (SPI), UART2/UART3, RS485, I2C |
| **Mainboard 板** | STM32 (Cortex-M) | RT-Thread | nRF24L01 (SPI), RS232, RS485, SPI (TM1629A) |
| **Remote 板** | STM32 (Cortex-M) | RT-Thread + LVGL | nRF24L01 (SPI), I2C (FT6336U), SPI (ST7789) |
| **Raster 板** | STM8 | 裸机 (Bare-metal) | UART1 |

---

## 3. 各板功能职责划分

> 各板详细业务流程规范(状态机、控制时序、外设矩阵)参见 [业务需求.md](./业务需求.md)。

### 3.1 Sensor 板(传感器板)

**定位**:CPR 模拟人体上的传感器数据采集与执行控制单元,负责采集按压、吹气等关键生理参数。

**核心职责**:
- **按压深度测量**:通过 UART2 接收外挂 Raster 光栅板的光编码器脉冲数计算按压深度
- **潮气量测量**:通过 Raster 光栅板的光编码器脉冲数计算潮气量
- **肩部震动检测**:STM32 内置 ADC 测量压电陶瓷片是否触发,检测施救者肩部拍打动作
- **按压点位检测**:ADC128S102CIMTX 采集薄膜压力传感器,判断按压位置(上/中/下/左/右五个方位 + 气道开启 LED + 吹气进胃 LED)
- **头部上仰检测**:MPU6050 安置在模拟人头部,测量吹气时头部上仰角度
- **光栅数据接收**:UART2 连接光栅板,接收按压深度、按压频率、有效/无效按压次数、潮气深度、吹气参数等数据
- **眼部状态显示**:OLED (0.66寸, PC10/PC11 I2C) + WS2812B (PA11 PWM) 协同控制眼睛状态(濒死: WS2812B最低亮度+OLED黑屏; 正常: WS2812B白灯全亮+OLED白底黑圆)
- **颈动脉模拟**:空心杯电机控制(颈动脉脉搏模拟,参见下方状态机)
- **异物检测**:CC6201 霍尔传感器检测异物
- **无线通信**:nRF24L01 与 Mainboard 通信
- **有线备用**:RS485 与 Mainboard 有线通信

**关键外设**:

| 外设 | 用途 | 接口 |
|------|------|------|
| STM32 内置 ADC | 肩部震动检测(压电陶瓷片) | 片内 ADC |
| ADC128S102CIMTX | 按压点位检测(薄膜压力传感器,5方位+2LED) | SPI |
| MPU6050 | 头部上仰角度检测(吹气时) | I2C |
| nRF24L01 | 无线通信(与 Mainboard) | SPI |
| UART2 | 光栅板数据接收(按压/吹气参数) | 串口 115200 |
| RS485 | 有线通信备用(与 Mainboard) | UART |
| WS2812B | 眼灯白光控制(PA11) | PWM |
| OLED (0.66寸) | 眼部屏幕(PC10/PC11) | I2C |
| 空心杯电机 | 颈动脉脉搏模拟 | PWM |
| CC6201 | 异物检测 | GPIO |

**Sensor 板业务流程总览**:

```
上电初始化 → 连接握手 → 接收开始指令 → 状态初始化(濒死)
    → 前置检查(意识/脉搏/异物/呼吸/急救呼叫)
    → 按压数据采集(触点检测+深度接收+频率计算)
    → 吹气数据采集(潮气量+头部角度)
    → 停止 → 数据上报 → 复位
```

详细流程参见 [业务需求.md §1](./业务需求.md#1-sensor-板业务流程规范)。

**颈动脉模拟的三个临床状态**:

| 训练阶段 | 颈动脉状态 | 搏动性质 | 模拟的临床意义 |
|----------|-----------|---------|---------------|
| 初始化状态(未开始按压) | 无搏动 | 无跳动 | 心脏停跳,血液循环停止 |
| 按压过程中 | 被动搏动 | 搏动频率与按压频率一致 | 按压产生的人工血液循环 |
| 抢救成功后 | 自主搏动 | 恢复规律的自主跳动 | 自主循环恢复(ROSC),患者获救 |

详细状态机参见 [业务需求.md §1.5](./业务需求.md#15-颈动脉模拟状态机)。

**三路信号联合判别模块（新增于 2026-04-29）**:

当光电传感器(ST130B)检测到栅格条运动时，Sensor 板同步读取两路辅助信号进行联合判别：

| 辅助信号 | 传感器 | 按压特征 | 吹气特征 |
|----------|--------|---------|---------|
| 压力检测 | ADC128S102CIMTX (SPI) | **有压力**（超过阈值） | 无压力 |
| 头部姿态 | MPU6050 (I2C) | 头部基本不动 | **pitch ≥ +15°** |

**判别逻辑**：
- 压力有值 → 按压（发送 activate_pressure 给光栅板）
- 无压力 + MPU6050 上仰确认 → 吹气（发送 idle_pressure + activate_blow）
- 两路均无 → 丢弃（栅格回弹抖动）
- 两路均有 → 按压优先（压力信号更可靠）

**MPU6050 上仰判定参数**：

| 参数 | 默认值 | 说明 |
|------|--------|------|
| pitch 角阈值 | ≥ +15° | 头部上仰角度 |
| pitch 变化速率 | ≥ 30°/s | 排除缓慢晃动 |
| 确认窗口 | 50ms | 光电触发后采样窗口 |
| 持续时间 | ≥ 100ms | 持续超阈值时间 |
| 零位标定 | 启动时自动 | 补偿安装偏差 |

详细判别方案和代码实现参见 [业务需求.md §1.4](./业务需求.md#14-按压吹气三路信号联合判别方案)。

**源码结构**:
```
cpr-sensor-board/applications/
├── macBSP/
│   ├── adc1115idgsr.c/h       # ADC 芯片驱动
│   ├── adc128s102cimtx.c/h    # ADC 芯片驱动
│   ├── bsp_ws2812b.c/h        # WS2812B 眼灯控制 (PA11 PWM)
│   ├── bsp_oled_eye.c/h       # OLED 眼屏控制 (PC10/PC11 I2C, 0.66寸)
│   ├── bsp_rs485_*.c/h        # RS485 有线通信
│   ├── uart2_protocol.c/h     # UART2 光栅板通信
│   └── uart3_protocol.c/h     # UART3 通信
├── macMPU/
│   ├── bsp_mpu6050.c/h        # MPU6050 驱动
│   ├── bsp_mpu6050_calibration.c/h  # 校准
│   └── bsp_mpu6050_euler_angles.c/h # 欧拉角
├── macNRF/
│   ├── sensor_nrf24l01_driver.c/h   # nRF24L01 驱动
│   ├── sensor_nrf24l01_message.c/h  # 协议解析
│   └── sensor_nrf24l01_spi.c/h      # SPI 底层
├── macSYS/
│   ├── bsp_typedef.c/h        # 全局数据结构
│   └── rtt_system_work.c/h    # 系统定时任务
└── macTask/
    ├── sensor_nrf24l01_task.c  # nRF24L01 线程
    └── hardware_task.c         # 硬件控制线程
```

### 3.2 Mainboard 板(主板/显示主控板)

**定位**:系统核心业务逻辑处理中心,负责数据汇聚、评分运算、指令调度和外设控制。

**核心职责**:
- 接收 Sensor 板数据(nRF24L01 Pipe1 / RS485 有线)
- 接收 Remote 板指令(nRF24L01 Pipe2)
- CPR 评分算法执行
- TM1629A 数码管/光条显示驱动
- WT588D 语音播报控制
- DM32 热敏打印机控制
- 触摸按键响应
- 系统模式管理(训练/考核/竞赛)

**关键外设**:

| 外设 | 用途 | 接口 |
|------|------|------|
| nRF24L01 | 无线通信(双 Pipe) | SPI |
| TM1629A | 数码管 + 光条显示 | SPI (软件) |
| WT588D | 语音播报 | UART |
| DM32 Printer | 热敏打印 | UART (RS232) |
| RS485 | 有线通信(连接 Sensor) | UART |
| 触摸按键 | 操作输入 | I2C / GPIO |

**Mainboard 板业务流程总览**:

```
上电初始化 → 变量初始化(默认训练模式) → nRF进入PRX接收
    → 接收Sensor/Remote连接请求 → 完成握手
    → 等待TOUCH_START → 循环发送开始指令给Sensor → 收到ACK后停止循环
    → 进入CPR业务循环 → 评分运算 → 成绩保存 → 打印输出
```

详细流程参见 [业务需求.md §2](./业务需求.md#2-mainboard-板业务流程规范)。

**源码结构**:
```
cpr-display-main-board/applications/
├── macBSP/
│   ├── bsp_rs232_dev.c/h      # RS232 设备通信(打印机)
│   ├── bsp_rs485_*.c/h        # RS485 有线通信
│   ├── bsp_wt588d.c/h         # 语音播报
│   ├── rd_dm32_printer.c/h    # 热敏打印机
│   └── bsp_led.c/h            # LED 控制
├── macNRF/
│   ├── mainboard_nrf24l01_driver.c/h   # nRF24L01 驱动
│   ├── mainboard_nrf24l01_message.c/h  # 协议解析(Pipe 区分)
│   ├── mainboard_cpr_protocol.h        # CPR 统一协议定义
│   └── mainboard_nrf24l01_spi.c/h      # SPI 底层
├── macSYS/
│   ├── bsp_typedef.c/h        # 全局数据结构 (RecordStruct, System_Config_t)
│   ├── cpr_record.c/h         # CPR 记录
│   └── rtt_system_work.c/h    # 系统定时
├── macFlash/
│   └── macFlash.c/h           # Flash 存储
└── macTASK/
    ├── mainboard_nrf24l01_task.c  # nRF24L01 三线程架构
    ├── touch_task.c               # 触摸按键
    ├── nixietube_task.c           # 数码管
    ├── lightbar_task.c            # 光条
    ├── wt588d_task.c              # 语音
    ├── printer_task.c             # 打印
    └── start_task.c               # 状态检查
```

### 3.3 Remote 板(遥控设备)

**定位**:用户操作界面,基于 LVGL 图形库的触摸屏交互终端。

**核心职责**:
- LVGL GUI 渲染(ST7789 LCD 驱动)
- 触摸输入(FT6336U 电容触摸屏)
- 向 Mainboard 发送模式切换/开始/停止等指令
- 接收 Mainboard 反馈数据(按压位置 LED 颜色、成绩等)
- 电池电量 ADC 检测

**关键约束**:
> ⚠️ **Remote 板不与 Sensor 板直接通信**,所有 Sensor 数据经 Mainboard 中转。
> ⚠️ **Remote 板不参与有线连接**,仅通过 nRF24L01 与 Mainboard 通信。

**GUI 页面**:
- `screen_main` - 主页面(模式选择)
- `screen_menu` - 菜单页面
- `screen_operation` - 操作页面(训练/考核进行中)
- `screen_setting` - 设置页面(时间、达标率)
- `screen_data` - 数据页面(按压位置 LED 反馈)

**源码结构**:
```
cpr-remote-device/applications/
├── macBSP/
│   ├── st7789_driver.c/h      # ST7789 LCD 驱动
│   ├── st7789_spi.c/h         # SPI 底层
│   ├── ft6336u_driver.c/h     # 触摸屏驱动
│   ├── ft6336u_iic.c/h        # I2C 底层
│   ├── lcd_driver.c/h         # LCD 抽象层
│   ├── bsp_battery.c/h        # 电池检测
│   ├── bsp_key.c/h            # 按键
│   └── bsp_led.c/h            # LED
├── macNRF/
│   ├── remote_nrf24l01_driver.c/h   # nRF24L01 驱动
│   ├── remote_nrf24l01_message.c/h  # 协议解析
│   └── remote_nrf24l01_spi.c/h      # SPI 底层
├── macGUI/
│   ├── lvgl_gui/              # GUI 主逻辑
│   ├── lvgl_custom/           # 自定义页面
│   ├── lvgl_fonts/            # 字体资源
│   ├── lvgl_images/           # 图片资源
│   └── porting/               # LVGL 移植层
├── macSYS/
│   ├── bsp_typedef.c/h        # 全局结构
│   └── rtt_system_work.c/h    # 系统定时
└── macTask/
    ├── remote_nrf24l01_task.c # nRF24L01 线程
    ├── ft6336u_task.c         # 触摸任务
    └── adc_task.c             # ADC 任务
```

### 3.4 Raster 板(光栅板)

**定位**:高精度按压深度检测单元,基于光栅编码器原理。

**核心职责**:
- 光栅编码器 A/B 相信号中断采集(GPIO EXTI)
- 按压深度计算(脉冲计数 × 每脉冲毫米数)
- 按压方向判断(A/B 相序)
- 纯脉冲数据采集与增量上报
- 通过 UART1 将实时数据和成绩数据发送给 Sensor 板

**平台**:STM8S 裸机(无 RTOS),纯中断驱动。

#### 物理结构

- Raster 光栅板 PCB 两端各有 2 个 ST130B 光电传感器,共 4 个
- PCB 板竖直固定在模拟人体内
- 中间放置栅格条(光栅尺),可沿竖直方向运动
- 按压动作:按压模拟人胸部 → 栅格条向下运动 → 释放后回弹(弹簧恢复)
- 吹气动作:吹气使模拟人胸腔鼓起 → 栅格条向上运动 → 泄气时胸腔塌陷 → 栅格条向下回到初始状态
- 光电传感器通过检测栅格条的运动方向和脉冲数,计算按压深度和潮气量

#### 与 Sensor 板交互

#### 与 Sensor 板交互

1. **实时数据上报**:每 100ms 通过 UART1 发送 8 字节原始脉冲增量帧
   - 帧格式: `0xAA + 0x04 + TYPE + CNT_H + CNT_L + DIR + CHK + 0x55`
   - TYPE: 0x01=按压, 0x02=吹气
   - CNT: int16_t 脉冲增量（自上次发送以来的变化量）
   - DIR: int8_t, -1=回弹/泄气, 0=静止, 1=下压/充气
   - 按压和吹气不同时发生, `direction_blow != 0` 时发吹气帧, 否则发按压帧
2. **停止指令接收**:Sensor 板通过 UART1 发送停止指令, 帧格式 `0xAA + 0x02 + 0x03 + 0xFF + CHK + 0x55`（6字节）,收到后直接返回待机状态
3. **模式切换指令接收**:Sensor 板通过 UART2 发送模式切换指令（CMD=0x11），光栅板收到后切换检测模式（按压/吹气/空闲），影响主循环中 TYPE 字段的发送逻辑。

#### 成绩计算职责边界

**全部成绩计算已移至 Sensor 板**。光栅板不再进行任何计算。

**Sensor 板接管的计算**（原光栅板职责）:
- 脉冲增量 → 累计深度: `depth_mm += pulse_count × MM_PER_PULSE_01`
- 按压/吹气事件检测
- 深度正确性判断（过大/不足）
- 按压频率计算
- 循环计数与统计
- 成绩结构体组装与上报

**Sensor 板原有计算**:
- 按压位置(需 MPU6050)、头部上仰(需 MPU6050)、震动检测(需压电 ADC)、异物检测(需 CC6201)

#### 中断处理流程

**A/B 相中断配置**:
- GPIOC Pin6/7: 吹气光栅编码器（PCB 上端，主负责吹气检测）
- GPIOD Pin3/4: 按压光栅编码器（PCB 下端，主负责按压检测）
- 均配置为 EXTI 双边沿触发（上升沿+下降沿）
- 使用 4 倍频正交解码（查表法），每组 2 个传感器错开一个相位

**中断处理逻辑**:
```c
// Port C 中断处理: 吹气主检测通道 (GPIOC Pin6/7)
void EXTI_PORTC_IRQHandler(void) {
    // 4倍频正交解码,更新 direction_blow / depth_count_blow
    // direction_blow: -1=泄气, 0=静止, 1=充气
}

// Port D 中断处理: 按压主检测通道 (GPIOD Pin3/4)
void EXTI_PORTD_IRQHandler(void) {
    // 4倍频正交解码,更新 direction_press / depth_count_press
    // direction_press: -1=回弹, 0=静止, 1=下压
    // 同时更新兼容旧变量 direction / depth_count
}
```

#### 光栅板状态机

```
                    ┌─────────────────┐
                    │  待机状态 (IDLE) │
                    │  上电初始化完成  │
                    └────────┬────────┘
                             │
                    收到开始指令 (0x01)
                             │
                             ▼
                    ┌─────────────────┐
                    │  采集中 (ACTIVE) │
                    │  中断计数开启    │
                    │  定时上报数据    │
                    └────────┬────────┘
                             │
              ┌──────────────┼──────────────┐
              │              │              │
        下压(1)        静止(0)     充气(1)
              │              │              │
              ▼              ▼              ▼
        ┌──────────┐  ┌──────────┐  ┌──────────┐
        │ 按压累计 │  │ 等待操作 │  │ 吹气累计 │
        │depth_p++ │  │          │  │depth_b++ │
        └────┬─────┘  └──────────┘  └────┬─────┘
             │                           │
        回弹(-1)                    泄气(-1)
             │                           │
             ▼                           ▼
        ┌──────────┐               ┌──────────┐
        │ 按压结束 │               │ 吹气结束 │
        │ 更新统计 │               │ 更新统计 │
        └──────────┘               └──────────┘
                             │
                    收到停止指令 (0x03)
                             │
                             ▼
                    ┌─────────────────┐
                    │  待机状态 (IDLE) │
                    │  (所有成绩计算   │
                    │   由Sensor板完成)│
                    └─────────────────┘
```

#### 错误处理

| 错误类型 | 检测方法 | 处理方式 |
|----------|----------|----------|
| UART发送失败 | 检测发送完成标志 | 重试3次,失败则记录错误 |
| 脉冲计数溢出 | 检测计数器最大值 | 清零并记录警告 |
| 方向判断异常 | 检测连续相同方向脉冲 | 忽略异常脉冲 |
| CRC校验错误 | 接收Sensor指令时校验 | 丢弃该帧,等待重发 |
| 数据帧超时 | 定时器检测 | 重新发送上一帧数据 |

**源码结构**:
```
cpr-raster-board/
├── MDK_APP/
│   ├── app_bsp.c/h            # GPIO 中断配置(A/B 相)
│   ├── app_timming.c/h        # 定时任务
│   ├── app_calculator.c/h     # 成绩计算模块
│   └── app_report.c/h         # 成绩上报模块
├── MDK_System/
│   ├── Inc/
│   │   ├── app_usart.h        # UART1 驱动
│   │   ├── app_message.h      # 消息发送(深度+方向)
│   │   ├── app_general_tim.h  # 定时器
│   │   └── app_sys.h          # 系统辅助(全局变量/状态机)
│   └── Scr/
│       ├── app_usart.c        # UART1 驱动
│       ├── app_message.c      # 消息发送与接收处理
│       ├── app_general_tim.c  # 定时器
│       └── app_sys.c          # 系统辅助
└── MDK_User/
    ├── main.c                 # 主循环(状态机)
    ├── stm8s_it.c             # 中断处理(光栅编码器+TIM1+UART1)
    └── stm8s_it.h
```

---

## 4. 通信拓扑与交互链路

### 4.1 通信链路总览

```
                    ┌─────────────────────────────────────────┐
                    │            Mainboard (主控)              │
                    │                                         │
                    │  ┌─────────┐  ┌─────────┐  ┌────────┐  │
                    │  │nRF Pipe1│  │nRF Pipe2│  │ RS485  │  │
                    │  │ (Sensor)│  │ (Remote)│  │(Wired) │  │
                    │  └────┬────┘  └────┬────┘  └───┬────┘  │
                    └───────┼────────────┼───────────┼───────┘
                            │            │           │
              ┌─────────────┤            │           ├─────────────┐
              │             │            │           │             │
              ▼             ▼            ▼           │             │
     ┌─────────────┐  ┌──────────┐  ┌──────────┐    │             │
     │  Raster 板  │  │ Sensor板 │  │ Remote板 │    │             │
     │  (STM8)     │  │ (STM32)  │  │ (STM32)  │    │             │
     │             │  │          │  │ LVGL GUI │    │             │
     │ UART ───────┼──│ UART2    │  │          │    │             │
     └─────────────┘  │    ▲     │  └──────────┘    │             │
                      │    │     │                   │             │
                      │    └─────┼───────────────────┘             │
                      │   RS485  │  (有线备用)                      │
                      └──────────┘                                 │
                                                                   │
                    ◄── Remote 不与 Sensor 直接通信 ──►            │
                    ◄── Remote 不参与有线连接 ─────────►           │
```

### 4.2 通信链路详细说明

| 链路 | 源 | 目标 | 协议 | Pipe/通道 | 说明 |
|------|-----|------|------|-----------|------|
| **1 Sensor → Mainboard** | Sensor 板 | Mainboard 板 | nRF24L01 | Pipe1 | 主通道:传感器数据上报 |
| **2 Mainboard → Sensor** | Mainboard 板 | Sensor 板 | nRF24L01 | Pipe1 | 反向:控制指令下发 |
| **3 Remote → Mainboard** | Remote 板 | Mainboard 板 | nRF24L01 | Pipe2 | 模式切换、开始/停止指令 |
| **4 Mainboard → Remote** | Mainboard 板 | Remote 板 | nRF24L01 | Pipe2 | 成绩反馈、LED 颜色控制 |
| **5 Raster → Sensor** | Raster 板 | Sensor 板 | UART (115200) | 串口直连 | 光栅原始脉冲增量（每100ms） |
| **6 Sensor ⇄ Mainboard** | 双向 | 双向 | RS485 Modbus | 有线 | 无线故障时的有线备用通道 |
| **7 Sensor → Mainboard (成绩)** | Sensor 板 | Mainboard 板 | nRF24L01 | Pipe1 | 成绩数据（Sensor 板计算） |

### 4.3 nRF24L01 Pipe 分配

主板 nRF24L01 工作在 **PRX(接收)** 模式,通过 Pipe 区分数据来源:

| Pipe | 对应设备 | 地址配置 | 数据方向 |
|------|----------|----------|----------|
| Pipe1 | Sensor 板 | `DEVICE_SENSOR_ID = 0x0005` | Sensor → Mainboard |
| Pipe2 | Remote 板 | `DEVICE_REMOTE_ID = 0x0004` | Remote → Mainboard |
| Pipe0 | 回环/自用 | 默认地址 | PTX 发送时使用 |

> 主板 nRF24L01 以 PRX 模式接收,需要发送时切换为 PTX 模式(通过 `nRF24L01_Set_Role_Mode()` 动态切换),发送完毕再切回 PRX。

---

## 5. 通信协议设计

### 5.1 nRF24L01 无线协议(主通信协议)

#### 5.1.1 帧格式

```
┌────────┬────────┬──────┬────────────────┬──────┬────────┬──────────┬──────┐
│ HEAD1  │ HEAD2  │ LEN  │   设备编码     │ 命令 │ 命令   │ 参数列表 │ CRC  │
│        │        │      │ DEVICE_ID      │ 类型 │ 状态   │ DATA     │      │
│ 0x55   │ 0xAA   │ 1B   │   2B           │ 1B   │  1B    │ 0~24B   │ 2B   │
└────────┴────────┴──────┴────────────────┴──────┴────────┴──────────┴──────┘

LEN:覆盖区域 = 设备编码(2B) + 命令类型(1B) + 命令状态(1B) + 参数列表(NB),即 LEN = 4 + data_len
CRC:CRC16-Modbus 校验,计算范围从 LEN 字节开始到参数列表末尾
```

#### 5.1.2 帧命令类型与应答状态

**命令类型(CMD_TYPE 字段)**:

| CMD_TYPE 值 | 宏定义 | 说明 |
|-------------|--------|------|
| 0x31 | `FRAME_TYPE_ACT` | 动作命令(连接、控制、响应) |
| 0x32 | `FRAME_TYPE_SET` | 参数设置 |
| 0x33 | `FRAME_TYPE_GET` | 参数获取 |
| 0x66 | `FRAME_TYPE_POST` | 主动上报 |

**命令状态(CMD_STATUS 字段)**:

| CMD_STATUS 值 | 宏定义 | 说明 |
|---------------|--------|------|
| 0x00 | `FRAME_STATE_ERR` | 校验出错 |
| 0x01 | `FRAME_STATE_ASK` | 请求(上位机→下位机) |
| 0x02 | `FRAME_STATE_ACK` | 应答(下位机→上位机) |

#### 5.1.3 设备编码(DEVICE_ID, 2字节)

设备编码为 nRF24L01 地址的 2 字节标识,用于帧中区分数据来源:

| 设备 | DEVICE_ID_H | DEVICE_ID_L | 完整值 | 说明 |
|------|-------------|-------------|--------|------|
| **Sensor 板** | `0x00` | `0x05` | `0x0005` | `DEVICE_SENSOR_ID` |
| **Remote 板** | `0x00` | `0x04` | `0x0004` | `DEVICE_REMOTE_ID` |

> Mainboard 不发送自己的帧,而是通过 `nrf24l01_build_sensor_frame()` 或 `nrf24l01_build_remote_frame()` 构建帧,帧中的 DEVICE_ID 填写目标设备的 ID。接收端通过 Pipe 区分来源。

#### 5.1.4 统一命令码定义

以下命令码定义来源于源码 `mainboard_nrf24l01_message.h`(Mainboard 端)和 `sensor_nrf24l01_message.h`(Sensor 端),是**实际运行代码**中使用的 `FRAME_NRF24_*_CMD` 常量。

> Sensor 和 Mainboard 共享同一命令码值空间(0x01~0x06),通过 CMD_TYPE + CMD_STATUS 组合区分方向。例如 `0x02` 在 Mainboard 端是「发送开始指令」,在 Sensor 端是「确认开始指令」,方向由 `FRAME_STATE_ASK/ACK` 区分。

**连接类**:

| CMD | Mainboard 端宏 | Sensor 端宏 | 方向 | 说明 |
|-----|---------------|-------------|------|------|
| 0x01 | `FRAME_NRF24_CONNECT_CTRL_PANEL_CMD` | `FRAME_NRF24_ASK_CONNECT_PANEL_CMD` | 双向 | 连接请求/确认 |

**Mainboard → Sensor 控制命令**(Mainboard 端定义):

| CMD | 宏定义 | CMD_TYPE | 说明 |
|-----|--------|----------|------|
| 0x02 | `FRAME_NRF24_SEND_TO_SENSOR_START_CMD` | `FRAME_TYPE_ACT` | 发送开始指令 |
| 0x03 | `FRAME_NRF24_ACK_SHOKE_SENSOR_CMD` | `FRAME_TYPE_ACT` | 应答压电反馈 |
| 0x04 | `FRAME_NRF24_ASK_WS2812B_LEVEL_CMD` | `FRAME_TYPE_ACT` | 设置眼部状态(濒死/正常, WS2812B+OLED联动) |
| 0x05 | `FRAME_NRF24_ASK_MOTOR_STATUS_CMD` | `FRAME_TYPE_ACT` | 设置空心杯电机工作模式 |
| 0x06 | `FRAME_NRF24_ACK_CC6201_CMD` | `FRAME_TYPE_ACT` | 应答磁传感器状态 |

**Sensor → Mainboard 上报命令**(Sensor 端定义):

| CMD | 宏定义 | CMD_TYPE | 说明 |
|-----|--------|----------|------|
| 0x02 | `FRAME_NRF24_ACK_START_CMD` | `FRAME_TYPE_ACT` | 确认收到开始指令 |
| 0x03 | `FRAME_NRF24_ASK_SHOKE_SENSOR_CMD` | `FRAME_TYPE_ACT` | 压电陶瓷片震动反馈 |
| 0x04 | `FRAME_NRF24_ACK_WS2812B_LEVEL_CMD` | `FRAME_TYPE_ACT` | 眼部状态设置应答 |
| 0x05 | `FRAME_NRF24_ACK_MOTOR_STATUS_CMD` | `FRAME_TYPE_ACT` | 电机状态应答 |
| 0x06 | `FRAME_NRF24_ASK_CC6201_CMD` | `FRAME_TYPE_ACT` | 磁传感器状态上报 |

**说明**:
- 同一 CMD 值在两端可能有不同宏名,代表同一命令的发送端/接收端视角
- `CMD_STATUS` 字段 (`FRAME_STATE_ASK=0x01` 请求 / `FRAME_STATE_ACK=0x02` 应答) 区分请求与应答
- 传感器数据上报(按压/吹气/角度等)通过 `CMD_SENSOR_DATA (0x10)` 传输,定义在 `cpr_cmd_t` 中(规划中,尚未集成到实际帧)

#### 5.1.5 传感器数据 Payload

```c
typedef struct __attribute__((packed)) {
    uint16_t press_depth;     // 按压深度 (0-1000), 单位0.1mm
    uint16_t press_freq;      // 按压频率 (次/分钟)
    uint16_t tidal_volume;    // 吹气量
    int16_t  angle_x;         // MPU6050 X轴角度
    int16_t  angle_y;         // MPU6050 Y轴角度
    uint8_t  hall_status;     // 霍尔传感器状态 (0:有异物 1:无异物)
    uint8_t  position;        // 按压位置 1-7
} sensor_data_payload_t;
```

#### 5.1.6 CRC 校验

```c
uint16_t CrcCalc_Crc16Modbus(uint8_t *dat, uint8_t len)
{
    uint16_t CRC_index = 0xFFFF;
    for (uint8_t i = 0; i < len; i++) {
        CRC_index ^= dat[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (CRC_index & 0x0001)
                CRC_index = (CRC_index >> 1) ^ 0xA001;
            else
                CRC_index >>= 1;
        }
    }
    return CRC_index;
}
```

### 5.2 Raster 板串口协议(实时数据帧)

**物理参数**:UART1, 115200bps, 8N1
**用途**:光栅板 → Sensor 板,每 100ms 发送一次原始脉冲增量和方向

**帧格式（8字节）**:
```
Byte:  [0]    [1]    [2]    [3]    [4]    [5]    [6]    [7]
       0xAA   0x04   TYPE   CNT_H  CNT_L  DIR    CHK    0x55
       帧头   长度   类型   计数H  计数L  方向   校验   帧尾
```

- **帧头 (0xAA)**:固定标识
- **长度 (0x04)**:数据域长度(TYPE+CNT_H+CNT_L+DIR = 4字节)
- **类型**:1 字节,`0x01`=按压数据,`0x02`=吹气数据
- **脉冲计数**:2 字节(int16_t),自上次发送以来的脉冲增量,正=正向,负=反向
- **方向**:1 字节(signed int8_t),`-1`=回弹/泄气, `0`=静止, `1`=下压/充气
- **校验**:Byte[0]~Byte[5] 累加和
- **帧尾 (0x55)**:固定标识

**发送频率**:每 100ms 发送一次

**Sensor 板收到后的处理**:
- 累加脉冲增量: `cumulative += pulse_count`
- 计算深度: `depth_01mm = cumulative × MM_PER_PULSE_01`（MM_PER_PULSE_01 = 5, 即 0.5mm/脉冲）
- 使用深度值进行后续事件检测和成绩计算

> 光栅板通过模式变量 `g_raster_detect_mode` 和物理信号 `direction_blow` 联合判断：当 `g_raster_detect_mode == RASTER_DETECT_BLOW`（Sensor 板判别为吹气模式）或 `direction_blow != 0`（光电编码器检测到吹气运动）时发送吹气帧（TYPE=0x02），其余时间发送按压帧（TYPE=0x01）。模式变量由 Sensor 板通过 UART2 发送模式切换指令（CMD=0x11）控制。

**Sensor → 光栅板 模式切换指令（6字节，新增于 2026-04-29）**:

当 Sensor 板通过三路信号联合判别确定是按压还是吹气后，向光栅板发送模式切换指令，使光栅板切换检测逻辑：

```
Byte:  [0]    [1]    [2]    [3]    [4]    [5]
       0xAA   0x02   0x11   DATA   CHK    0x55
       帧头   长度   模式   数据   校验   帧尾
```

| DATA 值 | 宏定义 | 含义 | 触发条件 |
|---------|--------|------|---------|
| 0x01 | CMD_ACTIVATE_PRESSURE | 进入按压检测模式 | ADC128S 压力超过阈值 |
| 0x02 | CMD_IDLE_PRESSURE | 退出按压检测模式 | 无压力且非吹气 |
| 0x03 | CMD_ACTIVATE_BLOW | 进入吹气检测模式 | MPU6050 上仰确认 + 无压力 |

- CMD=0x11 为模式切换类别，DATA 区分子指令
- **帧格式复用**：帧头/帧尾/校验与停止指令一致，通过 CMD=0x11 区分
- **校验**: Byte[0]~Byte[3] 累加和
- **光栅板响应**: 收到模式切换指令后，切换光电传感器的检测逻辑和数据帧上报类型（TYPE 字段）
- **状态变化时才发送**: 避免重复发送，减少 UART 流量

### 5.3 Sensor 板成绩数据(原光栅板职责)

**说明**:原光栅板负责的成绩计算已全部移至 Sensor 板。Sensor 板接收光栅板原始脉冲数据后,自行完成深度计算、事件检测、成绩统计,并通过 nRF24L01 上报给 Mainboard。

#### 5.3.1 成绩数据结构

Sensor 板内部使用的成绩数据结构(64字节):

```c
// Sensor 板内部成绩数据结构 (64字节, packed) — 所有成绩由Sensor板计算
typedef struct __attribute__((packed)) {
    // --- 按压报告 (13 x uint16_t = 26B) ---
    uint16_t press_total;         // 按压总次数
    uint16_t press_correct;       // 按压正确次数
    uint16_t press_out_of_std;    // 标准频率外次数
    uint16_t press_error_total;   // 按压错误总数(过大+不足+位置错误+未回弹)
    uint16_t press_accuracy;      // 正确率 (0-100, 单位%)
    uint16_t press_too_few;       // 少按次数
    uint16_t press_too_many;      // 多按次数
    uint16_t press_depth_avg;     // 平均按压深度 (0.1mm)
    uint16_t press_freq_avg;      // 平均按压频率 (次/分)
    uint16_t press_too_deep;      // 按压过大次数 (深度>60mm)
    uint16_t press_too_shallow;   // 按压不足次数 (深度<50mm)
    uint16_t press_no_rebound;    // 未回弹数
    uint16_t press_position_err;  // 位置错误次数

    // --- 吹气报告 (11 x uint16_t = 22B) ---
    uint16_t blow_avg_time;       // 吹气平均时间 (ms)
    uint16_t cycle_gap_time;      // 两个循环间按压间断时间 (ms)
    uint16_t blow_error_total;    // 吹气错误总数(过大+不足)
    uint16_t blow_correct;        // 吹气正确次数
    uint16_t blow_accuracy;       // 正确率 (0-100, 单位%)
    uint16_t blow_too_few;        // 少吹次数
    uint16_t blow_too_many;       // 多吹次数
    uint16_t blow_too_much;       // 吹气过大次数 (深度>60mm=600ml)
    uint16_t blow_too_little;     // 吹气不足次数 (深度<50mm=500ml)
    uint16_t blow_total;          // 吹气总次数
    uint16_t tidal_avg;           // 平均潮气量 (0.1mm, 1mm≈100ml)

    // --- 循环统计 (4 x uint16_t = 8B) ---
    uint16_t press_in_5_cycles;   // 5个循环内按压总数
    uint16_t blow_in_5_cycles;    // 5个循环内吹气总数
    uint16_t press_out_5_cycles;  // 5个循环外按压总数
    uint16_t blow_out_5_cycles;   // 5个循环外吹气总数

    // --- 结果 (1xuint16_t + 2xuint8_t = 4B) ---
    uint16_t total_time;          // 实际用时(秒)
    uint8_t  cycle_count;         // 完成循环数
    uint8_t  result;              // 急救结果 (0:失败 1:成功)

    uint8_t  reserved[6];         // 预留,凑齐64字节
} RasterReport_t;  // 29xuint16_t=58B + 2xuint8_t=2B + 6B = 64B
```

#### 5.3.2 成绩数据帧格式

成绩帧由 Sensor 板生成,通过 nRF24L01 上报给 Mainboard(格式参见 §5.1 nRF24L01 无线协议)。

> ⚠️ 注意:此帧不再由光栅板 UART1 发送,改为 Sensor 板内部计算后通过无线链路上报。

**触发条件**:
- 训练/考核/竞赛模式结束时,Mainboard 发送停止指令
- Sensor 板转发停止指令到光栅板（光栅板收到后直接返回待机）
- Sensor 板停止接收脉冲数据,使用已累计的原始数据完成最终成绩计算
- Sensor 板通过 nRF24L01 将成绩上报给 Mainboard

**数据流向**:
```
STM8 光栅板 (纯采集)
    │ UART1 发送原始脉冲增量帧 (8 Bytes, 每100ms)
    ▼
Sensor 板 (UART2 接收, 累加脉冲, 计算深度, 执行全部成绩计算)
    │ nRF24L01 成绩数据上报
    ▼
Mainboard 板 (接收并存储)
    │ Flash 写入
    ▼
DM32 打印机 (用户触发打印)
```

#### 5.3.3 成绩计算参数配置

**按压参数**:

| 参数 | 宏定义 | 默认值 | 说明 |
|------|--------|--------|------|
| `MM_PER_PULSE_01` | `MM_PER_PULSE_01` | 5 (0.1mm) | 每个脉冲对应的0.1mm数 |
| `PRESS_DEPTH_MIN_01MM` | `PRESS_DEPTH_MIN_01MM` | 500 (0.1mm=50mm) | 按压最小深度 |
| `PRESS_DEPTH_MAX_01MM` | `PRESS_DEPTH_MAX_01MM` | 600 (0.1mm=60mm) | 按压最大深度 |
| `PRESS_FREQ_MIN` | `PRESS_FREQ_MIN` | 100次/分 | 按压最小频率 |
| `PRESS_FREQ_MAX` | `PRESS_FREQ_MAX` | 120次/分 | 按压最大频率 |

**吹气参数**:

| 参数 | 宏定义 | 默认值 | 说明 |
|------|--------|--------|------|
| `MM_PER_PULSE` | `MM_PER_PULSE_01` | 5 (0.1mm) | 每个脉冲对应的0.1mm数 |
| `MIN_DEPTH` | `BLOW_DEPTH_MIN_01MM` | 500 (0.1mm) | 最小吹气深度，约500ml |
| `MAX_DEPTH` | `BLOW_DEPTH_MAX_01MM` | 600 (0.1mm) | 最大吹气深度，约600ml |

**循环参数**:

| 参数 | 宏定义 | 默认值 | 说明 |
|------|--------|--------|------|
| `PRESS_PER_CYCLE` | `CYCLE_PRESS_COUNT` | 30 | 每个循环按压次数 |
| `BREATH_PER_CYCLE` | `CYCLE_BREATH_COUNT` | 2 | 每个循环吹气次数 |
| `TOTAL_CYCLES` | `CYCLE_TOTAL` | 5 | 总循环数 |

**结果判定参数**:

| 参数 | 宏定义 | 默认值 | 说明 |
|------|--------|--------|------|
| `PRESS_PASS_RATE` | `RESULT_PRESS_PASS` | 80% | 按压合格率阈值 |
| `BREATH_PASS_RATE` | `RESULT_BREATH_PASS` | 80% | 吹气合格率阈值 |

### 5.4 RS485 有线协议(Modbus RTU)

**物理参数**:RS485, 波特率可配, Modbus RTU 帧格式

**功能码**:

| 功能码 | 宏定义 | 说明 |
|--------|--------|------|
| 0x03 | `MODBUS_READ_HOLDING` | 读保持寄存器 |
| 0x06 | `MODBUS_WRITE_SINGLE` | 写单个寄存器 |
| 0x10 | `MODBUS_WRITE_MULTIPLE` | 写多个寄存器 |

**寄存器映射**:

| 地址 | 名称 | 类型 | 说明 |
|------|------|------|------|
| 0x0001 | `REG_PRESS_DEPTH` | uint16 | 按压深度 (0.1mm) |
| 0x0002 | `REG_PRESS_FREQ` | uint16 | 按压频率 (次/分) |
| 0x0003 | `REG_TIDAL_VOLUME` | uint16 | 吹气量 |
| 0x0004 | `REG_ANGLE_X` | int16 | MPU6050 X轴角度 |
| 0x0005 | `REG_ANGLE_Y` | int16 | MPU6050 Y轴角度 |
| 0x0006 | `REG_HALL_STATUS` | uint16 | 霍尔传感器状态 (0:有异物 1:无异物) |
| 0x0007 | `REG_POSITION` | uint16 | 按压位置 1-7 |
| 0x0008 | `REG_WS2812_LEVEL` | uint16 | 眼部状态 (0:濒死 WS2812B最低+OLED黑屏 1:正常 WS2812B白灯+OLED白底黑圆) |
| 0x0009 | `REG_MOTOR_STATUS` | uint16 | 电机状态 (0:关闭 1:随按压 2:自主) |
| 0x000A | `REG_DEVICE_STATUS` | uint16 | 设备状态位图 |

**数据采集帧 (功能码 0x03)**:Mainboard 一次读取 0x0001~0x000A 共 10 个寄存器,获取完整传感器数据。

**控制写入帧 (功能码 0x10)**:Mainboard 写入 0x0008~0x0009,控制眼灯和电机。

**异常响应**:功能码最高位置 1 + 异常码(0x01~0x04)

---

## 6. 核心业务流程

### 6.1 系统启动与连接建立流程

```
┌──────────┐                 ┌────────────┐                 ┌──────────┐
│ Sensor板 │                 │ Mainboard板 │                 │ Remote板 │
└────┬─────┘                 └──────┬──────┘                 └────┬─────┘
     │                              │                             │
     │  [上电初始化]                 │  [上电初始化]                │  [上电初始化]
     │  nRF24L01 初始化              │  nRF24L01 PRX 模式          │  nRF24L01 初始化
     │  LVGL/LCD 初始化             │  外设初始化                  │  等待连接
     │                              │                             │
     │── ASK_CONNECT (Pipe1) ──────►│                             │
     │   (每500ms重试)              │                             │
     │                              │                             │
     │                              │◄── ASK_CONNECT (Pipe2) ─────│
     │                              │   (每150ms重试)              │
     │                              │                             │
     │                              │ [识别来源: Pipe1=Sensor]     │
     │                              │ [识别来源: Pipe2=Remote]     │
     │                              │                             │
     │◄─ ACK_CONNECT (Pipe1) ──────│  (重试3次)                  │
     │   (nrf_if_connected = 1)     │                             │
     │                              │── ACK_CONNECT (Pipe2) ─────►│
     │                              │  (重试3次)                  │
     │                              │   (nrf_if_connected = 1)    │
     │                              │                             │
     │  [连接建立完成]               │  [两设备均已连接]            │  [连接建立完成]
     │  进入正常数据上报模式          │  进入业务调度模式            │  根据场景决定UI行为：
     │                              │                             │  - 用户未按"开始"：保持首页
     │                              │                             │  - 用户已按"开始"：自动跳转菜单
     │                              │                             │
```

### 6.2 正常训练/考核流程

```
┌──────────┐         ┌────────────┐         ┌──────────┐         ┌──────────┐
│ Raster板 │         │ Sensor板   │         │Mainboard │         │ Remote板 │
└────┬─────┘         └─────┬──────┘         └────┬─────┘         └────┬─────┘
     │                     │                     │                     │
     │                     │                     │◄── MODE_IN ────────│
     │                     │                     │  (训练/考核/竞赛)    │
     │                     │                     │                     │
     │                     │◄── START_CMD ──────│                     │
     │                     │  (开始指令)          │                     │
     │                     │                     │                     │
     │                     │── ACK_START ───────►│                     │
     │                     │                     │                     │
     │  ┌─────────────────────────────────────────────────────────────┐
     │  │                    CPR 操作循环                              │
     │  │  [考核模式: 额外执行操作流程检查(意识/脉搏/呼吸/异物等)]    │
     │  │                                                             │
     │  │  ┌─用户按压──►光栅A/B相中断──►脉冲增量发送──┐               │
     │  │  │    Raster: 每100ms发送原始脉冲增量帧      │               │
     │  │  │    Sensor: 累加脉冲→深度+ADC触点+频率计算 │               │
     │  │  │    Mainboard: 评分运算                    │               │
     │  │  └──────────────────────────────────────────┘               │
     │  │                                                             │
     │  │  ┌─用户吹气──►光栅脉冲增量发送──┐                           │
     │  │  │    Sensor: 累加脉冲→潮气量+MPU6050头部角度 │             │
     │  │  └────────────────────────────────────────────┘             │
     │  │                                                             │
     │  └─────────────────────────────────────────────────────────────┘
     │                     │                     │                     │
     │                     │                     │◄── MODE_OUT ───────│
     │                     │                     │  (退出模式)          │
     │                     │                     │                     │
     │  [Sensor板计算最终成绩]  │                     │                     │
     │                     │── 成绩数据(nRF24L01) ─►│                     │
     │                     │                     │── Flash存储 ───────►│
     │                     │                     │── CMD_REPORT_SAVED ─►│
     │                     │                     │   (LVGL显示成绩单)   │
     │                     │                     │                     │
     │                     │                     │◄── PRINT_REQ ──────│
     │                     │                     │── DM32打印 ────────►│
     │                     │                     │                     │
```

### 6.3 数据流详解

#### 6.3.1 按压数据流

```
用户按压 → 光栅编码器 → Raster板(中断计数) → UART发送深度方向
    → Sensor板(UART2接收) → ADC128S102CIMTX(薄膜压力传感器点位检测) → MPU6050(头部角度)
    → nRF24L01打包 → Mainboard(Pipe1接收) → 评分运算
    → 数码管显示/语音播报 → nRF24L01(Pipe2) → Remote(LVGL显示)
```

#### 6.3.2 吹气数据流

```
用户吹气 → 光栅编码器 → Raster板(中断计数) → UART发送潮气量
    → Sensor板(UART2接收) → MPU6050(头部上仰角度确认) → 三路联合判别确认吹气
    → nRF24L01打包 → Mainboard(Pipe1) → 潮气量评分
    → 数码管/语音 → Remote(LVGL)
```

#### 6.3.3 异物检测数据流

```
CC6201霍尔传感器 → Sensor板检测状态变化
    → nRF24L01(CMD_ASK_CC6201) → Mainboard(Pipe1)
    → 状态记录 + 语音提示 + Remote(LVGL显示)
```

#### 6.3.4 按压位置LED控制数据流

```
Mainboard评分运算 → 判断7个按压位置状态
    → nRF24L01(CMD_PRESS_LED_CTRL, Pipe2)
    → Remote(LVGL circle更新颜色)
    颜色映射: WHITE=未按, RED=错误, YELLOW=偏移, GREEN=正确
```

#### 6.3.5 成绩单数据流向

```
STM8 光栅板(纯脉冲采集)
    ↓ UART 串口(115200, 8字节脉冲增量帧, 每100ms)
Sensor 板(接收原始数据, 累加脉冲, 执行全部成绩计算)
    ↓ nRF24L01 成绩数据上报
Mainboard 板(记录成绩)
    ↓ Flash 存储
DM32 热敏打印机(用户触发打印)
```

#### 6.3.6 按压/吹气联合判别数据流（新增于 2026-04-29）

````
光电传感器(ST130B)中断触发 → Sensor板接收中断信号
    ├─ ADC128S102CIMTX 读取压力值 ──┐
    ├─ MPU6050 读取 pitch 角 ──────┤
    │                              ▼
    │                    Sensor板联合判别模块
    │                              │
    │              ┌───────────────┼───────────────┐
    │              ▼               ▼               ▼
    │         有压力?          无压力+上仰?      两路均无
    │              │               │               │
    │              ▼               ▼               ▼
    │          activate         idle+activate    丢弃
    │         _pressure        _blow          (抖动)
    │              │               │
    │              ▼               ▼
    │         UART2 发送      UART2 发送
    │         给光栅板         给光栅板
    │              │               │
    │              ▼               ▼
    │         光栅板切换       光栅板切换
    │         按压检测模式     吹气检测模式
    ```

### 6.4 考核模式操作流程检查

考核模式下,系统在标准 CPR 操作流程基础上,额外检测并记录以下 10 项操作步骤:

| 序号 | 检查项 | 检测方法 | 数据来源 |
|------|--------|---------|---------|
| 1 | 意识判断 | 压电陶瓷片是否在规定时间内触发(肩部拍打) | Sensor 板 STM32 内置 ADC |
| 2 | 脉搏检查 | 空心杯电机状态是否被读取 | Sensor 板 |
| 3 | 检查呼吸 | 光栅板数据辅助判断 | Sensor 板 / Raster |
| 4 | 急救呼叫 | 操作者按键确认或语音识别 | Mainboard 板 |
| 5 | 清除异物 | CC6201 从"有异物"→"无异物" | Sensor 板 CC6201 |
| 6 | 开放气道 | MPU6050 检测头部上仰角度达标 | Sensor 板 MPU6050 |
| 7 | 急救前脉搏检查 | 开始按压前是否检查脉搏 | Sensor 板 |
| 8 | 急救后脉搏检查 | 停止按压后是否检查脉搏 | Sensor 板 |
| 9 | 急救前瞳孔检查 | 开始急救前是否检查瞳孔状态 | Sensor 板 |
| 10 | 急救后瞳孔检查 | 急救结束后是否检查瞳孔状态 | Sensor 板 |

每项结果记录为"完成"或"未完成",纳入考核成绩单的**操作流程检查**区域打印。

详细规范参见 [业务需求.md §4.2](./业务需求.md#42-考核模式成绩单)。

---

## 7. 线程架构与任务调度

### 7.1 Mainboard 板线程架构

```
┌─────────────────────────────────────────────────────────────────┐
│                    Mainboard RT-Thread 线程架构                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  Priority 11 ┌──────────────────────────────┐                   │
│              │ start_thread_entry            │ 状态检查线程       │
│              │ (1024B, 周期20ms)             │ CC6201状态监控    │
│              └──────────────────────────────┘                   │
│                                                                  │
│  Priority 21 ┌──────────────────────────────┐                   │
│              │ nRF24L01_Decode_entry         │ 连接应答线程       │
│              │ (1024B, 周期50ms)             │ Sensor/Remote ACK│
│              └──────────────────────────────┘                   │
│                                                                  │
│  Priority 22 ┌──────────────────────────────┐                   │
│              │ nRF24L01_Thread_entry         │ nRF接收主线程     │
│              │ (4096B, IRQ等待)              │ PRX模式等待中断   │
│              └──────────────────────────────┘                   │
│                                                                  │
│  Priority 23 ┌──────────────────────────────┐                   │
│              │ nRF24L01_Data_Transmit_entry  │ 数据发送线程       │
│              │ (2048B, 周期50ms)             │ PTX发送+重试      │
│              └──────────────────────────────┘                   │
│                                                                  │
│  Priority 24 ┌──────────────────────────────┐                   │
│              │ touch_thread_entry            │ 触摸按键扫描       │
│              └──────────────────────────────┘                   │
│                                                                  │
│  Priority 25 ┌──────────────────────────────┐                   │
│              │ nixietube_thread_entry        │ 数码管刷新         │
│              │ lightbar_thread_entry         │ 光条刷新           │
│              │ wt588d_thread_entry           │ 语音播报           │
│              │ printer_thread_entry          │ 打印机任务         │
│              └──────────────────────────────┘                   │
│                                                                  │
│  同步机制:                                                       │
│  ├─ nrf24_irq_sem: 二值信号量 (IRQ → 接收线程)                   │
│  ├─ nrf24_send_sem: 二值信号量 (触发发送)                         │
│  ├─ nrf24_mutex: 互斥锁 (保护nRF24L01硬件访问)                   │
│  └─ Record.sensor_connect_pending: 原子标志位                    │
│     Record.remote_connect_pending: 原子标志位                    │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

**三线程协作模式**:

| 线程 | 职责 | 工作模式 |
|------|------|----------|
| `nRF24L01_Thread_entry` | 接收数据、识别来源(Pipe)、调用协议解析 | PRX 等待 IRQ |
| `nRF24L01_Decode_entry` | 处理连接请求、发送 ACK | 周期轮询 pending 标志 |
| `nRF24L01_Data_Transmit_Thread_entry` | 主动发送控制指令到 Sensor | PTX 模式 + 重试 |

> ⚠️ `Decode_entry` 和 `Data_Transmit` 共享 nRF24L01 硬件,通过 `nrf24_mutex` 互斥访问。

### 7.2 Sensor 板线程架构

```
┌─────────────────────────────────────────────────────────────────┐
│                    Sensor RT-Thread 线程架构                      │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  Priority 10 ┌──────────────────────────────┐                   │
│              │ uart2_thread_entry            │ 光栅数据接收线程   │
│              │ (1024B, UART2中断驱动)        │ 循环队列+互斥锁   │
│              └──────────────────────────────┘                   │
│                                                                  │
│  Priority 22 ┌──────────────────────────────┐                   │
│              │ nRF24L01_Thread_entry         │ nRF收发一体线程   │
│              │ (4096B, 周期20ms)             │ PTX/PRX动态切换   │
│              └──────────────────────────────┘                   │
│                                                                  │
│  Priority 23 ┌──────────────────────────────┐                   │
│              │ Hard_Thread_entry             │ 硬件控制线程       │
│              │ (4096B, 周期500ms)            │ WS2812B/CC6201    │
│              └──────────────────────────────┘                   │
│                                                                  │
│  RTT Timer  ┌──────────────────────────────┐                    │
│  (10ms)     │ Timing_10ms()                 │ 电机控制/数据处理  │
│             └──────────────────────────────┘                    │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

**Sensor 板 nRF 线程状态机**:

```
                    ┌─────────────────┐
                    │  未连接状态      │
                    │ (nrf_if_connected│
                    │      == 0)      │
                    └────────┬────────┘
                             │
              ┌──────────────┼──────────────┐
              ▼                             ▼
    ┌──────────────────┐         ┌──────────────────┐
    │ PTX: 每500ms     │         │ PRX: 打开200ms   │
    │ 发送 ASK_CONNECT │         │ 接收窗口等 ACK   │
    │ (Pipe1)          │         │                   │
    └────────┬─────────┘         └────────┬──────────┘
             │                            │
             │      收到 ACK_CONNECT      │
             └──────────┬─────────────────┘
                        ▼
                    ┌─────────────────┐
                    │  已连接状态      │
                    │ (nrf_if_connected│
                    │      == 1)      │
                    └────────┬────────┘
                             │
              ┌──────────────┼──────────────┐
              ▼              ▼              ▼
    ┌──────────────┐ ┌──────────────┐ ┌──────────────┐
    │ 每60ms上报   │ │ 每160ms     │ │ 接收Mainboard│
    │ 按压/震动/   │ │ PRX窗口    │ │ 控制指令     │
    │ 电机/CC6201  │ │ (偶尔)     │ │ (WS/Motor等) │
    └──────────────┘ └──────────────┘ └──────────────┘
```

### 7.3 Remote 板线程架构

```
┌─────────────────────────────────────────────────────────────────┐
│                    Remote RT-Thread 线程架构                      │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  Priority 9  ┌──────────────────────────────┐                   │
│              │ nRF24L01_Thread_entry         │ nRF收发线程       │
│              │ (4096B, 状态机驱动)           │ PTX+PRX窗口      │
│              │                               │                   │
│              │ nRF24L01_Decode_Thread_entry  │ 解码+GUI更新     │
│              │ (4096B, 周期50ms)             │ LED颜色/模式     │
│              └──────────────────────────────┘                   │
│                                                                  │
│  Priority 10 ┌──────────────────────────────┐                   │
│              │ ft6336u_thread_entry          │ 触摸屏扫描       │
│              └──────────────────────────────┘                   │
│                                                                  │
│  Priority 11 ┌──────────────────────────────┐                   │
│              │ adc_thread_entry              │ 电池电量检测     │
│              └──────────────────────────────┘                   │
│                                                                  │
│  LVGL Timer ┌──────────────────────────────┐                    │
│             │ lv_timer_handler()            │ LVGL 定时刷新     │
│             │ (周期 5ms)                    │                   │
│             └──────────────────────────────┘                    │
│                                                                  │
│  事件机制:                                                       │
│  ├─ nrf24l01_events: 事件集                                      │
│  │   └─ EVENT_NRF24_ACK_BODY_LED: 按压位置LED颜色更新            │
│  └─ Record.mode_data_in_set: 模式设置标志                        │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

**Remote 板 nRF 状态机**:

```
    ┌──────────────────┐
    │ DISCONNECTED     │
    │ 每150ms:         │
    │ PTX发ASK_CONNECT │──► 立即切PRX等待100ms
    │ (Pipe2)          │
    └────────┬─────────┘
             │ 收到 ACK_CONNECT
             ▼
    ┌──────────────────┐
    │ CONNECTED        │
    │ nrf_if_connected │
    │      == 1        │
    └────────┬─────────┘
             │
    ┌────────┼────────┐
    ▼        ▼        ▼
  每400ms  Decode   GUI
  心跳     线程     刷新
          处理LED
          颜色事件
```

### 7.4 Raster 板(裸机,中断驱动)

```
┌─────────────────────────────────────────────────────────────────┐
│                    Raster STM8 主循环                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  中断服务 (EXTI)                                                 │
│  ├─ GPIOC Pin6/7 (A/B相) ──► PC_ISR: 计数+方向判断              │
│  └─ GPIOD Pin3/4 (A/B相) ──► PD_ISR: 计数+方向判断              │
│                                                                  │
│  主循环 (while(1))                                               │
│  ├─ USART1_ProcessRxData()              // 处理Sensor板指令       │
│  ├─ RASTER_ACTIVE: 每100ms非阻塞发送                             │
│  │   ├─ delta_press = depth_count_press - prev_press             │
│  │   ├─ delta_blow = depth_count_blow - prev_blow                │
│  │   └─ if (g_raster_detect_mode == BLOW || direction_blow != 0)│
│  │       → USART1_SendRealtimeData(delta_blow, dir_blow, 0x02)  │
│  │      else                                                     │
│  │       → USART1_SendRealtimeData(delta_press, dir_press, 0x01)│
│  └─ Delay_ms(5)                                                  │
│                                                                  │
│  定时器 (TIM1)                                                   │
│  └─ 系统滴答 (Systick)                                          │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

---

## 8. 错误处理与容错机制

### 8.1 连接建立容错

| 场景 | 处理机制 | 重试策略 |
|------|----------|----------|
| Sensor 连接请求未被响应 | Sensor 每 500ms 重发 ASK_CONNECT | 无上限,直到收到 ACK |
| Mainboard ACK 丢失 | Mainboard 连续发送 3 次 ACK | 每次间隔 50ms |
| Remote 连接请求未被响应 | Remote 每 150ms 重发 ASK_CONNECT | 无上限,直到收到 ACK |
| SPI 硬件通信异常 | 启动时回环检测 (`nRF24L01_Check_SPI_Community`) | 失败则 LOG_E 告警 |

### 8.2 数据传输容错

| 场景 | 处理机制 |
|------|----------|
| nRF24L01 发送失败 (MAX_RT) | `nrf24l01_send_with_retry()` 自动重试 N 次 |
| CRC 校验失败 | 帧丢弃,`FRAME_STATE_ERR` 标记 |
| 接收队列溢出 (UART2) | 循环队列满时丢弃新数据并打印告警 |
| Pipe 来源未知 (`SRC_UNKNOWN`) | 帧丢弃,不处理 |

### 8.3 nRF24L01 发送重试机制

```c
rt_err_t nrf24l01_send_with_retry(nrf24_t nrf24, uint8_t order,
                                   nrf24_pipe_et pipe, uint8_t max_retry)
{
    for (uint8_t retry = 0; retry < max_retry; retry++) {
        /* 切换到 PTX 模式 */
        nRF24L01_Set_Role_Mode(nrf24, ROLE_PTX);
        nRF24L01_Flush_TX_FIFO(nrf24);

        /* 发送 */
        nrf24l01_order_to_pipe(nrf24, order, pipe);
        nrf24->nrf24_ops.nrf24_set_ce();
        rt_thread_mdelay(10);
        nrf24->nrf24_ops.nrf24_reset_ce();

        /* 检查发送状态 */
        uint8_t status = nRF24L01_Read_Status_Register(nrf24);
        if (status & NRF24BITMASK_TX_DS) {
            /* 发送成功 */
            nRF24L01_Set_Role_Mode(nrf24, ROLE_PRX);
            return RT_EOK;
        }

        /* 发送失败,检查是否 PLOS 超限 */
        uint8_t observe = nRF24L01_Read_Reg_Data(nrf24, NRF24REG_OBSERVE_TX);
        if ((observe >> 4) > 0x0A) {
            /* 丢包太多,切换频道 */
            nRF24L01_Flush_TX_FIFO(nrf24);
        }

        rt_thread_mdelay(10);
    }

    /* 恢复 PRX 模式 */
    nRF24L01_Set_Role_Mode(nrf24, ROLE_PRX);
    return -RT_ETIMEOUT;
}
```

### 8.4 诊断信息输出

Mainboard 在发送 ACK 时输出完整诊断信息:

```
DIAG retry 0: STATUS=0x0E(MAX_RT=0 TX_DS=1 RX_DR=0)
           OBSERVE=0x00(PLOS=0 ARC=0) FIFO=0x11
           CONFIG=0x0E PRIM_RX=0
           TX_ADDR=A1B2C3D4E5
           RX_P1=A1B2C3D405
```

### 8.5 链路健康检测

| 检测项 | 方法 | 周期 |
|--------|------|------|
| SPI 链路 | 启动时回环测试 | 一次 |
| nRF 连接状态 | `Record.nrf_if_connected` 标志 | 持续监控 |
| 心跳超时 | `last_sensor_heartbeat` / `last_remote_heartbeat` 时间戳对比 | 周期性 |
| Raster 数据超时 | UART2 接收超时检测 | 周期性 |
| RS485 链路 | Modbus 帧 CRC 校验 | 每帧 |

### 8.6 Fallback 机制

```
┌─────────────────────────────────────────────┐
│              通信 Fallback 链                │
├─────────────────────────────────────────────┤
│                                              │
│  优先级 1: nRF24L01 无线通信                  │
│     │                                        │
│     │  连接失败 / 数据超时 / PLOS 过高        │
│     ▼                                        │
│  优先级 2: RS485 有线通信                     │
│     │  (wired_connect_flag = 1)              │
│     │                                        │
│     │  需要物理连接线缆                       │
│     ▼                                        │
│  优先级 3: 系统告警 + LED 指示                │
│     │  (两路均不可用)                         │
│                                              │
└─────────────────────────────────────────────┘

触发条件:
- nRF 连续 N 次发送失败 → 设置 wired_connect_flag = 1
- 切换到 RS485 Modbus 通信
- nRF24L01 线程可被 suspend 以节省资源
```

---

## 9. 有线/无线切换机制

### 9.1 连接状态枚举

```c
typedef enum {
    CONN_WIRED      = 0,   // 有线优先,nRF 应关闭/挂起
    CONN_WIRELESS   = 1,   // 无线模式,nRF 应运行
    CONN_UNKNOWN    = 2    // 初始/过渡状态
} conn_state_t;
```

### 9.2 切换流程

```
                        ┌──────────────┐
                        │  CONN_UNKNOWN │
                        │  (上电默认)   │
                        └──────┬───────┘
                               │
                    ┌──────────┴──────────┐
                    ▼                     ▼
         ┌──────────────────┐  ┌──────────────────┐
         │ 检测有线连接     │  │ 无有线连接        │
         │ (RS485 信号检测) │  │                   │
         └────────┬─────────┘  └────────┬─────────┘
                  │                     │
                  ▼                     ▼
         ┌──────────────────┐  ┌──────────────────┐
         │ CONN_WIRED       │  │ CONN_WIRELESS    │
         │ nRF suspend      │  │ nRF 正常运行     │
         │ RS485 Modbus通信 │  │ nRF24L01通信     │
         │ Remote不可用     │  │ Remote可用       │
         └──────────────────┘  └──────────────────┘
```

### 9.3 Remote 板不参与有线通信

> **关键约束**:Remote 板不参与有线连接通讯。
>
> 原因分析:
> - Remote 是操作者手持的 UI 设备,物理位置不固定
> - RS485 有线连接仅用于 Sensor 与 Mainboard 之间的近距高速通信
> - Remote 与 Mainboard 始终通过 nRF24L01 无线通信
>
> 因此在有线模式下:
> - Sensor 板通过 RS485 向 Mainboard 发送数据
> - Mainboard 仍通过 nRF24L01 与 Remote 通信
> - 系统保持"有线数据 + 无线遥控"的混合模式

### 9.4 有线模式下的数据格式

RS485 有线模式使用 Modbus RTU 协议,Sensor 作为从机(地址 0x01),Mainboard 作为主机。

```
Mainboard (Master)                    Sensor (Slave 0x01)
      │                                      │
      │──── Read Holding Reg (0x03) ────────►│
      │     Start=0x0001, Num=10             │
      │                                      │
      │◄──── Response: 10 registers ────────│
      │     [press_depth][press_freq]        │
      │     [tidal_volume][angle_x][angle_y] │
      │     [hall_status][position]...       │
      │                                      │
      │──── Write Multiple (0x10) ──────────►│
      │     [ws2812_level][motor_status]     │
      │                                      │
```

---

## 10. 扩展性与维护性

### 10.1 模块化设计

```
┌─────────────────────────────────────────────────┐
│                 分层架构                          │
├─────────────────────────────────────────────────┤
│                                                  │
│  ┌─────────────────────────────────────────────┐ │
│  │ 应用层 (macAPP/macTASK)                     │ │
│  │ 业务逻辑、线程调度、GUI 页面                  │ │
│  └─────────────────┬───────────────────────────┘ │
│                    │                              │
│  ┌─────────────────▼───────────────────────────┐ │
│  │ 协议层 (macNRF/*_message)                   │ │
│  │ 帧构建、帧解析、CRC校验、命令分发             │ │
│  └─────────────────┬───────────────────────────┘ │
│                    │                              │
│  ┌─────────────────▼───────────────────────────┐ │
│  │ 驱动层 (macNRF/*_driver, macBSP)            │ │
│  │ nRF24L01驱动、UART驱动、RS485驱动、ADC驱动  │ │
│  └─────────────────┬───────────────────────────┘ │
│                    │                              │
│  ┌─────────────────▼───────────────────────────┐ │
│  │ 硬件抽象层 (macNRF/*_spi, *_iic)            │ │
│  │ SPI、I2C、GPIO 底层操作                      │ │
│  └─────────────────────────────────────────────┘ │
│                                                  │
└─────────────────────────────────────────────────┘
```

### 10.2 扩展点设计

| 扩展方向 | 实现方式 | 说明 |
|----------|----------|------|
| 新增传感器 | 在 Sensor 板添加新 ADC/I2C 驱动 | 扩展 `sensor_data_payload_t` |
| 新增命令 | 在 `*_message.h` 中添加 CMD 宏 | 统一帧格式不变 |
| 新增显示设备 | Mainboard 添加新 SPI 设备线程 | 参考 `nixietube_task.c` |
| 新增 GUI 页面 | Remote 添加 LVGL screen 文件 | 参考 `setup_scr_screen_*.c` |
| 通信协议升级 | 修改 `mainboard_cpr_protocol.h` | 新旧版本通过版本号字段兼容 |

### 10.3 协议版本兼容

建议在帧格式中预留版本字段:

```c
typedef struct __attribute__((packed)) {
    uint8_t  head1;           // 0x55
    uint8_t  head2;           // 0xAA
    uint8_t  version;         // 协议版本(未来扩展)
    uint8_t  len;
    uint8_t  dev_type;
    uint8_t  cmd;
    uint8_t  status;
    uint16_t seq;
    uint8_t  payload[CPR_MAX_PAYLOAD];
    uint16_t crc;
} cpr_packet_v2_t;
```

### 10.4 维护性建议

1. **日志系统**:使用 RT-Thread 的 `LOG_I/LOG_E/LOG_W` 宏统一日志格式,通过 `Record.ulog_cnt` 实现日志序号追踪
2. **在线调试**:通过 `rt_kprintf` 输出 nRF 收发原始数据(hex dump),便于协议层调试
3. **诊断输出**:Mainboard 的 `nRF24L01_Decode_entry` 中包含完整的寄存器诊断输出(STATUS/OBSERVE/FIFO/CONFIG/TX_ADDR/RX_ADDR)
4. **编译开关**:使用 `#if 0` / `#if 1` 控制调试信息输出,避免发布版本性能损耗

### 10.5 资源使用约束

| 资源 | Mainboard | Sensor | Remote | Raster |
|------|-----------|--------|--------|--------|
| 线程栈 | 4096+2048+1024+... | 4096+1024+4096 | 4096+4096+... | N/A (裸机) |
| nRF24L01 FIFO | 32B × 3 | 32B × 3 | 32B × 3 | N/A |
| UART 缓冲 | 2048B | 512B × 2 | N/A | 64B |
| 互斥锁 | nrf24_mutex, uart2_buf_lock | uart2_buf_lock | N/A | N/A |
| 信号量 | nrf24_irq_sem, nrf24_send_sem | 同上 | 同上 | N/A |
| 事件集 | N/A | nrf24l01_events | nrf24l01_events | N/A |

---

## 11. 附录:关键数据结构

### 11.1 全局记录结构(Mainboard)

```c
typedef struct {
    rt_uint8_t   Empty;
    rt_uint16_t  kprintf_cnt;
    rt_uint16_t  ulog_cnt;
    /* 连接管理 */
    rt_uint8_t   wired_connect_flag;          // 0:无线  1:有线
    uint8_t      sensor_connect_pending;      // Sensor 连接请求待处理
    uint8_t      remote_connect_pending;      // Remote 连接请求待处理
    uint8_t      sensor_connected;
    uint8_t      remote_connected;
    uint32_t     last_sensor_heartbeat;
    uint32_t     last_remote_heartbeat;
    /* 命令 ACK 状态 */
    uint8_t      shoke_cmd_ack;
    uint8_t      cc6201_cmd_ack;
    uint8_t      sensor_start_cmd_ack;
    uint8_t      sensor_wsrgb_cmd_ack;
    uint8_t      sensor_motor_cmd_ack;
} RecordStruct;
```

### 11.2 系统配置结构(Mainboard)

```c
typedef struct {
    System_Mode_t current_mode;     // 训练/考核/竞赛
    uint8_t       start_status;     // 0:未开始 1:已开始 2:已结束
    uint8_t       cc6201_state;     // 0:有异物  1:无异物
    uint8_t       eyes_rgb_level;   // 0:濒死(WS2812B最低+OLED黑屏) 1:正常(WS2812B白灯+OLED白底黑圆)
    uint8_t       motor_work_sta;   // 0:关闭 1:随按压 2:自主震动
    Mode_Params_t params[MODE_MAX]; // 各模式参数(时间/达标率/计数)
} System_Config_t;
```

### 11.3 Sensor 板标志结构

```c
typedef struct {
    rt_uint8_t  start;            // 0:未开始 1:已开始
    rt_uint8_t  shoke_ack;        // 震动反馈
    rt_uint8_t  ws2812b_ack;      // RGB 灯反馈
    rt_uint8_t  motor_ack;        // 电机反馈
    rt_uint8_t  cc6201_ack;       // 磁传感器反馈
    rt_uint8_t  last_cc6201_state;// 上次 Hall 状态
} FlagStruct;
```

### 11.4 nRF24L01 数据来源枚举

```c
typedef enum {
    SRC_UNKNOWN = 0,
    SRC_FROM_SENSOR,      // Pipe1
    SRC_FROM_REMOTE,      // Pipe2
    SRC_FROM_MAIN         // 本板(极少用)
} cpr_src_type_t;
```

---

### 11.5 三种模式成绩单打印格式(DM32 热敏打印机)

DM32 热敏打印机通过 RS232 接口连接 Mainboard 板,根据系统模式输出不同内容的成绩单。

详细格式规范参见 [业务需求.md §4](./业务需求.md#4-三种模式成绩单规范)。

#### 训练模式

包含:基础信息 + 参数设定 + 按压报告 + 吹气报告 + 循环统计 + 结果

```
========================================
       心肺复苏操作训练考核
========================================
场次号:__________  考生号:__________
考试时间:____月____日____时____分
----------------------------------------
            参数设定
模式: 训练              限时:____
按压合格率: ____%       吹气合格率: ____%
----------------------------------------
            按压报告
按压平均频率:____次/分
标准频率外次数:____
按压错误总数:____
按压正确:____          正确率:____%
少按次数:____          多按次数:____
位置错误:____          按压不足:____
按压过大:____          未回弹数:____
----------------------------------------
            吹气报告
吹气平均时间:____ms
吹气错误总数:____
吹气正确:____          正确率:____
少吹次数:____          多吹次数:____
吹气过大:____          吹气不足:____
----------------------------------------
5个循环内按压总数:____
5个循环内吹气总数:____
5个循环外按压总数:____
5个循环外吹气总数:____
----------------------------------------
实际用时:____分____秒
急救结果:____
========================================
```

#### 考核模式

在训练模式基础上,**额外包含**:
- 操作流程检查区域(10项操作记录)
- 吹气报告中的"两个循环间按压间断时间"

```
========================================
       心肺复苏操作训练考核
========================================
场次号:__________  考生号:__________
考试时间:____月____日____时____分
----------------------------------------
            参数设定
模式: 考核              限时:____
按压合格率: ____%       吹气合格率: ____%
----------------------------------------
意识判断:____      脉搏检查:____
检查呼吸:____      急救呼叫:____
清除异物:____      开放气道:____
急救前脉搏检查:____
急救后脉搏检查:____
急救前瞳孔检查:____
急救后瞳孔检查:____
----------------------------------------
            按压报告
(同训练模式)
----------------------------------------
            吹气报告
吹气平均时间:____ms
两个循环间按压间断时间:____ms
(其余同训练模式)
----------------------------------------
(循环统计 + 结果同训练模式)
========================================
```

#### 竞赛模式

在训练模式基础上,**额外包含**:
- 吹气报告中的"两个循环间按压间断时间"

```
========================================
       心肺复苏操作训练考核
========================================
场次号:__________  考生号:__________
考试时间:____月____日____时____分
----------------------------------------
            参数设定
模式: 竞赛              限时:____
按压合格率: ____%       吹气合格率: ____%
----------------------------------------
            按压报告
(同训练模式)
----------------------------------------
            吹气报告
吹气平均时间:____ms
两个循环间按压间断时间:____ms
(其余同训练模式)
----------------------------------------
(循环统计 + 结果同训练模式)
========================================
```

#### 三种模式差异对比

| 打印项 | 训练模式 | 考核模式 | 竞赛模式 |
|--------|---------|---------|---------|
| 基础信息(场次号/考生号/时间) | ✓ | ✓ | ✓ |
| 参数设定(模式/限时/合格率) | ✓ | ✓ | ✓ |
| 操作流程检查(10项) | ✗ | ✓ | ✗ |
| 按压报告 | ✓ | ✓ | ✓ |
| 吹气报告 | ✓ | ✓ | ✓ |
| 两个循环间按压间断时间 | ✗ | ✓ | ✓ |
| 循环统计 | ✓ | ✓ | ✓ |
| 实际用时 + 急救结果 | ✓ | ✓ | ✓ |
| 是否强制评分 | 否 | 是 | 是 |

---

## 版本变更记录

| 版本 | 日期 | 变更内容 |
|------|------|---------|
| v1.0 | 2026-04-24 | 初始版本,硬件架构、通信协议、线程架构、数据结构 |
| v2.0 | 2026-04-25 | **重构合并**:统一命令码定义(合并 FRAME_NRF24_ 系列与 CMD_ 系列),删除 Sensor 外设表重复条目,统一子节编号格式;**交叉补充**:§3 各板业务流程总览(引用业务需求.md),颈动脉模拟三状态,三种模式成绩单格式(§11.5),考核模式操作流程检查(§6.4);**协议优化**:明确 5.2 实时帧与 5.3 成绩帧的边界区分 |
| v2.2 | 2026-05-23 | §6.1 补充 Remote UI 连接后的两种场景跳转逻辑 |
| v2.1 | 2026-05-12 | 眼部板由 WS2812B 单组件变更为 OLED(0.66寸) + WS2812B(PA11) 双组件；§3.1 Sensor职责更新、关键外设表新增OLED行、源码结构新增bsp_oled_eye；眼灯亮度由3档改为濒死/正常2态；§5.1.4命令描述、§5.4寄存器说明、§11.2系统配置结构同步更新 |

---

> **文档结束**
>
> 本文档基于 `cpr-assessment-training-machine` 实际代码分析生成。
> 如有架构调整,请同步更新本文档和 [业务需求.md](./业务需求.md) 并通知相关开发人员。
