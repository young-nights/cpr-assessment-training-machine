/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-29     Administrator       the first version
 */
#include <lcd_driver.h>


#define LCD_CLEAR_SEND_NUMBER 7680 /* 240*320/10 */
rt_uint16_t BACK_COLOR = WHITE, FORE_COLOR = BLACK;




/**
  * @brief  屏幕初始化函数
  * @retval void
  */
int lcd_init(void)
{
    tft_spi_init();

    LCD_RST_SET(PIN_LOW);
    rt_thread_mdelay(200);
    LCD_RST_SET(PIN_HIGH);
    rt_thread_mdelay(200);
    lcd_display_on();
    rt_thread_mdelay(200);

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


#define USE_INVERSION_ON 0
#if USE_INVERSION_ON
    /* Display Inversion On */
    st7789_send_cmd(0x21);
#else
    /* Display Inversion Off */
    st7789_send_cmd(0x20);
#endif
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

void lcd_display_on(void)
{
    LCD_BLK_SET(PIN_HIGH);
}

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



