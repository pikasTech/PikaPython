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
 * $Date:        01. December 2020
 * $Revision:    V.0.5.0
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
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-declarations"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wc11-extensions"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
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


/*! \note arm_2d_op_alpha_cl_msk_t inherits from arm_2d_op_src_t explicitly 
 */
typedef struct arm_2d_op_alpha_cl_msk_t {
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
        uint16_t hwColour;
        uint32_t wColour;
    };
} arm_2d_op_alpha_cl_msk_t;

/*! \note arm_2d_op_fill_cl_t inherits from arm_2d_op_t explicitly 
 */
typedef struct arm_2d_op_alpha_fill_cl_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile 
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    union {
        uint16_t hwColour;
        uint32_t wColour;
    };
    uint_fast8_t chRatio;                       //!< transparency ratio 
} arm_2d_op_alpha_fill_cl_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*----------------------------------------------------------------------------*
 * Copy tile to destination with specified transparency ratio (0~255)         *
 *----------------------------------------------------------------------------*/

extern 
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_rgb565_alpha_blending( 
                                     const arm_2d_tile_t *ptSource,
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     uint_fast8_t chRatio);

extern 
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_rgb888_alpha_blending( 
                                     const arm_2d_tile_t *ptSource,
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     uint_fast8_t chRatio);
                                     

/*----------------------------------------------------------------------------*
 * Fill a specified region with a given colour and transparency ratio (0~255) *
 *----------------------------------------------------------------------------*/
extern 
arm_fsm_rt_t arm_2d_rgb565_fill_colour_with_alpha( 
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     arm_2d_color_rgb565_t tColour,
                                     uint_fast8_t chRatio);

extern 
arm_fsm_rt_t arm_2d_rgb888_fill_colour_with_alpha( 
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     arm_2d_color_rgb888_t tColour,
                                     uint_fast8_t chRatio);

/*----------------------------------------------------------------------------*
 * Blend tile and background with a specified transparency ratio(0~255) and a *
 * specified transparency color mask                                          *
 *----------------------------------------------------------------------------*/
extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_rbg565_alpha_blending_with_colour_masking(
                                            const arm_2d_tile_t *ptSource, 
                                            const arm_2d_tile_t *ptTarget, 
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio,
                                            arm_2d_color_rgb565_t tColour);
                                            
extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_rbg888_alpha_blending_with_colour_masking(
                                            const arm_2d_tile_t *ptSource, 
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio,
                                            arm_2d_color_rgb888_t tColour);



#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
