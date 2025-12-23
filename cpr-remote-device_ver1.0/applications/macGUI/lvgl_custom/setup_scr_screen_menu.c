/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-11-02     Administrator       the first version
 */
#include "bsp_sys.h"




void setup_scr_screen_menu(lvgl_ui_t *ui)
{
    //Write codes screen_menu
    ui->screen_menu = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_menu, 240, 320);
    lv_obj_set_scrollbar_mode(ui->screen_menu, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_menu, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_cont_menu
    ui->screen_menu_cont_menu = lv_obj_create(ui->screen_menu);
    lv_obj_set_pos(ui->screen_menu_cont_menu, 0, 0);
    lv_obj_set_size(ui->screen_menu_cont_menu, 240, 320);
    lv_obj_set_scrollbar_mode(ui->screen_menu_cont_menu, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_menu_cont_menu, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_menu_cont_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_cont_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_menu_cont_menu, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_menu_cont_menu, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_menu_cont_menu, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_menu_cont_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_menu_cont_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_menu_cont_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_menu_cont_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_menu_cont_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_label_8_printer
    ui->screen_menu_label_8_printer = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_8_printer, "打印");
    lv_label_set_long_mode(ui->screen_menu_label_8_printer, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_menu_label_8_printer, 92, 289);
    lv_obj_set_size(ui->screen_menu_label_8_printer, 55, 18);

    //Write style for screen_menu_label_8_printer, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_menu_label_8_printer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_label_8_printer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_menu_label_8_printer, lv_color_hex(0x2F35DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_menu_label_8_printer, &lv_font_AlimamaDongFangDaKai_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_menu_label_8_printer, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_menu_label_8_printer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_menu_label_8_printer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_menu_label_8_printer, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_menu_label_8_printer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_menu_label_8_printer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_menu_label_8_printer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_menu_label_8_printer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_menu_label_8_printer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_menu_label_8_printer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_label_1_assess
    ui->screen_menu_label_1_assess = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_1_assess, "考核模式");
    lv_label_set_long_mode(ui->screen_menu_label_1_assess, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_menu_label_1_assess, 10, 74);
    lv_obj_set_size(ui->screen_menu_label_1_assess, 65, 18);

    //Write style for screen_menu_label_1_assess, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_menu_label_1_assess, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_label_1_assess, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_menu_label_1_assess, lv_color_hex(0x2F35DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_menu_label_1_assess, &lv_font_AlimamaDongFangDaKai_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_menu_label_1_assess, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_menu_label_1_assess, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_menu_label_1_assess, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_menu_label_1_assess, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_menu_label_1_assess, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_menu_label_1_assess, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_menu_label_1_assess, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_menu_label_1_assess, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_menu_label_1_assess, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_menu_label_1_assess, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_label_2_competation
    ui->screen_menu_label_2_competation = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_2_competation, "竞赛模式");
    lv_label_set_long_mode(ui->screen_menu_label_2_competation, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_menu_label_2_competation, 85, 74);
    lv_obj_set_size(ui->screen_menu_label_2_competation, 68, 19);

    //Write style for screen_menu_label_2_competation, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_menu_label_2_competation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_label_2_competation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_menu_label_2_competation, lv_color_hex(0x2F35DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_menu_label_2_competation, &lv_font_AlimamaDongFangDaKai_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_menu_label_2_competation, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_menu_label_2_competation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_menu_label_2_competation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_menu_label_2_competation, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_menu_label_2_competation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_menu_label_2_competation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_menu_label_2_competation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_menu_label_2_competation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_menu_label_2_competation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_menu_label_2_competation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_label_3_train
    ui->screen_menu_label_3_train = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_3_train, "训练模式");
    lv_label_set_long_mode(ui->screen_menu_label_3_train, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_menu_label_3_train, 160, 74);
    lv_obj_set_size(ui->screen_menu_label_3_train, 68, 19);

    //Write style for screen_menu_label_3_train, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_menu_label_3_train, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_label_3_train, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_menu_label_3_train, lv_color_hex(0x2F35DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_menu_label_3_train, &lv_font_AlimamaDongFangDaKai_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_menu_label_3_train, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_menu_label_3_train, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_menu_label_3_train, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_menu_label_3_train, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_menu_label_3_train, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_menu_label_3_train, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_menu_label_3_train, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_menu_label_3_train, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_menu_label_3_train, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_menu_label_3_train, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_label_4_score
    ui->screen_menu_label_4_score = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_4_score, "成绩查询");
    lv_label_set_long_mode(ui->screen_menu_label_4_score, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_menu_label_4_score, 9, 183);
    lv_obj_set_size(ui->screen_menu_label_4_score, 65, 18);

    //Write style for screen_menu_label_4_score, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_menu_label_4_score, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_label_4_score, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_menu_label_4_score, lv_color_hex(0x2F35DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_menu_label_4_score, &lv_font_AlimamaDongFangDaKai_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_menu_label_4_score, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_menu_label_4_score, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_menu_label_4_score, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_menu_label_4_score, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_menu_label_4_score, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_menu_label_4_score, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_menu_label_4_score, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_menu_label_4_score, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_menu_label_4_score, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_menu_label_4_score, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_label_7_settings
    ui->screen_menu_label_7_settings = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_7_settings, "设置");
    lv_label_set_long_mode(ui->screen_menu_label_7_settings, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_menu_label_7_settings, 10, 290);
    lv_obj_set_size(ui->screen_menu_label_7_settings, 65, 18);

    //Write style for screen_menu_label_7_settings, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_menu_label_7_settings, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_label_7_settings, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_menu_label_7_settings, lv_color_hex(0x2F35DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_menu_label_7_settings, &lv_font_AlimamaDongFangDaKai_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_menu_label_7_settings, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_menu_label_7_settings, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_menu_label_7_settings, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_menu_label_7_settings, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_menu_label_7_settings, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_menu_label_7_settings, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_menu_label_7_settings, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_menu_label_7_settings, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_menu_label_7_settings, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_menu_label_7_settings, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_label_5_operation
    ui->screen_menu_label_5_operation = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_5_operation, "功能操作");
    lv_label_set_long_mode(ui->screen_menu_label_5_operation, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_menu_label_5_operation, 84, 183);
    lv_obj_set_size(ui->screen_menu_label_5_operation, 65, 18);

    //Write style for screen_menu_label_5_operation, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_menu_label_5_operation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_label_5_operation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_menu_label_5_operation, lv_color_hex(0x2F35DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_menu_label_5_operation, &lv_font_AlimamaDongFangDaKai_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_menu_label_5_operation, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_menu_label_5_operation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_menu_label_5_operation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_menu_label_5_operation, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_menu_label_5_operation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_menu_label_5_operation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_menu_label_5_operation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_menu_label_5_operation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_menu_label_5_operation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_menu_label_5_operation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_label_9_switch
    ui->screen_menu_label_9_switch = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_9_switch, "关闭");
    lv_label_set_long_mode(ui->screen_menu_label_9_switch, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_menu_label_9_switch, 164, 289);
    lv_obj_set_size(ui->screen_menu_label_9_switch, 65, 18);

    //Write style for screen_menu_label_9_switch, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_menu_label_9_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_label_9_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_menu_label_9_switch, lv_color_hex(0x2F35DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_menu_label_9_switch, &lv_font_AlimamaDongFangDaKai_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_menu_label_9_switch, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_menu_label_9_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_menu_label_9_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_menu_label_9_switch, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_menu_label_9_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_menu_label_9_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_menu_label_9_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_menu_label_9_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_menu_label_9_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_menu_label_9_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_img_2_competation
    ui->screen_menu_img_2_competation = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_2_competation, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_2_competation, &_competation_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_2_competation, 50,50);
    lv_img_set_angle(ui->screen_menu_img_2_competation, 0);
    lv_obj_set_pos(ui->screen_menu_img_2_competation, 95, 22);
    lv_obj_set_size(ui->screen_menu_img_2_competation, 50, 50);

    //Write style for screen_menu_img_2_competation, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_2_competation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_2_competation, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_img_2_competation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_menu_img_2_competation, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_img_3_train
    ui->screen_menu_img_3_train = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_3_train, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_3_train, &_train_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_3_train, 50,50);
    lv_img_set_angle(ui->screen_menu_img_3_train, 0);
    lv_obj_set_pos(ui->screen_menu_img_3_train, 172, 22);
    lv_obj_set_size(ui->screen_menu_img_3_train, 50, 50);

    //Write style for screen_menu_img_3_train, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_3_train, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_3_train, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_img_3_train, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_menu_img_3_train, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_img_4_score
    ui->screen_menu_img_4_score = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_4_score, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_4_score, &_score_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_4_score, 50,50);
    lv_img_set_angle(ui->screen_menu_img_4_score, 0);
    lv_obj_set_pos(ui->screen_menu_img_4_score, 19, 127);
    lv_obj_set_size(ui->screen_menu_img_4_score, 50, 50);

    //Write style for screen_menu_img_4_score, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_4_score, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_4_score, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_img_4_score, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_menu_img_4_score, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_img_7_setting
    ui->screen_menu_img_7_setting = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_7_setting, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_7_setting, &_setting_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_7_setting, 50,50);
    lv_img_set_angle(ui->screen_menu_img_7_setting, 0);
    lv_obj_set_pos(ui->screen_menu_img_7_setting, 19, 234);
    lv_obj_set_size(ui->screen_menu_img_7_setting, 50, 50);

    //Write style for screen_menu_img_7_setting, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_7_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_7_setting, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_img_7_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_menu_img_7_setting, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_img_6_instruction
    ui->screen_menu_img_6_instruction = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_6_instruction, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_6_instruction, &_specification_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_6_instruction, 50,50);
    lv_img_set_angle(ui->screen_menu_img_6_instruction, 0);
    lv_obj_set_pos(ui->screen_menu_img_6_instruction, 172, 127);
    lv_obj_set_size(ui->screen_menu_img_6_instruction, 50, 50);

    //Write style for screen_menu_img_6_instruction, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_6_instruction, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_6_instruction, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_img_6_instruction, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_menu_img_6_instruction, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_img_1_assess
    ui->screen_menu_img_1_assess = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_1_assess, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_1_assess, &_assess_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_1_assess, 50,50);
    lv_img_set_angle(ui->screen_menu_img_1_assess, 0);
    lv_obj_set_pos(ui->screen_menu_img_1_assess, 19, 22);
    lv_obj_set_size(ui->screen_menu_img_1_assess, 50, 50);

    //Write style for screen_menu_img_1_assess, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_1_assess, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_1_assess, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_img_1_assess, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_menu_img_1_assess, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_img_8_printer
    ui->screen_menu_img_8_printer = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_8_printer, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_8_printer, &_printer_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_8_printer, 50,50);
    lv_img_set_angle(ui->screen_menu_img_8_printer, 0);
    lv_obj_set_pos(ui->screen_menu_img_8_printer, 95, 234);
    lv_obj_set_size(ui->screen_menu_img_8_printer, 50, 50);

    //Write style for screen_menu_img_8_printer, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_8_printer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_8_printer, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_img_8_printer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_menu_img_8_printer, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_img_9_switch
    ui->screen_menu_img_9_switch = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_9_switch, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_9_switch, &_switch_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_9_switch, 50,50);
    lv_img_set_angle(ui->screen_menu_img_9_switch, 0);
    lv_obj_set_pos(ui->screen_menu_img_9_switch, 172, 234);
    lv_obj_set_size(ui->screen_menu_img_9_switch, 50, 50);

    //Write style for screen_menu_img_9_switch, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_9_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_9_switch, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_img_9_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_menu_img_9_switch, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_img_5_operation
    ui->screen_menu_img_5_operation = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_5_operation, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_5_operation, &_operation_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_5_operation, 50,50);
    lv_img_set_angle(ui->screen_menu_img_5_operation, 0);
    lv_obj_set_pos(ui->screen_menu_img_5_operation, 95, 127);
    lv_obj_set_size(ui->screen_menu_img_5_operation, 50, 50);

    //Write style for screen_menu_img_5_operation, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_5_operation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_5_operation, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_img_5_operation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_menu_img_5_operation, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_label_6_instructions
    ui->screen_menu_label_6_instructions = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_6_instructions, "使用说明");
    lv_label_set_long_mode(ui->screen_menu_label_6_instructions, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_menu_label_6_instructions, 159, 183);
    lv_obj_set_size(ui->screen_menu_label_6_instructions, 65, 18);

    //Write style for screen_menu_label_6_instructions, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_menu_label_6_instructions, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_label_6_instructions, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_menu_label_6_instructions, lv_color_hex(0x2F35DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_menu_label_6_instructions, &lv_font_AlimamaDongFangDaKai_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_menu_label_6_instructions, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_menu_label_6_instructions, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_menu_label_6_instructions, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_menu_label_6_instructions, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_menu_label_6_instructions, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_menu_label_6_instructions, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_menu_label_6_instructions, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_menu_label_6_instructions, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_menu_label_6_instructions, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_menu_label_6_instructions, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_menu_label_printing
    ui->screen_menu_label_printing = lv_label_create(ui->screen_menu);
    lv_label_set_text(ui->screen_menu_label_printing, "打印中...");
    lv_label_set_long_mode(ui->screen_menu_label_printing, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_menu_label_printing, 54, 125);
    lv_obj_set_size(ui->screen_menu_label_printing, 130, 70);
    lv_obj_add_flag(ui->screen_menu_label_printing, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_menu_label_printing, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_menu_label_printing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_label_printing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_menu_label_printing, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_menu_label_printing, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_menu_label_printing, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_menu_label_printing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_menu_label_printing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_menu_label_printing, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_menu_label_printing, 206, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_menu_label_printing, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_menu_label_printing, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_menu_label_printing, 23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_menu_label_printing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_menu_label_printing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_menu_label_printing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_menu_label_printing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);


    //The custom code of screen_menu.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_menu);

    //Init events for screen.
    events_init_screen_menu(ui);
}







//-----------------------------------------------------------------------------------------------------------------------
//考核模式
static void screen_menu_img_1_assess_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);

    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(ui, &ui->screen_data, ui->screen_data_del, &ui->screen_menu_del, setup_scr_screen_data, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        Record.menu_index = 2;
        Record.mode_data_in = 1;
        Record.mode_data_in_set = 1;
        break;
    }
    default:
        break;
    }
}

// 竞赛模式
static void screen_menu_img_2_competation_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(ui, &ui->screen_data, ui->screen_data_del, &ui->screen_menu_del, setup_scr_screen_data, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        Record.menu_index = 2;
        Record.mode_data_in = 2;
        Record.mode_data_in_set = 1;
        break;
    }
    default:
        break;
    }
}

// 训练模式
static void screen_menu_img_3_train_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(ui, &ui->screen_data, ui->screen_data_del, &ui->screen_menu_del, setup_scr_screen_data, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        Record.menu_index = 2;
        Record.mode_data_in = 3;
        Record.mode_data_in_set = 1;
        break;
    }
    default:
        break;
    }
}

// 操作功能
static void screen_menu_img_5_operation_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(ui, &ui->screen_operation, ui->screen_operation_del, &ui->screen_menu_del, setup_scr_screen_operation, LV_SCR_LOAD_ANIM_NONE, 0, 200, true, true);
        Record.menu_index = 2;
        break;
    }
    default:
        break;
    }
}

// 设置
static void screen_menu_img_7_setting_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(ui, &ui->screen_setting, ui->screen_setting_del, &ui->screen_menu_del, setup_scr_screen_setting, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        Record.menu_index = 2;
        break;
    }
    default:
        break;
    }
}

// 打印
static void screen_menu_img_8_printer_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        /* 1. 显示“打印中...” */
        lv_obj_clear_flag(ui->screen_menu_label_printing, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

// 开关
static void screen_menu_img_9_switch_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        battery_recharge_disable();
        break;
    }
    default:
        break;
    }
}


void events_init_screen_menu (lvgl_ui_t *ui)
{
    lv_obj_add_event_cb(ui->screen_menu_img_1_assess, screen_menu_img_1_assess_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_menu_img_2_competation, screen_menu_img_2_competation_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_menu_img_3_train, screen_menu_img_3_train_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_menu_img_5_operation, screen_menu_img_5_operation_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_menu_img_7_setting, screen_menu_img_7_setting_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_menu_img_8_printer, screen_menu_img_8_printer_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_menu_img_9_switch, screen_menu_img_9_switch_event_handler, LV_EVENT_CLICKED, ui);

}


