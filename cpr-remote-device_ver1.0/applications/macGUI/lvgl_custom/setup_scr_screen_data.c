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




void update_left_bar(lvgl_ui_t *ui, int percentage);
void update_right_bar(lvgl_ui_t *ui, int percentage);
lv_color_t get_gradient_color(int percentage);

void setup_scr_screen_data(lvgl_ui_t *ui)
{
    // ==================== 创建屏幕 ====================
    ui->screen_data = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_data, 240, 320);
    lv_obj_set_scrollbar_mode(ui->screen_data, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(ui->screen_data, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ==================== 白色背景容器 ====================
    ui->screen_data_cont_data = lv_obj_create(ui->screen_data);
    lv_obj_set_pos(ui->screen_data_cont_data, 0, 0);
    lv_obj_set_size(ui->screen_data_cont_data, 240, 320);
    lv_obj_set_scrollbar_mode(ui->screen_data_cont_data, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(ui->screen_data_cont_data, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_data_cont_data, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_data_cont_data, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_data_cont_data, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_data_cont_data, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ==================== 标题 ====================
    ui->screen_data_label_1_model = lv_label_create(ui->screen_data_cont_data);
    lv_label_set_text(ui->screen_data_label_1_model, "心肺复苏操作模型");
    lv_obj_set_pos(ui->screen_data_label_1_model, 40, 12);
    lv_obj_set_size(ui->screen_data_label_1_model, 161, 40);
    lv_obj_set_style_text_color(ui->screen_data_label_1_model, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_data_label_1_model, &lv_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_data_label_1_model, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_data_label_1_model, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_data_label_1_model, 8, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ==================== 7 个黑色圆环表盘 ====================
    // 定义一个宏，用于设置圆圈的基本样式
#define SET_CIRCLE_STYLE(obj) \
    /* 1. 设置为圆形 */ \
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, LV_PART_MAIN | LV_STATE_DEFAULT); \
    \
    /* 2. 设置白色填充色（与父容器背景色一致） */ \
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT); \
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT); \
    \
    /* 3. 设置黑色边界（边框） */ \
    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT); /* 边框宽度 */ \
    lv_obj_set_style_border_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT); /* 黑色边界 */ \
    \
    /* 4. 移除阴影 */ \
    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT)


    ui->screen_data_circle_1 = lv_obj_create(ui->screen_data_cont_data);
    lv_obj_set_pos(ui->screen_data_circle_1, 64, 165);
    lv_obj_set_size(ui->screen_data_circle_1, 25, 25);
    SET_CIRCLE_STYLE(ui->screen_data_circle_1);

    ui->screen_data_circle_2 = lv_obj_create(ui->screen_data_cont_data);
    lv_obj_set_pos(ui->screen_data_circle_2, 150, 164);
    lv_obj_set_size(ui->screen_data_circle_2, 25, 25);
    SET_CIRCLE_STYLE(ui->screen_data_circle_2);

    ui->screen_data_circle_3 = lv_obj_create(ui->screen_data_cont_data);
    lv_obj_set_pos(ui->screen_data_circle_3, 108, 163);
    lv_obj_set_size(ui->screen_data_circle_3, 25, 25);
    SET_CIRCLE_STYLE(ui->screen_data_circle_3);

    ui->screen_data_circle_4 = lv_obj_create(ui->screen_data_cont_data);
    lv_obj_set_pos(ui->screen_data_circle_4, 107, 117);
    lv_obj_set_size(ui->screen_data_circle_4, 25, 25);
    SET_CIRCLE_STYLE(ui->screen_data_circle_4);

    ui->screen_data_circle_5 = lv_obj_create(ui->screen_data_cont_data);
    lv_obj_set_pos(ui->screen_data_circle_5, 108, 208);
    lv_obj_set_size(ui->screen_data_circle_5, 25, 25);
    SET_CIRCLE_STYLE(ui->screen_data_circle_5);

    ui->screen_data_circle_6 = lv_obj_create(ui->screen_data_cont_data);
    lv_obj_set_pos(ui->screen_data_circle_6, 108, 73);
    lv_obj_set_size(ui->screen_data_circle_6, 25, 25);
    SET_CIRCLE_STYLE(ui->screen_data_circle_6);

    ui->screen_data_circle_7 = lv_obj_create(ui->screen_data_cont_data);
    lv_obj_set_pos(ui->screen_data_circle_7, 108, 253);
    lv_obj_set_size(ui->screen_data_circle_7, 25, 25);
    SET_CIRCLE_STYLE(ui->screen_data_circle_7);


    // ==================== 充电格样式进度条（左右各40格，正好240px）===================
    const int GRID_COUNT    = 40;      // 40格
    const int GRID_HEIGHT   = 4;       // 每格高4px
    const int GRID_GAP      = 2;       // 间隙2px → 每格占6px
    const int BAR_WIDTH     = 30;

    // ---- 左边进度条（从底部向上点亮）----
    ui->screen_data_bar_left_grid = lv_obj_create(ui->screen_data_cont_data);
    lv_obj_set_pos(ui->screen_data_bar_left_grid, 10, 60);
    lv_obj_set_size(ui->screen_data_bar_left_grid, BAR_WIDTH, (GRID_COUNT * (GRID_HEIGHT + GRID_GAP) + 5));
    lv_obj_set_style_bg_opa(ui->screen_data_bar_left_grid, 0, 0);
    lv_obj_set_style_border_width(ui->screen_data_bar_left_grid, 1, 0);
    lv_obj_set_style_border_color(ui->screen_data_bar_left_grid, lv_color_hex(0x000000), 0);
    lv_obj_clear_flag(ui->screen_data_bar_left_grid, LV_OBJ_FLAG_SCROLLABLE);

    // ---- 右边进度条（从顶部向下点亮）----
    ui->screen_data_bar_right_grid = lv_obj_create(ui->screen_data_cont_data);
    lv_obj_set_pos(ui->screen_data_bar_right_grid, 230 - BAR_WIDTH, 60);
    lv_obj_set_size(ui->screen_data_bar_right_grid, BAR_WIDTH, (GRID_COUNT * (GRID_HEIGHT + GRID_GAP) + 5));
    lv_obj_set_style_bg_opa(ui->screen_data_bar_right_grid, 0, 0);
    lv_obj_set_style_border_width(ui->screen_data_bar_right_grid, 1, 0);
    lv_obj_set_style_border_color(ui->screen_data_bar_right_grid, lv_color_hex(0x000000), 0);
    lv_obj_clear_flag(ui->screen_data_bar_right_grid, LV_OBJ_FLAG_SCROLLABLE);


    // 创建40个小格（共用一套循环，左右都创建）
    for (int side = 0; side < 2; side++) {
        lv_obj_t *parent = side ? ui->screen_data_bar_right_grid : ui->screen_data_bar_left_grid;

        for (int i = 0; i < GRID_COUNT; i++) {
            lv_obj_t *cell = lv_obj_create(parent);
            lv_obj_set_size(cell, BAR_WIDTH - 4, GRID_HEIGHT);
            lv_obj_set_pos(cell, 2, i * (GRID_HEIGHT + GRID_GAP) + 1);
            lv_obj_set_style_radius(cell, 2, 0);
            lv_obj_set_style_border_width(cell, 0, 0);
            lv_obj_set_style_shadow_width(cell, 0, 0);

            // 默认未点亮：深灰不透明
            lv_obj_set_style_bg_color(cell, lv_color_hex(0x444444), 0);
            lv_obj_set_style_bg_opa(cell, 255, 0);

            // 存格子序号，方便后面快速取（可选）
            lv_obj_set_user_data(cell, (void*)(intptr_t)i);
        }
    }

    // ==================== 初始化为0% ====================
//    update_left_bar(ui, 100);
//    update_right_bar(ui, 50);

    // ==================== 更新布局 ====================
    lv_obj_update_layout(ui->screen_data);
}




// 假设这是您的颜色映射函数
lv_color_t get_gradient_color(int percentage)
{
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;

    // 定义关键颜色点 (R, G, B)
    // 浅绿 (0%) - R:0, G:120, B:0 (深草绿)
    // 深绿 (50%) - R:0, G:50, B:0 (墨绿)
    // 黄色 (75%) - R:160, G:120, B:0 (暗金/褐黄)
    // 红色 (100%) - R:140, G:0, B:0 (深红/酒红)

    if (percentage <= 50) {
        // 0% (深草绿) -> 50% (墨绿)
        float factor = (float)percentage / 50.0f; // 0.0 -> 1.0

        int r = (int)(0 - (0 - 0) * factor);
        int g = (int)(120 - (120 - 50) * factor);
        int b = (int)(0 - (0 - 0) * factor);
        return lv_color_make(r, g, b);
    } else if (percentage <= 75) {
        // 50% (墨绿) -> 75% (暗金)
        float factor = ((float)percentage - 50.0f) / 25.0f; // 0.0 -> 1.0

        int r = (int)(0 + (160 - 0) * factor);
        int g = (int)(50 + (120 - 50) * factor);
        int b = (int)(0 + (0 - 0) * factor); // 保持 B 不变
        return lv_color_make(r, g, b);
    } else { // 75% 到 100%
        // 75% (暗金) -> 100% (深红)
        float factor = ((float)percentage - 75.0f) / 25.0f; // 0.0 -> 1.0

        int r = (int)(160 + (140 - 160) * factor);
        int g = (int)(120 - (120 - 0) * factor);
        int b = (int)(0 + (0 - 0) * factor); // 保持 B 不变
        return lv_color_make(r, g, b);
    }
}




#define GRID_COUNT      40
#define GRID_HEIGHT     4
#define GRID_GAP        2
#define BAR_WIDTH       30
#define BAR_TOTAL_H     (GRID_COUNT * (GRID_HEIGHT + GRID_GAP))

typedef struct {
    int  target_filled;     // 目标格子数 (0~40)
    int  current_filled;    // 当前格子数
    bool anim_running;
    lv_timer_t *timer;
} bar_anim_t;

static bar_anim_t anim_left  = {0};
static bar_anim_t anim_right = {0};



/*========================== 5. 单格呼吸灯动画 ==========================*/
/**
 * @brief 单元格点亮动画效果
 * @param cell 要执行动画的单元格对象
 * 
 * 功能说明：
 * 1. 先将缩放恢复到100%（256 = 100%）
 * 2. 设置背景不透明度动画（保持255，确保完全显示）
 * 3. 执行缩放弹跳动画：从100%放大到110%，然后回弹到100%
 */
static void cell_light_up(lv_obj_t *cell)
{
    lv_obj_set_style_transform_zoom(cell, 256, 0);  // 恢复 100%

    // 第一个动画：背景不透明度动画（确保完全显示）
    lv_anim_t a;
    lv_anim_init(&a);
    // 把动画绑定到cell对象上
    lv_anim_set_var(&a, cell);
    lv_anim_set_values(&a, 255, 255);
    // 动画执行时间200ms
    lv_anim_set_time(&a, 200);
    // 设置动画执行时的回调函数
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_style_bg_opa);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);

    lv_anim_init(&a);
    lv_anim_set_var(&a, cell);
    lv_anim_set_values(&a, 256, 282);   // 110%
    lv_anim_set_time(&a, 120);
    lv_anim_set_repeat_count(&a, 1);
    lv_anim_set_playback_time(&a, 120);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_style_transform_zoom);
    lv_anim_start(&a);
}


static void cell_light_down(lv_obj_t *cell)
{

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, cell);
    lv_anim_set_values(&a, 255, 255);
    lv_anim_set_time(&a, 180);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_style_bg_opa);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_start(&a);
}


/*========================== 左边条定时器（兼容 v8.3） ==========================*/
static void left_timer_cb(lv_timer_t *t)
{
    bar_anim_t *a = (bar_anim_t *)t->user_data;
    lvgl_ui_t *ui = &guider_lvgl;        // 直接用全局变量

    if (a->current_filled == a->target_filled) {
        a->anim_running = false;
        lv_timer_del(t);
        return;
    }

    int step = (a->current_filled < a->target_filled) ? 1 : -1;
    int idx  = a->current_filled;
    a->current_filled += step;

    lv_obj_t *cell = lv_obj_get_child(ui->screen_data_bar_left_grid, GRID_COUNT - 1 - idx);

    if (step > 0) {
        int pct = (idx + 1) * 100 / GRID_COUNT;
        lv_obj_set_style_bg_color(cell, get_gradient_color(pct), 0);
        cell_light_up(cell);
    } else {
        lv_obj_set_style_bg_color(cell, lv_color_hex(0x444444), 0);
        cell_light_down(cell);
    }
}

/*========================== 右边条定时器（兼容 v8.3） ==========================*/
static void right_timer_cb(lv_timer_t *t)
{
    bar_anim_t *a = (bar_anim_t *)t->user_data;
    lvgl_ui_t *ui = &guider_lvgl;        // 直接用全局变量

    if (a->current_filled == a->target_filled) {
        a->anim_running = false;
        lv_timer_del(t);
        return;
    }

    int step = (a->current_filled < a->target_filled) ? 1 : -1;
    int idx  = a->current_filled;
    a->current_filled += step;

    lv_obj_t *cell = lv_obj_get_child(ui->screen_data_bar_right_grid, idx);

    if (step > 0) {
        int pct = (GRID_COUNT - idx) * 100 / GRID_COUNT;
        lv_obj_set_style_bg_color(cell, get_gradient_color(pct), 0);
        cell_light_up(cell);
    } else {
        lv_obj_set_style_bg_color(cell, lv_color_hex(0x444444), 0);
        cell_light_down(cell);
    }
}


/*========================== 8. 对外更新接口（左右完全独立！） ==========================*/
/* 更新左边条：按压深度（0~100）*/
void update_left_bar(lvgl_ui_t *ui, int percentage)
{
    if (percentage < 0)   percentage = 0;
    if (percentage > 100) percentage = 100;
    int target = percentage * GRID_COUNT / 100;

    anim_left.target_filled = target;

    int diff = abs(anim_left.target_filled - anim_left.current_filled);
    int period = (diff <= 5) ? 80 : (diff <= 15) ? 50 : 25;

    if (anim_left.anim_running) {
        lv_timer_set_period(anim_left.timer, period);
    } else {
        if (anim_left.current_filled != anim_left.target_filled) {
            anim_left.anim_running = true;
            anim_left.timer = lv_timer_create(left_timer_cb, period, &anim_left);
        }
    }
}

/* 更新右边条：按压频率（0~100）*/
void update_right_bar(lvgl_ui_t *ui, int percentage)
{
    if (percentage < 0)   percentage = 0;
    if (percentage > 100) percentage = 100;
    int target = percentage * GRID_COUNT / 100;

    anim_right.target_filled = target;

    int diff = abs(anim_right.target_filled - anim_right.current_filled);
    int period = (diff <= 5) ? 80 : (diff <= 15) ? 50 : 25;

    if (anim_right.anim_running) {
        lv_timer_set_period(anim_right.timer, period);
    } else {
        if (anim_right.current_filled != anim_right.target_filled) {
            anim_right.anim_running = true;
            anim_right.timer = lv_timer_create(right_timer_cb, period, &anim_right);
        }
    }
}

/*========================== 10. 圆环颜色更新接口 ==========================*/
void update_circle_color(lv_obj_t *circle, int color_type)
{
    if (!circle) return;

    lv_color_t color;
    switch(color_type) {
        case CIRCLE_COLOR_RED:    color = lv_color_hex(0xFF0000); break; // 纯红
        case CIRCLE_COLOR_YELLOW: color = lv_color_hex(0xFFFF00); break; // 纯黄
        case CIRCLE_COLOR_GREEN:  color = lv_color_hex(0x00FF00); break; // 纯绿
        case CIRCLE_COLOR_WHITE:
        default:                  color = lv_color_hex(0xFFFFFF); break; // 纯白
    }
    lv_obj_set_style_bg_color(circle, color, LV_PART_MAIN | LV_STATE_DEFAULT);
    
    // 如果是白色，确保不透明度是255（背景也是白的）
    // 如果是彩色，也确保不透明度255
    lv_obj_set_style_bg_opa(circle, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void update_circle_by_index(lvgl_ui_t *ui, int index, int color_type)
{
    lv_obj_t *circle = NULL;
    switch(index) {
        case 1: circle = ui->screen_data_circle_1; break;
        case 2: circle = ui->screen_data_circle_2; break;
        case 3: circle = ui->screen_data_circle_3; break;
        case 4: circle = ui->screen_data_circle_4; break;
        case 5: circle = ui->screen_data_circle_5; break;
        case 6: circle = ui->screen_data_circle_6; break;
        case 7: circle = ui->screen_data_circle_7; break;
        default: return; // 索引越界
    }
    update_circle_color(circle, color_type);
}



void cleanup_data_screen_animation(void)
{
    // 停止左条动画
    if (anim_left.anim_running && anim_left.timer != NULL) {
        lv_timer_del(anim_left.timer);
        anim_left.timer = NULL;
        anim_left.anim_running = false;
        anim_left.current_filled = 0;  // 可选：复位
    }

    // 停止右条动画
    if (anim_right.anim_running && anim_right.timer != NULL) {
        lv_timer_del(anim_right.timer);
        anim_right.timer = NULL;
        anim_right.anim_running = false;
        anim_right.current_filled = 0;
    }
}
