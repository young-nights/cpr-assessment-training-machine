/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-12-05     18452       the first version
 */
#include "adc1115idgsr.h"

#include "rtdbg.h"


#if 0

iicStructure_t ads1115_iic = {
        .i2c_name       = "i2c1",   /* 设备结点 */
        .i2c_addr       = ADS1115_GND_ADDR,     /* ads1115的iic地址 */
};






/**
 * @brief  向 ADS1115 的指定寄存器写入 16bit 配置数据（高字节在前）
 * @param  reg   : 寄存器地址（0x00 ~ 0x03）
 *                  - 0x00 : Conversion register（转换结果）
 *                  - 0x01 : Config register    （配置寄存器） ← 我们最常用
 *                  - 0x02 : Lo_thresh register （低阈值）
 *                  - 0x03 : Hi_thresh register （高阈值）
 * @param  data  : 要写入的 16bit 数据（ADS1115 所有寄存器都是 16bit）
 * @retval RT_EOK  : 写入成功
 *         RT_ERROR: 写入失败（I2C 通信异常）
 */
rt_err_t iic_ads1115_write_reg(uint8_t reg, uint16_t data)
{
    uint8_t buf[3];   // I2C 要发送的完整数据包：寄存器地址 + 16bit 数据
    struct rt_i2c_msg ads1115_msg;   // RT-Thread 的 I2C 消息结构体

    /* 第一步：组装要发送的数据 */
    buf[0] = reg;                    // 第1个字节：目标寄存器地址
    buf[1] = (uint8_t)(data >> 8);   // 第2个字节：数据高8位（ADS1115 是大端模式）
    buf[2] = (uint8_t)(data & 0xFF); // 第3个字节：数据低8位

    /* 第二步：填充 I2C 消息结构体 */
    ads1115_msg.addr  = ads1115_iic.i2c_addr;   // 从设备地址（7位），比如 0x48
    ads1115_msg.flags = RT_I2C_WR;              // 写操作（RT_I2C_WR = 0）
    ads1115_msg.buf   = buf;                    // 要发送的数据缓冲区指针
    ads1115_msg.len   = 3;                      // 总共发送 3 个字节（1字节寄存器地址 + 2字节数据）

    if (rt_i2c_transfer(ads1115_iic.i2c_bus, &ads1115_msg, 1) == 1)
        return RT_EOK;
    else
        return RT_ERROR;
}




/**
 * @brief  从 ADS1115 的指定寄存器读取 16bit 数据（高字节在前）
 * @param  reg       : 要读取的寄存器地址
 *                     - 0x00 : Conversion 寄存器（转换结果，最常用！）
 *                     - 0x01 : Config 寄存器（当前配置）
 *                     - 0x02 : Lo_thresh 寄存器
 *                     - 0x03 : Hi_thresh 寄存器
 * @param  i2c_dat   : 指针，用于返回读取到的 16bit 数据
 * @retval RT_EOK    : 读取成功
 *         RT_ERROR  : 读取失败（I2C 总线错误、从机无应答等）
 */
rt_err_t iic_ads1115_read_reg(rt_uint8_t reg, rt_uint16_t* i2c_dat)
{

    uint8_t tmp = reg;
    uint8_t buf[2];
    struct rt_i2c_msg msgs[2];

    msgs[0].addr  = ads1115_iic.i2c_addr;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = &tmp;
    msgs[0].len   = 1;

    msgs[1].addr  = ads1115_iic.i2c_addr;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf   = buf;
    msgs[1].len   = 2;

    if (rt_i2c_transfer(ads1115_iic.i2c_bus, msgs, 2) == 2) {
        *i2c_dat = (buf[0] << 8) | buf[1];
        return RT_EOK;
    }
    return RT_ERROR;
}



/**
 * @brief  向 ADS1115 的配置寄存器（Config Register）写入一个完整的 16bit 配置值
 *         并自动帮你启动一次单次转换（最常用场景）
 *
 * @param  config  : 你想设置的配置值（15位有效，第15位稍后会被函数自动置1）
 *                   通常由下面这些宏组合而成：
 *                   - MUX[14:12]    : 输入通道选择（单端/差分）
 *                   - PGA[11:9]     : 可编程增益（量程）
 *                   - MODE[8]       : 0=连续转换, 1=单次转换（省电）
 *                   - DR[7:5]       : 数据速率（8~860SPS）
 *                   - COMP_MODE[4]  : 比较器模式
 *                   - COMP_POL[3]   : 比较器极性
 *                   - COMP_LAT[2]   : 锁存模式
 *                   - COMP_QUE[1:0] : 比较器触发次数
 *
 * @retval RT_EOK   : 配置写入成功，单次转换已启动
 *         RT_ERROR : I2C通信失败
 *
 * @note   ADS1115 配置寄存器的最高位是 OS 位（Bit 15）：
 *           - 写 1 → 启动一次单次转换（仅在单次模式下有效）
 *           - 读 1 → 表示当前正在转换或转换已完成
 *           - 读 0 → 表示还在转换中
 *         所以我们一般在写配置时都把最高位强制置1，这样写完配置就立刻开始一次转换
 */
rt_err_t ads1115_set_config(uint16_t config)
{
    return iic_ads1115_write_reg(ADS1115_REG_CONFIG, config | 0x8000); // OS=1 开始单次转换
}



rt_err_t ads1115_start_single_conversion(uint8_t channel)
{
    uint16_t config = 0;

    // 基础配置
    config |= 0x8000;                       // OS = 1 启动转换
    config |= ADS1115_MODE_SINGLE;          // 单次模式
    config |= ADS1115_DR_128SPS;            // 128SPS
    config |= ADS1115_PGA_6_144V;           // ±6.144V（完美覆盖 0~5V）

    // 通道选择
    switch (channel) {
        case 0: config |= ADS1115_MUX_AIN0_GND; break;
        case 1: config |= ADS1115_MUX_AIN1_GND; break;
        case 2: config |= ADS1115_MUX_AIN2_GND; break;
        case 3: config |= ADS1115_MUX_AIN3_GND; break;
        default: return RT_ERROR;
    }

    return iic_ads1115_write_reg(ADS1115_REG_CONFIG, config);
}


/**
 * @brief  读取 ADS1115 指定通道的原始 16bit ADC 值（有符号）
 *         自动启动单次转换 → 智能等待转换完成 → 返回带符号的原始码值
 *
 * @param  channel : 通道号 0~3（对应 AIN0~AIN3 单端对地）
 * @return int16_t : 转换完成的原始 ADC 值
 *                   范围：-32768 ~ +32767
 *                   正电压时：0 ~ 32767
 *                   负电压（差分模式）时：会出现负数
 *                   失败或超时：返回 0（注意：0 也是一个合法值！建议配合返回值判断）
 *
 * @note   本函数内部自带超时保护 + 智能轮询，绝对不会死等
 */
int16_t ads1115_read_raw(uint8_t channel)
{
    uint16_t val;       // 临时存放寄存器读取的值（配置寄存器或转换结果）
    int retry = 100;    // 最大重试 100 次 ≈ 100ms 超时保护

    if (ads1115_start_single_conversion(channel) != RT_EOK){
        return 0;
    }


    // 等待转换完成（轮询配置寄存器最高位）
    do {
        rt_thread_mdelay(1);
        if (iic_ads1115_read_reg(ADS1115_REG_CONFIG, &val) == RT_EOK) {
            if (val & 0x8000) break;  // OS=1 表示转换完成
        }
    } while (--retry > 0);

    if (retry == 0) {
        rt_kprintf("ADS1115 timeout!\r\n");
        return 0;
    }

    /* ADS1115 输出是 16bit 有符号整数，左对齐（高位在前）
               直接强转为 int16_t 即可自动处理符号扩展 */
    if (iic_ads1115_read_reg(ADS1115_REG_CONVERT, &val) == RT_EOK) {
        return (int16_t)val;  // 有符号扩展
    }
    return 0;
}


/**
 * @brief   ads1115初始化IIC总线
 * @param   None
 * @return  None
 */
int ads1115_device_init(void)
{
    ads1115_iic.i2c_bus = (struct rt_i2c_bus_device *)(rt_device_find(ads1115_iic.i2c_name));
    if(ads1115_iic.i2c_bus != RT_NULL){
        rt_kprintf("PRINTF:%d. ads1115_iic bus is found!\r\n",Record.kprintf_cnt++);
        rt_kprintf("PRINTF:%d. ADS1115 init success, addr=0x%02X\r\n", Record.kprintf_cnt++, ads1115_iic.i2c_addr);
    }
    else {
        rt_kprintf("ads1115_iic bus can't find!\r\n");
    }
    return RT_EOK;
}
INIT_APP_EXPORT(ads1115_device_init);


//------------------------------------------------------------------------------------------------------------------



/* 读取电压（浮点） */
float ads1115_read_voltage(uint8_t channel)
{
    if(channel > 3){
        LOG_E("ADS1115 read channel out of range.");
    }

    int16_t raw = ads1115_read_raw(channel);
    float lsb;

    lsb = 6.144f / 32768.0f;  // ±6.144V 范围

    return raw * lsb;
}

/* 单位转换： V -> mV */
int ads1115_read_voltage_mv(uint8_t channel)
{
    float v = ads1115_read_voltage(channel);
    if (v < 0) return -1;
    return (int)(v * 1000.0f + 0.5f);  // 四舍五入
}



float ads1115_get_ain0(void)
{
    return ads1115_read_voltage(0);
}

float ads1115_get_ain1(void)
{
    return ads1115_read_voltage(1);
}




#endif

