/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-11-04     Administrator       the first version
 */
#include "bsp_sys.h"



void setup_scr_screen_operation(lvgl_ui_t *ui)
{
    // ==================== 创建屏幕（screen_operation） ====================
    // 创建一个新屏幕对象，作为 LVGL 的顶级容器（NULL 表示父对象为屏幕）
    ui->screen_operation = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_operation, 240, 320);                    // 设置屏幕尺寸 240x320（常见 TFT 屏幕分辨率）
    lv_obj_set_scrollbar_mode(ui->screen_operation, LV_SCROLLBAR_MODE_OFF); // 关闭滚动条

    // 屏幕主容器样式：透明背景（便于叠加其他层）
    lv_obj_set_style_bg_opa(ui->screen_operation, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ==================== 创建主容器 cont_operation ====================
    // 主内容容器，覆盖整个屏幕，用于放置所有按钮
    ui->screen_operation_cont_operation = lv_obj_create(ui->screen_operation);
    lv_obj_set_pos(ui->screen_operation_cont_operation, 0, 0);          // 位置 (0,0)
    lv_obj_set_size(ui->screen_operation_cont_operation, 240, 320);     // 大小满屏
    lv_obj_set_scrollbar_mode(ui->screen_operation_cont_operation, LV_SCROLLBAR_MODE_OFF);

    // 主容器样式：白色背景 + 蓝色边框（视觉上像一个卡片）
    lv_obj_set_style_border_width(ui->screen_operation_cont_operation, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_operation_cont_operation, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_operation_cont_operation, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_operation_cont_operation, LV_BORDER_SIDE_FULL, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_operation_cont_operation, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_operation_cont_operation, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_operation_cont_operation, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_operation_cont_operation, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_operation_cont_operation, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_operation_cont_operation, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_operation_cont_operation, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_operation_cont_operation, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_operation_cont_operation, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ==================== 按钮 11：瞳孔检查 ====================
    ui->screen_operation_btn_opera_11 = lv_btn_create(ui->screen_operation_cont_operation);
    lv_obj_add_flag(ui->screen_operation_btn_opera_11, LV_OBJ_FLAG_CHECKABLE); // 可选中（切换按钮）
    ui->screen_operation_btn_opera_11_label = lv_label_create(ui->screen_operation_btn_opera_11);
    lv_label_set_text(ui->screen_operation_btn_opera_11_label, "瞳孔\n检查");   // \n 实现换行
    lv_label_set_long_mode(ui->screen_operation_btn_opera_11_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_operation_btn_opera_11_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_operation_btn_opera_11, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_operation_btn_opera_11_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_operation_btn_opera_11, 174, 210);       // 位置
    lv_obj_set_size(ui->screen_operation_btn_opera_11, 56, 77);        // 大小

    // 按钮默认样式：蓝色背景、白字、圆角
    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_11, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_11, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_operation_btn_opera_11, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_operation_btn_opera_11, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_operation_btn_opera_11, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_11, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_11, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_operation_btn_opera_11, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_operation_btn_opera_11, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_operation_btn_opera_11, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    /* 按下（选中）状态：橙红色背景 + 白色文字 */
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_11, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_11, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_11, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_color(ui->screen_operation_btn_opera_11, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_11, 8, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_spread(ui->screen_operation_btn_opera_11, 2, LV_PART_MAIN | LV_STATE_CHECKED);

    // ==================== 按钮 10：呼吸检查 ====================
    ui->screen_operation_btn_opera_10 = lv_btn_create(ui->screen_operation_cont_operation);
    lv_obj_add_flag(ui->screen_operation_btn_opera_10, LV_OBJ_FLAG_CHECKABLE);
    ui->screen_operation_btn_opera_10_label = lv_label_create(ui->screen_operation_btn_opera_10);
    lv_label_set_text(ui->screen_operation_btn_opera_10_label, "呼吸\n检查");
    lv_label_set_long_mode(ui->screen_operation_btn_opera_10_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_operation_btn_opera_10_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_operation_btn_opera_10, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_operation_btn_opera_10_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_operation_btn_opera_10, 113, 210);
    lv_obj_set_size(ui->screen_operation_btn_opera_10, 56, 77);

    // 样式同上（所有按钮默认样式一致）
    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_10, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_10, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_operation_btn_opera_10, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_operation_btn_opera_10, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_operation_btn_opera_10, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_10, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_10, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_operation_btn_opera_10, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_operation_btn_opera_10, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_operation_btn_opera_10, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    /* 按下（选中）状态：橙红色背景 + 白色文字 */
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_10, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_10, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_10, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_color(ui->screen_operation_btn_opera_10, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_10, 8, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_spread(ui->screen_operation_btn_opera_10, 2, LV_PART_MAIN | LV_STATE_CHECKED);


    // ==================== 按钮 9：脉搏检查 ====================
    ui->screen_operation_btn_opera_9 = lv_btn_create(ui->screen_operation_cont_operation);
    lv_obj_add_flag(ui->screen_operation_btn_opera_9, LV_OBJ_FLAG_CHECKABLE);
    ui->screen_operation_btn_opera_9_label = lv_label_create(ui->screen_operation_btn_opera_9);
    lv_label_set_text(ui->screen_operation_btn_opera_9_label, "脉搏\n检查");
    lv_label_set_long_mode(ui->screen_operation_btn_opera_9_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_operation_btn_opera_9_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_operation_btn_opera_9, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_operation_btn_opera_9_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_operation_btn_opera_9, 174, 114);
    lv_obj_set_size(ui->screen_operation_btn_opera_9, 56, 77);

    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_9, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_9, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_operation_btn_opera_9, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_operation_btn_opera_9, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_operation_btn_opera_9, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_9, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_9, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_operation_btn_opera_9, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_operation_btn_opera_9, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_operation_btn_opera_9, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    /* 按下（选中）状态：橙红色背景 + 白色文字 */
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_9, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_9, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_9, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_color(ui->screen_operation_btn_opera_9, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_9, 8, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_spread(ui->screen_operation_btn_opera_9, 2, LV_PART_MAIN | LV_STATE_CHECKED);


    // ==================== 按钮 8：急救呼吸 ====================
    ui->screen_operation_btn_opera_8 = lv_btn_create(ui->screen_operation_cont_operation);
    lv_obj_add_flag(ui->screen_operation_btn_opera_8, LV_OBJ_FLAG_CHECKABLE);
    ui->screen_operation_btn_opera_8_label = lv_label_create(ui->screen_operation_btn_opera_8);
    lv_label_set_text(ui->screen_operation_btn_opera_8_label, "急救\n呼吸");
    lv_label_set_long_mode(ui->screen_operation_btn_opera_8_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_operation_btn_opera_8_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_operation_btn_opera_8, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_operation_btn_opera_8_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_operation_btn_opera_8, 113, 114);
    lv_obj_set_size(ui->screen_operation_btn_opera_8, 56, 77);

    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_8, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_8, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_operation_btn_opera_8, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_operation_btn_opera_8, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_operation_btn_opera_8, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_8, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_8, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_operation_btn_opera_8, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_operation_btn_opera_8, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_operation_btn_opera_8, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    /* 按下（选中）状态：橙红色背景 + 白色文字 */
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_8, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_8, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_8, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    /* 按下时加个轻微阴影，让视觉更突出 */
    lv_obj_set_style_shadow_color(ui->screen_operation_btn_opera_8, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_8, 8, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_spread(ui->screen_operation_btn_opera_8, 2, LV_PART_MAIN | LV_STATE_CHECKED);


    // ==================== 按钮 7：清除异物 ====================
    ui->screen_operation_btn_opera_7 = lv_btn_create(ui->screen_operation_cont_operation);
    lv_obj_add_flag(ui->screen_operation_btn_opera_7, LV_OBJ_FLAG_CHECKABLE);
    ui->screen_operation_btn_opera_7_label = lv_label_create(ui->screen_operation_btn_opera_7);
    lv_label_set_text(ui->screen_operation_btn_opera_7_label, "清除\n异物");
    lv_label_set_long_mode(ui->screen_operation_btn_opera_7_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_operation_btn_opera_7_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_operation_btn_opera_7, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_operation_btn_opera_7_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_operation_btn_opera_7, 174, 17);
    lv_obj_set_size(ui->screen_operation_btn_opera_7, 56, 77);

    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_7, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_7, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_operation_btn_opera_7, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_operation_btn_opera_7, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_operation_btn_opera_7, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_7, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_7, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_operation_btn_opera_7, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_operation_btn_opera_7, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_operation_btn_opera_7, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    /* 按下（选中）状态：橙红色背景 + 白色文字 */
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_7, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_7, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_7, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    /* 按下时加个轻微阴影，让视觉更突出 */
    lv_obj_set_style_shadow_color(ui->screen_operation_btn_opera_7, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_7, 8, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_spread(ui->screen_operation_btn_opera_7, 2, LV_PART_MAIN | LV_STATE_CHECKED);


    // ==================== 按钮 6：意识判断 ====================
    ui->screen_operation_btn_opera_6 = lv_btn_create(ui->screen_operation_cont_operation);
    lv_obj_add_flag(ui->screen_operation_btn_opera_6, LV_OBJ_FLAG_CHECKABLE);
    ui->screen_operation_btn_opera_6_label = lv_label_create(ui->screen_operation_btn_opera_6);
    lv_label_set_text(ui->screen_operation_btn_opera_6_label, "意识\n判断");
    lv_label_set_long_mode(ui->screen_operation_btn_opera_6_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_operation_btn_opera_6_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_operation_btn_opera_6, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_operation_btn_opera_6_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_operation_btn_opera_6, 113, 17);
    lv_obj_set_size(ui->screen_operation_btn_opera_6, 56, 77);

    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_6, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_6, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_operation_btn_opera_6, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_operation_btn_opera_6, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_operation_btn_opera_6, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_6, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_6, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_operation_btn_opera_6, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_operation_btn_opera_6, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_operation_btn_opera_6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    /* 按下（选中）状态：橙红色背景 + 白色文字 */
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_6, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_6, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_6, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    /* 按下时加个轻微阴影，让视觉更突出 */
    lv_obj_set_style_shadow_color(ui->screen_operation_btn_opera_6, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_6, 8, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_spread(ui->screen_operation_btn_opera_6, 2, LV_PART_MAIN | LV_STATE_CHECKED);


    // ==================== 按钮 5：脉搏无 ====================
    ui->screen_operation_btn_opera_5 = lv_btn_create(ui->screen_operation_cont_operation);
    lv_obj_add_flag(ui->screen_operation_btn_opera_5, LV_OBJ_FLAG_CHECKABLE);
    ui->screen_operation_btn_opera_5_label = lv_label_create(ui->screen_operation_btn_opera_5);
    lv_label_set_text(ui->screen_operation_btn_opera_5_label, "脉搏无");
    lv_label_set_long_mode(ui->screen_operation_btn_opera_5_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_operation_btn_opera_5_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_operation_btn_opera_5, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_operation_btn_opera_5_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_operation_btn_opera_5, 13, 247);
    lv_obj_set_size(ui->screen_operation_btn_opera_5, 89, 44);

    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_5, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_5, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_operation_btn_opera_5, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_operation_btn_opera_5, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_operation_btn_opera_5, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_5, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_5, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_operation_btn_opera_5, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_operation_btn_opera_5, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_operation_btn_opera_5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    /* 按下（选中）状态：橙红色背景 + 白色文字 */
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_5, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_5, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_5, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    /* 按下时加个轻微阴影，让视觉更突出 */
    lv_obj_set_style_shadow_color(ui->screen_operation_btn_opera_5, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_5, 8, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_spread(ui->screen_operation_btn_opera_5, 2, LV_PART_MAIN | LV_STATE_CHECKED);


    // ==================== 按钮 4：脉搏有 ====================
    ui->screen_operation_btn_opera_4 = lv_btn_create(ui->screen_operation_cont_operation);
    lv_obj_add_flag(ui->screen_operation_btn_opera_4, LV_OBJ_FLAG_CHECKABLE);
    ui->screen_operation_btn_opera_4_label = lv_label_create(ui->screen_operation_btn_opera_4);
    lv_label_set_text(ui->screen_operation_btn_opera_4_label, "脉搏有");
    lv_label_set_long_mode(ui->screen_operation_btn_opera_4_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_operation_btn_opera_4_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_operation_btn_opera_4, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_operation_btn_opera_4_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_operation_btn_opera_4, 13, 189);
    lv_obj_set_size(ui->screen_operation_btn_opera_4, 89, 44);

    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_4, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_4, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_operation_btn_opera_4, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_operation_btn_opera_4, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_operation_btn_opera_4, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_4, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_4, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_operation_btn_opera_4, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_operation_btn_opera_4, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_operation_btn_opera_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* 按下（选中）状态：橙红色背景 + 白色文字 */
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_4, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_4, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_4, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    /* 按下时加个轻微阴影，让视觉更突出 */
    lv_obj_set_style_shadow_color(ui->screen_operation_btn_opera_4, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_4, 8, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_spread(ui->screen_operation_btn_opera_4, 2, LV_PART_MAIN | LV_STATE_CHECKED);

    // ==================== 按钮 3：瞳孔缩小 ====================
    ui->screen_operation_btn_opera_3 = lv_btn_create(ui->screen_operation_cont_operation);
    lv_obj_add_flag(ui->screen_operation_btn_opera_3, LV_OBJ_FLAG_CHECKABLE);
    ui->screen_operation_btn_opera_3_label = lv_label_create(ui->screen_operation_btn_opera_3);
    lv_label_set_text(ui->screen_operation_btn_opera_3_label, "瞳孔缩小");
    lv_label_set_long_mode(ui->screen_operation_btn_opera_3_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_operation_btn_opera_3_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_operation_btn_opera_3, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_operation_btn_opera_3_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_operation_btn_opera_3, 13, 131);
    lv_obj_set_size(ui->screen_operation_btn_opera_3, 89, 44);

    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_3, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_operation_btn_opera_3, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_operation_btn_opera_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_operation_btn_opera_3, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_3, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_operation_btn_opera_3, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_operation_btn_opera_3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_operation_btn_opera_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* 按下（选中）状态：橙红色背景 + 白色文字 */
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_3, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_3, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_3, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_color(ui->screen_operation_btn_opera_3, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_3, 8, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_spread(ui->screen_operation_btn_opera_3, 2, LV_PART_MAIN | LV_STATE_CHECKED);

    // ==================== 按钮 2：瞳孔放大 ====================
    ui->screen_operation_btn_opera_2 = lv_btn_create(ui->screen_operation_cont_operation);
    lv_obj_add_flag(ui->screen_operation_btn_opera_2, LV_OBJ_FLAG_CHECKABLE);
    ui->screen_operation_btn_opera_2_label = lv_label_create(ui->screen_operation_btn_opera_2);
    lv_label_set_text(ui->screen_operation_btn_opera_2_label, "瞳孔放大");
    lv_label_set_long_mode(ui->screen_operation_btn_opera_2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_operation_btn_opera_2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_operation_btn_opera_2, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_operation_btn_opera_2_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_operation_btn_opera_2, 13, 73);
    lv_obj_set_size(ui->screen_operation_btn_opera_2, 89, 44);

    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_2, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_operation_btn_opera_2, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_operation_btn_opera_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_operation_btn_opera_2, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_2, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_operation_btn_opera_2, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_operation_btn_opera_2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_operation_btn_opera_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* 按下（选中）状态：橙红色背景 + 白色文字 */
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_2, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_2, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_color(ui->screen_operation_btn_opera_2, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_2, 8, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_spread(ui->screen_operation_btn_opera_2, 2, LV_PART_MAIN | LV_STATE_CHECKED);

    // ==================== 按钮 1：瞳孔正常 ====================
    ui->screen_operation_btn_opera_1 = lv_btn_create(ui->screen_operation_cont_operation);
    lv_obj_add_flag(ui->screen_operation_btn_opera_1, LV_OBJ_FLAG_CHECKABLE);
    ui->screen_operation_btn_opera_1_label = lv_label_create(ui->screen_operation_btn_opera_1);
    lv_label_set_text(ui->screen_operation_btn_opera_1_label, "瞳孔正常");
    lv_label_set_long_mode(ui->screen_operation_btn_opera_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_operation_btn_opera_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_operation_btn_opera_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_operation_btn_opera_1_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_operation_btn_opera_1, 13, 15);
    lv_obj_set_size(ui->screen_operation_btn_opera_1, 89, 44);

    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_1, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_operation_btn_opera_1, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_operation_btn_opera_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_operation_btn_opera_1, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_1, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_operation_btn_opera_1, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_operation_btn_opera_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_operation_btn_opera_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);


    /* 按下（选中）状态：橙红色背景 + 白色文字 */
    lv_obj_set_style_bg_color(ui->screen_operation_btn_opera_1, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui->screen_operation_btn_opera_1, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_operation_btn_opera_1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    /* 按下时加个轻微阴影，让视觉更突出 */
    lv_obj_set_style_shadow_color(ui->screen_operation_btn_opera_1, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_operation_btn_opera_1, 8, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_spread(ui->screen_operation_btn_opera_1, 2, LV_PART_MAIN | LV_STATE_CHECKED);

    // ==================== 自定义代码区域 ====================
    //The custom code of screen_operation.


    // ==================== 更新布局 ====================
    // 强制刷新布局，确保所有控件位置正确
    lv_obj_update_layout(ui->screen_operation);

    // ==================== 初始化事件 ====================
    // 为屏幕绑定事件回调（如按钮点击）
    events_init_screen_operation(ui);
}





static void screen_operation_btn_opera_11_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        break;
    }
    default:
        break;
    }
}

static void screen_operation_btn_opera_10_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        break;
    }
    default:
        break;
    }
}

static void screen_operation_btn_opera_9_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        break;
    }
    default:
        break;
    }
}

static void screen_operation_btn_opera_8_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        break;
    }
    default:
        break;
    }
}

static void screen_operation_btn_opera_7_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        break;
    }
    default:
        break;
    }
}

static void screen_operation_btn_opera_6_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        break;
    }
    default:
        break;
    }
}

// "脉搏无"按钮
static void screen_operation_btn_opera_5_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        if(lv_obj_has_state(btn, LV_STATE_CHECKED)) {
            lv_obj_clear_state(ui->screen_operation_btn_opera_4, LV_STATE_CHECKED);
        }
        break;
    }
    default:
        break;
    }
}

// "脉搏有"按钮
static void screen_operation_btn_opera_4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        if(lv_obj_has_state(btn, LV_STATE_CHECKED)) {
            lv_obj_clear_state(ui->screen_operation_btn_opera_5, LV_STATE_CHECKED);
        }
        break;
    }
    default:
        break;
    }
}

// "瞳孔缩小"按钮
static void screen_operation_btn_opera_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        if(lv_obj_has_state(btn, LV_STATE_CHECKED))
        {
            // 互斥：清除其他两个按钮的选中状态
            lv_obj_clear_state(ui->screen_operation_btn_opera_1, LV_STATE_CHECKED);
            lv_obj_clear_state(ui->screen_operation_btn_opera_2, LV_STATE_CHECKED);
        }
        break;
    }
    default:
        break;
    }
}

// "瞳孔放大"按钮
static void screen_operation_btn_opera_2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);

    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        if(lv_obj_has_state(btn, LV_STATE_CHECKED))
        {
            // 互斥：清除其他两个按钮的选中状态
            lv_obj_clear_state(ui->screen_operation_btn_opera_1, LV_STATE_CHECKED);
            lv_obj_clear_state(ui->screen_operation_btn_opera_3, LV_STATE_CHECKED);
        }
        break;
    }
    default:
        break;
    }
}

// "瞳孔正常"按钮
static void screen_operation_btn_opera_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);

    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        if(lv_obj_has_state(btn, LV_STATE_CHECKED))
        {
            // 互斥：清除其他两个按钮的选中状态
            lv_obj_clear_state(ui->screen_operation_btn_opera_2, LV_STATE_CHECKED);
            lv_obj_clear_state(ui->screen_operation_btn_opera_3, LV_STATE_CHECKED);
        }
        break;
    }
    default:
        break;
    }
}




void events_init_screen_operation (lvgl_ui_t *ui)
{
    lv_obj_add_event_cb(ui->screen_operation_btn_opera_11, screen_operation_btn_opera_11_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_operation_btn_opera_10, screen_operation_btn_opera_10_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_operation_btn_opera_9, screen_operation_btn_opera_9_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_operation_btn_opera_8, screen_operation_btn_opera_8_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_operation_btn_opera_7, screen_operation_btn_opera_7_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_operation_btn_opera_6, screen_operation_btn_opera_6_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_operation_btn_opera_5, screen_operation_btn_opera_5_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_operation_btn_opera_4, screen_operation_btn_opera_4_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_operation_btn_opera_3, screen_operation_btn_opera_3_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_operation_btn_opera_2, screen_operation_btn_opera_2_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_operation_btn_opera_1, screen_operation_btn_opera_1_event_handler, LV_EVENT_VALUE_CHANGED, ui);
}



