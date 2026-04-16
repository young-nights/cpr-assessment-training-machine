/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: MIT
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-10-15     Meco Man     The first version
 */

#ifndef LV_RT_THREAD_CONF_H
#define LV_RT_THREAD_CONF_H

#ifdef __RTTHREAD__

#define LV_RTTHREAD_INCLUDE <rtthread.h>
#include LV_RTTHREAD_INCLUDE


/*====================
   颜色设置
 *====================*/

/*颜色深度：1 (1字节/像素), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
#define LV_COLOR_DEPTH 16

/*RGB565 颜色高低字节交换。用于 8bit 接口（如 SPI）显示时需要*/
#define LV_COLOR_16_SWAP 1

/*启用透明背景绘制。
 *使用 opa、不透明度、transform_* 样式时必须开启。
 *也可用于 UI 叠加在视频/OSD 等图层之上*/
#define LV_COLOR_SCREEN_TRANSP 0

/*颜色混合（融合）取整方式调整。GPU 混合计算可能有差异。
 * 0: 向下取整；64: ≥0.75 向上；128: ≥0.5 向上；192: ≥0.25 向上；254: 几乎全向上*/
#define LV_COLOR_MIX_ROUND_OFS 0

/*图像色度键（Chroma Key）颜色，像素为该色时不绘制（常用于绿幕）*/
#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00)         /*纯绿色*/


/*=========================
   内存设置
 *=========================*/

#ifdef RT_USING_HEAP
#  define LV_MEM_CUSTOM 1
#  define LV_MEM_CUSTOM_INCLUDE LV_RTTHREAD_INCLUDE
#  define LV_MEM_CUSTOM_ALLOC   rt_malloc
#  define LV_MEM_CUSTOM_FREE    rt_free
#  define LV_MEM_CUSTOM_REALLOC rt_realloc
#endif

/*渲染过程使用的中间缓冲区最大数量。
 *不足时会打印错误日志*/
#define LV_MEM_BUF_MAX_NUM 16

/*使用标准 memcpy/memset 代替 LVGL 自带实现（有时更快）*/
#define LV_MEMCPY_MEMSET_STD 0


/*====================
   HAL 设置（硬件抽象层）
 *====================*/

/*默认显示刷新周期（ms），LVGL 将以此周期重绘变化区域*/
#define LV_DISP_DEF_REFR_PERIOD PKG_LVGL_DISP_REFR_PERIOD      /*30ms ≈ 33FPS*/

/*输入设备（如触摸）轮询周期（ms）*/
#define LV_INDEV_DEF_READ_PERIOD 30     /*30ms*/

/*使用自定义 tick 来源（毫秒），无需手动调用 lv_tick_inc()*/
#ifdef __PERF_COUNTER__
    #define LV_TICK_CUSTOM 1
    #if LV_TICK_CUSTOM
        extern uint32_t SystemCoreClock;
        /*自定义 tick 头文件*/
        #define LV_TICK_CUSTOM_INCLUDE          "perf_counter.h"
        /*获取当前毫秒数的表达式*/
        #define LV_TICK_CUSTOM_SYS_TIME_EXPR    get_system_ms()
    #endif   /*LV_TICK_CUSTOM*/
#else
    #define LV_TICK_CUSTOM 1
    #if LV_TICK_CUSTOM
        #define LV_TICK_CUSTOM_INCLUDE LV_RTTHREAD_INCLUDE         /*系统时间头文件*/
        #define LV_TICK_CUSTOM_SYS_TIME_EXPR (rt_tick_get_millisecond())     /*毫秒表达式*/
    #endif   /*LV_TICK_CUSTOM*/
#endif       /*__PERF_COUNTER__*/

/*默认 DPI（每英寸像素），影响默认控件大小、间距等*/
#define LV_DPI_DEF 130     /*130 px/inch*/


/*=======================
* 功能配置
*=======================*/

/*-------------
* 绘制引擎
*-----------*/

/*启用复杂绘制引擎。
*需要绘制阴影、渐变、圆角、圆形、弧线、倾斜线、图像变换或遮罩时必须开启*/
#define LV_DRAW_COMPLEX 1
#if LV_DRAW_COMPLEX != 0
/*阴影缓存大小。
 *LV_SHADOW_CACHE_SIZE 为最大缓存阴影尺寸（shadow_width + radius）
 *缓存消耗 LV_SHADOW_CACHE_SIZE^2 RAM*/
#define LV_SHADOW_CACHE_SIZE 0

/*圆形抗锯齿缓存数量。
 *缓存 1/4 圆周数据，radius * 4 字节/圆*/
#define LV_CIRCLE_CACHE_SIZE 4
#endif /*LV_DRAW_COMPLEX*/

/*简单图层（Simple Layer）用于 style_opa < 255 的控件。
*先缓冲为图像再混合显示。
*可分块缓冲以减少单次分配大小*/
#define LV_LAYER_SIMPLE_BUF_SIZE (24 * 1024)      /*目标缓冲 24KB*/
#define LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE (3 * 1024)  /*失败时回退 3KB*/

/*默认图像缓存大小。
*缓存已打开的图像，适合 PNG/JPG 等复杂解码器*/
#define LV_IMG_CACHE_DEF_SIZE 0

/*渐变最大停留点数（stops），每增加一个停留点多耗 (sizeof(lv_color_t) + 1) 字节*/
#define LV_GRADIENT_MAX_STOPS 2

/*渐变缓存默认大小（字节），0 表示不缓存*/
#define LV_GRAD_CACHE_DEF_SIZE 0

/*启用渐变抖动（在低色深屏幕上实现视觉平滑过渡）*/
#define LV_DITHER_GRADIENT 0
#if LV_DITHER_GRADIENT
/*启用误差扩散抖动（视觉更好，但更耗 CPU 和 RAM）*/
#define LV_DITHER_ERROR_DIFFUSION 0
#endif

/*软件旋转最大缓冲区大小（仅软件旋转启用时使用）*/
#define LV_DISP_ROT_MAX_BUF (10*1024)

/*-------------
 * GPU 加速
 *-----------*/

/*使用 STM32 DMA2D (Chrom-Art) GPU*/
#if LV_USE_GPU_STM32_DMA2D
    #define LV_GPU_DMA2D_CMSIS_INCLUDE
#endif

/*使用 SWM341 DMA2D GPU*/
#if LV_USE_GPU_SWM341_DMA2D
    #define LV_GPU_SWM341_DMA2D_INCLUDE "SWM341.h"
#endif

/*使用 NXP PXP GPU (iMX RTxxx)*/
#if LV_USE_GPU_NXP_PXP
    /*1: 自动初始化 PXP 中断；0: 需手动调用 lv_gpu_nxp_pxp_init()*/
    #define LV_USE_GPU_NXP_PXP_AUTO_INIT 0
#endif

#if LV_USE_GPU_RA6M3_G2D
    #define LV_GPU_RA6M3_G2D_INCLUDE "hal_data.h"
#endif

/*使用 SDL 渲染器（PC 模拟器）*/
#define LV_USE_GPU_SDL 0
#if LV_USE_GPU_SDL
    #define LV_GPU_SDL_INCLUDE_PATH <SDL2/SDL.h>
    #define LV_GPU_SDL_LRU_SIZE (1024 * 1024 * 8)  /*纹理缓存 8MB*/
    #define LV_GPU_SDL_CUSTOM_BLEND_MODE (SDL_VERSION_ATLEAST(2, 0, 6))
#endif

/*-------------
 * 日志系统
 *-----------*/

/*启用日志模块*/
#define LV_USE_LOG 0
#if LV_USE_LOG
    /*日志等级*/
    #define LV_LOG_LEVEL LV_LOG_LEVEL_USER

    /*1: 使用 printf 输出；0: 需注册回调 lv_log_register_print_cb()*/
    #define LV_LOG_PRINTF 1

    /*各模块 TRACE 日志开关（产生大量日志）*/
    #define LV_LOG_TRACE_MEM        1
    #define LV_LOG_TRACE_TIMER      1
    #define LV_LOG_TRACE_INDEV      1
    #define LV_LOG_TRACE_DISP_REFR  1
    #define LV_LOG_TRACE_EVENT      1
    #define LV_LOG_TRACE_OBJ_CREATE 1
    #define LV_LOG_TRACE_LAYOUT     1
    #define LV_LOG_TRACE_ANIM       1
#endif  /*LV_USE_LOG*/

/*-------------
 * 断言
 *-----------*/

/*启用各种断言检查*/
#define LV_USE_ASSERT_NULL          1   /*参数为空检查（推荐）*/
#define LV_USE_ASSERT_MALLOC        1   /*内存分配失败检查（推荐）*/
#define LV_USE_ASSERT_STYLE         0   /*样式初始化检查*/
#define LV_USE_ASSERT_MEM_INTEGRITY 0   /*内存完整性检查（慢）*/
#define LV_USE_ASSERT_OBJ           0   /*对象类型/存在检查（慢）*/

#define LV_ASSERT_HANDLER_INCLUDE LV_RTTHREAD_INCLUDE
#define LV_ASSERT_HANDLER RT_ASSERT(0);

/*-------------
 * 其他功能
 *-----------*/

/*显示 CPU 使用率和 FPS*/
#define LV_USE_PERF_MONITOR 0
#if LV_USE_PERF_MONITOR
    #define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT
#endif

/*显示内存使用和碎片情况（需 LV_MEM_CUSTOM=0）*/
#define LV_USE_MEM_MONITOR 0
#if LV_USE_MEM_MONITOR
    #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
#endif

/*在重绘区域绘制随机色块（调试用）*/
#define LV_USE_REFR_DEBUG 0

/*自定义 snprintf 函数*/
#define LV_SPRINTF_CUSTOM 1
#if LV_SPRINTF_CUSTOM
    #define LV_SPRINTF_INCLUDE LV_RTTHREAD_INCLUDE
    #define lv_snprintf  rt_snprintf
    #define lv_vsnprintf rt_vsnprintf
    #define LV_SPRINTF_USE_FLOAT 0
#endif



/*用户自定义数据（可在 lv_obj_t 中存储 void* user_data）*/
#define LV_USE_USER_DATA 1

/*垃圾回收（用于高级语言绑定如 Micropython）*/
#define LV_ENABLE_GC 0

/*=====================
 * 编译器设置
 *====================*/

/*大数据（大端）系统设为 1*/
#ifdef ARCH_CPU_BIG_ENDIAN
#  define LV_BIG_ENDIAN_SYSTEM 1
#else
#  define LV_BIG_ENDIAN_SYSTEM 0
#endif


/*tick、timer、flush 等函数属性定义*/
#define LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TIMER_HANDLER
#define LV_ATTRIBUTE_FLUSH_READY

/*缓冲区对齐要求*/
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE 4
#ifdef rt_align /* >= RT-Thread v5.0.0 */
#  define LV_ATTRIBUTE_MEM_ALIGN rt_align(RT_ALIGN_SIZE)
#else
#  define LV_ATTRIBUTE_MEM_ALIGN ALIGN(RT_ALIGN_SIZE)
#endif



/*大常量数组（如字体位图）属性*/
#define LV_ATTRIBUTE_LARGE_CONST
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY

/*高性能函数放快速 RAM*/
#define LV_ATTRIBUTE_FAST_MEM

/*GPU/DMA 可访问变量*/
#define LV_ATTRIBUTE_DMA

/*导出常量到绑定层（如 Micropython）*/
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning

/*使用 int32_t 坐标（范围 -4M..4M），否则 int16_t (-32k..32k)*/
#define LV_USE_LARGE_COORD 0

/*==================
 * 字体使用
 *===================*/

/*Montserrat 字体（ASCII + 部分符号，bpp=4）*/
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 0
/*... 后续大字体全部 0 ...*/
#define LV_FONT_MONTSERRAT_48 0

/*子像素字体、抗锯齿压缩字体等演示功能*/
#define LV_FONT_MONTSERRAT_12_SUBPX 0
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0

/*波斯/希伯来、CJK、Unifont 等特殊字体*/
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0
#define LV_FONT_SIMSUN_16_CJK            0
#define LV_FONT_UNSCII_8  0
#define LV_FONT_UNSCII_16 0

/*自定义字体声明（可在此声明自己的字体）*/
#define LV_FONT_CUSTOM_DECLARE

/*默认字体*/
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/*支持大字体或大量字符的字体格式*/
#define LV_FONT_FMT_TXT_LARGE 0

/*启用字体压缩*/
#define LV_USE_FONT_COMPRESSED 0

/*子像素渲染（LCD 屏幕更锐利）*/
#define LV_USE_FONT_SUBPX 0
#if LV_USE_FONT_SUBPX
    #define LV_FONT_SUBPX_BGR 0  /*0: RGB；1: BGR 顺序*/
#endif

/* glyph 未找到时绘制占位符*/
#define LV_USE_FONT_PLACEHOLDER 1

/*=================
 * 文本设置
 *=================*/

/*字符串编码：UTF-8 或 ASCII*/
#define LV_TXT_ENC LV_TXT_ENC_UTF8

/*自动换行字符*/
#define LV_TXT_BREAK_CHARS " ,.;:-_"

/*长单词换行策略*/
#define LV_TXT_LINE_BREAK_LONG_LEN 0
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

/*文本 recolor 控制字符*/
#define LV_TXT_COLOR_CMD "#"

/*双向文本支持（LTR + RTL 混合）*/
#define LV_USE_BIDI 0

/*阿拉伯/波斯字符变形处理*/
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/*==================
 * 控件使用
 *==================*/

/*基础控件*/
#define LV_USE_ARC        1   // 圆弧控件：可拖动的圆形进度条、音量旋钮、仪表盘刻度
#define LV_USE_BAR        1   // 进度条：水平/垂直，支持百分比、动画、渐变
#define LV_USE_BTN        1   // 基础按钮：点击、长按、状态切换，所有可点击对象的祖先
#define LV_USE_BTNMATRIX  1   // 按钮矩阵：一次性生成 N×M 键盘/软键阵列，比如计算器键盘
#define LV_USE_CANVAS     1   // 画布：可动态绘制直线、矩形、文本、图片，做“截图”或“图层”
#define LV_USE_CHECKBOX   1   // 复选框：打勾/取消，可配文字，支持三态（未选/半选/全选）
#define LV_USE_DROPDOWN   1   // 下拉列表：点击后弹出选项列表，可滚动、可多选
#define LV_USE_IMG        1   // 基础图片：显示 PNG/JPEG/SVG/字体图标，支持缩放、旋转
#define LV_USE_LABEL      1   // 基础文本：单行/多行、自动换行、样式丰富
#if LV_USE_LABEL
    #define LV_LABEL_TEXT_SELECTION 1   // 在标签里长按可弹出“全选/复制”光标，类似手机文本选中
    #define LV_LABEL_LONG_TXT_HINT  1   // 长文本优化：只渲染可视区域，节省内存，适合几万行日志
#endif
#define LV_USE_LINE       1   // 直线：任意两点连线，可设颜色、宽度、虚线，画分割线/网格
#define LV_USE_ROLLER     1   // 滚轮选择器：像 iOS 时间选择器，上下滑动选数字/字符串
#define LV_USE_SLIDER     1   // 滑块：拖动选值，带进度条背景，常用于音量、亮度
#define LV_USE_SWITCH     1   // 开关：左右滑动切换 ON/OFF，自带动画，像手机设置里的开关
#define LV_USE_TEXTAREA   1   // 多行文本框：可编辑、光标、密码掩码、滚动条、与虚拟键盘联动
#define LV_USE_TABLE      1   // 表格：行列单元格，可合并、可滚动，做数据报表或设置页

/*==================
 * 扩展组件
 *==================*/

/*高级控件*/
#define LV_USE_ANIMIMG    1   // 动画图片控件：把一张多帧的 PNG/SVG 当成动画播放
#define LV_USE_CALENDAR   0   // 日历控件：带月视图、日选择、高亮日期
#define LV_USE_CHART      1   // 图表控件：折线、柱状、点状图，可实时刷新数据
#define LV_USE_COLORWHEEL 1   // 色轮控件：用户拖动选颜色，带 HSV 圆盘
#define LV_USE_IMGBTN     1   // 图片按钮：normal/pressed/disabled 三态用三张图表示
#define LV_USE_KEYBOARD   0   // 虚拟键盘：与文本框联动，可切换布局，支持数字/符号
#define LV_USE_LED        1   // LED 灯效果：纯色、透明、闪烁，用来显示状态
#define LV_USE_LIST       1   // 列表控件：垂直可滚动，带图标/箭头，常用于菜单
#define LV_USE_MENU       1   // 高级菜单：支持层级、复选框、Radio、分隔线
#define LV_USE_METER      1   // 仪表盘：圆弧刻度、指针、多通道，模仿汽车仪表
#define LV_USE_MSGBOX     1   // 消息框：弹出提示/确认/询问，自动居中
#define LV_USE_SPAN       1   // 富文本片段：在同一行内混用不同颜色、字体、大小
#define LV_USE_SPINBOX    1   // 数字编辑框：带增减按钮，可设范围、步长、小数位
#define LV_USE_SPINNER    1   // 等待动画：转圈菊花，表示“加载中”
#define LV_USE_TABVIEW    1   // 页签视图：顶部或底部页签，左右滑动切换页面
#define LV_USE_TILEVIEW   1   // 瓷砖视图：上下左右四向滑动，类似手机 Launcher
#define LV_USE_WIN        1   // 窗口控件：带标题栏、关闭按钮、可拖动调整大小

/*-----------
 * 主题
 *----------*/
#ifdef RTE_GRAPHICS_LVGL_USE_EXTRA_THEMES
    #define LV_USE_THEME_DEFAULT 1   /*完整主题*/
    #define LV_USE_THEME_BASIC   1   /*简洁主题*/
    #define LV_USE_THEME_MONO    1   /*单色主题*/
#else
    #define LV_USE_THEME_DEFAULT 0
    #define LV_USE_THEME_BASIC   0
    #define LV_USE_THEME_MONO    0
#endif

/*-----------
 * 布局
 *----------*/
#define LV_USE_FLEX 1   /*类似 CSS Flexbox*/
#define LV_USE_GRID 1   /*类似 CSS Grid*/

/*-----------
 * 其他
 *----------*/
#define LV_USE_SNAPSHOT 0      /*对象截图*/
#define LV_USE_MONKEY   0      /*猴子测试*/
#define LV_USE_GRIDNAV  0
#define LV_USE_FRAGMENT 0
#define LV_USE_IMGFONT  0
#define LV_USE_MSG      0
#define LV_USE_IME_PINYIN 0    /*拼音输入法*/

/*==================
* 示例
*==================*/
#define LV_BUILD_EXAMPLES 0    /*编译示例代码（发布时建议关闭）*/



#endif /*__RTTHREAD__*/

#endif /*LV_CONF_H*/
