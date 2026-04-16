/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-21     Administrator       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_ST7789_SPI_H_
#define APPLICATIONS_MACBSP_INC_ST7789_SPI_H_

#include "bsp_sys.h"

extern struct rt_spi_device *spi_dev_tft;

/* 片选引脚 -- CS */
#define     TFT_CS_PORT     LCD_CS_GPIO_Port
#define     TFT_CS_PIN      LCD_CS_Pin

#define     LCD_CS_SET(bit) if(bit) \
                            HAL_GPIO_WritePin ( TFT_CS_PORT, TFT_CS_PIN , GPIO_PIN_SET )\
                            else \
                            HAL_GPIO_WritePin ( TFT_CS_PORT, TFT_CS_PIN , GPIO_PIN_RESET )

/* 复位引脚 -- RST */
#define     TFT_RST_PORT    LCD_RST_GPIO_Port
#define     TFT_RST_PIN     LCD_RST_Pin

#define     LCD_RST_SET(bit) if(bit) \
                             HAL_GPIO_WritePin ( TFT_RST_PORT, TFT_RST_PIN , GPIO_PIN_SET );\
                             else \
                             HAL_GPIO_WritePin ( TFT_RST_PORT, TFT_RST_PIN , GPIO_PIN_RESET );

/* 背光引脚 -- BLK */
#define     TFT_BLK_PORT    LCD_BLK_GPIO_Port
#define     TFT_BLK_PIN     LCD_BLK_Pin

#define     LCD_BLK_SET(bit) if(bit) \
                             HAL_GPIO_WritePin ( TFT_BLK_PORT, TFT_BLK_PIN , GPIO_PIN_SET );\
                             else \
                             HAL_GPIO_WritePin ( TFT_BLK_PORT, TFT_BLK_PIN , GPIO_PIN_RESET );

/* 数据/指令引脚 -- DC */
#define     TFT_DC_PORT    LCD_DC_GPIO_Port
#define     TFT_DC_PIN     LCD_DC_Pin

#define     LCD_DC_SET(bit)  if(bit) \
                             HAL_GPIO_WritePin ( TFT_DC_PORT, TFT_DC_PIN , GPIO_PIN_SET );\
                             else \
                             HAL_GPIO_WritePin ( TFT_DC_PORT, TFT_DC_PIN , GPIO_PIN_RESET );



//---------spi函数声明-------------------
void tft_spi_init(void);
rt_err_t st7789_send_cmd(const rt_uint8_t cmd);
rt_err_t st7789_send_data(const rt_uint8_t data);
rt_err_t st7789_send_half_word(const rt_uint16_t data);





#endif /* APPLICATIONS_MACBSP_INC_ST7789_SPI_H_ */
