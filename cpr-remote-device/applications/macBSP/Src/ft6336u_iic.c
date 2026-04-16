/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-30     18452       the first version
 */
#include <ft6336u_iic.h>




iicStructure_t ft6336u_iic = {
        .i2c_name       = "i2c1",   /* 设备结点 */
        .i2c_addr       = 0x38,     /* ft6336u的iic地址 */
        .i2c_write_flag = 0x70,     /* ft6336u的写标志   */
        .i2c_read_flag  = 0x71,     /* ft6336u的读标志   */
};


/**
 * @brief   ft6336u初始化IIC总线
 * @param   None
 * @return  None
 */
void ft6336u_device_init(void)
{
    ft6336u_iic.i2c_bus = (struct rt_i2c_bus_device *)(rt_device_find(ft6336u_iic.i2c_name));
    if(ft6336u_iic.i2c_bus != RT_NULL){
        rt_kprintf("PRINTF:%d. ft6336u_iic bus is found!\r\n",Record.kprintf_cnt++);
    }
    else {
        rt_kprintf("ft6336u_iic bus can't find!\r\n");
    }
}


/**
 * @brief   ft6336u向寄存器中写入一个字节的数据
 * @param  *bus     : 设备结点
 *         *i2c_reg : 寄存器地址(传入的是地址指针)
 * @return
 */
rt_err_t iic_ft6336u_write_reg(struct rt_i2c_bus_device *bus,rt_uint8_t *i2c_reg)
{
    /* 定义IIC消息结构体 */
    struct rt_i2c_msg   ft6336u_msg;
    rt_uint8_t *SendDat;
    SendDat = i2c_reg;

    ft6336u_msg.addr  = ft6336u_iic.i2c_addr;
    ft6336u_msg.flags = RT_I2C_WR;
    ft6336u_msg.buf   = SendDat; /* 这里传递的是数据变量的地址 */
    ft6336u_msg.len   = 1;

    if(rt_i2c_transfer(bus, &ft6336u_msg, 1) == 1){
        return RT_EOK;
    }
    else {
        return RT_ERROR;
    }
}




/**
 * @brief   向 FT6336U 指定寄存器连续写入 n 个字节数据
 * @param   bus        : i2c 总线句柄
 * @param   reg        : 起始寄存器地址（单字节）
 * @param   data       : 待写入数据的缓冲区首地址
 * @param   n          : 待写入的字节数
 * @return  rt_err_t   : RT_EOK / RT_ERROR
 */
rt_err_t iic_ft6336u_write_reg_nbytes(struct rt_i2c_bus_device *bus,
                                      rt_uint8_t reg,
                                      rt_uint8_t *data,
                                      rt_size_t n)
{
    /* 构造一次完整的消息：reg + data[0..n-1] */
    rt_uint8_t buf[32];                 /* 根据实际一次传输长度调整大小 */
    if (n + 1 > sizeof(buf))
        return RT_ERROR;

    buf[0] = reg;
    rt_memcpy(&buf[1], data, n);

    struct rt_i2c_msg msg;
    msg.addr  = ft6336u_iic.i2c_addr;
    msg.flags = RT_I2C_WR;
    msg.buf   = buf;
    msg.len   = n + 1;                  /* 加上寄存器地址本身的长度 */

    return (rt_i2c_transfer(bus, &msg, 1) == 1) ? RT_EOK : RT_ERROR;
}



/**
 * @brief   ft6336u向寄存器中写入多个字节的数据(reg+data)
 * @param  *bus     : 设备结点
 *         *data_buf: 数组地址(传入的是地址指针)
 * @return
 */
rt_err_t iic_ft6336u_write_reg_datas(struct rt_i2c_bus_device *bus,rt_uint8_t* data_buf)
{
    /* 定义IIC消息结构体 */
    struct rt_i2c_msg   ft6336u_msg;
    rt_uint8_t *SendDat;
    rt_uint8_t len;
    SendDat = data_buf;

    len = sizeof(SendDat);

    ft6336u_msg.addr  = ft6336u_iic.i2c_addr;
    ft6336u_msg.flags = RT_I2C_WR;
    ft6336u_msg.buf   = SendDat;
    ft6336u_msg.len   = len;

    if(rt_i2c_transfer(bus, &ft6336u_msg, 1) == 1){
        return RT_EOK;
    }
    else {
        return RT_ERROR;
    }
}



/**
 * @brief   ft6336u读取数据
 * @param  *bus     : iic设备句柄
 *          len     : 要读取的数据长度
 *          i2c_dat : 读取数据存入的数组
 * @return
 */
rt_err_t iic_ft6336u_read_reg(struct rt_i2c_bus_device *bus, rt_uint8_t len,rt_uint8_t* i2c_dat)
{


    struct  rt_i2c_msg  ft6336u_msg;

    ft6336u_msg.addr  = ft6336u_iic.i2c_addr;
    ft6336u_msg.flags = RT_I2C_RD;
    ft6336u_msg.buf   = i2c_dat;
    ft6336u_msg.len   = len;

    /* 调用I2C设备接口传输数据 */
    if(rt_i2c_transfer(bus, &ft6336u_msg, 1) == 1){
        return RT_EOK;
    }
    else {
        return RT_ERROR;
    }
}



