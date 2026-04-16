2025-08-22 10:57:59 星期五
# 外部SRAM（SPI）的使用
-  场景1：把LVGL管理的内存空间放到外部SRAM
	**核心目标**
	**让 LVGL 的动态内存（lv_mem_alloc）全部落到外挂 23LC1024 SRAM 中，而不是 STM32 内部RAM**

------------

	步骤一：修改 lv_rt_thread_conf.h，让 LVGL 使用自定义的内存管理函数，即使用23lc1024的sram_malloc / sram_free 
	步骤二：实现 SRAM 的 malloc / free，由于23LC1024是SPI RAM，没有物理地址映射，所以需要手动分配一个简易内存分配器，运行在 SPI SRAM 上。

------------

**流程**
1. 在外部sram中定义一个静态内存池用于给LVGL使用，这里使用的sram使用的是128K大小，因此定义一个128K大小的内存池
```C
#define SRAM_MEM_SIZE (128 * 1024)  // 128KB
```





