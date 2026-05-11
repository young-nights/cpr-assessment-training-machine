/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include "bsp_sys.h"
/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
    #define MY_DISP_HOR_RES    240
#endif

#ifndef MY_DISP_VER_RES
    #define MY_DISP_VER_RES    320
#endif

// 使用内部RAM作为图形缓冲区
#define USE_INTERNAL_RAM        1

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);




/**
 * LVGL requires a buffer where it internally draws the widgets.
 * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
 * The buffer has to be greater than 1 display row
 *
 * There are 3 buffering configurations:
 * 1. Create ONE buffer:
 *      LVGL will draw the display's content here and writes it to your display
 *
 * 2. Create TWO buffer:
 *      LVGL will draw the display's content to a buffer and writes it your display.
 *      You should use DMA to write the buffer's content to the display.
 *      It will enable LVGL to draw the next part of the screen to the other buffer while
 *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
 *
 * 3. Double buffering
 *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
 *      This way LVGL will always provide the whole rendered screen in `flush_cb`
 *      and you only need to change the frame buffer's address.
 */

void lv_port_disp_init(void)
{


    /* LVGL使用单缓冲模式 */
    #define LV_USE_ONE_BUFFER 0
    /* LVGL使用双缓冲模式 */
    #define LV_USE_TWO_BUFFER 1
    #define LV_USE_DOUBLE_BUFFER 0

    /* 初始化LCD显示屏驱动 */
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/
    #if LV_USE_ONE_BUFFER
    /***
     * @note 单缓冲模式
     *! 在这种配置中，LVGL 使用一个缓冲区来绘制显示内容。绘制完成后，将缓冲区的内容通过 flush_cb 回调函数写入显示设备。
     *! 这种方式简单直接，但在缓冲区写入显示设备期间，LVGL 无法进行新的绘制操作，可能会导致一些延迟。
     */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    static lv_color_t buf_1[MY_DISP_HOR_RES * 10];                          /*A buffer for 10 rows*/
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/
    #endif


    #if LV_USE_TWO_BUFFER
    /***
     * @note 双缓冲模式
     *! 在这种配置中，LVGL 使用两个缓冲区来实现双缓冲。
     *! LVGL 在一个缓冲区绘制显示内容，同时可以通过 DMA（直接内存访问）将另一个缓冲区的内容写入显示设备。
     *! 这样，渲染和刷新可以并行进行，提高效率。
     */
    static lv_disp_draw_buf_t draw_buf_dsc_2;
#if USE_INTERNAL_RAM
    // 使用内部 SRAM 作为 draw buffer
    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];                        /*A buffer for 10 rows*/
    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 10];                        /*An other buffer for 10 rows*/
#endif
    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/
    #endif /*LV_USE_TWO_BUFFER*/


    #if LV_USE_DOUBLE_BUFFER
    /***
     * @note 双缓冲模式（全屏）
     *! 在这种配置中，设置了两个与屏幕大小相同的缓冲区，并且设置 disp_drv.full_refresh = 1。
     *! LVGL 会将整个渲染后的屏幕内容提供给 flush_cb 回调函数，只需要切换帧缓冲区的地址即可。这种方式适用于需要频繁更新整个屏幕的应用。
     */
    static lv_disp_draw_buf_t draw_buf_dsc_3;
    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*A screen sized buffer*/
    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*Another screen sized buffer*/
    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,
                          MY_DISP_VER_RES * LV_VER_RES_MAX);   /*Initialize the display buffer*/
    #endif
    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                  /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_2;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}



/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
    lcd_init();
}



/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{

#if USE_INTERNAL_RAM
    /* color_p is a buffer pointer; the buffer is provided by LVGL */
    lcd_fill_array(area->x1, area->y1, area->x2, area->y2, color_p);

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
#endif

}



