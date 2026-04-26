/**
  * @file    app_calculator.c
  * @brief   CPR成绩计算模块（从光栅板移至Sensor板）
  *
  * Change Logs:
  * Date           Author       Notes
  * 2026-04-27     coder       从光栅板移植成绩计算逻辑至Sensor板
  */

#include "app_calculator.h"
#include "rtthread.h"

/* ====== 获取系统滴答(ms) ====== */
static uint32_t get_tick_ms(void)
{
    return (uint32_t)rt_tick_get() * (1000 / RT_TICK_PER_SECOND);
}

/* ====== 内部状态 ====== */

/* 按压状态追踪 */
typedef struct {
    uint16_t depth_01mm;          /* 当前深度 (0.1mm) */
    int8_t   dir;                 /* 当前方向 */
    uint8_t  in_press_cycle;      /* 是否在按压周期内 */
    uint32_t press_start_tick;    /* 按压开始时刻 */
    uint32_t rebound_start_tick;  /* 回弹开始时刻 */
    uint16_t max_depth;           /* 当前按压周期最大深度 */
    uint8_t  rebound_detected;    /* 是否检测到回弹 */
} press_state_t;

/* 吹气状态追踪 */
typedef struct {
    uint16_t depth_01mm;          /* 当前深度 (0.1mm) */
    int8_t   dir;                 /* 当前方向 */
    uint8_t  in_blow_cycle;       /* 是否在吹气周期内 */
    uint32_t blow_start_tick;     /* 吹气开始时刻 */
    uint16_t max_depth;           /* 当前吹气周期最大深度 */
    uint8_t  exhale_detected;     /* 是否检测到泄气 */
} blow_state_t;

/* 单次按压记录 */
typedef struct {
    uint16_t depth_01mm;          /* 按压深度 (0.1mm) */
    uint16_t freq_10x;            /* 频率 x10 (次/分) */
    uint8_t  too_deep;            /* 过深 */
    uint8_t  too_shallow;         /* 不足 */
    uint8_t  no_rebound;          /* 未回弹 */
} press_record_t;

/* 单次吹气记录 */
typedef struct {
    uint16_t depth_01mm;          /* 吹气深度 (0.1mm) */
    uint16_t duration_ms;         /* 吹气持续时间 */
    uint8_t  too_much;            /* 过大 */
    uint8_t  too_little;          /* 不足 */
} blow_record_t;

#define MAX_PRESS_RECORDS   200
#define MAX_BLOW_RECORDS    40

static press_state_t  s_press;
static blow_state_t   s_blow;

static press_record_t s_press_records[MAX_PRESS_RECORDS];
static uint16_t       s_press_count = 0;

static blow_record_t  s_blow_records[MAX_BLOW_RECORDS];
static uint16_t       s_blow_count = 0;

static uint32_t       s_start_tick = 0;       /* 训练开始时刻 */
static uint32_t       s_last_press_tick = 0;   /* 上一次按压完成时刻 */
static uint16_t       s_cycle_press_count = 0; /* 当前循环按压计数 */
static uint16_t       s_cycle_blow_count = 0;  /* 当前循环吹气计数 */
static uint8_t        s_cycle_count = 0;       /* 已完成循环数 */
static uint32_t       s_cycle_gap_sum = 0;     /* 循环间间隔时间累计 */
static uint8_t        s_cycle_gap_count = 0;   /* 循环间隔计数 */

/* ====== 辅助函数 ====== */

/**
  * @brief  判断按压是否达标
  */
static uint8_t is_press_correct(uint16_t depth, uint16_t freq_10x)
{
    if (depth < PRESS_DEPTH_MIN_01MM || depth > PRESS_DEPTH_MAX_01MM)
        return 0;
    if (freq_10x < (PRESS_FREQ_MIN * 10) || freq_10x > (PRESS_FREQ_MAX * 10))
        return 0;
    return 1;
}

/**
  * @brief  判断吹气是否达标
  */
static uint8_t is_blow_correct(uint16_t depth)
{
    if (depth < BLOW_DEPTH_MIN_01MM || depth > BLOW_DEPTH_MAX_01MM)
        return 0;
    return 1;
}

/* ====== 公开接口 ====== */

/**
  * @brief  初始化计算器，重置所有状态
  */
void Calculator_Init(void)
{
    /* 清零按压状态 */
    memset(&s_press, 0, sizeof(press_state_t));
    /* 清零吹气状态 */
    memset(&s_blow, 0, sizeof(blow_state_t));
    /* 清零记录 */
    memset(s_press_records, 0, sizeof(s_press_records));
    memset(s_blow_records, 0, sizeof(s_blow_records));

    s_press_count = 0;
    s_blow_count = 0;
    s_start_tick = get_tick_ms();
    s_last_press_tick = 0;
    s_cycle_press_count = 0;
    s_cycle_blow_count = 0;
    s_cycle_count = 0;
    s_cycle_gap_sum = 0;
    s_cycle_gap_count = 0;
}

/**
  * @brief  处理每个采样周期的按压/吹气数据
  * @param  press_depth: 当前按压深度 (0.1mm)
  * @param  press_dir:   按压方向 (-1=回弹, 0=静止, 1=下压)
  * @param  blow_depth:  当前吹气深度 (0.1mm)
  * @param  blow_dir:    吹气方向 (-1=泄气, 0=静止, 1=充气)
  */
void Calculator_Process(uint16_t press_depth, int8_t press_dir,
                        uint16_t blow_depth, int8_t blow_dir)
{
    uint32_t now = get_tick_ms();

    /* ====== 按压处理 ====== */

    /* 检测按压开始: 方向从静止/回弹变为下压 */
    if (press_dir == 1 && s_press.dir != 1)
    {
        /* 新的按压开始 */
        s_press.in_press_cycle = 1;
        s_press.press_start_tick = now;
        s_press.max_depth = press_depth;
        s_press.rebound_detected = 0;

        /* 计算循环间隔 (上一次按压完成到本次按压开始) */
        if (s_last_press_tick > 0 && s_press_count > 0)
        {
            uint32_t gap = now - s_last_press_tick;
            s_cycle_gap_sum += gap;
            s_cycle_gap_count++;
        }
    }

    /* 按压进行中: 更新最大深度 */
    if (s_press.in_press_cycle && press_dir == 1)
    {
        if (press_depth > s_press.max_depth)
        {
            s_press.max_depth = press_depth;
        }
    }

    /* 检测回弹: 方向从下压变为回弹或静止 */
    if (s_press.in_press_cycle && s_press.dir == 1 && press_dir != 1)
    {
        s_press.rebound_detected = 1;
        s_press.rebound_start_tick = now;
    }

    /* 按压结束: 回弹后恢复到静止 */
    if (s_press.in_press_cycle && s_press.rebound_detected && press_dir == 0)
    {
        /* 记录本次按压 */
        if (s_press_count < MAX_PRESS_RECORDS)
        {
            uint16_t freq_10x = 0;

            /* 计算频率: 60000ms / 按压周期(ms) */
            if (s_press_count > 0 && s_last_press_tick > 0)
            {
                uint32_t period = now - s_last_press_tick;
                if (period > 0)
                    freq_10x = (uint16_t)(600000UL / period);
            }

            s_press_records[s_press_count].depth_01mm = s_press.max_depth;
            s_press_records[s_press_count].freq_10x = freq_10x;
            s_press_records[s_press_count].too_deep = (s_press.max_depth > PRESS_DEPTH_MAX_01MM) ? 1 : 0;
            s_press_records[s_press_count].too_shallow = (s_press.max_depth < PRESS_DEPTH_MIN_01MM) ? 1 : 0;
            s_press_records[s_press_count].no_rebound = 0;

            s_press_count++;
            s_cycle_press_count++;
            s_last_press_tick = now;

            /* 检查是否完成一个循环 (30次按压) */
            if (s_cycle_press_count >= CYCLE_PRESS_COUNT)
            {
                /* 按压阶段完成，等待吹气 */
            }
        }

        s_press.in_press_cycle = 0;
        s_press.rebound_detected = 0;
    }

    /* 检测未回弹: 按压后方向变为下压而没有经过静止 */
    if (s_press.in_press_cycle && s_press.rebound_detected == 0 &&
        s_press.dir == 1 && press_dir == 1 && press_depth == 0)
    {
        /* 深度归零但方向仍为下压，可能是异常 */
    }

    s_press.depth_01mm = press_depth;
    s_press.dir = press_dir;

    /* ====== 吹气处理 ====== */

    /* 检测吹气开始: 方向从静止/泄气变为充气 */
    if (blow_dir == 1 && s_blow.dir != 1)
    {
        s_blow.in_blow_cycle = 1;
        s_blow.blow_start_tick = now;
        s_blow.max_depth = blow_depth;
        s_blow.exhale_detected = 0;
    }

    /* 吹气进行中: 更新最大深度 */
    if (s_blow.in_blow_cycle && blow_dir == 1)
    {
        if (blow_depth > s_blow.max_depth)
        {
            s_blow.max_depth = blow_depth;
        }
    }

    /* 检测泄气: 方向从充气变为泄气或静止 */
    if (s_blow.in_blow_cycle && s_blow.dir == 1 && blow_dir != 1)
    {
        s_blow.exhale_detected = 1;
    }

    /* 吹气结束: 泄气后恢复到静止 */
    if (s_blow.in_blow_cycle && s_blow.exhale_detected && blow_dir == 0)
    {
        /* 记录本次吹气 */
        if (s_blow_count < MAX_BLOW_RECORDS)
        {
            uint32_t duration = now - s_blow.blow_start_tick;

            s_blow_records[s_blow_count].depth_01mm = s_blow.max_depth;
            s_blow_records[s_blow_count].duration_ms = (uint16_t)((duration > 65535) ? 65535 : duration);
            s_blow_records[s_blow_count].too_much = (s_blow.max_depth > BLOW_DEPTH_MAX_01MM) ? 1 : 0;
            s_blow_records[s_blow_count].too_little = (s_blow.max_depth < BLOW_DEPTH_MIN_01MM) ? 1 : 0;

            s_blow_count++;
            s_cycle_blow_count++;

            /* 检查是否完成一个循环 (30次按压 + 2次吹气) */
            if (s_cycle_press_count >= CYCLE_PRESS_COUNT &&
                s_cycle_blow_count >= CYCLE_BLOW_COUNT)
            {
                s_cycle_count++;
                s_cycle_press_count = 0;
                s_cycle_blow_count = 0;
            }
        }

        s_blow.in_blow_cycle = 0;
        s_blow.exhale_detected = 0;
    }

    s_blow.depth_01mm = blow_depth;
    s_blow.dir = blow_dir;
}

/**
  * @brief  最终计算，生成成绩报告
  * @param  report: 输出成绩数据结构指针
  */
void Calculator_Finalize(RasterReport_t *report)
{
    uint32_t now = get_tick_ms();
    uint32_t total_ms;
    uint32_t press_depth_sum = 0;
    uint32_t press_freq_sum = 0;
    uint32_t blow_time_sum = 0;
    uint32_t blow_depth_sum = 0;
    uint16_t press_correct_count = 0;
    uint16_t blow_correct_count = 0;
    uint16_t press_too_deep_count = 0;
    uint16_t press_too_shallow_count = 0;
    uint16_t press_no_rebound_count = 0;
    uint16_t blow_too_much_count = 0;
    uint16_t blow_too_little_count = 0;
    uint16_t i;

    if (report == NULL)
        return;

    memset(report, 0, sizeof(RasterReport_t));

    /* 总用时 */
    total_ms = now - s_start_tick;
    report->total_time = (uint16_t)(total_ms / 1000);

    /* 完成循环数 */
    report->cycle_count = s_cycle_count;

    /* ====== 按压统计 ====== */
    report->press_total = s_press_count;

    for (i = 0; i < s_press_count; i++)
    {
        press_depth_sum += s_press_records[i].depth_01mm;
        press_freq_sum += s_press_records[i].freq_10x;

        if (is_press_correct(s_press_records[i].depth_01mm, s_press_records[i].freq_10x))
            press_correct_count++;

        if (s_press_records[i].too_deep)
            press_too_deep_count++;
        if (s_press_records[i].too_shallow)
            press_too_shallow_count++;
        if (s_press_records[i].no_rebound)
            press_no_rebound_count++;
    }

    report->press_correct = press_correct_count;
    report->press_too_deep = press_too_deep_count;
    report->press_too_shallow = press_too_shallow_count;
    report->press_no_rebound = press_no_rebound_count;

    /* 按压错误总数 */
    report->press_error_total = press_too_deep_count + press_too_shallow_count +
                                 press_no_rebound_count + report->press_position_err;

    /* 平均深度 */
    if (s_press_count > 0)
        report->press_depth_avg = (uint16_t)(press_depth_sum / s_press_count);

    /* 平均频率 */
    if (s_press_count > 0)
        report->press_freq_avg = (uint16_t)(press_freq_sum / s_press_count);

    /* 正确率 */
    if (s_press_count > 0)
        report->press_accuracy = (uint16_t)((uint32_t)press_correct_count * 100 / s_press_count);

    /* 5循环内/外按压数 */
    report->press_in_5_cycles = (s_press_count <= CYCLE_PRESS_COUNT * CPR_TOTAL_CYCLES) ?
                                 s_press_count : (CYCLE_PRESS_COUNT * CPR_TOTAL_CYCLES);
    report->press_out_5_cycles = (s_press_count > CYCLE_PRESS_COUNT * CPR_TOTAL_CYCLES) ?
                                  (s_press_count - CYCLE_PRESS_COUNT * CPR_TOTAL_CYCLES) : 0;

    /* 少按/多按 */
    uint16_t expected_press = CYCLE_PRESS_COUNT * s_cycle_count;
    if (s_press_count < expected_press)
        report->press_too_few = expected_press - s_press_count;
    else if (s_press_count > expected_press)
        report->press_too_many = s_press_count - expected_press;

    /* 频率不达标次数 */
    for (i = 0; i < s_press_count; i++)
    {
        if (s_press_records[i].freq_10x > 0 &&
            (s_press_records[i].freq_10x < PRESS_FREQ_MIN * 10 ||
             s_press_records[i].freq_10x > PRESS_FREQ_MAX * 10))
        {
            report->press_out_of_std++;
        }
    }

    /* ====== 吹气统计 ====== */
    report->blow_total = s_blow_count;

    for (i = 0; i < s_blow_count; i++)
    {
        blow_depth_sum += s_blow_records[i].depth_01mm;
        blow_time_sum += s_blow_records[i].duration_ms;

        if (is_blow_correct(s_blow_records[i].depth_01mm))
            blow_correct_count++;

        if (s_blow_records[i].too_much)
            blow_too_much_count++;
        if (s_blow_records[i].too_little)
            blow_too_little_count++;
    }

    report->blow_correct = blow_correct_count;
    report->blow_too_much = blow_too_much_count;
    report->blow_too_little = blow_too_little_count;
    report->blow_error_total = blow_too_much_count + blow_too_little_count;

    /* 吹气平均时间 */
    if (s_blow_count > 0)
        report->blow_avg_time = (uint16_t)(blow_time_sum / s_blow_count);

    /* 平均潮气量 */
    if (s_blow_count > 0)
        report->tidal_avg = (uint16_t)(blow_depth_sum / s_blow_count);

    /* 吹气正确率 */
    if (s_blow_count > 0)
        report->blow_accuracy = (uint16_t)((uint32_t)blow_correct_count * 100 / s_blow_count);

    /* 5循环内/外吹气数 */
    report->blow_in_5_cycles = (s_blow_count <= CYCLE_BLOW_COUNT * CPR_TOTAL_CYCLES) ?
                                s_blow_count : (CYCLE_BLOW_COUNT * CPR_TOTAL_CYCLES);
    report->blow_out_5_cycles = (s_blow_count > CYCLE_BLOW_COUNT * CPR_TOTAL_CYCLES) ?
                                 (s_blow_count - CYCLE_BLOW_COUNT * CPR_TOTAL_CYCLES) : 0;

    /* 少吹/多吹 */
    uint16_t expected_blow = CYCLE_BLOW_COUNT * s_cycle_count;
    if (s_blow_count < expected_blow)
        report->blow_too_few = expected_blow - s_blow_count;
    else if (s_blow_count > expected_blow)
        report->blow_too_many = s_blow_count - expected_blow;

    /* 循环间隔平均时间 */
    if (s_cycle_gap_count > 0)
        report->cycle_gap_time = (uint16_t)(s_cycle_gap_sum / s_cycle_gap_count);

    /* ====== 总体结果 ====== */
    /* 成功条件: 完成5个循环，按压正确率>=60%，吹气正确率>=60% */
    if (s_cycle_count >= CPR_TOTAL_CYCLES &&
        report->press_accuracy >= 60 &&
        report->blow_accuracy >= 60)
    {
        report->result = 1; /* 成功 */
    }
    else
    {
        report->result = 0; /* 失败 */
    }
}
