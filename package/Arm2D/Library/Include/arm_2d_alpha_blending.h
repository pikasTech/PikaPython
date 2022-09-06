/*
 * Copyright (C) 2022 Arm Limited or its affiliates. All rights reserved.
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
 * $Date:        17. June 2022
 * $Revision:    V.1.0.1
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

/*!
 * \addtogroup Deprecated
 * @{
 */
#define arm_2d_rgb565_fill_colour_with_alpha                                    \
            arm_2d_rgb565_fill_colour_with_opacity

#define arm_2d_rgb888_fill_colour_with_alpha                                    \
            arm_2d_rgb888_fill_colour_with_opacity

#define arm_2d_cccn888_fill_colour_with_alpha                                   \
            arm_2d_cccn888_fill_colour_with_opacity

#define arm_2d_gray8_fill_colour_with_alpha_mask                                \
            arm_2d_gray8_fill_colour_with_mask

#define arm_2d_rgb565_fill_colour_with_alpha_mask                               \
            arm_2d_rgb565_fill_colour_with_mask

#define arm_2d_rgb888_fill_colour_with_alpha_mask                               \
            arm_2d_rgb888_fill_colour_with_mask

#define arm_2d_cccn888_fill_colour_with_alpha_mask                              \
            arm_2d_cccn888_fill_colour_with_mask

#define arm_2d_gray8_fill_colour_with_alpha_mask_and_opacity                    \
            arm_2d_gray8_fill_colour_with_mask_and_opacity

#define arm_2d_rgb565_fill_colour_with_alpha_mask_and_opacity                   \
            arm_2d_rgb565_fill_colour_with_mask_and_opacity

#define arm_2d_rgb888_fill_colour_with_alpha_mask_and_opacity                   \
            arm_2d_rgb888_fill_colour_with_mask_and_opacity

#define arm_2d_cccn888_fill_colour_with_alpha_mask_and_opacity                  \
            arm_2d_cccn888_fill_colour_with_mask_and_opacity

#define arm_2d_gray8_alpha_blending_with_colour_masking                         \
            arm_2d_gray8_alpha_blending_with_colour_keying

#define arm_2d_rgb565_alpha_blending_with_colour_masking                        \
            arm_2d_rgb565_alpha_blending_with_colour_keying

#define arm_2d_rgb888_alpha_blending_with_colour_masking                        \
            arm_2d_rgb888_alpha_blending_with_colour_keying

#define arm_2d_cccn888_alpha_blending_with_colour_masking                       \
            arm_2d_cccn888_alpha_blending_with_colour_keying

#define arm_2d_gray8_tile_copy_with_alpha_masks                                 \
            arm_2d_gray8_tile_copy_with_masks

#define arm_2d_rgb565_tile_copy_with_alpha_masks                                \
            arm_2d_rgb565_tile_copy_with_masks

#define arm_2d_cccn888_tile_copy_with_alpha_masks                               \
            arm_2d_cccn888_tile_copy_with_masks

#define arm_2d_rgb888_tile_copy_with_alpha_mask                                 \
            arm_2d_rgb888_tile_copy_with_masks

/*! @} */

/*!
 * \addtogroup gAlpha 4 Alpha Blending Operations
 * @{
 */

/*============================ MACROFIED FUNCTIONS ===========================*/

#define arm_2d_gray8_alpha_blending(  __SRC_ADDR,   /*   source tile address */ \
                                      __DES_ADDR,   /*   target tile address */ \
                                      __REGION_ADDR,/*   region address */      \
                                      __ALPHA)      /*   alpha */               \
            arm_2dp_gray8_alpha_blending(  NULL,                                \
                                            (__SRC_ADDR),                       \
                                            (__DES_ADDR),                       \
                                            (__REGION_ADDR),                    \
                                            (__ALPHA))

#define arm_2d_rgb565_alpha_blending( __SRC_ADDR,   /*   source tile address */ \
                                      __DES_ADDR,   /*   target tile address */ \
                                      __REGION_ADDR,/*   region address */      \
                                      __ALPHA)      /*   alpha */               \
            arm_2dp_rgb565_alpha_blending(  NULL,                               \
                                            (__SRC_ADDR),                       \
                                            (__DES_ADDR),                       \
                                            (__REGION_ADDR),                    \
                                            (__ALPHA))
                                            
#define arm_2d_rgb888_alpha_blending( __SRC_ADDR,   /*   source tile address */ \
                                      __DES_ADDR,   /*   target tile address */ \
                                      __REGION_ADDR,/*   region address */      \
                                      __ALPHA)      /*   alpha */               \
            arm_2dp_cccn888_alpha_blending(  NULL,                              \
                                            (__SRC_ADDR),                       \
                                            (__DES_ADDR),                       \
                                            (__REGION_ADDR),                    \
                                            (__ALPHA))

#define arm_2d_cccn888_alpha_blending( __SRC_ADDR,   /*   source tile address */\
                                      __DES_ADDR,   /*   target tile address */ \
                                      __REGION_ADDR,/*   region address */      \
                                      __ALPHA)      /*   alpha */               \
            arm_2dp_cccn888_alpha_blending(  NULL,                              \
                                            (__SRC_ADDR),                       \
                                            (__DES_ADDR),                       \
                                            (__REGION_ADDR),                    \
                                            (__ALPHA))

#define arm_2d_gray8_fill_colour_with_opacity(                                  \
                                      __DES_ADDR,   /*   target tile address */ \
                                      __REGION_ADDR,/*   region address */      \
                                      __COLOUR,     /*   colour */              \
                                      __ALPHA)      /*   alpha */               \
            arm_2dp_gray8_fill_colour_with_opacity(  NULL,                      \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__COLOUR),                    \
                                                 (__ALPHA))

#define arm_2d_rgb565_fill_colour_with_opacity(                                 \
                                      __DES_ADDR,   /*   target tile address */ \
                                      __REGION_ADDR,/*   region address */      \
                                      __COLOUR,     /*   colour */              \
                                      __ALPHA)      /*   alpha */               \
            arm_2dp_rgb565_fill_colour_with_opacity(  NULL,                     \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__COLOUR),                    \
                                                 (__ALPHA))

#define arm_2d_rgb888_fill_colour_with_opacity(                                 \
                                      __DES_ADDR,   /*   target tile address */ \
                                      __REGION_ADDR,/*   region address */      \
                                      __COLOUR,     /*   colour */              \
                                      __ALPHA)      /*   alpha */               \
            arm_2dp_cccn888_fill_colour_with_opacity(  NULL,                    \
                                 (__DES_ADDR),                                  \
                                 (__REGION_ADDR),                               \
                                 (arm_2d_color_cccn888_t){(__COLOUR).tValue},   \
                                 (__ALPHA))


#define arm_2d_cccn888_fill_colour_with_opacity(                                \
                                      __DES_ADDR,   /*   target tile address */ \
                                      __REGION_ADDR,/*   region address */      \
                                      __COLOUR,     /*   colour */              \
                                      __ALPHA)      /*   alpha */               \
            arm_2dp_cccn888_fill_colour_with_opacity(  NULL,                    \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__COLOUR),                    \
                                                 (__ALPHA))

#define arm_2d_gray8_fill_colour_with_mask(                                     \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __ALPHA_ADDR,   /*   alpha tile address */  \
                                    __COLOUR)       /*   colour */              \
            arm_2dp_gray8_fill_colour_with_mask(                                \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR))

#define arm_2d_rgb565_fill_colour_with_mask(                                    \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __ALPHA_ADDR,   /*   alpha tile address */  \
                                    __COLOUR)       /*   colour */              \
            arm_2dp_rgb565_fill_colour_with_mask(                               \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR))

#define arm_2d_rgb888_fill_colour_with_mask(                                    \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __ALPHA_ADDR,   /*   alpha tile address */  \
                                    __COLOUR)       /*   colour */              \
            arm_2dp_cccn888_fill_colour_with_mask(                              \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (arm_2d_color_cccn888_t){(__COLOUR).tValue})

#define arm_2d_cccn888_fill_colour_with_mask(                                   \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __ALPHA_ADDR,   /*   alpha tile address */  \
                                    __COLOUR)       /*   colour */              \
            arm_2dp_cccn888_fill_colour_with_mask(                              \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR))

#define arm_2d_gray8_fill_colour_with_mask_and_opacity(                         \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __ALPHA_ADDR,   /*   alpha tile address */  \
                                    __COLOUR,       /*   colour */              \
                                    __OPACITY)                                  \
            arm_2dp_gray8_fill_colour_with_mask_and_opacity(                    \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR),                                \
                                     (__OPACITY))

#define arm_2d_rgb565_fill_colour_with_mask_and_opacity(                        \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __ALPHA_ADDR,   /*   alpha tile address */  \
                                    __COLOUR,       /*   colour */              \
                                    __OPACITY)                                  \
            arm_2dp_rgb565_fill_colour_with_mask_and_opacity(                   \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR),                                \
                                     (__OPACITY))

#define arm_2d_rgb888_fill_colour_with_mask_and_opacity(                        \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __ALPHA_ADDR,   /*   alpha tile address */  \
                                    __COLOUR,       /*   colour */              \
                                    __OPACITY)                                  \
            arm_2dp_cccn888_fill_colour_with_mask_and_opacity(                  \
                                  NULL,                                         \
                                 (__TARGET_ADDR),                               \
                                 (__REGION_ADDR),                               \
                                 (__ALPHA_ADDR),                                \
                                 (arm_2d_color_cccn888_t){(__COLOUR).tValue},   \
                                 (__OPACITY))

#define arm_2d_cccn888_fill_colour_with_mask_and_opacity(                       \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __ALPHA_ADDR,   /*   alpha tile address */  \
                                    __COLOUR,       /*   colour */              \
                                    __OPACITY)                                  \
            arm_2dp_cccn888_fill_colour_with_mask_and_opacity(                  \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR),                                \
                                     (__OPACITY))

#define arm_2d_gray8_alpha_blending_with_colour_keying(                         \
                                      __SRC_ADDR,   /*   source tile address */ \
                                      __DES_ADDR,   /*   target tile address */ \
                                      __REGION_ADDR,/*   region address */      \
                                      __ALPHA,     /*   colour */               \
                                      __COLOUR)      /*   alpha */              \
            arm_2dp_gray8_alpha_blending_with_colour_keying(                    \
                                                    NULL,                       \
                                                 (__SRC_ADDR),                  \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__ALPHA),                     \
                                                 (__COLOUR))

#define arm_2d_rgb565_alpha_blending_with_colour_keying(                        \
                                      __SRC_ADDR,   /*   source tile address */ \
                                      __DES_ADDR,   /*   target tile address */ \
                                      __REGION_ADDR,/*   region address */      \
                                      __ALPHA,      /*   colour */              \
                                      __COLOUR)     /*   alpha */               \
            arm_2dp_rgb565_alpha_blending_with_colour_keying(                   \
                                                    NULL,                       \
                                                 (__SRC_ADDR),                  \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__ALPHA),                     \
                                                 (__COLOUR))

#define arm_2d_rgb888_alpha_blending_with_colour_keying(                        \
                                      __SRC_ADDR,   /*   source tile address */ \
                                      __DES_ADDR,   /*   target tile address */ \
                                      __REGION_ADDR,/*   region address */      \
                                      __ALPHA,      /*   colour */              \
                                      __COLOUR)     /*   alpha */               \
            arm_2dp_cccn888_alpha_blending_with_colour_keying(                  \
                                        NULL,                                   \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__REGION_ADDR),                            \
                                    (__ALPHA),                                  \
                                    (arm_2d_color_cccn888_t){(__COLOUR).tValue})

#define arm_2d_cccn888_alpha_blending_with_colour_keying(                       \
                                      __SRC_ADDR,   /*   source tile address */ \
                                      __DES_ADDR,   /*   target tile address */ \
                                      __REGION_ADDR,/*   region address */      \
                                      __ALPHA,      /*   colour */              \
                                      __COLOUR)     /*   alpha */               \
            arm_2dp_cccn888_alpha_blending_with_colour_keying(                  \
                                                    NULL,                       \
                                                 (__SRC_ADDR),                  \
                                                 (__DES_ADDR),                  \
                                                 (__REGION_ADDR),               \
                                                 (__ALPHA),                     \
                                                 (__COLOUR))

#define arm_2d_gray8_tile_copy_with_masks(                                      \
                                    __SRC_ADDR,     /*   source tile address */ \
                                    __SRC_MSK_ADDR, /*   source mask address */ \
                                    __DES_ADDR,     /*   target tile address */ \
                                    __DES_MSK_ADDR, /*   target mask address */ \
                                    __REGION,       /*   region address */      \
                                    __MODE)         /*   copy mode */           \
            arm_2dp_gray8_tile_copy_with_masks(                                 \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))

#define arm_2d_rgb565_tile_copy_with_masks(                                     \
                                    __SRC_ADDR,     /*   source tile address */ \
                                    __SRC_MSK_ADDR, /*   source mask address */ \
                                    __DES_ADDR,     /*   target tile address */ \
                                    __DES_MSK_ADDR, /*   target mask address */ \
                                    __REGION,       /*   region address */      \
                                    __MODE)         /*   copy mode */           \
            arm_2dp_rgb565_tile_copy_with_masks(                                \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))

#define arm_2d_rgb888_tile_copy_with_masks                                      \
            arm_2d_cccn888_tile_copy_with_masks

#define arm_2d_cccn888_tile_copy_with_masks(                                    \
                                    __SRC_ADDR,     /*   source tile address */ \
                                    __SRC_MSK_ADDR, /*   source mask address */ \
                                    __DES_ADDR,     /*   target tile address */ \
                                    __DES_MSK_ADDR, /*   target mask address */ \
                                    __REGION,       /*   region address */      \
                                    __MODE)         /*   copy mode */           \
            arm_2dp_cccn888_tile_copy_with_masks(                               \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))

#define arm_2d_gray8_tile_copy_with_src_mask(                                   \
                                    __SRC_ADDR,     /*   source tile address */ \
                                    __SRC_MSK_ADDR, /*   source mask address */ \
                                    __DES_ADDR,     /*   target tile address */ \
                                    __REGION,       /*   region address */      \
                                    __MODE)         /*   copy mode */           \
            arm_2dp_gray8_tile_copy_with_src_mask(                              \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__REGION),                             \
                                        (__MODE))


#define arm_2d_rgb565_tile_copy_with_src_mask(                                  \
                                    __SRC_ADDR,     /*   source tile address */ \
                                    __SRC_MSK_ADDR, /*   source mask address */ \
                                    __DES_ADDR,     /*   target tile address */ \
                                    __REGION,       /*   region address */      \
                                    __MODE)         /*   copy mode */           \
            arm_2dp_rgb565_tile_copy_with_src_mask(                             \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__REGION),                             \
                                        (__MODE))


#define arm_2d_rgb888_tile_copy_with_src_mask                                   \
            arm_2d_cccn888_tile_copy_with_src_mask

#define arm_2d_cccn888_tile_copy_with_src_mask(                                 \
                                    __SRC_ADDR,     /*   source tile address */ \
                                    __SRC_MSK_ADDR, /*   source mask address */ \
                                    __DES_ADDR,     /*   target tile address */ \
                                    __REGION,       /*   region address */      \
                                    __MODE)         /*   copy mode */           \
            arm_2dp_cccn888_tile_copy_with_src_mask(                            \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__REGION),                             \
                                        (__MODE))

#define arm_2d_gray8_tile_copy_with_des_mask(                                   \
                                    __SRC_ADDR,     /*   source tile address */ \
                                    __DES_ADDR,     /*   target tile address */ \
                                    __DES_MSK_ADDR, /*   target mask address */ \
                                    __REGION,       /*   region address */      \
                                    __MODE)         /*   copy mode */           \
            arm_2dp_gray8_tile_copy_with_des_mask(                              \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))


#define arm_2d_rgb565_tile_copy_with_des_mask(                                  \
                                    __SRC_ADDR,     /*   source tile address */ \
                                    __DES_ADDR,     /*   target tile address */ \
                                    __DES_MSK_ADDR, /*   target mask address */ \
                                    __REGION,       /*   region address */      \
                                    __MODE)         /*   copy mode */           \
            arm_2dp_rgb565_tile_copy_with_des_mask(                             \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))

#define arm_2d_rgb888_tile_copy_with_des_mask                                   \
            arm_2d_cccn888_tile_copy_with_des_mask

#define arm_2d_cccn888_tile_copy_with_des_mask(                                 \
                                    __SRC_ADDR,     /*   source tile address */ \
                                    __DES_ADDR,     /*   target tile address */ \
                                    __DES_MSK_ADDR, /*   target mask address */ \
                                    __REGION,       /*   region address */      \
                                    __MODE)         /*   copy mode */           \
            arm_2dp_cccn888_tile_copy_with_des_mask(                            \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__DES_ADDR),                           \
                                        (__DES_MSK_ADDR),                       \
                                        (__REGION),                             \
                                        (__MODE))

/*============================ TYPES =========================================*/

/*!
 * \brief control block for alpha-blending operations
 * \note arm_2d_op_alpha_t inherits from arm_2d_op_src_t explicitly 
 */
typedef struct arm_2d_op_alpha_t {
    inherit(arm_2d_op_core_t);                  //!< base
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile 
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;                                   //!< target
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile 
    }Source;                                    //!< source
    uint32_t wMode;                             //!< copy mode
    uint8_t chRatio;                            //!< opacity
} arm_2d_op_alpha_t;


/*!
 * \brief control block for alpha-blending-with-colour-keying operations
 * \note arm_2d_op_alpha_cl_key_t inherits from arm_2d_op_src_t explicitly 
 */
typedef struct arm_2d_op_alpha_cl_key_t {
    inherit(arm_2d_op_core_t);                  //!< base
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile 
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;                                   //!< target
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile 
    }Source;                                    //!< source
    uint32_t wMode;                             //!< copy mode
    uint8_t chRatio;                            //!< opacity

    union {
        uint8_t  chColour;                      //!< 8bit key colour
        uint16_t hwColour;                      //!< 16bit key colour
        uint32_t wColour;                       //!< 32bit key colour
    };
} arm_2d_op_alpha_cl_key_t;


/*!
 * \brief control block for colour-filling-with-mask operations
 * \note arm_2d_op_fill_cl_msk_t inherits from arm_2d_op_src_t explicitly 
 */
typedef struct arm_2d_op_fill_cl_msk_t {
    inherit(arm_2d_op_core_t);                  //!< base
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;                                   //!< target
    struct {
        const arm_2d_tile_t     *ptTile;        //!< Alpha Mask tile
    } Mask;                                     //!< mask
    uint32_t wMode;                             //!< copy mode

    union {
        uint8_t  chColour;                      //!< 8bit key colour
        uint16_t hwColour;                      //!< 16bit key colour
        uint32_t wColour;                       //!< 32bit key colour
    };
} arm_2d_op_fill_cl_msk_t;


/*!
 * \brief control block for colour-filling-with-mask-and-opacity operations
 * \note arm_2d_op_fill_cl_msk_t inherits from arm_2d_op_src_t explicitly 
 */
typedef struct arm_2d_op_alpha_fill_cl_msk_opc_t {
    inherit(arm_2d_op_core_t);                  //!< core
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;                                   //!< target
    struct {
        const arm_2d_tile_t     *ptTile;        //!< Alpha Mask tile
    } Mask;                                     //!< mask
    uint32_t wMode;                             //!< copy mode
    union {
        uint8_t  chColour;                      //!< 8bit key colour
        uint16_t hwColour;                      //!< 16bit key colour
        uint32_t wColour;                       //!< 32bit key colour
    };
    uint8_t chRatio;                            //!< opacity
} arm_2d_op_alpha_fill_cl_msk_opc_t;

/*!
 * \brief control block for colour-filling-with-opacity operations
 * \note arm_2d_op_fill_cl_t inherits from arm_2d_op_t explicitly 
 */
typedef struct arm_2d_op_fill_cl_opc_t {
    inherit(arm_2d_op_core_t);                  //!< base 
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile 
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;                                   //!< target
    union {
        uint8_t  chColour;                      //!< 8bit key colour
        uint16_t hwColour;                      //!< 16bit key colour
        uint32_t wColour;                       //!< 32bit key colour
    };
    uint8_t chRatio;                             //!< opacity
} arm_2d_op_fill_cl_opc_t;

/*!
 * \brief control block for copy with masks operations
 * 
 */
typedef arm_2d_op_src_msk_t arm_2d_op_cp_msk_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*----------------------------------------------------------------------------*
 * Copy tile to destination with specified transparency ratio (0~255)         *
 *----------------------------------------------------------------------------*/

/*!
 * \brief blend a source tile to a target tile with a specified opacity
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] chRatio the opacity
 * \return arm_fsm_rt_t the operation result
 */
extern 
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_gray8_alpha_blending(  arm_2d_op_alpha_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio);

/*!
 * \brief blend a source tile to a target tile with a specified opacity
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] chRatio the opacity
 * \return arm_fsm_rt_t the operation result
 */
extern 
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb565_alpha_blending( arm_2d_op_alpha_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio);

/*!
 * \brief blend a source tile to a target tile with a specified opacity
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] chRatio the opacity
 * \return arm_fsm_rt_t the operation result
 */
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

/*!
 * \brief fill a target tile with a given gray8 colour and a specified opacity
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] tColour a gray8 colour
 * \param[in] chRatio the opacity
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_opacity( 
                                                arm_2d_op_fill_cl_opc_t *ptOP,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion,
                                                arm_2d_color_gray8_t tColour,
                                                uint_fast8_t chRatio);

/*!
 * \brief fill a target tile with a given rgb565 colour and a specified opacity
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] tColour a rgb565 colour
 * \param[in] chRatio the opacity
 * \return arm_fsm_rt_t the operation result
 */
extern 
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_opacity( 
                                                arm_2d_op_fill_cl_opc_t *ptOP,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion,
                                                arm_2d_color_rgb565_t tColour,
                                                uint_fast8_t chRatio);

/*!
 * \brief fill a target tile with a given cccn888 colour and a specified opacity
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] tColour a cccn888 colour
 * \param[in] chRatio the opacity
 * \return arm_fsm_rt_t the operation result
 */
extern 
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_opacity( 
                                                arm_2d_op_fill_cl_opc_t *ptOP,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion,
                                                arm_2d_color_cccn888_t tColour,
                                                uint_fast8_t chRatio);

/*----------------------------------------------------------------------------*
 * Fill tile with a specified colour and an alpha mask                        *
 *----------------------------------------------------------------------------*/

/*!
 * \brief fill a target tile with a given gray8 colour and a mask on target side
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] ptAlpha the mask on the target side
 * \param[in] tColour a gray8 colour
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_mask(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_gray8_t tColour);

/*!
 * \brief fill a target tile with a given rgb565 colour and a mask on target side
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] ptAlpha the mask on the target side
 * \param[in] tColour a rgb565 colour
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_mask( 
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_rgb565_t tColour);

/*!
 * \brief fill a target tile with a given cccn888 colour and a mask on target side
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] ptAlpha the mask on the target side
 * \param[in] tColour a cccn888 colour
 * \return arm_fsm_rt_t the operation result
 */
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

/*!
 * \brief fill a target tile with a given gray8 colour, a mask on target side and an opacity
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] ptAlpha the mask on the target side
 * \param[in] tColour a gray8 colour
 * \param[in] chOpacity the opacity
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_gray8_t tColour,
                                        uint8_t chOpacity);

/*!
 * \brief fill a target tile with a given rgb565 colour, a mask on target side and an opacity
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] ptAlpha the mask on the target side
 * \param[in] tColour a rgb565 colour
 * \param[in] chOpacity the opacity
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_rgb565_t tColour,
                                        uint8_t chOpacity);

/*!
 * \brief fill a target tile with a given cccn888 colour, a mask on target side and an opacity
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] ptAlpha the mask on the target side
 * \param[in] tColour a cccn888 colour
 * \param[in] chOpacity the opacity
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_cccn888_t tColour,
                                        uint8_t chOpacity);

/*----------------------------------------------------------------------------*
 * Blend tile and background with a specified transparency ratio(0~255) and a *
 * specified transparency color mask                                          *
 *----------------------------------------------------------------------------*/

/*!
 * \brief blend a source tile to a target tile with a opacity and colour keying
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] chRatio the opacity
 * \param[in] tColour the key colour
 * \return arm_fsm_rt_t the operation result
 */
extern 
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_gray8_alpha_blending_with_colour_keying(
                                            arm_2d_op_alpha_cl_key_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio,
                                            arm_2d_color_gray8_t tColour);

/*!
 * \brief blend a source tile to a target tile with a opacity and colour keying
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] chRatio the opacity
 * \param[in] tColour the key colour
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb565_alpha_blending_with_colour_keying(
                                                arm_2d_op_alpha_cl_key_t *ptOP,
                                                const arm_2d_tile_t *ptSource, 
                                                const arm_2d_tile_t *ptTarget, 
                                                const arm_2d_region_t *ptRegion,
                                                uint_fast8_t chRatio,
                                                arm_2d_color_rgb565_t tColour);

/*!
 * \brief blend a source tile to a target tile with a opacity and colour keying
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] chRatio the opacity
 * \param[in] tColour the key colour
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_cccn888_alpha_blending_with_colour_keying(
                                                arm_2d_op_alpha_cl_key_t *ptOP,
                                                const arm_2d_tile_t *ptSource, 
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion,
                                                uint_fast8_t chRatio,
                                                arm_2d_color_cccn888_t tColour);

/*----------------------------------------------------------------------------*
 * Copy tile to destination with both a source mask and a target mask         *
 *----------------------------------------------------------------------------*/

/*!
 * \brief copy a source tile to a target tile with masks in a given mode
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptSrcMask the mask on the source side
 * \param[in] ptTarget the target tile
 * \param[in] ptDesMask the mask on the target side
 * \param[in] ptRegion the target region
 * \param[in] wMode the copy mode
 * \return arm_fsm_rt_t the operation result
 */
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

/*!
 * \brief copy a source tile to a target tile with masks in a given mode
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptSrcMask the mask on the source side
 * \param[in] ptTarget the target tile
 * \param[in] ptDesMask the mask on the target side
 * \param[in] ptRegion the target region
 * \param[in] wMode the copy mode
 * \return arm_fsm_rt_t the operation result
 */
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

/*!
 * \brief copy a source tile to a target tile with masks in a given mode
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptSrcMask the mask on the source side
 * \param[in] ptTarget the target tile
 * \param[in] ptDesMask the mask on the target side
 * \param[in] ptRegion the target region
 * \param[in] wMode the copy mode
 * \return arm_fsm_rt_t the operation result
 */
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
/*!
 * \brief copy a source tile to a target tile with a source mask in a given mode
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptSrcMask the mask on the source side
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] wMode the copy mode
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_gray8_tile_copy_with_src_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode); 

/*!
 * \brief copy a source tile to a target tile with a source mask in a given mode
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptSrcMask the mask on the source side
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] wMode the copy mode
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_rgb565_tile_copy_with_src_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode);

/*!
 * \brief copy a source tile to a target tile with a source mask in a given mode
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptSrcMask the mask on the source side
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] wMode the copy mode
 * \return arm_fsm_rt_t the operation result
 */
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

/*!
 * \brief copy a source tile to a target tile with a target mask in a given mode
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptDesMask the mask on the target side
 * \param[in] ptRegion the target region
 * \param[in] wMode the copy mode
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_gray8_tile_copy_with_des_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode); 

/*!
 * \brief copy a source tile to a target tile with a target mask in a given mode
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptDesMask the mask on the target side
 * \param[in] ptRegion the target region
 * \param[in] wMode the copy mode
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_rgb565_tile_copy_with_des_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode);

/*!
 * \brief copy a source tile to a target tile with a target mask in a given mode
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptDesMask the mask on the target side
 * \param[in] ptRegion the target region
 * \param[in] wMode the copy mode
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_cccn888_tile_copy_with_des_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode); 

/*! @} */

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
