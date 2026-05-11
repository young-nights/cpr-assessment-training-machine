/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-21     Administrator       the first version
 */
#include "st7789_spi.h"


#define DBG_TAG    "drv.lcd"
#define DBG_LVL    DBG_INFO

//以下是SPI以及LCD引脚配置相关的函数---------------------------------------------------------------------------------------------

/* 宏定义SPI名称 */
#define     TFT_SPI_NAME    "tft_spi1"
/* 宏定义SPI总线 */
#define     TFT_SPI_BUS     "spi1"
/* 宏定义SPI句柄 */
struct rt_spi_device *spi_dev_tft;



/**
  * @brief  SPI Initialization
  * @retval void
  */
void tft_spi_init(void)
{
    /* 将SPI设备名挂载到总线 */
    rt_hw_spi_device_attach(TFT_SPI_BUS, TFT_SPI_NAME, TFT_CS_PORT, TFT_CS_PIN);
    /* 查找SPI设备 */
    spi_dev_tft = (struct rt_spi_device *)rt_device_find(TFT_SPI_NAME);
    if(spi_dev_tft == NULL){
        rt_kprintf("PRINTF:%d. SPI Device is not created!\r\n",Record.kprintf_cnt++);
    }
    else{
        rt_kprintf("PRINTF:%d. SPI Device is successfully!\r\n",Record.kprintf_cnt++);
    }
    /***
     *! 配置SPI结构体参数
     *! data_width: spi传输数据长度为8bits
     *! max_hz: spi的最大工作频率；
     *! mode-> RT_SPI_MASTER: 主机模式；
     *! mode-> RT_SPI_MODE_0: 工作模式0（CPOL:0  CPHA:0）
     *! mode-> RT_SPI_MSB: 通讯数据高位在前低位在后
     * */
    struct rt_spi_configuration spi1_cfg;
    spi1_cfg.data_width = 8;
    spi1_cfg.max_hz = 42*1000*1000; /* 42M,SPI max 42MHz,lcd 4-wire spi */
    spi1_cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_2 | RT_SPI_MSB;
    rt_spi_configure(spi_dev_tft, &spi1_cfg); /* 使能参数 */
}




/**
  * @brief  spi send command function
  * @retval void
  */
rt_err_t st7789_send_cmd(const rt_uint8_t cmd)
{
    rt_size_t len;

    LCD_DC_SET(PIN_LOW);
    len = rt_spi_send(spi_dev_tft, &cmd, 1);

    if (len != 1){
        LOG_I("lcd_write_cmd error. %d", len);
        return -RT_ERROR;
    }
    else{
        return RT_EOK;
    }
}


/**
  * @brief  spi send data(one byte) function
  * @retval void
  */
rt_err_t st7789_send_data(const rt_uint8_t data)
{
    rt_size_t len;

    LCD_DC_SET(PIN_HIGH);
    len = rt_spi_send(spi_dev_tft, &data, 1);

    if (len != 1){
        LOG_I("lcd_write_data error. %d", len);
        return -RT_ERROR;
    }
    else{
        return RT_EOK;
    }
}


/**
  * @brief  spi send data(half word) function
  * @retval void
  */
rt_err_t st7789_send_half_word(const rt_uint16_t data)
{
    rt_size_t len;

    char sendbuf[2] = {0};

    sendbuf[0] = data >> 8;
    sendbuf[1] = data;

    LCD_DC_SET(PIN_HIGH);
    len = rt_spi_send(spi_dev_tft, sendbuf, 2);

    if (len != 2){
        LOG_I("lcd_write_half_word error. %d", len);
        return -RT_ERROR;
    }
    else{
        return RT_EOK;
    }
}











