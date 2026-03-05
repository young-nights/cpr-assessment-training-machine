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


#define USE_PWM_METHOD  0
#define USE_SPI_METHOD  1


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


#elif USE_PWM_METHOD

#define LED_COUNT 10  // LED 数量
#define BITS_PER_LED 24  // 每个 LED 24 比特 (GRB)
#define RESET_BITS 50  // 复位脉冲 (至少 50μs, 对应 ~40 比特)

#define PWM_HI 60  // '1' 比特占空比 (2/3 * 90)
#define PWM_LO 30  // '0' 比特占空比 (1/3 * 90)
#define PWM_RESET 0  // 复位低电平

typedef struct {
    uint8_t g;  // Green
    uint8_t r;  // Red
    uint8_t b;  // Blue
} ws_rgb_t;
extern ws_rgb_t leds[LED_COUNT];

void ws2812b_init(void);
void ws2812b_update(void);
void ws2812b_set_color(uint16_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);


#endif


#endif /* APPLICATIONS_MACBSP_INC_BSP_WS2812B_H_ */
