/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-05-12     coder        the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_BSP_OLED_EYE_H_
#define APPLICATIONS_MACBSP_INC_BSP_OLED_EYE_H_

#include "bsp_sys.h"

/* --- Hardware parameters --- */
#define OLED_I2C_ADDR       0x3C    /* 7-bit I2C address */
#define OLED_WIDTH          128
#define OLED_HEIGHT         64
#define OLED_PAGES          (OLED_HEIGHT / 8)   /* 8 pages for 64-pixel height */

/* --- SSD1306 commands --- */
#define SSD1306_CMD_SET_MUX         0xA8
#define SSD1306_CMD_SET_OFFSET      0xD3
#define SSD1306_CMD_SET_START_LINE  0x40
#define SSD1306_CMD_SEG_REMAP       0xA0
#define SSD1306_CMD_COM_SCAN_DIR    0xC0
#define SSD1306_CMD_COM_PINS        0xDA
#define SSD1306_CMD_SET_CONTRAST    0x81
#define SSD1306_CMD_ENTIRE_ON       0xA4
#define SSD1306_CMD_NORMAL          0xA6
#define SSD1306_CMD_SET_CLK_DIV     0xD5
#define SSD1306_CMD_SET_CHARGE      0xD9
#define SSD1306_CMD_SET_VCOM        0xDB
#define SSD1306_CMD_CHARGE_PUMP     0x8D
#define SSD1306_CMD_DISPLAY_OFF     0xAE
#define SSD1306_CMD_DISPLAY_ON      0xAF
#define SSD1306_CMD_PAGE_ADDR       0x22
#define SSD1306_CMD_COL_ADDR        0x21

/* --- Function prototypes --- */
void oled_eye_init(void);
void oled_eye_dying(void);
void oled_eye_normal(void);

#endif /* APPLICATIONS_MACBSP_INC_BSP_OLED_EYE_H_ */
