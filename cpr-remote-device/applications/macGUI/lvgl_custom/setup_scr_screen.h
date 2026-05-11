/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-08-29     18452       the first version
 */
#ifndef APPLICATIONS_MACGUI_LVGL_CUSTOM_SETUP_SCR_SCREEN_H_
#define APPLICATIONS_MACGUI_LVGL_CUSTOM_SETUP_SCR_SCREEN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef struct
{
    lv_obj_t *screen_main;
    bool screen_main_del;
    lv_obj_t *screen_main_cont_main;
    lv_obj_t *screen_main_btn_1_start;
    lv_obj_t *screen_main_btn_1_start_label;
    lv_obj_t *screen_main_label_1_title;
    lv_obj_t *screen_main_label_2_connect;
    lv_obj_t *screen_menu;
    bool screen_menu_del;
    lv_obj_t *screen_menu_cont_menu;
    lv_obj_t *screen_menu_img_9_switch;
    lv_obj_t *screen_menu_img_8_printer;
    lv_obj_t *screen_menu_img_7_setting;
    lv_obj_t *screen_menu_img_6_instruction;
    lv_obj_t *screen_menu_img_5_operation;
    lv_obj_t *screen_menu_img_4_score;
    lv_obj_t *screen_menu_img_3_train;
    lv_obj_t *screen_menu_img_2_competation;
    lv_obj_t *screen_menu_img_1_assess;
    lv_obj_t *screen_menu_label_9_switch;
    lv_obj_t *screen_menu_label_8_printer;
    lv_obj_t *screen_menu_label_7_settings;
    lv_obj_t *screen_menu_label_6_instructions;
    lv_obj_t *screen_menu_label_5_operation;
    lv_obj_t *screen_menu_label_4_score;
    lv_obj_t *screen_menu_label_3_train;
    lv_obj_t *screen_menu_label_2_competation;
    lv_obj_t *screen_menu_label_1_assess;
    lv_obj_t *screen_menu_label_printing;
    lv_obj_t *screen_data;
    bool screen_data_del;
    lv_obj_t *screen_data_cont_data;
    lv_obj_t *screen_data_circle_1;
    lv_obj_t *screen_data_circle_2;
    lv_obj_t *screen_data_label_1_model;
    lv_obj_t *screen_data_circle_3;
    lv_obj_t *screen_data_circle_4;
    lv_obj_t *screen_data_circle_5;
    lv_obj_t *screen_data_circle_6;
    lv_obj_t *screen_data_circle_7;
    lv_obj_t *screen_data_bar_left_grid;   // 左边 50 个小格的父容器
    lv_obj_t *screen_data_bar_right_grid;  // 右边 50 个小格的父容器
    lv_obj_t *screen_setting;
    bool screen_setting_del;
    lv_obj_t *screen_setting_cont_setting;
    lv_obj_t *screen_setting_btn_plus;
    lv_obj_t *screen_setting_btn_plus_label;
    lv_obj_t *screen_setting_btn_minus;
    lv_obj_t *screen_setting_btn_minus_label;
    lv_obj_t *screen_setting_btn_3_air_rate;
    lv_obj_t *screen_setting_btn_3_air_rate_label;
    lv_obj_t *screen_setting_btn_2_press_rate;
    lv_obj_t *screen_setting_btn_2_press_rate_label;
    lv_obj_t *screen_setting_btn_1_time_set;
    lv_obj_t *screen_setting_btn_1_time_set_label;
    lv_obj_t *screen_setting_label_air_rate;
    lv_obj_t *screen_setting_label_press_rate;
    lv_obj_t *screen_setting_label_time_value;
    lv_obj_t *screen_setting_label_set_title;
    lv_obj_t *screen_operation;
    bool screen_operation_del;
    lv_obj_t *screen_operation_cont_operation;
    lv_obj_t *screen_operation_btn_opera_11;
    lv_obj_t *screen_operation_btn_opera_11_label;
    lv_obj_t *screen_operation_btn_opera_10;
    lv_obj_t *screen_operation_btn_opera_10_label;
    lv_obj_t *screen_operation_btn_opera_9;
    lv_obj_t *screen_operation_btn_opera_9_label;
    lv_obj_t *screen_operation_btn_opera_8;
    lv_obj_t *screen_operation_btn_opera_8_label;
    lv_obj_t *screen_operation_btn_opera_7;
    lv_obj_t *screen_operation_btn_opera_7_label;
    lv_obj_t *screen_operation_btn_opera_6;
    lv_obj_t *screen_operation_btn_opera_6_label;
    lv_obj_t *screen_operation_btn_opera_5;
    lv_obj_t *screen_operation_btn_opera_5_label;
    lv_obj_t *screen_operation_btn_opera_4;
    lv_obj_t *screen_operation_btn_opera_4_label;
    lv_obj_t *screen_operation_btn_opera_3;
    lv_obj_t *screen_operation_btn_opera_3_label;
    lv_obj_t *screen_operation_btn_opera_2;
    lv_obj_t *screen_operation_btn_opera_2_label;
    lv_obj_t *screen_operation_btn_opera_1;
    lv_obj_t *screen_operation_btn_opera_1_label;


}lvgl_ui_t;
extern lvgl_ui_t guider_lvgl;

typedef void (*ui_setup_scr_t)(lvgl_ui_t * ui);




/* lvgl_gui.c文件 */
void ui_load_scr_animation(lvgl_ui_t *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);
void init_scr_del_flag(lvgl_ui_t *ui);
void lv_user_gui_init(void);


/* setup_scr_screen_data.c文件 */
/* 圆环颜色定义 */
#define CIRCLE_COLOR_WHITE   0
#define CIRCLE_COLOR_RED     1
#define CIRCLE_COLOR_YELLOW  2
#define CIRCLE_COLOR_GREEN   3
void update_circle_color(lv_obj_t *circle, int color_type);
void update_circle_by_index(lvgl_ui_t *ui, int index, int color_type);
void cleanup_data_screen_animation(void);

/* setup_scr_screen.c文件 */
void setup_scr_screen_main(lvgl_ui_t *ui);
void setup_scr_screen_menu(lvgl_ui_t *ui);
void setup_scr_screen_data(lvgl_ui_t *ui);
void setup_scr_screen_setting(lvgl_ui_t *ui);
void setup_scr_screen_operation(lvgl_ui_t *ui);


/* lvgl_events.c文件 */
void events_init_screen_main (lvgl_ui_t *ui);
void events_init_screen_menu (lvgl_ui_t *ui);
void events_init_screen_setting (lvgl_ui_t *ui);
void events_init_screen_operation(lvgl_ui_t *ui);

/* gui_custom.c文件 */
void lock_screen_input(void);




LV_IMG_DECLARE(_competation_alpha_50x50);
LV_IMG_DECLARE(_train_alpha_50x50);
LV_IMG_DECLARE(_score_alpha_50x50);
LV_IMG_DECLARE(_setting_alpha_50x50);
LV_IMG_DECLARE(_specification_alpha_50x50);
LV_IMG_DECLARE(_assess_alpha_50x50);
LV_IMG_DECLARE(_printer_alpha_50x50);
LV_IMG_DECLARE(_switch_alpha_50x50);
LV_IMG_DECLARE(_operation_alpha_50x50);

LV_FONT_DECLARE(lv_font_AlimamaDongFangDaKai_18)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_AlimamaDongFangDaKai_24)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_16)
LV_FONT_DECLARE(lv_font_AlimamaDongFangDaKai_15)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_20)
LV_FONT_DECLARE(lv_font_montserratMedium_40)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_26)
LV_FONT_DECLARE(lv_font_AlimamaDongFangDaKai_21)
LV_FONT_DECLARE(lv_font_AlimamaDongFangDaKai_16)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_12)




#ifdef __cplusplus
}
#endif


#endif /* APPLICATIONS_MACGUI_LVGL_CUSTOM_SETUP_SCR_SCREEN_H_ */
