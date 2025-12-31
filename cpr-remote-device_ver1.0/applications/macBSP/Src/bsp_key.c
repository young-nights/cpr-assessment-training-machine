
/*
 * bsp_key.c
 *
 *  Created on: Jun 16, 2025
 *      Author: Administrator
 */


#include "bsp_key.h"


/* Key value table */
const char keys[3][3] = {
    {'3', '2', '1'},
    {'6', '5', '4'},
    {'9', '8', '7'}
};



/* ------------------- 消抖 & 事件 ------------------- */
static uint8_t debounce_cnt[3][3] = {{0}};      // 计数器
static uint8_t stable_state[3][3] = {{1}};      // 1 = 释放，0 = 按下（上电默认高）
key_event_t current_key = {0, 0, KEY_NONE};


/* GPIO control functions */
void GPIO_SetColumn(MatrixKey_ColumnName_TypeDef col, MatrixKey_Status_TypeDef state) {
    GPIO_PinState pinState = (state == Matrix_SET) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    GPIO_TypeDef* port;
    uint16_t pin;

    switch (col) {
        case Matrix_Column_1:
            port = Matrixkey_Column1_GPIO_Port;
            pin = Matrixkey_Column1_Pin;
            break;
        case Matrix_Column_2:
            port = Matrixkey_Column2_GPIO_Port;
            pin = Matrixkey_Column2_Pin;
            break;
        case Matrix_Column_3:
            port = Matrixkey_Column3_GPIO_Port;
            pin = Matrixkey_Column3_Pin;
            break;

        default:
            return;
    }
    HAL_GPIO_WritePin(port, pin, pinState);
}



uint8_t GPIO_ReadRow(MatrixKey_RowName_TypeDef row) {
    GPIO_TypeDef* port;
    uint16_t pin;

    switch (row) {
        case Matrix_Row_1:
            port = Matrixkey_Row1_GPIO_Port;
            pin = Matrixkey_Row1_Pin;
            break;
        case Matrix_Row_2:
            port = Matrixkey_Row2_GPIO_Port;
            pin = Matrixkey_Row2_Pin;
            break;
        case Matrix_Row_3:
            port = Matrixkey_Row3_GPIO_Port;
            pin = Matrixkey_Row3_Pin;
            break;
        default:
            return 1;  // Default high
    }
    return (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET) ? 0 : 1;  // 0 if low (pressed), 1 if high
}




/* Event trigger callback function pointer */
void (*key_event_callback)(key_event_t event) = NULL;

/**
  * @brief  Set the callback function for key events.
  * @param  callback: Pointer to the callback function.
  * @retval void
  */
void MatrixKey_SetCallback(void (*callback)(key_event_t event)) {
    key_event_callback = callback;
}


// 定义自定义回调函数
void MyCustomKeyHandler(key_event_t event) {
    char key_value = keys[event.row - 1][event.col - 1];  // 获取键值（从 keys 表中）

    if (event.state == KEY_PRESS) {
        rt_kprintf("Custom Handler: Key '%c' pressed at Row %d, Col %d\n", key_value, event.row, event.col);
        //-------------------------------------------------------------------------------------------------------------------------
        // 新增逻辑：当键值为'1'时，进入设置页面
        if(key_value == '1' && Record.nrf_if_connected == 1)
        {

        }
        //-------------------------------------------------------------------------------------------------------------------------
        // 新增逻辑：当键值为'2'时，进入操作页面
        if(key_value == '2' && Record.nrf_if_connected == 1 )
        {

        }
        //-------------------------------------------------------------------------------------------------------------------------
        // 新增逻辑：当键值为 '3' 时，切换回 menu 页面
        if (key_value == '3' && Record.menu_index == 2 && Record.nrf_if_connected == 1) {
            // 获取当前活动屏幕
            lv_obj_t *current_scr = lv_scr_act();
            // 确定当前屏幕的 del 标志（根据 guider_lvgl 结构匹配）
            bool *current_del = NULL;

            if (current_scr == guider_lvgl.screen_data) {
                current_del = &guider_lvgl.screen_data_del;
                cleanup_data_screen_animation();
            } else if (current_scr == guider_lvgl.screen_setting) {
                current_del = &guider_lvgl.screen_setting_del;
            } else if (current_scr == guider_lvgl.screen_operation) {
                current_del = &guider_lvgl.screen_operation_del;
            }
            // 给设备发送退出指令
            nrf24l01_order_to_pipe(_nrf24,Order_nRF24L01_ASK_Data_Mode_Out,NRF24_PIPE_2);
            // 执行页面切换到 menu（使用提供的 ui_load_scr_animation 函数）
            ui_load_scr_animation(&guider_lvgl, &guider_lvgl.screen_menu, guider_lvgl.screen_menu_del, current_del, setup_scr_screen_menu, LV_SCR_LOAD_ANIM_NONE, 0, 100, true, true);
            Record.menu_index = 1;
            Record.mode_data_in = 0;
            Record.mode_data_in_set = 0;
        }

        //-------------------------------------------------------------------------------------------------------------------------
        // 在设置模式，且数值选中可调，按键键值为 '4' 时，对设置数值增加
        if(key_value == '4' && Record.setting_mode == 1 && Record.nrf_if_connected == 1)
        {
            if(Flag.air_rate_set == 1){
                Record.set_air_rate += 1;
                if (Record.set_air_rate > 100) Record.set_air_rate = 100;
                lv_label_set_text_fmt(guider_lvgl.screen_setting_label_air_rate, "%d%%", Record.set_air_rate);

            }
            else if(Flag.press_rate_set == 1){
                Record.set_press_rate += 1;
                if (Record.set_press_rate >= 100) Record.set_press_rate = 100;
                lv_label_set_text_fmt(guider_lvgl.screen_setting_label_press_rate, "%d%%", Record.set_press_rate);
            }
            else if(Flag.work_time_set == 1){
                Record.set_work_time +=10;
                if (Record.set_work_time > 990) Record.set_work_time = 990;
                lv_label_set_text_fmt(guider_lvgl.screen_setting_label_time_value, "%ds", Record.set_work_time);

            }
        }
        //-------------------------------------------------------------------------------------------------------------------------
        // 新增逻辑：在竞赛/训练/考核模式页面，按键键值为 '5' 时，开始项目
        if(key_value == '5' && Record.nrf_if_connected == 1)
        {

        }
        //-------------------------------------------------------------------------------------------------------------------------
        // 新增逻辑：在竞赛/训练/考核模式页面，按键键值为 '6' 时，复位项目
        if(key_value == '6' && Record.nrf_if_connected == 1 )
        {

        }
        //-------------------------------------------------------------------------------------------------------------------------
        // 新增逻辑：在设置模式，且数值选中可调，按键键值为 '7' 时，对设置数值减少
        if(key_value == '7' && Record.setting_mode == 1 && Record.nrf_if_connected == 1)
        {
            if(Flag.air_rate_set == 1){
                Record.set_air_rate -= 1;
                if (Record.set_air_rate <= 1) Record.set_air_rate = 1;
                lv_label_set_text_fmt(guider_lvgl.screen_setting_label_air_rate, "%d%%", Record.set_air_rate);
            }
            else if(Flag.press_rate_set == 1){
                Record.set_press_rate -= 1;
                if (Record.set_press_rate <= 1) Record.set_press_rate = 1;
                lv_label_set_text_fmt(guider_lvgl.screen_setting_label_press_rate, "%d%%", Record.set_press_rate);
            }
            else if(Flag.work_time_set == 1){
                Record.set_work_time -=10;
                if (Record.set_work_time <= 10) Record.set_work_time = 10;
                lv_label_set_text_fmt(guider_lvgl.screen_setting_label_time_value, "%ds", Record.set_work_time);
            }
        }
        //-------------------------------------------------------------------------------------------------------------------------
        // 新增逻辑：当键值为'8'时，开始打印



        //-------------------------------------------------------------------------------------------------------------------------
        // 新增逻辑：当键值为'9'时，关闭设备


    }
    else if (event.state == KEY_RELEASE && Record.nrf_if_connected) {
        rt_kprintf("Custom Handler: Key '%c' released at Row %d, Col %d\n", key_value, event.row, event.col);
    }
}


// 在初始化函数中注册回调（例如在 main() 或其他 init 函数中调用）
void App_Key_Init(void) {
    // 注册自定义回调，替换默认处理
    MatrixKey_SetCallback(MyCustomKeyHandler);
}



/**
  * @brief  Default event handler (can be overridden by user callback).
  *         Prints the key event.
  * @param  event: The key event.
  * @retval void
  */
void Default_Key_Event_Handler(key_event_t event) {
    if (event.state == KEY_PRESS) {
        rt_kprintf("Key pressed: %c (Row %d, Col %d)\n", keys[event.row-1][event.col-1], event.row, event.col);
    } else if (event.state == KEY_RELEASE) {
        rt_kprintf("Key released: %c (Row %d, Col %d)\n", keys[event.row-1][event.col-1], event.row, event.col);
    }
}


/* ------------------- 扫描 + 消抖 ------------------- */
void MatrixKey_Scan(void)
{
    uint8_t row, col;
    uint8_t cur_level;

    current_key.state = KEY_NONE;   // 每次扫描先清空

    for (col = Matrix_Column_1; col <= Matrix_Column_3; ++col) {
        /* 拉低当前列，其余列拉高 */
        for (uint8_t c = Matrix_Column_1; c <= Matrix_Column_3; ++c)
            GPIO_SetColumn(c, (c == col) ? Matrix_RESET : Matrix_SET);

        rt_thread_mdelay(1);   // 让电平稳定

        for (row = Matrix_Row_1; row <= Matrix_Row_3; ++row) {
            cur_level = GPIO_ReadRow(row);               // 0 = 低（按下），1 = 高（释放）

            uint8_t r = row - 1, c = col - 1;             // 数组下标

            if (cur_level == 0) {                        // 检测到低电平
                if (debounce_cnt[r][c] < DEBOUNCE_THRESHOLD)
                    debounce_cnt[r][c]++;

                if (debounce_cnt[r][c] == DEBOUNCE_THRESHOLD && stable_state[r][c] == 1) {
                    /* 确认按下 */
                    stable_state[r][c] = 0;
                    current_key.row   = row;
                    current_key.col   = col;
                    current_key.state = KEY_PRESS;

                    if (key_event_callback)
                        key_event_callback(current_key);
                    else
                        Default_Key_Event_Handler(current_key);
                }
            } else {                                     // 检测到高电平
                if (debounce_cnt[r][c] > 0)
                    debounce_cnt[r][c]--;

                if (debounce_cnt[r][c] == 0 && stable_state[r][c] == 0) {
                    /* 确认释放 */
                    stable_state[r][c] = 1;
                    current_key.row   = row;
                    current_key.col   = col;
                    current_key.state = KEY_RELEASE;

                    if (key_event_callback)
                        key_event_callback(current_key);
                    else
                        Default_Key_Event_Handler(current_key);
                }
            }
        }
    }

    /* 扫描结束，所有列恢复高电平 */
    for (col = Matrix_Column_1; col <= Matrix_Column_3; ++col)
        GPIO_SetColumn(col, Matrix_SET);
}


/*---------------------------------------------------------------------------------------------------------------*/
/* 以下是按键扫描线程的创建以及回调函数                                                                          */
/*---------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  This thread entry is used for key scan
  * @retval void
  */
void Matrixkey_Thread_entry(void* parameter)
{

    for(;;)
    {
        MatrixKey_Scan();
        rt_thread_mdelay(20);
    }
}



/**
  * @brief  This is a Initialization for matrix key
  * @retval int
  */
rt_thread_t Matrixkey_Task_Handle = RT_NULL;
int Matrixkey_Thread_Init(void)
{
    Matrixkey_Task_Handle = rt_thread_create("Matrixkey_Thread_entry", Matrixkey_Thread_entry, RT_NULL, 4096, 9, 50);
    /* 检查是否创建成功,成功就启动线程 */
    if(Matrixkey_Task_Handle != RT_NULL)
    {
        rt_kprintf("PRINTF:%d. Matrixkey_Thread_entry is Succeed!! \r\n",Record.kprintf_cnt++);
        App_Key_Init();
        rt_thread_startup(Matrixkey_Task_Handle);
    }
    else {
        rt_kprintf("PRINTF:%d. Matrixkey_Thread_entry is Failed \r\n",Record.kprintf_cnt++);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(Matrixkey_Thread_Init);
