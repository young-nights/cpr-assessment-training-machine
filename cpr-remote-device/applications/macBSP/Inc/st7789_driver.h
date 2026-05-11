/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-21     Administrator       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_ST7789_DRIVER_H_
#define APPLICATIONS_MACBSP_INC_ST7789_DRIVE_H_
#include "bsp_sys.h"

#define ST7789_TEST     0

#if ST7789_TEST
//-----------屏幕模式------------------
/***
 *! 宏定义：USE_HORIZONTAL(屏幕模式)
 *! USE_HORIZONTAL->0/1：竖屏
 *! USE_HORIZONTAL->2/3：横屏
 **/
#define USE_HORIZONTAL 0

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define TFT_Width   240
#define TFT_Height  320

#else
#define TFT_Width   240
#define TFT_Height  320
#endif




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

//---------lcd函数声明-------------------
void lcd_rest(void);
int lcd_init(void);
void lcd_set_color(rt_uint16_t back, rt_uint16_t fore);
void lcd_display_on(void);
void lcd_display_off(void);
void lcd_enter_sleep(void);
void lcd_exit_sleep(void);
void lcd_address_set(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2);
void lcd_clear(rt_uint16_t color);
void lcd_draw_point(rt_uint16_t x, rt_uint16_t y);
void lcd_draw_point_color(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color);
void lcd_fill(rt_uint16_t x_start, rt_uint16_t y_start, rt_uint16_t x_end, rt_uint16_t y_end, rt_uint16_t color);
void lcd_fill_array(rt_uint16_t x_start, rt_uint16_t y_start, rt_uint16_t x_end, rt_uint16_t y_end, void *pcolor);
void lcd_draw_line(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2);
void lcd_draw_circle(rt_uint16_t x0, rt_uint16_t y0, rt_uint8_t r);
rt_err_t lcd_show_image(rt_uint16_t x, rt_uint16_t y, rt_uint16_t length, rt_uint16_t wide, const rt_uint8_t *p);

#endif /* ST7789_TEST */

#endif /* APPLICATIONS_MACBSP_INC_ST7789_DRIVER_H_ */
