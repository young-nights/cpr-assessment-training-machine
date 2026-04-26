/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PACKAGES_FAL_LATEST_INC_FAL_CONFIG_H_
#define PACKAGES_FAL_LATEST_INC_FAL_CONFIG_H_

#include <rtconfig.h>
#include <board.h>
#include "bsp_sys.h"

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev stm32f1_onchip_flash;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
    &stm32f1_onchip_flash,                                           \
}

/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table —— 无 Bootloader（不需要远程升级） */
#define FAL_PART_TABLE                                                          \
{                                                                               \
    {FAL_PART_MAGIC_WORD, "app",     "stm32_onchip",         0,   496*1024, 0}, \
    {FAL_PART_MAGIC_WORD, "data",    "stm32_onchip", 496*1024,    16*1024, 0},  \
}
#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* PACKAGES_FAL_LATEST_INC_FAL_CONFIG_H_ */
