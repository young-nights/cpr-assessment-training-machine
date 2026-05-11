/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-08-30     zphu       the first version
 */
#include "falFlash_Test.h"







/**
 * @brief  片内 Flash / 分区 读写擦除测试函数（适配 STM32F103ZET6）
 * @param  fal_index : FAL_READ_INDEX / FAL_WRITE_INDEX / FAL_ERASE_INDEX
 * @param  dev_name  : 设备名或分区名 ("stm32_onchip" 或 "app" 或 "data")
 * @param  offset    : 相对偏移地址
 * @param  size      : 操作大小（字节）
 * @param  wr_data   : 写入时的数据缓冲区
 * @return None
 */
#define FAL_HEX_WIDTH                 16

void falFlash_Onchip_Optional(uint8_t fal_index, char *dev_name, uint32_t offset, uint32_t size, uint8_t *wr_data)
{
#define __is_print(ch)   ((unsigned int)((ch) - ' ') < 127u - ' ')

    int result = 0;
    size_t i = 0, j = 0;
    const struct fal_flash_dev *flash_dev = NULL;
    const struct fal_partition *part_dev = NULL;

    /* 查找设备或分区 */
    if ((flash_dev = fal_flash_device_find(dev_name)) != NULL) {
        part_dev = NULL;
    } else if ((part_dev = fal_partition_find(dev_name)) != NULL) {
        flash_dev = NULL;
    } else {
        rt_kprintf("【FAL】Device or partition '%s' NOT found!\n", dev_name);
        fal_show_part_table();
        return;
    }

    /* 打印设备信息 */
    if (flash_dev) {
        rt_kprintf("【FAL】Flash Device: %s | addr: 0x%08X | len: %dKB\n",
                   flash_dev->name, flash_dev->addr, flash_dev->len / 1024);
    } else if (part_dev) {
        rt_kprintf("【FAL】Partition: %s | flash: %s | offset: 0x%08X | len: %dKB\n",
                   part_dev->name, part_dev->flash_name, part_dev->offset, part_dev->len / 1024);
    }

    /*****************************************************************************************************************/
    /* 读操作 */
    /*****************************************************************************************************************/
    if (fal_index == FAL_READ_INDEX) {
        uint8_t *data = rt_malloc(size);
        if (data) {
            if (flash_dev) {
                result = flash_dev->ops.read(offset, data, size);
            } else if (part_dev) {
                result = fal_partition_read(part_dev, offset, data, size);
            }

            if (result >= 0) {
                rt_kprintf("Read success! Addr: 0x%08X, Size: %d bytes\n", offset, size);
                rt_kprintf("Offset(h)  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");

                for (i = 0; i < (uint32_t)size; i += FAL_HEX_WIDTH) {
                    rt_kprintf("[%08X] ", offset + i);
                    for (j = 0; j < FAL_HEX_WIDTH; j++) {
                        if (i + j < (uint32_t)size)
                            rt_kprintf("%02X ", data[i + j]);
                        else
                            rt_kprintf("   ");
                    }
                    for (j = 0; j < FAL_HEX_WIDTH; j++) {
                        if (i + j < (uint32_t)size)
                            rt_kprintf("%c", __is_print(data[i + j]) ? data[i + j] : '.');
                    }
                    rt_kprintf("\n");
                }
            } else {
                rt_kprintf("Read failed! Error code: %d\n", result);
            }
            rt_free(data);
        } else {
            rt_kprintf("Low memory!\n");
        }
    }

    /*****************************************************************************************************************/
    /* 擦除操作 */
    /*****************************************************************************************************************/
    if (fal_index == FAL_ERASE_INDEX) {
        if (flash_dev) {
            result = flash_dev->ops.erase(offset, size);
        } else if (part_dev) {
            result = fal_partition_erase(part_dev, offset, size);
        }

        if (result >= 0) {
            rt_kprintf("Erase success! Addr: 0x%08X, Size: %d bytes\n", offset, size);
        } else {
            rt_kprintf("Erase failed! Error code: %d\n", result);
        }
    }

    /*****************************************************************************************************************/
    /* 写操作 */
    /*****************************************************************************************************************/
    if (fal_index == FAL_WRITE_INDEX && wr_data) {
        uint8_t *data = rt_malloc(size);
        if (data) {
            memcpy(data, wr_data, size);   // 更安全

            if (flash_dev) {
                result = flash_dev->ops.write(offset, data, size);
            } else if (part_dev) {
                result = fal_partition_write(part_dev, offset, data, size);
            }

            if (result >= 0) {
                rt_kprintf("Write success! Addr: 0x%08X, Size: %d bytes\n", offset, size);
            } else {
                rt_kprintf("Write failed! Error code: %d\n", result);
            }
            rt_free(data);
        } else {
            rt_kprintf("Low memory!\n");
        }
    }
}










