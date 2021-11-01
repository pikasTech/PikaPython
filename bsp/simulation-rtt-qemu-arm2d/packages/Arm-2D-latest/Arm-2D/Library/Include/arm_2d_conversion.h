/*
 * Copyright (C) 2020 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* ----------------------------------------------------------------------
 * Project:      Arm-2D Library
 * Title:        #include "arm_2d.h"
 * Description:  Public header file to contain the APIs for colour space
 *               conversions
 *
 * $Date:        01. December 2020
 * $Revision:    V.0.5.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_CONVERSION_H__
#define __ARM_2D_CONVERSION_H__

/*============================ INCLUDES ======================================*/

#include "arm_2d_types.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define arm_2d_convert_colour_to_rgb888(__SRC_ADDR, /*!< source tile address */ \
                                        __DES_ADDR /*!< target tile address */) \
            arm_2dp_convert_colour_to_rgb888(   NULL,                           \
                                                (__SRC_ADDR),                   \
                                                (__DES_ADDR))

#define arm_2d_convert_colour_to_rgb565(__SRC_ADDR, /*!< source tile address */ \
                                        __DES_ADDR /*!< target tile address */) \
            arm_2dp_convert_colour_to_rgb565(   NULL,                           \
                                                (__SRC_ADDR),                   \
                                                (__DES_ADDR))

/*============================ TYPES =========================================*/

typedef arm_2d_op_src_t arm_2d_op_cl_convt_t;



/*! \brief 3x16-bit packed RGB color
 *!        autovectorizer friendly format
 */
typedef union {
    uint16_t        RGB[3];
    struct {
        uint16_t        R;
        uint16_t        G;
        uint16_t        B;
    };
} __arm_2d_color_fast_rgb_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*----------------------------------------------------------------------------*
 * RGB565 channels extraction/packing                                         *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE void __arm_2d_rgb565_unpack(uint16_t hwColor,
                                            __arm_2d_color_fast_rgb_t * ptRGB)
{
    /* uses explicit extraction, leading to a more efficient autovectorized code */
    uint16_t maskRunpk = 0x001f, maskGunpk = 0x003f;

    ptRGB->R = (uint16_t) ((hwColor & maskRunpk) << 3);
    ptRGB->B = (uint16_t) ((hwColor >> 11) << 3);
    ptRGB->G = (uint16_t) (((hwColor >> 5) & maskGunpk) << 2);
}

__STATIC_INLINE uint16_t __arm_2d_rgb565_pack(__arm_2d_color_fast_rgb_t * ptRGB)
{
    arm_2d_color_rgb565_t tOutput = {
        .u5R = (uint16_t) ptRGB->R >> 3,
        .u6G = (uint16_t) ptRGB->G >> 2,
        .u5B = (uint16_t) ptRGB->B >> 3,
    };
    return tOutput.tValue;
}

__STATIC_INLINE uint32_t __arm_2d_cccn888_pack(__arm_2d_color_fast_rgb_t * ptRGB)
{
    arm_2d_color_rgba8888_t tOutput = {
        .u8R = (uint16_t) ptRGB->R,
        .u8G = (uint16_t) ptRGB->G,
        .u8B = (uint16_t) ptRGB->B,
        .u8A = 0xFF,
    };
    return tOutput.tValue;
}


/*----------------------------------------------------------------------------*
 * Colour Conversion                                                          *
 *----------------------------------------------------------------------------*/

extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_convert_colour_to_rgb888(  arm_2d_op_cl_convt_t *ptOP,
                                                const arm_2d_tile_t *ptSource,
                                                const arm_2d_tile_t *ptTarget);

extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_convert_colour_to_rgb565(  arm_2d_op_cl_convt_t *ptOP,
                                                const arm_2d_tile_t *ptSource,
                                                const arm_2d_tile_t *ptTarget);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
