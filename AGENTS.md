# AGENTS.md — cpr-assessment-training-machine 项目约束

> 适用对象：本仓库的一切协作者（人类开发者与 AI Agent）。
> 追加约束直接编辑本文档并提交；约束以本文件为唯一事实源。
> 创建：2026-09-19，依据 2026-09-18/19 两轮代码审查的实证教训。

## 1. 改动源与 Git 纪律

- 修改一律在 WSL 仓库内进行：`/home/whites/embedded_item/cpr-assessment-training-machine`
- 修改前先 `git pull`；修改后必须 `git add` + `commit` + `push`（origin = github.com/young-nights/cpr-assessment-training-machine）
- origin 统一使用 SSH：`git@github.com:young-nights/cpr-assessment-training-machine.git`。HTTPS 方式本机无 git 凭证会导致 push 认证失败（2026-09-19 实证）；本机 SSH key 已注册 GitHub 账号 young-nights，与工作区其他仓库同一规范
- Windows 侧通过 `\\wsl.localhost\Ubuntu-24.04\...` 访问本仓库时只读查看，禁止绕过 git 直接编辑文件
- 提交信息格式：`feat|fix|docs|refactor(scope): 英文描述`（与既有提交历史一致）
- 四个子工程（cpr-display-main-board / cpr-sensor-device / cpr-remote-device / cpr-raster-board）同仓演进；涉及多板协议的改动必须在同一提交内联动，禁止拆散提交

## 2. 事实源层级（硬件接口）

- 现状基准：`docs/hardware-interface-as-built.md`（代码反推，逐项标注 ✅已实现 / ⚠️仅定义零调用 / ❌缺失 / ❓存疑）
- 硬件配置金标准：各板 `cubemx/cubemx.ioc` + `cubemx/MDK-ARM/*.uvprojx`；cpr-raster-board 例外——金标准为 `MDK_Project/raster_project.ewp`（IAR EWSTM8 工程，`MDK_*` 目录名是历史命名，不代表工具链）
- 设计目标文档：`docs/hardware-interface.md`、`docs/cpr_business_framework.md` —— 与 as-built 冲突时以 as-built 为准，设计文档排期修订
- 禁止依据设计文档声称"代码已有某外设/引脚"；接入新硬件前先查 as-built 的实现状态列

## 3. 语义多端同步（眼灯/意识状态红线）

- 眼灯状态字段 `eyes_rgb_level` 当前四处语义不一致：docs v2.1（0=濒死/1=正常）、mainboard `bsp_typedef.h:84` 注释（0=关闭/1=涣散/2=清醒）、测试用例表 TC-START-005（期望 CPR 开始=0 濒死）、运行现实（上电恒白光）。统一定义落地前，禁止任何一端单方面修改该字段语义或赋值
- 协议命令（nRF24 命令码 0x01~0x22、raster UART `0xAA..0x55` 帧）变更必须同步六处：mainboard + sensor + remote + raster + docs + 测试用例表，同一提交内完成
- 代码并存两套帧定义：实际收发路径用 `0x55 0xAA LEN ID_H ID_L TYPE STATE DATA CRC16`（message.c 组帧，设备 ID sensor=0x0005/remote=0x0004）；`mainboard_cpr_protocol.h` 的 `cpr_packet_t`（dev_type+seq 版）未在收发路径使用。确认废弃的一套须删除并同步 docs，禁止继续新增引用

## 4. 文档与代码一致性

- 文档不得引用不存在的源码文件或未配置的引脚（教训：docs 三处引用 `bsp_oled_eye.c`，仓库零命中；README 写 remote=STM32F103RGTx，.ioc 实证 STM32F103RFT6）
- 测试用例表的期望值与日志文案必须与代码实际行为一致后才可勾选（教训：TC-START-005 期望 level=0+日志"眼部状态(濒死)"，代码实际发 level=1+日志"灯光亮度1"）
- 源码注释与代码矛盾时以代码为准并即刻修正注释（已知待修：raster `app_usart.c` 注释 PA4/PA5 vs 代码实际 PD6=RX/PD5=TX；`app_sys.c` 注释 STM8S003F6P6 vs 工程 STM8S003F3）
- 新增或修改外设驱动后，必须同步更新 `docs/hardware-interface-as-built.md` 对应行的引脚依据与实现状态
- sensor 板设计外设（nRF24/MPU6050/CC6201/电机/压电/OLED/光栅 UART）在 .ioc 中零配置，docs 给出的引脚无代码依据——补硬件时必须走 CubeMX/工程重新配置，禁止直接按 docs 引脚接线

## 5. 嵌入式工作区公约（继承）

- `embedded_lib/` 只读；工程路径自包含，工程文件/Makefile 不得引用 `embedded_lib/`
- 源码注释英文 + Doxygen；需求/设计文档可用中文
- 踩坑经验写入 `embedded_experience/{family}-经验文档.md`

## 6. AI Agent 协作流程

- 代码交付链：coder 编写+自审 → evaluator 二次审查 → 主管交付；文档类任务：clerk 产出 → 主管对照代码抽查 → 入库
- 修改前必须说明改动范围；高风险操作（删除文件、批量重命名、协议语义变更）需用户确认后执行
- 补齐空实现（mainboard `touch_task.c` 意识判断/脉搏/急救呼叫/清除异物 handler、remote GUI/矩阵键回调、sensor `rtt_system_work` 节拍回调）时，必须同步更新测试用例表与 as-built 文档

## 变更记录

- 2026-09-19：初建。依据：2026-09-18 意识外设审查（眼灯语义四处矛盾、文档漂移、死代码）+ 2026-09-19 硬件接口 as-built 反推（raster 板完整摸底、README 型号漂移、LED 宏 6 组缺陷、双帧定义并存、sensor 外设零配置实锤）
- 2026-09-19：第 1 节补充 origin SSH 约定（HTTPS push 无凭证认证失败实证，改用 SSH 后推送成功）
