# CPR 心肺复苏训练考核系统 - 代码完善指南

## 项目概述

CPR 心肺复苏训练考核系统，包含四个硬件板卡：Sensor 板（传感器板）、Mainboard 板（主板）、Remote 板（遥控板）、Raster 板（光栅板）。

## 系统架构

### 通信拓扑

```
Sensor 板 ←→ nRF24L01 ←→ Mainboard 板 ←→ nRF24L01 ←→ Remote 板
    ↕ (UART2)                                                   
Raster 板                                                    
    ↕ (RS485, 有线备用)                                         
Mainboard 板                                                  
```

### 交互规则

- Sensor 板通过 nRF24L01 与 Mainboard 通讯
- Mainboard 板通过 nRF24L01 与 Remote 通讯
- Remote 板不与 Sensor 板直接通讯
- Raster 板通过 UART 串口与 Sensor 板通信
- 有线模式下 Sensor 与 Mainboard 通过 RS485 通讯，Remote 不参与

## 数据流向

### 传感器数据流

```
Raster 光栅板（ST130B 光电传感器采集脉冲）
    ↓ UART 串口计算出按压/吹气参数
Sensor 板（接收数据）
    ↓ nRF24L01 转发
Mainboard 板（业务逻辑处理、评分、显示、语音播报）
    ↓ nRF24L01 反馈
Remote 板（LVGL GUI 显示）
```

### 成绩数据流

```
STM8 光栅板（计算所有成绩数据）
    ↓ UART 串口
Sensor 板（转发成绩）
    ↓ nRF24L01
Mainboard 板（记录到 Flash）
    ↓ 打印
DM32 热敏打印机
```

## 代码完善任务清单

### 1. Raster 板（STM8）完善

#### 1.1 光栅数据采集

- [ ] 实现 ST130B 光电传感器脉冲计数
- [ ] 实现按压方向判断（栅格条向下=按压，向上=吹气）
- [ ] 实现脉冲数到深度/潮气量的转换算法
- [ ] 实现 UART 数据帧发送到 Sensor 板

#### 1.2 成绩计算

- [ ] 实现按压参数计算：平均频率、频率偏差、错误总数、正确次数、正确率、少按/多按、位置错误、按压不足/过大、未回弹数
- [ ] 实现吹气参数计算：平均时间、错误总数、正确次数、正确率、少吹/多吹、吹气过大/不足
- [ ] 实现循环统计：5 个循环内外按压/吹气总数
- [ ] 实现时间统计和结果判定
- [ ] 实现 CRC16 校验

#### 1.3 串口协议

- [ ] 定义 RasterReport_t 数据结构（参考业务框架文档）
- [ ] 实现串口帧格式：HEAD + LEN + DATA + CRC
- [ ] 实现超时重传机制

### 2. Sensor 板完善

#### 2.1 UART2 接收

- [ ] 实现 UART2 接收中断处理
- [ ] 实现数据帧解析（HEAD + LEN + DATA + CRC）
- [ ] 实现 RasterReport_t 结构体接收
- [ ] 实现数据有效性校验

#### 2.2 肩部震动检测

- [ ] 实现 ADC 采集压电陶瓷片信号
- [ ] 实现震动阈值判断
- [ ] 实现肩部拍打动作检测

#### 2.3 头部上仰检测

- [ ] 实现 MPU6050 数据采集
- [ ] 实现欧拉角计算
- [ ] 实现头部上仰角度阈值判断

#### 2.4 nRF24L01 转发

- [ ] 实现成绩数据打包转发到 Mainboard
- [ ] 实现实时传感器数据转发（按压深度、潮气量等）

### 3. Mainboard 板完善

#### 3.1 nRF24L01 接收

- [ ] 实现 Pipe1 接收 Sensor 数据
- [ ] 实现 Pipe2 接收 Remote 指令
- [ ] 实现成绩数据接收和解析

#### 3.2 业务逻辑

- [ ] 实现训练/考核/竞赛三种模式
- [ ] 实现限时逻辑
- [ ] 实现按压/吹气合格率判定
- [ ] 实现急救结果判定

#### 3.3 数据存储

- [ ] 实现成绩单数据结构定义
- [ ] 实现 Flash 存储接口
- [ ] 实现历史记录查询

#### 3.4 打印输出

- [ ] 实现 DM32 热敏打印机驱动
- [ ] 实现成绩单格式化输出
- [ ] 实现中文字库支持

#### 3.5 语音播报

- [ ] 实现 WT588D 语音驱动
- [ ] 实现按压/吹气提示音
- [ ] 实现错误报警音

#### 3.6 显示驱动

- [ ] 实现 TM1629A 数码管显示
- [ ] 实现光条 LED 显示
- [ ] 实现实时数据刷新

### 4. Remote 板完善

#### 4.1 LVGL GUI

- [ ] 实现模式选择界面
- [ ] 实现实时数据显示界面
- [ ] 实现参数设置界面
- [ ] 实现历史记录查看界面

#### 4.2 触摸交互

- [ ] 实现 FT6336U 触摸驱动
- [ ] 实现触摸按键响应
- [ ] 实现手势操作

#### 4.3 数据接收

- [ ] 实现 nRF24L01 数据接收
- [ ] 实现成绩数据接收和显示
- [ ] 实现 LED 颜色控制

## 关键数据结构

### 传感器实时数据

```c
typedef struct {
    uint16_t press_depth;     // 当前按压深度 (mm)
    uint16_t press_freq;      // 当前按压频率 (次/分)
    uint16_t breath_volume;   // 当前潮气量 (ml)
    uint8_t  shoulder_tap;    // 肩部拍打 (0/1)
    uint8_t  head_tilt;       // 头部上仰角度 (度)
    uint8_t  foreign_body;    // 异物检测 (0/1)
    uint16_t crc;
} __attribute__((packed)) SensorRealtime_t;
```

### 成绩单数据

```c
typedef struct {
    uint16_t session_id;         // 场次号
    uint16_t examinee_id;        // 考生号
    uint8_t  mode;               // 模式 (0=训练, 1=考核, 2=竞赛)
    uint16_t time_limit;         // 限时 (秒)
    uint8_t  press_pass_rate;    // 按压合格率 (%)
    uint8_t  breath_pass_rate;   // 吹气合格率 (%)
    
    // 按压报告
    uint16_t press_avg_freq;
    uint16_t press_out_of_std;
    uint16_t press_error_total;
    uint16_t press_correct;
    uint16_t press_accuracy;
    uint16_t press_too_few;
    uint16_t press_too_many;
    uint16_t press_pos_error;
    uint16_t press_insufficient;
    uint16_t press_excessive;
    uint16_t press_no_rebound;
    
    // 吹气报告
    uint16_t breath_avg_time;
    uint16_t breath_error_total;
    uint16_t breath_correct;
    uint16_t breath_accuracy;
    uint16_t breath_too_few;
    uint16_t breath_too_many;
    uint16_t breath_excessive;
    uint16_t breath_insufficient;
    
    // 循环统计
    uint16_t cycle5_press_in;
    uint16_t cycle5_breath_in;
    uint16_t cycle5_press_out;
    uint16_t cycle5_breath_out;
    
    // 结果
    uint32_t actual_duration;    // 实际用时 (秒)
    uint8_t  result;             // 急救结果
    
    uint16_t crc;
} __attribute__((packed)) CPRReport_t;
```

## 通信协议

### nRF24L01 帧格式

```
┌────────┬────────┬──────┬──────────┬──────┬────────┬──────┬────────────┬──────┐
│ HEAD1  │ HEAD2  │ LEN  │ DEV_TYPE │ CMD  │ STATUS │ SEQ  │  PAYLOAD   │ CRC  │
│ 0x55   │ 0xAA   │ 1B   │   1B     │ 1B   │  1B    │ 2B   │  0~24B    │ 2B   │
└────────┴────────┴──────┴──────────┴──────┴────────┴──────┴────────────┴──────┘
```

### 命令码定义

```c
#define CMD_SENSOR_REALTIME    0x01  // 传感器实时数据
#define CMD_SENSOR_REPORT      0x02  // 传感器报告数据
#define CMD_RASTER_REPORT      0x03  // 光栅板成绩报告
#define CMD_REMOTE_CONTROL     0x10  // 遥控器控制指令
#define CMD_REMOTE_STATUS      0x11  // 遥控器状态
#define CMD_MAINBOARD_ACK      0x20  // 主板应答
#define CMD_MAINBOARD_FEEDBACK 0x21  // 主板反馈
```

### UART 帧格式（Raster → Sensor）

```
┌────────┬──────┬──────────────────┬──────┐
│  HEAD  │ LEN  │      DATA        │ CRC  │
│  0xAA  │ 1B   │ RasterReport_t   │ 2B   │
└────────┴──────┴──────────────────┴──────┘
```

## 开发注意事项

1. **RT-Thread 线程安全**：所有共享数据访问必须使用互斥锁或信号量
2. **nRF24L01 Pipe 切换**：发送前需切换为 PTX 模式，发送后切回 PRX
3. **CRC 校验**：所有通信数据必须包含 CRC16 校验
4. **超时重传**：关键指令需要 ACK 确认，超时重试 3 次
5. **数据字节序**：STM8 为大端序，STM32 为小端序，通信时注意字节序转换
6. **Flash 写入**：避免频繁写入，采用追加写入+索引方式
7. **ADC 采样**：压电陶瓷片信号需滤波处理，消除噪声
8. **MPU6050 校准**：开机时需执行校准流程，消除零漂

## 文件目录结构

```
cpr-assessment-training-machine/
├── cpr-sensor-board/           # Sensor 板代码
├── cpr-display-main-board/     # Mainboard 板代码
├── cpr-remote-device/          # Remote 板代码
├── cpr-raster-board/           # Raster 板代码 (STM8)
├── LOG/                        # 文档目录
│   ├── cpr_business_framework.md   # 业务框架文档
│   └── SKILL.md                    # 代码完善指南（本文档）
└── README.md
```
