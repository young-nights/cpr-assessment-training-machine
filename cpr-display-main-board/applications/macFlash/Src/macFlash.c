/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-07-18     zphu       the first version
 */
#include "macFlash.h"
#include <stm32f1xx.h>
#include <fal.h>


#ifndef FAL_USING_STM32F1_ONCHIP_FLASH_DEV_NAME
#define FAL_USING_STM32F1_ONCHIP_FLASH_DEV_NAME             "stm32_onchip"
#endif

static int init(void);
static int read(long offset, uint8_t *buf, size_t size);
static int write(long offset, const uint8_t *buf, size_t size);
static int erase(long offset, size_t size);


const struct fal_flash_dev stm32f1_onchip_flash =
{
    .name       = FAL_USING_STM32F1_ONCHIP_FLASH_DEV_NAME,
    .addr       = 0x08000000,
    .len        = 512 * 1024,      // 必须是 512KB
    .blk_size   = 2048,            // STM32F1 Page Size = 2KB
    .ops        = {init, read, write, erase},
    .write_gran = 8,
};



static int init(void)
{
    /* do nothing now */
    return RT_EOK;
}



/**
 * @brief  片内flash读取指定地址的数据
 * @param  offset     : 读取数据的 Flash 偏移地址
 *         *buf       : 存放待读取数据的缓冲区
 *         size       ：待读取数据的大小
 * @return 返回实际读取的数据大小
 */
static int read(long offset, uint8_t *buf, size_t size)
{
    size_t i;
    uint32_t addr = stm32f1_onchip_flash.addr + offset;
    for (i = 0; i < size; i++, addr++, buf++)
    {
        *buf = *(uint8_t *) addr;
    }

    return size;
}




/**
 * @brief  片内flash向指定地址写入数据 - 每次写入一个字（4个字节）
 * @param  offset     : 写入数据的 Flash 偏移地址
 *         *buf       : 存放待读取数据的缓冲区
 *         size       ：待写入数据的大小（个数/字）
 * @return 返回实际写入的数据大小
 */
static int write(long offset, const uint8_t *buf, size_t size)
{
    size_t i;
    uint32_t read_data;
    uint32_t addr = stm32f1_onchip_flash.addr + offset;

    HAL_FLASH_Unlock();

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);             /*!< FLASH End of Operation flag              */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);         /*!< Option Byte Error                        */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGERR);           /*!< FLASH Programming error flag             */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);          /*!< FLASH Write protected error flag         */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY);             /*!< FLASH Busy flag                          */
    for (i = 0; i < size; i++, buf++)
    {
        /* write data */
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, *buf);
        read_data = *(uint32_t *) addr;
        addr += 4;
        /* check data */
        if (read_data != *buf)
        {
            return -1;
        }
    }
    HAL_FLASH_Lock();

    return size;
}


/**
 * @brief  片内flash擦除指定地址的数据
 * @param  offset     : 写入数据的 Flash 偏移地址
 *         size       ：待读取数据的大小
 * @return 实际擦除区域的大小
 */
static int erase(long offset, size_t size)
{
    FLASH_EraseInitTypeDef EraseInitStruct = {0};
    uint32_t PAGEError = 0;

    uint32_t addr = stm32f1_onchip_flash.addr + offset;

    /* start erase */
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);             /*!< FLASH End of Operation flag                */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);         /*!< Option Byte Error                          */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGERR);           /*!< FLASH Programming error flag               */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);          /*!< FLASH Write protected error flag           */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY);             /*!< FLASH Busy flag                            */

    // 配置擦除操作
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;/*!< Erase sector                               */
    EraseInitStruct.PageAddress = addr;                 /*!< Data sector start addr                     */
    EraseInitStruct.NbPages = size;                     /*!< Erase one page size sector                 */
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
    {
        HAL_FLASH_Lock();
        Error_Handler( );
    }
    HAL_FLASH_Lock();

    return size;
}







