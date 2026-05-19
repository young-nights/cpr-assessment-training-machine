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

/* Forward declarations */
static void screen_setting_row_plus_event_handler(lv_event_t *e);
static void screen_setting_row_minus_event_handler(lv_event_t *e);

static void screen_setting_btn_back_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lvgl_ui_t *ui = lv_event_get_user_data(e);
        ui_btn_press_feedback(lv_event_get_target(e));
        ui_load_scr_animation(ui, &ui->screen_menu, ui->screen_menu_del, &ui->screen_setting_del, setup_scr_screen_menu, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        Record.menu_index = 1;
    }
}

void setup_scr_screen_setting(lvgl_ui_t *ui)
{
    // ==================== 创建屏幕(screen_setting) ====================
    // 创建一个新屏幕对象,作为 LVGL 的顶级容器(NULL 表示父对象为屏幕)
    ui->screen_setting = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_setting, 240, 320);                    // 设置屏幕尺寸 240x320(常见 TFT 屏幕分辨率)
    lv_obj_set_scrollbar_mode(ui->screen_setting, LV_SCROLLBAR_MODE_OFF); // 关闭滚动条

    // 屏幕主容器样式:透明背景(便于叠加其他层)
    lv_obj_set_style_bg_opa(ui->screen_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    // ==================== 创建主容器 cont_setting ====================
    // 主内容容器,覆盖整个屏幕,用于放置所有控件
    ui->screen_setting_cont_setting = lv_obj_create(ui->screen_setting);
    lv_obj_set_pos(ui->screen_setting_cont_setting, 0, 0);          // 位置 (0,0)
    lv_obj_set_size(ui->screen_setting_cont_setting, 240, 320);     // 大小满屏
    lv_obj_set_scrollbar_mode(ui->screen_setting_cont_setting, LV_SCROLLBAR_MODE_OFF);

    // 主容器样式:白色背景,无边框,无阴影,全填充
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

    /* ====== Page title ====== */
    lv_obj_t *label_title = lv_label_create(ui->screen_setting_cont_setting);
    lv_label_set_text(label_title, "\u53c2\u6570\u8bbe\u7f6e");  /* 参数设置 */
    lv_obj_set_pos(label_title, 40, 12);
    lv_obj_set_size(label_title, 160, 30);
    lv_obj_set_style_text_color(label_title, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label_title, &lv_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(label_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(label_title, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Row 1: Work Time */
    lv_obj_t *row1 = lv_obj_create(ui->screen_setting_cont_setting);
    lv_obj_set_pos(row1, 10, 55);
    lv_obj_set_size(row1, 220, 55);
    lv_obj_clear_flag(row1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(row1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(row1, lv_color_hex(0xF5F7FA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(row1, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(row1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(row1, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(row1, lv_color_hex(0xDDDDDD), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(row1, 120, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(row1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui->screen_setting_btn_1_time_set = lv_btn_create(row1);
    lv_obj_add_flag(ui->screen_setting_btn_1_time_set, LV_OBJ_FLAG_CHECKABLE);
    ui->screen_setting_btn_1_time_set_label = lv_label_create(ui->screen_setting_btn_1_time_set);
    lv_label_set_text(ui->screen_setting_btn_1_time_set_label, "\u5de5\u4f5c\u65f6\u95f4");  /* 工作时间 */
    lv_obj_align(ui->screen_setting_btn_1_time_set_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_setting_btn_1_time_set, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_setting_btn_1_time_set_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_setting_btn_1_time_set, 8, 10);
    lv_obj_set_size(ui->screen_setting_btn_1_time_set, 95, 35);
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_1_time_set, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_setting_btn_1_time_set, lv_color_hex(0x009ea9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_setting_btn_1_time_set, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_btn_1_time_set, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_1_time_set, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_btn_1_time_set, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_btn_1_time_set, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_btn_1_time_set, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_setting_btn_1_time_set, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_setting_btn_1_time_set, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);

    ui->screen_setting_label_time_value = lv_label_create(row1);
    lv_label_set_text(ui->screen_setting_label_time_value, "2 min");
    lv_obj_set_pos(ui->screen_setting_label_time_value, 108, 18);
    lv_obj_set_size(ui->screen_setting_label_time_value, 50, 20);
    lv_obj_set_style_text_color(ui->screen_setting_label_time_value, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_label_time_value, &lv_font_montserratMedium_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_label_time_value, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_setting_label_time_value, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *btn1_minus = lv_btn_create(row1);
    lv_obj_t *btn1_minus_label = lv_label_create(btn1_minus);
    lv_label_set_text(btn1_minus_label, "-");
    lv_obj_align(btn1_minus_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(btn1_minus, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(btn1_minus, 165, 13);
    lv_obj_set_size(btn1_minus, 22, 28);
    lv_obj_set_style_bg_opa(btn1_minus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn1_minus, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn1_minus, lv_color_hex(0x009ea9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(btn1_minus, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn1_minus, lv_color_hex(0x009ea9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(btn1_minus, &lv_font_montserratMedium_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(btn1_minus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *btn1_plus = lv_btn_create(row1);
    lv_obj_t *btn1_plus_label = lv_label_create(btn1_plus);
    lv_label_set_text(btn1_plus_label, "+");
    lv_obj_align(btn1_plus_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(btn1_plus, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(btn1_plus, 192, 13);
    lv_obj_set_size(btn1_plus, 22, 28);
    lv_obj_set_style_bg_opa(btn1_plus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn1_plus, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn1_plus, lv_color_hex(0x009ea9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(btn1_plus, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn1_plus, lv_color_hex(0x009ea9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(btn1_plus, &lv_font_montserratMedium_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(btn1_plus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Row 2: Press Rate */
    lv_obj_t *row2 = lv_obj_create(ui->screen_setting_cont_setting);
    lv_obj_set_pos(row2, 10, 120);
    lv_obj_set_size(row2, 220, 55);
    lv_obj_clear_flag(row2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(row2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(row2, lv_color_hex(0xF5F7FA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(row2, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(row2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(row2, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(row2, lv_color_hex(0xDDDDDD), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(row2, 120, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(row2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui->screen_setting_btn_2_press_rate = lv_btn_create(row2);
    lv_obj_add_flag(ui->screen_setting_btn_2_press_rate, LV_OBJ_FLAG_CHECKABLE);
    ui->screen_setting_btn_2_press_rate_label = lv_label_create(ui->screen_setting_btn_2_press_rate);
    lv_label_set_text(ui->screen_setting_btn_2_press_rate_label, "\u6309\u538b\u8fbe\u6807\u7387");  /* 按压达标率 */
    lv_obj_align(ui->screen_setting_btn_2_press_rate_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_setting_btn_2_press_rate, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_setting_btn_2_press_rate_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_setting_btn_2_press_rate, 8, 10);
    lv_obj_set_size(ui->screen_setting_btn_2_press_rate, 95, 35);
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_2_press_rate, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_setting_btn_2_press_rate, lv_color_hex(0x009ea9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_setting_btn_2_press_rate, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_btn_2_press_rate, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_2_press_rate, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_btn_2_press_rate, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_btn_2_press_rate, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_btn_2_press_rate, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_setting_btn_2_press_rate, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_setting_btn_2_press_rate, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);

    ui->screen_setting_label_press_rate = lv_label_create(row2);
    lv_label_set_text(ui->screen_setting_label_press_rate, "90%");
    lv_obj_set_pos(ui->screen_setting_label_press_rate, 108, 18);
    lv_obj_set_size(ui->screen_setting_label_press_rate, 50, 20);
    lv_obj_set_style_text_color(ui->screen_setting_label_press_rate, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_label_press_rate, &lv_font_montserratMedium_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_label_press_rate, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_setting_label_press_rate, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *btn2_minus = lv_btn_create(row2);
    lv_obj_t *btn2_minus_label = lv_label_create(btn2_minus);
    lv_label_set_text(btn2_minus_label, "-");
    lv_obj_align(btn2_minus_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(btn2_minus, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(btn2_minus, 165, 13);
    lv_obj_set_size(btn2_minus, 22, 28);
    lv_obj_set_style_bg_opa(btn2_minus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn2_minus, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn2_minus, lv_color_hex(0x009ea9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(btn2_minus, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn2_minus, lv_color_hex(0x009ea9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(btn2_minus, &lv_font_montserratMedium_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(btn2_minus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *btn2_plus = lv_btn_create(row2);
    lv_obj_t *btn2_plus_label = lv_label_create(btn2_plus);
    lv_label_set_text(btn2_plus_label, "+");
    lv_obj_align(btn2_plus_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(btn2_plus, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(btn2_plus, 192, 13);
    lv_obj_set_size(btn2_plus, 22, 28);
    lv_obj_set_style_bg_opa(btn2_plus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn2_plus, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn2_plus, lv_color_hex(0x009ea9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(btn2_plus, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn2_plus, lv_color_hex(0x009ea9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(btn2_plus, &lv_font_montserratMedium_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(btn2_plus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Row 3: Air Rate */
    lv_obj_t *row3 = lv_obj_create(ui->screen_setting_cont_setting);
    lv_obj_set_pos(row3, 10, 185);
    lv_obj_set_size(row3, 220, 55);
    lv_obj_clear_flag(row3, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(row3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(row3, lv_color_hex(0xF5F7FA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(row3, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(row3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(row3, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(row3, lv_color_hex(0xDDDDDD), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(row3, 120, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(row3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui->screen_setting_btn_3_air_rate = lv_btn_create(row3);
    lv_obj_add_flag(ui->screen_setting_btn_3_air_rate, LV_OBJ_FLAG_CHECKABLE);
    ui->screen_setting_btn_3_air_rate_label = lv_label_create(ui->screen_setting_btn_3_air_rate);
    lv_label_set_text(ui->screen_setting_btn_3_air_rate_label, "\u6f6e\u6c14\u8fbe\u6807\u7387");  /* 潮气达标率 */
    lv_obj_align(ui->screen_setting_btn_3_air_rate_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_setting_btn_3_air_rate, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_setting_btn_3_air_rate_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_setting_btn_3_air_rate, 8, 10);
    lv_obj_set_size(ui->screen_setting_btn_3_air_rate, 95, 35);
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_3_air_rate, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_setting_btn_3_air_rate, lv_color_hex(0x009ea9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_setting_btn_3_air_rate, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_btn_3_air_rate, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_3_air_rate, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_btn_3_air_rate, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_btn_3_air_rate, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_btn_3_air_rate, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_setting_btn_3_air_rate, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_setting_btn_3_air_rate, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);

    ui->screen_setting_label_air_rate = lv_label_create(row3);
    lv_label_set_text(ui->screen_setting_label_air_rate, "85%");
    lv_obj_set_pos(ui->screen_setting_label_air_rate, 108, 18);
    lv_obj_set_size(ui->screen_setting_label_air_rate, 50, 20);
    lv_obj_set_style_text_color(ui->screen_setting_label_air_rate, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_label_air_rate, &lv_font_montserratMedium_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_label_air_rate, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_setting_label_air_rate, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *btn3_minus = lv_btn_create(row3);
    lv_obj_t *btn3_minus_label = lv_label_create(btn3_minus);
    lv_label_set_text(btn3_minus_label, "-");
    lv_obj_align(btn3_minus_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(btn3_minus, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(btn3_minus, 165, 13);
    lv_obj_set_size(btn3_minus, 22, 28);
    lv_obj_set_style_bg_opa(btn3_minus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn3_minus, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn3_minus, lv_color_hex(0x009ea9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(btn3_minus, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn3_minus, lv_color_hex(0x009ea9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(btn3_minus, &lv_font_montserratMedium_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(btn3_minus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *btn3_plus = lv_btn_create(row3);
    lv_obj_t *btn3_plus_label = lv_label_create(btn3_plus);
    lv_label_set_text(btn3_plus_label, "+");
    lv_obj_align(btn3_plus_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(btn3_plus, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(btn3_plus, 192, 13);
    lv_obj_set_size(btn3_plus, 22, 28);
    lv_obj_set_style_bg_opa(btn3_plus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn3_plus, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn3_plus, lv_color_hex(0x009ea9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(btn3_plus, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn3_plus, lv_color_hex(0x009ea9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(btn3_plus, &lv_font_montserratMedium_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(btn3_plus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Back button at bottom */
    lv_obj_t *btn_back = lv_btn_create(ui->screen_setting_cont_setting);
    lv_obj_t *btn_back_label = lv_label_create(btn_back);
    lv_label_set_text(btn_back_label, "\u8fd4\u56de");  /* 返回 */
    lv_obj_align(btn_back_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(btn_back, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(btn_back_label, LV_PCT(100));
    lv_obj_set_pos(btn_back, 85, 260);
    lv_obj_set_size(btn_back, 70, 32);
    lv_obj_set_style_bg_opa(btn_back, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn_back, lv_color_hex(0x999999), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn_back, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(btn_back, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(btn_back, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(btn_back, lv_color_hex(0x888888), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn_back, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(btn_back, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(btn_back, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Wire back button to return to menu */
    lv_obj_add_event_cb(btn_back, screen_setting_btn_back_event_handler, LV_EVENT_CLICKED, ui);

    /* Per-row +/- button event wiring */
    lv_obj_add_event_cb(btn1_plus, screen_setting_row_plus_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(btn1_minus, screen_setting_row_minus_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(btn2_plus, screen_setting_row_plus_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(btn2_minus, screen_setting_row_minus_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(btn3_plus, screen_setting_row_plus_event_handler, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(btn3_minus, screen_setting_row_minus_event_handler, LV_EVENT_CLICKED, ui);

    // ==================== 自定义代码区域 ====================
    //The custom code of screen_setting.


    // ==================== 更新布局 ====================
    // 强制刷新布局,确保所有控件位置正确
    lv_obj_update_layout(ui->screen_setting);

    // ==================== 初始化事件 ====================
    // 为屏幕绑定事件回调(如按钮点击)
    events_init_screen_setting(ui);
}










//-----------------------------------------------------------------------------------------------------------------------

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
            /* Mutex: clear other buttons */
            lv_obj_clear_state(ui->screen_setting_btn_3_air_rate, LV_STATE_CHECKED);
            lv_obj_clear_state(ui->screen_setting_btn_2_press_rate, LV_STATE_CHECKED);

            Record.setting_mode = 1;
            Flag.work_time_set = 1;
        }
        else
        {
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
            /* Mutex: clear other buttons */
            lv_obj_clear_state(ui->screen_setting_btn_1_time_set, LV_STATE_CHECKED);
            lv_obj_clear_state(ui->screen_setting_btn_3_air_rate, LV_STATE_CHECKED);

            Record.setting_mode = 1;
            Flag.press_rate_set = 1;
        }
        else
        {
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
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lvgl_ui_t *ui = (lvgl_ui_t *)lv_event_get_user_data(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        if(lv_obj_has_state(btn, LV_STATE_CHECKED))
        {
            /* Mutex: clear other buttons */
            lv_obj_clear_state(ui->screen_setting_btn_1_time_set, LV_STATE_CHECKED);
            lv_obj_clear_state(ui->screen_setting_btn_2_press_rate, LV_STATE_CHECKED);

            Record.setting_mode = 1;
        }
        else
        {
            Record.setting_mode = 0;
        }

        break;
    }
    default:
        break;
    }
}


/* Per-row + button handler: determine which row by checking which label shares the same parent */
static void screen_setting_row_plus_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *row = lv_obj_get_parent(btn);

    if (row == lv_obj_get_parent(ui->screen_setting_label_time_value)) {
        Record.set_work_time += 10;
        if (Record.set_work_time > 990) Record.set_work_time = 990;
        lv_label_set_text_fmt(ui->screen_setting_label_time_value, "%ds", Record.set_work_time);
    } else if (row == lv_obj_get_parent(ui->screen_setting_label_press_rate)) {
        Record.set_press_rate += 1;
        if (Record.set_press_rate >= 100) Record.set_press_rate = 100;
        lv_label_set_text_fmt(ui->screen_setting_label_press_rate, "%d%%", Record.set_press_rate);
    } else if (row == lv_obj_get_parent(ui->screen_setting_label_air_rate)) {
        Record.set_air_rate += 1;
        if (Record.set_air_rate > 100) Record.set_air_rate = 100;
        lv_label_set_text_fmt(ui->screen_setting_label_air_rate, "%d%%", Record.set_air_rate);
    }
}

/* Per-row - button handler */
static void screen_setting_row_minus_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    lvgl_ui_t *ui = lv_event_get_user_data(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *row = lv_obj_get_parent(btn);

    if (row == lv_obj_get_parent(ui->screen_setting_label_time_value)) {
        Record.set_work_time -= 10;
        if (Record.set_work_time <= 10) Record.set_work_time = 10;
        lv_label_set_text_fmt(ui->screen_setting_label_time_value, "%ds", Record.set_work_time);
    } else if (row == lv_obj_get_parent(ui->screen_setting_label_press_rate)) {
        Record.set_press_rate -= 1;
        if (Record.set_press_rate <= 1) Record.set_press_rate = 1;
        lv_label_set_text_fmt(ui->screen_setting_label_press_rate, "%d%%", Record.set_press_rate);
    } else if (row == lv_obj_get_parent(ui->screen_setting_label_air_rate)) {
        Record.set_air_rate -= 1;
        if (Record.set_air_rate <= 1) Record.set_air_rate = 1;
        lv_label_set_text_fmt(ui->screen_setting_label_air_rate, "%d%%", Record.set_air_rate);
    }
}

void events_init_screen_setting (lvgl_ui_t *ui)
{
    /* Checkable button callbacks (mutual exclusion) */
    lv_obj_add_event_cb(ui->screen_setting_btn_1_time_set, screen_setting_btn_1_time_set_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_setting_btn_2_press_rate, screen_setting_btn_2_press_rate_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_setting_btn_3_air_rate, screen_setting_btn_3_air_rate_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    /* Note: per-row +/- and back button callbacks are registered inline in setup_scr_screen_setting */
}












