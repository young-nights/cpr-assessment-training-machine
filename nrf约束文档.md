# OpenClaw Agent 约束条件 - CPR 项目 nRF24L01 无线通讯完善（最终版）

> **文档目的**：为 OpenClaw Agent 提供 CPR（心肺复苏）三板无线通讯的**最终约束条件和控制流程逻辑**。  
> 严格基于已调优的地址规划、Pipe 隔离、双 pending 标志、来源区分机制，确保 Sensor ↔ Main、Remote ↔ Main 双向可靠通信，同时 Sensor 与 Remote 完全隔离。

---

## 1. 系统角色与通讯目标

| 设备          | 角色 | MCU          | 主要功能                     | 通讯目标 |
|---------------|------|--------------|------------------------------|----------|
| 主显示板      | PRX  | STM32F103ZE  | 控制面板、显示、语音、打印   | 与 Sensor、Remote 双向 |
| 传感器板      | PTX  | STM32F103    | 采集按压/吹气/角度数据       | 仅与 Main 双向 |
| 遥控设备      | PTX  | STM32F103    | 手持 GUI 操作界面            | 仅与 Main 双向 |

**核心约束**：Sensor 与 Remote **严禁直接通信**，必须全部通过主显示板路由。

---

## 2. nRF24L01 地址与 Pipe 规划（最终统一方案）

| 配置项                  | 主显示板 (PRX)                                      | 传感器板 (PTX)                                      | 遥控设备 (PTX)                                      | 说明 |
|-------------------------|-----------------------------------------------------|-----------------------------------------------------|-----------------------------------------------------|------|
| **TX Addr**             | `0x55 0x0A 0x01 0x89 0xAA`                         | `0x55 0x0A 0x01 0x89 0x01`                         | `0x55 0x0A 0x01 0x89 0x03`                         | 主板下发统一用 0xAA |
| **RX Addr_P0**          | `0x55 0x0A 0x01 0x89 0xAA` (公共ACK)               | `0x55 0x0A 0x01 0x89 0xAA` (监听ACK)               | `0x55 0x0A 0x01 0x89 0xAA` (监听ACK)               | 所有设备接收主板 ACK |
| **RX Addr_P1**          | `0x55 0x0A 0x01 0x89 0x01` (监听Sensor)            | `0x55 0x0A 0x01 0x89 0x01` (接收主板专用回复)      | -                                                   | Sensor 专用通道 |
| **RX Addr_P2**          | `0x55 0x0A 0x01 0x89 0x03` (监听Remote)            | -                                                   | `0x55 0x0A 0x01 0x89 0x03` (接收主板专用回复)      | Remote 专用通道 |
| **EN_RXADDR**           | Pipe0 + Pipe1 + Pipe2 开启                          | Pipe0 + Pipe1 开启                                  | Pipe0 + Pipe2 开启                                  | 主板区分来源，设备只监听必要 Pipe |
| **prim_rx**             | ROLE_PRX                                            | ROLE_PTX                                            | ROLE_PTX                                            | 角色固定 |

---

## 3. 连接建立与并发处理流程

### 3.1 连接流程（支持并发）

1. Sensor / Remote 上电后，以 200~500ms 周期发送 `ASK_Connect_Control_Panel`（PTX 模式）。
2. 主板在 Pipe1 收到 Sensor 请求、在 Pipe2 收到 Remote 请求。
3. 主板协议解析层根据 Pipe + 设备ID 设置对应 `pending` 标志：
   - `Record.sensor_connect_pending = 1`
   - `Record.remote_connect_pending = 1`
4. `nRF24L01_Decode_entry` 独立处理每个 pending：
   - Sensor 请求 → 用 **Pipe1** 发送 ACK
   - Remote 请求 → 用 **Pipe2** 发送 ACK
5. 双方收到 ACK 后标记 `connected = 1`，进入定时数据上报/指令下发阶段。

**重要**：两个设备同时发起连接请求时，**不会相互覆盖**，主板会分别回复。

---

## 4. 双向通信控制流程

### 4.1 Sensor → Main（数据上报）
- Sensor 定时（50~100ms）发送 `SENSOR_DATA`（压力、吹气、角度等）
- 主板在 **Pipe1** 接收 → `src = SRC_FROM_SENSOR`
- 主板更新数码管、LED灯条、语音播报等业务

### 4.2 Main → Sensor（指令下发）
- 主板需要下发指令时，短暂切换 PTX → 用 **Pipe1** 发送
- Sensor 在 Pipe1 接收主板专用回复

### 4.3 Remote → Main（模式/参数指令）
- Remote 发送 `REMOTE_CMD`（模式切换、参数设置等）
- 主板在 **Pipe2** 接收 → `src = SRC_FROM_REMOTE`
- 主板处理后通过事件触发 Decode_entry

### 4.4 Main → Remote（反馈）
- 主板下发 LED 状态、成绩等 → 用 **Pipe2** 发送

---

## 5. 关键约束（必须严格遵守）

1. **硬件引脚绝对禁止修改**（见最初约束条件）。
2. **线程架构严格保留**：只在现有 `nRF24L01_Thread_entry`、`nRF24L01_Decode_entry`、`nRF24L01_Data_Transmit_Thread_entry` 内完善。
3. **协议必须使用来源区分**（`cpr_src_type_t`）：`SRC_FROM_SENSOR` / `SRC_FROM_REMOTE`。
4. **ACK 发送必须匹配 Pipe**：
   - Sensor 的 ACK → Pipe1
   - Remote 的 ACK → Pipe2
5. **并发连接**：使用 `sensor_connect_pending` 和 `remote_connect_pending` 独立标志。
6. **有线模式**：主板通过 PC6 检测，有线模式下挂起 nRF 线程，走 RS485。
7. **掉线检测**：连续 5 次心跳缺失视为掉线，触发语音提醒。

---

## 6. 协议与解析要求

- 统一使用 `cpr_packet_t` 结构（含 cmd、seq、payload、crc）。
- 主板接收时必须传入 `cpr_src_type_t *out_src` 参数。
- 解析函数只保留一个统一入口 `nrf24l01_portocol_get_command(...)`（已删除老的 `get_sensor_command`）。
- `nrf24l01_protocol_operation` 必须根据 `src` 参数做分支处理。

---

## 7. 推荐下一步验证流程

1. 烧录修改后的三板代码。
2. 先只开 Main + Sensor，确认连接建立、数据上报、ACK 正常。
3. 再打开 Remote，确认两个设备都能独立连接且互不干扰。
4. 测试同时上电场景，观察 Decode_entry 是否能分别回复两个设备。
5. 检查日志中 Pipe 编号和来源标识是否正确。

---

**文档生成时间**：2026-04-04  
**版本**：v2.0（最终调优版，支持并发连接 + Pipe 隔离）

---

此文件已完全按照你当前代码结构和前面所有调优结果整理，可直接保存为 `OpenClaw_CPR_nRF24_Constraint_Final.md` 使用。

如需增加**掉线检测逻辑**、**心跳实现**或**完整协议头文件示例**，请告诉我，我立即补充。