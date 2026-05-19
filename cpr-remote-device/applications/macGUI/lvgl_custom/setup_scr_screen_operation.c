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

/* Forward declarations */
static void screen_operation_btn_back_event_handler(lv_event_t *e);


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

    // ==================== Page title ====================
    lv_obj_t *label_title = lv_label_create(ui->screen_operation_cont_operation);
    lv_label_set_text(label_title, "\u529f\u80fd\u64cd\u4f5c");  /* 功能操作 */
    lv_obj_set_pos(label_title, 40, 12);
    lv_obj_set_size(label_title, 160, 25);
    lv_obj_set_style_text_color(label_title, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label_title, &lv_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(label_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(label_title, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Unified button style macro */
    #define SETUP_OP_BTN(btn, label, x, y, text) \
        btn = lv_btn_create(ui->screen_operation_cont_operation); \
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE); \
        label = lv_label_create(btn); \
        lv_label_set_text(label, text); \
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP); \
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0); \
        lv_obj_set_style_pad_all(btn, 0, LV_STATE_DEFAULT); \
        lv_obj_set_width(label, LV_PCT(100)); \
        lv_obj_set_pos(btn, x, y); \
        lv_obj_set_size(btn, 100, 44); \
        lv_obj_set_style_bg_opa(btn, 255, LV_PART_MAIN | LV_STATE_DEFAULT); \
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT); \
        lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT); \
        lv_obj_set_style_radius(btn, 8, LV_PART_MAIN | LV_STATE_DEFAULT); \
        lv_obj_set_style_shadow_width(btn, 2, LV_PART_MAIN | LV_STATE_DEFAULT); \
        lv_obj_set_style_shadow_color(btn, lv_color_hex(0x1565C0), LV_PART_MAIN | LV_STATE_DEFAULT); \
        lv_obj_set_style_shadow_opa(btn, 180, LV_PART_MAIN | LV_STATE_DEFAULT); \
        lv_obj_set_style_shadow_ofs_y(btn, 1, LV_PART_MAIN | LV_STATE_DEFAULT); \
        lv_obj_set_style_text_color(btn, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT); \
        lv_obj_set_style_text_font(btn, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT); \
        lv_obj_set_style_text_opa(btn, 255, LV_PART_MAIN | LV_STATE_DEFAULT); \
        lv_obj_set_style_text_align(btn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT); \
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xFF5722), LV_PART_MAIN | LV_STATE_CHECKED); \
        lv_obj_set_style_text_color(btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED); \
        lv_obj_set_style_shadow_color(btn, lv_color_hex(0xD84315), LV_PART_MAIN | LV_STATE_CHECKED); \
        lv_obj_set_style_shadow_width(btn, 6, LV_PART_MAIN | LV_STATE_CHECKED);

    int col_l = 10;   /* left column x */
    int col_r = 130;   /* right column x */
    int row1 = 48;     /* first row y */
    int row_gap = 52;   /* gap between rows */

    /* Row 1: 瞳孔检查 / 呼吸检查 */
    SETUP_OP_BTN(ui->screen_operation_btn_opera_11, ui->screen_operation_btn_opera_11_label,
                 col_r, row1, "\u77b3\u5b54\u68c0\u67e5")  /* 瞳孔检查 */
    SETUP_OP_BTN(ui->screen_operation_btn_opera_10, ui->screen_operation_btn_opera_10_label,
                 col_l, row1, "\u547c\u5438\u68c0\u67e5")  /* 呼吸检查 */

    /* Row 2: 脉搏检查 / 急救呼吸 */
    SETUP_OP_BTN(ui->screen_operation_btn_opera_9, ui->screen_operation_btn_opera_9_label,
                 col_r, row1 + row_gap, "\u8109\u640f\u68c0\u67e5")  /* 脉搏检查 */
    SETUP_OP_BTN(ui->screen_operation_btn_opera_8, ui->screen_operation_btn_opera_8_label,
                 col_l, row1 + row_gap, "\u6025\u6551\u547c\u5438")  /* 急救呼吸 */

    /* Row 3: 气道开放 / 心脏按压 */
    SETUP_OP_BTN(ui->screen_operation_btn_opera_7, ui->screen_operation_btn_opera_7_label,
                 col_r, row1 + row_gap * 2, "\u6c14\u9053\u5f00\u653e")  /* 气道开放 */
    SETUP_OP_BTN(ui->screen_operation_btn_opera_6, ui->screen_operation_btn_opera_6_label,
                 col_l, row1 + row_gap * 2, "\u5fc3\u810f\u6309\u538b")  /* 心脏按压 */

    /* Row 4: 脉搏有 / 脉搏无 */
    SETUP_OP_BTN(ui->screen_operation_btn_opera_4, ui->screen_operation_btn_opera_4_label,
                 col_r, row1 + row_gap * 3, "\u8109\u640f\u6709")  /* 脉搏有 */
    SETUP_OP_BTN(ui->screen_operation_btn_opera_5, ui->screen_operation_btn_opera_5_label,
                 col_l, row1 + row_gap * 3, "\u8109\u640f\u65e0")  /* 脉搏无 */

    /* Row 5: 瞳孔放大 / 瞳孔缩小 */
    SETUP_OP_BTN(ui->screen_operation_btn_opera_2, ui->screen_operation_btn_opera_2_label,
                 col_r, row1 + row_gap * 4, "\u77b3\u5b54\u653e\u5927")  /* 瞳孔放大 */
    SETUP_OP_BTN(ui->screen_operation_btn_opera_3, ui->screen_operation_btn_opera_3_label,
                 col_l, row1 + row_gap * 4, "\u77b3\u5b54\u7f29\u5c0f")  /* 瞳孔缩小 */

    /* Row 6: 瞳孔正常 (single, centered) */
    SETUP_OP_BTN(ui->screen_operation_btn_opera_1, ui->screen_operation_btn_opera_1_label,
                 70, row1 + row_gap * 5, "\u77b3\u5b54\u6b63\u5e38")  /* 瞳孔正常 */
    lv_obj_set_size(ui->screen_operation_btn_opera_1, 100, 44);

    /* Back button */
    lv_obj_t *btn_back = lv_btn_create(ui->screen_operation_cont_operation);
    lv_obj_t *btn_back_label = lv_label_create(btn_back);
    lv_label_set_text(btn_back_label, "\u8fd4\u56de");  /* 返回 */
    lv_obj_align(btn_back_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(btn_back, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(btn_back_label, LV_PCT(100));
    lv_obj_set_pos(btn_back, 85, 300);
    lv_obj_set_size(btn_back, 70, 28);
    lv_obj_set_style_bg_opa(btn_back, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn_back, lv_color_hex(0x999999), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn_back, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(btn_back, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(btn_back, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(btn_back, lv_color_hex(0x888888), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn_back, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(btn_back, &lv_font_AlimamaDongFangDaKai_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(btn_back, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(btn_back, screen_operation_btn_back_event_handler, LV_EVENT_CLICKED, ui);

    #undef SETUP_OP_BTN

    // ==================== 自定义代码区域 ====================
    //The custom code of screen_operation.


    // ==================== 更新布局 ====================
    // 强制刷新布局，确保所有控件位置正确
    lv_obj_update_layout(ui->screen_operation);

    // ==================== 初始化事件 ====================
    // 为屏幕绑定事件回调（如按钮点击）
    events_init_screen_operation(ui);
}


static void screen_operation_btn_back_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lvgl_ui_t *ui = lv_event_get_user_data(e);
        ui_btn_press_feedback(lv_event_get_target(e));
        ui_load_scr_animation(ui, &ui->screen_menu, ui->screen_menu_del, &ui->screen_operation_del, setup_scr_screen_menu, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        Record.menu_index = 1;
    }
}


static void screen_operation_btn_opera_11_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
        ui_btn_press_feedback(lv_event_get_target(e));
        // TODO: Send pupil check command via NRF24 to Mainboard
        LOG_I("Pupil check button pressed");
        break;
    default:
        break;
    }
}

static void screen_operation_btn_opera_10_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
        ui_btn_press_feedback(lv_event_get_target(e));
        // TODO: Send breath check command via NRF24 to Mainboard
        LOG_I("Breath check button pressed");
        break;
    default:
        break;
    }
}

static void screen_operation_btn_opera_9_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
        ui_btn_press_feedback(lv_event_get_target(e));
        // TODO: Send pulse check command via NRF24 to Mainboard
        LOG_I("Pulse check button pressed");
        break;
    default:
        break;
    }
}

static void screen_operation_btn_opera_8_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
        ui_btn_press_feedback(lv_event_get_target(e));
        // TODO: Send emergency breath command via NRF24 to Mainboard
        LOG_I("Emergency breath button pressed");
        break;
    default:
        break;
    }
}

static void screen_operation_btn_opera_7_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
        ui_btn_press_feedback(lv_event_get_target(e));
        // TODO: Send airway open command via NRF24 to Mainboard
        LOG_I("Airway open button pressed");
        break;
    default:
        break;
    }
}

static void screen_operation_btn_opera_6_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
        ui_btn_press_feedback(lv_event_get_target(e));
        // TODO: Send heart compression command via NRF24 to Mainboard
        LOG_I("Heart compression button pressed");
        break;
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
        ui_btn_press_feedback(btn);
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
        ui_btn_press_feedback(btn);
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
        ui_btn_press_feedback(btn);
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
        ui_btn_press_feedback(btn);
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
        ui_btn_press_feedback(btn);
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
