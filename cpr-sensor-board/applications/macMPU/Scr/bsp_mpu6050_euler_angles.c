/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-02-23     teati       the first version
 * 2026-04-29     coder       添加头部上仰检测逻辑（head_tilt_detect_update/calibrate）
 */
#include "bsp_mpu6050_euler_angles.h"





//以下是解算MPU6500的数据函数驱动--------------------------------------------------------------------------------------------------------
#define GRAVITY         9.80665f                // 标准重力加速度(m/s²)
#define RAD_TO_DEG      57.2957795f             // 弧度转角度的转换因子
#define SAMPLE_RATE     100                     // 采样频率(Hz)
#define DT              1.0f/SAMPLE_RATE        // 采样周期
#define ALPHA           0.99f                   // 互补滤波系数

struct mpu6xxx_3axes bsp_accel, bsp_gyro;


/* ====== 头部上仰检测共享数据 ====== */
head_tilt_shared_st head_tilt_data = {
    .pitch_zero_offset = 0.0f,
    .is_head_tilt_up   = 0,
    .tilt_start_tick   = 0,
    .params = {
        .pressure_threshold   = 3500.0f,         /* 默认压力 ADC 阈值 */
        .pitch_threshold_deg  = PITCH_THRESHOLD_DEG,
        .pitch_rate_threshold = PITCH_RATE_THRESHOLD,
        .confirm_window_ms    = BLOW_CONFIRM_WINDOW_MS,
        .blow_hold_ms         = BLOW_HOLD_MS
    }
};

/* 静态变量：用于计算 pitch 变化速率 */
static float    s_prev_pitch = 0.0f;
static uint32_t s_prev_tick  = 0;
static uint8_t  s_zero_calibrated = 0;  /* 零位标定完成标志 */





/**
  * @brief  x轴与y轴坐标转换
  * @param  accel,gyro的数据结构
  * @retval None
  */
static void transform_coordinates(struct mpu6xxx_3axes *accel, struct mpu6xxx_3axes *gyro, transform_coordinates_StructType mode)
{
    if(mode == nothing){
        return;
    }
    else if(mode == x_axis_interchangeable_y_axis){

        rt_int16_t empty;

        empty = accel->x;
        accel->x = accel->y;
        accel->y = empty;

        empty = gyro->x;
        gyro->x = gyro->y;
        gyro->y = empty;
    }
}




/**
  * @brief  利用加速度计的数据计算初始角度
  * @param  accel,gyro的三轴数据
  * @retval None
  */
static void get_accel_angles(struct mpu6xxx_3axes *accel, float *pitch, float *roll)
{
    /***
     *! 单位转换:由MPU6500得到的加速度三轴数据单位均是mg,因此需要将单位转换为m/s²
     *! 1g = 9.80665m/s²
     */
    float ax = accel->x * 0.001f * GRAVITY;
    float ay = accel->y * 0.001f * GRAVITY;
    float az = accel->z * 0.001f * GRAVITY;

    /***
     *! pitch       ：表示物体在垂直轴上的倾斜程度，单位为度数
     *! atan2f      ：反正切函数，返回两个参数的反正切值，范围在 −π 到 π 之间，单位为弧度
     *! RAD_TO_DEG  ：弧度转换为度数的转换因子
     *! 当 ax 为正时，−ax 为负，表示物体沿 x 轴正方向倾斜，俯仰角为负
     *! 当 ax 为负时，−ax 为正，表示物体沿 x 轴负方向倾斜，俯仰角为正
     */
    *pitch = atan2f(-ax, sqrtf(ay*ay + az*az)) * RAD_TO_DEG;

    /***
     *!    ay(表示沿y轴的加速度)
     *!    az(表示沿z轴的加速度)
     */
    *roll  = atan2f(ay, az) * RAD_TO_DEG;

}



/**
  * @brief  互补滤波姿态解算
  * @param  EulerAngles ：声明的欧拉角结构体
  * @retval None
  */
void calculate_euler_angles(struct mpu6xxx_device *dev, EulerAngles *angles)
{
    static float pitch = 0.0f, roll = 0.0f;
    float pitch_acc, roll_acc;

    // 获取传感器数据
    mpu6xxx_get_accel(dev, &bsp_accel);
    mpu6xxx_get_gyro(dev, &bsp_gyro);



    // 坐标系转换（根据实际安装方向）
    transform_coordinates(&bsp_accel, &bsp_gyro, nothing);

    // 从加速度计获取角度
    get_accel_angles(&bsp_accel, &pitch_acc, &roll_acc);

    // 陀螺仪积分（转换为deg/s）
    float gyro_x = bsp_gyro.x * 0.1f; // 原单位是deg/10s
    float gyro_y = bsp_gyro.y * 0.1f;

    pitch += gyro_y * DT; // 绕Y轴旋转对应pitch
    roll  += gyro_x * DT; // 绕X轴旋转对应roll

    // 互补滤波融合
    pitch = ALPHA * pitch + (1 - ALPHA) * pitch_acc;
    roll  = ALPHA * roll  + (1 - ALPHA) * roll_acc;

    // Yaw需要磁力计数据（此处未实现）
    angles->pitch = pitch;
    angles->roll  = roll;
    angles->yaw   = 0.0f; // 需磁力计支持
}


/**
  * @brief  头部上仰检测更新
  * @param  current_pitch: 当前融合后的 pitch 角度 (°)
  * @param  current_tick:  当前系统 tick (ms)
  *
  *         判定条件（全部满足才确认上仰）：
  *         1. 相对 pitch ≥ 阈值 (默认15°)
  *         2. pitch 变化速率 ≥ 阈值 (默认30°/s)
  *         3. 满足以上条件持续 ≥ 持续时间 (默认100ms)
  */
void head_tilt_detect_update(float current_pitch, uint32_t current_tick)
{
    /* 首次调用时初始化前值 */
    if (s_prev_tick == 0) {
        s_prev_pitch = current_pitch - head_tilt_data.pitch_zero_offset;
        s_prev_tick  = current_tick;
        return;
    }

    float relative_pitch = current_pitch - head_tilt_data.pitch_zero_offset;

    float dt = (current_tick - s_prev_tick) / 1000.0f;
    if (dt <= 0) dt = 0.005f; /* 最小 5ms，防止除零 */

    float pitch_rate = (relative_pitch - s_prev_pitch) / dt;

    /* 条件1: pitch 角超过阈值（上仰为正方向） */
    uint8_t angle_ok = (relative_pitch >= head_tilt_data.params.pitch_threshold_deg);
    /* 条件2: 变化速率超过阈值 */
    uint8_t rate_ok  = (fabsf(pitch_rate) >= head_tilt_data.params.pitch_rate_threshold);

    if (angle_ok && rate_ok) {
        if (head_tilt_data.tilt_start_tick == 0) {
            head_tilt_data.tilt_start_tick = current_tick;
        }
        /* 条件3: 持续时间足够 */
        if ((current_tick - head_tilt_data.tilt_start_tick) >= head_tilt_data.params.blow_hold_ms) {
            head_tilt_data.is_head_tilt_up = 1;
        }
    } else {
        head_tilt_data.tilt_start_tick = 0;
        head_tilt_data.is_head_tilt_up = 0;
    }

    s_prev_pitch = relative_pitch;
    s_prev_tick  = current_tick;
}


/**
  * @brief  启动时自动标定 pitch 零位
  * @param  startup_pitch: 启动时采集的 pitch 初始值 (°)
  *
  *         在 MPU6050 初始化完成后、正式检测前调用，
  *         取前若干次 pitch 均值作为零位补偿
  */
void head_tilt_calibrate_zero(float startup_pitch)
{
    head_tilt_data.pitch_zero_offset = startup_pitch;
    s_prev_pitch = 0.0f;
    s_prev_tick  = 0;
    head_tilt_data.is_head_tilt_up = 0;
    head_tilt_data.tilt_start_tick = 0;
    s_zero_calibrated = 1;

    rt_kprintf("HEAD TILT: zero calibrated, offset = %.2f°\n", startup_pitch);
}




/*---------------------------------------------------------------------------------------------------------------*/
/* 以下是平衡小车姿态解算线程的创建以及回调函数                                                                                                                                                                                                */
/*---------------------------------------------------------------------------------------------------------------*/

/**
  * @brief  数据解码回调函数入口
  * @retval void
  */
EulerAngles carEulerAngles;
void euler_angles_thread_entry(void* parameter)
{

#define USER_PRINTF_EULER_ANGELES 0

    /* 启动前延迟等待传感器稳定，然后标定零位 */
    rt_thread_mdelay(2000);
    calculate_euler_angles(mpu6050_dev, &carEulerAngles);
    head_tilt_calibrate_zero(carEulerAngles.pitch);

    while(1)
    {
        calculate_euler_angles(mpu6050_dev, &carEulerAngles);

        /* 头部上仰检测 */
        head_tilt_detect_update(carEulerAngles.pitch, rt_tick_get_millisecond());

#if USER_PRINTF_EULER_ANGELES
        rt_kprintf("Pitch:%0.2f°, Roll:%0.2f°, Yaw:%0.2f°, tilt:%d \r\n",
                   carEulerAngles.pitch, carEulerAngles.roll, carEulerAngles.yaw,
                   head_tilt_data.is_head_tilt_up);
#endif
        rt_thread_mdelay(1000/SAMPLE_RATE);
    }
}


/**
  * @brief  初始化数据解码函数
  * @retval int
  */
rt_thread_t euler_angles_Thread_Handle;
int euler_angles_Thread_Init(void)
{
    euler_angles_Thread_Handle = rt_thread_create("euler_angles_thread_entry", euler_angles_thread_entry, RT_NULL, 3072, 5, 20);
    if(euler_angles_Thread_Handle != RT_NULL){
        rt_kprintf("PRINTF:%d. euler_angles_Thread_Handle is created!!\r\n",Record.kprintf_cnt++);
        rt_thread_startup(euler_angles_Thread_Handle);
    }
    else {
        rt_kprintf("PRINTF:%d. euler_angles_Thread_Handle is not created!!\r\n",Record.kprintf_cnt++);
    }
    return RT_EOK;
}
