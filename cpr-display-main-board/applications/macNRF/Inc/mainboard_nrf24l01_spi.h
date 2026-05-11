/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-07-29     Administrator       the first version
 */
#ifndef APPLICATIONS_MACNRF_INC_MAINBOARD_NRF24L01_SPI_H_

#define APPLICATIONS_MACNRF_INC_MAINBOARD_NRF24L01_SPI_H_
#include "bsp_sys.h"
#include <macNRF/Inc/mainboard_nrf24l01_driver.h>


#define USE_CUSTOMER_NRF24L01 1
#if USE_CUSTOMER_NRF24L01


/* 片选引脚 -- CE (使用 RT-Thread rt_pin API) */
#define     nRF24_CS_SET(bit) rt_pin_write(GET_PIN(D, 8), (bit) ? PIN_HIGH : PIN_LOW)


/* SPI引脚 -- NSS/CSN (用于 rt_hw_spi_device_attach) */
#define     nRF24_NSS_PORT     nRF24L01_CSN_GPIO_Port
#define     nRF24_NSS_PIN      nRF24L01_CSN_Pin

/* CSN 控制 (使用 RT-Thread rt_pin API) */
#define     nRF24_NSS_SET(bit) rt_pin_write(GET_PIN(B, 12), (bit) ? PIN_HIGH : PIN_LOW)


extern const struct nRF24L01_FUNC_OPS g_nrf24_func_ops;


#endif

#endif /* APPLICATIONS_MACNRF_INC_MAINBOARD_NRF24L01_SPI_H_ */
