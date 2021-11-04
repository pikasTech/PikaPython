#ifndef __ARM_2D__COMMON_H
#define __ARM_2D__COMMON_H

#include <stdbool.h>
#include "Arm2D_Box.h"
#include "BaseObj.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "dataStrs.h"
#include "lcd_printf.h"
#include "pikaScript.h"

#ifndef ARM2DQEMUBOOTER_APP_ARM2D_H_
#define APPLICATIONS_APP_ARM2D_H_

typedef struct __pika_arm2d_element_info_t {
    int x, x_last;
    int y, y_last;
    int alpha, alpha_last;
} pika_arm2d_element_info_t;

typedef struct __pika_arm2d_box_info_t {
	pika_arm2d_element_info_t elem_info;
	arm_2d_region_t arg2d_regin;
    int wight, wight_last;
    int hight, hight_last;
    uint16_t color_code, color_code_last;
} pika_arm2d_box_info_t;

typedef struct __pika_arm2d_window_t {
    arm_2d_tile_t* pfb_tile_now;
    bool pfb_is_new_frame;
    arm_2d_region_list_item_t* dirty_region_list;
    PikaObj* pika_windows_object;
    PikaObj* pika_background_object;
    PikaObj* pika_elems_object;
} pika_arm2d_window_t;

extern pika_arm2d_window_t pika_arm2d_window;

/* GLCD RGB color definitions */
#define GLCD_COLOR_BLACK 0x0000      /*   0,   0,   0 */
#define GLCD_COLOR_NAVY 0x000F       /*   0,   0, 128 */
#define GLCD_COLOR_DARK_GREEN 0x03E0 /*   0, 128,   0 */
#define GLCD_COLOR_DARK_CYAN 0x03EF  /*   0, 128, 128 */
#define GLCD_COLOR_MAROON 0x7800     /* 128,   0,   0 */
#define GLCD_COLOR_PURPLE 0x780F     /* 128,   0, 128 */
#define GLCD_COLOR_OLIVE 0x7BE0      /* 128, 128,   0 */
#define GLCD_COLOR_LIGHT_GREY 0xC618 /* 192, 192, 192 */
#define GLCD_COLOR_DARK_GREY 0x7BEF  /* 128, 128, 128 */
#define GLCD_COLOR_BLUE 0x001F       /*   0,   0, 255 */
#define GLCD_COLOR_GREEN 0x07E0      /*   0, 255,   0 */
#define GLCD_COLOR_CYAN 0x07FF       /*   0, 255, 255 */
#define GLCD_COLOR_RED 0xF800        /* 255,   0,   0 */
#define GLCD_COLOR_MAGENTA 0xF81F    /* 255,   0, 255 */
#define GLCD_COLOR_YELLOW 0xFFE0     /* 255, 255, 0   */
#define GLCD_COLOR_WHITE 0xFFFF      /* 255, 255, 255 */

void pika_arm2d_init(void);
void pika_arm2d_update();

#endif /* ARM2DQEMUBOOTER_APP_ARM2D_H_ */

uint16_t getColorCode(char* colorName);
typedef struct {
    arm_2d_op_rotate_t tOP;
    const arm_2d_tile_t* ptTile;
    float fAngle;
    float fAngleSpeed;
    arm_2d_location_t tCentre;
    arm_2d_region_t tRegion;
} rotate_tile_t;

#endif
