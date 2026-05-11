/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-29     Administrator       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_LCD_DRIVER_H_
#define APPLICATIONS_MACBSP_INC_LCD_DRIVE_H_

#include "bsp_sys.h"


#define TFT_Width   240
#define TFT_Height  320


//-----------颜色定义------------------
#define WHITE   0xFFFF  //白色
#define YELLOW  0xFFE0  //黄色
#define BRRED   0XFC07  //棕红色
#define PINK    0XF81F  //粉色
#define RED     0xF800  //红色
#define BROWN   0XBC40  //棕色
#define GRAY    0X8430  //灰色
#define GBLUE   0X07FF  //兰色
#define GREEN   0x07E0  //绿色
#define BLUE    0x001F  //蓝色
#define BLACK   0x0000  //黑色

extern  rt_uint16_t BACK_COLOR;  //背景色
extern  rt_uint16_t FORE_COLOR;  //点的颜色


int lcd_init(void);
void lcd_set_color(rt_uint16_t back, rt_uint16_t fore);
void lcd_display_on(void);
void lcd_display_off(void);
void lcd_enter_sleep(void);
void lcd_exit_sleep(void);
void lcd_address_set(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2);
void lcd_clear(rt_uint16_t color);
void lcd_fill_array(rt_uint16_t x_start, rt_uint16_t y_start, rt_uint16_t x_end, rt_uint16_t y_end, void *pcolor);


#endif /* APPLICATIONS_MACBSP_INC_LCD_DRIVER_H_ */
