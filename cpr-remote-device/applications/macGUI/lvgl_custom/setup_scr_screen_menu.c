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
#include "setup_scr_screen.h"


/* Helper: apply card style to a menu card container */
static void menu_card_style(lv_obj_t *card)
{
    lv_obj_set_style_bg_opa(card, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(card, lv_color_hex(0xF0F4F8), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(card, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(card, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(card, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(card, lv_color_hex(0xCCCCCC), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(card, 100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(card, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(card, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    /* Pressed state */
    lv_obj_set_style_bg_color(card, lv_color_hex(0xE0E7EE), LV_PART_MAIN|LV_STATE_PRESSED);
}

/* Helper: style a label for menu card bottom text */
static void menu_label_style(lv_obj_t *lbl)
{
    lv_obj_set_style_border_width(lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lbl, lv_color_hex(0x2F35DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl, &lv_font_AlimamaDongFangDaKai_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(lbl, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
}

void setup_scr_screen_menu(lvgl_ui_t *ui)
{
    /* Create screen */
    ui->screen_menu = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_menu, 240, 320);
    lv_obj_set_scrollbar_mode(ui->screen_menu, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(ui->screen_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    /* Main container with flex grid layout */
    ui->screen_menu_cont_menu = lv_obj_create(ui->screen_menu);
    lv_obj_set_pos(ui->screen_menu_cont_menu, 0, 0);
    lv_obj_set_size(ui->screen_menu_cont_menu, 240, 320);
    lv_obj_set_scrollbar_mode(ui->screen_menu_cont_menu, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(ui->screen_menu_cont_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_cont_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_menu_cont_menu, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_menu_cont_menu, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_menu_cont_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_menu_cont_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_menu_cont_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_menu_cont_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_menu_cont_menu, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    /* Flex flow: 3-column wrap grid */
    lv_obj_set_flex_flow(ui->screen_menu_cont_menu, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(ui->screen_menu_cont_menu, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(ui->screen_menu_cont_menu, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->screen_menu_cont_menu, 8, LV_PART_MAIN|LV_STATE_DEFAULT);

    /* === Create images first (before reparenting into cards) === */
    ui->screen_menu_img_1_assess = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_1_assess, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_1_assess, &_assess_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_1_assess, 25, 25);
    lv_obj_set_size(ui->screen_menu_img_1_assess, 50, 50);
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_1_assess, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_1_assess, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    ui->screen_menu_label_1_assess = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_1_assess, "考核模式");
    lv_obj_set_size(ui->screen_menu_label_1_assess, 65, 18);
    menu_label_style(ui->screen_menu_label_1_assess);

    ui->screen_menu_img_2_competation = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_2_competation, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_2_competation, &_competation_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_2_competation, 25, 25);
    lv_obj_set_size(ui->screen_menu_img_2_competation, 50, 50);
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_2_competation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_2_competation, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    ui->screen_menu_label_2_competation = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_2_competation, "竞赛模式");
    lv_obj_set_size(ui->screen_menu_label_2_competation, 68, 18);
    menu_label_style(ui->screen_menu_label_2_competation);

    ui->screen_menu_img_3_train = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_3_train, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_3_train, &_train_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_3_train, 25, 25);
    lv_obj_set_size(ui->screen_menu_img_3_train, 50, 50);
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_3_train, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_3_train, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    ui->screen_menu_label_3_train = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_3_train, "训练模式");
    lv_obj_set_size(ui->screen_menu_label_3_train, 68, 18);
    menu_label_style(ui->screen_menu_label_3_train);

    ui->screen_menu_img_4_score = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_4_score, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_4_score, &_score_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_4_score, 25, 25);
    lv_obj_set_size(ui->screen_menu_img_4_score, 50, 50);
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_4_score, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_4_score, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    ui->screen_menu_label_4_score = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_4_score, "成绩查询");
    lv_obj_set_size(ui->screen_menu_label_4_score, 65, 18);
    menu_label_style(ui->screen_menu_label_4_score);

    ui->screen_menu_img_5_operation = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_5_operation, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_5_operation, &_operation_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_5_operation, 25, 25);
    lv_obj_set_size(ui->screen_menu_img_5_operation, 50, 50);
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_5_operation, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_5_operation, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    ui->screen_menu_label_5_operation = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_5_operation, "功能操作");
    lv_obj_set_size(ui->screen_menu_label_5_operation, 65, 18);
    menu_label_style(ui->screen_menu_label_5_operation);

    ui->screen_menu_img_6_instruction = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_6_instruction, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_6_instruction, &_specification_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_6_instruction, 25, 25);
    lv_obj_set_size(ui->screen_menu_img_6_instruction, 50, 50);
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_6_instruction, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_6_instruction, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    ui->screen_menu_label_6_instructions = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_6_instructions, "使用说明");
    lv_obj_set_size(ui->screen_menu_label_6_instructions, 65, 18);
    menu_label_style(ui->screen_menu_label_6_instructions);

    ui->screen_menu_img_7_setting = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_7_setting, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_7_setting, &_setting_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_7_setting, 25, 25);
    lv_obj_set_size(ui->screen_menu_img_7_setting, 50, 50);
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_7_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_7_setting, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    ui->screen_menu_label_7_settings = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_7_settings, "设置");
    lv_obj_set_size(ui->screen_menu_label_7_settings, 65, 18);
    menu_label_style(ui->screen_menu_label_7_settings);

    ui->screen_menu_img_8_printer = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_8_printer, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_8_printer, &_printer_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_8_printer, 25, 25);
    lv_obj_set_size(ui->screen_menu_img_8_printer, 50, 50);
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_8_printer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_8_printer, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    ui->screen_menu_label_8_printer = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_8_printer, "打印");
    lv_obj_set_size(ui->screen_menu_label_8_printer, 55, 18);
    menu_label_style(ui->screen_menu_label_8_printer);

    ui->screen_menu_img_9_switch = lv_img_create(ui->screen_menu_cont_menu);
    lv_obj_add_flag(ui->screen_menu_img_9_switch, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_menu_img_9_switch, &_switch_alpha_50x50);
    lv_img_set_pivot(ui->screen_menu_img_9_switch, 25, 25);
    lv_obj_set_size(ui->screen_menu_img_9_switch, 50, 50);
    lv_obj_set_style_img_recolor_opa(ui->screen_menu_img_9_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_menu_img_9_switch, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    /* "关闭" changed to "返回" */
    ui->screen_menu_label_9_switch = lv_label_create(ui->screen_menu_cont_menu);
    lv_label_set_text(ui->screen_menu_label_9_switch, "返回");
    lv_obj_set_size(ui->screen_menu_label_9_switch, 65, 18);
    menu_label_style(ui->screen_menu_label_9_switch);

    /* === Reparent image+label pairs into card containers === */
    {
        lv_obj_t *imgs[] = {
            ui->screen_menu_img_1_assess,    ui->screen_menu_img_2_competation,
            ui->screen_menu_img_3_train,     ui->screen_menu_img_4_score,
            ui->screen_menu_img_5_operation, ui->screen_menu_img_6_instruction,
            ui->screen_menu_img_7_setting,   ui->screen_menu_img_8_printer,
            ui->screen_menu_img_9_switch
        };
        lv_obj_t *lbls[] = {
            ui->screen_menu_label_1_assess,       ui->screen_menu_label_2_competation,
            ui->screen_menu_label_3_train,        ui->screen_menu_label_4_score,
            ui->screen_menu_label_5_operation,    ui->screen_menu_label_6_instructions,
            ui->screen_menu_label_7_settings,     ui->screen_menu_label_8_printer,
            ui->screen_menu_label_9_switch
        };
        for (int i = 0; i < 9; i++) {
            /* Create card container */
            lv_obj_t *card = lv_obj_create(ui->screen_menu_cont_menu);
            lv_obj_set_size(card, 70, 80);
            lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
            menu_card_style(card);

            /* Reparent image into card */
            lv_obj_set_parent(imgs[i], card);
            lv_obj_clear_flag(imgs[i], LV_OBJ_FLAG_FLOATING);
            lv_obj_set_pos(imgs[i], 10, 5);
            lv_obj_set_style_radius(imgs[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);

            /* Reparent label into card */
            lv_obj_set_parent(lbls[i], card);
            lv_obj_clear_flag(lbls[i], LV_OBJ_FLAG_FLOATING);
            lv_obj_set_pos(lbls[i], 2, 58);
            lv_obj_set_size(lbls[i], 66, 18);
        }
    }

    /* === Printing label: dark rounded card style === */
    ui->screen_menu_label_printing = lv_label_create(ui->screen_menu);
    lv_label_set_text(ui->screen_menu_label_printing, "打印中...");
    lv_label_set_long_mode(ui->screen_menu_label_printing, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_menu_label_printing, 55, 124);
    lv_obj_set_size(ui->screen_menu_label_printing, 130, 70);
    lv_obj_add_flag(ui->screen_menu_label_printing, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_border_width(ui->screen_menu_label_printing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_menu_label_printing, 12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_menu_label_printing, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_menu_label_printing, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_menu_label_printing, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_menu_label_printing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_menu_label_printing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_menu_label_printing, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_menu_label_printing, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_menu_label_printing, lv_color_hex(0x333333), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_menu_label_printing, 24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_menu_label_printing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_menu_label_printing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_menu_label_printing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_menu_label_printing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    /* Update layout and init events */
    lv_obj_update_layout(ui->screen_menu);
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
        ui_btn_press_feedback(lv_event_get_target(e));
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

// Competition mode
static void screen_menu_img_2_competation_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_btn_press_feedback(lv_event_get_target(e));
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

// Training mode
static void screen_menu_img_3_train_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_btn_press_feedback(lv_event_get_target(e));
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

// Operation
static void screen_menu_img_5_operation_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_btn_press_feedback(lv_event_get_target(e));
        ui_load_scr_animation(ui, &ui->screen_operation, ui->screen_operation_del, &ui->screen_menu_del, setup_scr_screen_operation, LV_SCR_LOAD_ANIM_NONE, 0, 200, true, true);
        Record.menu_index = 2;
        break;
    }
    default:
        break;
    }
}

// Settings
static void screen_menu_img_7_setting_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_btn_press_feedback(lv_event_get_target(e));
        ui_load_scr_animation(ui, &ui->screen_setting, ui->screen_setting_del, &ui->screen_menu_del, setup_scr_screen_setting, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        Record.menu_index = 2;
        break;
    }
    default:
        break;
    }
}

// Print
static void screen_menu_img_8_printer_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_btn_press_feedback(lv_event_get_target(e));
        lv_obj_clear_flag(ui->screen_menu_label_printing, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

// Switch (back to main)
static void screen_menu_img_9_switch_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_btn_press_feedback(lv_event_get_target(e));
        battery_recharge_disable();
        break;
    }
    default:
        break;
    }
}


/* Score query - feedback only */
static void screen_menu_img_4_score_event_handler (lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        ui_btn_press_feedback(lv_event_get_target(e));
    }
}

/* Instructions - feedback only */
static void screen_menu_img_6_instruction_event_handler (lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        ui_btn_press_feedback(lv_event_get_target(e));
    }
}

void events_init_screen_menu (lvgl_ui_t *ui)
{
    lv_obj_add_event_cb(ui->screen_menu_img_1_assess, screen_menu_img_1_assess_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_menu_img_2_competation, screen_menu_img_2_competation_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_menu_img_3_train, screen_menu_img_3_train_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_menu_img_4_score, screen_menu_img_4_score_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_menu_img_5_operation, screen_menu_img_5_operation_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_menu_img_6_instruction, screen_menu_img_6_instruction_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_menu_img_7_setting, screen_menu_img_7_setting_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_menu_img_8_printer, screen_menu_img_8_printer_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_menu_img_9_switch, screen_menu_img_9_switch_event_handler, LV_EVENT_CLICKED, ui);
}


