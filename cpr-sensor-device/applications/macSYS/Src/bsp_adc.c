/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-04-18     Administrator       the first version
 * 2026-04-20     Grok             前10秒学习 + CH1基于1000~1100区间大幅偏离检测（已修复链接错误）
 */
#include "bsp_adc.h"
#include <rtthread.h>

#if 1

#define bool    _Bool
#define true    1
#define false   0

#define ADC1_DEVICE_NAME    "adc1"
#define ADC_CH0_CHANNEL     0
#define ADC_CH1_CHANNEL     1

/* ==================== 参数（根据你的描述优化） ==================== */
#define CALIB_SAMPLES       1000    /* 前10秒纯学习 */

#define WINDOW_SIZE         30

/* CH0 参数 */
#define CH0_LOW_TH          65
#define CH0_RANGE_TH        180
#define CH0_LOW_COUNT_TH    9

/* CH1 参数：静态区间 1000~1100，大幅偏离即振动 */
#define CH1_STATIC_MIN      1000
#define CH1_STATIC_MAX      1100
#define CH1_DEVIATION_TH    150     /* 偏离幅度阈值 */
#define CH1_DEVIATION_COUNT_TH  8   /* 窗口内至少8个偏离点 */

#define VIB_CONFIRM_COUNT   6
#define VIB_HOLD_MS         600

#define DRIFT_UPDATE_INTERVAL 3000
#define MAX_DRIFT_PER_STEP  1
/* ================================================ */

typedef struct {
    char adc_dev1_name[16];
    int  adc_channel_0;
    int  adc_channel_1;
    struct rt_device_adc *adc_dev;
} _adc_init;

static _adc_init adc_dev1 = {
    .adc_dev1_name = ADC1_DEVICE_NAME,
    .adc_channel_0 = ADC_CH0_CHANNEL,
    .adc_channel_1 = ADC_CH1_CHANNEL,
};

/* 全局变量 */
static rt_uint16_t adc_window_ch0[WINDOW_SIZE];
static rt_uint16_t adc_window_ch1[WINDOW_SIZE];

static uint8_t     win_idx_ch0 = 0, win_idx_ch1 = 0;
static bool        window_full_ch0 = false, window_full_ch1 = false;

static uint32_t    baseline_mean_ch0 = 0;
static uint32_t    baseline_mean_ch1 = 0;

static uint32_t    calib_cnt = 0;
static bool        calibrated = false;

static uint32_t    last_vib_tick = 0;
static uint32_t    last_baseline_update = 0;

static rt_uint16_t adc_ch0_val = 0;
static rt_uint16_t adc_ch1_val = 0;

static uint8_t     vib_confirm_counter = 0;

/* 通用更新窗口函数（带 low_th 参数） */
static void update_window(rt_uint16_t value, rt_uint16_t *window, uint8_t *p_idx, bool *p_full,
                          uint16_t *min_val, uint16_t *max_val, uint8_t *count, uint16_t threshold)
{
    window[*p_idx] = value;
    *p_idx = (*p_idx + 1) % WINDOW_SIZE;
    if (*p_idx == 0) *p_full = true;

    if (!(*p_full)) return;

    *min_val = 4095;
    *max_val = 0;
    *count = 0;

    for (int i = 0; i < WINDOW_SIZE; i++) {
        rt_uint16_t v = window[i];
        if (v < *min_val) *min_val = v;
        if (v > *max_val) *max_val = v;
        if (v < threshold) (*count)++;
    }
}

/* CH1 专用：统计偏离 1000~1100 的数量 */
static void update_window_ch1(rt_uint16_t value, rt_uint16_t *window, uint8_t *p_idx, bool *p_full,
                              uint16_t *min_val, uint16_t *max_val, uint8_t *deviation_count)
{
    window[*p_idx] = value;
    *p_idx = (*p_idx + 1) % WINDOW_SIZE;
    if (*p_idx == 0) *p_full = true;

    if (!(*p_full)) return;

    *min_val = 4095;
    *max_val = 0;
    *deviation_count = 0;

    for (int i = 0; i < WINDOW_SIZE; i++) {
        rt_uint16_t v = window[i];
        if (v < *min_val) *min_val = v;
        if (v > *max_val) *max_val = v;
        if (v < (CH1_STATIC_MIN - CH1_DEVIATION_TH) || v > (CH1_STATIC_MAX + CH1_DEVIATION_TH)) {
            (*deviation_count)++;
        }
    }
}

int piezo_is_vibrating(rt_uint16_t ch0_val, rt_uint16_t ch1_val)
{
    uint32_t now = rt_tick_get();
    uint16_t minv, maxv;
    uint8_t low_count_ch0 = 0, deviation_count_ch1 = 0;
    bool current_is_vib = false;

    /* 1. 前10秒纯学习 */
    if (!calibrated) {
        baseline_mean_ch0 += ch0_val;
        baseline_mean_ch1 += ch1_val;
        calib_cnt++;
        if (calib_cnt >= CALIB_SAMPLES) {
            baseline_mean_ch0 /= CALIB_SAMPLES;
            baseline_mean_ch1 /= CALIB_SAMPLES;
            calibrated = true;
            last_baseline_update = now;
            rt_kprintf("PRINTF:%d. 【10秒学习完成】CH0基线=%d, CH1基线=%d\r\n",
                       Record.kprintf_cnt++, (int)baseline_mean_ch0, (int)baseline_mean_ch1);
        }
        return 0;
    }

    /* CH0 判断 */
    update_window(ch0_val, adc_window_ch0, &win_idx_ch0, &window_full_ch0, &minv, &maxv, &low_count_ch0, CH0_LOW_TH);
    if (window_full_ch0) {
        uint16_t range = maxv - minv;
        if (low_count_ch0 >= CH0_LOW_COUNT_TH && range >= CH0_RANGE_TH) {
            current_is_vib = true;
        }
    }

    /* CH1 判断：大幅偏离1000~1100区间 */
    update_window_ch1(ch1_val, adc_window_ch1, &win_idx_ch1, &window_full_ch1, &minv, &maxv, &deviation_count_ch1);
    if (window_full_ch1) {
        uint16_t range = maxv - minv;
        if (deviation_count_ch1 >= CH1_DEVIATION_COUNT_TH || range >= 500) {
            current_is_vib = true;
        }
    }

    /* 连续确认 */
    if (current_is_vib) {
        vib_confirm_counter++;
        if (vib_confirm_counter >= VIB_CONFIRM_COUNT) {
            last_vib_tick = now;
            return 1;
        }
    } else {
        vib_confirm_counter = 0;
    }

    /* 振动保持 */
    if ((now - last_vib_tick) < (VIB_HOLD_MS * RT_TICK_PER_SECOND / 1000)) {
        return 1;
    }

    /* 缓慢基线更新 */
    if ((now - last_vib_tick) > (2500 * RT_TICK_PER_SECOND / 1000) &&
        (now - last_baseline_update) > (DRIFT_UPDATE_INTERVAL * RT_TICK_PER_SECOND / 1000)) {

        int32_t delta0 = (int32_t)ch0_val - (int32_t)baseline_mean_ch0;
        if (delta0 > MAX_DRIFT_PER_STEP) delta0 = MAX_DRIFT_PER_STEP;
        if (delta0 < -MAX_DRIFT_PER_STEP) delta0 = -MAX_DRIFT_PER_STEP;

        int32_t delta1 = (int32_t)ch1_val - (int32_t)baseline_mean_ch1;
        if (delta1 > MAX_DRIFT_PER_STEP) delta1 = MAX_DRIFT_PER_STEP;
        if (delta1 < -MAX_DRIFT_PER_STEP) delta1 = -MAX_DRIFT_PER_STEP;

        baseline_mean_ch0 += delta0;
        baseline_mean_ch1 += delta1;
        last_baseline_update = now;
    }

    return 0;
}

/* ====================== 初始化 ====================== */
int ADC_Init(void)
{
    adc_dev1.adc_dev = (struct rt_device_adc*)rt_device_find(adc_dev1.adc_dev1_name);
    if (adc_dev1.adc_dev != RT_NULL) {
        rt_kprintf("PRINTF:%d. adc1 device is created !! \r\n", Record.kprintf_cnt++);
    } else {
        rt_kprintf("PRINTF:%d. adc1 device created failed !! \r\n", Record.kprintf_cnt++);
        return RT_ERROR;
    }

    rt_adc_enable((rt_adc_device_t)adc_dev1.adc_dev, adc_dev1.adc_channel_0);
    rt_adc_enable((rt_adc_device_t)adc_dev1.adc_dev, adc_dev1.adc_channel_1);
    return RT_EOK;
}

void adc_thread_entry(void* parameter)
{
    while (1)
    {
        adc_ch0_val = rt_adc_read((rt_adc_device_t)adc_dev1.adc_dev, adc_dev1.adc_channel_0);
        adc_ch1_val = rt_adc_read((rt_adc_device_t)adc_dev1.adc_dev, adc_dev1.adc_channel_1);

        if (piezo_is_vibrating(adc_ch0_val, adc_ch1_val)){
            Flag.shoke_ack = 1; // 这时可以执行向mainboard发送震动消息
//            rt_kprintf("<VIB>: CH0=%d, CH1=%d (系统振动中)\n", adc_ch0_val, adc_ch1_val);
        }
        else{
//            rt_kprintf("<STATIC>: CH0=%d, CH1=%d\n", adc_ch0_val, adc_ch1_val);
        }

        rt_thread_mdelay(10);
    }
}

rt_thread_t ADC_Thread_Handle;
int ADC_Thread_Init(void)
{
    ADC_Thread_Handle = rt_thread_create("adc_thread_entry", adc_thread_entry, RT_NULL, 1024, 11, 300);
    if (ADC_Thread_Handle != RT_NULL) {
        rt_kprintf("PRINTF:%d. ADC Thread is created!!\r\n", Record.kprintf_cnt++);
        ADC_Init();
        rt_thread_startup(ADC_Thread_Handle);
    } else {
        rt_kprintf("PRINTF:%d. ADC Thread is not created!!\r\n", Record.kprintf_cnt++);
    }
    return RT_EOK;
}

#endif
