#ifndef __ARM_2D__COMMON_H
#define __ARM_2D__COMMON_H

#include "arm_2d_helper.h"
#include <stdbool.h>
#include "Arm2D_Box.h"
#include "BaseObj.h"
#include "dataStrs.h"
#include "pikaScript.h"
#include "arm_2d_cfg.h"

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

typedef struct __pika_arm2d_globals_t {
    arm_2d_tile_t* pfb_tile_now;
    bool pfb_is_new_frame;
    arm_2d_region_list_item_t* dirty_region_list;
    PikaObj* pika_windows_object;
    PikaObj* pika_background_object;
    PikaObj* pika_elems_object;
} pika_arm2d_globals_t;

extern pika_arm2d_globals_t pika_arm2d_globals;

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

#define __implement_tile(__NAME, __WIDTH, __HEIGHT, __TYPE, ...)                \
            static __TYPE                                            \
                __NAME##Buffer[(__WIDTH) * (__HEIGHT)];                         \
            const arm_2d_tile_t __NAME = {                                      \
                .tRegion = {                                                    \
                    .tSize = {(__WIDTH), (__HEIGHT)},                           \
                },                                                              \
                .tInfo.bIsRoot = true,                                          \
                .pchBuffer = (uint8_t *)__NAME##Buffer,                         \
                __VA_ARGS__                                                     \
            };

#define implement_tile(__NAME, __WIDTH, __HEIGHT, __TYPE, ...)                  \
            __implement_tile(__NAME, __WIDTH, __HEIGHT, __TYPE, ##__VA_ARGS__)

#endif

#ifndef __GLCD_CFG_COLOUR_DEPTH__ 
#define __GLCD_CFG_COLOUR_DEPTH__    16
#endif

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define __arm_2d_color_t         arm_2d_color_gray8_t
#   define COLOUR_INT               uint8_t
#   define ARM_2D_COLOUR            ARM_2D_COLOUR_GRAY8

#   define arm_2d_draw_pattern      arm_2d_c8bit_draw_pattern
#   define arm_2dp_fill_colour_with_opacity                                     \
                arm_2dp_gray8_fill_colour_with_opacity
#   define arm_2d_fill_colour_with_mask                                         \
                arm_2d_gray8_fill_colour_with_mask
#   define arm_2d_fill_colour_with_mask_and_opacity                             \
                arm_2d_gray8_fill_colour_with_mask_and_opacity
#   define arm_2d_alpha_blending_with_colour_keying                             \
                arm_2d_gray8_alpha_blending_with_colour_keying
#   define arm_2d_tile_copy_with_masks                                          \
                arm_2d_gray8_tile_copy_with_masks
#   define arm_2d_tile_fill_with_x_mirror                                       \
                arm_2d_c8bit_tile_fill_with_x_mirror
#   define arm_2d_tile_fill_with_y_mirror                                       \
                arm_2d_c8bit_tile_fill_with_y_mirror
#   define arm_2d_tile_fill_with_xy_mirror                                      \
                arm_2d_c8bit_tile_fill_with_xy_mirror
#   define arm_2d_tile_fill_only                                                \
                arm_2d_c8bit_tile_fill_only
#   define arm_2d_tile_copy_with_x_mirror                                       \
                arm_2d_c8bit_tile_copy_with_x_mirror
#   define arm_2d_tile_copy_with_y_mirror                                       \
                arm_2d_c8bit_tile_copy_with_y_mirror
#   define arm_2d_tile_copy_with_xy_mirror                                      \
                arm_2d_c8bit_tile_copy_with_xy_mirror
#   define arm_2d_tile_copy_only                                                \
                arm_2d_c8bit_tile_copy_only
#   define arm_2d_tile_copy                                                     \
                arm_2d_c8bit_tile_copy
#   define arm_2d_fill_colour                                                   \
                arm_2d_c8bit_fill_colour
#   define arm_2d_tile_copy_with_colour_keying                                  \
                arm_2d_c8bit_tile_copy_with_colour_keying
#   define arm_2d_fill_colour_with_opacity                                      \
                arm_2d_gray8_fill_colour_with_opacity
#   define arm_2d_alpha_blending                                                \
                arm_2d_gray8_alpha_blending
#   define arm_2d_tile_copy_with_des_mask                                       \
                arm_2d_gray8_tile_copy_with_des_mask
#   define arm_2d_tile_copy_with_src_mask                                       \
                arm_2d_gray8_tile_copy_with_src_mask
#   define arm_2dp_tile_transform_with_opacity                                  \
                arm_2dp_gray8_tile_transform_with_opacity
#   define arm_2dp_tile_transform_with_src_mask_and_opacity                     \
                arm_2dp_gray8_tile_transform_with_src_mask_and_opacity
#   define arm_2dp_tile_rotation                arm_2dp_gray8_tile_rotation
#   define arm_2dp_tile_rotation_with_alpha                                     \
                arm_2dp_gray8_tile_rotation_with_alpha
#   define arm_2dp_tile_rotation_with_src_mask_and_opacity                      \
                arm_2dp_gray8_tile_rotation_with_src_mask_and_opacity
#   define arm_2dp_tile_rotation_with_src_mask                                  \
                arm_2dp_gray8_tile_rotation_with_src_mask

#   define arm_2d_tile_rotation                                  \
                arm_2d_gray8_tile_rotation

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define __arm_2d_color_t         arm_2d_color_rgb565_t
#   define COLOUR_INT               uint16_t
#   define ARM_2D_COLOUR            ARM_2D_COLOUR_RGB565

#   define arm_2d_draw_pattern      arm_2d_rgb16_draw_pattern 
#   define arm_2dp_fill_colour_with_opacity                                     \
                arm_2dp_rgb565_fill_colour_with_opacity
#   define arm_2d_fill_colour_with_mask                                         \
                arm_2d_rgb565_fill_colour_with_mask
#   define arm_2d_fill_colour_with_mask_and_opacity                             \
                arm_2d_rgb565_fill_colour_with_mask_and_opacity
#   define arm_2d_alpha_blending_with_colour_keying                             \
                arm_2d_rgb565_alpha_blending_with_colour_keying
#   define arm_2d_tile_copy_with_masks                                          \
                arm_2d_rgb565_tile_copy_with_masks
#   define arm_2d_tile_fill_with_x_mirror                                       \
                arm_2d_rgb16_tile_fill_with_x_mirror
#   define arm_2d_tile_fill_with_y_mirror                                       \
                arm_2d_rgb16_tile_fill_with_y_mirror
#   define arm_2d_tile_fill_with_xy_mirror                                      \
                arm_2d_rgb16_tile_fill_with_xy_mirror
#   define arm_2d_tile_fill_only                                                \
                arm_2d_rgb16_tile_fill_only
#   define arm_2d_tile_copy_with_x_mirror                                       \
                arm_2d_rgb16_tile_copy_with_x_mirror
#   define arm_2d_tile_copy_with_y_mirror                                       \
                arm_2d_rgb16_tile_copy_with_y_mirror
#   define arm_2d_tile_copy_with_xy_mirror                                      \
                arm_2d_rgb16_tile_copy_with_xy_mirror
#   define arm_2d_tile_copy_only                                                \
                arm_2d_rgb16_tile_copy_only
#   define arm_2d_tile_copy                                                     \
                arm_2d_rgb16_tile_copy
#   define arm_2d_fill_colour                                                   \
                arm_2d_rgb16_fill_colour
#   define arm_2d_tile_copy_with_colour_keying                                  \
                arm_2d_rgb16_tile_copy_with_colour_keying
#   define arm_2d_fill_colour_with_opacity                                      \
                arm_2d_rgb565_fill_colour_with_opacity
#   define arm_2d_alpha_blending                                                \
                arm_2d_rgb565_alpha_blending
#   define arm_2d_tile_copy_with_des_mask                                       \
                arm_2d_rgb565_tile_copy_with_des_mask
#   define arm_2d_tile_copy_with_src_mask                                       \
                arm_2d_rgb565_tile_copy_with_src_mask
#   define arm_2dp_tile_transform_with_opacity                                  \
                arm_2dp_rgb565_tile_transform_with_opacity
#   define arm_2dp_tile_transform_with_src_mask_and_opacity                     \
                arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity
#   define arm_2dp_tile_rotation                arm_2dp_rgb565_tile_rotation
#   define arm_2dp_tile_rotation_with_alpha                                     \
                arm_2dp_rgb565_tile_rotation_with_alpha
#   define arm_2dp_tile_rotation_with_src_mask_and_opacity                      \
                arm_2dp_rgb565_tile_rotation_with_src_mask_and_opacity
#   define arm_2dp_tile_rotation_with_src_mask                                  \
                arm_2dp_rgb565_tile_rotation_with_src_mask


#   define arm_2d_tile_rotation                                  \
                arm_2d_rgb565_tile_rotation
#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define __arm_2d_color_t         arm_2d_color_cccn888_t
#   define COLOUR_INT               uint32_t
#   define ARM_2D_COLOUR            ARM_2D_COLOUR_CCCA8888

#   define arm_2d_draw_pattern      arm_2d_rgb32_draw_pattern 
#   define arm_2dp_fill_colour_with_opacity                                     \
                arm_2dp_cccn888_fill_colour_with_opacity
#   define arm_2d_fill_colour_with_mask                                         \
                arm_2d_cccn888_fill_colour_with_mask
#   define arm_2d_fill_colour_with_mask_and_opacity                             \
                arm_2d_cccn888_fill_colour_with_mask_and_opacity
#   define arm_2d_alpha_blending_with_colour_keying                             \
                arm_2d_cccn888_alpha_blending_with_colour_keying
#   define arm_2d_tile_copy_with_masks                                          \
                arm_2d_cccn888_tile_copy_with_masks
#   define arm_2d_tile_fill_with_x_mirror                                       \
                arm_2d_rgb32_tile_fill_with_x_mirror
#   define arm_2d_tile_fill_with_y_mirror                                       \
                arm_2d_rgb32_tile_fill_with_y_mirror
#   define arm_2d_tile_fill_with_xy_mirror                                      \
                arm_2d_rgb32_tile_fill_with_xy_mirror
#   define arm_2d_tile_fill_only                                                \
                arm_2d_rgb32_tile_fill_only
#   define arm_2d_tile_copy_with_x_mirror                                       \
                arm_2d_rgb32_tile_copy_with_x_mirror
#   define arm_2d_tile_copy_with_y_mirror                                       \
                arm_2d_rgb32_tile_copy_with_y_mirror
#   define arm_2d_tile_copy_with_xy_mirror                                      \
                arm_2d_rgb32_tile_copy_with_xy_mirror
#   define arm_2d_tile_copy_only                                                \
                arm_2d_rgb32_tile_copy_only
#   define arm_2d_tile_copy                                                     \
                arm_2d_rgb32_tile_copy
#   define arm_2d_fill_colour                                                   \
                arm_2d_rgb32_fill_colour
#   define arm_2d_tile_copy_with_colour_keying                                  \
                arm_2d_rgb32_tile_copy_with_colour_keying
#   define arm_2d_fill_colour_with_opacity                                      \
                arm_2d_cccn888_fill_colour_with_opacity
#   define arm_2d_alpha_blending                                                \
                arm_2d_cccn888_alpha_blending
#   define arm_2d_tile_copy_with_des_mask                                       \
                arm_2d_cccn888_tile_copy_with_des_mask
#   define arm_2d_tile_copy_with_src_mask                                       \
                arm_2d_cccn888_tile_copy_with_src_mask
#   define arm_2dp_tile_transform_with_opacity                                  \
                arm_2dp_cccn888_tile_transform_with_opacity
#   define arm_2dp_tile_transform_with_src_mask_and_opacity                     \
                arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity
#   define arm_2dp_tile_rotation                arm_2dp_cccn888_tile_rotation
#   define arm_2dp_tile_rotation_with_alpha                                     \
                arm_2dp_cccn888_tile_rotation_with_alpha
#   define arm_2dp_tile_rotation_with_src_mask_and_opacity                      \
                arm_2dp_cccn888_tile_rotation_with_src_mask_and_opacity
#   define arm_2dp_tile_rotation_with_src_mask                                  \
                arm_2dp_cccn888_tile_rotation_with_src_mask

#   define arm_2d_tile_rotation                                  \
                arm_2d_cccn888_tile_rotation

#else
#   error Unsupported colour depth!
#endif
