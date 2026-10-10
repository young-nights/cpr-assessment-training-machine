# Sensor ↔ Mainboard 交互功能开发规划

- **版本**: v1.0
- **日期**: 2026-09-27
- **范围**: 仅聚焦 Sensor 板与 Mainboard 板之间的通信交互与数据流
- **进度**: 连接/开始序列/眼灯/电机/异物/震动 已完成 | 实时数据/成绩/显示 待开发

---

## 一、两端交互现状总览

### 1.1 NRF24L01 命令矩阵（已完成 ✅ / 未完成 ❌）

| 命令 | CMD | 方向 | Sensor 端 | Mainboard 端 | 状态 |
|------|-----|------|-----------|-------------|------|
| 连接请求 | 0x01 | Sensor→MB | ✅ 发送+解析 | ✅ 接收+ACK | ✅ |
| 开始指令 | 0x02 | MB→Sensor | ✅ 接收+ACK | ✅ 发送(step0) | ✅ |
| 震动上报 | 0x03 | Sensor→MB | ✅ 发送 | ✅ 接收+ACK | ✅ |
| 眼灯状态 | 0x04 | MB→Sensor | ✅ 接收+ACK | ✅ 发送(step1) | ✅ |
| 电机状态 | 0x05 | MB→Sensor | ✅ 接收+ACK | ✅ 发送(step2) | ✅ |
| 异物上报 | 0x06 | Sensor→MB | ✅ 发送 | ✅ 接收+ACK | ✅ |
| **按压数据** | 0x10 | Sensor→MB | ❌ 未实现 | ❌ 未实现 | **❌** |
| **吹气数据** | 0x10 | Sensor→MB | ❌ 未实现 | ❌ 未实现 | **❌** |
| **成绩上报** | 0x66 | Sensor→MB | ❌ 未实现 | ❌ 未实现 | **❌** |
| **停止指令** | 0x07 | MB→Sensor | ❌ 未实现 | ❌ 未实现 | **❌** |
| **复位指令** | 0x08 | MB→Sensor | ❌ 未实现 | ❌ 未实现 | **❌** |

### 1.2 数据流现状

```
✅ 已通:  连接握手 → 开始序列 → 眼灯/电机控制 → 异物/震动上报
❌ 断链:  按压/吹气实时数据 → Mainboard 显示
❌ 断链:  成绩数据 → Mainboard 存储/打印
❌ 断链:  停止/复位指令 → Sensor 状态切换
```

---

## 二、待开发任务（按依赖顺序）

### 任务 1: 实时按压/吹气数据上报（Sensor→Mainboard）

> **临床意义**: 施救者按压时，Mainboard 数码管实时显示**按压深度光条**和**频率**，形成"操作→反馈→调整"闭环。

**Sensor 端**:
- `sensor_nrf24l01_message.c` 新增 `CMD_SENSOR_DATA(0x10)` 帧构建函数
- Payload 结构（20 字节）:
  ```
  [press_depth:2] [press_freq:2] [blow_depth:2] [body_led:1]
  [press_total:2] [press_correct:2] [blow_total:2] [blow_correct:2]
  [cycle_count:1] [reserved:2]
  ```
- 在 `hardware_task.c` 或 `uart2_protocol.c` 数据更新后触发发送（每 200ms 或事件驱动）

**Mainboard 端**:
- `mainboard_nrf24l01_message.c` 新增 `CMD_SENSOR_DATA(0x10)` 解析分支
- 解析 payload 到 `MySysCfg.params[]` 对应字段
- 触发显示更新事件

**验证**: 施救者按压 → Mainboard 数码管显示频率实时变化

---

### 任务 2: Mainboard 定时回调与实时显示（Mainboard 端）

> **临床意义**: 数码管/光条是施救者唯一的实时反馈界面。

**`rtt_system_work.c` 回调填充**:
```
Timing_1ms():   LED_DrvScan() — LED 呼吸/闪烁
Timing_10ms():  触摸按键扫描节拍
Timing_500ms(): 心跳检测（sensor/remote last_heartbeat 超时 3s→断开）
Timing_1s():    倒计时递减，0 时自动停止并触发成绩计算
```

**显示联动**:
- 按压频率 → `nixietube_task.c` 数码管（已有函数待调用）
- 按压深度 → `lightbar_task.c` 光条（`TM1638_Set_LEDBar()` 待调用）
- 倒计时 → `nixietube_task.c`（已有函数待调用）

**验证**: TC-MAIN-PER-008~009 通过

---

### 任务 3: 停止/复位指令（Mainboard→Sensor）

> **临床意义**: 教官按"停止"→ 系统冻结数据、计算成绩；按"复位"→ 回到初始状态准备下一轮。

**Mainboard 端**:
- `nrf24l01_message.c` 新增停止(`0x07`)/复位(`0x08`)发送函数
- `touch_task.c` TOUCH_STOP/TOUCH_RESET 触发发送

**Sensor 端**:
- `sensor_nrf24l01_message.c` 新增 `0x07`/`0x08` 接收处理
- 停止: 置位 `Flag.stop`，停止数据采集，触发成绩计算
- 复位: 清零所有数据，`Flag.start=0`，回到初始状态

**验证**: Mainboard 按停止 → Sensor 停止采集并计算成绩

---

### 任务 4: 成绩数据上报（Sensor→Mainboard）

> **临床意义**: 训练结束后，成绩是评估施救者临床能力的最终依据。

**Sensor 端**:
- `app_calculator.c` 的 `Calculator_Finalize()` 已产出 `RasterReport_t`（64B）
- 新增成绩 NRF 上报（`CMD_TYPE_POST=0x66`），payload=RasterReport_t
- 停止指令触发后发送

**Mainboard 端**:
- 新增成绩帧解析 → 保存到 `cpr_record.c` Flash
- 成绩完成后触发 `TOUCH_PRINTER` 打印

**验证**: TC-CALC-001~019 + TC-MAIN-PER-013~014

---

### 任务 5: 按压位置实时上报（Sensor→Mainboard）

> **临床意义**: Mainboard Body1~7 LED 指示施救者按压位置是否正确（胸骨下半段）。

**Sensor 端**:
- ADC128S102 检测结果（`body_led_type`）打包到 `CMD_SENSOR_DATA` payload
- 位置变化时立即上报

**Mainboard 端**:
- 解析 `body_led` 字段 → 驱动 Body1~7 GPIO LED

**验证**: 按压不同位置 → Mainboard 对应 LED 亮

---

## 三、开发顺序与依赖

```
任务2 (Mainboard 定时+显示)  ←── 最先做，无依赖，解锁显示基础
     │
任务1 (实时数据上报)         ←── 依赖任务2（数据到了要能显示）
     │
任务3 (停止/复位)            ←── 依赖任务1（数据流跑通后才能停）
     │
任务4 (成绩上报)             ←── 依赖任务3（停止后才计算成绩）
     │
任务5 (按压位置)             ←── 可与任务1 并行
```

**建议顺序**: 任务2 → 任务1 → 任务5 → 任务3 → 任务4

---

## 四、新增命令码定义（需同步六处）

| 命令码 | 宏名 | 方向 | 说明 |
|--------|------|------|------|
| 0x10 | `FRAME_NRF24_CMD_SENSOR_DATA` | Sensor→MB | 实时按压/吹气数据 |
| 0x07 | `FRAME_NRF24_CMD_STOP` | MB→Sensor | 停止采集 |
| 0x08 | `FRAME_NRF24_CMD_RESET` | MB→Sensor | 复位到初始状态 |
| 0x66 | `FRAME_NRF24_CMD_REPORT_SCORE` | Sensor→MB | 成绩数据上报 |

> ⚠️ 按 AGENTS.md 约束，命令码变更必须同步：mainboard + sensor + remote + raster + head + docs + 测试用例表

---

## 五、Payload 格式定义

### 5.1 实时数据帧 (CMD=0x10, Sensor→Mainboard)

```
字节   字段                类型      说明
[0-1]  press_depth         uint16   当前按压深度 (0.1mm)
[2-3]  press_freq          uint16   当前按压频率 (次/分)
[4-5]  blow_depth          uint16   当前吹气深度 (0.1mm)
[6]    body_led_type       uint8    按压位置 (0=无, 1~7=方位)
[7-8]  press_total         uint16   按压总次数
[9-10] press_correct       uint16   按压正确次数
[11-12] blow_total          uint16   吹气总次数
[13-14] blow_correct        uint16   吹气正确次数
[15]   cycle_count          uint8    已完成循环数
[16-17] reserved             uint16   预留
```

### 5.2 成绩上报帧 (CMD=0x66, Sensor→Mainboard)

- Payload = `RasterReport_t`（64 字节，见 `app_calculator.h`）
- 触发时机: 收到停止指令后

---

## 六、测试用例映射

| 任务 | 对应测试用例 |
|------|------------|
| 任务1 实时数据 | TC-PRESS-003 (触点位置上报) + TC-MAIN-PER-008 (数码管) |
| 任务2 显示联动 | TC-MAIN-PER-008~009 (数码管+光条) |
| 任务3 停止/复位 | TC-MAIN-PER-004 (复位) + TC-MODE-009 (限时停止) |
| 任务4 成绩上报 | TC-CALC-001~019 + TC-MAIN-PER-013~014 |
| 任务5 按压位置 | TC-PRESS-002~003 (触点检测+上报) |
