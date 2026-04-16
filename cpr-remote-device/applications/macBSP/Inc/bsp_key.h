/*
 * bsp_key.h
 *
 *  Created on: May 16, 2024
 *      Author: zphu
 */

#ifndef MACBSP_INC_BSP_KEY_H_

#define MACBSP_INC_BSP_KEY_H_

#include "bsp_sys.h"





typedef enum {
    Matrix_Column_1 = (0x01),
    Matrix_Column_2,
    Matrix_Column_3,
} MatrixKey_ColumnName_TypeDef;

typedef enum {
    Matrix_Row_1 = (0x01),
    Matrix_Row_2,
    Matrix_Row_3,
} MatrixKey_RowName_TypeDef;

typedef enum {
    Matrix_RESET = 0,
    Matrix_SET
} MatrixKey_Status_TypeDef;

/* Non-blocking scan specific */
typedef enum {
    KEY_NONE = 0,
    KEY_RELEASE,
    KEY_PRESS,
} key_state_t;

typedef struct {
    uint8_t row;        /* 1~3 */
    uint8_t col;        /* 1~3 */
    uint8_t state;      /* KEY_PRESS / KEY_RELEASE */
} key_event_t;

/* ---------- 消抖宏 ---------- */
#define DEBOUNCE_THRESHOLD   5   /* 计数阈值，约 5×10 ms = 50 ms */

void MatrixKey_Scan(void);


#endif /* MACBSP_INC_BSP_KEY_H_ */
