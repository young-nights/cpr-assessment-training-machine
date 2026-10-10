# 光栅板 ↔ Sensor ↔ Mainboard 数据链路开发规划

- **版本**: v2.0
- **日期**: 2026-09-27
- **范围**: 从光栅板数据采集到 Mainboard 实时显示的完整数据链路
- **数据流**: `光栅板(Raster) → Sensor → Mainboard`
- **进度**: 连接/控制指令已完成 | 数据采集→显示→成绩 待打通

---

## 一、完整数据链路现状

```
光栅板(Raster)              Sensor 板                    Mainboard 板
STM8S003F3                  STM32F103RC                  STM32F103ZE
───────────                 ───────────                  ───────────
A/B相中断计数                UART2 接收解析                NRF24L01 接收
    │                           │                            │
    │ 8字节脉冲帧(每100ms)       │                            │
    ├──── UART1 ──────────────►│                            │
    │  0xAA 0x04 TYPE CNT DIR   │ 累加脉冲→计算深度            │
    │  CHK 0x55                 │ 计算频率/事件检测            │
    │                           │ 计算成绩(RasterReport)      │
    │                           │                            │
    │ 开始/停止/模式切换指令       │                            │
    │◄──── UART1 ───────────────┤                            │
    │  0xAA 0x02 CMD DATA       │                            │
    │  CHK 0x55                 │                            │
    │                           │                            │
    │                           │ 实时数据帧(CMD=0x10)         │
    │                           ├──── NRF24L01 ────────────►│
    │                           │  深度/频率/位置/次数          │ 数码管/光条显示
    │                           │                            │
    │                           │ 成绩帧(CMD=0x66)            │
    │                           ├──── NRF24L01 ────────────►│
    │                           │  RasterReport_t(64B)       │ Flash存储+打印
    │                           │                            │
    │                           │◄──── NRF24L01 ─────────────┤
    │                           │  停止(CMD=0x07)/复位(0x08)  │
    │                           │  眼灯(0x04)/电机(0x05)      │
```

---

## 二、光栅板 ↔ Sensor 通信（UART2）

### 2.1 帧协议现状

| 方向 | 帧类型 | 格式 | Sensor 端 | Raster 端 | 状态 |
|------|--------|------|-----------|-----------|------|
| Raster→Sensor | 脉冲增量帧(100ms) | `0xAA 0x04 TYPE CNT_H CNT_L DIR CHK 0x55` | ✅ 解析+累加 | ✅ 发送 | ✅ |
| Sensor→Raster | 开始采集 | `0xAA 0x02 0x01 0xFF CHK 0x55` | ✅ 发送 | ✅ 接收→ACTIVE | ✅ |
| Sensor→Raster | 停止采集 | `0xAA 0x02 0x03 0xFF CHK 0x55` | ✅ 发送 | ✅ 接收→IDLE | ✅ |
| Sensor→Raster | 模式切换(按压) | `0xAA 0x02 0x11 0x01 CHK 0x55` | ✅ 发送 | ✅ 切按压模式 | ✅ |
| Sensor→Raster | 模式切换(空闲) | `0xAA 0x02 0x11 0x02 CHK 0x55` | ✅ 发送 | ✅ 切空闲模式 | ✅ |
| Sensor→Raster | 模式切换(吹气) | `0xAA 0x02 0x11 0x03 CHK 0x55` | ✅ 发送 | ✅ 切吹气模式 | ✅ |

### 2.2 数据字段语义

**脉冲增量帧**（Raster→Sensor，每 100ms）:
```
TYPE:  0x01=按压数据, 0x02=吹气数据
CNT:   int16_t, 自上次发送以来的脉冲增量（正=正向, 负=反向）
DIR:   int8_t, -1=回弹/泄气, 0=静止, 1=下压/充气
CHK:   Byte[0]~[5] 累加和
```

**Sensor 端处理**（`uart2_protocol.c`）:
```
按压: g_raster_press_cumulative += pulse_count
      g_raster_press_depth_01mm = cumulative × 5  (每脉冲 0.5mm)
吹气: g_raster_blow_cumulative += pulse_count
      g_raster_blow_depth_01mm = cumulative × 5
```

### 2.3 待完善项

- [ ] **传感器验证**: 光栅板上电→Sensor 接收脉冲帧→深度值正确（硬件测试）
- [ ] **开始/停止联动**: Sensor 收到 Mainboard 开始指令后发送 Raster 开始帧
- [ ] **停止联动**: Sensor 收到 Mainboard 停止指令后发送 Raster 停止帧
- [ ] **模式切换集成**: 三路联合判别结果驱动 Raster 模式切换（已在 `uart2_protocol.c` 实现）

---

## 三、Sensor ↔ Mainboard 通信（NRF24L01）

### 3.1 命令矩阵（✅/❌）

| 命令 | CMD | 方向 | Sensor 端 | Mainboard 端 | 状态 |
|------|-----|------|-----------|-------------|------|
| 连接请求 | 0x01 | Sensor→MB | ✅ | ✅ | ✅ |
| 开始指令 | 0x02 | MB→Sensor | ✅ | ✅ | ✅ |
| 震动上报 | 0x03 | Sensor→MB | ✅ | ✅ | ✅ |
| 眼灯状态 | 0x04 | MB→Sensor | ✅ | ✅ | ✅ |
| 电机状态 | 0x05 | MB→Sensor | ✅ | ✅ | ✅ |
| 异物上报 | 0x06 | Sensor→MB | ✅ | ✅ | ✅ |
| **实时数据** | **0x10** | Sensor→MB | ❌ | ❌ | **❌** |
| **停止指令** | **0x07** | MB→Sensor | ❌ | ❌ | **❌** |
| **复位指令** | **0x08** | MB→Sensor | ❌ | ❌ | **❌** |
| **成绩上报** | **0x66** | Sensor→MB | ❌ | ❌ | **❌** |

### 3.2 待完善项（同 v1.0）

- [ ] 实时数据帧(CMD=0x10)上报与解析
- [ ] Mainboard 定时回调+数码管/光条显示
- [ ] 停止/复位指令(CMD=0x07/0x08)
- [ ] 成绩数据上报(CMD=0x66)
- [ ] 按压位置实时上报

---

## 四、开发任务（按依赖顺序）

### 任务 1: Raster↔Sensor 数据采集链路验证 ⭐前提

> **临床意义**: 施救者按压胸部 → 光栅编码器检测 → Sensor 计算深度/频率。这是所有临床数据的**源头**。

**当前状态**: 代码已实现（Raster 发送 + Sensor 解析），需**硬件联调验证**

**验证步骤**:
1. 光栅板上电 → 进入 IDLE 状态
2. Sensor 发送开始帧 → Raster 进入 ACTIVE，每 100ms 发送脉冲帧
3. 手动移动栅格条 → Sensor `g_raster_press_cumulative` 正确累加
4. Sensor 发送模式切换帧 → Raster 正确切换按压/吹气/空闲模式
5. Sensor 发送停止帧 → Raster 回到 IDLE

**验证标准**: TC-RASTER-006~013 + TC-PRESS-004~008

---

### 任务 2: Sensor 开始/停止→Raster 联动

> **临床意义**: Mainboard 按"开始"→ Sensor 启动采集 → Raster 开始计数；按"停止"→ 全链路停止。

**当前状态**: `USART2_Send_Start_To_Raster()` / `USART2_Send_Stop_To_Raster()` 已实现，但**未接入 NRF 命令处理流程**

**实施步骤**:
1. Sensor 收到 Mainboard `START_CMD(0x02)` → 调用 `USART2_Send_Start_To_Raster()`
2. Sensor 收到 Mainboard `STOP_CMD(0x07)` → 调用 `USART2_Send_Stop_To_Raster()`
3. Sensor 收到 Mainboard `RESET_CMD(0x08)` → 发送停止帧 + 清零数据

---

### 任务 3: Sensor 实时数据上报→Mainboard

> **临床意义**: 按压深度/频率实时传到 Mainboard 数码管/光条，施救者看到"操作→反馈"闭环。

**Sensor 端**:
- 新增 `CMD_SENSOR_DATA(0x10)` 帧构建
- Payload: 深度/频率/位置/次数（见 §5.1）
- 触发时机: 每 200ms 或按压事件后

**Mainboard 端**:
- 新增 `CMD_SENSOR_DATA(0x10)` 解析
- 更新 `MySysCfg.params[]` 显示字段

---

### 任务 4: Mainboard 定时回调+显示

> **临床意义**: 数码管显示按压频率、光条显示按压深度、倒计时显示剩余时间。

**`rtt_system_work.c`**:
- `Timing_1ms()`: LED 扫描
- `Timing_500ms()`: 心跳检测
- `Timing_1s()`: 倒计时递减

**显示**: `nixietube_task.c` + `lightbar_task.c` 已有函数待调用

---

### 任务 5: 成绩数据上报

> **临床意义**: 训练结束→成绩单→临床能力评定。

**流程**: Mainboard 发停止 → Sensor 停止采集 → `Calculator_Finalize()` → 成绩 NRF 上报 → Mainboard Flash 存储 → 打印

---

## 五、Payload 格式定义

### 5.1 实时数据帧 (CMD=0x10, Sensor→Mainboard)

```
字节    字段                类型     说明
[0-1]   press_depth         uint16   当前按压深度 (0.1mm)
[2-3]   press_freq          uint16   当前按压频率 (次/分)
[4-5]   blow_depth          uint16   当前吹气深度 (0.1mm)
[6]     body_led_type       uint8    按压位置 (0=无, 1~7=方位)
[7-8]   press_total         uint16   按压总次数
[9-10]  press_correct       uint16   按压正确次数
[11-12] blow_total          uint16   吹气总次数
[13-14] blow_correct        uint16   吹气正确次数
[15]    cycle_count          uint8    已完成循环数
[16-17] reserved             uint16   预留
```

### 5.2 成绩上报帧 (CMD=0x66)

- Payload = `RasterReport_t`（64 字节，见 `app_calculator.h`）
- 触发: 收到停止指令后

---

## 六、开发顺序

```
任务1 (Raster↔Sensor 验证)    ←── 前提，确保数据源头正确
     │
任务2 (开始/停止联动)          ←── 联动控制采集生命周期
     │
任务3 (实时数据上报)           ←── 数据流入 Mainboard
     │
任务4 (Mainboard 显示)         ←── 施救者看到反馈
     │
任务5 (成绩上报)               ←── 训练结束输出成绩单
```

---

## 七、测试用例映射

| 任务 | 测试用例 |
|------|---------|
| 任务1 Raster↔Sensor | TC-RASTER-001~015 + TC-PRESS-004~008 + TC-BLOW-004~005 |
| 任务2 联动 | TC-START-003/006/009 + TC-RASTER-010 (停止) |
| 任务3 实时数据 | TC-PRESS-003 (位置上报) |
| 任务4 显示 | TC-MAIN-PER-008~009 |
| 任务5 成绩 | TC-CALC-001~019 + TC-MAIN-PER-013~014 |
