# CPR Sensor Board - 初始化顺序规划 (避免栈异常)

**目标**：严格INIT阶段 + 线程prio/栈规划，防死锁/溢出/时序错。

## 🎯 RT-Thread INIT阶段顺序

```
0.START → 1.BOARD → 2.PRELOAD → 3.DEVICE → 4.COMPONENT → 5.ENV → 6.APP → 7.USER_MAIN
```

## 📋 推荐初始化顺序

### 阶段1: BSP/硬件 (INIT_BOARD_EXPORT prio1)
```
1. HAL_Init() / SystemClock_Config()          # 时钟/中断
2. GPIO/I2C/SPI/UART Pinmux                  # 引脚复用
```

### 阶段2: 设备驱动 (INIT_DEVICE_EXPORT prio3)
```
3. USART2_Init() / USART3_Init()             # UART/RS485
4. adc128s102_spi_init()                     # ADC SPI
5. ads1115_device_init()                     # ADS1115 I2C
```

### 阶段3: 组件 (INIT_COMPONENT_EXPORT prio4)
```
6. MPU6050_Init()                            # IMU pkg
```

### 阶段4: APP线程 (INIT_APP_EXPORT prio6)
```
7. nRF24L01_Thread_Init()                    # nRF无线 prio10 stack4096
8. WS2812B_Thread_Init()                     # LED prio12 stack2048
9. adc128s102_thread_init()                  # ADC prio9 stack1024
10. uart2_decodeThread_Init()                 # UART2 prio11 stack2048
11. uart3_decode_thread_init()                # RS485 prio11 stack2048
12. mpu6xxx_calibrate_thread_init()           # IMU校准 prio6 stack2048
```

## 🧵 线程优先级规划

| 优先级 | 线程 | Stack | 说明 |
|--------|------|-------|------|
| **5**  | IMU Euler | 2048 | 最高，实时姿态 |
| **6**  | MPU Calib | 2048 | 校准 |
| **9**  | ADC128S102 | 1024 | 采样 |
| **10** | nRF24L01 | 4096 | 无线IRQ |
| **11** | UART2/3 Decode | 2048 | 协议 |
| **12** | WS2812B | 2048 | LED |

## ⚠️ 栈异常预防

```
1. nRF24L01: 4096 (SPI+IRQ+buffer)
2. UART Decode: 2048 (环缓冲+解析)
3. 禁用低栈线程重入
4. rtconfig.h: RT_USING_OVERFLOW_CHECK
```

## 📝 main.c 示例

```c
INIT_BOARD_EXPORT(bsp_hardware_init);     // 1-2
INIT_DEVICE_EXPORT(device_drivers_init);  // 3-4
INIT_APP_EXPORT(app_threads_init);        // 7-12
```

**编译**：`scons --target=mdk5`

---

*龙虾主管规划 2026-04-17*
