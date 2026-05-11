/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-11-03     Administrator       the first version
 */
#include "bsp_sys.h"




/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-11-03     Administrator       the first version
 */
#include "setup_scr_screen.h"




void setup_scr_screen_setting(lvgl_ui_t *ui)
{
    // ==================== 创建屏幕（screen_setting） ====================
    // 创建一个新屏幕对象，作为 LVGL 的顶级容器（NULL 表示父对象为屏幕）
    ui->screen_setting = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_setting, 240, 320);                    // 设置屏幕尺寸 240x320（常见 TFT 屏幕分辨率）
    lv_obj_set_scrollbar_mode(ui->screen_setting, LV_SCROLLBAR_MODE_OFF); // 关闭滚动条

    // 屏幕主容器样式：透明背景（便于叠加其他层）
    lv_obj_set_style_bg_opa(ui->screen_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    // ==================== 创建主容器 cont_setting ====================
    // 主内容容器，覆盖整个屏幕，用于放置所有控件
    ui->screen_setting_cont_setting = lv_obj_create(ui->screen_setting);
    lv_obj_set_pos(ui->screen_setting_cont_setting, 0, 0);          // 位置 (0,0)
    lv_obj_set_size(ui->screen_setting_cont_setting, 240, 320);     // 大小满屏
    lv_obj_set_scrollbar_mode(ui->screen_setting_cont_setting, LV_SCROLLBAR_MODE_OFF);

    // 主容器样式：白色背景，无边框，无阴影，全填充
    lv_obj_set_style_border_width(ui->screen_setting_cont_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_cont_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_setting_cont_setting, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_setting_cont_setting, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_setting_cont_setting, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_setting_cont_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_setting_cont_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_setting_cont_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_setting_cont_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_cont_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    // ==================== 创建 “+” 按钮 ====================
    // 用于数值增加的按钮，初始隐藏
    ui->screen_setting_btn_plus = lv_btn_create(ui->screen_setting_cont_setting);
    ui->screen_setting_btn_plus_label = lv_label_create(ui->screen_setting_btn_plus);
    lv_label_set_text(ui->screen_setting_btn_plus_label, "+");
    lv_label_set_long_mode(ui->screen_setting_btn_plus_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_setting_btn_plus_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_setting_btn_plus, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_setting_btn_plus_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_setting_btn_plus, 188, 249);
    lv_obj_set_size(ui->screen_setting_btn_plus, 45, 45);
    lv_obj_add_flag(ui->screen_setting_btn_plus, LV_OBJ_FLAG_HIDDEN);

    // “+” 按钮默认样式：透明背景、蓝色边框、白色文字
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_plus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_setting_btn_plus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_btn_plus, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_plus, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->screen_setting_btn_plus, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->screen_setting_btn_plus, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->screen_setting_btn_plus, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(ui->screen_setting_btn_plus, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(ui->screen_setting_btn_plus, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_btn_plus, lv_color_hex(0x191717), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_btn_plus, &lv_font_montserratMedium_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_plus, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_btn_plus, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);


    // ==================== 创建 “-” 按钮 ====================
    // 用于数值减少的按钮，初始隐藏
    ui->screen_setting_btn_minus = lv_btn_create(ui->screen_setting_cont_setting);
    ui->screen_setting_btn_minus_label = lv_label_create(ui->screen_setting_btn_minus);
    lv_label_set_text(ui->screen_setting_btn_minus_label, "-");
    lv_label_set_long_mode(ui->screen_setting_btn_minus_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_setting_btn_minus_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_setting_btn_minus, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_setting_btn_minus_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_setting_btn_minus, 10, 249);
    lv_obj_set_size(ui->screen_setting_btn_minus, 45, 45);
    lv_obj_add_flag(ui->screen_setting_btn_minus, LV_OBJ_FLAG_HIDDEN);

    // “-” 按钮默认样式：透明背景、蓝色边框、白色文字
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_minus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_setting_btn_minus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_btn_minus, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_minus, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->screen_setting_btn_minus, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->screen_setting_btn_minus, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->screen_setting_btn_minus, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(ui->screen_setting_btn_minus, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(ui->screen_setting_btn_minus, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_btn_minus, lv_color_hex(0x191717), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_btn_minus, &lv_font_montserratMedium_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_minus, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_btn_minus, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    // ==================== 创建 “潮气达标率” 按钮 ====================
    // 可切换（checkable）的按钮，用于选择设置项
    ui->screen_setting_btn_3_air_rate = lv_btn_create(ui->screen_setting_cont_setting);
    lv_obj_add_flag(ui->screen_setting_btn_3_air_rate, LV_OBJ_FLAG_CHECKABLE); // 可选中
    ui->screen_setting_btn_3_air_rate_label = lv_label_create(ui->screen_setting_btn_3_air_rate);
    lv_label_set_text(ui->screen_setting_btn_3_air_rate_label, "潮气达标率");
    lv_label_set_long_mode(ui->screen_setting_btn_3_air_rate_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_setting_btn_3_air_rate_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_setting_btn_3_air_rate, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_setting_btn_3_air_rate_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_setting_btn_3_air_rate, 55, 167);     // 位置
    lv_obj_set_size(ui->screen_setting_btn_3_air_rate, 130, 37);    // 大小

    // “潮气达标率” 按钮默认样式：青绿背景、白色文字、圆角、阴影
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_3_air_rate, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_setting_btn_3_air_rate, lv_color_hex(0x009ea9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_setting_btn_3_air_rate, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_setting_btn_3_air_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_btn_3_air_rate, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_3_air_rate, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->screen_setting_btn_3_air_rate, lv_color_hex(0x0d4b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->screen_setting_btn_3_air_rate, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->screen_setting_btn_3_air_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(ui->screen_setting_btn_3_air_rate, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(ui->screen_setting_btn_3_air_rate, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_btn_3_air_rate, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_btn_3_air_rate, &lv_font_AlimamaDongFangDaKai_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_3_air_rate, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_btn_3_air_rate, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    /* 按下（选中）状态：橙红色背景 + 白色文字 + 保持阴影 */
    lv_obj_set_style_bg_color(ui->screen_setting_btn_3_air_rate, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_3_air_rate, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_setting_btn_3_air_rate, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_color(ui->screen_setting_btn_3_air_rate, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);

    // ==================== 创建 “按压达标率” 按钮 ====================
    ui->screen_setting_btn_2_press_rate = lv_btn_create(ui->screen_setting_cont_setting);
    lv_obj_add_flag(ui->screen_setting_btn_2_press_rate, LV_OBJ_FLAG_CHECKABLE);
    ui->screen_setting_btn_2_press_rate_label = lv_label_create(ui->screen_setting_btn_2_press_rate);
    lv_label_set_text(ui->screen_setting_btn_2_press_rate_label, "按压达标率");
    lv_label_set_long_mode(ui->screen_setting_btn_2_press_rate_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_setting_btn_2_press_rate_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_setting_btn_2_press_rate, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_setting_btn_2_press_rate_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_setting_btn_2_press_rate, 55, 107);
    lv_obj_set_size(ui->screen_setting_btn_2_press_rate, 130, 37);

    // “按压达标率” 按钮默认样式：同上，青绿背景
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_2_press_rate, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_setting_btn_2_press_rate, lv_color_hex(0x009ea9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_setting_btn_2_press_rate, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_setting_btn_2_press_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_btn_2_press_rate, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_2_press_rate, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->screen_setting_btn_2_press_rate, lv_color_hex(0x0d4b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->screen_setting_btn_2_press_rate, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->screen_setting_btn_2_press_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(ui->screen_setting_btn_2_press_rate, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(ui->screen_setting_btn_2_press_rate, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_btn_2_press_rate, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_btn_2_press_rate, &lv_font_AlimamaDongFangDaKai_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_2_press_rate, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_btn_2_press_rate, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    /* 按下（选中）状态：橙红色背景 + 白色文字 + 保持阴影 */
    lv_obj_set_style_bg_color(ui->screen_setting_btn_2_press_rate, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_2_press_rate, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_setting_btn_2_press_rate, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_color(ui->screen_setting_btn_2_press_rate, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);


    // ==================== 创建 “时间设置” 按钮 ====================
    ui->screen_setting_btn_1_time_set = lv_btn_create(ui->screen_setting_cont_setting);
    lv_obj_add_flag(ui->screen_setting_btn_1_time_set, LV_OBJ_FLAG_CHECKABLE);
    ui->screen_setting_btn_1_time_set_label = lv_label_create(ui->screen_setting_btn_1_time_set);
    lv_label_set_text(ui->screen_setting_btn_1_time_set_label, "时间设置");
    lv_label_set_long_mode(ui->screen_setting_btn_1_time_set_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_setting_btn_1_time_set_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_setting_btn_1_time_set, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_setting_btn_1_time_set_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_setting_btn_1_time_set, 55, 47);
    lv_obj_set_size(ui->screen_setting_btn_1_time_set, 130, 37);

    // “时间设置” 按钮默认样式：同上，青绿背景
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_1_time_set, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_setting_btn_1_time_set, lv_color_hex(0x009ea9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_setting_btn_1_time_set, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_setting_btn_1_time_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_btn_1_time_set, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_1_time_set, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->screen_setting_btn_1_time_set, lv_color_hex(0x0d4b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->screen_setting_btn_1_time_set, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->screen_setting_btn_1_time_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(ui->screen_setting_btn_1_time_set, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(ui->screen_setting_btn_1_time_set, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_btn_1_time_set, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_btn_1_time_set, &lv_font_AlimamaDongFangDaKai_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_1_time_set, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_btn_1_time_set, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    /* 按下（选中）状态：橙红色背景 + 白色文字 + 保持阴影 */
    lv_obj_set_style_bg_color(ui->screen_setting_btn_1_time_set, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_1_time_set, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_setting_btn_1_time_set, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_color(ui->screen_setting_btn_1_time_set, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);

    // ==================== 创建 “潮气达标率” 值标签 ====================
    // 显示当前潮气达标率值，初始隐藏
    ui->screen_setting_label_air_rate = lv_label_create(ui->screen_setting_cont_setting);
    lv_label_set_text(ui->screen_setting_label_air_rate, "50%");
    lv_label_set_long_mode(ui->screen_setting_label_air_rate, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_setting_label_air_rate, 70, 244);
    lv_obj_set_size(ui->screen_setting_label_air_rate, 109, 43);
    lv_obj_add_flag(ui->screen_setting_label_air_rate, LV_OBJ_FLAG_HIDDEN);

    // 值标签样式：无背景、无边框、黑色文字、居中
    lv_obj_set_style_border_width(ui->screen_setting_label_air_rate, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_setting_label_air_rate, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_setting_label_air_rate, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_setting_label_air_rate, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_label_air_rate, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_label_air_rate, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_label_air_rate, &lv_font_SourceHanSerifSC_Regular_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_label_air_rate, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_setting_label_air_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_setting_label_air_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_label_air_rate, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_setting_label_air_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_setting_label_air_rate, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_setting_label_air_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_setting_label_air_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_setting_label_air_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_label_air_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);


    // ==================== 创建 “按压达标率” 值标签 ====================
    // 显示当前按压达标率值，初始隐藏
    ui->screen_setting_label_press_rate = lv_label_create(ui->screen_setting_cont_setting);
    lv_label_set_text(ui->screen_setting_label_press_rate, "70%");
    lv_label_set_long_mode(ui->screen_setting_label_press_rate, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_setting_label_press_rate, 70, 244);
    lv_obj_set_size(ui->screen_setting_label_press_rate, 109, 43);
    lv_obj_add_flag(ui->screen_setting_label_press_rate, LV_OBJ_FLAG_HIDDEN);

    // 值标签样式：同上，无背景、黑色文字
    lv_obj_set_style_border_width(ui->screen_setting_label_press_rate, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_setting_label_press_rate, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_setting_label_press_rate, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_setting_label_press_rate, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_label_press_rate, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_label_press_rate, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_label_press_rate, &lv_font_SourceHanSerifSC_Regular_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_label_press_rate, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_setting_label_press_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_setting_label_press_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_label_press_rate, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_setting_label_press_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_setting_label_press_rate, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_setting_label_press_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_setting_label_press_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_setting_label_press_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_label_press_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);


    // ==================== 创建 “时间值” 标签 ====================
    // 显示当前时间设置值，初始隐藏
    ui->screen_setting_label_time_value = lv_label_create(ui->screen_setting_cont_setting);
    lv_label_set_text(ui->screen_setting_label_time_value, "200s");
    lv_label_set_long_mode(ui->screen_setting_label_time_value, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_setting_label_time_value, 70, 244);
    lv_obj_set_size(ui->screen_setting_label_time_value, 109, 43);
    lv_obj_add_flag(ui->screen_setting_label_time_value, LV_OBJ_FLAG_HIDDEN);

    // 值标签样式：同上，无背景、黑色文字
    lv_obj_set_style_border_width(ui->screen_setting_label_time_value, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_border_opa(ui->screen_setting_label_time_value, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_border_color(ui->screen_setting_label_time_value, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_border_side(ui->screen_setting_label_time_value, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_radius(ui->screen_setting_label_time_value, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_text_color(ui->screen_setting_label_time_value, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_text_font(ui->screen_setting_label_time_value, &lv_font_SourceHanSerifSC_Regular_26, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_text_opa(ui->screen_setting_label_time_value, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_text_letter_space(ui->screen_setting_label_time_value, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_text_line_space(ui->screen_setting_label_time_value, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_text_align(ui->screen_setting_label_time_value, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_bg_opa(ui->screen_setting_label_time_value, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_pad_top(ui->screen_setting_label_time_value, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_pad_right(ui->screen_setting_label_time_value, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_pad_bottom(ui->screen_setting_label_time_value, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_pad_left(ui->screen_setting_label_time_value, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_shadow_width(ui->screen_setting_label_time_value, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    // ==================== 创建 “设置” 标题标签 ====================
    ui->screen_setting_label_set_title = lv_label_create(ui->screen_setting_cont_setting);
    lv_label_set_text(ui->screen_setting_label_set_title, "设置");   // 标题文本
    lv_label_set_long_mode(ui->screen_setting_label_set_title, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_setting_label_set_title, 68, 5);       // 位置
    lv_obj_set_size(ui->screen_setting_label_set_title, 100, 32);    // 大小

    // 标题标签样式：无背景、无边框、黑色文字、居中
    lv_obj_set_style_border_width(ui->screen_setting_label_set_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_radius(ui->screen_setting_label_set_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_text_color(ui->screen_setting_label_set_title, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_text_font(ui->screen_setting_label_set_title, &lv_font_AlimamaDongFangDaKai_21, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_text_opa(ui->screen_setting_label_set_title, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_text_letter_space(ui->screen_setting_label_set_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_text_line_space(ui->screen_setting_label_set_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_text_align(ui->screen_setting_label_set_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_bg_opa(ui->screen_setting_label_set_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_pad_top(ui->screen_setting_label_set_title, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_pad_right(ui->screen_setting_label_set_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_pad_bottom(ui->screen_setting_label_set_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_pad_left(ui->screen_setting_label_set_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
   lv_obj_set_style_shadow_width(ui->screen_setting_label_set_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    // ==================== 自定义代码区域 ====================
    //The custom code of screen_setting.


    // ==================== 更新布局 ====================
    // 强制刷新布局，确保所有控件位置正确
    lv_obj_update_layout(ui->screen_setting);

    // ==================== 初始化事件 ====================
    // 为屏幕绑定事件回调（如按钮点击）
    events_init_screen_setting(ui);
}










//-----------------------------------------------------------------------------------------------------------------------

static void screen_setting_btn_plus_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);

    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(lv_obj_has_state(ui->screen_setting_btn_1_time_set, LV_STATE_CHECKED))
        {
            Record.set_work_time += 10;
            if (Record.set_work_time > 990) Record.set_work_time = 990;
            lv_label_set_text_fmt(ui->screen_setting_label_time_value, "%ds", Record.set_work_time);
        }
        else if(lv_obj_has_state(ui->screen_setting_btn_2_press_rate, LV_STATE_CHECKED))
        {
            Record.set_press_rate += 1;
            if (Record.set_press_rate >= 100) Record.set_press_rate = 100;
            lv_label_set_text_fmt(ui->screen_setting_label_press_rate, "%d%%", Record.set_press_rate);
        }
        else if(lv_obj_has_state(ui->screen_setting_btn_3_air_rate, LV_STATE_CHECKED))
        {
            Record.set_air_rate += 1;
            if (Record.set_air_rate > 100) Record.set_air_rate = 100;
            lv_label_set_text_fmt(ui->screen_setting_label_air_rate, "%d%%", Record.set_air_rate);
        }

        break;
    }
    default:
        break;
    }
}

static void screen_setting_btn_minus_event_handler (lv_event_t *e)
{
    LV_LOG_USER("Clicked");
    lv_event_code_t code = lv_event_get_code(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);

    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(lv_obj_has_state(ui->screen_setting_btn_1_time_set, LV_STATE_CHECKED))
        {
            Record.set_work_time -= 10;
            if (Record.set_work_time <= 10) Record.set_work_time = 10;
            lv_label_set_text_fmt(ui->screen_setting_label_time_value, "%ds", Record.set_work_time);
        }
        else if(lv_obj_has_state(ui->screen_setting_btn_2_press_rate, LV_STATE_CHECKED))
        {
            Record.set_press_rate -= 1;
            if (Record.set_press_rate <= 1) Record.set_press_rate = 1;
            lv_label_set_text_fmt(ui->screen_setting_label_press_rate, "%d%%", Record.set_press_rate);
        }
        else if(lv_obj_has_state(ui->screen_setting_btn_3_air_rate, LV_STATE_CHECKED))
        {
            Record.set_air_rate -= 1;
            if (Record.set_air_rate <= 1) Record.set_air_rate = 1;
            lv_label_set_text_fmt(ui->screen_setting_label_air_rate, "%d%%", Record.set_air_rate);
        }
        break;
    }
    default:
        break;
    }
}



static void screen_setting_btn_1_time_set_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lvgl_ui_t *ui = (lvgl_ui_t *)lv_event_get_user_data(e);

    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        if (lv_obj_has_state(btn, LV_STATE_CHECKED))
        {
            // 互斥：清除其他按钮的选中状态
            lv_obj_clear_state(ui->screen_setting_btn_3_air_rate, LV_STATE_CHECKED);
            lv_obj_clear_state(ui->screen_setting_btn_2_press_rate, LV_STATE_CHECKED);
            // 显示当前相关控件
            lv_obj_clear_flag(ui->screen_setting_btn_minus, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui->screen_setting_label_time_value, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui->screen_setting_btn_plus, LV_OBJ_FLAG_HIDDEN);
            // 隐藏其他 label
            lv_obj_add_flag(ui->screen_setting_label_press_rate, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui->screen_setting_label_air_rate, LV_OBJ_FLAG_HIDDEN);

            Record.setting_mode = 1;
            Flag.work_time_set = 1;
        }
        else
        {
            // 如果取消选中，隐藏当前相关控件
            lv_obj_add_flag(ui->screen_setting_btn_minus, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui->screen_setting_label_time_value, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui->screen_setting_btn_plus, LV_OBJ_FLAG_HIDDEN);

            Record.setting_mode = 0;
            Flag.work_time_set = 0;
        }
        break;
    }
    default:
        break;
    }
}


static void screen_setting_btn_2_press_rate_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lvgl_ui_t *ui = (lvgl_ui_t *)lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        if (lv_obj_has_state(btn, LV_STATE_CHECKED))
        {
            // 互斥：清除其他按钮的选中状态
            lv_obj_clear_state(ui->screen_setting_btn_1_time_set, LV_STATE_CHECKED);
            lv_obj_clear_state(ui->screen_setting_btn_3_air_rate, LV_STATE_CHECKED);
            // 显示当前相关控件
            lv_obj_clear_flag(ui->screen_setting_btn_minus, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui->screen_setting_btn_plus, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui->screen_setting_label_press_rate, LV_OBJ_FLAG_HIDDEN);
            // 隐藏其他 label
            lv_obj_add_flag(ui->screen_setting_label_time_value, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui->screen_setting_label_air_rate, LV_OBJ_FLAG_HIDDEN);

            Record.setting_mode = 1;
            Flag.press_rate_set = 1;
        }
        else
        {
            // 如果取消选中，隐藏当前相关控件
            lv_obj_add_flag(ui->screen_setting_btn_minus, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui->screen_setting_btn_plus, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui->screen_setting_label_press_rate, LV_OBJ_FLAG_HIDDEN);

            Record.setting_mode = 0;
            Flag.press_rate_set = 0;
        }

        break;
    }
    default:
        break;
    }
}


static void screen_setting_btn_3_air_rate_event_handler (lv_event_t *e)
{
    // 获取当前事件触发类型
    lv_event_code_t code = lv_event_get_code(e);
    // 获取触发事件的控件对象
    lv_obj_t *btn = lv_event_get_target(e);
    // 获取在注册事件时传入的用户数据
    lvgl_ui_t *ui = (lvgl_ui_t *)lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        if(lv_obj_has_state(btn, LV_STATE_CHECKED))
        {
            // 互斥：清除其他两个按钮的选中状态
            lv_obj_clear_state(ui->screen_setting_btn_1_time_set, LV_STATE_CHECKED);
            lv_obj_clear_state(ui->screen_setting_btn_2_press_rate, LV_STATE_CHECKED);

            // 显示当前相关控件
            lv_obj_clear_flag(ui->screen_setting_label_air_rate, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui->screen_setting_btn_plus, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui->screen_setting_btn_minus, LV_OBJ_FLAG_HIDDEN);

            // 隐藏其他 label
            lv_obj_add_flag(ui->screen_setting_label_time_value, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui->screen_setting_label_press_rate, LV_OBJ_FLAG_HIDDEN);

            Record.setting_mode = 1;
        }
        else
        {
            // 取消选中时，隐藏所有
            lv_obj_add_flag(ui->screen_setting_label_air_rate, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui->screen_setting_btn_plus, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui->screen_setting_btn_minus, LV_OBJ_FLAG_HIDDEN);

            Record.setting_mode = 0;
        }

        break;
    }
    default:
        break;
    }
}


void events_init_screen_setting (lvgl_ui_t *ui)
{
    // 设置工作时间按键回调事件注册函数
    lv_obj_add_event_cb(ui->screen_setting_btn_1_time_set, screen_setting_btn_1_time_set_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    // 设置压力达标率的按键回调事件注册函数
    lv_obj_add_event_cb(ui->screen_setting_btn_2_press_rate, screen_setting_btn_2_press_rate_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    // 设置潮气达标率的按键回调事件注册函数
    lv_obj_add_event_cb(ui->screen_setting_btn_3_air_rate, screen_setting_btn_3_air_rate_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    // 数值加按键回调注册函数
    lv_obj_add_event_cb(ui->screen_setting_btn_plus, screen_setting_btn_plus_event_handler, LV_EVENT_CLICKED, ui);
    // 数值减按键回调注册函数
    lv_obj_add_event_cb(ui->screen_setting_btn_minus, screen_setting_btn_minus_event_handler, LV_EVENT_CLICKED, ui);
}












