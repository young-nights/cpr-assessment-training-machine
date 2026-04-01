/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-07-17     18452       the first version
 */
#include "bsp_sys.h"

#define NIXIETUBE_PRINTF_DEBUG  0
//---------------------------------------------------------------------------------------
#define TM1629A_A_CLK_H()      HAL_GPIO_WritePin(TM1629A_A_CLK_GPIO_Port, TM1629A_A_CLK_Pin, GPIO_PIN_SET)
#define TM1629A_A_CLK_L()      HAL_GPIO_WritePin(TM1629A_A_CLK_GPIO_Port, TM1629A_A_CLK_Pin, GPIO_PIN_RESET)
#define TM1629A_A_DIO_H()      HAL_GPIO_WritePin(TM1629A_A_DIO_GPIO_Port, TM1629A_A_DIO_Pin, GPIO_PIN_SET)
#define TM1629A_A_DIO_L()      HAL_GPIO_WritePin(TM1629A_A_DIO_GPIO_Port, TM1629A_A_DIO_Pin, GPIO_PIN_RESET)
#define TM1629A_A_STB_H()      HAL_GPIO_WritePin(TM1629A_A_STB_GPIO_Port, TM1629A_A_STB_Pin,  GPIO_PIN_SET)
#define TM1629A_A_STB_L()      HAL_GPIO_WritePin(TM1629A_A_STB_GPIO_Port, TM1629A_A_STB_Pin,  GPIO_PIN_RESET)
//---------------------------------------------------------------------------------------
#define TM1629A_B_CLK_H()      HAL_GPIO_WritePin(TM1629A_B_CLK_GPIO_Port, TM1629A_B_CLK_Pin, GPIO_PIN_SET)
#define TM1629A_B_CLK_L()      HAL_GPIO_WritePin(TM1629A_B_CLK_GPIO_Port, TM1629A_B_CLK_Pin, GPIO_PIN_RESET)
#define TM1629A_B_DIO_H()      HAL_GPIO_WritePin(TM1629A_B_DIO_GPIO_Port, TM1629A_B_DIO_Pin, GPIO_PIN_SET)
#define TM1629A_B_DIO_L()      HAL_GPIO_WritePin(TM1629A_B_DIO_GPIO_Port, TM1629A_B_DIO_Pin, GPIO_PIN_RESET)
#define TM1629A_B_STB_H()      HAL_GPIO_WritePin(TM1629A_B_STB_GPIO_Port, TM1629A_B_STB_Pin,  GPIO_PIN_SET)
#define TM1629A_B_STB_L()      HAL_GPIO_WritePin(TM1629A_B_STB_GPIO_Port, TM1629A_B_STB_Pin,  GPIO_PIN_RESET)



// 位选地址（显示数据）---------------------------------------------------------------------------------------
// 说明一下：把数码管的位选引脚接入到芯片的SEG1~SEG16引脚上，使实现多个数码管的控制
#define SEG_1_ADDR      0x01
#define SEG_2_ADDR      0x02
#define SEG_3_ADDR      0x04
#define SEG_4_ADDR      0x08
#define SEG_5_ADDR      0x10
#define SEG_6_ADDR      0x20
#define SEG_7_ADDR      0x40
#define SEG_8_ADDR      0x80
#define SEG_9_ADDR      0x01
#define SEG_10_ADDR     0x02
#define SEG_11_ADDR     0x04
#define SEG_12_ADDR     0x08
#define SEG_13_ADDR     0x10
#define SEG_14_ADDR     0x20
#define SEG_15_ADDR     0x40
#define SEG_16_ADDR     0x80


//---------------------------------------------------------------------------------------

const rt_uint8_t seg_code[10] = {
        0x3F,   /*! 0 */
        0x30,   /*! 1 */
        0x5B,   /*! 2 */
        0x4F,   /*! 3 */
        0x66,   /*! 4 */
        0x6D,   /*! 5 */
        0x7D,   /*! 6 */
        0x07,   /*! 7 */
        0x7F,   /*! 8 */
        0x6F    /*! 9 */
};


const rt_uint8_t seg_code_t[10]={
        0xFC,   /*! 0 */
        0x60,   /*! 1 */
        0xDA,   /*! 2 */
        0xF2,   /*! 3 */
        0x66,   /*! 4 */
        0xB6,   /*! 5 */
        0xBE,   /*! 6 */
        0xE0,   /*! 7 */
        0xFE,   /*! 8 */
        0xF6    /*! 9 */
};

//---------------------------------------------------------------------------------------

void TM1629A_Delay_us(uint32_t us);
void TM1629A_Write_Byte(TM16xxSelect chip , rt_uint8_t data);
void TM1629A_Digital_Tube_Set_Brightness(TM16xxSelect chip, rt_uint8_t level);
void TM1629A_Set_Cmd(TM16xxSelect chip, rt_uint8_t cmd);
void TM1629A_Write_Data(TM16xxSelect chip, rt_uint8_t drig_addr, TM1629x_SEG_SELECT seg_addr);
void TM1629A_Display_Digit(TM16xxSelect chip, TM1629x_SEG_SELECT digit_pos ,rt_uint8_t number);

/**
  * @brief  Delay function(uint:us)
  * @param  us 微妙数
  * @retval void
  */
void TM1629A_Delay_us(uint32_t us)
{
    for(; us; us--) __NOP();
}


/**
  * @brief  TM1629A Chip send one byte and select tube to write
  * @param  chip 选择指定芯片
  *         data 需要写入的数据
  * @retval void
  * @note   这个工程板载了2个TM1629A芯片，因此这个写单个字节函数通过选型进行公用
  */
/* 发送 1 字节（低位先行） */
void TM1629A_Write_Byte(TM16xxSelect chip , rt_uint8_t data)
{
    if(chip == TM1629A_A)
    {
        for(rt_uint8_t i = 0; i < 8; i++)
        {
            TM1629A_A_CLK_L();
            if(data & 0x01){
                TM1629A_A_DIO_H();
            }
            else {
                TM1629A_A_DIO_L();
            }
            TM1629A_Delay_us(5);
            TM1629A_A_CLK_H();
            TM1629A_Delay_us(5);
            data >>= 1;
        }
    }
    else if(chip == TM1629A_B)
    {
        for(rt_uint8_t i = 0; i < 8; i++)
        {
            TM1629A_B_CLK_L();
            if(data & 0x01){
                TM1629A_B_DIO_H();
            }
            else {
                TM1629A_B_DIO_L();
            }
            TM1629A_Delay_us(5);
            TM1629A_B_CLK_H();
            TM1629A_Delay_us(5);
            data >>= 1;
        }
    }
}



/**
  * @brief  TM1629A set data command
  * @param  cmd  : 数据命令指令
  * @retval void
  * @note
  *         ---------------------------------------------------
  *          指令      |    功能               |   说明
  *         ---------------------------------------------------
  *          0x44   |   数据读写模式  |   写数据到显示寄存器
  *                 |   地址增加模式  |   固定地址
  *                 |   模式设置          |   普通模式
  *         ---------------------------------------------------
  *          0x40   |   数据读写模式  |   写数据到显示寄存器
  *                 |   地址增加模式  |   自动地址增加
  *                 |   模式设置          |   普通模式
  *         ---------------------------------------------------
  *          0x8F   |   显示控制指令  |   显示开，亮度等级7
  *         ---------------------------------------------------
  *          0x80   |   显示控制指令  |   显示关，亮度等级0
  */
void TM1629A_Set_Cmd(TM16xxSelect chip, rt_uint8_t cmd)
{
    if(chip == TM1629A_A)
    {
        TM1629A_A_STB_H();
        TM1629A_Delay_us(5);
        TM1629A_A_STB_L();
        TM1629A_Delay_us(5);
        TM1629A_Write_Byte(chip, cmd);
    }
    else if(chip == TM1629A_B)
    {
        TM1629A_B_STB_H();
        TM1629A_Delay_us(5);
        TM1629A_B_STB_L();
        TM1629A_Delay_us(5);
        TM1629A_Write_Byte(chip, cmd);
    }
}



/**
  * @brief  TM1629A Set digital tube Brightness
  * @param  level: 亮度等级0~7
  *         tube : 选择控制数码管的芯片
  * @retval void
  * @note   找到寄存器手册的显示控制命令设置（用于设置亮度强度和显示使能）
  */
void TM1629A_Digital_Tube_Set_Brightness(TM16xxSelect chip, rt_uint8_t level)
{
    #define DISPLAY_RES_BASE    0x88

    if(level > 7){
        level = 7;
    }

    rt_uint8_t cmd = DISPLAY_RES_BASE | level;

    TM1629A_Set_Cmd(chip, cmd);
}




/**
  * @brief  TM1629A Write display data
  * @param  drig_addr : 决定数码管哪个段显示
  *         seg_addr  : 决定数码管哪个位显示
  * @retval void
  * @note
  */
void TM1629A_Write_Data(TM16xxSelect chip, rt_uint8_t drig_addr,  TM1629x_SEG_SELECT seg_addr)
{
    if(chip == TM1629A_A)
    {
        TM1629A_A_STB_H();
        TM1629A_Delay_us(5);
        TM1629A_A_STB_L();
        TM1629A_Delay_us(5);
        TM1629A_Write_Byte(chip, drig_addr); // 控制数码管段显示
        TM1629A_Write_Byte(chip, seg_addr);  // 控制数码管位显示
    }
    else if(chip == TM1629A_B)
    {
        TM1629A_B_STB_H();
        TM1629A_Delay_us(5);
        TM1629A_B_STB_L();
        TM1629A_Delay_us(5);
        TM1629A_Write_Byte(chip, drig_addr); // 控制数码管段显示
        TM1629A_Write_Byte(chip, seg_addr);  // 控制数码管位显示
    }
}





//---------------------------------------------------------------------------------------------------
/**
  * @brief  数字拼接，将四个三位数按顺序进行拼接，不满三位时，要在前面补零，一共要拼成16位数（最后四位补零）
  * @param  *number_buf : 拼接后进行存储的数组
  *         countdown   : 倒计时数
  *         press       ：按压数
  *         correct     : 正确计数
  *         error       : 错误计数
  * @retval void
  * @note   NULL
  */
void press_digital_splicing(char *number_buf, int countdown, int press, int correct, int error)
{
    /* 按格式一次性写入，最后自带 '\0' */
    sprintf((char*)number_buf, "%03d%03d%03d%03d0000",
            countdown,   /* 取后 3 位，防止溢出 */
            press,
            correct,
            error);
#if NIXIETUBE_PRINTF_DEBUG
    rt_kprintf("%s\r\n",number_buf);
#endif
}

//---------------------------------------------------------------------------------------------------
/**
  * @brief  数字拼接，将3个2位数和1个1位数按顺序进行拼接，不满2位时，要在前面补零，一共要拼成16位数
  * @param  *number_buf : 拼接后进行存储的数组
  *         countdown   : 时间数
  *         press       ：频率数
  *         correct     : 正确计数
  *         error       : 错误计数
  * @retval void
  * @note   NULL
  */
void blow_digital_splicing(char *number_buf, int countdown, int press, int correct, int error)
{
    /* 按格式一次性写入，最后自带 '\0' */
    sprintf((char*)number_buf, "%02d%02d%02d%01d000000000",
            countdown,   /* 取后 3 位，防止溢出 */
            press,
            correct,
            error);
#if NIXIETUBE_PRINTF_DEBUG
    rt_kprintf("%s\r\n",number_buf);
#endif
}


/**
  * @brief  数字拆分，将传入的格式化后的字符串数字，拆分成单个数字进行存储
  * @param  *number_buf : 拼接后进行存储的数组
  *         *digit_buf  : 准备存储拆分后单个数字的数组
  * @retval void
  * @note   NULL
  */
void split_to_digits(char *number_buf, uint8_t digit_buf[16])
{
    /* 把传入的已经格式化后的拼接数组进行数字拆分 */
    for(rt_uint8_t i = 0; i < 16; i++)
    {
        digit_buf[i] = number_buf[i] - '0';
    }
    /* 打印调试（可选） */
#if NIXIETUBE_PRINTF_DEBUG
//#if 1
    for (uint8_t i = 0; i < 16; i++)
    {
        rt_kprintf("%d ", digit_buf[i]);
    }
    rt_kprintf("\r\n");
#endif
}


/**
  * @brief  数字拆分，将传入的格式化后的字符串数字，拆分成单个数字进行存储
  * @param  *number_buf : 拼接后进行存储的数组
  *         *digit_buf  : 准备存储拆分后单个数字的数组
  * @retval void
  * @note   NULL
  */
/* 行数固定为 8，列数固定为 16 */
#define ROWS 8
#define COLS 16
void build_digit_matrix(uint8_t digit_buf[COLS], uint8_t matrix[ROWS * COLS])
{
    /* 逐列处理 */
    for (uint8_t col = 0; col < COLS; ++col)
    {
        /* 查表 */
        uint8_t val = seg_code_t[digit_buf[col]];
        /* 逐行展开：高位(bit7) → 低位(bit0) */
        for (uint8_t row = 0; row < ROWS; ++row)
        {
            /* 取出第 (7 - row) 位 */
            uint8_t bit = (val >> (7 - row)) & 0x01;
            matrix[row * COLS + col] = bit;
        }
    }

#if NIXIETUBE_PRINTF_DEBUG
//#if 1
    /* 打印矩阵，方便验证 */
    rt_kprintf("-----------------------------------------------------\r\n");
    for (uint8_t r = 0; r < ROWS; ++r)
    {
        for (uint8_t c = 0; c < COLS; ++c)
        {
            printf("%u ", matrix[r * COLS + c]);
        }
        printf("\n");
    }
#endif

}



/**
  * @brief  将拆分得到数组重新进行拼接
  * @param  matrix : 8*16的二进制数组
  *         hex_buf: 转换成16进制字节
  * @retval void
  * @note   bit0 bit1 ... bit7  → byte0 (bit0是LSB, bit7是MSB)
  *         bit8 bit9 ... bit15 → byte1 (bit8是LSB, bit15是MSB)
  */
void matrix_to_hex(const uint8_t matrix[ROWS * COLS], uint8_t hex_buf[ROWS * 2])
{
    for (uint8_t row = 0; row < ROWS; ++row)
    {
        uint8_t byte0 = 0; // bit0~bit7 → byte0
        uint8_t byte1 = 0; // bit8~bit15 → byte1

        for (uint8_t col = 0; col < 8; ++col)
        {
            byte0 |= (matrix[row * COLS + col] << col);
        }

        for (uint8_t col = 8; col < 16; ++col)
        {
            byte1 |= (matrix[row * COLS + col] << (col - 8));
        }

        hex_buf[row * 2 + 0] = byte0;
        hex_buf[row * 2 + 1] = byte1;
    }


#if NIXIETUBE_PRINTF_DEBUG
//#if 1
    /* 打印数据，方便验证 */
    printf("Hex output (row-wise, 2 bytes per row):\n");
    for (uint8_t i = 0; i < 8; ++i)
    {
        printf("Row%d: 0x%02X 0x%02X\n", i, hex_buf[i * 2], hex_buf[i * 2 + 1]);
    }
#endif
}



/**
  * @brief  设置自动地址模式，把数字转换得到的16进制数，依次传入显示地址，进行数码管显示
  * @param
  *
  * @retval void
  * @note
  */
void TM1629A_Show_Number(TM16xxSelect chip, uint8_t *hex_buf)
{
    for(rt_uint8_t i = 0; i < 16; i++){
        TM1629A_Set_Cmd(chip, 0xC0 + i);
        TM1629A_Write_Byte(chip, hex_buf[i]);
    }
}






/**
  * @brief  This thread entry is used for NixieTube scanning
  * @retval void
  */
void NixieTube_Thread_entry(void* parameter)
{
    // 设置固定地址
    TM1629A_Set_Cmd(TM1629A_A, 0x44);
    // 设置显示开，亮度最高
    TM1629A_Set_Cmd(TM1629A_A, 0x8F);

    // 设置固定地址
    TM1629A_Set_Cmd(TM1629A_B, 0x44);
    // 设置显示开，亮度最高
    TM1629A_Set_Cmd(TM1629A_B, 0x8F);


    /* 用于存储拼接后得到的数字字符串数组 */
    char press_string_buf[16] = {0};
    /* 用于存储拆分格式化后数字字符串的单个数字的数组 */
    rt_uint8_t press_single_digit_buf[16] = {0};
    /* 用于存储将单个数字按列排序，按行进行二进制拆分后得到的8*16矩阵 */
    uint8_t press_matrix[ROWS * COLS];
    /* 用于存储将矩阵数据转换为16进制数的数组 */
    uint8_t press_matrix_to_hex_buf[8*2];

    /* 用于存储拼接后得到的数字字符串数组 */
    char blow_string_buf[16] = {0};
    /* 用于存储拆分格式化后数字字符串的单个数字的数组 */
    rt_uint8_t blow_single_digit_buf[16] = {0};
    /* 用于存储将单个数字按列排序，按行进行二进制拆分后得到的8*16矩阵 */
    uint8_t blow_matrix[ROWS * COLS];
    /* 用于存储将矩阵数据转换为16进制数的数组 */
    uint8_t blow_matrix_to_hex_buf[8*2];
    for(;;)
    {
        //------------------------------------------------------------------------------------------------
        press_digital_splicing(press_string_buf,MySysCfg.params[MySysCfg.current_mode].Number_CountDown,
                                                MySysCfg.params[MySysCfg.current_mode].Number_Press_Frequency,
                                                MySysCfg.params[MySysCfg.current_mode].Number_Press_Correct,
                                                MySysCfg.params[MySysCfg.current_mode].Number_Press_Error);
        split_to_digits(press_string_buf,press_single_digit_buf);
        build_digit_matrix(press_single_digit_buf,press_matrix);
        matrix_to_hex(press_matrix,press_matrix_to_hex_buf);
        TM1629A_Show_Number(TM1629A_A,press_matrix_to_hex_buf);

        blow_digital_splicing(blow_string_buf,MySysCfg.params[MySysCfg.current_mode].Number_Blow_Time,
                                              MySysCfg.params[MySysCfg.current_mode].Number_Blow_Correct,
                                              MySysCfg.params[MySysCfg.current_mode].Number_Blow_Error,
                                              MySysCfg.params[MySysCfg.current_mode].Number_Cycle);
        split_to_digits(blow_string_buf,blow_single_digit_buf);
        build_digit_matrix(blow_single_digit_buf,blow_matrix);
        matrix_to_hex(blow_matrix,blow_matrix_to_hex_buf);
        TM1629A_Show_Number(TM1629A_B,blow_matrix_to_hex_buf);
        rt_thread_mdelay(2);
    }

}



/**
  * @brief  This is a Initialization for water level Check
  * @retval int
  */
int NixieTube_Thread_Init(void)
{
    rt_thread_t NixieTube_Task_Handle = RT_NULL;
    /* 创建检查一些系统状态标志的线程  -- 优先级：11 */
    NixieTube_Task_Handle = rt_thread_create("NixieTube_Thread_entry", NixieTube_Thread_entry, RT_NULL, 2048, 10, 30);
    /* 检查是否创建成功,成功就启动线程 */
    if(NixieTube_Task_Handle != RT_NULL)
    {
        rt_kprintf("PRINTF:%d. NixieTube_Thread_entry is Succeed!! \r\n",Record.kprintf_cnt++);
        rt_thread_startup(NixieTube_Task_Handle);
    }
    else {
        rt_kprintf("PRINTF:%d. NixieTube_Thread_entry is Failed \r\n",Record.kprintf_cnt++);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(NixieTube_Thread_Init);





