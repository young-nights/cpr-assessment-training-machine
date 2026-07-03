/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-01-26     Administrator       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_BSP_WS2812B_H_
#define APPLICATIONS_MACBSP_INC_BSP_2812B_H_


#include "bsp_sys.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "stm32f1xx_hal.h"

#define USE_PWM_METHOD  0
#define USE_SPI_METHOD  0


#if USE_SPI_METHOD

/* SPI引脚 -- NSS */
#define     WS2812B_NSS_PORT     SPI2_NSS_GPIO_Port
#define     WS2812B_NSS_PIN      SPI2_NSS_Pin


#define     WS2812B_NSS_SET(bit) if(bit) \
                                 HAL_GPIO_WritePin ( WS2812B_NSS_PORT, WS2812B_NSS_PIN , GPIO_PIN_SET );\
                                 else \
                                 HAL_GPIO_WritePin ( WS2812B_NSS_PORT, WS2812B_NSS_PIN , GPIO_PIN_RESET );



extern struct rt_spi_device *ws2812b_spi_dev;

// 函数声明 ------------------------------------------------------------
int WS2812B_SPI_Init(void);


#ifndef WS2812B_LED_NUMS
#define WS2812B_LED_NUMS 77
#endif

#define WS2812B_RGB_BITS 24 // 每颗灯 24 位数据（GRB 各 8 位）
#define WS2812B_CODE_0 0xC0 // 用 SPI 发 0b11000000，模拟 0 码（高 220 ns）
#define WS2812B_CODE_1 0xF0 // 用 SPI 发 0b11110000，模拟 1 码（高 580 ns）


#define WS2812B_COLOR_BLACK     0x000000    // 熄灭
#define WS2812B_COLOR_RED       0xFF0000    // 正红
#define WS2812B_COLOR_ORANGE    0xF08784    // 暖橙
#define WS2812B_COLOR_YELLOW    0xFF7F27    // 金黄
#define WS2812B_COLOR_GREEN     0x7FFF00    // 翠绿
#define WS2812B_COLOR_CYAN      0x00FFFF    // 青色
#define WS2812B_COLOR_BLUE      0x0000FF    // 正蓝
#define WS2812B_COLOR_PURPLE    0x8B00FF    // 紫色
#define WS2812B_COLOR_WHITE     0xFFFFFF    // 正白


// 函数声明 -------------------------------------------------------
void ws2812b_table_init(void);
void ws2812b_set_brightness(uint8_t brightness);
void ws2812b_set_color(uint16_t index, uint32_t color);
void ws2812b_show(void);
void ws2812b_clear(void);
void ws2812b_set_all(uint32_t color);


// 测试函数------------------------------------------------------
void ws2812b_full_color_test(void);
void ws2812b_waterfall_light_test(void);
void ws2812b_brightness_gradient_test(void);
void ws2812b_breathing_light_test(void);
int rgb_test(int argc, char **argv);


#elif USE_PWM_METHOD



#define LED_COUNT       30          // LED数量，根据需要修改
#define RESET_PRE_MIN   60          // [FIX3] 复位前最小LED周期 (>50μs ≈50 cycles @1.25μs)
#define RESET_POST_MIN  60          // [FIX3] 复位后最小LED周期
#define LEDS_PER_DMA_IRQ 8          // [FIX4] 每个DMA中断处理的LED数 (中断频率减半)

#define PWM_PERIOD      89          // ARR=89，周期90 ticks
#define PWM_HIGH_0      28          // 逻辑0 ≈ 389ns
#define PWM_HIGH_1      56          // 逻辑1 ≈ 778ns

// 缓冲区：双缓冲 (HT/TC)，每个部分 LEDS_PER_DMA_IRQ * 24 个 uint16_t
extern uint16_t ws2812_buffer[2 * LEDS_PER_DMA_IRQ * 24];

// [FIX2] 全局信号量声明，供 ws2812b_demo_effects() 等待 DMA 完成
extern rt_sem_t dma_complete_sem;

// DMA 传输中标志 (供 IRQ handler 检查)
extern volatile uint8_t is_updating;

// 函数声明
void ws2812b_init(void);
void ws2812b_set_color(uint16_t index, uint8_t g, uint8_t r, uint8_t b);
void ws2812b_set_all(uint8_t g, uint8_t r, uint8_t b);
rt_err_t ws2812b_update(void);          // 非阻塞更新，返回 -RT_EBUSY 如果正在传输
void update_sequence(uint8_t is_tc);    // HT/TC 更新逻辑
void ws2812b_demo_effects(void);       // 演示效果函数
void ws2812b_set_white(uint8_t level);

#endif


#endif /* APPLICATIONS_MACBSP_INC_BSP_WS2812B_H_ */
