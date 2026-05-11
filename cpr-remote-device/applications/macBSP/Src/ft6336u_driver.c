/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-30     18452       the first version
 */
#include "ft6336u_driver.h"

#define USE_PRINTF_POINT_XY 0

FT6336U_IC_REG ft6336u_reg = {
        .ID_G_CIPHER_HIGH       = 0xA3, // 读取：芯片代号高字节
        .ID_G_CIPHER_MIDE       = 0x9F, // 读取：芯片代号中字节
        .ID_G_CIPHER_LOW        = 0xA0, // 读取：芯片代号低字节
        .ID_G_LIB_VERSION_H     = 0xA1, // 读取：APP库文件版本高字节
        .ID_G_LIB_VERSION_L     = 0xA2, // 读取：APP库文件版本低字节
        .ID_G_FIRMID            = 0xA6, // 读取：固件版本
        .ID_G_FOCALTECH_ID      = 0xA8, // 读取：VENDOR ID
        .TD_STATUS              = 0x02, // 读取：最大报点个数
        .P1_XH                  = 0x03, // 读取：第一个点的X坐标高四位
        .P1_XL                  = 0x04, // 读取：第一个点的X坐标低八位
        .P1_YH                  = 0x05, // 读取：第一个点的Y坐标高四位
        .P1_YL                  = 0x06, // 读取：第一个点的Y坐标低八位
        .P2_XH                  = 0x09, // 读取：第二个点的X坐标高四位
        .P2_XL                  = 0x0A, // 读取：第二个点的X坐标低八位
        .P2_YH                  = 0x0B, // 读取：第二个点的Y坐标高四位
        .P2_YL                  = 0x0C, // 读取：第二个点的Y坐标低八位
        .ID_G_THGROUP           = 0x80, // 读取：触摸阈值寄存器指令（默认值：0xBB）
        .ID_G_THDIFF            = 0x85, // 读取：点滤波范围阈值（默认值：0xA0）

        .ID_G_FACE_DEC_MODE     = 0xB0, // 设置：近距离感应使能
        .ID_G_CTRL              = 0x86, // 设置：monitor开关模式
        .ID_G_TIMEENTERMONITOR  = 0x87, // 设置：没触摸进入monitor延时时间
        .ID_G_MODE              = 0xA4, // 设置：报点模式设置
};


/**
 * @brief  ft6336u芯片复位函数
 * @param  void
 * @return NULL
 */
void FT6336U_Reset(void)
{
    TOUCH_RST_SET(0);
    rt_thread_mdelay(120);
    TOUCH_RST_SET(1);
    rt_thread_mdelay(300);
}



/**
 * @brief  ft6336u工作模式切换
 * @param  void
 * @return NULL
 */
#define WORK_MODE       0xf4    // 工作模式
#define FACTORY_MODE    0xf3    // 工厂模式
void FT6336U_Work_Mode_Set(struct rt_i2c_bus_device *bus, rt_uint8_t mode)
{
    static rt_uint8_t change_cmd,change_reg = 0;
    if(mode == 0xF4)
    {
        change_reg = 0x00;
        change_cmd = 0x00;
        iic_ft6336u_write_reg_nbytes(bus, change_reg, &change_cmd, 1);
        rt_kprintf("PRINTF:%d. Set work mode for FT6336U chip.\r\n",Record.kprintf_cnt++);
    }
    else if (mode == 0xF3)
    {
        change_reg = 0x40;
        change_cmd = 0x00;
        iic_ft6336u_write_reg_nbytes(bus, change_reg, &change_cmd, 1);
        rt_kprintf("PRINTF:%d. Set factory mode for FT6336U chip.\r\n",Record.kprintf_cnt++);
    }
}



/**
 * @brief  读取ft6336u芯片的内置信息
 * @param  void
 * @return NULL
 */
FT6336U_IC_INFO ft6336u_info;
void FT6336U_Read_Info(struct rt_i2c_bus_device *bus,FT6336U_IC_INFO *info)
{

    rt_uint8_t info_buf[3] = { 0 };
    rt_uint8_t libv_buf[2] = { 0 };
    rt_uint8_t frame_version = 0;
    rt_uint8_t vendor_id = 0;

    /* 设置工作模式 */
    FT6336U_Work_Mode_Set(bus, WORK_MODE);

    //--------------------------------------------------------------
    /*! 根据数据手册，进行读操作时，按照 “从机地址 + 发送标志 + 寄存器地址 ”进行第一次发送
     *! 读取ft6336u的芯片类型码
     * */

    iic_ft6336u_write_reg(bus, &ft6336u_reg.ID_G_CIPHER_HIGH);
    iic_ft6336u_read_reg(bus, 1, &info_buf[0]);

    iic_ft6336u_write_reg(bus, &ft6336u_reg.ID_G_CIPHER_MIDE);
    iic_ft6336u_read_reg(bus, 1, &info_buf[1]);

    iic_ft6336u_write_reg(bus, &ft6336u_reg.ID_G_CIPHER_LOW);
    iic_ft6336u_read_reg(bus, 1, &info_buf[2]);

    info->CPIPHER_INFO = (info_buf[0] << 16) + (info_buf[1] << 8) + info_buf[2];
    rt_kprintf("PRINTF:%d. info->CPIPHER_INFO == %x \r\n",Record.kprintf_cnt++,info->CPIPHER_INFO);
    if(info->CPIPHER_INFO == 0x642600){
        rt_kprintf("PRINTF:%d. This Touch Chip Type is FT6236G. \r\n",Record.kprintf_cnt++);
    }
    else if(info->CPIPHER_INFO == 0x642601){
        rt_kprintf("PRINTF:%d. This Touch Chip Type is FT6336G. \r\n",Record.kprintf_cnt++);
    }
    else if(info->CPIPHER_INFO == 0x642602){
        rt_kprintf("PRINTF:%d. This Touch Chip Type is FT6336U. \r\n",Record.kprintf_cnt++);
    }
    else if(info->CPIPHER_INFO == 0x642603){
        rt_kprintf("PRINTF:%d. This Touch Chip Type is FT6426. \r\n",Record.kprintf_cnt++);
    }
    else{
        rt_kprintf("PRINTF:%d. This Touch Chip Type is not found. \r\n",Record.kprintf_cnt++);
    }
    rt_kprintf("\r\n");
    //--------------------------------------------------------------
    /*! 读取ft6336u的库版本信息
     * */
    iic_ft6336u_write_reg(bus, &ft6336u_reg.ID_G_LIB_VERSION_H);
    iic_ft6336u_read_reg(bus, 1, &libv_buf[0]);

    iic_ft6336u_write_reg(bus, &ft6336u_reg.ID_G_LIB_VERSION_L);
    iic_ft6336u_read_reg(bus, 1, &libv_buf[1]);

    info->LIB_VERSION = (libv_buf[0] << 8) + libv_buf[1];
    rt_kprintf("PRINTF:%d. Touch Chip library file version is 0x%02x. \r\n",Record.kprintf_cnt++,info->LIB_VERSION);
    rt_kprintf("\r\n");
    //--------------------------------------------------------------
    /*! 读取ft6336u的固件版本
     * */
    iic_ft6336u_write_reg(bus, &ft6336u_reg.ID_G_FIRMID);
    iic_ft6336u_read_reg(bus, 1, &frame_version);
    info->FIRMWARE_VERSION = frame_version;
    rt_kprintf("PRINTF:%d. Touch Chip frameware version is 0x%02x. \r\n",Record.kprintf_cnt++,info->FIRMWARE_VERSION);
    rt_kprintf("\r\n");
    //--------------------------------------------------------------
    /*! 读取ft6336u的VENDOR ID
     * */
    iic_ft6336u_write_reg(bus, &ft6336u_reg.ID_G_FOCALTECH_ID);
    iic_ft6336u_read_reg(bus, 1, &vendor_id);
    info->VENDOR_ID = vendor_id;
    rt_kprintf("PRINTF:%d. Touch Chip vendor_id is 0x%02x \r\n",Record.kprintf_cnt++,info->VENDOR_ID);
    rt_kprintf("\r\n");
    //--------------------------------------------------------------
}


/**
 * @brief  读取触摸屏触点个数
 * @param  void
 * @return 触摸点个数
 */
rt_uint8_t FT6336U_Read_Finger_Number(struct rt_i2c_bus_device *bus)
{
    static rt_uint8_t fingers = 0;

    iic_ft6336u_write_reg(bus, &ft6336u_reg.TD_STATUS);
    iic_ft6336u_read_reg(bus, 1, &fingers);

    /* 如果手指数量大于2，说明数据错误，此时认为无手指触碰 */
    if(fingers > 2){
        fingers = 0;
    }

    return fingers;
}





/**
 * @brief   近距离感应使能
 * @param   cmd: 0 --> Disable
 *               1 --> Enable
 * @return  NULL
 * @note    自电容式触控芯片特有的 Hover（悬浮）/接近检测功能
 *          当手指 尚未真正接触 TP，但已经 距离面板 1-3 mm 左右 时，FT6336U 仍能检测到电场变化，从而提前知道“有手指正在靠近”。
 */
void FT6336U_Hover_Detection_Set(struct rt_i2c_bus_device *bus, rt_uint8_t cmd)
{
    iic_ft6336u_write_reg_nbytes(bus, ft6336u_reg.ID_G_FACE_DEC_MODE, &cmd, 1);
}




/**
 * @brief   设置触控芯片的一种 “诊断/监控工作模式”使能/失能
 * @param   cmd: 0 --> Disable
 *               1 --> Enable
 * @return  NULL
 * @note    把它打开后，芯片会把 原始电容值、基线、噪声、触发阈值 等内部调试数据源源不断地通过 I²C 打印出来，供产线或研发人员做 实时观测和调参。
 *          总结：
 *                  Monitor 模式 = 触控芯片的“串口打印调试”开关
 *                  ON  → 输出原始数据，便于抓错、校准灵敏度、看噪声
 *                  OFF → 正常工作模式，只上报标准坐标事件，不做额外数据输出
 */
void FT6336U_Monitor_Set(struct rt_i2c_bus_device *bus, rt_uint8_t cmd)
{
    iic_ft6336u_write_reg_nbytes(bus, ft6336u_reg.ID_G_CTRL, &cmd, 1);
}




/**
 * @brief   设置触摸芯片在手指离开屏幕多久以后，芯片才真正关掉 Monitor 诊断输出并回到正常省电模式
 * @param   time：延时时间数据（单位：10ms）
 * @return  NULL
 * @note    应用场景：
 *          · 打开 Monitor 模式 → 芯片会不断把原始电容、基线等调试数据通过 I²C 吐出来
 *          · 数据量大、功耗高，不适合一直开着
 *          延时设置：
 *          ·0x00：立即退出 Monitor（只要手一离开就停）
 *          ·0x14：手指离开后 200 ms 再停
 *          ·0x32：手指离开后 500 ms 再停，依次类推
 */
void FT6336U_Monitor_Time_Set(struct rt_i2c_bus_device *bus, rt_uint8_t time_d)
{
    iic_ft6336u_write_reg_nbytes(bus, ft6336u_reg.ID_G_TIMEENTERMONITOR, &time_d, 1);
}




/**
 * @brief   设置 FT6336U 的报点工作模式（ID_G_MODE）
 * @param   mode  : 0x00 --> 轮询模式
 *                  0x01 --> 中断模式
 * @return  NULL
 */
void FT6336U_Report_FingerPoint_Mode_Set(struct rt_i2c_bus_device *bus, rt_uint8_t mode)
{
    iic_ft6336u_write_reg_nbytes(bus, ft6336u_reg.ID_G_MODE, &mode, 1);
}





/**
 * @brief   读取触点的坐标值
 * @param
 * @return
 */
ft6336u_xy tp_dev_xy;
rt_err_t FT6336U_Read_Pressed_Point_xy(struct rt_i2c_bus_device *bus)
{
    rt_uint8_t coord[4] = {0};          /* 坐标临时缓存区 */

    /* 1. 如果没有报点，或者报点异常就不处理坐标 */
    if(Record.touch_fingers > 2){
        tp_dev_xy.point1_x  = 0;
        tp_dev_xy.point1_y  = 0;
        tp_dev_xy.point2_x  = 0;
        tp_dev_xy.point2_y  = 0;
        return RT_FALSE;
    }

    /* 2. 读第 1 点高4位X坐标寄存器：0x03 */
    iic_ft6336u_write_reg(bus, &ft6336u_reg.P1_XH);
    iic_ft6336u_read_reg(bus, 1, &coord[0]);
#if USE_PRINTF_POINT_XY
    rt_kprintf("PRINTF:%d. coord[0] = 0x%02x\r\n",Record.kprintf_cnt,coord[0]);
#endif

    /* 3. 读第 1 点低8位X坐标寄存器：0x04 */
    iic_ft6336u_write_reg(bus, &ft6336u_reg.P1_XL);
    iic_ft6336u_read_reg(bus, 1, &coord[1]);

    tp_dev_xy.point1_x = ((coord[0] & 0x0F) << 8) | coord[1] ;
#if USE_PRINTF_POINT_XY
    rt_kprintf("PRINTF:%d. tp_dev_xy.point1_x = 0x%04x\r\n",Record.kprintf_cnt,tp_dev_xy.point1_x);
#endif

    /* 4. 读第 1 点高4位Y坐标寄存器：0x05 */
    iic_ft6336u_write_reg(bus, &ft6336u_reg.P1_YH);
    iic_ft6336u_read_reg(bus, 1, &coord[2]);

    /* 5. 读第 1 点低8位Y坐标寄存器：0x06 */
    iic_ft6336u_write_reg(bus, &ft6336u_reg.P1_YL);
    iic_ft6336u_read_reg(bus, 1, &coord[3]);

    tp_dev_xy.point1_y = ((coord[2] & 0x0F) << 8) | coord[3] ;
#if USE_PRINTF_POINT_XY
    rt_kprintf("PRINTF:%d. tp_dev_xy.point1_y = 0x%04x\r\n",Record.kprintf_cnt, tp_dev_xy.point1_y);
#endif

    /* 6. 若还有第 2 点，再读 0x09~0x0C(可选) */
    if (Record.touch_fingers == 2)
    {
        rt_uint8_t coord_xy[4] = {0};

        /* 7. 读第 2 点高4位X坐标寄存器：0x09 */
        iic_ft6336u_write_reg(bus, &ft6336u_reg.P2_XH);
        iic_ft6336u_read_reg(bus, 1, &coord_xy[0]);

        /* 8. 读第 2 点低8位X坐标寄存器：0x0A */
        iic_ft6336u_write_reg(bus, &ft6336u_reg.P2_XL);
        iic_ft6336u_read_reg(bus, 1, &coord_xy[1]);

        tp_dev_xy.point2_x = ((coord_xy[0] & 0x0F) << 8) | coord_xy[1] ;
#if USE_PRINTF_POINT_XY
        rt_kprintf("PRINTF:%d. tp_dev_xy.point2_x = 0x%04x\r\n",Record.kprintf_cnt, tp_dev_xy.point2_x);
#endif

        /* 9. 读第 2 点高4位Y坐标寄存器：0x0B */
        iic_ft6336u_write_reg(bus, &ft6336u_reg.P2_YH);
        iic_ft6336u_read_reg(bus, 1, &coord_xy[2]);

        /* 5. 读第 1 点低8位Y坐标寄存器：0x0C */
        iic_ft6336u_write_reg(bus, &ft6336u_reg.P2_YL);
        iic_ft6336u_read_reg(bus, 1, &coord_xy[3]);

        tp_dev_xy.point2_y = ((coord_xy[2] & 0x0F) << 8) | coord_xy[3] ;
#if USE_PRINTF_POINT_XY
        rt_kprintf("PRINTF:%d. tp_dev_xy.point2_y = 0x%04x\r\n",Record.kprintf_cnt, tp_dev_xy.point2_y);
#endif
    }

    return RT_TRUE;
}



