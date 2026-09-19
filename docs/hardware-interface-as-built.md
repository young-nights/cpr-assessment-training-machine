# 硬件接口现状（代码反推 as-built）—— 基线 main@5490dcb

> **事实源**：仅以仓库代码为准（优先级：cubemx.ioc > uvprojx Device > bsp 源码实证）。`docs/hardware-interface.md` 等设计文档只出现在第 3 章差异对照，不作为实现证据。
> 仓库：`github.com/young-nights/cpr-assessment-training-machine`，本地 `main@5490dcb`（`fix(ws2812b): add HAL_TIM_PWM_MspInit for DMA channel mapping`，`git log -1` 实测）。
> 扫描时间：2026-09-19。实现状态图例：✅已实现 / ⚠️仅定义或部分实现（含零调用、空 handler）/ ❌缺失 / ❓存疑。

---

## 1. 总览：四板角色 + MCU/封装/时钟 + 板间互联

### 1.1 四板角色与 MCU（金标准 = .ioc + uvprojx）

| 子工程 | 角色 | MCU（.ioc 实证） | 封装 | uvprojx/工程 Device 交叉验证 | 时钟源 | OS/工具链 |
|---|---|---|---|---|---|---|
| cpr-display-main-board | 主板/裁判：触摸按键+LED+TM1629A×2/TM1638 显示+WT588D 语音+打印+nRF24 中心节点 | `Mcu.CPN=STM32F103ZET6`，`Mcu.Name=STM32F103Z(C-D-E)Tx`，`Mcu.UserName=STM32F103ZETx`（cubemx.ioc） | `Mcu.Package=LQFP144`（cubemx.ioc） | `<Device>STM32F103ZE</Device>`（cubemx/MDK-ARM/cubemx.uvprojx）；启动文件 startup_stm32f103xe.s | HSE 外部晶振 + PLL×9 → 72MHz：`RCC.PLLSourceVirtual=RCC_PLLSOURCE_HSE`、`RCC.PLLMUL=RCC_PLL_MUL9`、`RCC.SYSCLKFreq_VALUE=72000000`（cubemx.ioc） | RT-Thread（rtconfig.h:49-51 `RT_USING_CONSOLE`/`RT_CONSOLE_DEVICE_NAME "uart1"`） |
| cpr-sensor-device | 头部传感器：设计含眼灯/OLED/压电/电机/CC6201/nRF24；代码现状仅 WS2812B+USART1 | `Mcu.CPN=STM32F103RCT6`，`Mcu.UserName=STM32F103RCTx`（cubemx.ioc） | `Mcu.Package=LQFP64`（cubemx.ioc） | `<Device>STM32F103RC</Device>`（cubemx/MDK-ARM/cubemx.uvprojx）；startup_stm32f103xe.s | HSE+PLL×9=72MHz（cubemx.ioc：`RCC.PLLMUL=RCC_PLL_MUL9`、`RCC.SYSCLKFreq_VALUE=72000000`）；HSE 引脚 `Mcu.Pin0=PD0-OSC_IN`、`Mcu.Pin1=PD1-OSC_OUT` | RT-Thread（rtconfig.h:49-51 console=uart1） |
| cpr-remote-device | 遥控器：LVGL GUI(ST7789)+FT6336U 触摸+3×3 矩阵键+电池管理+nRF24 | `Mcu.CPN=STM32F103RFT6`，`Mcu.UserName=STM32F103RFTx`（cubemx.ioc） | `Mcu.Package=LQFP64`（cubemx.ioc） | `<Device>STM32F103RF</Device>`（cubemx/MDK-ARM/cubemx.uvprojx）；startup_stm32f103xg.s | HSE+PLL×9=72MHz（cubemx.ioc：`RCC.PLLMUL=RCC_PLL_MUL9`、`RCC.SYSCLKFreq_VALUE=72000000`）；ADC 时钟 `RCC.ADCPresc=RCC_ADCPCLK2_DIV6`、`RCC.ADCFreqValue=12000000` | RT-Thread + LVGL v8.3.10（rtconfig.h:204-205 `PKG_LVGL_USING_V080310`、`PKG_LVGL_VER_NUM 0x080310`） |
| cpr-raster-board | 光栅板（新发现）：双光栅正交编码采集（按压深度+吹气），UART1 上报 Sensor 板；纯采集模式 | **STM8S003F3**（`MDK_Project/raster_project.ewp`：`GenDeviceSelectMenu` state=`STM8S003F3\tSTM8S003F3`）；无 cubemx 目录、无 uvprojx，是 IAR 工程 | STM8S003F3 为 20-pin（TSSOP20）；封装字段不在 .ewp 中，标 ❓ | IAR EWSTM8 8.0（`raster_project.eww` + `.vscode/iar-vsc.json`：`"IAR Systems\\Embedded Workbench 8.0"`）；`MDK_Project/` 目录名虽叫 MDK，实为 IAR .eww/.ewp | HSI 内部 16MHz /1 分频（`MDK_System/Scr/app_sys.c:12-21`：`CLK_DeInit()`+`CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1)`，注释"HSI 16MHz"） | 裸机（main.c 无 OS，while(1)+中断；MDK_User/main.c） |

> ⚠️ README.md 板卡表写 remote=`STM32F103RGTx`，与 .ioc/uvprojx 实证 `STM32F103RF(RFTx)` 不符（README 非事实源，列入第 3/4 章）。

### 1.2 板间互联（nRF24 SPI 管脚 / PIPE 映射 / 有线链路 / 供电线索）

#### nRF24L01 无线（星型，主板为中心）

| 板 | SPI 总线与引脚（.ioc） | CSN | CE | IRQ | 驱动层实证 |
|---|---|---|---|---|---|
| mainboard | SPI2：`PB13.Signal=SPI2_SCK`、`PB14=SPI2_MISO`、`PB15=SPI2_MOSI`，Master，`SPI2.CalculateBaudRate=4.5 MBits/s`（cubemx.ioc） | PB12（.ioc `PB12.GPIO_Label=nRF24L01_CSN`） | PD8（.ioc `PD8.GPIO_Label=nRF24L01_CE`） | PD9（.ioc `PD9.GPIO_Label=nRF24L01_IRQ`，`PD9.GPIO_ModeDefaultEXTI=GPIO_MODE_IT_FALLING`，PULLUP） | 总线名 `"spi2"`（macNRF/Src/mainboard_nrf24l01_spi.c:19-21）；NSS 宏→`nRF24L01_CSN`（macNRF/Inc/mainboard_nrf24l01_spi.h:26-27）；IRQ `GET_PIN(D,9)`+`PIN_IRQ_MODE_FALLING`（spi.c:85-90） |
| remote | SPI3：`PB3.Signal=SPI3_SCK`、`PB4=SPI3_MISO`、`PB5=SPI3_MOSI`，Master，`SPI3.CalculateBaudRate=18.0 MBits/s`（cubemx.ioc） | PA15（.ioc `PA15.GPIO_Label=nRF24_CSN`） | PB11（.ioc `PB11.GPIO_Label=nRF24_CE`） | PB10（.ioc `PB10.GPIO_Label=nRF24_IRQ`，Signal=GPXTI10，PULLUP；`NVIC.EXTI15_10_IRQn=true`） | 总线名 `"spi3"`（macNRF/Src/remote_nrf24l01_spi.c:20-22）；NSS 宏→`nRF24_CSN`（remote_nrf24l01_spi.h:32-33）；IRQ `GET_PIN(B,10)`+FALLING（spi.c:86-91） |
| sensor | **无**：.ioc 仅 `Mcu.IP0..5=DMA/NVIC/RCC/SYS/TIM1/USART1`（`Mcu.IPNb=6`），无 SPI IP、无 nRF24 引脚 | ❌ | ❌ | ❌ | applications 下无 macNRF 目录（`find applications -type f` 仅 macBSP/macSYS/main.c）；bsp_sys.h:36 仅留空注释 `/* macNRF 头文件 */` |
| raster | **无**：MDK_FWlib 仅标准外设库全量拷贝，工程源文件（.ewp 列表）仅 main/it/sys/usart/message/tim/gpio/exti/clk/flash/i2c/spi 库文件，应用层无 nRF24 代码（MDK_User/MDK_APP/MDK_System 全扫描） | ❌ | ❌ | ❌ | — |

#### PIPE 与地址映射（主板侧实证）

| 项 | 值 | 代码依据 |
|---|---|---|
| 主板 TX_ADDR / RX_ADDR_P0 | `{0x55,0x0A,0x01,0x89,0xAA}` | macNRF/Src/mainboard_nrf24l01_driver.c:88（tx_addr）、:91（rx_addr_pipe0 同址，收 ACK 用） |
| 主板 PIPE1（→Sensor）地址 | `{0x55,0x0A,0x01,0x89,0x01}` | mainboard_nrf24l01_driver.c:94（rx_addr_pipe1） |
| 主板 PIPE2（→Remote）地址 | 低字节 `0x02`（高 4 字节同 PIPE1） | mainboard_n24l01_driver.c:105（`param->rx_addr_p2 = 0x02`，注释 `RX_ADDR_P2 = RX_ADDR_P1[4:1] + 0x02`） |
| 主板 PIPE3~5 | 低字节 `0xF0`（未用） | mainboard_nrf24l01_driver.c:107-109 |
| Remote TX_ADDR | `{0x55,0x0A,0x01,0x89,0x02}`（= 主板 PIPE2 地址） | macNRF/Src/remote_nrf24l01_driver.c:82（注释"主板 Pipe2 对应的地址"） |
| Remote RX_ADDR_P0 | `{0x55,0x0A,0x01,0x89,0xAA}`（= 主板 TX_ADDR，收 ACK） | remote_nrf24l01_driver.c:85 |
| 帧内设备 ID | Sensor=`0x0005`，Remote=`0x0004` | 主板：macNRF/Inc/mainboard_nrf24l01_message.h:12-16（`DEVICE_REMOTE_ID_L=0x04`、`DEVICE_SENSOR_ID_L=0x05`）；Remote：remote_nrf24l01_message.h:19-20 |
| PIPE 路由约定 | Pipe1→Sensor(0x05)，Pipe2→Remote(0x04) | mainboard_nrf24l01_message.c:394 注释 + 各 case：`pipe_num==NRF24_PIPE_1` 走 `nrf24l01_build_sensor_frame`（:400 等），`NRF24_PIPE_2` 走 `nrf24l01_build_remote_frame`（:404 等） |
| 主板实际 TX 路由 | 连接应答 Sensor→PIPE_1（task.c:232）、Remote→PIPE_2（task.c:293）；`SEND_To_Sensor_Start`→PIPE_1（task.c:335）；`WS2812_Level`→PIPE_1（task.c:355）；`Motor_Status`→PIPE_1（task.c:366）；`Shoke`→PIPE_1（task.c:377）；`CC6201`→PIPE_1（task.c:388）；`Remote_Start_Status`→PIPE_2（task.c:343,408） | macTASK/mainboard_nrf24l01_task.c 对应行 |
| Remote 实际 TX 路由 | 连接请求/模式进出/启动指令全部走 `NRF24_PIPE_2` | macTask/remote_nrf24l01_task.c:165（connect, 500ms 重试）、:311/:314（Mode_In/Out）、remote bsp_key.c:130（按键'3'发 Mode_Out） |

#### 空中帧格式（as-built，实际发送路径）

发送侧一律用 `0x55 0xAA LEN ID_H ID_L TYPE STATUS DATA[n] CRC16-Modbus`：

- 组帧：`out_frame[0]=0x55; [1]=0xAA; [2]=4+data_len; [3..4]=DEVICE_*_ID_H/L; [5]=cmd_type; [6]=cmd_status; …CRC`（mainboard_nrf24l01_message.c:40-62 build_remote_frame、:66-88 build_sensor_frame）；CRC16-Modbus（message.c:17-35 `CrcCalc_Crc16Modbus`）。
- TYPE/STATUS：`FRAME_TYPE_ACT=0x31 / SET=0x32 / GET=0x33 / POST=0x66`，`STATE_ASK=0x01 / ACK=0x02 / ERR=0x00`（mainboard message.h:28-34；remote message.h:34-39 同值）。
- 命令码（payload[0]）：`0x01` 连接、`0x02` Sensor 启动、`0x03` 压电回应、`0x04` WS2812B 眼灯挡位、`0x05` 电机模式、`0x06` CC6201 回应、`0x11` Remote 启动、`0x12` 模式切换、`0x13` 时间设置、`0x21` 启动 ACK、`0x22` 状态同步（mainboard message.h:37-49；remote message.h:44-53）。
- ⚠️ 并存的第二套定义：`mainboard_cpr_protocol.h:41-51` 定义 `cpr_packet_t`（head1/head2/len/**dev_type**/cmd/status/**seq**/payload[24]/**crc**），但已扫描的实际收发路径（message.c build/parse + task.c 调用）未使用该结构（`cpr_build_packet` 调用点未定位，标 ❓存疑，见第 4 章）。

#### 有线链路与供电线索

| 链路 | as-built 状态 | 代码依据 |
|---|---|---|
| raster → sensor（光栅脉冲数据） | raster 侧 ✅：UART1 115200 8N1，TX=PD5、RX=PD6（代码 GPIO_Init 实证）；每 100ms 发 `0xAA 0x04 TYPE CNT_H CNT_L DIR CHK 0x55`；接收 Sensor 下行 `0xAA LEN CMD DATA CHK 0x55`（启动 0x01/停止 0x03/模式 0x11） | raster：app_usart.c:29（`UART1_Init(115200,…)`）、:36-37（`GPIO_Init(GPIOD, GPIO_PIN_6, IN_PU)` / `GPIO_Init(GPIOD, GPIO_PIN_5, OUT_PP)`）；app_message.c:32-55（TX 帧）、:93-180（RX 解析）；MDK_User/main.c:79-108（100ms 发送调度） |
| 同上，sensor 侧 | ❌：sensor .ioc 无 USART2/PA2/PA3（`Mcu.PinsNb=8`，引脚仅 PD0/PD1/PA9/PA10/PA11/PA13/PA14），applications 无协议解析代码 → 对端缺失 | cpr-sensor-device/cubemx/cubemx.ioc（Pin 列表）；find applications 全列表 |
| mainboard 有线检测 | ✅：PC6 `WIRED_CONNECT_CHECK` 输入下拉；`Wired_Read_In()` 读取；检测到有线时挂起 nRF 线程 | .ioc：`PC6.GPIO_Label=WIRED_CONNECT_CHECK`、`PC6.Signal=GPIO_Input`、`GPIO_PuPd=GPIO_PULLDOWN`；bsp_hard.c:14-17；mainboard_main.c:98-104（`rt_thread_suspend(nRF24L01_*`） |
| mainboard 打印/RS232/RS485 | 框架 ✅ / UART 绑定 ❓：.ioc 具备 UART4（PC10/PC11，9600，DMA2_Ch3 RX + DMA2_Ch5 TX）+ USART3（PB10/PB11，115200）+ PRINTER_CTS=PC12；代码打开名为 `"rs232-printer"` 的设备，但实例↔UART 绑定参数未在扫描范围内定位 | .ioc：`Dma.UART4_RX.0.Instance=DMA2_Channel3`、`Dma.UART4_TX.1.Instance=DMA2_Channel5`、`UART4.BaudRate=9600`、`USART3.BaudRate=115200`、`PC12.GPIO_Label=PRINTER_CTS`；printer_task.c:22（`rt_device_find("rs232-printer")`）；rd_dm32_printer.h:17-20（CTS 读宏）；bsp_rs485_drv.h:24-26（`RS485_USING_DMA_RX/RS485_USING_DMA_TX`）；bsp_rs485_dev.c:206 / bsp_rs232_dev.c:208（create 接口存在） |
| 供电线索 | 仅遥控器有代码证据：锂电池+TP5400/TP4500 充电管理，ADC 监测充电电流(IN0/PA0)与电池电压(IN1/PA1)，BAT_EN=PC3 默认输出高 | .ioc：`PA0-WKUP.GPIO_Label=BAT_PROG`(`ADCx_IN0`)、`PA1=BAT_VOL`(`ADCx_IN1`)、`PA3=BAT_CHARG`(Input)、`PC2=BAT_STDBY`(Input PULLUP)、`PC3=BAT_EN`(Output PULLUP `PinState=GPIO_PIN_SET`)、`ADC1.ContinuousConvMode=ENABLE`、`ADC1.NbrOfConversion=2`；bsp_battery.c:19-37（TP5400/Rprog 说明）、:44-59（EN 控制）、:68-84（充电/充满检测）、:98-128（电流/电压换算）；bsp_battery.h:16-22（`REF_VOL_MV=3300`、`RPROG_KOHM=2`、`CURRENT_FACTOR=1100`）；adc_task.c:15-17,63-64,86-87（adc1 ch0/ch1 采集） |
| 主板/sensor/raster 供电管理 | ❌：已扫描代码中无电源管理/充电相关代码 | 三板 applications 全量 grep（bat/power/charge/电源）无命中（sensor C7 批量 grep；mainboard/raster 文件树无相关驱动文件） |

---

## 2. 每板分节：外设清单表

### 2.1 cpr-display-main-board（主板/裁判）—— STM32F103ZETx LQFP144 @72MHz

| 外设 | 用途 | 总线与引脚 | 代码依据（file:line / .ioc 键=值） | 实现状态 |
|---|---|---|---|---|
| USART1 | 调试控制台 | PA9=TX、PA10=RX | .ioc：`PA9.Signal=USART1_TX`、`PA10.Signal=USART1_RX`（Asynchronous）；rtconfig.h:51 `RT_CONSOLE_DEVICE_NAME "uart1"` | ✅ |
| USART3 | 有线通信（RS232/RS485 之一，绑定存疑） | PB10=TX、PB11=RX，115200 | .ioc：`PB10.Signal=USART3_TX`、`PB11.Signal=USART3_RX`、`USART3.BaudRate=115200`、`USART3.Parity=PARITY_NONE`；bsp_rs232_dev.c:208 / bsp_rs485_dev.c:206（框架存在） | ⚠️ 框架在，UART 实例绑定未定位（第4章#2） |
| UART4 | 打印机/RS485 候选（绑定存疑） | PC10=TX、PC11=RX，9600；DMA2_Ch3=RX、DMA2_Ch5=TX | .ioc：`PC10.Signal=UART4_TX`、`PC11.Signal=UART4_RX`、`UART4.BaudRate=9600`、`Dma.UART4_RX.0.Instance=DMA2_Channel3`、`Dma.UART4_TX.1.Instance=DMA2_Channel5`、`NVIC.DMA2_Channel3_IRQn=true` | ⚠️ 同上 |
| SPI2 + nRF24L01 | 无线中心节点（PRX），PIPE1→Sensor、PIPE2→Remote | SCK=PB13、MISO=PB14、MOSI=PB15；CSN=PB12、CE=PD8、IRQ=PD9(EXTI↓) | .ioc：`PB13=SPI2_SCK`/`PB14=SPI2_MISO`/`PB15=SPI2_MOSI`（Full_Duplex_Master）、`PB12=nRF24L01_CSN`、`PD8=nRF24L01_CE`、`PD9=nRF24L01_IRQ`（`GPXTI9`，`GPIO_MODE_IT_FALLING`）；spi.c:19-21,85-90；driver.c:88-109（PIPE 地址）；message.c:382-515（路由） | ✅ |
| CPR nRF24 协议层 | 帧组包/解析/CRC16、命令集 | — | message.c:17-35（CRC16-Modbus）、:40-88（组帧）、:100+（`nrf24l01_portocol_get_command`）；message.h:28-49（TYPE/命令码）；task.c:169（按帧内设备 ID 路由，非 STATUS 管道号） | ✅（发送+接收解析路径均在；第二套 cpr_packet_t 存疑） |
| 触摸按键 ×14 | 假人面板操作按键 | TOUCH_IN1=PD15、IN2=PG1、IN3=PG2、IN4=PG0、IN5=PG3、IN6=PF15、IN7=PF13、IN8=PG4、IN9=PF14、IN10=PE2、IN11=PE3、IN12=PE4、IN13=PE5、IN14=PE6（全部 GPIO_Input 下拉） | .ioc：各 `PD15.GPIO_Label=TOUCH_IN1`…`PE6=TOUCH_IN14`，`Signal=GPIO_Input`、`GPIO_PuPd=GPIO_PULLDOWN`；touch_task.c:38-53（引脚↔功能映射表）、:61-140（扫描状态机 30ms 消抖/1000ms 长按） | ✅ 扫描框架 |
| ├ START 键 | 启动流程+语音 VOICE_2 | 同上 TOUCH_IN1 | touch_task.c:154-173（`WT588D_Set_Cmd(WT588D_ADDR_VOICE_2)`+LED 控制） | ✅ |
| ├ RESET 键 | 复位+语音 VOICE_3 | TOUCH_IN7 | touch_task.c:175-193 | ✅ |
| ├ TRAIN/ASSESS/COMPETITION 键 | 模式切换+语音 | TOUCH_IN2/3/4 | touch_task.c:198-207 / 209-217 / 219-227 | ✅ |
| ├ SETTING 键 | 进出设置模式+语音 VOICE_6/7 | TOUCH_IN9 | touch_task.c:229-279（三模式分支） | ✅ |
| ├ PLUS/MINUS 键 | 设置值±10（限幅） | TOUCH_IN6/5 | touch_task.c:281-291 / 293-305 | ✅ |
| ├ PRINTER 键 | 打印触发 | TOUCH_IN8 | touch_task.c:307-312（仅 `rt_kprintf("Function now is printing in progress.")`） | ⚠️ 占位日志，无打印调用 |
| ├ REMOVE_FOREIGN 键 | 清除异物 | TOUCH_IN10 | touch_task.c:314-317（handler 体为空） | ❌ 空实现 |
| ├ EMERGENCY_CALL 键 | 急救呼叫 | TOUCH_IN11 | touch_task.c:319-322（handler 体为空） | ❌ 空实现 |
| ├ SPHYMOSCOPY 键 | 脉搏检测 | TOUCH_IN13 | touch_task.c:324-327（handler 体为空） | ❌ 空实现 |
| └ CONSCIOUS_JUDGMENT 键 | 意识判断 | TOUCH_IN14 | touch_task.c:329-332（handler 体为空） | ❌ 空实现 |
| 指示灯 LED ×22（枚举） | 流程/模式/按压位置指示 | DEBUG=PE1、CONSCIOUS_JUDGMENT=PF1、SPHYGMOSCOPY=PF0、CHECK_BREATH=PC15、EMERGENCY_CALL=PC14、REMOVE_FOREIGN=PC13、BODY1=PD7、BODY2=PG9、BODY3=PG10、BODY4=PG11、BODY5=PG12、BODY6=PG13、BODY7=PG14、PRINTER=PD10、RESET=PD12、SETTING=PD13、ASSESS=PE9、TRAIN=PG8、MINUS=PE12、PLUS=PG5、COMPETITION=PG7、START=PE10 | .ioc：各 `GPIO_Label=LED_*`、`Signal=GPIO_Output`（如 `PF1.GPIO_Label=LED_CONSCIOUS_JUDGMENT`、`PG9=LED_BODY2`…）；bsp_led.h:15-79（宏）、:81-106（枚举 22 项）、:13（`LED_NUM (30)`）；bsp_led.c:259-375（`LED_Out` switch 全枚举）、:171-234（1ms 扫描/闪烁/呼吸） | ✅ 驱动框架（含闪烁/呼吸/渐变） |
| ├ LED 宏缺陷（6 组） | ON/OFF 同写 `GPIO_PIN_RESET`，物理效果=常灭或不可控 | 涉及：意识判断(PF1)/脉搏(PF0)/检查呼吸(PC15)/急救呼叫(PC14)/清除异物(PC13)/打印(PD10) | bsp_led.h:18-19（Conscious_Judgment ON/OFF 均 RESET）、:21-22（Sphygmoscopy）、:24-25（Check_Breath）、:27-28（Emergency_Call）、:30-31（Remove_Foreign）、:75-76（**Printer，线索之外新发现**：`macLED_Printer_ON()` 也写 RESET） | ⚠️ 已知线索为 5 组，代码实测 **6 组**（多 Printer 一组） |
| TM1629A ×2 | 数码管显示（A=按压矩阵、B=吹气矩阵） | A：DIO=PE13、CLK=PE14、STB=PE15；B：DIO=PA4、CLK=PA5、STB=PA6 | .ioc：`PE13=TM1629A_A_DIO`/`PE14=A_CLK`/`PE15=A_STB`、`PA4=TM1629A_B_DIO`/`PA5=B_CLK`/`PA6=B_STB`；nixietube_task.c:14-26（GPIO 宏）、:107-139（双芯片字节写）、:434-441（初始化命令 0x44/0x8F）、:471/:480（A 显示按压、B 显示吹气） | ✅ |
| TM1638 | LED 光条（按压位置灯条） | DIO=PD0、CLK=PD1、STB=PD2 | .ioc：`PD0=TM1638_DIO`、`PD1=TM1638_CLK`、`PD2=TM1638_STB`；lightbar_task.c:14-19（GPIO 宏）、:23-28（`TM1638_Write_Byte/Set_Cmd/Set_LEDBar` 声明）、:84-114（写实现） | ✅ 驱动在（调用链深度未逐行验证，标 ✅框架） |
| WT588D 语音 | 语音播报（欢迎/模式/开始/复位/打印/按压吹气评价等 17 条） | DATA=PC7（Output PULLUP）、CS=PC8、CLK=PC9、RESET=PD3、BUSY=PD4 | .ioc：`PC7.GPIO_Label=WT588D_DATA`（`GPIO_PuPd=GPIO_PULLUP`）、`PC8=WT588D_CS`、`PC9=WT588D_CLK`、`PD3=WT588D_RESET`、`PD4=WT588D_BUSY`；bsp_wt588d.h:17-24（RST/CS/CLK/DATA 宏）、:29-38（音量 0xE0-0xE7/循环 0xF2/停止 0xFE）、:40-56（语音地址 0x00-0x10 共 17 条）；bsp_wt588d.c:32-48（低位先行 150µs 时序写）、:98（`HAL_GPIO_ReadPin(WT588D_BUSY…)`）；wt588d_task.c:20-51（复位→播 VOICE_0→音量 LEVEL7→busy 轮询线程） | ✅（BUSY 引脚方向存疑，第4章#1） |
| RD-DM32 热敏打印 | 成绩单打印 | CTS=PC12（GPIO_Input）；数据口=UART（绑定存疑） | .ioc：`PC12.GPIO_Label=PRINTER_CTS`、`PC12.Signal=GPIO_Input`；rd_dm32_printer.h:17-20（`Printer_CTS_Read()` 宏）；rd_dm32_printer.c:24-137（复位/进纸/走纸/制表/中文模式等命令集，经 `rs232_send`）；printer_task.c:22-45（打开 `"rs232-printer"` 设备、复位+中文模式+测试打印 `rd_test_print` MSH 命令 :89-101） | ⚠️ 命令集✅，UART 绑定存疑 |
| 有线连接检测 | 有线/无线模式切换 | PC6（Input PULLDOWN） | .ioc：`PC6.GPIO_Label=WIRED_CONNECT_CHECK`；bsp_hard.c:14-17；mainboard_main.c:98-104（有线→挂起 nRF 线程） | ✅ |
| FAL Flash | 参数/记录掉电存储 | 片内 Flash | rtconfig.h:79-82（`RT_USING_FAL`/`FAL_PART_HAS_TABLE_CFG`）；macFlash/macFlash.c、falFlash_Test.c 文件存在（find 实证） | ✅（分区表内容未逐行核对） |
| SWD 调试 | — | PA13=SWDIO、PA14=SWCLK | .ioc：`PA13.Signal=SYS_JTMS-SWDIO`、`PA14=SYS_JTCK-SWCLK` | ✅ |

### 2.2 cpr-sensor-device（头部传感器）—— STM32F103RCTx LQFP64 @72MHz

> .ioc 金标准：`Mcu.IPNb=6`，IP 仅 `DMA/NVIC/RCC/SYS/TIM1/USART1`；`Mcu.PinsNb=8`，引脚仅 PD0-OSC_IN、PD1-OSC_OUT、PA9、PA10、PA11、PA13、PA14、VP_Systick。**无 I2C、无 SPI、无 ADC 外设**。sensor cubemx/Inc/main.h 的 "Private defines" 段为空（无任何引脚宏）。

| 外设 | 用途 | 总线与引脚 | 代码依据（file:line / .ioc 键=值） | 实现状态 |
|---|---|---|---|---|
| USART1 | 调试控制台（唯一通信口） | PA9=TX、PA10=RX | .ioc：`PA9.Signal=USART1_TX`、`PA10.Signal=USART1_RX`；rtconfig.h:51 `RT_CONSOLE_DEVICE_NAME "uart1"`、:83-85（`RT_USING_SERIAL`/`V1`，`RT_SERIAL_RB_BUFSZ 64`） | ✅ |
| TIM1_CH4 PWM + DMA1_Ch4 | WS2812B 眼灯数据输出（PWM 位编码） | **PA11**（`PA11.Signal=S_TIM1_CH4`）；DMA1_Channel4（`Dma.Request0=TIM1_CH4/TRIG/COM`、`Dma.…Instance=DMA1_Channel4`、`Direction=DMA_MEMORY_TO_PERIPH`、HALFWORD、`Priority=DMA_PRIORITY_HIGH`） | .ioc：`SH.S_TIM1_CH4.0=TIM1_CH4,PWM Generation4 CH4`、`TIM1.Channel-PWM\ Generation4\ CH4=TIM_CHANNEL_4`、`TIM1.Period=89`、`NVIC.DMA1_Channel4_IRQn=true`；cubemx main.h：`extern TIM_HandleTypeDef htim1; extern DMA_HandleTypeDef hdma_tim1_ch4_trig_com`；cubemx/Src/main.c:253-258（DMA1 时钟+DMA1_Channel4_IRQn 使能）；bsp_ws2812b.c:475（extern hdma）、:523-524（`__HAL_RCC_TIM1_CLK_ENABLE/__HAL_RCC_DMA1_CLK_ENABLE`）、:636（`HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_4, …)`） | ✅ |
| WS2812B 眼灯 | 上电白光点亮（PWM+DMA 双缓冲） | 同上 PA11 | applications/main.c:49-57（`ws2812b_init(); ws2812b_set_brightness(100); ws2812b_set_all(255,255,255); ws2812b_update();` + DMA 信号量等待）；bsp_ws2812b.h:20（`USE_PWM_METHOD 1`）、:21（`USE_SPI_METHOD 0`）、:85（`LED_COUNT 30`）、:90-92（`PWM_PERIOD 89`/`PWM_HIGH_0 28`/`PWM_HIGH_1 56`）；bsp_ws2812b.c:699-700（DMA1_Channel4 IRQ 在 cubemx/Src/stm32f1xx_it.c） | ✅ 上电路径 |
| └ ws2812b_set_white() | 眼灯状态（0=濒死暗光 /1=正常全白） | — | 定义：bsp_ws2812b.c:824-851；声明：bsp_ws2812b.h:107；**全仓库 grep 仅此 2 处命中（定义+声明），零调用点**（`grep -rn ws2812b_set_white --include=*.c --include=*.h` 实测） | ⚠️ 死代码（复核确认线索） |
| └ WS2812B SPI 备选路径 | SPI2 NSS=PB14 方案（设计口径） | — | bsp_ws2812b.h:21 `USE_SPI_METHOD 0`（**编译禁用**）；:27-28 宏 `SPI2_NSS_GPIO_Port/SPI2_NSS_Pin` 引用 CubeMX 符号，但 sensor cubemx main.h 无该宏、.ioc 无 SPI2 → 该分支当前不可编译 | ❌（禁用且引脚不存在） |
| nRF24L01 | 无线上报（设计口径） | — | .ioc 无 SPI/相关引脚；applications 无 macNRF 目录（find 实证）；bsp_sys.h:36 空占位 `/* macNRF 头文件 */`；bsp_typedef.h:24-25 仅字段 `nrf_if_connected`/`nrf_rec_start_cmd`；bsp_typedef.c:20-21 仅全局 `rt_event_t nrf24l01_events`（无使用者） | ❌ 钩子占位（复核确认线索） |
| OLED 眼屏（0.66" 软 I2C PC10/PC11） | 眼部表情显示（docs 口径） | — | .ioc 无 PC10/PC11（PinsNb=8 列表不含）；sensor main.h 无引脚宏；全仓库 `find -name "bsp_oled_eye*"` 无结果；sensor applications grep `oled|ssd1306|PC10|PC11|0.66` 零命中 | ❌（docs 漂移实锤，见第3章） |
| MPU6050 | 姿态/角度（设计口径） | — | .ioc 无 I2C IP/引脚；bsp_sys.h:41 空占位 `/* macMPU中的头文件 */`；仅类型定义 bsp_typedef.h:44-58（`mpu6xxxStruct`）+:25 全局 `mpu6xxxParameter`（bsp_typedef.c:25）；grep `mpu` 无驱动文件 | ❌ 仅结构体 |
| CC6201 霍尔（异物检测） | 磁检测（设计口径） | — | .ioc 无对应引脚；仅声明 bsp_typedef.h:124 `void CC6201_Hall_Sensor_Ctrl(SWITCH_et sta);` + 标志字段 :95-96（`cc6201_ack/last_cc6201_state`）；**全 sensor applications grep 无该函数定义** | ❌ 仅声明无实现 |
| 空心杯电机（脉搏模拟） | 电机驱动（设计口径） | — | .ioc 无对应引脚；仅字段 bsp_typedef.h:36（`motor_work_sta`）、:94（`motor_ack`）；grep `motor` 无驱动文件 | ❌ 仅字段 |
| 压电陶瓷片（肩部拍打/意识判断） | 振动检测（设计口径） | — | sensor applications grep `piezo|压电|shoke|SHAKE` 仅命中 bsp_typedef.h:92 `shoke_ack` 标志字段一行 | ❌ 仅字段 |
| ADC128S102 / ADS1115 | 外部 ADC（README/docs 口径） | — | .ioc 无 SPI/ADC 外设；applications grep `adc128|ads1115` 零命中 | ❌ |
| 光栅数据 UART 接收 | 接收 raster 脉冲帧（设计口径 UART2 PA2/PA3） | — | .ioc 无 USART2、无 PA2/PA3；applications 无协议解析代码（全文件列表仅 bsp_ws2812b/bsp_sys/bsp_typedef/rtt_system_work/main） | ❌ 对端缺失 |
| rtt_system_work 系统节拍 | 1/10/50/500/1000ms 回调 | — | rtt_system_work.c:16-49：`Timing_1ms/10ms/50ms/500ms/1s` **函数体全部为空**；sysTimer 线程 :84-97 正常创建 | ⚠️ 框架在、回调空 |
| Debug_LED_Ctrl | 调试灯（docs 口径 PA15） | — | bsp_typedef.h:123 仅声明；grep 无定义；.ioc 无 PA15 | ❌ 仅声明 |

### 2.3 cpr-remote-device（遥控器）—— STM32F103RFTx LQFP64 @72MHz + LVGL 8.3.10

| 外设 | 用途 | 总线与引脚 | 代码依据（file:line / .ioc 键=值） | 实现状态 |
|---|---|---|---|---|
| USART1 | 调试控制台 | PA9=TX、PA10=RX | .ioc：`PA9.Signal=USART1_TX`、`PA10.Signal=USART1_RX`；rtconfig.h:53 `RT_CONSOLE_DEVICE_NAME "uart1"` | ✅ |
| SPI1 + ST7789 LCD | GUI 屏（240×320，LVGL） | SCK=PA5、MOSI=PA7（TX-only：`PA5.Mode=TX_Only_Simplex_Unidirect_Master`、`PA7.Mode=TX_Only_Simplex_Unidirect_Master`，`SPI1.CalculateBaudRate=18.0 MBits/s`）；CS=PB0、DC=PC4、RST=PC5、BLK=PB15 | .ioc：`PB0.GPIO_Label=LCD_CS`、`PC4=LCD_DC`、`PC5=LCD_RST`、`PB15=LCD_BLK`（均 GPIO_Output HIGH 速度）；st7789_spi.c:19-21（`TFT_SPI_BUS "spi1"`/`TFT_SPI_NAME "tft_spi1"`）、:34（`rt_hw_spi_device_attach(…TFT_CS_PORT, TFT_CS_PIN)`）、:53-54（`max_hz=42M`、`RT_SPI_MODE_2`）；st7789_driver.c:32-34（RST 复位时序）、:159/:168（BLK 开/关） | ✅ |
| LVGL GUI | 五屏交互（main/menu/operation/setting/data） | 同 LCD | packages/LVGL-v8.3.10/（目录实证）；rtconfig.h:200-205（`PKG_USING_LVGL`、`PKG_LVGL_VER_NUM 0x080310`、线程 prio 6/栈 10240/刷新 30ms）；macGUI/lvgl_custom/setup_scr_screen_main.c/menu.c/operation.c/setting.c/data.c（文件实证）；lv_port_disp.c/lv_port_indev.c（porting 层） | ✅ |
| FT6336U 电容触摸 | 触摸输入 | RST=PA4（.ioc `PA4.GPIO_Label=TOUCH_RST`，Output HIGH 速度）；INT=PA6（.ioc `PA6.GPIO_Label=TOUCH_INT`、`PA6.GPIO_ModeDefaultEXTI=GPIO_MODE_IT_FALLING`、PULLUP）；I2C 数据线：**.ioc 无 I2C IP、无 PB6/PB7** | 驱动：ft6336u_iic.c:15-20（`i2c_name="i2c1"`、`i2c_addr=0x38`、读写标志 0x70/0x71）；ft6336u_iic.h:28-34（RST 宏→PA4）；ft6336u_driver.c:48-50（复位时序）、:116-130（芯片 ID 识别 FT6236G/FT6336G/FT6336U/FT6426）；lv_port_indev.c:236-279（`touchpad_read` 读 `tp_dev_xy.point1_x/y`） | ⚠️ 驱动完整，但 I2C 总线/引脚配置缺失（rtconfig.h:91-92 有 `RT_USING_I2C`+`RT_USING_I2C_BITOPS`，未见 BSP_USING_I2C1/软 I2C 引脚配置；`rt_device_find("i2c1")` 失败路径见 ft6336u_iic.c:34-36）→ 存疑（第4章#5） |
| SPI3 + nRF24L01 | 无线上报（PTX→主板 PIPE2） | SCK=PB3、MISO=PB4、MOSI=PB5（Full_Duplex_Master，18MBits/s）；CSN=PA15、CE=PB11、IRQ=PB10 | .ioc 键见 1.2 表；remote_nrf24l01_spi.c:20-22,31,86-91；driver.c:82-98（TX/RX 地址）；task.c:165（连接请求 PIPE2 500ms 重试）、:311/:314（模式进出 PIPE2） | ✅ |
| 3×3 矩阵键盘 | 物理按键输入 | 行：PB12/13/14（`Matrixkey_Row1/2/3`，GPIO_Input）；列：PC6/7/8（`Matrixkey_Column1/2/3`，GPIO_Output PULLUP，`PinState=GPIO_PIN_SET`） | .ioc 对应键；bsp_key.c:14-18（键值表 `{3,2,1/6,5,4/9,8,7}`）、:29-77（列输出/行读取）、:232-290（扫描+消抖阈值 5）、:300-333（Matrixkey 线程 20ms） | ✅ 扫描框架 |
| ├ 按键'4'/'7' | 设置值加/减 | 同上 | bsp_key.c:140-159（'4'：air/press rate+1、work_time+10，限幅）、:174-191（'7'：递减） | ✅（本地参数调整） |
| ├ 按键'3' | 退出模式→发 nRF24 指令 | 同上 | bsp_key.c:115-136：`nrf24l01_order_to_pipe(_nrf24, Order_nRF24L01_ASK_Data_Mode_Out, NRF24_PIPE_2)`（:130）+ LVGL 切屏 | ✅（唯一发 nRF24 的按键） |
| └ 按键'1'/'2'/'5'/'6'/'8'/'9' | 进设置页/进操作页/开始/复位/打印/关机 | 同上 | bsp_key.c:103-106（'1' if 体空）、:109-112（'2' 空）、:162-165（'5' 空）、:168-171（'6' 空）、:193-199（'8'/'9' 仅注释无代码） | ❌ 空实现 |
| LVGL operation 屏按钮 ×11 | 操作流程 GUI 按键 | 触摸（FT6336U） | 标签实证 setup_scr_screen_operation.c:51(btn11=瞳孔检查)、:82(btn10=呼吸检查)、:114(btn9=脉搏检查)、:145(btn8=急救呼吸)、:177(btn7=清除异物)、:209(btn6=意识判断)、:241(btn5=脉搏无)、:273(btn4=脉搏有)、:305(btn3=瞳孔缩小)、:336(btn2=瞳孔放大)、:367(btn1=瞳孔正常)；事件注册 :614-624（全部 `LV_EVENT_VALUE_CHANGED`） | ⚠️ UI 在，回调无 nRF24 发送 |
| └ operation 按钮回调 | 应触发业务指令 | — | handler 实现 setup_scr_screen_operation.c:413-612：函数体内**仅有** `lv_obj_has_state/lv_obj_clear_state` 互斥选中逻辑（如 :513-514、:532、:553-554、:576-577、:599-600），**无任何 `nrf24l01_order_to_pipe`/Record 写入/发送调用**；macGUI 全目录 grep `nrf|order_to_pipe` 仅命中 setup_scr_screen_main.c:146-148（`Record.nrf_send_start=1` 标志） | ❌ 回调不发指令（复核确认线索：btn6=意识判断、btn11=瞳孔检查、btn3/2/1=瞳孔缩小/放大/正常） |
| └ main 屏启动按钮 | 置启动标志 | 触摸 | setup_scr_screen_main.c:165（`LV_EVENT_PRESSED` 注册）、:146-148（`if(Record.nrf_if_connected){Record.nrf_send_start=1;}`）；实际发送在 remote_nrf24l01_task.c（标志驱动） | ✅ 间接链路 |
| ADC1 电池监测 | 充电电流+电池电压 | IN0=PA0（BAT_PROG）、IN1=PA1（BAT_VOL），连续转换双通道 | .ioc：`ADC1.Channel-0#…=ADC_CHANNEL_0`/`Channel-1=ADC_CHANNEL_1`、`NbrOfConversion=2`、`ContinuousConvMode=ENABLE`、采样 `ADC_SAMPLETIME_1CYCLE_5`；adc_task.c:15-17（设备"adc1"ch0/ch1）、:63-64（`rt_adc_enable`×2）、:86-94（8 次采样均值→电流/电压换算）、:105-113（充电/充满状态判定） | ✅ |
| TP5400 充电管理 GPIO | 充电使能/状态检测 | EN=PC3、CHARG=PA3、STDBY=PC2 | .ioc 见 1.2 供电行；bsp_battery.c:44-59（EN 置位/清零）、:68-71（CHARG 低=充电中）、:81-84（STDBY 低=充满）；bsp_battery.h:16-22（换算常数） | ✅ |
| LED_GREEN | 状态指示 | PC0（.ioc `PC0.GPIO_Label=LED_GREEN`，GPIO_Output） | remote bsp_led.h:15-16（`macLED_GREEN_ON/OFF`）、:21（枚举仅 `LED_Name_Green=0x01`）、:13（`LED_NUM (30)` 占位）；bsp_led.c:264-267（LED_Out 仅 Green 分支）、:292-324（LED 扫描线程 10ms） | ✅（单灯） |
| SWD | — | PA13/PA14 | .ioc：`PA13=SYS_JTMS-SWDIO`、`PA14=SYS_JTCK-SWCLK` | ✅ |

### 2.4 cpr-raster-board（光栅板，新发现）—— STM8S003F3，IAR EWSTM8 8.0，HSI 16MHz，裸机

> 板性质判定：STM8 裸机固件工程（非光栅传感器硬件本身，是编码器采集板）。目录结构 MDK_APP/MDK_FWlib/MDK_Project/MDK_System/MDK_User + IAR .eww/.ewp + STM8S 标准外设库（stm8s_*.c/h 全量）。readme.md 给出深度换算：齿距 2.00mm、25 齿距=50.00mm 行程、4 倍正交=1 脉冲 0.5mm（`深度(mm)=depth_count×2.00/4`）。

| 外设 | 用途 | 总线与引脚 | 代码依据（file:line / 工程键=值） | 实现状态 |
|---|---|---|---|---|
| MCU/时钟 | STM8S003F3，HSI 16MHz/1 | — | raster_project.ewp：`GenDeviceSelectMenu`=`STM8S003F3`；app_sys.c:12-21（`CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1)`） | ✅ |
| TIM1 | 1ms 系统滴答 | 片内 | app_general_tim.c:10-26：`TIM1_DeInit(); TIM1_TimeBaseInit(100, TIM1_COUNTERMODE_UP, 160, 0); TIM1_ARRPreloadConfig(ENABLE); TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE)`（16MHz/100=160kHz，/160=1kHz→1ms）；MDK_User/stm8s_it.c `TIM1_IRQHandler`（vector 0x0D）：`g_system_tick_ms++`、`TimingDelay_Decrement()`、`TIM1_ClearITPendingBit(TIM1_IT_UPDATE)` | ✅ |
| UART1 | 上报光栅数据给 Sensor 板 + 接收控制指令 | **TX=PD5、RX=PD6**（代码 GPIO 实证）；115200 8N1 | app_usart.c:29（`UART1_Init(115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, …UART1_MODE_TXRX_ENABLE)`）、:36-37（`GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT)`=RX；`GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST)`=TX）、:31（RXNE 中断使能）；stm8s_it.c `UART1_RX_IRQHandler`（vector 18，环形队列接收 `UART1_Receive`）；⚠️ 同文件 :12-13 注释写 "UART1-RX→PA4 / TX→PA5" 与代码 GPIO_Init 矛盾（第4章#8） | ✅（引脚以代码 GPIO_Init 为准） |
| 光栅编码器-按压 | 按压深度 4 倍频正交解码 | A=PD3、B=PD4（上拉输入+中断） | app_bsp.c:13-14（`GPIO_Init(GPIOD, GPIO_PIN_3/4, GPIO_MODE_IN_PU_IT)`）、:19（GPIOD `EXTI_SENSITIVITY_RISE_FALL` 双边沿）、:22（ITC 优先级 2）；stm8s_it.c `EXTI_PORTD_IRQHandler`（vector 6）：读 `GPIO_ReadInputPin(GPIOD, GPIO_PIN_3/4)`、quad_table 查表解码、`depth_count_press += delta`、`direction_press=-1回弹/0静止/1下压` | ✅ |
| 光栅编码器-吹气 | 吹气量 4 倍频正交解码 | A=PC6、B=PC7（上拉输入+中断） | app_bsp.c:9-10（`GPIO_Init(GPIOC, GPIO_PIN_6/7, GPIO_MODE_IN_PU_IT)`）、:17（GPIOC 双边沿）、:21（优先级 2）；stm8s_it.c `EXTI_PORTC_IRQHandler`（vector 5）：读 GPIOC PIN_6/7、`depth_count_blow += delta`、`direction_blow=-1泄气/0静止/1充气` | ✅ |
| UART1 TX 协议 | 每 100ms 上报脉冲增量 | — | app_message.c:32-55：帧 `0xAA 0x04 TYPE CNT_H CNT_L DIR CHK 0x55`（CHK=Byte0..5 累加和）；TYPE `0x01`=按压、`0x02`=吹气（app_message.h:18,23-24；main.c:104-108 按 `g_raster_detect_mode`/方向选择帧型）；100ms 调度 main.c:79-89（`g_system_tick_ms - last_send_tick >= 100`） | ✅ |
| UART1 RX 协议 | 接收 Sensor 板控制指令 | — | app_message.c:101-180 `USART1_ProcessRxData`：`0xAA LEN CMD DATA CHK 0x55`；CMD `0x01+0xFF`=开始（清零计数、`raster_state=RASTER_ACTIVE`，:157-165）、`0x03+0xFF`=停止（:166-170）、`0x11+MODE`=模式切换（:171-175；MODE `0x01`按压/`0x02`空闲/`0x03`吹气，app_message.h:41-43） | ✅ |
| 状态机 | IDLE 待机 / ACTIVE 采集 | — | app_sys.h:14-18（`raster_state_t`）；main.c:62-113（主循环：RX 解析→状态机→发送）；main.c:22-23 全局 `volatile raster_state_t raster_state = RASTER_IDLE` | ✅ |
| 深度换算参数 | 0.5mm/脉冲 | — | app_sys.h:5（`#define MM_PER_PULSE_01 5 /* 每脉冲 0.5mm = 5 (0.1mm单位) */`）；cpr-raster-board/readme.md（齿距 2.00mm/25 齿距/4 倍频推导） | ✅ |
| nRF24 / 无线 | — | — | 工程无 nRF24 相关源文件（.ewp 源文件列表 + MDK_User/MDK_APP/MDK_System 全扫描） | ❌（设计上由 Sensor 板转接） |

---

## 3. 与 docs/hardware-interface.md 的差异对照

> 本章 docs 内容仅作对照对象，**不是实现证据**。docs 版本：v1.0，日期 2026-05-20（docs/hardware-interface.md:3-4）；另有 docs/hardware-interface.en.md（英文镜像）与 docs/cpr_business_framework.md（v2.1，2026-05-12）。

### 3.1 设计有、代码无（docs 声称 → 代码实测缺失/不符）

| # | docs 声称（出处 docs 内行号） | 代码实测 | 差异性质 |
|---|---|---|---|
| 1 | OLED 眼屏 0.66" 软 I2C，SDA=PC10/SCL=PC11，"定义在 bsp_oled_eye.c"（hardware-interface.md:341-348,555；cpr_business_framework.md:181,1710 v2.1 变更记录） | 全仓库 `find -name "bsp_oled_eye*"` **零结果**；sensor .ioc `Mcu.PinsNb=8` 无 PC10/PC11；sensor main.h 无引脚宏；applications grep 零命中 | **文件不存在 + 引脚无配置**（已知漂移实例，复核确认） |
| 2 | sensor WS2812B 走 SPI2，NSS=PB14，77 颗灯，0码=0xC0/1码=0xF0（hardware-interface.md:303,325-332） | sensor .ioc 无 SPI2/PB14；代码实为 **TIM1_CH4 PWM+DMA1_Ch4，PA11**（.ioc `PA11.Signal=S_TIM1_CH4`；bsp_ws2812b.c:636）；`USE_SPI_METHOD=0` 禁用（bsp_ws2812b.h:21），77 仅存在于禁用分支宏（bsp_ws2812b.h:44-46），PWM 路径 `LED_COUNT=30`（:85） | 总线方式/引脚/灯数三重不符 |
| 3 | sensor MPU6050 I2C1，SCL=PB0/SDA=PC5（hardware-interface.md:305,334-339） | sensor .ioc 无 I2C IP、无 PB0/PC5；无驱动文件；仅结构体 bsp_typedef.h:44-58 | 全缺 |
| 4 | sensor ADC128S102 SPI3，CS=PA4（hardware-interface.md:304,316-323,381） | sensor .ioc 无 SPI3/PA4；grep 零命中 | 全缺 |
| 5 | sensor nRF24 SPI1，CE=PB7/CSN=PB6/IRQ=PD2（hardware-interface.md:302,308-315,377,543-548 附录） | sensor 无 macNRF 目录、无相关引脚/宏；附录引用的 `cpr-sensor-board/cubemx/Inc/main.h` **路径本身不存在**（实为 `cpr-sensor-device/cubemx/Inc/main.h`，且该文件无任何引脚宏） | 全缺 + 文档路径错误 |
| 6 | sensor 光栅板 UART2，TX=PA2/RX=PA3（hardware-interface.md:300,378,479） | sensor .ioc 无 USART2/PA2/PA3；无解析代码 | 全缺 |
| 7 | sensor UART3 PB10/PB11 协议通信（hardware-interface.md:301,379） | sensor .ioc 无 USART3 | 全缺 |
| 8 | sensor CC6201 霍尔 MAGNETIC_STAT=PC1（hardware-interface.md:363,385） | .ioc 无 PC1；仅声明 bsp_typedef.h:124 无定义 | 全缺 |
| 9 | sensor 空心杯电机 CTRL1=PC2/CTRL2=PC3/KEY1=PC14/KEY2=PC13（hardware-interface.md:350-357,384） | .ioc 无这些引脚；仅字段 bsp_typedef.h:36,94 | 全缺 |
| 10 | sensor 压电陶瓷 SHAKE_DOUT0=PB8/DOUT1=PB9 意识判断（hardware-interface.md:364-365,386） | .ioc 无 PB8/PB9；仅字段 bsp_typedef.h:92 `shoke_ack` | 全缺 |
| 11 | sensor DEBUG_LED=PA15（hardware-interface.md:371） | .ioc 无 PA15；`Debug_LED_Ctrl` 仅声明 bsp_typedef.h:123 | 全缺 |
| 12 | remote FT6336U 走 I2C1，SCL=PB6/SDA=PB7（hardware-interface.md:207,223-224,271） | remote .ioc **无 I2C IP、无 PB6/PB7**（`Mcu.IP0..6=ADC1/NVIC/RCC/SPI1/SPI3/SYS/USART1`）；代码仅按设备名 `"i2c1"` 查找（ft6336u_iic.c:16,30），引脚无任何配置依据 | 引脚无代码/配置支撑（存疑级，第4章#5） |
| 13 | sensor 板带 RS485 有线通信（hardware-interface.md:387） | sensor applications 无 rs485 相关文件 | 全缺 |
| 14 | 系统为"三块独立 STM32F103 板卡"（hardware-interface.md:5,23） | 代码实为 **四板**：cpr-raster-board（STM8S003F3 IAR 完整工程）存在且实现完整采集协议 | docs 缺整个 raster 板章节 |
| 15 | docs 附录 sensor 宏清单（hardware-interface.md:545-553：SPI1_NSS/SPI2_NSS/DEBUG_LED/nRF24_*/SPHYGMUS_*/MAGNETIC_STAT/SHAKE_*） | sensor cubemx/Inc/main.h "Private defines" 段为空，**一个宏都没有**（V21 实测全文） | 附录清单与实际生成头文件完全不符 |
| 16 | mainboard LED"总数 22（bsp_led.h 中 LED_NUM=30 含预留）"（hardware-interface.md:111） | bsp_led.h:13 `LED_NUM (30)`、枚举 22 项（:81-106） | 数字陈述一致，非差异；但 docs 未提及 6 组 ON/OFF 宏缺陷（见 3.2-a） |

### 3.2 代码有、设计无（docs 未覆盖的 as-built 事实）

| # | 代码事实 | 代码依据 | docs 状态 |
|---|---|---|---|
| a | mainboard **6 组** LED 宏 ON/OFF 同写 `GPIO_PIN_RESET`（意识判断/脉搏/检查呼吸/急救呼叫/清除异物/**打印**） | bsp_led.h:18-31,75-76 | docs 完全未提该缺陷；且线索口径 5 组，实测含 Printer 共 6 组 |
| b | **cpr-raster-board 整板**：STM8S003F3、HSI 16M、TIM1 1ms、UART1 PD5/PD6 115200、双正交光栅（按压 PD3/PD4 + 吹气 PC6/PC7）、`0xAA..0x55` 上下行协议、100ms 上报、0.5mm/脉冲 | raster_project.ewp；app_sys.c:12-21；app_usart.c:29,36-37；app_bsp.c:9-22；stm8s_it.c 两个 EXTI handler；app_message.c:32-180；readme.md | hardware-interface.md 无 raster 板章节（仅 sensor 侧声称 UART2 连接） |
| c | nRF24 五管道**地址表**（`{0x55,0x0A,0x01,0x89,AA/01/02/F0…}`） | mainboard driver.c:87-109；remote driver.c:81-98 | docs 5.4 仅列 CE/CSN/IRQ 引脚，无地址分配 |
| d | on-air 实际帧格式=`0x55 0xAA LEN ID_H ID_L TYPE STATE DATA CRC16`（TYPE=0x31/32/33/66，设备 ID 0x0004/0x0005） | message.c:40-88；message.h:12-16,28-34 | docs 5.3 描述的是 `cpr_packet_t`（dev_type+seq 版）——与实际发送路径用的 ID 字段版**不是同一套** |
| e | WT588D **17 条语音地址表**（0x00 欢迎 ~0x10 时间到）+ 音量命令 0xE0-0xE7 | bsp_wt588d.h:29-56 | docs 113-121 仅列 5 根引脚，无语音内容映射 |
| f | remote 矩阵键盘**键值表**（{3,2,1/6,5,4/9,8,7}）与按键功能映射（'3'退出模式发 PIPE2 指令、'4'/'7'调参、其余空实现） | bsp_key.c:14-18,96-205 | docs 237-246 仅列行列引脚，无键值/功能语义 |
| g | mainboard 有线检测→**挂起 nRF 线程**的联动逻辑 | mainboard_main.c:98-104 | docs:167 仅说"检测假人是否通过有线方式连接" |
| h | sensor WS2812B **PWM 双缓冲 DMA 架构参数**：LED_COUNT=30、PWM_PERIOD=89 tick、0码 28/1码 56、HT/TC 双半缓冲、快照防竞态 | bsp_ws2812b.h:85-92；bsp_ws2812b.c:475-543,613-640 | docs 描述的是已被替换的 SPI 77 灯方案，PWM 方案零记载 |
| i | remote TP5400 充电**电流-电阻表**（Rprog 10k→130mA … 1.1k→1000mA，IBAT=VPROG/RPROG×1100）+ 电池分压公式（R104=30k/R105=15k/R106=10k，Vout=(Vin+10)/6） | bsp_battery.c:16-37,89-128；bsp_battery.h:16-22 | docs:258 仅一行"RPROG=2kΩ" |
| j | WT588D_BUSY（PD4）在 .ioc 中配置为 **GPIO_Output**，代码却按输入读取 | .ioc `PD4.Signal=GPIO_Output`（cubemx.ioc:247-250）；bsp_wt588d.c:98 `HAL_GPIO_ReadPin(WT588D_BUSY…)` | docs:121 只标"BUSY"，未反映方向配置矛盾 |
| k | mainboard 线程创建顺序：nRF24→LightBar→NixieTube→Printer→Touch→WT588D→start | mainboard_main.c:74-80 | docs 无线章节无启动顺序记载 |
| l | remote GUI operation 屏 11 按钮的完整标签/位置/互斥关系 | setup_scr_screen_operation.c:48-367,614-624 | docs 无 GUI 层描述 |

---

## 4. 存疑与未接线项清单

| # | 存疑项 | 矛盾/缺口 | 证据 | 建议核验方式 |
|---|---|---|---|---|
| 1 | mainboard WT588D_BUSY（PD4）方向 | .ioc 配 `GPIO_Output`，驱动按输入读（busy 判定） | .ioc cubemx.ioc:247-250（`PD4.Signal=GPIO_Output`）vs bsp_wt588d.c:98（`HAL_GPIO_ReadPin`） | 查硬件原理图 BUSY 线实际方向；若为输入需改 .ioc |
| 2 | mainboard 打印机/RS232/RS485 的 UART 实例绑定 | "rs232-printer" 设备名打开成功与否、绑定 UART3 还是 UART4，实例配置表未在扫描代码中定位 | printer_task.c:22；bsp_rs232_dev.c:208/bsp_rs485_dev.c:206（create 接口，参数来自 pcfg 表，表内容未命中）；.config:931-932（`CONFIG_PKG_USING_RS485/RS232 is not set`，驱动是本地 macBSP 非软件包） | 运行日志或全量 grep 实例注册点（bsp_*_dev 注册调用/INIT_APP_EXPORT 链） |
| 3 | mainboard LED 硬件极性 | 6 组宏 ON/OFF 同值属确定缺陷；其余 16 组宏按 ON=SET/OFF=RESET 假设**高电平点亮**，实际 LED 接法（共阳/共阴）未知 | bsp_led.h:15-79 | 原理图确认 LED 极性；若低有效则所有宏极性需反转 |
| 4 | `bsp_led.h LED_NUM=30` vs 枚举 22 项 | 宏值 30 是占位，真实 LED=22；`LED_DrvScan` 按 `LED_GetNumber()=30` 扫描 30 项，后 8 项索引越出枚举语义（switch 无分支，`LED_Out` 空操作） | bsp_led.h:13,81-106；bsp_led.c:177-179,259-375 | 确认 30 是否预留扩展；建议宏改 22 或补实现 |
| 5 | remote FT6336U 的 `i2c1` 总线是否存在 | 驱动按名查找 `"i2c1"`；rtconfig.h 使能了 `RT_USING_I2C`+`RT_USING_I2C_BITOPS`，但 .ioc 无 I2C 外设/PB6/PB7，rtconfig.h 未见 `BSP_USING_I2C1` 或软 I2C 引脚配置；`rt_device_find` 失败仅打印错误继续（ft6336u_iic.c:34-36）→ 触摸可能整体不可用 | ft6336u_iic.c:15-36；remote rtconfig.h:91-92；remote .ioc IP/Pin 列表；V27（remote main.h grep I2C/PB6/PB7 零命中） | `list_device` 运行时确认；或检查 drv_soft_i2c 引脚宏是否在其他配置头定义 |
| 6 | remote MCU 型号 README 漂移 | README.md 板卡表=`STM32F103RGTx`；.ioc `Mcu.CPN=STM32F103RFT6`/uvprojx `<Device>STM32F103RF</Device>` | README.md（板级分工表）；cpr-remote-device/cubemx/cubemx.ioc；cubemx.uvprojx | 以 .ioc 为准（RF）；README 待修订 |
| 7 | `cpr_packet_t`（protocol.h 版帧）是否在用 | 代码并存两套帧协议定义；实际收发路径用 build_*_frame（ID 字段版）；`cpr_build_packet/cpr_parse_packet` 调用点未在扫描中定位 | mainboard_cpr_protocol.h:41-51,79-82 vs message.c:40-88,400-515 | grep 两函数调用点；确认后删并一套 |
| 8 | raster UART1 引脚注释与代码矛盾 | app_usart.c:12-13 注释"RX→PA4/TX→PA5"，实际 GPIO_Init 为 GPIOD PIN6=RX/PIN5=TX（STM8S003F3 UART1 硬件映射 PD5=TX/PD6=RX 与代码一致，注释错） | app_usart.c:12-13 vs :36-37 | 以代码为准；修正注释 |
| 9 | raster MCU 型号注释漂移 | .ewp=`STM8S003F3`；app_sys.c:7 注释"配置 STM8S003F6P6" | raster_project.ewp `GenDeviceSelectMenu`；app_sys.c:7 | 以 .ewp 为准（F3）；注释待修订 |
| 10 | raster→sensor 有线链路对端 | raster 侧协议完整（TX 帧+RX 命令解析）；sensor 侧无 UART2/无解析代码，链路单边存在 | raster app_message.c 全文 vs sensor .ioc Pin 列表 + applications 文件列表 | sensor 板补 UART+协议层，或确认另有接收板 |
| 11 | sensor 全部设计外设的接线 | nRF24/MPU6050/CC6201/电机/压电/OLED/ADC128/光栅 UART：代码仅存结构体字段+函数声明+空 include 占位，.ioc 仅 USART1+TIM1_CH4 → **这些外设在 as-built 中没有任何引脚依据，不得按 docs 引脚接线** | sensor .ioc（IP=6/Pins=8）；bsp_sys.h:36,41（空占位）；bsp_typedef.h:24-25,36,44-58,92-96,123-124；find/grep 各驱动零命中 | 需求确认后走 CubeMX 重新配置+补 BSP；当前状态标 ❌ |
| 12 | mainboard 触摸 4 业务键空实现 | 清除异物/急救呼叫/脉搏检测/意识判断 handler 体为空；PRINTER 键仅日志 | touch_task.c:307-332 | 补业务逻辑（LED 宏缺陷修复需同步） |
| 13 | remote GUI operation 屏 11 按钮无指令输出 | 回调仅 UI 互斥选中；矩阵键 '1'/'2'/'5'/'6'/'8'/'9' 同为空 → 遥控器当前仅键'3'（退模式）真正发出 nRF24 指令 | setup_scr_screen_operation.c:413-612；bsp_key.c:103-199 | 补回调→`nrf24l01_order_to_pipe` 映射（对照 mainboard message.h:37-49 命令码） |
| 14 | sensor `rtt_system_work` 节拍回调全空 | Timing_1ms/10ms/50ms/500ms/1s 函数体为空，sysTimer 线程在跑但无业务 | sensor rtt_system_work.c:16-49 | 与 #11 一并补 |
| 15 | `ws2812b_set_white()` 死代码 | 定义+声明存在、全仓库零调用；眼灯状态业务（濒死/正常）未接线 | bsp_ws2812b.c:824；bsp_ws2812b.h:107；grep 仅 2 命中 | 待 nRF24 眼灯挡位命令（mainboard 侧 0x04 已发 PIPE1，msg.c:458-471）在 sensor 侧实现接收后调用 |
| 16 | mainboard 眼灯/电机/压电/CC6201 的下行命令有发无收 | 主板按 PIPE1 组帧发送 WS2812_Level/Motor/Shoke/CC6201 回应（task.c:355,366,377,388），sensor 侧无接收实现 | mainboard_nrf24l01_task.c 对应行 vs sensor 无 macNRF | 同 #11/#15 |
| 17 | raster 板封装/供电 | .ewp 仅给型号 STM8S003F3，封装字段不在工程文件中；无电源管理代码 | raster_project.ewp；MDK_* 全扫描 | 按 STM8S003F3P6(TSSOP20) 常见形态推断需原理图确认，标 ❓ |
| 18 | docs v2.1 "bsp_oled_eye.c" 漂移 | docs 三处引用（hardware-interface.md:555 / .en.md:555 / cpr_business_framework.md:181,1710），仓库实测无此文件 | find 全仓库零结果 | 入库新文档时同步提示主管修订旧 docs |

---

*Clerk 检索完毕。本文档每一行均标注 file:line 或 .ioc/工程键值；docs 内容仅出现于第 3 章对照；不确定项一律标 ❓/存疑，未编造任何引脚或型号。*
