/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-30     18452       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_FT6336U_IIC_H_
#define APPLICATIONS_MACBSP_INC_FT6336U_IIC_H_

#include "bsp_sys.h"


/* 软件iic配置参数结构体 */
typedef struct {
    char        i2c_name[16];
    uint8_t     i2c_addr;
    uint8_t     i2c_read_flag;
    uint8_t     i2c_write_flag;
    struct rt_i2c_bus_device    *i2c_bus;
}iicStructure_t;
extern iicStructure_t ft6336u_iic;


/* 复位引脚 -- RST */
#define     TOUCH_RST_PORT       TOUCH_RST_GPIO_Port
#define     TOUCH_RST_PIN        TOUCH_RST_Pin

#define     TOUCH_RST_SET(bit)   if(bit) \
                                 HAL_GPIO_WritePin ( TOUCH_RST_PORT, TOUCH_RST_PIN , GPIO_PIN_SET );\
                                 else \
                                 HAL_GPIO_WritePin ( TOUCH_RST_PORT, TOUCH_RST_PIN , GPIO_PIN_RESET );


//-----------函数声明------------------
void ft6336u_device_init(void);
rt_err_t iic_ft6336u_write_reg(struct rt_i2c_bus_device *bus,rt_uint8_t *i2c_reg);
rt_err_t iic_ft6336u_write_reg_datas(struct rt_i2c_bus_device *bus,rt_uint8_t* data_buf);
rt_err_t iic_ft6336u_read_reg(struct rt_i2c_bus_device *bus, rt_uint8_t len,rt_uint8_t* i2c_dat);
rt_err_t iic_ft6336u_write_reg_nbytes(struct rt_i2c_bus_device *bus,rt_uint8_t reg,rt_uint8_t *data,rt_size_t n);

#endif /* APPLICATIONS_MACBSP_INC_FT6336U_IIC_H_ */
