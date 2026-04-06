/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-11-11     18452       the first version
 */
#include <mainboard_sys.h>


#define TOUCH_KEY_TOTAL  14
#define DEBOUNCE_TIME_MS    30      // 消抖时间 30ms
#define LONG_PRESS_TIME_MS 1000     // 长按判定时间 1000ms

typedef enum {
    KEY_STATE_IDLE = 0,    // 未按下
    KEY_STATE_DEBOUNCE,    // 消抖中
    KEY_STATE_PRESSED,     // 已按下
    KEY_STATE_LONG_PRESS   // 长按触发后
} key_state_t;

typedef struct {
    key_state_t state;
    rt_tick_t   last_change_time;
    rt_tick_t   press_start_time;
    rt_bool_t   event_sent_short;   // 短按事件是否已发送
    rt_bool_t   event_sent_long;    // 长按事件是否已发送
} touch_key_t;

static touch_key_t touch_keys[TOUCH_KEY_TOTAL] = {0};

static const struct {
    GPIO_TypeDef* port;
    uint16_t      pin;
    Touch_Type_et key_id;
} touch_pins[TOUCH_KEY_TOTAL] = {
    { TOUCH_IN1_GPIO_Port,  TOUCH_IN1_Pin,  TOUCH_START },
    { TOUCH_IN2_GPIO_Port,  TOUCH_IN2_Pin,  TOUCH_TRAIN },
    { TOUCH_IN3_GPIO_Port,  TOUCH_IN3_Pin,  TOUCH_ASSESS },
    { TOUCH_IN4_GPIO_Port,  TOUCH_IN4_Pin,  TOUCH_COMPETITION },
    { TOUCH_IN5_GPIO_Port,  TOUCH_IN5_Pin,  TOUCH_MINUS },
    { TOUCH_IN6_GPIO_Port,  TOUCH_IN6_Pin,  TOUCH_PLUS },
    { TOUCH_IN7_GPIO_Port,  TOUCH_IN7_Pin,  TOUCH_RESET },
    { TOUCH_IN8_GPIO_Port,  TOUCH_IN8_Pin,  TOUCH_PRINTER },
    { TOUCH_IN9_GPIO_Port,  TOUCH_IN9_Pin,  TOUCH_SETTING },
    { TOUCH_IN10_GPIO_Port, TOUCH_IN10_Pin, TOUCH_REMOVE_FOREIGN },
    { TOUCH_IN11_GPIO_Port, TOUCH_IN11_Pin, TOUCH_EMERGENCY_CALL },
    { TOUCH_IN12_GPIO_Port, TOUCH_IN12_Pin, TOUCH_CHECK_BREATH },
    { TOUCH_IN13_GPIO_Port, TOUCH_IN13_Pin, TOUCH_SPHYMOSCOPY },
    { TOUCH_IN14_GPIO_Port, TOUCH_IN14_Pin, TOUCH_CONSCIOUS_JUDGMENT },
};



/* 外部声明，你需要在别的文件中处理这些事件 */
extern void Touch_Key_Event_Handler(Touch_Type_et key, rt_uint8_t event);
// event: 1=短按抬起  2=长按触发  3=长按持续（可重复触发）

static void touch_scan(void)
{
    rt_tick_t now = rt_tick_get();

    for(int i = 0; i < TOUCH_KEY_TOTAL; i++)
    {
        rt_uint8_t level = HAL_GPIO_ReadPin(touch_pins[i].port, touch_pins[i].pin);
        touch_key_t *k = &touch_keys[i];

        switch(k->state)
        {
            case KEY_STATE_IDLE:
                if(level == GPIO_PIN_SET)  // 检测到按下
                {
                    k->state = KEY_STATE_DEBOUNCE;
                    k->last_change_time = now;
                    k->press_start_time = now;
                    k->event_sent_short = RT_FALSE;
                    k->event_sent_long  = RT_FALSE;
                }
                break;

            case KEY_STATE_DEBOUNCE:
                if(now - k->last_change_time >= rt_tick_from_millisecond(DEBOUNCE_TIME_MS))
                {
                    rt_uint8_t level_now = HAL_GPIO_ReadPin(touch_pins[i].port, touch_pins[i].pin);
                    if(level_now == GPIO_PIN_SET)
                    {
                        k->state = KEY_STATE_PRESSED;
                        // 按下瞬间可以在这里发按下事件
                    }
                    else
                    {
                        k->state = KEY_STATE_IDLE;  // 抖动，放弃
                    }
                }
                break;

            case KEY_STATE_PRESSED:
                if(level == GPIO_PIN_RESET)  // 抬起
                {
                    if(!k->event_sent_short && !k->event_sent_long)
                    {
                        // 短按抬起事件
                        Touch_Key_Event_Handler(touch_pins[i].key_id, 1);
                    }
                    k->state = KEY_STATE_IDLE;
                }
                else
                {
                    // 还在按着，检查长按
                    if(now - k->press_start_time >= rt_tick_from_millisecond(LONG_PRESS_TIME_MS))
                    {
                        if(!k->event_sent_long)
                        {
                            Touch_Key_Event_Handler(touch_pins[i].key_id, 2);  // 长按第一次触发
                            k->event_sent_long = RT_TRUE;
                        }
                        k->state = KEY_STATE_LONG_PRESS;
                    }
                }
                break;

            case KEY_STATE_LONG_PRESS:
                if(level == GPIO_PIN_RESET)
                {
                    k->state = KEY_STATE_IDLE;
                }
                else
                {
                    // 长按持续中，可重复触发（比如加减键连续加减）
                    if(now - k->last_change_time >= rt_tick_from_millisecond(200)) // 每200ms触发一次
                    {
                        Touch_Key_Event_Handler(touch_pins[i].key_id, 3);
                        k->last_change_time = now;
                    }
                }
                break;
        }
    }
}



void Touch_Key_Event_Handler(Touch_Type_et key, rt_uint8_t event)
{
    switch(event)
    {
        case 1: rt_kprintf("Key Short Press: %d\n", key); break;
        case 2: rt_kprintf("Key Long Press Start: %d\n", key); break;
        case 3: rt_kprintf("Key Long Press Hold: %d\n", key); break;
    }
    // 开始按键一次都没按下-------------------------------------------------------------------------
    if(event == 1 && key == TOUCH_START && MySysCfg.start_status == 0 && MySysCfg.start_press_cnt == 0 && MySysCfg.reset_press_cnt == 1)
    {
        MySysCfg.start_status = 1;
        MySysCfg.start_press_cnt = 1;
        MySysCfg.reset_press_cnt = 0;
        MySysCfg.setting_mode = 0;

        // 语音播报：开始工作
        WT588D_Set_Cmd(WT588D_ADDR_VOICE_2);

        // 开始状态下，除了开始LED，把其他灯都先熄灭
        LED_On(LED_Name_Start);
        LED_Off(LED_Name_Reset);
        LED_Off(LED_Name_Setting);
        /***
         * !开始状态下不允许设置、打印、加、减、切换模式
         * !允许复位、
         */

    }
    // 复位按键-------------------------------------------------------------------------
    if(event == 1 && key == TOUCH_RESET && \
       (MySysCfg.start_status == 1 || MySysCfg.start_status == 2) && \
       MySysCfg.start_press_cnt == 1 && \
       MySysCfg.reset_press_cnt == 0)
    {
        MySysCfg.start_status = 0;
        MySysCfg.start_press_cnt = 0;
        MySysCfg.reset_press_cnt = 1;
        // 语音播报：复位
        WT588D_Set_Cmd(WT588D_ADDR_VOICE_3);

        // 复位状态下，除了复位LED，把其他灯都先熄灭
        LED_Off(LED_Name_Start);
        LED_On(LED_Name_Reset);
        // 数值重置
        system_params_init();

    }



    // 训练模式按键：未开始状态下可以切换模式-------------------------------------------------------------------------
    if(event == 1 && key == TOUCH_TRAIN && MySysCfg.start_status == 0)
    {
        MySysCfg.current_mode = MODE_TRAIN;
        // 语音播报：训练模式
        WT588D_Set_Cmd(WT588D_ADDR_VOICE_5);

        LED_On(LED_Name_Train);
        LED_Off(LED_Name_Assess);
        LED_Off(LED_Name_Competition);
    }
    // 考核模式按键：未开始状态下可以切换模式-------------------------------------------------------------------------
    if(event == 1 && key == TOUCH_ASSESS && MySysCfg.start_status == 0)
    {
        MySysCfg.current_mode = MODE_ASSESS;
        // 语音播报：考核模式
        WT588D_Set_Cmd(WT588D_ADDR_VOICE_4);
        LED_Off(LED_Name_Train);
        LED_On(LED_Name_Assess);
        LED_Off(LED_Name_Competition);
    }
    // 竞赛模式按键：未开始状态下可以切换模式-------------------------------------------------------------------------
    if(event == 1 && key == TOUCH_COMPETITION && MySysCfg.start_status == 0)
    {
        MySysCfg.current_mode = MODE_COMPETE;
        // 语音播报：竞赛模式
        WT588D_Set_Cmd(WT588D_ADDR_VOICE_1);
        LED_Off(LED_Name_Train);
        LED_Off(LED_Name_Assess);
        LED_On(LED_Name_Competition);
    }
    // 设置按键-------------------------------------------------------------------------
    if(event == 1 && key == TOUCH_SETTING && MySysCfg.current_mode == MODE_TRAIN && MySysCfg.start_status == 0)
    {
        Record.touch_set_cnt ++;
        if(Record.touch_set_cnt == 1){
            MySysCfg.setting_mode = 1;
            // 语音播报：进入设置模式，请设置工作时间、达标率等参数
            WT588D_Set_Cmd(WT588D_ADDR_VOICE_6);
            LED_On(LED_Name_Setting);
        }
        else{
            Record.touch_set_cnt = 0;
            MySysCfg.setting_mode = 0;
            // 语音播报：退出设置模式
            WT588D_Set_Cmd(WT588D_ADDR_VOICE_7);
            LED_Off(LED_Name_Setting);
        }
    }
    else if(event == 1 && key == TOUCH_SETTING && MySysCfg.current_mode == MODE_ASSESS && MySysCfg.start_status == 0)
    {
        Record.touch_set_cnt++;
        if(Record.touch_set_cnt == 1){
            MySysCfg.setting_mode = 1;
            // 语音播报：进入设置模式，请设置工作时间、达标率等参数
            WT588D_Set_Cmd(WT588D_ADDR_VOICE_6);
            LED_On(LED_Name_Setting);
        }
        else{
            Record.touch_set_cnt = 0;
            MySysCfg.setting_mode = 0;
            // 语音播报：退出设置模式
            WT588D_Set_Cmd(WT588D_ADDR_VOICE_7);
            LED_Off(LED_Name_Setting);
        }
    }
    else if(event == 1 && key == TOUCH_SETTING && MySysCfg.current_mode == MODE_COMPETE && MySysCfg.start_status == 0)
    {
        Record.touch_set_cnt++;
        if(Record.touch_set_cnt == 1){
            MySysCfg.setting_mode = 1;
            // 语音播报：进入设置模式，请设置工作时间、达标率等参数
            WT588D_Set_Cmd(WT588D_ADDR_VOICE_6);
            LED_On(LED_Name_Setting);
        }
        else{
            Record.touch_set_cnt = 0;
            MySysCfg.setting_mode = 0;
            // 语音播报：退出设置模式
            WT588D_Set_Cmd(WT588D_ADDR_VOICE_7);
            LED_Off(LED_Name_Setting);
        }
    }
    // -------------------------------------------------------------------------
    if(event == 1 && key == TOUCH_PLUS && MySysCfg.setting_mode == 1 && MySysCfg.start_status == 0)
    {
        MySysCfg.params[MySysCfg.current_mode].Number_CountDown += 10;
        if(MySysCfg.params[MySysCfg.current_mode].Number_CountDown >= 990){
            MySysCfg.params[MySysCfg.current_mode].Number_CountDown = 990;
        }

        LED_On(LED_Name_Plus_Sign);
        rt_thread_mdelay(200);
        LED_Off(LED_Name_Plus_Sign);
    }
    // -------------------------------------------------------------------------
    if(event == 1 && key == TOUCH_MINUS && MySysCfg.setting_mode == 1 && MySysCfg.start_status == 0)
    {
        MySysCfg.params[MySysCfg.current_mode].Number_CountDown -= 10;

        if(MySysCfg.params[MySysCfg.current_mode].Number_CountDown <= 0){
            MySysCfg.params[MySysCfg.current_mode].Number_CountDown = 0;
        }


        LED_On(LED_Name_Minus_Sign);
        rt_thread_mdelay(200);
        LED_Off(LED_Name_Minus_Sign);
    }
    // 打印触摸按键：这个功能只有在完成一次完整的流程后才会触发-------------------------------------------------------------------------
    if(event == 1 && key == TOUCH_PRINTER && MySysCfg.start_status == 2)
    {


       rt_kprintf("Function now is printing in progress.\n");
    }
    // 清除异物按键-------------------------------------------------------------------------
    if(event == 1 && key == TOUCH_REMOVE_FOREIGN)
    {

    }
    // 急救呼叫按键-------------------------------------------------------------------------
    if(event == 1 && key == TOUCH_EMERGENCY_CALL)
    {

    }
    // 脉搏检测按键-------------------------------------------------------------------------
    if(event == 1 && key == TOUCH_SPHYMOSCOPY)
    {

    }
    // 意识判断按键-------------------------------------------------------------------------
    if(event == 1 && key == TOUCH_CONSCIOUS_JUDGMENT)
    {

    }
}



/**
  * @brief  This thread entry is used for touch pad scanning
  * @retval void
  */
void Touch_Thread_entry(void* parameter)
{


    /* 可选：初始化所有按键结构体 */
    for(int i=0; i<TOUCH_KEY_TOTAL; i++)
    {
        touch_keys[i].state = KEY_STATE_IDLE;
    }

    for(;;)
    {
        touch_scan();
        rt_thread_mdelay(20);
    }
}








/**
  * @brief  This is a Initialization for water level Check
  * @retval int
  */
int Touch_Thread_Init(void)
{
    rt_thread_t Touch_Task_Handle = RT_NULL;
    /* 创建检查一些系统状态标志的线程  -- 优先级：25 */
    Touch_Task_Handle = rt_thread_create("Touch_Thread_entry", Touch_Thread_entry, RT_NULL, 1024, 11, 100);
    /* 检查是否创建成功,成功就启动线程 */
    if(Touch_Task_Handle != RT_NULL)
    {
        rt_kprintf("PRINTF:%d. Touch_Thread_entry is Succeed!! \r\n",Record.kprintf_cnt++);
        rt_thread_startup(Touch_Task_Handle);
    }
    else {
        rt_kprintf("PRINTF:%d. Touch_Thread_entry is Failed \r\n",Record.kprintf_cnt++);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(Touch_Thread_Init);



