/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-07-18     zphu       the first version
 */
#ifndef APPLICATIONS_MACSYS_INC_MACFLASH_H_
#define APPLICATIONS_MACSYS_INC_MACFLASH_H_
#include "bsp_sys.h"

/* 控制falFlash_Onchip_Optional函数：读\写\擦除，这三个模式的指令宏 */
#define FAL_READ_INDEX                1
#define FAL_WRITE_INDEX               2
#define FAL_ERASE_INDEX               3



#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
 #define PAGE_SIZE          (0x400)    //页的大小1K
 #define FLASH_SIZE         (0x20000)  //Flash空间128K
#elif defined STM32F10X_CL
 #define PAGE_SIZE          (0x400)    //页的大小1K
 #define FLASH_SIZE         (0x20000)  //Flash空间128K
#elif defined STM32F10X_HD || defined (STM32F10X_HD_VL)
 #define PAGE_SIZE          (0x800)    //页的大小2K
 #define FLASH_SIZE         (0x80000)  //Flash空间512K
#elif defined STM32F10X_XL
 #define PAGE_SIZE          (0x800)    //页的大小2K
 #define FLASH_SIZE         (0x100000) //Flash空间1M 
#else 
 #error "Please select first the STM32 device to be used (in stm32f10x.h)"    
#endif


/* 定义各个扇区的起始地址 */
/* Base address of the Flash sectors (STM32F103ZET6 - 512KB, 2KB per sector) */

#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Sector 0,   2 Kbyte */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08000800) /* Sector 1,   2 Kbyte */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08001000) /* Sector 2,   2 Kbyte */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x08001800) /* Sector 3,   2 Kbyte */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08002000) /* Sector 4,   2 Kbyte */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08002800) /* Sector 5,   2 Kbyte */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08003000) /* Sector 6,   2 Kbyte */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08003800) /* Sector 7,   2 Kbyte */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08004000) /* Sector 8,   2 Kbyte */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x08004800) /* Sector 9,   2 Kbyte */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x08005000) /* Sector 10,  2 Kbyte */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x08005800) /* Sector 11,  2 Kbyte */
#define ADDR_FLASH_SECTOR_12    ((uint32_t)0x08006000) /* Sector 12,  2 Kbyte */
#define ADDR_FLASH_SECTOR_13    ((uint32_t)0x08006800) /* Sector 13,  2 Kbyte */
#define ADDR_FLASH_SECTOR_14    ((uint32_t)0x08007000) /* Sector 14,  2 Kbyte */
#define ADDR_FLASH_SECTOR_15    ((uint32_t)0x08007800) /* Sector 15,  2 Kbyte */
#define ADDR_FLASH_SECTOR_16    ((uint32_t)0x08008000) /* Sector 16,  2 Kbyte */
#define ADDR_FLASH_SECTOR_17    ((uint32_t)0x08008800) /* Sector 17,  2 Kbyte */
#define ADDR_FLASH_SECTOR_18    ((uint32_t)0x08009000) /* Sector 18,  2 Kbyte */
#define ADDR_FLASH_SECTOR_19    ((uint32_t)0x08009800) /* Sector 19,  2 Kbyte */
#define ADDR_FLASH_SECTOR_20    ((uint32_t)0x0800A000) /* Sector 20,  2 Kbyte */
#define ADDR_FLASH_SECTOR_21    ((uint32_t)0x0800A800) /* Sector 21,  2 Kbyte */
#define ADDR_FLASH_SECTOR_22    ((uint32_t)0x0800B000) /* Sector 22,  2 Kbyte */
#define ADDR_FLASH_SECTOR_23    ((uint32_t)0x0800B800) /* Sector 23,  2 Kbyte */
#define ADDR_FLASH_SECTOR_24    ((uint32_t)0x0800C000) /* Sector 24,  2 Kbyte */
#define ADDR_FLASH_SECTOR_25    ((uint32_t)0x0800C800) /* Sector 25,  2 Kbyte */
#define ADDR_FLASH_SECTOR_26    ((uint32_t)0x0800D000) /* Sector 26,  2 Kbyte */
#define ADDR_FLASH_SECTOR_27    ((uint32_t)0x0800D800) /* Sector 27,  2 Kbyte */
#define ADDR_FLASH_SECTOR_28    ((uint32_t)0x0800E000) /* Sector 28,  2 Kbyte */
#define ADDR_FLASH_SECTOR_29    ((uint32_t)0x0800E800) /* Sector 29,  2 Kbyte */
#define ADDR_FLASH_SECTOR_30    ((uint32_t)0x0800F000) /* Sector 30,  2 Kbyte */
#define ADDR_FLASH_SECTOR_31    ((uint32_t)0x0800F800) /* Sector 31,  2 Kbyte */
#define ADDR_FLASH_SECTOR_32    ((uint32_t)0x08010000) /* Sector 32,  2 Kbyte */
#define ADDR_FLASH_SECTOR_33    ((uint32_t)0x08010800) /* Sector 33,  2 Kbyte */
#define ADDR_FLASH_SECTOR_34    ((uint32_t)0x08011000) /* Sector 34,  2 Kbyte */
#define ADDR_FLASH_SECTOR_35    ((uint32_t)0x08011800) /* Sector 35,  2 Kbyte */
#define ADDR_FLASH_SECTOR_36    ((uint32_t)0x08012000) /* Sector 36,  2 Kbyte */
#define ADDR_FLASH_SECTOR_37    ((uint32_t)0x08012800) /* Sector 37,  2 Kbyte */
#define ADDR_FLASH_SECTOR_38    ((uint32_t)0x08013000) /* Sector 38,  2 Kbyte */
#define ADDR_FLASH_SECTOR_39    ((uint32_t)0x08013800) /* Sector 39,  2 Kbyte */
#define ADDR_FLASH_SECTOR_40    ((uint32_t)0x08014000) /* Sector 40,  2 Kbyte */
#define ADDR_FLASH_SECTOR_41    ((uint32_t)0x08014800) /* Sector 41,  2 Kbyte */
#define ADDR_FLASH_SECTOR_42    ((uint32_t)0x08015000) /* Sector 42,  2 Kbyte */
#define ADDR_FLASH_SECTOR_43    ((uint32_t)0x08015800) /* Sector 43,  2 Kbyte */
#define ADDR_FLASH_SECTOR_44    ((uint32_t)0x08016000) /* Sector 44,  2 Kbyte */
#define ADDR_FLASH_SECTOR_45    ((uint32_t)0x08016800) /* Sector 45,  2 Kbyte */
#define ADDR_FLASH_SECTOR_46    ((uint32_t)0x08017000) /* Sector 46,  2 Kbyte */
#define ADDR_FLASH_SECTOR_47    ((uint32_t)0x08017800) /* Sector 47,  2 Kbyte */
#define ADDR_FLASH_SECTOR_48    ((uint32_t)0x08018000) /* Sector 48,  2 Kbyte */
#define ADDR_FLASH_SECTOR_49    ((uint32_t)0x08018800) /* Sector 49,  2 Kbyte */
#define ADDR_FLASH_SECTOR_50    ((uint32_t)0x08019000) /* Sector 50,  2 Kbyte */
#define ADDR_FLASH_SECTOR_51    ((uint32_t)0x08019800) /* Sector 51,  2 Kbyte */
#define ADDR_FLASH_SECTOR_52    ((uint32_t)0x0801A000) /* Sector 52,  2 Kbyte */
#define ADDR_FLASH_SECTOR_53    ((uint32_t)0x0801A800) /* Sector 53,  2 Kbyte */
#define ADDR_FLASH_SECTOR_54    ((uint32_t)0x0801B000) /* Sector 54,  2 Kbyte */
#define ADDR_FLASH_SECTOR_55    ((uint32_t)0x0801B800) /* Sector 55,  2 Kbyte */
#define ADDR_FLASH_SECTOR_56    ((uint32_t)0x0801C000) /* Sector 56,  2 Kbyte */
#define ADDR_FLASH_SECTOR_57    ((uint32_t)0x0801C800) /* Sector 57,  2 Kbyte */
#define ADDR_FLASH_SECTOR_58    ((uint32_t)0x0801D000) /* Sector 58,  2 Kbyte */
#define ADDR_FLASH_SECTOR_59    ((uint32_t)0x0801D800) /* Sector 59,  2 Kbyte */
#define ADDR_FLASH_SECTOR_60    ((uint32_t)0x0801E000) /* Sector 60,  2 Kbyte */
#define ADDR_FLASH_SECTOR_61    ((uint32_t)0x0801E800) /* Sector 61,  2 Kbyte */
#define ADDR_FLASH_SECTOR_62    ((uint32_t)0x0801F000) /* Sector 62,  2 Kbyte */
#define ADDR_FLASH_SECTOR_63    ((uint32_t)0x0801F800) /* Sector 63,  2 Kbyte */
#define ADDR_FLASH_SECTOR_64    ((uint32_t)0x08020000) /* Sector 64,  2 Kbyte */
#define ADDR_FLASH_SECTOR_65    ((uint32_t)0x08020800) /* Sector 65,  2 Kbyte */
#define ADDR_FLASH_SECTOR_66    ((uint32_t)0x08021000) /* Sector 66,  2 Kbyte */
#define ADDR_FLASH_SECTOR_67    ((uint32_t)0x08021800) /* Sector 67,  2 Kbyte */
#define ADDR_FLASH_SECTOR_68    ((uint32_t)0x08022000) /* Sector 68,  2 Kbyte */
#define ADDR_FLASH_SECTOR_69    ((uint32_t)0x08022800) /* Sector 69,  2 Kbyte */
#define ADDR_FLASH_SECTOR_70    ((uint32_t)0x08023000) /* Sector 70,  2 Kbyte */
#define ADDR_FLASH_SECTOR_71    ((uint32_t)0x08023800) /* Sector 71,  2 Kbyte */
#define ADDR_FLASH_SECTOR_72    ((uint32_t)0x08024000) /* Sector 72,  2 Kbyte */
#define ADDR_FLASH_SECTOR_73    ((uint32_t)0x08024800) /* Sector 73,  2 Kbyte */
#define ADDR_FLASH_SECTOR_74    ((uint32_t)0x08025000) /* Sector 74,  2 Kbyte */
#define ADDR_FLASH_SECTOR_75    ((uint32_t)0x08025800) /* Sector 75,  2 Kbyte */
#define ADDR_FLASH_SECTOR_76    ((uint32_t)0x08026000) /* Sector 76,  2 Kbyte */
#define ADDR_FLASH_SECTOR_77    ((uint32_t)0x08026800) /* Sector 77,  2 Kbyte */
#define ADDR_FLASH_SECTOR_78    ((uint32_t)0x08027000) /* Sector 78,  2 Kbyte */
#define ADDR_FLASH_SECTOR_79    ((uint32_t)0x08027800) /* Sector 79,  2 Kbyte */
#define ADDR_FLASH_SECTOR_80    ((uint32_t)0x08028000) /* Sector 80,  2 Kbyte */
#define ADDR_FLASH_SECTOR_81    ((uint32_t)0x08028800) /* Sector 81,  2 Kbyte */
#define ADDR_FLASH_SECTOR_82    ((uint32_t)0x08029000) /* Sector 82,  2 Kbyte */
#define ADDR_FLASH_SECTOR_83    ((uint32_t)0x08029800) /* Sector 83,  2 Kbyte */
#define ADDR_FLASH_SECTOR_84    ((uint32_t)0x0802A000) /* Sector 84,  2 Kbyte */
#define ADDR_FLASH_SECTOR_85    ((uint32_t)0x0802A800) /* Sector 85,  2 Kbyte */
#define ADDR_FLASH_SECTOR_86    ((uint32_t)0x0802B000) /* Sector 86,  2 Kbyte */
#define ADDR_FLASH_SECTOR_87    ((uint32_t)0x0802B800) /* Sector 87,  2 Kbyte */
#define ADDR_FLASH_SECTOR_88    ((uint32_t)0x0802C000) /* Sector 88,  2 Kbyte */
#define ADDR_FLASH_SECTOR_89    ((uint32_t)0x0802C800) /* Sector 89,  2 Kbyte */
#define ADDR_FLASH_SECTOR_90    ((uint32_t)0x0802D000) /* Sector 90,  2 Kbyte */
#define ADDR_FLASH_SECTOR_91    ((uint32_t)0x0802D800) /* Sector 91,  2 Kbyte */
#define ADDR_FLASH_SECTOR_92    ((uint32_t)0x0802E000) /* Sector 92,  2 Kbyte */
#define ADDR_FLASH_SECTOR_93    ((uint32_t)0x0802E800) /* Sector 93,  2 Kbyte */
#define ADDR_FLASH_SECTOR_94    ((uint32_t)0x0802F000) /* Sector 94,  2 Kbyte */
#define ADDR_FLASH_SECTOR_95    ((uint32_t)0x0802F800) /* Sector 95,  2 Kbyte */
#define ADDR_FLASH_SECTOR_96    ((uint32_t)0x08030000) /* Sector 96,  2 Kbyte */
#define ADDR_FLASH_SECTOR_97    ((uint32_t)0x08030800) /* Sector 97,  2 Kbyte */
#define ADDR_FLASH_SECTOR_98    ((uint32_t)0x08031000) /* Sector 98,  2 Kbyte */
#define ADDR_FLASH_SECTOR_99    ((uint32_t)0x08031800) /* Sector 99,  2 Kbyte */
#define ADDR_FLASH_SECTOR_100   ((uint32_t)0x08032000) /* Sector 100, 2 Kbyte */
#define ADDR_FLASH_SECTOR_101   ((uint32_t)0x08032800) /* Sector 101, 2 Kbyte */
#define ADDR_FLASH_SECTOR_102   ((uint32_t)0x08033000) /* Sector 102, 2 Kbyte */
#define ADDR_FLASH_SECTOR_103   ((uint32_t)0x08033800) /* Sector 103, 2 Kbyte */
#define ADDR_FLASH_SECTOR_104   ((uint32_t)0x08034000) /* Sector 104, 2 Kbyte */
#define ADDR_FLASH_SECTOR_105   ((uint32_t)0x08034800) /* Sector 105, 2 Kbyte */
#define ADDR_FLASH_SECTOR_106   ((uint32_t)0x08035000) /* Sector 106, 2 Kbyte */
#define ADDR_FLASH_SECTOR_107   ((uint32_t)0x08035800) /* Sector 107, 2 Kbyte */
#define ADDR_FLASH_SECTOR_108   ((uint32_t)0x08036000) /* Sector 108, 2 Kbyte */
#define ADDR_FLASH_SECTOR_109   ((uint32_t)0x08036800) /* Sector 109, 2 Kbyte */
#define ADDR_FLASH_SECTOR_110   ((uint32_t)0x08037000) /* Sector 110, 2 Kbyte */
#define ADDR_FLASH_SECTOR_111   ((uint32_t)0x08037800) /* Sector 111, 2 Kbyte */
#define ADDR_FLASH_SECTOR_112   ((uint32_t)0x08038000) /* Sector 112, 2 Kbyte */
#define ADDR_FLASH_SECTOR_113   ((uint32_t)0x08038800) /* Sector 113, 2 Kbyte */
#define ADDR_FLASH_SECTOR_114   ((uint32_t)0x08039000) /* Sector 114, 2 Kbyte */
#define ADDR_FLASH_SECTOR_115   ((uint32_t)0x08039800) /* Sector 115, 2 Kbyte */
#define ADDR_FLASH_SECTOR_116   ((uint32_t)0x0803A000) /* Sector 116, 2 Kbyte */
#define ADDR_FLASH_SECTOR_117   ((uint32_t)0x0803A800) /* Sector 117, 2 Kbyte */
#define ADDR_FLASH_SECTOR_118   ((uint32_t)0x0803B000) /* Sector 118, 2 Kbyte */
#define ADDR_FLASH_SECTOR_119   ((uint32_t)0x0803B800) /* Sector 119, 2 Kbyte */
#define ADDR_FLASH_SECTOR_120   ((uint32_t)0x0803C000) /* Sector 120, 2 Kbyte */
#define ADDR_FLASH_SECTOR_121   ((uint32_t)0x0803C800) /* Sector 121, 2 Kbyte */
#define ADDR_FLASH_SECTOR_122   ((uint32_t)0x0803D000) /* Sector 122, 2 Kbyte */
#define ADDR_FLASH_SECTOR_123   ((uint32_t)0x0803D800) /* Sector 123, 2 Kbyte */
#define ADDR_FLASH_SECTOR_124   ((uint32_t)0x0803E000) /* Sector 124, 2 Kbyte */
#define ADDR_FLASH_SECTOR_125   ((uint32_t)0x0803E800) /* Sector 125, 2 Kbyte */
#define ADDR_FLASH_SECTOR_126   ((uint32_t)0x0803F000) /* Sector 126, 2 Kbyte */
#define ADDR_FLASH_SECTOR_127   ((uint32_t)0x0803F800) /* Sector 127, 2 Kbyte */




/* ====================== Flash 地址与大小定义 (STM32F103ZET6) ====================== */

/* Flash 基地址 */
#define ADDR_FLASH_START_ADDR_BASE      ADDR_FLASH_SECTOR_0

/* APP 应用程序起始地址（从 Sector 0 开始，共 496KB） */
#define ADDR_FLASH_START_ADDR_APP       ADDR_FLASH_SECTOR_0

/* 用户数据存储地址（data 分区，从 496KB 开始，共 16KB） */
#define ADDR_FLASH_START_ADDR_DATA      ADDR_FLASH_SECTOR_248     /* 496KB / 2KB = 248 */

/* Flash 容量相关定义（以 2KB 扇区为单位） */
#define FLASH_SIZE_GRANULARITY_2K       (1 * PAGE_SIZE)           /* 2KB */
#define FLASH_SIZE_GRANULARITY_16K      (8  * PAGE_SIZE)          /* 16KB = 8 * 2KB */
#define FLASH_SIZE_GRANULARITY_496K     (248 * PAGE_SIZE)         /* 496KB = 248 * 2KB */
#define FLASH_SIZE_GRANULARITY_512K     (256 * PAGE_SIZE)         /* 总容量 512KB */

#endif /* APPLICATIONS_MACSYS_INC_MACFLASH_H_ */
