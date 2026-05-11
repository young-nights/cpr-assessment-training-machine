/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-11-07     18452       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_RD_DM32_PRINTER_H_
#define APPLICATIONS_MACBSP_INC_RD_DM32_PRINTER_H_

#include "bsp_sys.h"


/* 引脚 -- CTS */
#define     Printer_CTS_PORT     PRINTER_CTS_GPIO_Port
#define     Printer_CTS_PIN      PRINTER_CTS_Pin

#define     Printer_CTS_Read()   HAL_GPIO_ReadPin(Printer_CTS_PORT, Printer_CTS_PIN)

/**
 * @brief 清除打印缓冲区中的数据，复位打印参数到当前打印机缺省参数
 * @param ASCII -> ESC @
 * @param HEX   -> 1B  40
 * @note  打印机接收缓冲区的数据并不被清除
 */
void rd_printer_reset_cmd(void);



/**
 * @brief 将打印缓冲区中的数据全部打印出来并返回标准模式
 * @param ASCII -> FF
 * @param HEX   -> 0C
 * @note  如果打印纸有预印刷黑标，则打印缓冲区中的数据后，走纸到黑标处，如果打印纸无黑标，则走纸30cm后停止
 */
void rd_printer_print_to_next_page_cmd(void);



/**
 * @brief 打印并换行
 * @param ASCII -> LF
 * @param HEX   -> 0A
 * @note
 */
void rd_printer_print_and_line_feed_cmd(void);



/**
 * @brief 打印并回车
 * @param ASCII -> CR
 * @param HEX   -> 0D
 * @note
 */
void rd_printer_print_and_enter_cmd(void);



/**
 * @brief 打印输出打印缓冲区中的数据，并进纸[n ⅹ 0.125mm(0.0049”)]
 * @param ASCII -> ESC J   n （0 ≤ n ≤ 255）
 * @param HEX   -> 1B  4A  n （0 ≤ n ≤ 255）
 * @note  打印结束后，将下一行的开始设定为打印起始位置
 *
 */
void rd_printer_print_and_formfeed_1_cmd(rt_uint8_t n);



/**
 * @brief 打印输出打印缓冲区中的数据，并进纸n行
 * @param ASCII -> ESC d   n （0 ≤ n ≤ 255）
 * @param HEX   -> 1B  64  n （0 ≤ n ≤ 255）
 * @note  一行的距离为24个垂直点距(0.125mm)
 *        打印结束后，该命令设置打印起始位置为行起点
 *
 */
void rd_printer_print_and_formfeed_2_cmd(rt_uint8_t n);



/**
 * @brief 允许/禁止反向打印
 * @param ASCII -> ESC c   n （0 ≤ n ≤ 1）
 * @param HEX   -> 1B  63  n （0 ≤ n ≤ 1）
 * @param 当n=1 时，允许反向打印，打印方向由左向右
 * @param 当n=0 时，禁止反向打印，打印方向由右向左
 * @note  通常在打印机垂直安装时， 会使用反向打印方式。 反向打印不但支持字符方式， 也支持图形方式。在反向打印图形时，请注意图形单元的打印顺序。
 */
void rd_printer_reverse_print_control_cmd(rt_uint8_t n);



/**
 * @brief 移动打印位置到下一个水平制表位置
 * @param ASCII -> HT
 * @param HEX   -> 09
 * @note  通过ESC D命令设置水平制表位的位置；如果没有设置下一个水平制表位置，则该命令被忽略；
 */
void rd_printer_move_to_horizontal_table_cmd(void);



/**
 * @brief 设置水平定位位置
 * @param ASCII -> ESC D   [n1 n2 ... nk] NULL
 * @param HEX   -> 1B  44  [n1 n2 ... nk] 00
 * @note  n 指定从一行开始的列号，用来设置水平定位位置
 *        k 表示将被设置水平定位点的总数
 *        ·水平制表位置作为一个值储存，这个值为n个西文字符宽度，是从行的开始测量的。字符宽度包括字符间距的缺省字符宽
 *        ·该命令不受字符放大命令(ESC X)的影响
 *        ·该命令删除了之前设定的水平定位位置
 *        ·字符打印位置超过定位位置将被处理为普通数据
 *        ·按升序传输[n]k， 并且在末尾放置一个NULL码0
 *        ·该命令中nk>n(k-1),如果nk小于或等于前面的值n(k-1)， 定位设定结束并且n(k-1)后面的数据按普通数据处理
 *        ·ESC D NULL 取消所有水平定位位置
 *        ·即使字符宽度变化，以前指定的水平定位位置也不变
 *        ·缺省定位位置为字体A (12 × 24)
 */
rt_size_t rd_printer_set_horizontal_tab_cmd(const rt_uint8_t *positions, rt_uint8_t count);



/**
 * @brief 取消/设置下划线
 * @param ASCII -> ESC -   n
 * @param HEX   -> 1B  2D  n
 * @note  n = 1,允许下划线打印；n=0，取消下划线打印
 *        ·反白及旋转字符此命令无效
 *        ·该命令仅对英文和汉字字符有效
 */
void rd_printer_set_underline_cmd(rt_uint8_t n);




/**
 * @brief 取消/设置反白打印
 * @param ASCII -> GS  B   n
 * @param HEX   -> 1D  42  n
 * @note  当n 的最低有效位为0时，取消反白模式
 *        当n 的最低有效位为1时，设置反白模式
 *        ·仅n 的最低位有效
 *        ·该命令对内置字符和用户自定义字符均有效
 *        ·该命令仅对英文和汉字字符有效
 */
void rd_printer_set_highlight_cmd(rt_uint8_t n);



/**
 * @brief 设置字符旋转打印
 * @param ASCII -> FS  2   n（0 ≤ n ≤ 3）
 * @param HEX   -> 1C  49  n（0 ≤ n ≤ 3）
 * @note  在90°或270°旋转模式下，字符放大命令的宽高放大方向与一般模式下的放大方向相反
 *        · n = 0，不旋转
 *        · n = 1，设置90°逆时针旋转
 *        · n = 2，设置180°逆时针旋转
 *        · n = 3，设置270°逆时针旋转
 */
void rd_printer_set_char_rotate_cmd(rt_uint8_t n);




/**
 * @brief 设置绝对打印位置
 * @param ASCII -> ESC  $   nL  nH（0 ≤ nL + (nH x 256) < 384）
 * @param HEX   -> 1B   24  nL  nH（0 ≤ nL + (nH x 256) < 384）
 * @note  设定从一行的开始到将要打印字符的位置之间的距离
 *        从一行的开始到打印位置的距离为N个水平点距
 *        nL nH是双字节无符号整数N的低位和高位， N = nL + nH *  ٛ 256
 *        如果设定的打印位置超出了可打印区域（ N>384），命令将被忽略
 */
void rd_printer_set_absolute_position_cmd(rt_uint16_t n);



/**
 * @brief 设置左侧不打印区域
 * @param ASCII -> ESC  l   n（0 ≤ n ≤ 32）
 * @param HEX   -> 1B   6C  n（0 ≤ n ≤ 32）
 * @note  从一行的开始到打印位置的距离为n个西文字符宽度
 *        ·如果设定的打印位置超出了可打印区域，命令将被忽略
 *        ·字符宽度包括字符间距的缺省字符宽ٛ
 */
void rd_printer_set_left_cannot_print_area_cmd(rt_uint8_t n);



/**
 * @brief 设置右侧不打印区域
 * @param ASCII -> ESC  Q   n（0 ≤ n ≤ 32）
 * @param HEX   -> 1B   51  n（0 ≤ n ≤ 32）
 * @note  从一行的开始到打印位置的距离为n个西文字符宽度
 *        ·如果设定的打印位置超出了可打印区域，命令将被忽略
 *        ·字符宽度包括字符间距的缺省字符宽ٛ
 */
void rd_printer_set_right_cannot_print_area_cmd(rt_uint8_t n);


/**
 * @brief 设置行间距
 * @param ASCII -> ESC  1   n（0 ≤ n ≤ 255）
 * @param HEX   -> 1B   31  n（0 ≤ n ≤ 255）
 * @note  设置字符行间距为n个垂直点距
 *        ·缺省值：3ٛ
 */
void rd_printer_set_line_space_cmd(rt_uint8_t n);


/**
 * @brief 设置字间距
 * @param ASCII -> ESC  SP   n（0 ≤ n ≤ 255）
 * @param HEX   -> 1B   20   n（0 ≤ n ≤ 255）
 * @note  设置字符字间距为n个水平点距
 *        ·缺省值：0
 */
void rd_printer_set_word_space_cmd(rt_uint8_t n);



/**
 * @brief 选择对齐方式
 * @param ASCII -> ESC  a    n（0 ≤ n ≤ 2）
 * @param HEX   -> 1B   61   n（0 ≤ n ≤ 2）
 * @note  设置字符字间距为n个水平点距
 *        ·缺省值：0
 *        ·n = 0，左对齐
 *        ·n = 1，居中
 *        ·n = 2，右对齐
 */
void rd_printer_set_alignment_cmd(rt_uint8_t n);



/**
 * @brief 选择对齐方式
 * @param ASCII -> FS   r    n（0 ≤ n ≤ 1）
 * @param HEX   -> 1C   72   n（0 ≤ n ≤ 1）
 * @note  n =0 选择上标，一行字符图形顶部对齐
 *        n =1 选择下标，一行字符图形低部对齐
 *        ·该命令对所有字符(英数字符和汉字) 有效
 *        ·如果n 在定义范围之外，忽略该命令
 */
void rd_printer_set_superscript_or_subscript_cmd(rt_uint8_t n);




/**
 * @brief 水平放大字符
 * @param ASCII -> ESC  U    n（0 ≤ n ≤ 8）
 * @param HEX   -> 1B   55   n（0 ≤ n ≤ 8）
 * @note  该命令对所有字符(英数字符和汉字) 有效
 *        n =1 选择下标，一行字符图形低部对齐
 *        ·如果n 在定义范围之外，忽略该命令
 */
void rd_printer_set_horizontal_exaggeration_char_cmd(rt_uint8_t n);



/**
 * @brief 垂直放大字符
 * @param ASCII -> ESC  V   n（0 ≤ n ≤ 8）
 * @param HEX   -> 1B   56   n（0 ≤ n ≤ 8）
 * @note  该命令对所有字符(英数字符和汉字) 有效
 *        n =1 选择下标，一行字符图形低部对齐
 *        ·如果n 在定义范围之外，忽略该命令
 */
void rd_printer_set_vertical_amplification_char_cmd(rt_uint8_t n);


/**
 * @brief 放大字符
 * @param ASCII -> ESC  X    n1  n2（0 ≤ n1,n2 ≤ 8）
 * @param HEX   -> 1B   58   n1  n2（0 ≤ n1,n2 ≤ 8）
 * @note  ·该命令对除条码识读字符外的所有字符(英数字符和汉字) 有效
 *        ·如果n 在定义范围之外，忽略该命令
 *        ·垂直方向是指进纸方向，水平方向与进纸方向垂直。然而，当字符方向顺时针旋转90°后，垂直方向与水平方向之间的关系颠倒，也就是说本命令优先级低于于FS 2 ，当两个命令同时有效时，字符显示是先旋转，再放大
 */
void rd_printer_set_enlarged_characters_cmd(rt_uint8_t n1, rt_uint8_t n2);


/**
 * @brief 进入汉字模式
 * @param ASCII -> FS  &
 * @param HEX   -> 1C  26
 * @note  上电后打印机默认为汉字打印模式
 */
void rd_printer_enter_chinese_mode_cmd(void);



/**
 * @brief 退出汉字模式
 * @param ASCII -> FS  .
 * @param HEX   -> 1C  2E
 * @note  取消汉字字符模式，当取消汉字字符模式后，超过0x80的编码仍然当作ASCII字符处理，将不再打印汉字，除非再用FS &命令选择汉字模式。 打印机进入汉字打印模式
 */
void rd_printer_quit_chinese_mode_cmd(void);



/**
 * @brief 选择6X8字符集1
 * @param ASCII -> ESC  6
 * @param HEX   -> 1B   36
 * @note  在该命令输入之后的所有字符均使用字符集1中的字符打印(见附录D)字符集1中有6× 8点阵字符224个，包括ASCII字符及各种图形符号等。代码范围20H~FFH(32~255)
 */
void rd_printer_select_6X8_character_1(void);



/**
 * @brief 选择6X8字符集2
 * @param ASCII -> ESC  7
 * @param HEX   -> 1B   37
 * @note  在该命令输入之后的所有字符均使用字符集1中的字符打印(见附录D)字符集1中有6× 8点阵字符224个，包括ASCII字符及各种图形符号等。代码范围20H~FFH(32~255)
 */
void rd_printer_select_6X8_character_2(void);






#endif /* APPLICATIONS_MACBSP_INC_RD_DM32_PRINTER_H_ */
