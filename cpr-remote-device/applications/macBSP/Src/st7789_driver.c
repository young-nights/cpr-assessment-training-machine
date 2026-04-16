/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-21     Administrator       the first version
 */
#include <st7789_driver.h>



// 这个驱动实测无问题，实际移植到lcd_driver.c文件中
#if ST7789_TEST


//以下是LCD显示相关的函数---------------------------------------------------------------------------------------------

#define LCD_CLEAR_SEND_NUMBER 7680 /* 240*320/10 */
rt_uint16_t BACK_COLOR = WHITE, FORE_COLOR = BLACK;




/**
  * @brief  复位函数
  * @retval void
  */
void lcd_rest(void)
{
    LCD_RST_SET(PIN_LOW);
    rt_thread_mdelay(200);
    LCD_RST_SET(PIN_HIGH);
    rt_thread_mdelay(200);
    lcd_display_on();
    rt_thread_mdelay(200);
}



/**
  * @brief  屏幕初始化函数
  * @retval void
  */
int lcd_init(void)
{
    tft_spi_init();
    lcd_rest();

    /* Memory Data Access Control */
    st7789_send_cmd(0x36);
    st7789_send_data(0x00);
    /* RGB 5-6-5-bit  */
    st7789_send_cmd(0x3A);
    st7789_send_data(0x65);
    /* Porch Setting */
    st7789_send_cmd(0xB2);
    st7789_send_data(0x0C);
    st7789_send_data(0x0C);
    st7789_send_data(0x00);
    st7789_send_data(0x33);
    st7789_send_data(0x33);
    /*  Gate Control */
    st7789_send_cmd(0xB7);
    st7789_send_data(0x35);
    /* VCOM Setting */
    st7789_send_cmd(0xBB);
    st7789_send_data(0x19);
    /* LCM Control */
    st7789_send_cmd(0xC0);
    st7789_send_data(0x2C);
    /* VDV and VRH Command Enable */
    st7789_send_cmd(0xC2);
    st7789_send_data(0x01);
    /* VRH Set */
    st7789_send_cmd(0xC3);
    st7789_send_data(0x12);
    /* VDV Set */
    st7789_send_cmd(0xC4);
    st7789_send_data(0x20);
    /* Frame Rate Control in Normal Mode */
    st7789_send_cmd(0xC6);
    st7789_send_data(0x0F);
    /* Power Control 1 */
    st7789_send_cmd(0xD0);
    st7789_send_data(0xA4);
    st7789_send_data(0xA1);
    /* Positive Voltage Gamma Control */
    st7789_send_cmd(0xE0);
    st7789_send_data(0xD0);
    st7789_send_data(0x04);
    st7789_send_data(0x0D);
    st7789_send_data(0x11);
    st7789_send_data(0x13);
    st7789_send_data(0x2B);
    st7789_send_data(0x3F);
    st7789_send_data(0x54);
    st7789_send_data(0x4C);
    st7789_send_data(0x18);
    st7789_send_data(0x0D);
    st7789_send_data(0x0B);
    st7789_send_data(0x1F);
    st7789_send_data(0x23);
    /* Negative Voltage Gamma Control */
    st7789_send_cmd(0xE1);
    st7789_send_data(0xD0);
    st7789_send_data(0x04);
    st7789_send_data(0x0C);
    st7789_send_data(0x11);
    st7789_send_data(0x13);
    st7789_send_data(0x2C);
    st7789_send_data(0x3F);
    st7789_send_data(0x44);
    st7789_send_data(0x51);
    st7789_send_data(0x2F);
    st7789_send_data(0x1F);
    st7789_send_data(0x1F);
    st7789_send_data(0x20);
    st7789_send_data(0x23);
    /* Display Inversion On */
    st7789_send_cmd(0x21);
    /* Sleep Out */
    st7789_send_cmd(0x11);
    /* wait for power stability */
    rt_thread_mdelay(100);

    /* display on */
    lcd_display_on();
    st7789_send_cmd(0x29);

    return RT_EOK;
}





/**
 * Set background color and foreground color
 *
 * @param   back    background color
 * @param   fore    fore color
 *
 * @return  void
 */
void lcd_set_color(rt_uint16_t back, rt_uint16_t fore)
{
    BACK_COLOR = back;
    FORE_COLOR = fore;
}

/**
  * @brief  开启显示屏背光
  * @retval void
  */
void lcd_display_on(void)
{
    LCD_BLK_SET(PIN_HIGH);
}

/**
  * @brief  关闭显示屏背光
  * @retval void
  */
void lcd_display_off(void)
{
    LCD_BLK_SET(PIN_LOW);
}

/* lcd enter the minimum power consumption mode and backlight off. */
void lcd_enter_sleep(void)
{
    lcd_display_off();
    rt_thread_mdelay(5);
    st7789_send_cmd(0x10);
}
/* lcd turn off sleep mode and backlight on. */
void lcd_exit_sleep(void)
{
    lcd_display_on();
    rt_thread_mdelay(5);
    st7789_send_cmd(0x11);
    rt_thread_mdelay(120);
}


/**
 * Set drawing area
 *
 * @param   x1      start of x position
 * @param   y1      start of y position
 * @param   x2      end of x position
 * @param   y2      end of y position
 *
 * @return  void
 */
void lcd_address_set(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2)
{
    st7789_send_cmd(0x2a);
    st7789_send_data(x1 >> 8);
    st7789_send_data(x1);
    st7789_send_data(x2 >> 8);
    st7789_send_data(x2);

    st7789_send_cmd(0x2b);
    st7789_send_data(y1 >> 8);
    st7789_send_data(y1);
    st7789_send_data(y2 >> 8);
    st7789_send_data(y2);

    st7789_send_cmd(0x2C);
}


/**
 * clear the lcd.
 *
 * @param   color       Fill color
 *
 * @return  void
 */
void lcd_clear(rt_uint16_t color)
{
    rt_uint16_t i, j;
    rt_uint8_t data[2] = {0};
    rt_uint8_t *buf = RT_NULL;

    data[0] = color >> 8;
    data[1] = color;
    lcd_address_set(0, 0, TFT_Width - 1, TFT_Height - 1);

    buf = rt_malloc(LCD_CLEAR_SEND_NUMBER);
    if (buf)
    {
        /* 3840 = 7680/2 color is 16 bit */
        for (j = 0; j < LCD_CLEAR_SEND_NUMBER / 2; j++)
        {
            buf[j * 2] =  data[0];
            buf[j * 2 + 1] =  data[1];
        }

        LCD_DC_SET(1);
        for (i = 0; i < 20; i++)
        {
            rt_spi_send(spi_dev_tft, buf, LCD_CLEAR_SEND_NUMBER);
        }
        rt_free(buf);
    }
    else
    {
        LCD_DC_SET(1);
        for (i = 0; i < TFT_Width; i++)
        {
            for (j = 0; j < TFT_Height; j++)
            {
                rt_spi_send(spi_dev_tft, data, 2);
            }
        }
    }
}


/**
 * display a point on the lcd.
 *
 * @param   x   x position
 * @param   y   y position
 *
 * @return  void
 */
void lcd_draw_point(rt_uint16_t x, rt_uint16_t y)
{
    lcd_address_set(x, y, x, y);
    st7789_send_half_word(FORE_COLOR);
}



/**
 * display a point on the lcd using the given colour.
 *
 * @param   x       x position
 * @param   y       y position
 * @param   color   color of point
 *
 * @return  void
 */
void lcd_draw_point_color(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color)
{
    lcd_address_set(x, y, x, y);
    st7789_send_half_word(color);
}


/**
 * full color on the lcd.
 *
 * @param   x_start     start of x position
 * @param   y_start     start of y position
 * @param   x_end       end of x position
 * @param   y_end       end of y position
 * @param   color       Fill color
 *
 * @return  void
 */
void lcd_fill(rt_uint16_t x_start, rt_uint16_t y_start, rt_uint16_t x_end, rt_uint16_t y_end, rt_uint16_t color)
{
    rt_uint16_t i = 0, j = 0;
    rt_uint32_t size = 0, size_remain = 0;
    rt_uint8_t *fill_buf = RT_NULL;

    size = (x_end - x_start + 1) * (y_end - y_start + 1) * 2;

    if (size > LCD_CLEAR_SEND_NUMBER)
    {
        /* the number of remaining to be filled */
        size_remain = size - LCD_CLEAR_SEND_NUMBER;
        size = LCD_CLEAR_SEND_NUMBER;
    }

    lcd_address_set(x_start, y_start, x_end, y_end);

    fill_buf = (rt_uint8_t *)rt_malloc(size);
    if (fill_buf)
    {
        /* fast fill */
        while (1)
        {
            for (i = 0; i < size / 2; i++)
            {
                fill_buf[2 * i] = color >> 8;
                fill_buf[2 * i + 1] = color;
            }
            LCD_DC_SET(1);
            rt_spi_send(spi_dev_tft, fill_buf, size);

            /* Fill completed */
            if (size_remain == 0)
                break;

            /* calculate the number of fill next time */
            if (size_remain > LCD_CLEAR_SEND_NUMBER)
            {
                size_remain = size_remain - LCD_CLEAR_SEND_NUMBER;
            }
            else
            {
                size = size_remain;
                size_remain = 0;
            }
        }
        rt_free(fill_buf);
    }
    else
    {
        for (i = y_start; i <= y_end; i++)
        {
            for (j = x_start; j <= x_end; j++)st7789_send_half_word(color);
        }
    }
}


/**
 * full color array on the lcd.
 *
 * @param   x_start     start of x position
 * @param   y_start     start of y position
 * @param   x_end       end of x position
 * @param   y_end       end of y position
 * @param   color       Fill color array's pointer
 *
 * @return  void
 */
void lcd_fill_array(rt_uint16_t x_start, rt_uint16_t y_start, rt_uint16_t x_end, rt_uint16_t y_end, void *pcolor)
{
    rt_uint32_t size = 0;

    size = (x_end - x_start + 1) * (y_end - y_start + 1) * 2/*16bit*/;
    lcd_address_set(x_start, y_start, x_end, y_end);
    LCD_DC_SET(1);
    rt_spi_send(spi_dev_tft, pcolor, size);
}


/**
 * display a line on the lcd.
 *
 * @param   x1      x1 position
 * @param   y1      y1 position
 * @param   x2      x2 position
 * @param   y2      y2 position
 *
 * @return  void
 */
void lcd_draw_line(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2)
{
    rt_uint16_t t;
    rt_uint32_t i = 0;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;

    if (y1 == y2)
    {
        /* fast draw transverse line */
        rt_uint32_t x_offset = 0;
        if (x1 < x2)
        {
            x_offset = x2 - x1;
            lcd_address_set(x1, y1, x2, y2);
        }
        else if (x1 > x2)
        {
            x_offset = x1 - x2;
            lcd_address_set(x2, y2, x1, y1);
        }
        else
        {
            lcd_draw_point(x1, y1);
            return;
        }

        rt_uint8_t line_buf[480] = {0};

        for (i = 0; i < x_offset; i++)
        {
            line_buf[2 * i] = FORE_COLOR >> 8;
            line_buf[2 * i + 1] = FORE_COLOR;
        }

        LCD_DC_SET(1);
        rt_spi_send(spi_dev_tft, line_buf, x_offset * 2);

        return ;
    }

    delta_x = x2 - x1;
    delta_y = y2 - y1;
    row = x1;
    col = y1;
    if (delta_x > 0)incx = 1;
    else if (delta_x == 0)incx = 0;
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0;
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)distance = delta_x;
    else distance = delta_y;
    for (t = 0; t <= distance + 1; t++)
    {
        lcd_draw_point(row, col);
        xerr += delta_x ;
        yerr += delta_y ;
        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}


/**
 * display a circle on the lcd.
 *
 * @param   x       x position of Center
 * @param   y       y position of Center
 * @param   r       radius
 *
 * @return  void
 */
void lcd_draw_circle(rt_uint16_t x0, rt_uint16_t y0, rt_uint8_t r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);
    while (a <= b)
    {
        lcd_draw_point(x0 - b, y0 - a);
        lcd_draw_point(x0 + b, y0 - a);
        lcd_draw_point(x0 - a, y0 + b);
        lcd_draw_point(x0 - b, y0 - a);
        lcd_draw_point(x0 - a, y0 - b);
        lcd_draw_point(x0 + b, y0 + a);
        lcd_draw_point(x0 + a, y0 - b);
        lcd_draw_point(x0 + a, y0 + b);
        lcd_draw_point(x0 - b, y0 + a);
        a++;
        //Bresenham
        if (di < 0)di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
        lcd_draw_point(x0 + a, y0 + b);
    }
}

/**
 * display the image on the lcd.
 *
 * @param   x       x position
 * @param   y       y position
 * @param   length  length of image
 * @param   wide    wide of image
 * @param   p       image
 *
 * @return   0: display success
 *          -1: the image is too large
 */
rt_err_t lcd_show_image(rt_uint16_t x, rt_uint16_t y, rt_uint16_t length, rt_uint16_t wide, const rt_uint8_t *p)
{
    RT_ASSERT(p);

    if (x + length > TFT_Width || y + wide > TFT_Height)
    {
        return -RT_ERROR;
    }

    lcd_address_set(x, y, x + length - 1, y + wide - 1);

    LCD_DC_SET(1);
    rt_spi_send(spi_dev_tft, p, length * wide * 2);

    return RT_EOK;
}


#endif



