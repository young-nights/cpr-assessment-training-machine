# CPR Sensor Board - main() 手动初始化顺序

**策略**：main() 后USER_MAIN阶段，手动顺序初始化（无INIT_EXPORT依赖）。

## main() 模板

```c
int main(void)
{
    // 0. HAL基础
    HAL_Init();
    SystemClock_Config();
    
    // 1. 控制台
    rt_console_init();
    
    // 2. 硬件Pin复用
    MX_GPIO_Init();
    
    // 3. 通信总线
    USART2_Init(); USART3_Init();  // UART/RS485
    
    // 4. 外设设备
    adc128s102_spi_init();         // ADC SPI
    ads1115_device_init();         // ADS1115 I2C
    
    // 5. 高级组件
    mpu6050_init();                // IMU
    
    // 6. 线程启动（prio降序）
    mpu6xxx_calibrate_thread_init();  // prio6
    adc128s102_thread_init();         // prio9  
    nRF24L01_Thread_Init();           // prio10 stack4096
    uart2_decodeThread_Init(); uart3_decode_thread_init();  // prio11
    WS2812B_Thread_Init();            // prio12
    
    return 0;
}
```

## 优先级理由

```
高prio先启动：IMU Calib(6) → ADC(9) → nRF(10) → UART(11) → LED(12)
避免：低prio阻塞高prio信号量/mutex
```

## 栈分配

| 线程 | 栈 | 理由 |
|------|----|------|
| nRF24L01 | 4096 | SPI buffer + IRQ栈 |
| UART Decode | 2048 | 环缓冲+解析 |
| WS2812B | 2048 | DMA缓冲 |
| ADC/MPU | 1024-2048 | 简单采样 |

## 验证检查

```c
// main末尾
rt_system_heap_init((void*)RT_HEAP_BEGIN, (void*)RT_HEAP_END);
LOG_I(\"All peripherals/threads initialized OK\");
```

**优势**：全控制顺序，易调试栈异常。

*龙虾主管 2026-04-17*
