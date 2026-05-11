/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-04-24     Administrator       the first version
 */
#ifndef APPLICATIONS_MACSYS_INC_CPR_RECORD_H_
#define APPLICATIONS_MACSYS_INC_CPR_RECORD_H_

#include "bsp_sys.h"


#define CPR_RECORD_MAGIC        0x43505221   // "CPR!"
#define CPR_MAX_RECORDS         100

typedef struct {
    uint32_t timestamp;           // 系统时间戳

    /* 基本信息 */
    uint8_t  mode;                // 0=训练 1=考核 2=竞赛
    uint8_t  score;               // 综合评分 0~100

    /* 按压参数 */
    uint8_t  press_avg_freq;      // 平均按压频率
    uint8_t  press_qualified_rate; // 按压合格率（%）
    uint16_t press_total;         // 按压总数
    uint16_t press_correct;       // 按压正确次数
    uint16_t press_wrong;         // 按压错误总数
    uint16_t press_less;          // 少按次数
    uint16_t press_more;          // 多按次数
    uint16_t press_position_err;  // 位置错误
    uint16_t press_insufficient;  // 按压不足
    uint16_t press_too_hard;      // 按压过大
    uint16_t press_no_rebound;    // 未回弹次数

    /* 吹气参数 */
    uint8_t  blow_avg_time;       // 平均吹气时间（秒/次）
    uint8_t  blow_qualified_rate; // 吹气合格率（%）
    uint16_t blow_total;          // 吹气总数
    uint16_t blow_correct;        // 吹气正确次数
    uint16_t blow_wrong;          // 吹气错误总数
    uint16_t blow_less;           // 少吹次数
    uint16_t blow_more;           // 多吹次数
    uint16_t blow_too_hard;       // 吹气过大
    uint16_t blow_insufficient;   // 吹气不足

    /* 循环统计 */
    uint16_t cycle5_press;        // 5个循环内按压总数
    uint16_t cycle5_blow;         // 5个循环内吹气总数
    uint16_t extra_press;         // 5个循环外按压总数
    uint16_t extra_blow;          // 5个循环外吹气总数

    uint32_t total_time_sec;      // 实际用时（秒）
    uint8_t  result;              // 0=不及格 1=及格 2=良好 3=优秀

    uint8_t  checksum;
} cpr_record_t;

int cpr_storage_init(void);
int cpr_save_record(const cpr_record_t *record);
int cpr_get_recent(cpr_record_t *records, int max_count);
void cpr_clear_all(void);






#endif /* APPLICATIONS_MACSYS_INC_CPR_RECORD_H_ */
