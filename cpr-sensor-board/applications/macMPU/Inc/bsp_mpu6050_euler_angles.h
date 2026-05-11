/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-02-23     teati       the first version
 * 2026-04-29     coder       添加头部上仰检测相关结构体和函数声明
 */
#ifndef APPLICATIONS_MACMPU_INC_BSP_MPU6050_EULER_ANGLES_H_
#define APPLICATIONS_MACMPU_INC_BSP_MPU6050_EULER_ANGLES_H_
#include "bsp_sys.h"


/* ====== 头部上仰检测参数默认值 ====== */
#define PITCH_THRESHOLD_DEG      15.0f   /* 上仰角度阈值 (°) */
#define PITCH_RATE_THRESHOLD     30.0f   /* 变化速率阈值 (°/s) */
#define BLOW_CONFIRM_WINDOW_MS   50      /* 确认窗口 (ms) */
#define BLOW_HOLD_MS             100     /* 持续确认时间 (ms) */


/**
  * @brief  判别状态枚举
  */
typedef enum {
    DETECT_IDLE = 0,        /* 空闲 */
    DETECT_PRESS_CONFIRMED, /* 确认按压 */
    DETECT_BLOW_CONFIRMED,  /* 确认吹气 */
    DETECT_PENDING,         /* 待确认 */
    DETECT_CONFLICT         /* 冲突异常 */
} detect_state_et;


/**
  * @brief  判别参数结构体
  */
typedef struct {
    float    pressure_threshold;       /* 压力 ADC 阈值 */
    float    pitch_threshold_deg;      /* pitch 上仰角度阈值 */
    float    pitch_rate_threshold;     /* pitch 变化速率阈值 (°/s) */
    uint32_t confirm_window_ms;        /* 确认窗口 (ms) */
    uint32_t blow_hold_ms;             /* 吹气持续确认时间 (ms) */
} detect_params_st;


/**
  * @brief  头部上仰检测共享数据
  */
typedef struct {
    float    pitch_zero_offset;    /* 零位标定值 */
    uint8_t  is_head_tilt_up;     /* 上仰确认标志: 0=未上仰, 1=确认上仰 */
    uint32_t tilt_start_tick;     /* 上仰开始计时 tick */
    detect_params_st params;      /* 判别参数 */
} head_tilt_shared_st;


extern head_tilt_shared_st head_tilt_data;


/* ====== 函数声明 ====== */

int  euler_angles_Thread_Init(void);
int  bsp_mpu6xxx_calibrate_Thread_Init(void);

/**
  * @brief  更新头部上仰检测状态
  * @param  current_pitch: 当前 pitch 角度 (°)
  * @param  current_tick:  当前系统 tick (ms)
  */
void head_tilt_detect_update(float current_pitch, uint32_t current_tick);

/**
  * @brief  启动时自动标定 pitch 零位
  * @param  startup_pitch: 启动时采集的 pitch 初始值 (°)
  */
void head_tilt_calibrate_zero(float startup_pitch);


#endif /* APPLICATIONS_MACMPU_INC_BSP_MPU6050_EULER_ANGLES_H_ */
