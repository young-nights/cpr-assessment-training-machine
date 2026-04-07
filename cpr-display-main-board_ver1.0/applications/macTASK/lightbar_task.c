/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-10-17     Administrator       the first version
 */
#include "bsp_sys.h"


//---------------------------------------------------------------------------------------
#define TM1638_CLK_H()      HAL_GPIO_WritePin(TM1638_CLK_GPIO_Port, TM1638_CLK_Pin,  GPIO_PIN_SET)
#define TM1638_CLK_L()      HAL_GPIO_WritePin(TM1638_CLK_GPIO_Port, TM1638_CLK_Pin,  GPIO_PIN_RESET)
#define TM1638_DIO_H()      HAL_GPIO_WritePin(TM1638_DIO_GPIO_Port, TM1638_DIO_Pin,  GPIO_PIN_SET)
#define TM1638_DIO_L()      HAL_GPIO_WritePin(TM1638_DIO_GPIO_Port, TM1638_DIO_Pin,  GPIO_PIN_RESET)
#define TM1638_STB_H()      HAL_GPIO_WritePin(TM1638_STB_GPIO_Port, TM1638_STB_Pin,  GPIO_PIN_SET)
#define TM1638_STB_L()      HAL_GPIO_WritePin(TM1638_STB_GPIO_Port, TM1638_STB_Pin,  GPIO_PIN_RESET)

//---------------------------------------------------------------------------------------

void TM1638_Delay_us(uint32_t us);
void TM1638_Write_Byte(rt_uint8_t data);
void TM1638_Set_Cmd(rt_uint8_t cmd);
void TM1638_Set_Brightness(FUNCTION_SWITCH status, rt_uint8_t level);
void TM1638_Write_Data(rt_uint8_t drig_addr, TM1629x_SEG_SELECT seg_addr);
void TM1638_Set_LEDBar(uint8_t count);


//---------------------------------------------------------------------------------------
/*
| 地址（十六进制） | 对应 GRID | 控制 SEG 位           |
| -------- | ------- | ------------------ |
| **00H**  | GRID1   | SEG1 ~ SEG8        |
| **01H**  | GRID1   | SEG9 ~ SEG10（+6空位） |
| **02H**  | GRID2   | SEG1 ~ SEG8        |
| **03H**  | GRID2   | SEG9 ~ SEG10       |
| **04H**  | GRID3   | SEG1 ~ SEG8        |
| **05H**  | GRID3   | SEG9 ~ SEG10       |
| **06H**  | GRID4   | SEG1 ~ SEG8        |
| **07H**  | GRID4   | SEG9 ~ SEG10       |
| ** ... **|  ...    | ...                |
*/
#define GRID_1L_ADDR     0x00   /*对应GRID1的输出SEG1~SEG8*/
#define GRID_1H_ADDR     0x01   /*对应GRID1的输出SEG9~SEG10*/
#define GRID_2L_ADDR     0x02   /*对应GRID2的输出SEG1~SEG8*/
#define GRID_2H_ADDR     0x03   /*对应GRID2的输出SEG9~SEG10*/
#define GRID_3L_ADDR     0x04   /*对应GRID3的输出SEG1~SEG8*/
#define GRID_3H_ADDR     0x05   /*对应GRID3的输出SEG9~SEG10*/
#define GRID_4L_ADDR     0x06   /*对应GRID4的输出SEG1~SEG8*/
#define GRID_4H_ADDR     0x07   /*对应GRID4的输出SEG9~SEG10*/
#define GRID_5L_ADDR     0x08   /*对应GRID5的输出SEG1~SEG8*/
#define GRID_5H_ADDR     0x09   /*对应GRID5的输出SEG9~SEG10*/
#define GRID_6L_ADDR     0x0A   /*对应GRID6的输出SEG1~SEG8*/
#define GRID_6H_ADDR     0x0B   /*对应GRID6的输出SEG9~SEG10*/
#define GRID_7L_ADDR     0x0C   /*对应GRID7的输出SEG1~SEG8*/
#define GRID_7H_ADDR     0x0D   /*对应GRID7的输出SEG9~SEG10*/
#define GRID_8L_ADDR     0x0E   /*对应GRID8的输出SEG1~SEG8*/
#define GRID_8H_ADDR     0x0F   /*对应GRID8的输出SEG9~SEG10*/




/**
  * @brief  Delay function(uint:us)
  * @param  us 微妙数
  * @retval void
  */
void TM1638_Delay_us(uint32_t us)
{
    for(; us; us--) __NOP();
}



/**
  * @brief  TM1638 Chip send one byte
  * @param  data 需要写入的数据
  * @retval void
  * @note   NULL
  */
/* 发送 1 字节（低位先行） */
void TM1638_Write_Byte(rt_uint8_t data)
{
    for(rt_uint8_t i = 0; i < 8; i++)
    {
        TM1638_CLK_L();
        if(data & 0x01){
            TM1638_DIO_H();
        }
        else {
            TM1638_DIO_L();
        }
        TM1638_Delay_us(5);
        TM1638_CLK_H();
        TM1638_Delay_us(5);
        data >>= 1;
    }
}




/**
  * @brief  TM1638 set data command
  * @param  cmd  : 数据命令指令
  * @retval void
  * @note
  */
void TM1638_Set_Cmd(rt_uint8_t cmd)
{
    TM1638_STB_H();
    TM1638_Delay_us(5);
    TM1638_STB_L();
    TM1638_Delay_us(5);
    TM1638_Write_Byte(cmd);
}



/**
  * @brief  TM1638 Set digital tube Brightness
  * @param  level: 亮度等级0~7
  *         switch:开关设置
  * @retval void
  * @note   找到寄存器手册的显示控制命令设置（用于设置亮度强度和显示使能）
  */
void TM1638_Set_Brightness(FUNCTION_SWITCH status, rt_uint8_t level)
{
#define DISPLAY_RES_BASE    0x88
    if(status == ON)
    {
        if(level > 7){
            level = 7;
        }

        rt_uint8_t cmd = DISPLAY_RES_BASE | level;

        TM1638_Set_Cmd(cmd);
    }
    else
    {
        rt_uint8_t cmd = 0x80;
        TM1638_Set_Cmd(cmd);
    }

}


/* 向指定地址写入 1Byte SEG 数据 */
void TM1638_Write_SegData(rt_uint8_t addr, rt_uint8_t seg_data)
{
    TM1638_STB_H(); TM1638_Delay_us(5);
    TM1638_STB_L(); TM1638_Delay_us(5);
    TM1638_Write_Byte(0x44);      // 固定地址写

    TM1638_STB_H(); TM1638_Delay_us(5);
    TM1638_STB_L(); TM1638_Delay_us(5);
    TM1638_Write_Byte(0xC0 | addr); // 地址命令
    TM1638_Write_Byte(seg_data);    // SEG 数据
    TM1638_STB_H();
}



/*==================== 全局缓存与刷新逻辑 ====================*/
static uint8_t seg_cache[16] = {0};   // 缓存 16 个地址的 SEG 值

/* 刷新整块屏幕（可选：初始化或需要全刷时调用） */
static void tm1638_flush_all(void)
{
    for(int i = 0; i < 8; i++)        // 一次写 8 地址，最多 16
    {
        TM1638_STB_H(); TM1638_Delay_us(5);
        TM1638_STB_L(); TM1638_Delay_us(5);
        TM1638_Write_Byte(0x44);      // 固定地址

        TM1638_STB_H(); TM1638_Delay_us(5);
        TM1638_STB_L(); TM1638_Delay_us(5);
        TM1638_Write_Byte(0xC0 | (i*2));      // 起始地址
        TM1638_Write_Byte(seg_cache[i*2]);    // 00H
        TM1638_Write_Byte(seg_cache[i*2+1]);  // 01H
        TM1638_STB_H();
    }
}


/* 根据 cnt（0~20）点亮 GRID1/2 的 LED（压力条） */
void TM1638_Set_PressBar(uint8_t cnt)
{
    if(cnt > 20) cnt = 20;
    /* 先把 GRID1/2 对应缓存清零 */
    seg_cache[GRID_1L_ADDR] = 0;
    seg_cache[GRID_1H_ADDR] = 0;
    seg_cache[GRID_2L_ADDR] = 0;
    seg_cache[GRID_2H_ADDR] = 0;

    for(int i = 0; i < cnt; i++)
    {
        if(i < 10)              // GRID1  SEG1~10
        {
            if(i < 8) seg_cache[GRID_1L_ADDR] |= (1 << i);
            else      seg_cache[GRID_1H_ADDR] |= (1 << (i-8));
        }
        else                    // GRID2  SEG1~10
        {
            int j = i - 10;
            if(j < 8) seg_cache[GRID_2L_ADDR] |= (1 << j);
            else      seg_cache[GRID_2H_ADDR] |= (1 << (j-8));
        }
    }
    /* 写回芯片 */
    TM1638_Write_SegData(GRID_1L_ADDR, seg_cache[GRID_1L_ADDR]);
    TM1638_Write_SegData(GRID_1H_ADDR, seg_cache[GRID_1H_ADDR]);
    TM1638_Write_SegData(GRID_2L_ADDR, seg_cache[GRID_2L_ADDR]);
    TM1638_Write_SegData(GRID_2H_ADDR, seg_cache[GRID_2H_ADDR]);
}



/* 根据 cnt（0~20）点亮 GRID3/4 的 LED（吹气条） */
void TM1638_Set_BlowBar(uint8_t cnt)
{
    if(cnt > 20) cnt = 20;
    seg_cache[GRID_3L_ADDR] = 0;
    seg_cache[GRID_3H_ADDR] = 0;
    seg_cache[GRID_4L_ADDR] = 0;
    seg_cache[GRID_4H_ADDR] = 0;

    for(int i = 0; i < cnt; i++)
    {
        if(i < 10)              // GRID3
        {
            if(i < 8) seg_cache[GRID_3L_ADDR] |= (1 << i);
            else      seg_cache[GRID_3H_ADDR] |= (1 << (i-8));
        }
        else                    // GRID4
        {
            int j = i - 10;
            if(j < 8) seg_cache[GRID_4L_ADDR] |= (1 << j);
            else      seg_cache[GRID_4H_ADDR] |= (1 << (j-8));
        }
    }
    TM1638_Write_SegData(GRID_3L_ADDR, seg_cache[GRID_3L_ADDR]);
    TM1638_Write_SegData(GRID_3H_ADDR, seg_cache[GRID_3H_ADDR]);
    TM1638_Write_SegData(GRID_4L_ADDR, seg_cache[GRID_4L_ADDR]);
    TM1638_Write_SegData(GRID_4H_ADDR, seg_cache[GRID_4H_ADDR]);
}




/*==================== 线程 + 采样接口 ====================*/
/* 用两个全局变量模拟实时 ADC 换算后的灯数，0~20 */
static uint8_t g_press_cnt = 10;
static uint8_t g_blow_cnt  = 10;

/* 用户把下面两个函数替换成真实 ADC 换算 */
uint8_t get_pressure_cnt(void)
{
    /* 示例：返回 0~20 */
    return g_press_cnt;
}
uint8_t get_blow_cnt(void)
{
    return g_blow_cnt;
}


/**
  * @brief  This thread entry is used for LightBar scanning
  * @retval void
  */
void LightBar_Thread_entry(void* parameter)
{
    // 设置固定地址-普通模式
    TM1638_Set_Cmd(0x4C);
    // 设置显示开，亮度最高
    TM1638_Set_Brightness(ON, 7);
    /* 上电先全灭 */
    for(int i = 0; i < 16; i++) {
        seg_cache[i] = 0;
    }
    tm1638_flush_all();

    for(;;)
    {
        g_blow_cnt++;
        g_press_cnt++;
        if(g_press_cnt > 20){
            g_press_cnt = 0;
        }
        if(g_blow_cnt > 20){
            g_blow_cnt = 0;
        }

        /* 1. 采样压力 & 吹气 */
        uint8_t press = get_pressure_cnt();
        uint8_t blow  = get_blow_cnt();

        /* 2. 刷新灯条 */
        TM1638_Set_PressBar(press);
        TM1638_Set_BlowBar(blow);

        rt_thread_mdelay(50);   // 20 Hz 刷新已足够
    }

}



/**
  * @brief  This is a Initialization for water level Check
  * @retval int
  */
int LightBar_Thread_Init(void)
{
    rt_thread_t LightBar_Task_Handle = RT_NULL;
    /* 创建检查一些系统状态标志的线程  -- 优先级：11 */
    LightBar_Task_Handle = rt_thread_create("LightBar_Thread_entry", LightBar_Thread_entry, RT_NULL, 1024, 10, 60);
    /* 检查是否创建成功,成功就启动线程 */
    if(LightBar_Task_Handle != RT_NULL)
    {
        rt_kprintf("PRINTF:%d. LightBar_Thread_entry is Succeed!! \r\n",Record.kprintf_cnt++);
        rt_thread_startup(LightBar_Task_Handle);
    }
    else {
        rt_kprintf("PRINTF:%d. LightBar_Thread_entry is Failed \r\n",Record.kprintf_cnt++);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(LightBar_Thread_Init);










