/*
 * Copyright (C) 2010-2021 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        arm-2d_draw.c
 * Description:  APIs for basic drawing 
 *
 * $Date:        29. April 2021
 * $Revision:    V.0.8.0
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
#   pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
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
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wswitch-enum"
#   pragma clang diagnostic ignored "-Wswitch"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define TO_Q16(x)           ((int32_t)(x) << 16)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

void __arm_2d_impl_rgb16_colour_filling(uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint16_t hwColour);

void __arm_2d_impl_rgb32_colour_filling(uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint32_t hwColour);
                                        
void __arm_2d_impl_rgb16_draw_pattern(  uint8_t *__RESTRICT pchSource,
                                        int32_t  iOffset,
                                        int16_t iSourceStride,
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint8_t chMode,
                                        uint16_t hwForeColour,
                                        uint16_t hwBackColour);
                                        
void __arm_2d_impl_rgb32_draw_pattern(  uint8_t *__RESTRICT pchSource,
                                        int32_t  iOffset,
                                        int16_t iSourceStride,
                                        uint32_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint8_t chMode,
                                        uint32_t wForeColour,
                                        uint32_t wBackColour);
                                        
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/



/*----------------------------------------------------------------------------*
 * Utiliteis                                                                  *
 *----------------------------------------------------------------------------*/
__arm_2d_point_adj_alpha_t 
__arm_2d_point_get_adjacent_alpha_fp(arm_2d_point_float_t *ptPoint)
{
    assert(NULL != ptPoint);
    float x = ptPoint->fX - (int32_t)ptPoint->fX;
    float y = ptPoint->fY - (int32_t)ptPoint->fY;
    
    int16_t tXOffset = x < 0 ? -1 : 0;
    int16_t tYOffset = y < 0 ? -1 : 0;
    
    __arm_2d_point_adj_alpha_t tResult = {
        .tMatrix = {
            [0] = {
                .tOffset = {
                    .iX = tXOffset,
                    .iY = tYOffset,
                },
            },
            [1] = {
                .tOffset = {
                    .iX = tXOffset + 1,
                    .iY = tYOffset,
                },
            },
            [2] = {
                .tOffset = {
                    .iX = tXOffset,
                    .iY = tYOffset + 1,
                },
            },
            [3] = {
                .tOffset = {
                    .iX = tXOffset + 1,
                    .iY = tYOffset +1,
                },
            },
        },
    };
    
    if (tXOffset) {
        if (tYOffset) {
            tResult.tMatrix[0].chAlpha = (uint8_t)((float)-x * (float)-y * 256.0f);
            tResult.tMatrix[1].chAlpha = (uint8_t)((float)(1.0f+x) * (float)-y * 256.0f);
            tResult.tMatrix[2].chAlpha = (uint8_t)((float)-x * (float)(1.0f+y) * 256.0f);
            tResult.tMatrix[3].chAlpha = (uint8_t)((float)(1.0f+x) * (float)(1.0f+y) * 256.0f);
        } else {
            tResult.tMatrix[0].chAlpha = (uint8_t)((float)-x * (float)(1.0f-y) * 256.0f);
            tResult.tMatrix[1].chAlpha = (uint8_t)((float)(1.0f+x) * (float)(1.0f-y) * 256.0f);
            tResult.tMatrix[2].chAlpha = (uint8_t)((float)-x * (float)y * 256.0f);
            tResult.tMatrix[3].chAlpha = (uint8_t)((float)(1.0f+x) * (float)y * 256.0f);
        }
    } else {
        if (tYOffset) {
            tResult.tMatrix[0].chAlpha = (uint8_t)((float)(1.0f-x) * (float)-y * 256.0f);
            tResult.tMatrix[1].chAlpha = (uint8_t)((float)x * (float)-y * 256.0f);
            tResult.tMatrix[2].chAlpha = (uint8_t)((float)(1.0f-x) * (float)(1.0f+y) * 256.0f);
            tResult.tMatrix[3].chAlpha = (uint8_t)((float)x * (float)(1.0f+y) * 256.0f);
        } else {
            tResult.tMatrix[0].chAlpha = (uint8_t)((float)(1.0f-x) * (float)(1.0f-y) * 256.0f);
            tResult.tMatrix[1].chAlpha = (uint8_t)((float)x * (float)(1.0f-y) * 256.0f);
            tResult.tMatrix[2].chAlpha = (uint8_t)((float)(1.0f-x) * (float)y * 256.0f);
            tResult.tMatrix[3].chAlpha = (uint8_t)((float)x * (float)y * 256.0f);
        }
    }
    
    return tResult;
}

__arm_2d_point_adj_alpha_t 
__arm_2d_point_get_adjacent_alpha_q16(arm_2d_point_fx_t *ptPoint)
{
    assert(NULL != ptPoint);
    int32_t x = ptPoint->X  & 0xFFFF;  //__QSUB(ptPoint->X, ptPoint->X & 0x7FFF0000);
    int32_t y = ptPoint->Y  & 0xFFFF;//__QSUB(ptPoint->Y, ptPoint->Y & 0x7FFF0000);
    
    if (ptPoint->X < 0) {
        x |= 0xFFFF0000;
    }
    if (ptPoint->Y < 0) {
        y |= 0xFFFF0000;
    }
    
    int16_t tXOffset = x < 0 ? -1 : 0;
    int16_t tYOffset = y < 0 ? -1 : 0;
    
    __arm_2d_point_adj_alpha_t tResult = {
        .tMatrix = {
            [0] = {
                .tOffset = {
                    .iX = tXOffset,
                    .iY = tYOffset,
                },
            },
            [1] = {
                .tOffset = {
                    .iX = tXOffset + 1,
                    .iY = tYOffset,
                },
            },
            [2] = {
                .tOffset = {
                    .iX = tXOffset,
                    .iY = tYOffset + 1,
                },
            },
            [3] = {
                .tOffset = {
                    .iX = tXOffset + 1,
                    .iY = tYOffset +1,
                },
            },
        },
    };


    if (tXOffset) {
        if (tYOffset) {
            tResult.tMatrix[0].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16(-x, -y),TO_Q16(256))) >> 16;
            tResult.tMatrix[1].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16((TO_Q16(1)+x),-y),TO_Q16(256)) >> 16);
            tResult.tMatrix[2].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16(-x,(TO_Q16(1)+y)), TO_Q16(256)) >> 16);
            tResult.tMatrix[3].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16((TO_Q16(1)+x), (TO_Q16(1)+y)), TO_Q16(256)) >> 16);
        } else {
            tResult.tMatrix[0].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16(-x,TO_Q16(256)),(TO_Q16(1)-y)) >> 16);
            tResult.tMatrix[1].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16((TO_Q16(1)+x),TO_Q16(256)),(TO_Q16(1)-y)) >> 16);
            tResult.tMatrix[2].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16(-x, TO_Q16(256)), y) >> 16);
            tResult.tMatrix[3].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16((TO_Q16(1)+x),TO_Q16(256)), y) >> 16);
        }
    } else {
        if (tYOffset) {
            tResult.tMatrix[0].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16((TO_Q16(1)-x), -y), TO_Q16(256)) >> 16);
            tResult.tMatrix[1].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16(x, -y), TO_Q16(256)) >> 16);
            tResult.tMatrix[2].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16((TO_Q16(1)-x), (TO_Q16(1)+y)), TO_Q16(256)) >> 16);
            tResult.tMatrix[3].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16(x, (1+y)), TO_Q16(256)) >> 16);
        } else {
            tResult.tMatrix[0].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16((TO_Q16(1)-x), (TO_Q16(1)-y)), TO_Q16(256)) >> 16);
            tResult.tMatrix[1].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16(x, (TO_Q16(1)-y)), TO_Q16(256)) >> 16);
            tResult.tMatrix[2].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16((TO_Q16(1)-x), y), TO_Q16(256)) >> 16);
            tResult.tMatrix[3].chAlpha = (uint8_t)( MUL_Q16(MUL_Q16(x, y), TO_Q16(256)) >> 16);
        }
    }
    
    return tResult;
}


/*----------------------------------------------------------------------------*
 * Draw a point with specified colour                                         *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_rgb16_draw_point(   const arm_2d_tile_t *ptTarget,
                                        const arm_2d_location_t tLocation,
                                        uint_fast16_t hwColour)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_drw_pt_t);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));
    
    arm_2d_region_t tPointRegion = {
        .tLocation = tLocation,
        .tSize = {1,1},
    };

    OP_CORE.ptOp = &ARM_2D_OP_DRAW_POINT_RGB16;
    OP_CORE.Preference.u2ACCMethods = ARM_2D_PREF_ACC_SW_ONLY;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = &tPointRegion;
    this.hwColour = hwColour;

    return __arm_2d_op_invoke(NULL);
}

                                       
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_rgb32_draw_point(   const arm_2d_tile_t *ptTarget,
                                        const arm_2d_location_t tLocation,
                                        uint32_t wColour)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_drw_pt_t);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));
    
    arm_2d_region_t tPointRegion = {
        .tLocation = tLocation,
        .tSize = {1,1},
    };

    OP_CORE.ptOp = &ARM_2D_OP_DRAW_POINT_RGB32;
    OP_CORE.Preference.u2ACCMethods = ARM_2D_PREF_ACC_SW_ONLY;
    
    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = &tPointRegion;
    this.wColour = wColour;

    return __arm_2d_op_invoke(NULL);
}

arm_fsm_rt_t __arm_2d_sw_draw_point(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_drw_pt_t, ptTask->ptOP)

    switch (OP_CORE.ptOp->Info.Colour.u3ColourSZ) {
        case ARM_2D_COLOUR_SZ_16BIT:
            (*(uint16_t *)ptTask->Param.tTileProcess.pBuffer) = this.hwColour;
            break;
        case ARM_2D_COLOUR_SZ_32BIT:
            (*(uint32_t *)ptTask->Param.tTileProcess.pBuffer) = this.wColour;
            break;
        default:
            return (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
    }

    return arm_fsm_rt_cpl;
}


/*----------------------------------------------------------------------------*
 * Fill tile with specified colour                                            *
 *----------------------------------------------------------------------------*/
extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_rgb16_fill_colour(  const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint_fast16_t hwColour)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_fill_cl_t);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));
    
    OP_CORE.ptOp = &ARM_2D_OP_COLOUR_FILL_RGB16;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.hwColour = hwColour;

    return __arm_2d_op_invoke(NULL);
}
                                       
extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_rgb32_fill_colour(  const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wColour)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_fill_cl_t);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));
    
    OP_CORE.ptOp = &ARM_2D_OP_COLOUR_FILL_RGB32;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.wColour = wColour;

    return __arm_2d_op_invoke(NULL);
}



arm_fsm_rt_t __arm_2d_rgb16_sw_colour_filling(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    
    __arm_2d_impl_rgb16_colour_filling( 
                    ptTask->Param.tTileProcess.pBuffer,
                    ptTask->Param.tTileProcess.iStride,
                    &(ptTask->Param.tTileProcess.tValidRegion.tSize),
                    this.hwColour);

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb32_sw_colour_filling(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
   
    __arm_2d_impl_rgb32_colour_filling( 
                    ptTask->Param.tTileProcess.pBuffer,
                    ptTask->Param.tTileProcess.iStride,
                    &(ptTask->Param.tTileProcess.tValidRegion.tSize),
                    this.wColour);

    return arm_fsm_rt_cpl;
}

/*----------------------------------------------------------------------------*
 * Draw a bit patterns                                                        *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(1,2)
arm_fsm_rt_t arm_2d_rgb16_draw_pattern(  const arm_2d_tile_t *ptPattern,
                                         const arm_2d_tile_t *ptTarget,
                                         const arm_2d_region_t *ptRegion,
                                         uint32_t wMode,
                                         uint16_t hwForeColour,
                                         uint16_t hwBackColour)
{
    assert(NULL != ptPattern);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_drw_patn_t);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));
    
    if (    (wMode &   (ARM_2D_DRW_PATN_MODE_NO_FG_COLOR    | 
                        ARM_2D_DRW_PATH_MODE_COMP_FG_COLOUR |
                        ARM_2D_DRW_PATN_MODE_WITH_BG_COLOR  ))
        == (ARM_2D_DRW_PATN_MODE_NO_FG_COLOR)) {
            
        //! nothing todo
        return arm_fsm_rt_cpl;
    }
    
    
    OP_CORE.ptOp = &ARM_2D_OP_DRAW_PATTERN_RGB16;
    
    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptPattern;
    this.wMode = wMode;
    this.Foreground.hwColour = hwForeColour;
    this.Background.hwColour = hwBackColour;

    return __arm_2d_op_invoke(NULL);
}


ARM_NONNULL(1,2)
arm_fsm_rt_t arm_2d_rgb32_draw_pattern( const arm_2d_tile_t *ptPattern,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode,
                                        uint32_t wForeColour,
                                        uint32_t wBackColour)
{

    assert(NULL != ptPattern);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_drw_patn_t);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));
    
    if (    (wMode &   (ARM_2D_DRW_PATN_MODE_NO_FG_COLOR    | 
                        ARM_2D_DRW_PATH_MODE_COMP_FG_COLOUR |
                        ARM_2D_DRW_PATN_MODE_WITH_BG_COLOR  ))
        == (ARM_2D_DRW_PATN_MODE_NO_FG_COLOR)) {
            
        //! nothing todo
        return arm_fsm_rt_cpl;
    }
    
    OP_CORE.ptOp = &ARM_2D_OP_DRAW_PATTERN_RGB32;
    
    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptPattern;
    this.wMode = wMode;
    this.Foreground.wColour = wForeColour;
    this.Background.wColour = wBackColour;

    return __arm_2d_op_invoke(NULL);
}

arm_fsm_rt_t __arm_2d_rgb16_sw_draw_pattern( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_drw_patn_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    uint32_t wMode = this.wMode;
    
#if 0
    if (!this.Source.ptTile->bHasEnforcedColour) {
        return (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
    } 
    
    if (ARM_2D_COLOUR_SZ_1BIT != this.Source.ptTile->tColourInfo.u3ColourSZ) {
        return (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
    }
#endif

    if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
    #if 0
        //! todo: add support for mirror
        __arm_2d_impl_rgb16_draw_pattern_with_mirror(
                                    ptTask->Param.tCopy.pSource ,
                                    ptTask->Param.tCopy.iSourceStride,
                                    ptTask->Param.tCopy.pTarget,
                                    ptTask->Param.tCopy.iTargetStride,
                                    &ptTask->Param.tCopy.tCopySize,
                                    wMode);
        break;

    #endif
    } else {
        //! draw bit-pattern
        __arm_2d_impl_rgb16_draw_pattern(
                                ptTask->Param.tCopy.tSource.pBuffer,
                                ptTask->Param.tCopy.tSource.nOffset,
                                ptTask->Param.tCopy.tSource.iStride,
                                ptTask->Param.tCopy.tTarget.pBuffer,
                                ptTask->Param.tCopy.tTarget.iStride,
                                &ptTask->Param.tCopy.tCopySize,
                                wMode,
                                this.Foreground.hwColour,
                                this.Background.hwColour);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb32_sw_draw_pattern( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_drw_patn_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    uint32_t wMode = this.wMode;
    
#if 0
    if (!this.Source.ptTile->bHasEnforcedColour) {
        return (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
    } 
    
    if (ARM_2D_COLOUR_SZ_1BIT != this.Source.ptTile->tColourInfo.u3ColourSZ) {
        return (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
    }
#endif

    if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
    #if 0
        //! todo: add support for mirror
        __arm_2d_impl_rgb32_draw_pattern_with_mirror(
                                    ptTask->Param.tCopy.pSource ,
                                    ptTask->Param.tCopy.iSourceStride,
                                    ptTask->Param.tCopy.pTarget,
                                    ptTask->Param.tCopy.iTargetStride,
                                    &ptTask->Param.tCopy.tCopySize,
                                    wMode);
    #endif
    } else {
        __arm_2d_impl_rgb32_draw_pattern(
                                ptTask->Param.tCopy.tSource.pBuffer,
                                ptTask->Param.tCopy.tSource.nOffset,
                                ptTask->Param.tCopy.tSource.iStride,
                                ptTask->Param.tCopy.tTarget.pBuffer,
                                ptTask->Param.tCopy.tTarget.iStride,
                                &ptTask->Param.tCopy.tCopySize,
                                wMode,
                                this.Foreground.wColour,
                                this.Background.wColour);
    }

    return arm_fsm_rt_cpl;
}

/*----------------------------------------------------------------------------*
 * Accelerable Low Level APIs                                                 *
 *----------------------------------------------------------------------------*/

/*! adding support with c code template */
#define __API_COLOUR        rgb16
#define __API_INT_TYPE      uint16_t
#include "__arm_2d_fill_colour.inc"


#define __API_COLOUR        rgb32
#define __API_INT_TYPE      uint32_t
#include "__arm_2d_fill_colour.inc"


/*! adding support with c code template */
#define __API_COLOUR        rgb16
#define __API_INT_TYPE      uint16_t
#include "__arm_2d_draw_pattern.inc"


#define __API_COLOUR        rgb32
#define __API_INT_TYPE      uint32_t
#include "__arm_2d_draw_pattern.inc"



#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_warning 174,177,188,68,513,144
#endif

#ifdef   __cplusplus
}
#endif
