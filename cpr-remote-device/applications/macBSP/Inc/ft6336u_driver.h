/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-30     18452       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_FT6336U_DRIVER_H_
#define APPLICATIONS_MACBSP_INC_FT6336U_DRIVER_H_
#include "bsp_sys.h"
#include "lv_area.h"



//FT6336U芯片相关信息结构液体
typedef struct
{
    unsigned long    CPIPHER_INFO;
    uint16_t    LIB_VERSION;
    uint8_t     FIRMWARE_VERSION;
    uint8_t     VENDOR_ID;
}FT6336U_IC_INFO;
extern FT6336U_IC_INFO ft6336u_info;



//FT6336U芯片坐标点信息
typedef struct{
    lv_coord_t point1_x;
    lv_coord_t point1_y;
    lv_coord_t point2_x;
    lv_coord_t point2_y;
}ft6336u_xy;
extern ft6336u_xy tp_dev_xy;

//-----------寄存器定义------------------
typedef struct
{
    uint8_t  ID_G_CIPHER_HIGH;      // 读取：芯片代号高字节
    uint8_t  ID_G_CIPHER_MIDE;      // 读取：芯片代号中字节
    uint8_t  ID_G_CIPHER_LOW;       // 读取：芯片代号低字节
    uint8_t  ID_G_LIB_VERSION_H;    // 读取：APP库文件版本高字节
    uint8_t  ID_G_LIB_VERSION_L;    // 读取：APP库文件版本低字节
    uint8_t  ID_G_FIRMID;           // 读取：固件版本
    uint8_t  ID_G_FOCALTECH_ID;     // 读取：VENDOR ID
    uint8_t  ID_G_FACE_DEC_MODE;    // 设置：近距离感应使能
    uint8_t  TD_STATUS;             // 读取：最大报点个数
    uint8_t  P1_XH;                 // 读取：第一个点的X坐标高四位
    uint8_t  P1_XL;                 // 读取：第一个点的X坐标低八位
    uint8_t  P1_YH;                 // 读取：第一个点的Y坐标高四位
    uint8_t  P1_YL;                 // 读取：第一个点的Y坐标低八位
    uint8_t  P2_XH;                 // 读取：第二个点的X坐标高四位
    uint8_t  P2_XL;                 // 读取：第二个点的X坐标低八位
    uint8_t  P2_YH;                 // 读取：第二个点的Y坐标高四位
    uint8_t  P2_YL;                 // 读取：第二个点的Y坐标低八位
    uint8_t  ID_G_THGROUP;          // 读取：触摸阈值寄存器指令（默认值：0xBB）
    uint8_t  ID_G_CTRL;             // 设置：monitor开关模式
    uint8_t  ID_G_THDIFF;           // 读取：点滤波范围阈值（默认值：0xA0）
    uint8_t  ID_G_TIMEENTERMONITOR; // 设置：没触摸进入monitor延时时间
    uint8_t  ID_G_MODE;             // 设置：报点INT模式（默认：中断触发）

}FT6336U_IC_REG;
extern FT6336U_IC_REG ft6336u_reg;





//-----------函数声明------------------
void FT6336U_Reset(void);
void FT6336U_Work_Mode_Set(struct rt_i2c_bus_device *bus, rt_uint8_t mode);
void FT6336U_Read_Info(struct rt_i2c_bus_device *bus,FT6336U_IC_INFO *info);
rt_uint8_t FT6336U_Read_Finger_Number(struct rt_i2c_bus_device *bus);
void FT6336U_Hover_Detection_Set(struct rt_i2c_bus_device *bus, rt_uint8_t cmd);
void FT6336U_Monitor_Set(struct rt_i2c_bus_device *bus, rt_uint8_t cmd);
void FT6336U_Monitor_Time_Set(struct rt_i2c_bus_device *bus, rt_uint8_t time_d);
void FT6336U_Report_FingerPoint_Mode_Set(struct rt_i2c_bus_device *bus, rt_uint8_t mode);
rt_err_t FT6336U_Read_Pressed_Point_xy(struct rt_i2c_bus_device *bus);


#endif /* APPLICATIONS_MACBSP_INC_FT6336U_DRIVER_H_ */
