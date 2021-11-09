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
 * Title:        #include "arm_2d_rotation.h"
 * Description:  Public header file to contain the APIs for rotation
 *
 * $Date:        29 April 2021
 * $Revision:    V.0.5.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_ROTATION_H__
#define __ARM_2D_ROTATION_H__

/*============================ INCLUDES ======================================*/

#include "arm_2d_types.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define arm_2d_gray8_tile_rotation_prepare(__SRC_TILE_ADDR,                     \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR)                       \
            arm_2dp_gray8_tile_rotation_prepare(  NULL,                         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   (__MSK_COLOUR))

#define arm_2d_rgb565_tile_rotation_prepare(__SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR)                       \
            arm_2dp_rgb565_tile_rotation_prepare(  NULL,                        \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   (__MSK_COLOUR))

                                     
#define arm_2d_rgb888_tile_rotation_prepare(__SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR)                       \
            arm_2dp_cccn888_tile_rotation_prepare(  NULL,                       \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   (__MSK_COLOUR))

#define arm_2d_rgb565_tile_rotation_with_alpha_prepare(                         \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_rgb565_tile_rotation_with_opacity_prepare(  NULL,           \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_rgb565_tile_rotation_with_opacity_prepare(                       \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_rgb565_tile_rotation_with_opacity_prepare(  NULL,           \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_rgb888_tile_rotation_with_alpha_prepare(                         \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_rotation_with_opacity_prepare(  NULL,          \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))
                                     
#define arm_2d_rgb888_tile_rotation_with_opacity_prepare(                       \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_rotation_with_opacity_prepare(  NULL,          \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))


#define arm_2d_cccn888_tile_rotation_with_alpha_prepare(                        \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_rotation_with_opacity_prepare(  NULL,          \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_cccn888_tile_rotation_with_opacity_prepare(                      \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_rotation_with_opacity_prepare(  NULL,          \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_tile_rotate( __DES_TILE_ADDR,                                    \
                            __DES_REGION_ADDR,                                  \
                            __DES_CENTRE_ADDR)                                  \
            arm_2dp_tile_rotate(NULL,                                           \
                                (__DES_TILE_ADDR),                              \
                                (__DES_REGION_ADDR),                            \
                                (__DES_CENTRE_ADDR))


/*! \note arm_2dp_gray8_tile_rotation(), arm_2dp_rgb565_tile_rotation() and 
 *!       arm_2d_2dp_rgb888_tile_rotation() relies on the boolean variable
 *!       bIsNewFrame. Please make sure you have define it with the correct
 *!       name and the corresponding value. If you don't use the PFB interfaces
 *!       for neight the low level rendering nor the high level GUI drawing, 
 *!       please find such variable with the value "true".
 */
 
#define arm_2dp_gray8_tile_rotation(    __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_rotation_prepare(                                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_rotate((arm_2d_op_rotate_t *)(__CB_ADDR),                  \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
 
#define arm_2dp_rgb565_tile_rotation(   __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_rotation_prepare(                               \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_rotate((arm_2d_op_rotate_t *)(__CB_ADDR),                  \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2dp_cccn888_tile_rotation(   __CB_ADDR,                             \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,...)                       \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_rotation_prepare(                              \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_rotate((arm_2d_op_rotate_t *)(__CB_ADDR),                  \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        

#define arm_2dp_rgb565_tile_rotation_with_alpha(                                \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO,...)                            \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_rotation_with_opacity_prepare(                  \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_rotate((arm_2d_op_rotate_t *)(__CB_ADDR),                  \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_rgb565_tile_rotation_with_opacity(                              \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO,...)                            \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_rotation_with_opacity_prepare(                  \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_rotate((arm_2d_op_rotate_t *)(__CB_ADDR),                  \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_rgb888_tile_rotation_with_alpha(                                \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_rotation_with_opacity_prepare(                 \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_rotate((arm_2d_op_rotate_t *)(__CB_ADDR),                  \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_rgb888_tile_rotation_with_opacity(                              \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_rotation_with_opacity_prepare(                 \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_rotate((arm_2d_op_rotate_t *)(__CB_ADDR),                  \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2dp_cccn888_tile_rotation_with_alpha(                               \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_rotation_with_opacity_prepare(                 \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_rotate((arm_2d_op_rotate_t *)(__CB_ADDR),                  \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_cccn888_tile_rotation_with_opacity(                             \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_rotation_with_opacity_prepare(                 \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_rotate((arm_2d_op_rotate_t *)(__CB_ADDR),                  \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_gray8_tile_rotation(                                             \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR, ...)                      \
        ({{                                                                     \
            arm_2dp_gray8_tile_rotation_prepare(                                \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_rotate(NULL,                                               \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2d_rgb565_tile_rotation(                                            \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR, ...)                      \
        ({{                                                                     \
            arm_2dp_rgb565_tile_rotation_prepare(                               \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_rotate(NULL,                                               \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2d_rgb888_tile_rotation(                                            \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR, ...)                      \
        ({{                                                                     \
            arm_2dp_cccn888_tile_rotation_prepare(                              \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_rotate(NULL,                                               \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb565_tile_rotation_with_alpha(                                 \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_rgb565_tile_rotation_with_opacity_prepare(                  \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_rotate(NULL,                                               \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2d_rgb565_tile_rotation_with_opacity(                               \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_rgb565_tile_rotation_with_opacity_prepare(                  \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_rotate(NULL,                                               \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2d_rgb888_tile_rotation_with_alpha(                                 \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_rotation_with_opacity_prepare(                 \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_rotate(NULL,                                               \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb888_tile_rotation_with_opacity(                               \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_rotation_with_opacity_prepare(                 \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_rotate(NULL,                                               \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_cccn888_tile_rotation_with_alpha(                                \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_rotation_with_opacity_prepare(                 \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_rotate(NULL,                                               \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_cccn888_tile_rotation_with_opacity(                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_rotation_with_opacity_prepare(                 \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_rotate(NULL,                                               \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


/*============================ TYPES =========================================*/

typedef struct __arm_2d_rotate_info_t {
    float                   fAngle;         //!< target angle
    arm_2d_location_t       tCenter;
    union {
        uint8_t             chColour;
        uint32_t            wColour;
        uint16_t            hwColour;
    } Mask;                                 //!< the colour to fill when out of range
    
ARM_PRIVATE(
    arm_2d_location_t       tDummySourceOffset;
    struct {
        arm_2d_region_t     tRegion;
        arm_2d_tile_t       tTile;
    } Target;
)
} __arm_2d_rotate_info_t;

/*! \brief arm_2d_op_rotat_t is inherit from arm_2d_op_src_orig_t
 */
typedef struct arm_2d_op_rotate_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile
    }Source;
    uint32_t wMode;
    
    struct {
        const arm_2d_tile_t     *ptTile;        //!< the origin tile
        arm_2d_tile_t           tDummySource;   //!< the buffer for the source
    }Origin;
    
    __arm_2d_rotate_info_t      tRotate;
    
} arm_2d_op_rotate_t;

/*! \brief arm_2d_op_rotate_opacity_t is inherit from arm_2d_op_rotate_t
 */
typedef struct arm_2d_op_rotate_opacity_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile
    }Source;
    uint32_t wMode;
    
    struct {
        const arm_2d_tile_t     *ptTile;        //!< the origin tile
        arm_2d_tile_t           tDummySource;   //!< the buffer for the source
    }Origin;
    
    __arm_2d_rotate_info_t      tRotate;
    uint8_t                     chRatio;
    
} arm_2d_op_rotate_opacity_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(2)
arm_2d_err_t arm_2dp_gray8_tile_rotation_prepare(
                                        arm_2d_op_rotate_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        uint8_t chFillColour);

extern
ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_rotation_prepare(
                                        arm_2d_op_rotate_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        uint16_t hwFillColour);
                                     
extern
ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_tile_rotation_prepare(
                                        arm_2d_op_rotate_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        uint32_t wFillColour);


extern
ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_rotation_with_opacity_prepare(
                                        arm_2d_op_rotate_opacity_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        uint16_t hwFillColour,
                                        uint_fast8_t chRatio);
                                     
extern
ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_tile_rotation_with_opacity_prepare(
                                        arm_2d_op_rotate_opacity_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        uint32_t wFillColour,
                                        uint_fast8_t chRatio);


extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_tile_rotate(arm_2d_op_rotate_t *ptOP,
                                 const arm_2d_tile_t *ptTarget,
                                 const arm_2d_region_t *ptRegion,
                                 const arm_2d_location_t *ptTargetCentre);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
