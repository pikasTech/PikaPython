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
 * Description:  Public header file to contain the alpha-blending related
 *               APIs
 *
 * $Date:        03. Oct 2021
 * $Revision:    V.1.0.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */
 
#ifndef __ARM_2D_ALPHA_BLENDING_H__
#define __ARM_2D_ALPHA_BLENDING_H__

/*============================ INCLUDES ======================================*/

#include "arm_2d_types.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wc11-extensions"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define arm_2d_gray8_alpha_blending(  __SRC_ADDR,   /*!< source tile address */ \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __ALPHA)      /*!< alpha */               \
            arm_2dp_gray8_alpha_blending(  NULL,                                \
                                            (__SRC_ADDR),                       \
                                            (__DES_ADDR),                       \
                                            (__REGION_ADDR),                    \
                                            (__ALPHA))

#define arm_2d_rgb565_alpha_blending( __SRC_ADDR,   /*!< source tile address */ \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __ALPHA)      /*!< alpha */               \
            arm_2dp_rgb565_alpha_blending(  NULL,                               \
                                            (__SRC_ADDR),                       \
                                            (__DES_ADDR),                       \
                                            (__REGION_ADDR),                    \
                                            (__ALPHA))
                                            
#define arm_2d_rgb888_alpha_blending( __SRC_ADDR,   /*!< source tile address */ \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __ALPHA)      /*!< alpha */               \
            arm_2dp_cccn888_alpha_blending(  NULL,                              \
                                            (__SRC_ADDR),                       \
                                            (__DES_ADDR),                       \
                                            (__REGION_ADDR),                    \
                                            (__ALPHA))

#define arm_2d_cccn888_alpha_blending( __SRC_ADDR,   /*!< source tile address */\
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __ALPHA)      /*!< alpha */               \
            arm_2dp_cccn888_alpha_blending(  NULL,                              \
                                            (__SRC_ADDR),                       \
                                            (__DES_ADDR),                       \
                                            (__REGION_ADDR),                    \
                                            (__ALPHA))

#define arm_2d_gray8_fill_colour_with_alpha(                                    \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __COLOUR,     /*!< colour */              \
                                      __ALPHA)      /*!< alpha */               \
            arm_2dp_gray8_fill_colour_with_opacity(  NULL,                      \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__COLOUR),                    \
                                                 (__ALPHA))

#define arm_2d_rgb565_fill_colour_with_alpha(                                   \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __COLOUR,     /*!< colour */              \
                                      __ALPHA)      /*!< alpha */               \
            arm_2dp_rgb565_fill_colour_with_opacity(  NULL,                     \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__COLOUR),                    \
                                                 (__ALPHA))
 
#define arm_2d_rgb565_fill_colour_with_opacity(                                 \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __COLOUR,     /*!< colour */              \
                                      __ALPHA)      /*!< alpha */               \
            arm_2dp_rgb565_fill_colour_with_opacity(  NULL,                     \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__COLOUR),                    \
                                                 (__ALPHA))

#define arm_2d_rgb888_fill_colour_with_alpha(                                   \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __COLOUR,     /*!< colour */              \
                                      __ALPHA)      /*!< alpha */               \
            arm_2dp_cccn888_fill_colour_with_opacity(  NULL,                    \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__COLOUR),                    \
                                                 (__ALPHA))

#define arm_2d_rgb888_fill_colour_with_opacity(                                 \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __COLOUR,     /*!< colour */              \
                                      __ALPHA)      /*!< alpha */               \
            arm_2dp_cccn888_fill_colour_with_opacity(  NULL,                    \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__COLOUR),                    \
                                                 (__ALPHA))

#define arm_2d_cccn888_fill_colour_with_alpha(                                  \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __COLOUR,     /*!< colour */              \
                                      __ALPHA)      /*!< alpha */               \
            arm_2dp_cccn888_fill_colour_with_opacity(  NULL,                    \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__COLOUR),                    \
                                                 (__ALPHA))

#define arm_2d_cccn888_fill_colour_with_opacity(                                \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __COLOUR,     /*!< colour */              \
                                      __ALPHA)      /*!< alpha */               \
            arm_2dp_cccn888_fill_colour_with_opacity(  NULL,                    \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__COLOUR),                    \
                                                 (__ALPHA))

#define arm_2d_gray8_fill_colour_with_alpha_mask(                               \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR)       /*!< colour */              \
            arm_2dp_gray8_fill_colour_with_mask(                                \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR))

#define arm_2d_gray8_fill_colour_with_mask(                                     \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR)       /*!< colour */              \
            arm_2dp_gray8_fill_colour_with_mask(                                \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR))

#define arm_2d_rgb565_fill_colour_with_alpha_mask(                              \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR)       /*!< colour */              \
            arm_2dp_rgb565_fill_colour_with_mask(                               \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR))

#define arm_2d_rgb565_fill_colour_with_mask(                                    \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR)       /*!< colour */              \
            arm_2dp_rgb565_fill_colour_with_mask(                               \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR))

#define arm_2d_rgb888_fill_colour_with_alpha_mask(                              \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR)       /*!< colour */              \
            arm_2dp_cccn888_fill_colour_with_mask(                              \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR))

#define arm_2d_rgb888_fill_colour_with_mask(                                    \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR)       /*!< colour */              \
            arm_2dp_cccn888_fill_colour_with_mask(                              \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR))

#define arm_2d_cccn888_fill_colour_with_alpha_mask(                             \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR)       /*!< colour */              \
            arm_2dp_cccn888_fill_colour_with_mask(                              \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR))

#define arm_2d_cccn888_fill_colour_with_mask(                                   \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR)       /*!< colour */              \
            arm_2dp_cccn888_fill_colour_with_mask(                              \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR))


#define arm_2d_gray8_fill_colour_with_alpha_mask_and_opacity(                   \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR,       /*!< colour */              \
                                    __OPACITY)                                  \
            arm_2dp_gray8_fill_colour_with_mask_and_opacity(                    \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR),                                \
                                     (__OPACITY))

#define arm_2d_gray8_fill_colour_with_mask_and_opacity(                         \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR,       /*!< colour */              \
                                    __OPACITY)                                  \
            arm_2dp_gray8_fill_colour_with_mask_and_opacity(                    \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR),                                \
                                     (__OPACITY))

#define arm_2d_rgb565_fill_colour_with_alpha_mask_and_opacity(                  \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR,       /*!< colour */              \
                                    __OPACITY)                                  \
            arm_2dp_rgb565_fill_colour_with_mask_and_opacity(                   \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR),                                \
                                     (__OPACITY))

#define arm_2d_rgb565_fill_colour_with_mask_and_opacity(                        \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR,       /*!< colour */              \
                                    __OPACITY)                                  \
            arm_2dp_rgb565_fill_colour_with_mask_and_opacity(                   \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR),                                \
                                     (__OPACITY))

#define arm_2d_rgb888_fill_colour_with_alpha_mask_and_opacity(                  \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR,       /*!< colour */              \
                                    __OPACITY)                                  \
            arm_2dp_cccn888_fill_colour_with_mask_and_opacity(                  \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR),                                \
                                     (__OPACITY))

#define arm_2d_rgb888_fill_colour_with_mask_and_opacity(                        \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR,       /*!< colour */              \
                                    __OPACITY)                                  \
            arm_2dp_cccn888_fill_colour_with_mask_and_opacity(                  \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR),                                \
                                     (__OPACITY))

#define arm_2d_cccn888_fill_colour_with_alpha_mask_and_opacity(                 \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR,       /*!< colour */              \
                                    __OPACITY)                                  \
            arm_2dp_cccn888_fill_colour_with_mask_and_opacity(                  \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR),                                \
                                     (__OPACITY))

#define arm_2d_cccn888_fill_colour_with_mask_and_opacity(                       \
                                    __TARGET_ADDR,  /*!< target tile address*/  \
                                    __REGION_ADDR,  /*!< target region address*/\
                                    __ALPHA_ADDR,   /*!< alpha tile address */  \
                                    __COLOUR,       /*!< colour */              \
                                    __OPACITY)                                  \
            arm_2dp_cccn888_fill_colour_with_mask_and_opacity(                  \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR),                                \
                                     (__OPACITY))

#define arm_2d_gray8_alpha_blending_with_colour_keying(                         \
                                      __SRC_ADDR,   /*!< source tile address */ \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __ALPHA,     /*!< colour */               \
                                      __COLOUR)      /*!< alpha */              \
            arm_2dp_gray8_alpha_blending_with_colour_keying(                    \
                                                    NULL,                       \
                                                 (__SRC_ADDR),                  \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__ALPHA),                     \
                                                 (__COLOUR))

#define arm_2d_gray8_alpha_blending_with_colour_masking(                        \
                                      __SRC_ADDR,   /*!< source tile address */ \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __ALPHA,     /*!< colour */               \
                                      __COLOUR)      /*!< alpha */              \
            arm_2dp_gray8_alpha_blending_with_colour_keying(                    \
                                                    NULL,                       \
                                                 (__SRC_ADDR),                  \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__ALPHA),                     \
                                                 (__COLOUR))


#define arm_2d_rgb565_alpha_blending_with_colour_keying(                        \
                                      __SRC_ADDR,   /*!< source tile address */ \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __ALPHA,     /*!< colour */               \
                                      __COLOUR)      /*!< alpha */              \
            arm_2dp_rgb565_alpha_blending_with_colour_keying(                   \
                                                    NULL,                       \
                                                 (__SRC_ADDR),                  \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__ALPHA),                     \
                                                 (__COLOUR))

#define arm_2d_rgb565_alpha_blending_with_colour_masking(                       \
                                      __SRC_ADDR,   /*!< source tile address */ \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __ALPHA,     /*!< colour */               \
                                      __COLOUR)      /*!< alpha */              \
            arm_2dp_rgb565_alpha_blending_with_colour_keying(                   \
                                                    NULL,                       \
                                                 (__SRC_ADDR),                  \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__ALPHA),                     \
                                                 (__COLOUR))

#define arm_2d_rgb888_alpha_blending_with_colour_masking(                       \
                                      __SRC_ADDR,   /*!< source tile address */ \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __ALPHA,     /*!< colour */               \
                                      __COLOUR)      /*!< alpha */              \
            arm_2dp_cccn888_alpha_blending_with_colour_keying(                  \
                                                    NULL,                       \
                                                 (__SRC_ADDR),                  \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__ALPHA),                     \
                                                 (__COLOUR))

#define arm_2d_rgb888_alpha_blending_with_colour_keying(                        \
                                      __SRC_ADDR,   /*!< source tile address */ \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __ALPHA,     /*!< colour */               \
                                      __COLOUR)      /*!< alpha */              \
            arm_2dp_cccn888_alpha_blending_with_colour_keying(                  \
                                                    NULL,                       \
                                                 (__SRC_ADDR),                  \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__ALPHA),                     \
                                                 (__COLOUR))

#define arm_2d_cccn888_alpha_blending_with_colour_masking(                      \
                                      __SRC_ADDR,   /*!< source tile address */ \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __ALPHA,     /*!< colour */               \
                                      __COLOUR)      /*!< alpha */              \
            arm_2dp_cccn888_alpha_blending_with_colour_keying(                  \
                                                    NULL,                       \
                                                 (__SRC_ADDR),                  \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__ALPHA),                     \
                                                 (__COLOUR))

#define arm_2d_cccn888_alpha_blending_with_colour_keying(                       \
                                      __SRC_ADDR,   /*!< source tile address */ \
                                      __DES_ADDR,   /*!< target tile address */ \
                                      __REGION_ADDR,/*!< region address */      \
                                      __ALPHA,     /*!< colour */               \
                                      __COLOUR)      /*!< alpha */              \
            arm_2dp_cccn888_alpha_blending_with_colour_keying(                  \
                                                    NULL,                       \
                                                 (__SRC_ADDR),                  \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__ALPHA),                     \
                                                 (__COLOUR))

#define arm_2d_gray8_tile_copy_with_masks(                                      \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __SRC_MSK_ADDR, /*!< source mask address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __DES_MSK_ADDR, /*!< target mask address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_gray8_tile_copy_with_masks(                                 \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))

#define arm_2d_gray8_tile_copy_with_alpha_masks(                                \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __SRC_MSK_ADDR, /*!< source mask address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __DES_MSK_ADDR, /*!< target mask address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_gray8_tile_copy_with_masks(                                 \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))

#define arm_2d_rgb565_tile_copy_with_masks(                                     \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __SRC_MSK_ADDR, /*!< source mask address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __DES_MSK_ADDR, /*!< target mask address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_rgb565_tile_copy_with_masks(                                \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))

#define arm_2d_rgb565_tile_copy_with_alpha_masks(                               \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __SRC_MSK_ADDR, /*!< source mask address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __DES_MSK_ADDR, /*!< target mask address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_rgb565_tile_copy_with_masks(                                \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))

#define arm_2d_cccn888_tile_copy_with_masks(                                    \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __SRC_MSK_ADDR, /*!< source mask address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __DES_MSK_ADDR, /*!< target mask address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_cccn888_tile_copy_with_masks(                               \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))

#define arm_2d_cccn888_tile_copy_with_alpha_masks(                              \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __SRC_MSK_ADDR, /*!< source mask address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __DES_MSK_ADDR, /*!< target mask address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_cccn888_tile_copy_with_masks(                               \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))

#define arm_2d_rgb888_tile_copy_with_masks(                                     \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __SRC_MSK_ADDR, /*!< source mask address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __DES_MSK_ADDR, /*!< target mask address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_cccn888_tile_copy_with_masks(                               \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))

#define arm_2d_rgb888_tile_copy_with_alpha_mask(                                \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __SRC_MSK_ADDR, /*!< source mask address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __DES_MSK_ADDR, /*!< target mask address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_cccn888_tile_copy_with_masks(                               \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))
                                        

#define arm_2d_gray8_tile_copy_with_src_mask(                                   \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __SRC_MSK_ADDR, /*!< source mask address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_gray8_tile_copy_with_src_mask(                              \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__REGION),                             \
                                        (__MODE))


#define arm_2d_rgb565_tile_copy_with_src_mask(                                  \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __SRC_MSK_ADDR, /*!< source mask address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_rgb565_tile_copy_with_src_mask(                             \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__REGION),                             \
                                        (__MODE))


#define arm_2d_cccn888_tile_copy_with_src_mask(                                 \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __SRC_MSK_ADDR, /*!< source mask address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_cccn888_tile_copy_with_src_mask(                            \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__REGION),                             \
                                        (__MODE))


#define arm_2d_rgb888_tile_copy_with_src_mask(                                  \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __SRC_MSK_ADDR, /*!< source mask address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_cccn888_tile_copy_with_src_mask(                            \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__REGION),                             \
                                        (__MODE))
                                        

#define arm_2d_gray8_tile_copy_with_des_mask(                                   \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __DES_MSK_ADDR, /*!< target mask address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_gray8_tile_copy_with_des_mask(                              \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))


#define arm_2d_rgb565_tile_copy_with_des_mask(                                  \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __DES_MSK_ADDR, /*!< target mask address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_rgb565_tile_copy_with_des_mask(                             \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))


#define arm_2d_cccn888_tile_copy_with_des_mask(                                 \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __DES_MSK_ADDR, /*!< target mask address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_cccn888_tile_copy_with_des_mask(                            \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))

#define arm_2d_rgb888_tile_copy_with_des_mask(                                  \
                                    __SRC_ADDR,     /*!< source tile address */ \
                                    __DES_ADDR,     /*!< target tile address */ \
                                    __DES_MSK_ADDR, /*!< target mask address */ \
                                    __REGION,       /*!< region address */      \
                                    __MODE)         /*!< copy mode */           \
            arm_2dp_cccn888_tile_copy_with_des_mask(                            \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))

/*============================ TYPES =========================================*/

/*! \note arm_2d_op_alpha_t inherits from arm_2d_op_src_t explicitly 
 */
typedef struct arm_2d_op_alpha_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile 
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile 
    }Source;
    uint32_t wMode;
    uint8_t chRatio;
} arm_2d_op_alpha_t;


/*! \note arm_2d_op_alpha_cl_key_t inherits from arm_2d_op_src_t explicitly 
 */
typedef struct arm_2d_op_alpha_cl_key_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile 
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile 
    }Source;
    uint32_t wMode;
    uint8_t chRatio;
    union {
        uint8_t  chColour;
        uint16_t hwColour;
        uint32_t wColour;
    };
} arm_2d_op_alpha_cl_key_t;


/*! \note arm_2d_op_fill_cl_msk_t inherits from arm_2d_op_src_t explicitly 
 */
typedef struct arm_2d_op_fill_cl_msk_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< Alpha Mask tile
    } Mask;
    uint32_t wMode;
    union {
        uint8_t  chColour;
        uint16_t hwColour;
        uint32_t wColour;
    };
} arm_2d_op_fill_cl_msk_t;


/*! \note arm_2d_op_fill_cl_msk_t inherits from arm_2d_op_src_t explicitly 
 */
typedef struct arm_2d_op_alpha_fill_cl_msk_opc_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< Alpha Mask tile
    } Mask;
    uint32_t wMode;
    union {
        uint8_t  chColour;
        uint16_t hwColour;
        uint32_t wColour;
    };
    uint8_t chOpacity;
} arm_2d_op_alpha_fill_cl_msk_opc_t;

/*! \note arm_2d_op_fill_cl_t inherits from arm_2d_op_t explicitly 
 */
typedef struct arm_2d_op_fill_cl_opc_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile 
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    union {
        uint8_t chColour;
        uint16_t hwColour;
        uint32_t wColour;
    };
    uint_fast8_t chRatio;                       //!< transparency ratio 
} arm_2d_op_fill_cl_opc_t;


typedef arm_2d_op_src_msk_t arm_2d_op_cp_msk_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*----------------------------------------------------------------------------*
 * Copy tile to destination with specified transparency ratio (0~255)         *
 *----------------------------------------------------------------------------*/

extern 
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_gray8_alpha_blending(  arm_2d_op_alpha_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio);
                             
extern 
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb565_alpha_blending( arm_2d_op_alpha_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio);

extern 
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_cccn888_alpha_blending( arm_2d_op_alpha_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio);
                                     

/*----------------------------------------------------------------------------*
 * Fill a specified region with a given colour and transparency ratio (0~255) *
 *----------------------------------------------------------------------------*/

extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_opacity( 
                                                arm_2d_op_fill_cl_opc_t *ptOP,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion,
                                                uint8_t chColour,
                                                uint_fast8_t chRatio);

extern 
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_opacity( 
                                                arm_2d_op_fill_cl_opc_t *ptOP,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion,
                                                arm_2d_color_rgb565_t tColour,
                                                uint_fast8_t chRatio);

extern 
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_opacity( 
                                                arm_2d_op_fill_cl_opc_t *ptOP,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion,
                                                arm_2d_color_rgb888_t tColour,
                                                uint_fast8_t chRatio);

/*----------------------------------------------------------------------------*
 * Fill tile with a specified colour and an alpha mask                        *
 *----------------------------------------------------------------------------*/

extern
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_mask(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        uint8_t chColour);

extern
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_mask( 
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_rgb565_t tColour);

extern                                  
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_mask( 
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_cccn888_t tColour);


/*----------------------------------------------------------------------------*
 * Fill tile with a specified colour, an alpha mask and a specified opacity   *
 *----------------------------------------------------------------------------*/

extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        uint8_t chColour,
                                        uint8_t chOpacity);
                                        
extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_rgb565_t tColour,
                                        uint8_t chOpacity);

extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_rgb888_t tColour,
                                        uint8_t chOpacity);

/*----------------------------------------------------------------------------*
 * Blend tile and background with a specified transparency ratio(0~255) and a *
 * specified transparency color mask                                          *
 *----------------------------------------------------------------------------*/

extern 
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_gray8_alpha_blending_with_colour_keying(
                                            arm_2d_op_alpha_cl_key_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio,
                                            uint8_t chColour);

extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb565_alpha_blending_with_colour_keying(
                                                arm_2d_op_alpha_cl_key_t *ptOP,
                                                const arm_2d_tile_t *ptSource, 
                                                const arm_2d_tile_t *ptTarget, 
                                                const arm_2d_region_t *ptRegion,
                                                uint_fast8_t chRatio,
                                                arm_2d_color_rgb565_t tColour);
                                            
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_cccn888_alpha_blending_with_colour_keying(
                                                arm_2d_op_alpha_cl_key_t *ptOP,
                                                const arm_2d_tile_t *ptSource, 
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion,
                                                uint_fast8_t chRatio,
                                                arm_2d_color_rgb888_t tColour);

/*----------------------------------------------------------------------------*
 * Copy tile to destination with both a source mask and a target mask         *
 *----------------------------------------------------------------------------*/

extern
ARM_NONNULL(2,3,4,5)
arm_fsm_rt_t arm_2dp_gray8_tile_copy_with_masks(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode); 

extern
ARM_NONNULL(2,3,4,5)
arm_fsm_rt_t arm_2dp_rgb565_tile_copy_with_masks(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode);

extern
ARM_NONNULL(2,3,4,5)
arm_fsm_rt_t arm_2dp_cccn888_tile_copy_with_masks(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode); 

/*----------------------------------------------------------------------------*
 * Copy tile to destination with a specified source mask                      *
 *----------------------------------------------------------------------------*/

extern
ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_gray8_tile_copy_with_src_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode); 

extern
ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_rgb565_tile_copy_with_src_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode);

extern
ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_cccn888_tile_copy_with_src_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode); 

/*----------------------------------------------------------------------------*
 * Copy tile to destination with a specified target mask                      *
 *----------------------------------------------------------------------------*/

extern
ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_gray8_tile_copy_with_des_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode); 

extern
ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_rgb565_tile_copy_with_des_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode);

extern
ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_cccn888_tile_copy_with_des_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode); 


#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
