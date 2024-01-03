#include "lvgl.h"

#define MY_DISP_HOR_RES 1024
#define MY_DISP_VER_RES 600

void disp_flush(lv_disp_drv_t* disp_drv,
                const lv_area_t* area,
                lv_color_t* color_p) {
    // if(disp_flush_enabled) {
    /*The most simple case (but also the slowest) to put all pixels to the
     * screen one-by-one*/

    int32_t x;
    int32_t y;
    for (y = area->y1; y <= area->y2; y++) {
        for (x = area->x1; x <= area->x2; x++) {
            /*Put a pixel to the display. For example:*/
            /*put_px(x, y, *color_p)*/
            color_p++;
        }
    }
    // }

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

void lv_port_disp_init(void) {
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    // disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy
     * its content to your display. The buffer has to be greater than 1 display
     * row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your
     * display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your
     * display. You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other
     * buffer while the data is being sent form the first buffer. It makes
     * rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in
     * `flush_cb` and you only need to change the frame buffer's address.
     */

    /* Example for 1) */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    static lv_color_t buf_1[MY_DISP_HOR_RES * 10]; /*A buffer for 10 rows*/
    lv_disp_draw_buf_init(
        &draw_buf_dsc_1, buf_1, NULL,
        MY_DISP_HOR_RES * 10); /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv; /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);   /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_1;

    /*Required for Example 3)*/
    // disp_drv.full_refresh = 1;

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in
     * LVGL. But if you have a different GPU you can use with this callback.*/
    // disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

void user_lvgl_init(void) {
    lv_init();
    lv_port_disp_init();
    // lv_port_indev_init();
}
