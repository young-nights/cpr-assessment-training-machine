/**
  * @file    app_calculator.h
  * @brief   CPR成绩计算模块（从光栅板移至Sensor板）
  *
  * Change Logs:
  * Date           Author       Notes
  * 2026-04-27     coder       从光栅板移植成绩计算逻辑至Sensor板
  */
#ifndef _APP_CALCULATOR_H_
#define _APP_CALCULATOR_H_

#include "bsp_sys.h"

/* ====== 成人CPR国际标准参数（AHA/ERC 2020指南） ====== */
#define PRESS_DEPTH_MIN_01MM     500    /* 最小按压深度 50mm */
#define PRESS_DEPTH_MAX_01MM     600    /* 最大按压深度 60mm */
#define PRESS_FREQ_MIN           100    /* 最小按压频率 100次/分 */
#define PRESS_FREQ_MAX           120    /* 最大按压频率 120次/分 */
#define BLOW_DEPTH_MIN_01MM      500    /* 最小吹气深度 50mm, 约500ml */
#define BLOW_DEPTH_MAX_01MM      600    /* 最大吹气深度 60mm, 约600ml */
#define CYCLE_PRESS_COUNT        30     /* 每个循环按压次数 */
#define CYCLE_BLOW_COUNT         2      /* 每个循环吹气次数 */
#define CPR_TOTAL_CYCLES         5      /* 标准CPR共5个循环 */

/* ====== 成绩数据结构（64字节） ====== */
typedef struct __attribute__((packed)) {
    /* --- 按压报告 (13 x uint16_t = 26B) --- */
    uint16_t press_total;         /* 按压总次数 */
    uint16_t press_correct;       /* 按压正确次数 */
    uint16_t press_out_of_std;    /* 标准频率外次数 */
    uint16_t press_error_total;   /* 按压错误总数 */
    uint16_t press_accuracy;      /* 正确率 (0-100, 单位%) */
    uint16_t press_too_few;       /* 少按次数 */
    uint16_t press_too_many;      /* 多按次数 */
    uint16_t press_depth_avg;     /* 平均按压深度 (0.1mm) */
    uint16_t press_freq_avg;      /* 平均按压频率 (次/分) */
    uint16_t press_too_deep;      /* 按压过大次数 (深度>60mm) */
    uint16_t press_too_shallow;   /* 按压不足次数 (深度<50mm) */
    uint16_t press_no_rebound;    /* 未回弹数 */
    uint16_t press_position_err;  /* 位置错误次数 */

    /* --- 吹气报告 (11 x uint16_t = 22B) --- */
    uint16_t blow_avg_time;       /* 吹气平均时间 (ms) */
    uint16_t cycle_gap_time;      /* 两个循环间按压间断时间 (ms) */
    uint16_t blow_error_total;    /* 吹气错误总数 */
    uint16_t blow_correct;        /* 吹气正确次数 */
    uint16_t blow_accuracy;       /* 正确率 (0-100, 单位%) */
    uint16_t blow_too_few;        /* 少吹次数 */
    uint16_t blow_too_many;       /* 多吹次数 */
    uint16_t blow_too_much;       /* 吹气过大次数 */
    uint16_t blow_too_little;     /* 吹气不足次数 */
    uint16_t blow_total;          /* 吹气总次数 */
    uint16_t tidal_avg;           /* 平均潮气量 (0.1mm) */

    /* --- 循环统计 (4 x uint16_t = 8B) --- */
    uint16_t press_in_5_cycles;   /* 5个循环内按压总数 */
    uint16_t blow_in_5_cycles;    /* 5个循环内吹气总数 */
    uint16_t press_out_5_cycles;  /* 5个循环外按压总数 */
    uint16_t blow_out_5_cycles;   /* 5个循环外吹气总数 */

    /* --- 结果 (1xuint16_t + 2xuint8_t = 4B) --- */
    uint16_t total_time;          /* 实际用时(秒) */
    uint8_t  cycle_count;         /* 完成循环数 */
    uint8_t  result;              /* 急救结果 (0:失败 1:成功) */

    uint8_t  reserved[4];         /* 预留，凑齐64字节 */
} RasterReport_t;

void Calculator_Init(void);
void Calculator_Process(uint16_t press_depth, int8_t press_dir,
                        uint16_t blow_depth, int8_t blow_dir);
void Calculator_Finalize(RasterReport_t *report);

#endif /* _APP_CALCULATOR_H_ */
