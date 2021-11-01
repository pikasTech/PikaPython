/*
 * Copyright (C) 2010-2020 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        arm-2d.c
 * Description:  APIs for various alpha related operations
 *
 * $Date:        03. Oct 2021
 * $Revision:    V.0.9.1
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/
#define __ARM_2D_IMPL__


#include "arm_2d.h"
#include "__arm_2d_impl.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wextra-semi-stmt"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#   pragma clang diagnostic ignored "-Wdouble-promotion"
#   pragma clang diagnostic ignored "-Wunused-parameter"
#   pragma clang diagnostic ignored "-Wimplicit-float-conversion"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   pragma clang diagnostic ignored "-Wtautological-pointer-compare"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wsign-compare"

#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/




/*----------------------------------------------------------------------------*
 * Accelerable Low Level APIs                                                 *
 *----------------------------------------------------------------------------*/
 
 
/*! adding support with c code template */

#define __API_COLOUR        gray8
#define __API_INT_TYPE      uint8_t
#define __API_PIXEL_BLENDING            __ARM_2D_PIXEL_BLENDING_GRAY8
            
#include "__arm_2d_alpha_blending.inc"


#define __API_COLOUR        rgb565
#define __API_INT_TYPE      uint16_t
#define __API_PIXEL_BLENDING            __ARM_2D_PIXEL_BLENDING_RGB565
#define __PATCH_ALPHA_BLENDING

#include "__arm_2d_alpha_blending.inc"


/*! adding support with c code template */
#define __API_COLOUR        cccn888
#define __API_INT_TYPE      uint32_t
#define __API_PIXEL_BLENDING            __ARM_2D_PIXEL_BLENDING_CCCN888

#include "__arm_2d_alpha_blending.inc"


#define __API_CAFWM_COLOUR                      gray8
#define __API_CAFWM_INT_TYPE                    uint8_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            8
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_GRAY8

#include "__arm_2d_alpha_mask.inc"

#define __API_CAFWM_CFG_1_HORIZONTAL_LINE       1
#define __API_CAFWM_COLOUR                      gray8
#define __API_CAFWM_INT_TYPE                    uint8_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            8
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_GRAY8

#include "__arm_2d_alpha_mask.inc"

#define __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT   1
#define __API_CAFWM_COLOUR                      gray8
#define __API_CAFWM_INT_TYPE                    uint8_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            8
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_GRAY8

#include "__arm_2d_alpha_mask.inc"



#define __API_CAFWM_COLOUR                      rgb565
#define __API_CAFWM_INT_TYPE                    uint16_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            16
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_RGB565

#include "__arm_2d_alpha_mask.inc"

#define __API_CAFWM_CFG_1_HORIZONTAL_LINE       1
#define __API_CAFWM_COLOUR                      rgb565
#define __API_CAFWM_INT_TYPE                    uint16_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            16
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_RGB565

#include "__arm_2d_alpha_mask.inc"

#define __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT   1
#define __API_CAFWM_COLOUR                      rgb565
#define __API_CAFWM_INT_TYPE                    uint16_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            16
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_RGB565

#include "__arm_2d_alpha_mask.inc"


#define __API_CAFWM_COLOUR                      cccn888
#define __API_CAFWM_INT_TYPE                    uint32_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            32
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_CCCN888
    
#include "__arm_2d_alpha_mask.inc"  
    
#define __API_CAFWM_CFG_1_HORIZONTAL_LINE       1
#define __API_CAFWM_COLOUR                      cccn888
#define __API_CAFWM_INT_TYPE                    uint32_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            32
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_CCCN888

#include "__arm_2d_alpha_mask.inc"

#define __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT   1
#define __API_CAFWM_COLOUR                      cccn888
#define __API_CAFWM_INT_TYPE                    uint32_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            32
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_CCCN888

#include "__arm_2d_alpha_mask.inc"


#define __API_ABW_COLOUR                        gray8
#define __API_ABW_INT_TYPE                      uint8_t
#define __API_ABW_INT_TYPE_BIT_NUM              8
#define __API_ABW_COLOUR_SZ                     ARM_2D_COLOUR_SZ_8BIT

#include "__arm_2d_alpha_blending_wrapper.inc"


#define __API_ABW_COLOUR                        rgb565
#define __API_ABW_INT_TYPE                      uint16_t
#define __API_ABW_INT_TYPE_BIT_NUM              16
#define __API_ABW_COLOUR_SZ                     ARM_2D_COLOUR_SZ_16BIT

#include "__arm_2d_alpha_blending_wrapper.inc"


#define __API_ABW_COLOUR                        cccn888
#define __API_ABW_INT_TYPE                      uint32_t
#define __API_ABW_INT_TYPE_BIT_NUM              32
#define __API_ABW_COLOUR_SZ                     ARM_2D_COLOUR_SZ_32BIT

#include "__arm_2d_alpha_blending_wrapper.inc"

extern
void __arm_2d_impl_rgb565_alpha_blending_direct(
                                        const uint16_t *phwSource,
                                        const uint16_t *phwBackground,
                                        uint16_t *phwDestination,
                                        uint32_t wPixelCount,
                                        uint_fast8_t chRatio);

extern
void __arm_2d_impl_cccn888_alpha_blending_direct(
                                        const uint32_t *__RESTRICT pwSource,
                                        const uint32_t *__RESTRICT pwBackground,
                                        uint32_t *pwDestination,
                                        uint32_t wPixelCount,
                                        uint_fast8_t chRatio);


/*----------------------------------------------------------------------------*
 * Copy tile to destination with specified masks                              *
 *----------------------------------------------------------------------------*/
 
static 
arm_2d_err_t  __arm_2dp_tile_copy_with_mask_validate(
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSrcMask,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_tile_t *ptDesMask,
                                            uint32_t wMode)
{
    if (NULL != ptSrcMask) {
        //! valid source mask tile
        if (0 == ptSrcMask->bHasEnforcedColour) {
            return ARM_2D_ERR_INVALID_PARAM;
        } else if ( (ARM_2D_COLOUR_SZ_8BIT != ptSrcMask->tColourInfo.u3ColourSZ)
               &&   (ARM_2D_CHANNEL_8in32 != ptSrcMask->tColourInfo.chScheme)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        
        arm_2d_cmp_t tCompare = arm_2d_tile_shape_compare(ptSrcMask, ptSource);
        
        /*! \note the source mask tile should be bigger than or equals to the  
         *!       source tile
         */
        if (ARM_2D_CMP_SMALLER == tCompare) {
        
            return ARM_2D_ERR_INVALID_PARAM;
        }
    }
    
    if (NULL != ptDesMask) {
        //! valid target mask tile
        if (0 == ptDesMask->bHasEnforcedColour) {
            return ARM_2D_ERR_INVALID_PARAM;
        } else if ( (ARM_2D_COLOUR_SZ_8BIT != ptDesMask->tColourInfo.u3ColourSZ)
               &&   (ARM_2D_CHANNEL_8in32 != ptDesMask->tColourInfo.chScheme)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        
        /*! \note the target mask tile should be bigger than or equals to the  
         *!       target tile in width
         */
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_width_compare(ptDesMask, ptTarget)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_height_compare(ptDesMask, ptTarget)) {
            if (1 != ptDesMask->tRegion.tSize.iHeight) {
                return ARM_2D_ERR_INVALID_PARAM;
            } else if (ARM_2D_CHANNEL_8in32 == ptDesMask->tColourInfo.chScheme) {
                /*! does NOT support a target mask consists of 1 horizontal line
                 *! in the special colour 'ARM_2D_CHANNEL_8in32'.
                 */
                return ARM_2D_ERR_INVALID_PARAM;
            }
        }
        
    }
    
    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(2,3,4,5)
arm_fsm_rt_t arm_2dp_gray8_tile_copy_with_masks(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptSrcMask);
    assert(NULL != ptTarget);
    assert(NULL != ptDesMask);

    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_2dp_tile_copy_with_mask_validate(
                                            ptSource, ptSrcMask,
                                            ptTarget, ptDesMask, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_MASK_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = ptSrcMask;
    this.Mask.ptTargetSide = ptDesMask;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2,3,4,5)
arm_fsm_rt_t arm_2dp_rgb565_tile_copy_with_masks(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptSrcMask);
    assert(NULL != ptTarget);
    assert(NULL != ptDesMask);

    
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_2dp_tile_copy_with_mask_validate(
                                            ptSource, ptSrcMask,
                                            ptTarget, ptDesMask, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_MASK_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = ptSrcMask;
    this.Mask.ptTargetSide = ptDesMask;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
    

}

ARM_NONNULL(2,3,4,5)
arm_fsm_rt_t arm_2dp_cccn888_tile_copy_with_masks(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptSrcMask);
    assert(NULL != ptTarget);
    assert(NULL != ptDesMask);
    
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_2dp_tile_copy_with_mask_validate(
                                            ptSource, ptSrcMask,
                                            ptTarget, ptDesMask, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_MASK_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = ptSrcMask;
    this.Mask.ptTargetSide = ptDesMask;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/*----------------------------------------------------------------------------*
 * Copy tile to destination with a specified source mask                      *
 *----------------------------------------------------------------------------*/


ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_gray8_tile_copy_with_src_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptSrcMask);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_2dp_tile_copy_with_mask_validate(
                                            ptSource, ptSrcMask,
                                            ptTarget, NULL, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = ptSrcMask;
    this.Mask.ptTargetSide = NULL;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_rgb565_tile_copy_with_src_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptSrcMask);
    assert(NULL != ptTarget);

    
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_2dp_tile_copy_with_mask_validate(
                                            ptSource, ptSrcMask,
                                            ptTarget, NULL, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = ptSrcMask;
    this.Mask.ptTargetSide = NULL;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
    

}

ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_cccn888_tile_copy_with_src_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptSrcMask);
    assert(NULL != ptTarget);
    
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_2dp_tile_copy_with_mask_validate(
                                            ptSource, ptSrcMask,
                                            ptTarget, NULL, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = ptSrcMask;
    this.Mask.ptTargetSide = NULL;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/*----------------------------------------------------------------------------*
 * Copy tile to destination with a specified target mask                      *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_gray8_tile_copy_with_des_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);
    assert(NULL != ptDesMask);

    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_2dp_tile_copy_with_mask_validate(
                                            ptSource, NULL,
                                            ptTarget, ptDesMask, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_DES_MASK_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = NULL;
    this.Mask.ptTargetSide = ptDesMask;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_rgb565_tile_copy_with_des_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);
    assert(NULL != ptDesMask);

    
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_2dp_tile_copy_with_mask_validate(
                                            ptSource, NULL,
                                            ptTarget, ptDesMask, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_DES_MASK_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = NULL;
    this.Mask.ptTargetSide = ptDesMask;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
    

}

ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_cccn888_tile_copy_with_des_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);
    assert(NULL != ptDesMask);
    
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_2dp_tile_copy_with_mask_validate(
                                            ptSource, NULL,
                                            ptTarget, ptDesMask, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_DES_MASK_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = NULL;
    this.Mask.ptTargetSide = ptDesMask;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

/*----------------------------------------------------------------------------*
 * Copy tile to destination with specified transparency ratio (0~255)         *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_gray8_alpha_blending(
                                     arm_2d_op_alpha_t *ptOP,
                                     const arm_2d_tile_t *ptSource,
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     uint_fast8_t chRatio)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_alpha_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_ALPHA_BLENDING_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = 0;
    this.chRatio = chRatio;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb565_alpha_blending(
                                     arm_2d_op_alpha_t *ptOP,
                                     const arm_2d_tile_t *ptSource,
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     uint_fast8_t chRatio)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_alpha_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_ALPHA_BLENDING_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = 0;
    this.chRatio = chRatio;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_cccn888_alpha_blending( arm_2d_op_alpha_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_alpha_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_ALPHA_BLENDING_RGB888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = 0;
    this.chRatio = chRatio;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

arm_fsm_rt_t __arm_2d_gray8_sw_alpha_blending(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);
    

#if 0 // todo: might implement this in the future
    if (   (ptTask->Param.tCopy.tSource.iStride
        ==  ptTask->Param.tCopy.tTarget.iStride)
        && (ptTask->Param.tCopy.tSource.iStride
        ==  ptTask->Param.tCopy.tCopySize.iWidth)) {

        //! direct blending
        __arm_2d_impl_gray8_alpha_blending_direct(
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tCopySize.iHeight
                                        * ptTask->Param.tCopy.tCopySize.iWidth,
                                    this.chRatio);
    } else 
#endif
    {
        __arm_2d_impl_gray8_alpha_blending(
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &(ptTask->Param.tCopy.tCopySize),
                                    this.chRatio);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_alpha_blending(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);
    

    if (   (ptTask->Param.tCopy.tSource.iStride
        ==  ptTask->Param.tCopy.tTarget.iStride)
        && (ptTask->Param.tCopy.tSource.iStride
        ==  ptTask->Param.tCopy.tCopySize.iWidth)) {

        //! direct blending
        __arm_2d_impl_rgb565_alpha_blending_direct(
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tCopySize.iHeight
                                        * ptTask->Param.tCopy.tCopySize.iWidth,
                                    this.chRatio);
    } else {
        __arm_2d_impl_rgb565_alpha_blending(
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &(ptTask->Param.tCopy.tCopySize),
                                    this.chRatio);
    }

    return arm_fsm_rt_cpl;
}


arm_fsm_rt_t __arm_2d_cccn888_sw_alpha_blending(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (   (ptTask->Param.tCopy.tSource.iStride
        ==  ptTask->Param.tCopy.tTarget.iStride)
        && (ptTask->Param.tCopy.tSource.iStride
        ==  ptTask->Param.tCopy.tCopySize.iWidth)) {

        //! direct blending
        __arm_2d_impl_cccn888_alpha_blending_direct(
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tCopySize.iHeight
                                        * ptTask->Param.tCopy.tCopySize.iWidth,
                                    this.chRatio);
    } else {
        __arm_2d_impl_cccn888_alpha_blending(
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &(ptTask->Param.tCopy.tCopySize),
                                    this.chRatio);
    }

    return arm_fsm_rt_cpl;
}

/*----------------------------------------------------------------------------*
 * Fill a specified region with a given colour and transparency ratio (0~255) *
 *----------------------------------------------------------------------------*/
 
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_opacity( 
                                                arm_2d_op_fill_cl_opc_t *ptOP,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion,
                                                uint8_t chColour,
                                                uint_fast8_t chRatio)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_fill_cl_opc_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_ALPHA_COLOUR_FILL_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.chColour = chColour;
    this.chRatio = chRatio;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
} 

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_opacity( 
                                                arm_2d_op_fill_cl_opc_t *ptOP,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion,
                                                arm_2d_color_rgb565_t tColour,
                                                uint_fast8_t chRatio)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_fill_cl_opc_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_ALPHA_COLOUR_FILL_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.hwColour = tColour.tValue;
    this.chRatio = chRatio;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}
 
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_opacity( 
                                                arm_2d_op_fill_cl_opc_t *ptOP,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion,
                                                arm_2d_color_rgb888_t tColour,
                                                uint_fast8_t chRatio)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_fill_cl_opc_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_ALPHA_COLOUR_FILL_RGB888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.wColour = tColour.tValue;
    this.chRatio = chRatio;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_opacity(
                                        __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_opc_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);
    
    __arm_2d_impl_gray8_colour_filling_with_opacity(  
                    ptTask->Param.tTileProcess.pBuffer,
                    ptTask->Param.tTileProcess.iStride,
                    &(ptTask->Param.tTileProcess.tValidRegion.tSize),
                    this.chColour,
                    this.chRatio);

    return arm_fsm_rt_cpl;
}
 
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_opacity(
                                        __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_opc_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);
    
    __arm_2d_impl_rgb565_colour_filling_with_opacity(  
                    ptTask->Param.tTileProcess.pBuffer,
                    ptTask->Param.tTileProcess.iStride,
                    &(ptTask->Param.tTileProcess.tValidRegion.tSize),
                    this.hwColour,
                    this.chRatio);

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_opacity(
                                        __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_opc_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    
    __arm_2d_impl_cccn888_colour_filling_with_opacity(  
                    ptTask->Param.tTileProcess.pBuffer,
                    ptTask->Param.tTileProcess.iStride,
                    &(ptTask->Param.tTileProcess.tValidRegion.tSize),
                    this.wColour,
                    this.chRatio);

    return arm_fsm_rt_cpl;
}


/*----------------------------------------------------------------------------*
 * Fill tile with a specified colour and an alpha mask                        *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_mask(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        uint8_t chColour)
{
    assert(NULL != ptTarget);

    if (NULL == ptAlpha) {
        return arm_2dp_c8bit_fill_colour(   (arm_2d_op_fill_cl_t *)ptOP,
                                            ptTarget,
                                            ptRegion,
                                            chColour);
    } else{
        ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptOP);

        //! valid alpha mask tile
        if (0 == ptAlpha->bHasEnforcedColour) {
            return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
        } else if ( (ARM_2D_COLOUR_SZ_8BIT != ptAlpha->tColourInfo.u3ColourSZ)
               &&   (ARM_2D_CHANNEL_8in32 != ptAlpha->tColourInfo.chScheme)) {
            return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
        }

        if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
            return arm_fsm_rt_on_going;
        }

        //memset(ptThis, 0, sizeof(*ptThis));

        OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_ALPHA_MASK_GRAY8;

        this.Target.ptTile = ptTarget;
        this.Target.ptRegion = ptRegion;
        this.Mask.ptTile = ptAlpha;
        this.wMode = 0;
        this.chColour = chColour;

        return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
    }
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_mask(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_rgb565_t tColour)
{
    assert(NULL != ptTarget);
    if (NULL == ptAlpha) {
        return arm_2dp_rgb16_fill_colour(   (arm_2d_op_fill_cl_t *)ptOP,
                                            ptTarget,
                                            ptRegion,
                                            tColour.tValue);
    } else{
        ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptOP);

        //! valid alpha mask tile
        if (0 == ptAlpha->bHasEnforcedColour) {
            return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
        } else if ( (ARM_2D_COLOUR_SZ_8BIT != ptAlpha->tColourInfo.u3ColourSZ)
               &&   (ARM_2D_CHANNEL_8in32 != ptAlpha->tColourInfo.chScheme)) {
            return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
        }

        if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
            return arm_fsm_rt_on_going;
        }

        //memset(ptThis, 0, sizeof(*ptThis));

        OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_ALPHA_MASK_RGB565;

        this.Target.ptTile = ptTarget;
        this.Target.ptRegion = ptRegion;
        this.Mask.ptTile = ptAlpha;
        this.wMode = 0;
        this.hwColour = tColour.tValue;

        return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
    }
}

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_mask(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_cccn888_t tColour)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);

    if (NULL == ptAlpha) {
        return arm_2dp_rgb32_fill_colour(   (arm_2d_op_fill_cl_t *)ptOP,
                                            ptTarget,
                                            ptRegion,
                                            tColour.tValue);
    } else{
    
        ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptOP);

        //! valid alpha mask tile
        if (0 == ptAlpha->bHasEnforcedColour) {
            return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
        } else if ( (ARM_2D_COLOUR_SZ_8BIT != ptAlpha->tColourInfo.u3ColourSZ)
               &&   (ARM_2D_CHANNEL_8in32 != ptAlpha->tColourInfo.chScheme)) {
            return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
        }

        if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
            return arm_fsm_rt_on_going;
        }

        //memset(ptThis, 0, sizeof(*ptThis));

        OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_ALPHA_MASK_CCCN888;

        this.Target.ptTile = ptTarget;
        this.Target.ptRegion = ptRegion;
        this.Mask.ptTile = ptAlpha;
        this.wMode = 0;
        this.wColour = tColour.tValue;

        return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
    }
}


arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_gray8_colour_filling_channel_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.chColour);
    #endif
    
    } else {
        __arm_2d_impl_gray8_colour_filling_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.chColour);
    }
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_rgb565_colour_filling_channel_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour);
    #endif
    } else {
        __arm_2d_impl_rgb565_colour_filling_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour);
    }
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_cccn888_colour_filling_channel_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour);
    #endif
    } else {
        __arm_2d_impl_cccn888_colour_filling_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour);
    }

    return arm_fsm_rt_cpl;
}



/*----------------------------------------------------------------------------*
 * Fill tile with a specified colour, an alpha mask and a specified opacity   *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        uint8_t chColour,
                                        uint8_t chOpacity)
{
    assert(NULL != ptTarget);
    if (NULL == ptAlpha) {
        return arm_2dp_gray8_fill_colour_with_opacity(
                                            (arm_2d_op_fill_cl_opc_t *)ptOP,
                                            ptTarget,
                                            ptRegion,
                                            chColour,
                                            chOpacity);
    } else{
    
        ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptOP);

        //! valid alpha mask tile
        if (0 == ptAlpha->bHasEnforcedColour) {
            return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
        } else if ( (ARM_2D_COLOUR_SZ_8BIT != ptAlpha->tColourInfo.u3ColourSZ)
               &&   (ARM_2D_CHANNEL_8in32 != ptAlpha->tColourInfo.chScheme)) {
            return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
        }

        if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
            return arm_fsm_rt_on_going;
        }

        //memset(ptThis, 0, sizeof(*ptThis));

        OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_ALPHA_MASK_AND_OPACITY_GRAY8;

        this.Target.ptTile = ptTarget;
        this.Target.ptRegion = ptRegion;
        this.Mask.ptTile = ptAlpha;
        this.wMode = 0;
        this.chColour = chColour;
        this.chOpacity = chOpacity;

        return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
    }
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_rgb565_t tColour,
                                        uint8_t chOpacity)
{
    assert(NULL != ptTarget);
    if (NULL == ptAlpha) {
        return arm_2dp_rgb565_fill_colour_with_opacity(
                                            (arm_2d_op_fill_cl_opc_t *)ptOP,
                                            ptTarget,
                                            ptRegion,
                                            tColour,
                                            chOpacity);
    } else{
    
        ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptOP);

        //! valid alpha mask tile
        if (0 == ptAlpha->bHasEnforcedColour) {
            return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
        } else if ( (ARM_2D_COLOUR_SZ_8BIT != ptAlpha->tColourInfo.u3ColourSZ)
               &&   (ARM_2D_CHANNEL_8in32 != ptAlpha->tColourInfo.chScheme)) {
            return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
        }

        if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
            return arm_fsm_rt_on_going;
        }

        //memset(ptThis, 0, sizeof(*ptThis));

        OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_ALPHA_MASK_AND_OPACITY_RGB565;

        this.Target.ptTile = ptTarget;
        this.Target.ptRegion = ptRegion;
        this.Mask.ptTile = ptAlpha;
        this.wMode = 0;
        this.hwColour = tColour.tValue;
        this.chOpacity = chOpacity;

        return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
    }
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_rgb888_t tColour,
                                        uint8_t chOpacity)
{
    assert(NULL != ptTarget);

    if (NULL == ptAlpha) {
        return arm_2dp_cccn888_fill_colour_with_opacity(
                                            (arm_2d_op_fill_cl_opc_t *)ptOP,
                                            ptTarget,
                                            ptRegion,
                                            tColour,
                                            chOpacity);
    } else{
        ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptOP);

        //! valid alpha mask tile
        if (0 == ptAlpha->bHasEnforcedColour) {
            return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
        } else if ( (ARM_2D_COLOUR_SZ_8BIT != ptAlpha->tColourInfo.u3ColourSZ)
               &&   (ARM_2D_CHANNEL_8in32 != ptAlpha->tColourInfo.chScheme)) {
            return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
        }

        if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
            return arm_fsm_rt_on_going;
        }

        //memset(ptThis, 0, sizeof(*ptThis));

        OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_ALPHA_MASK_AND_OPACITY_CCCN888;

        this.Target.ptTile = ptTarget;
        this.Target.ptRegion = ptRegion;
        this.Mask.ptTile = ptAlpha;
        this.wMode = 0;
        this.wColour = tColour.tValue;
        this.chOpacity = chOpacity;

        return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
    }
}


arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_gray8_colour_filling_channel_mask_opacity(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.chColour,
                        this.chOpacity);
    #endif
    } else {
        __arm_2d_impl_gray8_colour_filling_mask_opacity(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.chColour,
                        this.chOpacity);
    }
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_rgb565_colour_filling_channel_mask_opacity(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour,
                        this.chOpacity);
    #endif
    } else {
        __arm_2d_impl_rgb565_colour_filling_mask_opacity(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour,
                        this.chOpacity);
    }
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_cccn888_colour_filling_channel_mask_opacity(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour,
                        this.chOpacity);
    #endif
    } else {
        __arm_2d_impl_cccn888_colour_filling_mask_opacity(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour,
                        this.chOpacity);
    }

    return arm_fsm_rt_cpl;
}




/*----------------------------------------------------------------------------*
 * Blend tile and background with a specified transparency ratio(0~255) and a *
 * specified transparency color mask                                          *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_gray8_alpha_blending_with_colour_keying(
                                            arm_2d_op_alpha_cl_key_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio,
                                            uint8_t chColour)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_alpha_cl_key_t);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_ALPHA_BLENDING_WITH_COLOUR_KEYING_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = 0;
    this.chRatio = chRatio;
    this.chColour = chColour;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb565_alpha_blending_with_colour_keying(
                                            arm_2d_op_alpha_cl_key_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio,
                                            arm_2d_color_rgb565_t tColour)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_alpha_cl_key_t);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_ALPHA_BLENDING_WITH_COLOUR_KEYING_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = 0;
    this.chRatio = chRatio;
    this.hwColour = tColour.tValue;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_cccn888_alpha_blending_with_colour_keying(
                                            arm_2d_op_alpha_cl_key_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio,
                                            arm_2d_color_rgb888_t tColour)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_alpha_cl_key_t);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = & ARM_2D_OP_ALPHA_BLENDING_WITH_COLOUR_KEYING_RGB888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = 0;
    this.chRatio = chRatio;
    this.wColour = tColour.tValue;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

arm_fsm_rt_t __arm_2d_gray8_sw_alpha_blending_with_colour_keying(
                                        __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_cl_key_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);

    __arm_2d_impl_gray8_alpha_blending_colour_keying(
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &ptTask->Param.tCopy.tCopySize,
                                    this.chRatio,
                                    this.chColour);



    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_alpha_blending_with_colour_keying(
                                        __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_cl_key_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);


    __arm_2d_impl_rgb565_alpha_blending_colour_keying(
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &ptTask->Param.tCopy.tCopySize,
                                    this.chRatio,
                                    this.hwColour);



    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_alpha_blending_with_colour_keying(
                                        __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_cl_key_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    __arm_2d_impl_cccn888_alpha_blending_colour_keying(
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &ptTask->Param.tCopy.tCopySize,
                                    this.chRatio,
                                    this.wColour);

    return arm_fsm_rt_cpl;
}

/*----------------------------------------------------------------------------*
 * Accelerable Low Level APIs                                                 *
 *----------------------------------------------------------------------------*/


__WEAK
void __arm_2d_impl_rgb565_alpha_blending(   uint16_t *__RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast8_t chRatio)
{
    uint32_t iHeight = ptCopySize->iHeight;
    uint32_t iWidth  = ptCopySize->iWidth;
    uint16_t        ratioCompl = 256 - chRatio;
    
    ARM_2D_UNUSED(ratioCompl);
    
    for (uint32_t y = 0; y < iHeight; y++) {

#if (defined (__ARM_ARCH_8_1M_MAIN__) && (__ARM_ARCH_8_1M_MAIN__ == 1))
        /* M55 NOMVE optimization */
        register unsigned loopCnt  __asm("lr");
        loopCnt = iWidth;

        __asm volatile(
            "   dls             lr, lr                                  \n"
            ".p2align 2                                                 \n"
            "1:                                                         \n"
            /* read source / target pixel  */
            "   ldrh            r4, [%[src]], #2                        \n"
            "   ldrh            r5, [%[pTarget]]                        \n"

            /* unpack R */
            "   and             r0, r4, #31                             \n"
            "   and             r1, r5, #31                             \n"

            /* unpack B */
            "   lsr             r2, r4, #11                             \n"
            "   lsr             r3, r5, #11                             \n"

            /* unpack G */
            "   ubfx            r4, r4, #5, #6                          \n"
            "   ubfx            r5, r5, #5, #6                          \n"

            /* mix */
            "   mul             r0, r0, %[ratio0]                       \n"
            "   mul             r4, r4, %[ratio0]                       \n"
            "   mul             r2, r2, %[ratio0]                       \n"

            "   mla             r0, %[ratio1], r1, r0                   \n"
            "   mla             r1, %[ratio1], r5, r4                   \n"

            /* pack R */
            "   ubfx            r0, r0, #8, #5                          \n"
            /* shift & mask G */
            "   and             r1, %[cst], r1, lsr #3                  \n"
            /* MLA moved here to fill stall */
            "   mla             r5, %[ratio1], r3, r2                   \n"

            /* pack G */
            "   add             r0, r0, r1                              \n"
            /* pack R */
            "   bic             r1, r5, #255                            \n"
            "   orr             r0, r0, r1, lsl #3                      \n"

            "   strh            r0, [%[pTarget]], #2                    \n"
            "   le              lr, 1b                                  \n"

            : [src] "+r" (phwSourceBase), [pTarget] "+r" (phwTargetBase),
              [cnt] "+r" (loopCnt)
            : [ratio1] "r" ((256 - (uint_fast16_t)chRatio) ),
              [ratio0] "r" (chRatio), [cst] "r" (0x7e0 /* mask G */)
            : "r0", "r1", "r2", "r3",
              "r4", "r5", "memory"
        );
#else
        
        for (uint32_t x = 0; x < iWidth; x++) {
            __arm_2d_color_fast_rgb_t srcPix, targetPix;

            __arm_2d_rgb565_unpack(*phwSourceBase++, &srcPix);
            __arm_2d_rgb565_unpack(*phwTargetBase, &targetPix);

            for (int i = 0; i < 3; i++) {
                uint16_t        tmp =
                    (uint16_t) (srcPix.RGB[i] * chRatio) +
                    (targetPix.RGB[i] * ratioCompl);
                targetPix.RGB[i] = (uint16_t) (tmp >> 8);
            }
            /* pack merged stream */
            *phwTargetBase++ = __arm_2d_rgb565_pack(&targetPix);
        }
#endif
        phwSourceBase += (iSourceStride - iWidth);
        phwTargetBase += (iTargetStride - iWidth);
    }
}


__WEAK
void __arm_2d_impl_rgb565_alpha_blending_direct(const uint16_t *phwSource,
                                                const uint16_t *phwBackground,
                                                uint16_t *phwDestination,
                                                uint32_t wPixelCount,
                                                uint_fast8_t chRatio)
{
    uint16_t        ratioCompl = 256 - chRatio;
    
    ARM_2D_UNUSED(ratioCompl);
    
#if (defined (__ARM_ARCH_8_1M_MAIN__) && (__ARM_ARCH_8_1M_MAIN__ == 1))
        /* M55 NOMVE optimization */
        register unsigned loopCnt  __asm("lr");
        loopCnt = wPixelCount;

        __asm volatile(
            "   dls             lr, lr                                  \n"
            ".p2align 2                                                 \n"
            "1:                                                         \n"
            /* read source / target pixel  */
            "   ldrh            r4, [%[src]], #2                        \n"
            "   ldrh            r5, [%[backgrd]], #2                    \n"

            /* unpack R */
            "   and             r0, r4, #31                             \n"
            "   and             r1, r5, #31                             \n"

            /* unpack B */
            "   lsr             r2, r4, #11                             \n"
            "   lsr             r3, r5, #11                             \n"

            /* unpack G */
            "   ubfx            r4, r4, #5, #6                          \n"
            "   ubfx            r5, r5, #5, #6                          \n"

            /* mix */
            "   mul             r0, r0, %[ratio0]                       \n"
            "   mul             r4, r4, %[ratio0]                       \n"
            "   mul             r2, r2, %[ratio0]                       \n"

            "   mla             r0, %[ratio1], r1, r0                   \n"
            "   mla             r1, %[ratio1], r5, r4                   \n"

            /* pack R */
            "   ubfx            r0, r0, #8, #5                          \n"
            /* shift & mask G */
            "   and             r1, %[cst], r1, lsr #3                  \n"
            /* MLA moved here to fill stall */
            "   mla             r5, %[ratio1], r3, r2                   \n"

            /* pack G */
            "   add             r0, r0, r1                              \n"
            /* pack R */
            "   bic             r1, r5, #255                            \n"
            "   orr             r0, r0, r1, lsl #3                      \n"

            "   strh            r0, [%[pTarget]], #2                    \n"
            "   le              lr, 1b                                  \n"

            : [src] "+r" (phwSource), [backgrd] "+r" (phwBackground),
              [pTarget] "+r" (phwDestination), [cnt] "+r" (loopCnt)
            : [ratio1] "r" ((256 - (uint_fast16_t)chRatio) ),
              [ratio0] "r" (chRatio), [cst] "r" (0x7e0 /* mask G */)
            : "r0", "r1", "r2", "r3",
              "r4", "r5", "memory"
        );
#else
    do {
        __arm_2d_color_fast_rgb_t wSourcePixel, wBackgroundPixel, wTargetPixel;
        

        __arm_2d_rgb565_unpack(*phwSource++, &wSourcePixel);
        __arm_2d_rgb565_unpack(*phwBackground++, &wBackgroundPixel);

        for (int i = 0; i < 3; i++) {
            uint16_t        tmp =
                (uint16_t) (wSourcePixel.RGB[i] * chRatio) +
                (wBackgroundPixel.RGB[i] * ratioCompl);
            wTargetPixel.RGB[i] = (uint16_t) (tmp >> 8);
        }

        *phwDestination++ = __arm_2d_rgb565_pack(&wTargetPixel);
    } while (--wPixelCount);

#endif
}


__WEAK
void __arm_2d_impl_cccn888_alpha_blending_direct(const uint32_t *pwSource,
                                                const uint32_t *pwBackground,
                                                uint32_t *pwDestination,
                                                uint32_t wPixelCount,
                                                uint_fast8_t chRatio)
{
    do {
        uint_fast8_t n = sizeof(uint32_t) - 1;
        const uint8_t *pchSrc = (uint8_t *)(pwSource++);
        const uint8_t *pchBG = (uint8_t *)(pwBackground++);
        uint8_t *pchDes = (uint8_t *)(pwDestination++);

        do {
            *pchDes++ = (   ((uint_fast16_t)(*pchSrc++) * chRatio)
                        +   (   (uint_fast16_t)(*pchBG++)
                            *   (256 - (uint_fast16_t)chRatio))) >> 8;
        } while(--n);

    } while(--wPixelCount);
}


/*----------------------------------------------------------------------------*
 * Low Level IO Interfaces                                                    *
 *----------------------------------------------------------------------------*/


__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_ALPHA_MASK_GRAY8, 
                __arm_2d_gray8_sw_colour_filling_with_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_ALPHA_MASK_RGB565, 
                __arm_2d_rgb565_sw_colour_filling_with_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_ALPHA_MASK_RGB888, 
                __arm_2d_cccn888_sw_colour_filling_with_mask);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_ALPHA_MASK_AND_OPACITY_GRAY8, 
                __arm_2d_gray8_sw_colour_filling_with_mask_and_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_ALPHA_MASK_AND_OPACITY_RGB565, 
                __arm_2d_rgb565_sw_colour_filling_with_mask_and_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_ALPHA_MASK_AND_OPACITY_CCCN888, 
                __arm_2d_cccn888_sw_colour_filling_with_mask_and_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_GRAY8,
                __arm_2d_gray8_sw_alpha_blending);
__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_RGB565, 
                __arm_2d_rgb565_sw_alpha_blending);
__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_RGB888, 
                __arm_2d_cccn888_sw_alpha_blending);

__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_masks);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_masks);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_masks);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_masks);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_masks);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_masks);

__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_src_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_src_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_src_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_src_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_src_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_src_mask);


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_des_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_des_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_des_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_des_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_des_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_des_mask);


__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_WITH_COLOUR_KEYING_GRAY8, 
                __arm_2d_gray8_sw_alpha_blending_with_colour_keying);
__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_WITH_COLOUR_KEYING_RGB565, 
                __arm_2d_rgb565_sw_alpha_blending_with_colour_keying);
__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_WITH_COLOUR_KEYING_RGB888, 
                __arm_2d_cccn888_sw_alpha_blending_with_colour_keying);

__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_FILL_COLOUR_GRAY8, 
                __arm_2d_gray8_sw_colour_filling_with_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_FILL_COLOUR_RGB565, 
                __arm_2d_rgb565_sw_colour_filling_with_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_FILL_COLOUR_RGB888, 
                __arm_2d_cccn888_sw_colour_filling_with_opacity);


const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_ALPHA_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_ALPHA_MASK_GRAY8),
            .ptFillLike = NULL, 
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_ALPHA_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_ALPHA_MASK_RGB565),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_ALPHA_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_ALPHA_MASK_RGB888),
            .ptFillLike = NULL, 
        },
    },
};  



const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_ALPHA_MASK_AND_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_ALPHA_MASK_AND_OPACITY_GRAY8),
            .ptFillLike = NULL, 
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_ALPHA_MASK_AND_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_ALPHA_MASK_AND_OPACITY_RGB565),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_ALPHA_MASK_AND_OPACITY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_ALPHA_MASK_AND_OPACITY_CCCN888),
            .ptFillLike = NULL, 
        },
    },
};  

const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ALPHA_BLENDING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_GRAY8),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ALPHA_BLENDING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_RGB565),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_RGB888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ALPHA_BLENDING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_RGB888),
            .ptFillLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_WITH_COLOUR_KEYING_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ALPHA_BLENDING_WITH_COLOUR_KEYING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_WITH_COLOUR_KEYING_GRAY8),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_WITH_COLOUR_KEYING_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ALPHA_BLENDING_WITH_COLOUR_KEYING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_WITH_COLOUR_KEYING_RGB565),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_WITH_COLOUR_KEYING_RGB888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ALPHA_BLENDING_WITH_COLOUR_KEYING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_WITH_COLOUR_KEYING_RGB888),
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_COLOUR_FILL_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ALPHA_FILL_COLOUR,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_ALPHA_FILL_COLOUR_GRAY8),
        },
    },
};    

const __arm_2d_op_info_t ARM_2D_OP_ALPHA_COLOUR_FILL_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ALPHA_FILL_COLOUR,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_ALPHA_FILL_COLOUR_RGB565),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_COLOUR_FILL_RGB888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB888,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ALPHA_FILL_COLOUR,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_ALPHA_FILL_COLOUR_RGB888),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_CCCN888),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SRC_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SRC_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SRC_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_CCCN888),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_DES_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_DES_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_DES_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_CCCN888),
        },
    },
};




#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_warning 174,177,188,68,513,144
#endif

#ifdef   __cplusplus
}
#endif
