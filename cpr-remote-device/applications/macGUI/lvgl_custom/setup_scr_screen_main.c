/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-08-29     18452       the first version
 */
#include "bsp_sys.h"

lvgl_ui_t guider_lvgl;

void setup_scr_screen_main(lvgl_ui_t *ui)
{
    //Write codes screen_main
    ui->screen_main = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_main, 240, 320);
    lv_obj_set_scrollbar_mode(ui->screen_main, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_main, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_cont_main
    ui->screen_main_cont_main = lv_obj_create(ui->screen_main);
    lv_obj_set_pos(ui->screen_main_cont_main, 0, 0);
    lv_obj_set_size(ui->screen_main_cont_main, 240, 320);
    lv_obj_set_scrollbar_mode(ui->screen_main_cont_main, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_main_cont_main, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_cont_main, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_main_cont_main, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_main_cont_main, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_btn_1_start
    ui->screen_main_btn_1_start = lv_btn_create(ui->screen_main_cont_main);
    ui->screen_main_btn_1_start_label = lv_label_create(ui->screen_main_btn_1_start);
    lv_label_set_text(ui->screen_main_btn_1_start_label, "开始");
    lv_label_set_long_mode(ui->screen_main_btn_1_start_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_1_start_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_1_start, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_1_start_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_1_start, 119, 251);
    lv_obj_set_size(ui->screen_main_btn_1_start, 100, 50);

    //Write style for screen_main_btn_1_start, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_1_start, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_main_btn_1_start, lv_color_hex(0x009ea9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_main_btn_1_start, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_1_start, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_1_start, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_1_start, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->screen_main_btn_1_start, lv_color_hex(0x0d4b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->screen_main_btn_1_start, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->screen_main_btn_1_start, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(ui->screen_main_btn_1_start, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(ui->screen_main_btn_1_start, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_1_start, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_1_start, &lv_font_AlimamaDongFangDaKai_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_1_start, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_1_start, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_label_1_title
    ui->screen_main_label_1_title = lv_label_create(ui->screen_main_cont_main);
    lv_label_set_text(ui->screen_main_label_1_title, "心肺复苏模型遥控器");
    lv_label_set_long_mode(ui->screen_main_label_1_title, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_main_label_1_title, 45, 50);
    lv_obj_set_size(ui->screen_main_label_1_title, 150, 48);

    //Write style for screen_main_label_1_title, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_label_1_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_label_1_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_label_1_title, lv_color_hex(0xff0027), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_label_1_title, &lv_font_AlimamaDongFangDaKai_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_label_1_title, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_main_label_1_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_main_label_1_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_label_1_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_label_1_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_label_1_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_label_1_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_label_1_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_label_1_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_label_1_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_label_2_connect
    ui->screen_main_label_2_connect = lv_label_create(ui->screen_main_cont_main);
    lv_label_set_text(ui->screen_main_label_2_connect, "连接中...");
    lv_label_set_long_mode(ui->screen_main_label_2_connect, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_main_label_2_connect, 45, 125);
    lv_obj_set_size(ui->screen_main_label_2_connect, 150, 30);
    lv_obj_add_flag(ui->screen_main_label_2_connect, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_main_label_2_connect, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_label_2_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_label_2_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_label_2_connect, lv_color_hex(0x2F35DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_label_2_connect, &lv_font_AlimamaDongFangDaKai_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_label_2_connect, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_main_label_2_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_main_label_2_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_label_2_connect, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_label_2_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_label_2_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_label_2_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_label_2_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_label_2_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_label_2_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_main.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_main);

    //Init events for screen.
    events_init_screen_main(ui);
}






void setup_ui(lvgl_ui_t *ui)
{
    init_scr_del_flag(ui);
    setup_scr_screen_main(ui);
    lv_scr_load(ui->screen_main);
}




// 这个是主页面上的开始按钮
static void screen_main_btn_1_start_event_handler (lv_event_t *e)
{
    LV_LOG_USER("Clicked");
    if (Record.nrf_if_connected) {
        // 已连接，直接切换到控制菜单页面
        ui_load_scr_animation(&guider_lvgl, &guider_lvgl.screen_menu, guider_lvgl.screen_menu_del, &guider_lvgl.screen_main_del, setup_scr_screen_menu, LV_SCR_LOAD_ANIM_NONE, 0, 10, true, true);
        Record.menu_index = 1;
    }
    else {
        // 未连接，隐藏按钮，显示“连接中...”
        lv_obj_add_flag(guider_lvgl.screen_main_btn_1_start, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_lvgl.screen_main_label_2_connect, LV_OBJ_FLAG_HIDDEN);
        Record.nrf_connect_failed = 1;
    }
}



void events_init_screen_main (lvgl_ui_t *ui)
{
    lv_obj_add_event_cb(ui->screen_main_btn_1_start, screen_main_btn_1_start_event_handler, LV_EVENT_PRESSED, ui);
}



