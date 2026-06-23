#include <bsp_mpu6050_calibration.h>
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-02-19     Administrator       the first version
 */



//以下是MPU6500数据进行静态校准函数驱动--------------------------------------------------------------------------------------------------------
// 校准数据结构体
typedef struct {
    int16_t accel_min[3];  // 各轴最小值
    int16_t accel_max[3];  // 各轴最大值
    int16_t gyro_offset[3];// 陀螺仪偏移
} mpu6xxx_calib_data;


mpu6xxx_calib_data mpu6050_cali;


// 建议把这些极值和计数器做成 static 或全局
static int16_t accel_cali_min[3] = {32767, 32767, 32767};
static int16_t accel_cali_max[3] = {-32768, -32768, -32768};
static uint8_t accel_cali_count = 0;


// 可选：每次开始校准前重置（可以另外写一个开始校准的函数调用）
void mpu6xxx_accel_cali_reset(void)
{
    for(int i = 0; i < 3; i++) {
        accel_cali_min[i] = 32767;
        accel_cali_max[i] = -32768;
    }
    accel_cali_count = 0;
}


/* 陀螺仪校准函数（需静止放置） */
extern rt_err_t mpu6xxx_get_gyro_raw(struct mpu6xxx_device *dev, struct mpu6xxx_3axes *gyro);
rt_err_t bsp_mpu6xxx_calibrate_gyro(struct mpu6xxx_device *dev, mpu6xxx_calib_data *calib)
{
    struct mpu6xxx_3axes gyro;
    int32_t sum[3] = {0};
    const uint16_t samples = 200;

    // 采集数据
    for(int i=0; i<samples; i++) {
        mpu6xxx_get_gyro_raw(dev, &gyro);
        sum[0] += gyro.x;
        sum[1] += gyro.y;
        sum[2] += gyro.z;
        rt_thread_mdelay(10);
    }

    // 计算平均值
    calib->gyro_offset[0] = sum[0] / samples;
    calib->gyro_offset[1] = sum[1] / samples;
    calib->gyro_offset[2] = sum[2] / samples;

    // 写入偏移寄存器
    struct mpu6xxx_3axes offset = {
        calib->gyro_offset[0],
        calib->gyro_offset[1],
        calib->gyro_offset[2]
    };

    return mpu6xxx_set_gyro_offset(dev, &offset);
}









/* 加速度计六面校准函数 */
extern rt_err_t mpu6xxx_get_accel_raw(struct mpu6xxx_device *dev, struct mpu6xxx_3axes *accel);
rt_err_t mpu6xxx_calibrate_accel_6side(struct mpu6xxx_device *dev, mpu6xxx_calib_data *calib, rt_uint8_t sideValue)
{
    struct mpu6xxx_3axes accel;
    int16_t local_min[3] = {32767, 32767, 32767};
    int16_t local_max[3] = {-32768, -32768, -32768};

    // 打印当前面（提升用户体验）
    const char *face_name[] = {"", " +X", " -X", " +Y", " -Y", " +Z", " -Z"};
    if (sideValue >= 1 && sideValue <= 6) {
        rt_kprintf("开始校准第 %d 面：%s 朝下/朝上，请保持稳定 5-10 秒...\n",accel_cali_count + 1, face_name[sideValue]);
    } else {
        rt_kprintf("错误：无效的面编号 %d\n", sideValue);
        return RT_ERROR;
    }


    // 采集 50 组数据，计算本面局部极值
    for (int i = 0; i < 50; i++) {
        if (mpu6xxx_get_accel_raw(dev, &accel) != RT_EOK) {
            rt_kprintf("读取加速度失败！\n");
            return RT_ERROR;
        }

        // 统一用 int16_t 读取（最重要！）
        int16_t *raw = (int16_t *)&accel;
        for (int axis = 0; axis < 3; axis++) {
            int16_t val = raw[axis];
            if (val < local_min[axis]) local_min[axis] = val;
            if (val > local_max[axis]) local_max[axis] = val;
        }
        rt_thread_mdelay(10);
    }

    // 更新全局极值（跨所有面取最极端的）
    for (int axis = 0; axis < 3; axis++) {
        if (local_min[axis] < accel_cali_min[axis]) accel_cali_min[axis] = local_min[axis];
        if (local_max[axis] > accel_cali_max[axis]) accel_cali_max[axis] = local_max[axis];
    }

    accel_cali_count++;
    rt_kprintf("第 %d 面采集完成，总进度：%d/6\n", sideValue, accel_cali_count);

    // 六面全部完成
    if (accel_cali_count >= 6) {
        int16_t offset[3];
        for (int i = 0; i < 3; i++) {
            offset[i] = (accel_cali_max[i] + accel_cali_min[i]) / 2;
            // 可选：检查是否合理（max-min 应该接近 2g ≈ 32768）
            int16_t range = accel_cali_max[i] - accel_cali_min[i];
            if (range < 20000 || range > 40000) {
                rt_kprintf("警告：轴 %d 范围异常 (%d ~ %d)\n", i, accel_cali_min[i], accel_cali_max[i]);
            }
        }

        struct mpu6xxx_3axes accel_offset = { offset[0], offset[1], offset[2] };
        rt_err_t ret = mpu6xxx_set_accel_offset(dev, &accel_offset);

        if (ret == RT_EOK) {
            // 保存到 calib 结构体（如果需要持久化）
            memcpy(calib->accel_min, accel_cali_min, sizeof(accel_cali_min));
            memcpy(calib->accel_max, accel_cali_max, sizeof(accel_cali_max));

            rt_kprintf("六面校准完成！偏移值：X:%d, Y:%d, Z:%d\n",offset[0], offset[1], offset[2]);
        } else {
            rt_kprintf("写入加速度偏移失败！\n");
        }

        // 校准结束，重置计数器
        accel_cali_count = 0;
        return ret;
    }

    return RT_EOK;
}


//------------------------------------------------------------------------------------------------------


/**
  * @brief  MPU6XXX校准回调线程函数入口
  * @retval void
  */
void mpu6xxx_cali_thread_entry(void* parameter)
{

    while(1)
    {
        if(mpu6xxxParameter.if_start_master_cali_process == 1){

        }
        else{

        }


        /* 是否开启陀螺仪静态校准 */
        if(mpu6xxxParameter.if_start_gyro_cali_process == 1){
            rt_err_t if_finish_cali = bsp_mpu6xxx_calibrate_gyro(mpu6050_dev,&mpu6050_cali);
            if(if_finish_cali == RT_EOK){
                mpu6xxxParameter.if_start_gyro_cali_process = 0;
                rt_kprintf("Now is finished gyro static calibration.\r\n");

            }
            else{
                rt_kprintf("Now is not finished gyro static calibration.\r\n");

            }
        }


        rt_thread_mdelay(100);
    }
}




/**
  * @brief  初始化MPU6XXX的校准线程函数
  * @retval int
  */
rt_thread_t mpu6xxx_calibrate_Thread_Handle;
int bsp_mpu6xxx_calibrate_Thread_Init(void)
{
    mpu6xxx_calibrate_Thread_Handle = rt_thread_create("mpu6xxx_cali_thread_entry", mpu6xxx_cali_thread_entry, RT_NULL, 3072, 6, 500);
    if(mpu6xxx_calibrate_Thread_Handle != RT_NULL){
        rt_kprintf("PRINTF:%d. mpu6xxx calibrate thread is created!!\r\n",Record.kprintf_cnt++);
        rt_thread_startup(mpu6xxx_calibrate_Thread_Handle);
    }
    else {
        rt_kprintf("PRINTF:%d. mpu6xxx calibrate thread is not created!!\r\n",Record.kprintf_cnt++);
    }
    return RT_EOK;
}







