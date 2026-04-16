/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-11-06     18452       the first version
 */
#include "bsp_sys.h"



static bool input_is_locked = false;

/* 锁住整个屏幕输入 */
void lock_screen_input(void)
{
    if(input_is_locked) return;
    input_is_locked = true;

    /* 给当前屏幕加 DISABLED 状态，所有子对象都收不到输入 */
    lv_obj_add_state(lv_scr_act(), LV_STATE_DISABLED);

    /* 如果想让屏幕看起来“灰掉”，可再盖一个半透膜 */
    lv_obj_t *top = lv_layer_top();          /* 全局顶层 */
    static lv_obj_t *mask = NULL;
    if(mask == NULL) {
        mask = lv_obj_create(top);
        lv_obj_set_size(mask, LV_HOR_RES, LV_VER_RES);
        lv_obj_set_style_bg_color(mask, lv_color_hex(0x000000), 0);
        lv_obj_set_style_bg_opa(mask, LV_OPA_30, 0);
        lv_obj_set_style_border_width(mask, 0, 0);
        lv_obj_clear_flag(mask, LV_OBJ_FLAG_CLICKABLE); /* 自己不吃事件 */
    }
    lv_obj_clear_flag(mask, LV_OBJ_FLAG_HIDDEN);
}



/* 解锁 */
void unlock_screen_input(void)
{
    if(!input_is_locked) return;
    input_is_locked = false;

    lv_obj_clear_state(lv_scr_act(), LV_STATE_DISABLED);

    lv_obj_t *top = lv_layer_top();
    lv_obj_t *mask = lv_obj_get_child(top, 0);
    if(mask) lv_obj_add_flag(mask, LV_OBJ_FLAG_HIDDEN);
}







