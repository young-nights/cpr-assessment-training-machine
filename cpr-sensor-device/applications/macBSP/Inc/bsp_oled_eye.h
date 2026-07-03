/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-05-12     coder        the first version
 * 2026-05-12     coder        migrated from SSD1306 (128x64) to ST7315 (64x48)
 */
#ifndef APPLICATIONS_MACBSP_INC_BSP_OLED_EYE_H_
#define APPLICATIONS_MACBSP_INC_BSP_OLED_EYE_H_

#include "bsp_sys.h"

/* --- Hardware parameters --- */
#define OLED_I2C_ADDR       0x3C    /* 7-bit I2C address */
#define OLED_WIDTH          64
#define OLED_HEIGHT         48
#define OLED_PAGES          (OLED_HEIGHT / 8)   /* 6 pages for 48-pixel height */

/* --- Function prototypes --- */
void oled_eye_init(void);
void oled_eye_dying(void);
void oled_eye_normal(void);

#endif /* APPLICATIONS_MACBSP_INC_BSP_OLED_EYE_H_ */
