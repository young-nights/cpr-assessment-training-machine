/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-12-05     18452       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_ADC1115IDGSR_H_
#define APPLICATIONS_MACBSP_INC_ADC1115IDGSR_H_

#include "bsp_sys.h"


#if 0

// ADS1115 I2C 地址（根据 ADDR 引脚）
#define ADS1115_GND_ADDR    0x48    // ADDR引脚接GND时，芯片的IIC地址
#define ADS1115_VCC_ADDR    0x49    // ADDR引脚接VCC时，芯片的IIC地址
#define ADS1115_SCL_ADDR    0x4A    // ADDR引脚接SCL时，芯片的IIC地址
#define ADS1115_SDA_ADDR    0x4B    // ADDR引脚接SDA时，芯片的IIC地址

// ADS1115 寄存器地址
#define ADS1115_REG_CONVERT     0x00    // 转换寄存器
#define ADS1115_REG_CONFIG      0x01    // 配置寄存器
#define ADS1115_REG_LO_THRESH   0x02    // 低阈值
#define ADS1115_REG_HI_THRESH   0x03    // 高阈值



typedef enum {
    ADS1115_MODE_CONTINUOUS = 0x0000,
    ADS1115_MODE_SINGLE     = 0x0100,
} ads1115_mode_t;


// 量程对应的实际电压值（单位：V）
typedef enum {
    ADS1115_FSR_6_144V = 6144,   // mV
    ADS1115_FSR_4_096V = 4096,
    ADS1115_FSR_2_048V = 2048,
    ADS1115_FSR_1_024V = 1024,
    ADS1115_FSR_0_512V = 512,
    ADS1115_FSR_0_256V = 256,
} ads1115_fsr_mv_t;


// 配置寄存器位定义（简化常用）
typedef enum {
    ADS1115_MUX_AIN0_AIN1 = 0x0000,   // 差分
    ADS1115_MUX_AIN0_AIN3 = 0x1000,
    ADS1115_MUX_AIN1_AIN3 = 0x2000,
    ADS1115_MUX_AIN2_AIN3 = 0x3000,
    ADS1115_MUX_AIN0_GND  = 0x4000,   // 单端
    ADS1115_MUX_AIN1_GND  = 0x5000,
    ADS1115_MUX_AIN2_GND  = 0x6000,
    ADS1115_MUX_AIN3_GND  = 0x7000,
} ads1115_mux_t;


typedef enum {
    ADS1115_PGA_6_144V = 0x0000,   // ±6.144V
    ADS1115_PGA_4_096V = 0x0200,
    ADS1115_PGA_2_048V = 0x0400,   // 默认
    ADS1115_PGA_1_024V = 0x0600,
    ADS1115_PGA_0_512V = 0x0800,
    ADS1115_PGA_0_256V = 0x0A00,
} ads1115_pga_t;


typedef enum {
    ADS1115_DR_8SPS   = 0x0000,
    ADS1115_DR_16SPS  = 0x0020,
    ADS1115_DR_32SPS  = 0x0040,
    ADS1115_DR_64SPS  = 0x0060,
    ADS1115_DR_128SPS = 0x0080,   // 默认
    ADS1115_DR_250SPS = 0x00A0,
    ADS1115_DR_475SPS = 0x00C0,
    ADS1115_DR_860SPS = 0x00E0,
} ads1115_dr_t;


/* 软件iic配置参数结构体 */
typedef struct {
    char        i2c_name[16];
    uint8_t     i2c_addr;
    struct rt_i2c_bus_device    *i2c_bus;
}iicStructure_t;
extern iicStructure_t ads1115_iic;




//-----------函数声明------------------
int ads1115_device_init(void);
rt_err_t iic_ads1115_write_reg(uint8_t reg, uint16_t data);
rt_err_t iic_ads1115_read_reg(rt_uint8_t reg, rt_uint16_t* i2c_dat);
rt_err_t ads1115_set_config(uint16_t config);
rt_err_t ads1115_start_single_conversion(uint8_t channel);
int16_t ads1115_read_raw(uint8_t channel);
float ads1115_read_voltage(uint8_t channel);
int ads1115_read_voltage_mv(uint8_t channel);
float ads1115_get_ain0(void);
float ads1115_get_ain1(void);


#endif


#endif /* APPLICATIONS_MACBSP_INC_ADC1115IDGSR_H_ */
