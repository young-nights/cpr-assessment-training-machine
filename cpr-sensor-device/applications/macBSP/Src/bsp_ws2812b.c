/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-01-26     Administrator       the first version
 */

#include "bsp_sys.h"

/* WS2812B SPI访问互斥锁 (shared across SPI/PWM implementations) */
static rt_mutex_t ws2812_mutex = RT_NULL;

#if USE_SPI_METHOD


/* 宏定义SPI名称 */
#define     WS2812B_SPI_NAME    "ws2812b_spi2"
/* 宏定义SPI总线 */
#define     WS2812B_SPI_BUS     "spi2"
/* 创建SPI设备句柄 */
struct rt_spi_device *ws2812b_spi_dev = RT_NULL;



int WS2812B_SPI_Init(void)
{
    //--------------------------------------------------------------------------------------
    /* 将SPI设备名挂载到总线 */
    if (rt_hw_spi_device_attach(WS2812B_SPI_BUS, WS2812B_SPI_NAME, WS2812B_NSS_PORT, WS2812B_NSS_PIN) != RT_EOK)
    {
        LOG_E("Failed to attach SPI device %s.", WS2812B_SPI_NAME);
        return -RT_ERROR;
    }
    /* 查找SPI设备 */
    ws2812b_spi_dev = (struct rt_spi_device *)rt_device_find(WS2812B_SPI_NAME);
    if(ws2812b_spi_dev == NULL){
        LOG_E("LOG:%d. ws2812b spi device is not found!",Record.ulog_cnt++);
        return -RT_ERROR;
    }
    else{
        LOG_I("LOG:%d. ws2812b spi device is successfully!",Record.ulog_cnt++);
    }

    /***
     *! 配置SPI结构体参数
     *! data_width: spi传输数据长度为8bits
     *! max_hz: spi的最大工作频率；
     *! mode-> RT_SPI_MASTER: 主机模式；
     *! mode-> RT_SPI_MODE_0: 工作模式0（ CPOL:0 -- 空闲状态为低电平 ， CPHA:0 -- 第一个边沿采集数据 ）
     *! mode-> RT_SPI_MSB: 通讯数据高位在前低位在后
     * */
    struct rt_spi_configuration ws2812b_spi_cfg = { 0 };

    ws2812b_spi_cfg.data_width = 8;
    ws2812b_spi_cfg.max_hz = 6*1000*1000; /* 10M,SPI max 10MHz,lora 4-wire spi */
    ws2812b_spi_cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_3 | RT_SPI_MSB;
    rt_spi_configure(ws2812b_spi_dev, &ws2812b_spi_cfg); /* 使能参数 */

    //--------------------------------------------------------------------------------------

    return RT_EOK;
}




/* 已经编码好的0/1波形，MOSI可以直接输出的时序位流 */
static uint8_t ws2812b_buffer[WS2812B_LED_NUMS * WS2812B_RGB_BITS] = {0};
/* 原始 24-bit 颜色值（R8G8B8，未编码、未 gamma） */
static uint32_t ws2812b_colors[WS2812B_LED_NUMS] = {0};
/* Gamma 2.8 查找表，下标是 0-255，返回值是 gamma 校正后的 0-255 */
static uint8_t gamma_table[256];
static uint8_t global_brightness = 255;


/***
 * @brief 预先算出 Gamma 2.8 校正曲线的 256 点查表
 * @note  人眼对亮度是非线性的，把PWM占空比从0调到50%，看起来会远不到一半亮，使用Gamma 2.8表对亮度进行校正
 */
static void ws2812b_generate_gamma_table(void)
{
    for (int i = 0; i < 256; i++)
    {
        gamma_table[i] = (uint8_t)(powf((float)i / 255.0f, 2.8f) * 255.0f + 0.5f);
    }
}


/***
 * @brief 把用户传入的颜色值乘以当前的全局亮度值，做线性压缩后再进行Gamma校正，再转变成8-bit数据传到指定指针地址上
 * @note
 */
static void ws2812b_apply_brightness(uint8_t *r, uint8_t *g, uint8_t *b)
{
    *r = gamma_table[(*r * global_brightness) >> 8];
    *g = gamma_table[(*g * global_brightness) >> 8];
    *b = gamma_table[(*b * global_brightness) >> 8];
}


/***
 * @brief 把经过 “亮度设置 + Gamma校正” 后的 R、G、B字节按照WS2812B的位顺序要求，展开成24-bit数据位并存入缓冲区中
 */
static void ws2812b_encode_color(uint8_t r, uint8_t g, uint8_t b, uint8_t *buffer)
{
    uint8_t bits[WS2812B_RGB_BITS] = {0};

    for (int i = 0; i < 8; i++)
        bits[7 - i] = (g & (1 << i)) ? WS2812B_CODE_1 : WS2812B_CODE_0;
    for (int i = 0; i < 8; i++)
        bits[15 - i] = (r & (1 << i)) ? WS2812B_CODE_1 : WS2812B_CODE_0;
    for (int i = 0; i < 8; i++)
        bits[23 - i] = (b & (1 << i)) ? WS2812B_CODE_1 : WS2812B_CODE_0;

    memcpy(buffer, bits, WS2812B_RGB_BITS);
}


/***
 * @brief 设置全局亮度值
 * @param brightness: 0~255
 */
void ws2812b_set_brightness(uint8_t brightness)
{
    if (brightness > 255)
        brightness = 255;
    global_brightness = brightness;

    for (uint16_t i = 0; i < WS2812B_LED_NUMS; i++)
    {
        ws2812b_set_color(i, ws2812b_colors[i]);
    }
}


/***
 * @brief 把第index颗灯设置成指定颜色
 * @param index --> 要设置的灯序号
 *        color --> 要设置的颜色
 */
void ws2812b_set_color(uint16_t index, uint32_t color)
{
    if (ws2812_mutex) rt_mutex_take(ws2812_mutex, RT_WAITING_FOREVER);
    if (index >= WS2812B_LED_NUMS)
    {
        LOG_E("LED index %d out of range.", index);
        if (ws2812_mutex) rt_mutex_release(ws2812_mutex);
        return;
    }

    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    // Gamma补偿
    ws2812b_apply_brightness(&r, &g, &b);
    // 生成24-bit时序位
    ws2812b_encode_color(r, g, b, &ws2812b_buffer[index * WS2812B_RGB_BITS]);
    // 备份颜色
    ws2812b_colors[index] = color;
    if (ws2812_mutex) rt_mutex_release(ws2812_mutex);
}


/***
 * @brief 把当前缓冲区里的波形一次性甩到灯带，点亮 LED
 */
void ws2812b_show(void)
{
    if (ws2812_mutex) rt_mutex_take(ws2812_mutex, RT_WAITING_FOREVER);
    rt_spi_send(ws2812b_spi_dev, ws2812b_buffer, sizeof(ws2812b_buffer));
    if (ws2812_mutex) rt_mutex_release(ws2812_mutex);
}


/***
 * @brief 全灯熄灭，并把显存也清零
 */
void ws2812b_clear(void)
{
    if (ws2812_mutex) rt_mutex_take(ws2812_mutex, RT_WAITING_FOREVER);
    memset(ws2812b_buffer, WS2812B_CODE_0, sizeof(ws2812b_buffer));
    memset(ws2812b_colors, 0, sizeof(ws2812b_colors));
    ws2812b_show();
    if (ws2812_mutex) rt_mutex_release(ws2812_mutex);
}


/***
 * @brief 把整根灯带设成同一种颜色，但不点亮
 */
void ws2812b_set_all(uint32_t color)
{
    for (uint16_t i = 0; i < WS2812B_LED_NUMS; i++)
    {
        ws2812b_set_color(i, color);
    }
}



/***
 * @brief 初始化 编码波形数组、颜色数组以及Gamma校正列表
 */
void ws2812b_table_init(void)
{
    memset(ws2812b_buffer, WS2812B_CODE_0, sizeof(ws2812b_buffer));
    memset(ws2812b_colors, 0, sizeof(ws2812b_colors));

    ws2812b_generate_gamma_table();
}




// WS2812B的测试函数 --------------------------------------------------------------------------------

/* 颜色表，使用宏定义 */
static uint32_t color_table[] = {
    WS2812B_COLOR_RED,    // 红色
    WS2812B_COLOR_ORANGE, // 橙色
    WS2812B_COLOR_YELLOW, // 黄色
    WS2812B_COLOR_GREEN,  // 绿色
    WS2812B_COLOR_CYAN,   // 青色
    WS2812B_COLOR_BLUE,   // 蓝色
    WS2812B_COLOR_PURPLE, // 紫色
    WS2812B_COLOR_WHITE   // 白色
};




/* 全部灯切换颜色 */
void ws2812b_full_color_test(void)
{
    for (uint8_t i = 0; i < sizeof(color_table) / sizeof(color_table[0]); i++)
    {
        ws2812b_set_all(color_table[i]);
        ws2812b_show();
        rt_thread_mdelay(1000);
    }
}


/* 流水灯效果 */
void ws2812b_waterfall_light_test(void)
{
    ws2812b_clear();
    for (uint16_t i = 0; i < WS2812B_LED_NUMS; i++)
    {
        ws2812b_clear();
        ws2812b_set_color(i, WS2812B_COLOR_BLUE);
        ws2812b_show();
        rt_thread_mdelay(50);
    }
}

/* 亮度渐变测试 */
void ws2812b_brightness_gradient_test(void)
{
    ws2812b_set_all(WS2812B_COLOR_GREEN);
    ws2812b_show();

    for (uint8_t brightness = 255; brightness > 10; brightness -= 5)
    {
        ws2812b_set_brightness(brightness);
        ws2812b_show();
        rt_thread_mdelay(50);
    }

    for (uint8_t brightness = 10; brightness < 255; brightness += 5)
    {
        ws2812b_set_brightness(brightness);
        ws2812b_show();
        rt_thread_mdelay(50);
    }
}

/* 呼吸灯效果 */
void ws2812b_breathing_light_test(void)
{
    for (uint8_t brightness = 0; brightness < 255; brightness += 5)
    {
        ws2812b_set_brightness(brightness);
        ws2812b_show();
        rt_thread_mdelay(20);
    }

    for (uint8_t brightness = 255; brightness > 0; brightness -= 5)
    {
        ws2812b_set_brightness(brightness);
        ws2812b_show();
        rt_thread_mdelay(20);
    }
}



/* demo启动函数 */
static void ws2812b_demo_start(void)
{
    /* 确保 ws2812b_init 只执行一次 */
    static rt_bool_t initialized = RT_FALSE;

    if (!initialized)
    {
        WS2812B_SPI_Init();
        ws2812b_table_init();
        initialized = RT_TRUE;
    }

    /* 亮度渐变 */
    ws2812b_brightness_gradient_test();

    /* 全部灯切换颜色 */
    ws2812b_full_color_test();

    /* 流水灯效果 */
    ws2812b_waterfall_light_test();

    /* 呼吸灯效果 */
    ws2812b_breathing_light_test();
}

/* msh命令：启动WS2812B Demo */
int cmd1(void)
{
//    ws2812b_demo_start();
    WS2812B_SPI_Init();
    ws2812b_table_init();
    ws2812b_clear();
    ws2812b_show();
    ws2812b_set_brightness(255);
    rt_thread_mdelay(1000);
    ws2812b_set_all(WS2812B_COLOR_ORANGE);
    ws2812b_show();

    return RT_EOK;
}
MSH_CMD_EXPORT(cmd1, WS2812B_demo);



/**
 * @brief RGB test function - WS2812B color test for console
 *
 * Test sequence:
 *   1. Red (255, 0, 0)
 *   2. Green (0, 255, 0)
 *   3. Blue (0, 0, 255)
 *   4. Cyan (0, 255, 255)
 *   5. Magenta (255, 0, 255)
 *   6. Yellow (255, 255, 0)
 *   7. White (255, 255, 255)
 *   8. Breathing effect (white)
 *   9. Rainbow cycle
 *
 * Usage: type "rgb_test" in MSH console
 */
static int rgb_test(int argc, char **argv)
{
    RT_UNUSED(argc);
    RT_UNUSED(argv);

    rt_kprintf("[WS2812B] === RGB Test Start ===\n");

    /* Init */
    WS2812B_SPI_Init();
    ws2812b_table_init();
    ws2812b_set_brightness(255);

    /* Color table: R, G, B, name */
    typedef struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        const char *name;
    } color_test_t;

    static const color_test_t test_colors[] = {
        {255,   0,   0, "Red"      },
        {  0, 255,   0, "Green"    },
        {  0,   0, 255, "Blue"     },
        {  0, 255, 255, "Cyan"     },
        {255,   0, 255, "Magenta"  },
        {255, 255,   0, "Yellow"   },
        {255, 255, 255, "White"    },
    };

    uint8_t color_count = sizeof(test_colors) / sizeof(test_colors[0]);

    /* Step 1-7: Solid colors */
    for (uint8_t i = 0; i < color_count; i++)
    {
        rt_kprintf("[WS2812B] Color: %s (R=%d, G=%d, B=%d)\n",
                   test_colors[i].name, test_colors[i].r, test_colors[i].g, test_colors[i].b);
        ws2812b_set_all(test_colors[i].r, test_colors[i].g, test_colors[i].b);
        ws2812b_show();
        rt_thread_mdelay(1000);
    }

    /* Step 8: Breathing effect */
    rt_kprintf("[WS2812B] Breathing effect (white)...\n");
    for (uint8_t round = 0; round < 3; round++)
    {
        for (uint8_t brightness = 0; brightness < 255; brightness += 5)
        {
            ws2812b_set_all(brightness, brightness, brightness);
            ws2812b_show();
            rt_thread_mdelay(20);
        }
        for (uint8_t brightness = 255; brightness > 0; brightness -= 5)
        {
            ws2812b_set_all(brightness, brightness, brightness);
            ws2812b_show();
            rt_thread_mdelay(20);
        }
    }

    /* Step 9: Rainbow cycle */
    rt_kprintf("[WS2812B] Rainbow cycle...\n");
    for (uint16_t cycle = 0; cycle < 5; cycle++)
    {
        for (uint16_t led = 0; led < WS2812B_LED_NUMS; led++)
        {
            uint8_t r, g, b;
            uint8_t pos = (led * 256 / WS2812B_LED_NUMS + cycle * 32) & 0xFF;

            /* HSV to RGB (simplified) */
            if (pos < 85) {
                r = pos * 3;
                g = 255 - pos * 3;
                b = 0;
            } else if (pos < 170) {
                pos -= 85;
                r = 255 - pos * 3;
                g = 0;
                b = pos * 3;
            } else {
                pos -= 170;
                r = 0;
                g = pos * 3;
                b = 255 - pos * 3;
            }

            ws2812b_set_color(led, r, g, b);
        }
        ws2812b_show();
        rt_thread_mdelay(100);
    }

    /* Clear */
    ws2812b_clear();
    ws2812b_show();

    rt_kprintf("[WS2812B] === RGB Test Done ===\n");
    return RT_EOK;
}
MSH_CMD_EXPORT(rgb_test, WS2812B RGB color test);



#elif USE_PWM_METHOD


#define DBG_TAG "[WS2812B]"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

// 外部句柄（CubeMX 生成）
extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_tim1_ch4_trig_com;

// 内部宏定义
#define BYTES_PER_LED   3           // RGB=3, RGBW=4
#define BITS_PER_LED    24          // 8 bits per color
#define DMA_BUFF_LEN    (2 * LEDS_PER_DMA_IRQ * BITS_PER_LED)  // 双缓冲总长度
#define DMA_HALF_LEN    (DMA_BUFF_LEN / 2)                     // 半长
#define BITS_PER_IRQ    (LEDS_PER_DMA_IRQ * BITS_PER_LED)      // 每个中断处理的位数

// DMA传输缓冲区：uint16_t (HAL PWM DMA用 HalfWord)
// [FIX] 添加aligned(4)确保DMA对齐
__attribute__((aligned(4))) uint16_t ws2812_buffer[DMA_BUFF_LEN] = {0};

// Control variables
static volatile uint8_t is_updating = 0;    // Transfer in progress flag
static volatile uint16_t led_index = 0;     // Current LED cycle count
rt_sem_t dma_complete_sem = RT_NULL;        // Global semaphore for DMA completion

// Application color buffer (GRB, user modifies this array)
static uint8_t leds_color_data[BYTES_PER_LED * LED_COUNT] = {0};

// [FIX] Problem 1 & 5: Separate mutex for leds_color_data[] protection
static rt_mutex_t data_mutex = RT_NULL;

// [FIX] Problem 1 & 5: Snapshot of leds_color_data[] taken before DMA start.
// ISR reads from this snapshot, never from leds_color_data[] directly.
// This completely decouples ISR from user-thread writes.
static uint8_t snapshot_color_data[BYTES_PER_LED * LED_COUNT] = {0};

// Forward declaration
static void fill_led_pwm_data(uint16_t ledx, uint16_t *ptr);

/**
 * @brief Take a snapshot of leds_color_data[] into snapshot_color_data[] under data_mutex.
 *        Must be called before DMA start. After this, ISR reads from snapshot_color_data[]
 *        which is immutable during the DMA transfer, eliminating the race condition.
 */
static void snapshot_color_buffer(void)
{
    if (data_mutex) rt_mutex_take(data_mutex, RT_WAITING_FOREVER);
    memcpy(snapshot_color_data, leds_color_data, sizeof(snapshot_color_data));
    if (data_mutex) rt_mutex_release(data_mutex);
}

// 初始化
void ws2812b_init(void)
{
    // [FIX] 问题3: 显式使能TIM3和DMA1时钟
    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    // RT-Thread semaphore for DMA completion notification
    dma_complete_sem = rt_sem_create("ws_sem", 0, RT_IPC_FLAG_FIFO);
    RT_ASSERT(dma_complete_sem != RT_NULL);

    // WS2812B SPI access mutex (shared across SPI/PWM implementations)
    if (ws2812_mutex == RT_NULL) {
        ws2812_mutex = rt_mutex_create("ws2812_mtx", RT_IPC_FLAG_FIFO);
        RT_ASSERT(ws2812_mutex != RT_NULL);
    }

    // [FIX] Problem 1 & 5: Separate data_mutex for leds_color_data[] protection
    if (data_mutex == RT_NULL) {
        data_mutex = rt_mutex_create("ws_dat_mtx", RT_IPC_FLAG_FIFO);
        RT_ASSERT(data_mutex != RT_NULL);
    }

    // NVIC interrupt is configured by CubeMX in main.c
}



// ====================== 全局亮度控制 ======================
static uint8_t global_brightness = 100;   // 默认100%亮度（范围 0~100）

/**
 * @brief 设置 WS2812B 整体亮度（百分比）
 * @param brightness_percent 亮度百分比，范围 0~100
 */
void ws2812b_set_brightness(uint8_t brightness_percent)
{
    if (brightness_percent > 100)
        brightness_percent = 100;

    global_brightness = brightness_percent;

    rt_kprintf("WS2812B 亮度已设置为 %d%%", global_brightness);
}

/**
 * @brief 内部函数：对颜色应用亮度缩放（Gamma校正可选）
 * @note  在 set_color 时自动调用
 */
static void apply_brightness(uint8_t *g, uint8_t *r, uint8_t *b)
{
    if (global_brightness == 100)
        return;                     // 100% 不做处理，性能最好

    uint16_t scale = global_brightness;   // 0~100

    *g = ((*g * scale) / 100);
    *r = ((*r * scale) / 100);
    *b = ((*b * scale) / 100);
}


// Set single LED color (GRB order)
// [FIX] Problem 1: Use data_mutex to protect leds_color_data[] instead of ws2812_mutex.
// This allows set_color() to proceed even while DMA is running (DMA uses ws2812_buffer[],
// not leds_color_data[]).
void ws2812b_set_color(uint16_t index, uint8_t g, uint8_t r, uint8_t b)
{
    if (data_mutex) rt_mutex_take(data_mutex, RT_WAITING_FOREVER);
    if (index >= LED_COUNT) {
        rt_kprintf("LED index out of range: %d (max: %d)\n", index, LED_COUNT - 1);
        if (data_mutex) rt_mutex_release(data_mutex);
        return;
    }

    // Apply global brightness
    apply_brightness(&g, &r, &b);

    leds_color_data[index * BYTES_PER_LED + 0] = g;
    leds_color_data[index * BYTES_PER_LED + 1] = r;
    leds_color_data[index * BYTES_PER_LED + 2] = b;
    if (data_mutex) rt_mutex_release(data_mutex);
}

// 设置所有LED同一颜色
void ws2812b_set_all(uint8_t g, uint8_t r, uint8_t b)
{
    for (uint16_t i = 0; i < LED_COUNT; i++)
    {
        ws2812b_set_color(i, g, r, b);
    }
}

// Start update (non-blocking)
// [FIX] Problem 1 & 5: Snapshot leds_color_data[] into snapshot_color_data[] before DMA start.
// ISR reads from snapshot_color_data[] (immutable during transfer), not leds_color_data[].
// Other threads can freely modify leds_color_data[] during DMA transfer.
rt_err_t ws2812b_update(void)
{
    if (is_updating) {
        rt_kprintf("WS2812B update in progress, skipping\n");
        return -RT_EBUSY;
    }

    is_updating = 1;
    led_index = 0;

    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_4);  // Ensure clean start

    // Zero out the entire DMA buffer first
    memset(ws2812_buffer, 0, sizeof(ws2812_buffer));

    // Snapshot: copy leds_color_data[] -> snapshot_color_data[] under data_mutex.
    // After this, ISR reads from snapshot_color_data[] which is immutable during DMA.
    // Other threads can freely modify leds_color_data[] without affecting the transfer.
    snapshot_color_buffer();

    if (HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_4, (uint32_t *)ws2812_buffer, DMA_BUFF_LEN) != HAL_OK)
    {
        rt_kprintf("DMA start failed\n");
        is_updating = 0;
        return -RT_ERROR;
    }

    rt_kprintf("WS2812B update started\n");
    return RT_EOK;
}

// Update sequence (called from DMA HT/TC callbacks in ISR context)
// [FIX] Problem 4: Added boundary check before fill_led_pwm_data() call.
// [FIX] Problem 1 & 5: update_sequence() now only reads from ws2812_buffer[] which was
// snapshotted before DMA start. No access to leds_color_data[] at all.
void update_sequence(uint8_t is_tc)
{
    if (!is_updating) return;

    uint16_t *buf_ptr = is_tc ? &ws2812_buffer[DMA_HALF_LEN] : ws2812_buffer;

    // Fill next half of the buffer
    for (uint16_t i = 0; i < LEDS_PER_DMA_IRQ; i++)
    {
        if (led_index < RESET_PRE_MIN)
        {
            // Pre-reset: all zeros
            memset(&buf_ptr[i * BITS_PER_LED], 0, BITS_PER_LED * sizeof(uint16_t));
        }
        else if (led_index < RESET_PRE_MIN + LED_COUNT)
        {
            // Data region: boundary check before calling fill_led_pwm_data
            uint16_t led_idx = led_index - RESET_PRE_MIN;
            if (led_idx < LED_COUNT)
            {
                fill_led_pwm_data(led_idx, &buf_ptr[i * BITS_PER_LED]);
            }
            else
            {
                // Safety: zero fill if somehow out of range
                memset(&buf_ptr[i * BITS_PER_LED], 0, BITS_PER_LED * sizeof(uint16_t));
            }
        }
        else
        {
            // Post-reset: all zeros
            memset(&buf_ptr[i * BITS_PER_LED], 0, BITS_PER_LED * sizeof(uint16_t));
        }
        led_index++;
    }

    // Stop after all data sent + sufficient reset time
    if (led_index >= RESET_PRE_MIN + LED_COUNT + RESET_POST_MIN + 20)
    {
        HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_4);

        is_updating = 0;
        led_index = 0;
        rt_sem_release(dma_complete_sem);
        rt_kprintf("WS2812B update complete\n");
    }
}

/* DMA1_Channel4 IRQ handler is defined in cubemx/Src/stm32f1xx_it.c (CubeMX generated).
 * It calls HAL_DMA_IRQHandler which triggers the callbacks below. */

// DMA Half-Transfer callback (HT) - ISR context
// [FIX] Problem 5: Callbacks only operate on ws2812_buffer[],
// which is independent of leds_color_data[]. No data race.
void HAL_DMA_XferHalfCpltCallback(DMA_HandleTypeDef *hdma)
{
    if (hdma == &hdma_tim1_ch4_trig_com)
    {
        update_sequence(0);  // HT
    }
}

// DMA Transfer-Complete callback (TC) - ISR context
// [FIX] Problem 5: Same as above, only operates on ws2812_buffer[].
void HAL_DMA_XferCpltCallback(DMA_HandleTypeDef *hdma)
{
    if (hdma == &hdma_tim1_ch4_trig_com)
    {
        update_sequence(1);  // TC
    }
}

// Fill single LED PWM data (GRB order, MSB first)
// [FIX] Problem 4: Boundary check retained as safety net.
// [FIX] Problem 1 & 5: Reads from snapshot_color_data[] (immutable during DMA transfer),
//        NOT from leds_color_data[] (which may be modified by other threads).
static void fill_led_pwm_data(uint16_t ledx, uint16_t *ptr)
{
    if (ledx >= LED_COUNT) return;

    uint8_t g = snapshot_color_data[ledx * BYTES_PER_LED + 0];
    uint8_t r = snapshot_color_data[ledx * BYTES_PER_LED + 1];
    uint8_t b = snapshot_color_data[ledx * BYTES_PER_LED + 2];

    // GRB order, MSB first
    for (uint8_t i = 0; i < 8; i++)
    {
        ptr[i] = (g & (1 << (7 - i))) ? PWM_HIGH_1 : PWM_HIGH_0;
        ptr[8 + i] = (r & (1 << (7 - i))) ? PWM_HIGH_1 : PWM_HIGH_0;
        ptr[16 + i] = (b & (1 << (7 - i))) ? PWM_HIGH_1 : PWM_HIGH_0;
    }
}

// 演示效果函数
void ws2812b_demo_effects(void)
{
    static uint8_t demo_step = 0;
    static uint32_t last_time = 0;
    uint32_t current_time = rt_tick_get();

    // 每500ms切换一次效果
    if (current_time - last_time >= 1000)
    {
        last_time = current_time;

        switch (demo_step)
        {
            case 0: // 红色全亮
                ws2812b_set_all(255, 0, 0);
                ws2812b_update();
                if (rt_sem_take(dma_complete_sem, 100) != RT_EOK) rt_kprintf("WS2812B DMA 超时，跳过本次更新\n"); // [FIX3-5] 超时保护
                break;
            case 1: // 绿色全亮
                ws2812b_set_all(0, 255, 0);
                ws2812b_update();
                if (rt_sem_take(dma_complete_sem, 100) != RT_EOK) rt_kprintf("WS2812B DMA 超时，跳过本次更新\n"); // [FIX3-5] 超时保护
                break;
            case 2: // 蓝色全亮
                ws2812b_set_all(0, 0, 255);
                ws2812b_update();
                if (rt_sem_take(dma_complete_sem, 100) != RT_EOK) rt_kprintf("WS2812B DMA 超时，跳过本次更新\n"); // [FIX3-5] 超时保护
                break;
            case 3: // 白色全亮
                ws2812b_set_all(255, 255, 255);
                ws2812b_update();
                if (rt_sem_take(dma_complete_sem, 100) != RT_EOK) rt_kprintf("WS2812B DMA 超时，跳过本次更新\n"); // [FIX3-5] 超时保护
                break;
            case 4: // 流水灯效果
                for (int i = 0; i < LED_COUNT; i++)
                {
                    ws2812b_set_color(i, 255, 255, 0);  // 黄色
                    ws2812b_update();
                    if (rt_sem_take(dma_complete_sem, 100) != RT_EOK) rt_kprintf("WS2812B DMA 超时，跳过本次更新\n"); // [FIX3-5] 超时保护
                    rt_thread_mdelay(50);
                    ws2812b_set_color(i, 0, 0, 0);      // 关闭
                    ws2812b_update();
                    if (rt_sem_take(dma_complete_sem, 100) != RT_EOK) rt_kprintf("WS2812B DMA 超时，跳过本次更新\n"); // [FIX3-5] 超时保护
                }
                break;
            case 5: // 呼吸灯效果
                for (int brightness = 0; brightness < 255; brightness += 5)
                {
                    ws2812b_set_all(brightness, brightness, brightness);
                    ws2812b_update();
                    if (rt_sem_take(dma_complete_sem, 100) != RT_EOK) rt_kprintf("WS2812B DMA 超时，跳过本次更新\n"); // [FIX3-5] 超时保护
                    rt_thread_mdelay(20);
                }
                for (int brightness = 255; brightness > 0; brightness -= 5)
                {
                    ws2812b_set_all(brightness, brightness, brightness);
                    ws2812b_update();
                    if (rt_sem_take(dma_complete_sem, 100) != RT_EOK) rt_kprintf("WS2812B DMA 超时，跳过本次更新\n"); // [FIX3-5] 超时保护
                    rt_thread_mdelay(20);
                }
                break;
            default:
                demo_step = 0;
                break;
        }

        demo_step = (demo_step + 1) % 6;
    }
}


/**
 * @brief Set WS2812B eye LED state
 * @param state 0=dying(min brightness), 1=normal(full white)
 *
 * [FIX] Problem 2: Check ws2812b_update() return value before waiting on semaphore.
 *       If update returns -RT_EBUSY, skip the semaphore wait to avoid blocking.
 * [FIX] Problem 6: Handle case where semaphore or mutex may not be initialized yet.
 */
void ws2812b_set_white(uint8_t state)
{
    switch (state)
    {
        case 0:  // Dying - minimum brightness
            ws2812b_set_brightness(3);
            ws2812b_set_all(255, 255, 255);
            break;
        case 1:  // Normal - full white
            ws2812b_set_brightness(100);
            ws2812b_set_all(255, 255, 255);
            break;
        default:
            ws2812b_set_brightness(0);
            ws2812b_set_all(0, 0, 0);
            LOG_W("WS2812B set_white unknown state, turned off");
            break;
    }

    // [FIX] Problem 2: Only wait on semaphore if update was successfully started.
    // [FIX] Problem 6: Check dma_complete_sem != RT_NULL before take.
    rt_err_t ret = ws2812b_update();
    if (ret == RT_EOK)
    {
        if (dma_complete_sem != RT_NULL)
        {
            if (rt_sem_take(dma_complete_sem, 300) != RT_EOK)
            {
                LOG_W("WS2812B set_white: DMA timeout");
            }
        }
    }
    else if (ret == -RT_EBUSY)
    {
        LOG_W("WS2812B set_white: DMA busy, update skipped");
    }
    else
    {
        LOG_E("WS2812B set_white: DMA start failed (%d)", ret);
    }
}




#endif

