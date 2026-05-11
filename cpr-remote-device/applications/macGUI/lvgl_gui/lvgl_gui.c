/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-29     Administrator       the first version
 */
#include <macGUI/lvgl_gui/lvgl_gui.h>



void ui_load_scr_animation(lvgl_ui_t *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del)
{
    lv_obj_t * act_scr = lv_scr_act();

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
    if(auto_del) {
        gg_edata_task_clear(act_scr);
    }
#endif
    if (auto_del && is_clean) {
        lv_obj_clean(act_scr);
    }
    if (new_scr_del) {
        setup_scr(ui);
    }
    lv_scr_load_anim(*new_scr, anim_type, time, delay, auto_del);
    *old_scr_del = auto_del;
}



void init_scr_del_flag(lvgl_ui_t *ui)
{

    ui->screen_main_del = true;
    ui->screen_menu_del = true;
    ui->screen_data_del = true;
    ui->screen_setting_del = true;
    ui->screen_operation_del = true;
}


void lv_user_gui_init(void)
{

#if USE_LVGL_TEST_BUTTON
    lv_example_get_started_1();
#endif

#define USE_GUI_GUIDER 1
#if USE_GUI_GUIDER
    extern void setup_ui(lvgl_ui_t *ui);
    setup_ui(&guider_lvgl);
#endif
}



