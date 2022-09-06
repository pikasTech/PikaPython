/*
 * Copyright (C) 2010-2022 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        __arm-2d_core.c
 * Description:  Basic Tile operations
 *
 * $Date:        29. Aug 2022
 * $Revision:    V.1.3.1
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */


/*============================ INCLUDES ======================================*/

#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wempty-translation-unit"
#endif

#ifdef __ARM_2D_COMPILATION_UNIT
#undef __ARM_2D_COMPILATION_UNIT

#define __ARM_2D_IMPL__

#include "arm_2d.h"
#include "__arm_2d_impl.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
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
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#   pragma clang diagnostic ignored "-Wswitch-enum"
#   pragma clang diagnostic ignored "-Wswitch"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513
#elif defined(__IS_COMPILER_IAR__)
#   pragma diag_suppress=Pa089
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic ignored "-Wenum-compare"
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

__WEAK
arm_2d_runtime_feature_t ARM_2D_RUNTIME_FEATURE = {
    .TREAT_OUT_OF_RANGE_AS_COMPLETE         = 1,
    .HAS_DEDICATED_THREAD_FOR_2D_TASK       = 0,
};


const arm_2d_version_t ARM_2D_VERSION = {
    .Major = ARM_2D_VERSION_MAJOR,
    .Minor = ARM_2D_VERSION_MINOR,
    .Patch = ARM_2D_VERSION_PATCH,
};

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NOINIT
struct __arm_2d_op_control ARM_2D_CTRL;

#define __arm_2d_get_address_and_region_from_tile(  __TILE_PTR,                 \
                                                    __VALID_REGION_NAME,        \
                                                    __LOCATION_OFFSET_PTR,      \
                                                    __TYPE,                     \
                                                    __BUF_PTR_NAME)             \
    arm_2d_region_t __VALID_REGION_NAME;                                        \
    assert(NULL != (__TILE_PTR));                                               \
    __TYPE *(__BUF_PTR_NAME) = NULL;                                            \
                                                                                \
    (__TILE_PTR) = arm_2d_tile_get_root((__TILE_PTR),                           \
                                        &__VALID_REGION_NAME,                   \
                                         __LOCATION_OFFSET_PTR);                \
                                                                                \
    if (NULL != (__TILE_PTR)) {                                                 \
        (__BUF_PTR_NAME) = ((__TYPE *)((__TILE_PTR)->pchBuffer))                \
                        + (__TILE_PTR)->tRegion.tSize.iWidth *                  \
                                __VALID_REGION_NAME.tLocation.iY                \
                        + __VALID_REGION_NAME.tLocation.iX;                     \
    }



/*----------------------------------------------------------------------------*
 * Virtual Resource                                                           *
 *----------------------------------------------------------------------------*/
static arm_2d_err_t __load_virtual_resource(const arm_2d_tile_t *ptRootTile,
                                            __arm_2d_tile_param_t *ptParam)
{
    if (NULL == ptRootTile || NULL == ptParam) {
        return ARM_2D_ERR_NONE;
    }

    if (!ptRootTile->tInfo.bIsRoot) {
        return ARM_2D_ERR_NONE;
    }
    
    if (!ptRootTile->tInfo.bVirtualResource) {
        return ARM_2D_ERR_NONE;
    }
    
    /* a virtual resource must be marked as root tile */
    arm_2d_vres_t *ptRes = (arm_2d_vres_t *)ptRootTile;
    
    if (NULL == ptRes->Load) {
        return ARM_2D_ERR_MISSING_PARAM;
    }

    /* load virtual resource */
    intptr_t nAddress = (ptRes->Load)(  ptRes->pTarget,
                                        ptRes,
                                        &ptParam->tValidRegion);
    
    if ((intptr_t)NULL == nAddress) {
        return ARM_2D_ERR_IO_ERROR;
    }
    
    ptRes->tTile.nAddress = nAddress;
    /* update param */
    ptParam->pBuffer = (void *)nAddress;
    ptParam->nOffset = 0;
    ptParam->iStride = ptParam->tValidRegion.tSize.iWidth;
    ptParam->tValidRegion.tLocation = (arm_2d_location_t){0,0};

    return ARM_2D_ERR_NONE;
}

static void __depose_virtual_resource(const arm_2d_tile_t *ptSourceTile)
{
    if (NULL == ptSourceTile) {
        return;
    }
    arm_2d_region_t tValidRegion;
    ptSourceTile = __arm_2d_tile_get_root(  ptSourceTile, 
                                            &tValidRegion, 
                                            NULL, 
                                            NULL);

    if (NULL == ptSourceTile) {
        return ;
    }
    
    if (!ptSourceTile->tInfo.bVirtualResource) {
        return ;
    }

    /* a virtual resource must be marked as root tile */
    arm_2d_vres_t *ptRes = (arm_2d_vres_t *)ptSourceTile;

    intptr_t nAddress = ptRes->tTile.nAddress;
    ptRes->tTile.nAddress = (intptr_t)NULL;

    
    
    if (NULL == ptRes->Depose) {
        return ;
    }

    /* depose virtual resource */
    (ptRes->Depose)(ptRes->pTarget, ptRes, nAddress);
}

/*----------------------------------------------------------------------------*
 * Invoking Low level operations                                              *
 *----------------------------------------------------------------------------*/

void __arm_2d_sub_task_depose(arm_2d_op_core_t *ptOP)
{
    assert(NULL != ptOP);
    switch(ptOP->ptOp->Info.Param.chValue & ARM_2D_OP_INFO_PARAM_TILES_MASK) {
        case    ARM_2D_OP_INFO_PARAM_HAS_TARGET
            |   ARM_2D_OP_INFO_PARAM_HAS_TARGET_MASK: {
                arm_2d_op_msk_t *ptThis = (arm_2d_op_msk_t *)ptOP;
                __depose_virtual_resource(this.Mask.ptTile);
            }
            break;
        
        case    ARM_2D_OP_INFO_PARAM_HAS_TARGET
            |   ARM_2D_OP_INFO_PARAM_HAS_SOURCE: {
                arm_2d_op_src_t *ptThis = (arm_2d_op_src_t *)ptOP;
                __depose_virtual_resource(this.Source.ptTile);
            }
            break;
        
        case    ARM_2D_OP_INFO_PARAM_HAS_TARGET
            |   ARM_2D_OP_INFO_PARAM_HAS_SOURCE
            |   ARM_2D_OP_INFO_PARAM_HAS_SOURCE_MASK:
        case    ARM_2D_OP_INFO_PARAM_HAS_TARGET
            |   ARM_2D_OP_INFO_PARAM_HAS_SOURCE
            |   ARM_2D_OP_INFO_PARAM_HAS_TARGET_MASK:
        case    ARM_2D_OP_INFO_PARAM_HAS_TARGET
            |   ARM_2D_OP_INFO_PARAM_HAS_SOURCE
            |   ARM_2D_OP_INFO_PARAM_HAS_TARGET_MASK
            |   ARM_2D_OP_INFO_PARAM_HAS_SOURCE_MASK: {
                arm_2d_op_src_msk_t *ptThis = (arm_2d_op_src_msk_t *)ptOP;
                __depose_virtual_resource(this.Source.ptTile);
                __depose_virtual_resource(this.Mask.ptSourceSide);
                __depose_virtual_resource(this.Mask.ptTargetSide);
            }
            break;

        case    ARM_2D_OP_INFO_PARAM_HAS_TARGET
            |   ARM_2D_OP_INFO_PARAM_HAS_SOURCE
            |   ARM_2D_OP_INFO_PARAM_HAS_ORIGIN:{
                arm_2d_op_src_orig_t *ptThis = (arm_2d_op_src_orig_t *)ptOP;
                __depose_virtual_resource(this.Origin.ptTile);
            }
            break;

        case    ARM_2D_OP_INFO_PARAM_HAS_TARGET
            |   ARM_2D_OP_INFO_PARAM_HAS_ORIGIN
            |   ARM_2D_OP_INFO_PARAM_HAS_SOURCE
            |   ARM_2D_OP_INFO_PARAM_HAS_SOURCE_MASK:
        case    ARM_2D_OP_INFO_PARAM_HAS_TARGET
            |   ARM_2D_OP_INFO_PARAM_HAS_ORIGIN
            |   ARM_2D_OP_INFO_PARAM_HAS_SOURCE
            |   ARM_2D_OP_INFO_PARAM_HAS_TARGET_MASK:
        case    ARM_2D_OP_INFO_PARAM_HAS_TARGET
            |   ARM_2D_OP_INFO_PARAM_HAS_ORIGIN
            |   ARM_2D_OP_INFO_PARAM_HAS_SOURCE
            |   ARM_2D_OP_INFO_PARAM_HAS_TARGET_MASK
            |   ARM_2D_OP_INFO_PARAM_HAS_SOURCE_MASK: {
                arm_2d_op_src_orig_msk_t *ptThis = (arm_2d_op_src_orig_msk_t *)ptOP;
                __depose_virtual_resource(this.Origin.ptTile);
                __depose_virtual_resource(this.Mask.ptOriginSide);
                __depose_virtual_resource(this.Mask.ptTargetSide);
            }
            break;
    }
}

__WEAK
arm_fsm_rt_t __arm_2d_issue_sub_task_tile_process(  
                                        arm_2d_op_t *ptThis,
                                        __arm_2d_tile_param_t *ptParam)
{
    arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
    __arm_2d_sub_task_t *ptTask = &(__arm_2d_sub_task_t){
        .ptOP = (arm_2d_op_core_t *)ptThis,
        .Param.tTileProcess = *ptParam,
    };

    /* todo: update the tile process interface */

    /* call default software implementation */
    ARM_2D_RUN_DEFAULT( 0, __arm_2d_io_func_t);
    
    __arm_2d_sub_task_depose((arm_2d_op_core_t *)ptThis);
    return tResult;
}


__WEAK
arm_fsm_rt_t __arm_2d_issue_sub_task_fill(
                                    arm_2d_op_cp_t *ptThis,
                                    __arm_2d_tile_param_t *ptSource,
                                    __arm_2d_tile_param_t *ptTarget)
{
    arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
    
    __arm_2d_sub_task_t *ptTask = &(__arm_2d_sub_task_t){
        .ptOP = (arm_2d_op_core_t *)ptThis,
        .Param.tFill = {
            .tSource        = *ptSource,
            .tTarget        = *ptTarget,
        },
    };

    /* call default software implementation */
    ARM_2D_RUN_DEFAULT( 1,__arm_2d_io_func_t );

    __arm_2d_sub_task_depose((arm_2d_op_core_t *)ptThis);
    
    return tResult;
}

__WEAK
arm_fsm_rt_t __arm_2d_issue_sub_task_fill_with_mask(
                                    arm_2d_op_cp_t *ptThis,
                                    __arm_2d_tile_param_t *ptSource,
                                    __arm_2d_tile_param_t *ptSourceMask,
                                    __arm_2d_tile_param_t *ptTarget,
                                    __arm_2d_tile_param_t *ptTargetMask)
{
    arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
    
    __arm_2d_sub_task_t *ptTask = &(__arm_2d_sub_task_t){
        .ptOP = (arm_2d_op_core_t *)ptThis,
        .Param.tFillMask = {
            .use_as____arm_2d_param_fill_t = {
                .tSource        = *ptSource,
                .tTarget        = *ptTarget,
            },
        },
    };
    
    if (NULL == ptSourceMask){
        ptTask->Param.tFillMask.tSrcMask.bInvalid = true;
    } else {
        ptTask->Param.tFillMask.tSrcMask = *ptSourceMask;
    }
    
    if (NULL == ptTargetMask){
        ptTask->Param.tFillMask.tDesMask.bInvalid = true;
    } else {
        ptTask->Param.tFillMask.tDesMask = *ptTargetMask;
    }

    /* call default software implementation */
    ARM_2D_RUN_DEFAULT( 1,__arm_2d_io_func_t );

    __arm_2d_sub_task_depose((arm_2d_op_core_t *)ptThis);

    return tResult;
}

__WEAK
arm_fsm_rt_t __arm_2d_issue_sub_task_copy(arm_2d_op_cp_t *ptThis,
                                    __arm_2d_tile_param_t *ptSource,
                                    __arm_2d_tile_param_t *ptTarget,
                                    arm_2d_size_t * __RESTRICT ptCopySize)
{
    arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
    __arm_2d_sub_task_t *ptTask = &(__arm_2d_sub_task_t){
        .ptOP = (arm_2d_op_core_t *)ptThis,
        .Param.tCopy = {
            .tSource        = *ptSource,
            .tTarget        = *ptTarget,
            .tCopySize      = *ptCopySize,
        },
    };
    
    /* call default software implementation */
    ARM_2D_RUN_DEFAULT(0,__arm_2d_io_func_t );

    __arm_2d_sub_task_depose((arm_2d_op_core_t *)ptThis);

    return tResult;
}

__WEAK
arm_fsm_rt_t __arm_2d_issue_sub_task_copy_with_mask(
                                    arm_2d_op_cp_t *ptThis,
                                    __arm_2d_tile_param_t *ptSource,
                                    __arm_2d_tile_param_t *ptSourceMask,
                                    __arm_2d_tile_param_t *ptTarget,
                                    __arm_2d_tile_param_t *ptTargetMask,
                                    arm_2d_size_t * __RESTRICT ptCopySize)
{
    arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
    __arm_2d_sub_task_t *ptTask = &(__arm_2d_sub_task_t){
        .ptOP = (arm_2d_op_core_t *)ptThis,
        .Param.tCopyMask = {
            .use_as____arm_2d_param_copy_t = {
                .tSource        = *ptSource,
                .tTarget        = *ptTarget,
                .tCopySize      = *ptCopySize,
            },
        },
    };
    
    if (NULL == ptSourceMask){
        ptTask->Param.tCopyMask.tSrcMask.bInvalid = true;
    } else {
        ptTask->Param.tCopyMask.tSrcMask = *ptSourceMask;
    }
    
    if (NULL == ptTargetMask){
        ptTask->Param.tCopyMask.tDesMask.bInvalid = true;
    } else {
        ptTask->Param.tCopyMask.tDesMask = *ptTargetMask;
    }
    
    /* call default software implementation */
    ARM_2D_RUN_DEFAULT(0,__arm_2d_io_func_t );

    __arm_2d_sub_task_depose((arm_2d_op_core_t *)ptThis);
    return tResult;
}

__WEAK
arm_fsm_rt_t __arm_2d_issue_sub_task_fill_origin(
                                    arm_2d_op_cp_t *ptThis,
                                    __arm_2d_tile_param_t *ptSource,
                                    __arm_2d_tile_param_t *ptOrigin,
                                    __arm_2d_tile_param_t *ptTarget)
{
    arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
    
    
    __arm_2d_sub_task_t *ptTask = &(__arm_2d_sub_task_t){
        .ptOP = (arm_2d_op_core_t *)ptThis,
        .Param.tFillOrig = {
            .use_as____arm_2d_param_fill_t = {
                .tSource        = *ptSource,
                .tTarget        = *ptTarget,
            },
            .tOrigin        = *ptOrigin,
        },
    };

    /* call default software implementation */
    ARM_2D_RUN_DEFAULT( 1,__arm_2d_io_func_t );

    __arm_2d_sub_task_depose((arm_2d_op_core_t *)ptThis);
    return tResult;
}

__WEAK
arm_fsm_rt_t __arm_2d_issue_sub_task_copy_origin(
                                    arm_2d_op_cp_t *ptThis,
                                    __arm_2d_tile_param_t *ptSource,
                                    __arm_2d_tile_param_t *ptOrigin,
                                    __arm_2d_tile_param_t *ptTarget,
                                    arm_2d_size_t * __RESTRICT ptCopySize)
{
    arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
    __arm_2d_sub_task_t *ptTask = &(__arm_2d_sub_task_t){
        .ptOP = (arm_2d_op_core_t *)ptThis,
        .Param.tCopyOrig = {
            .use_as____arm_2d_param_copy_t = {
                .tSource        = *ptSource,
                .tTarget        = *ptTarget,
                .tCopySize      = *ptCopySize,
            },
            .tOrigin        = *ptOrigin,
        },
    };
    
    /* call default software implementation */
    ARM_2D_RUN_DEFAULT(0,__arm_2d_io_func_t );

    __arm_2d_sub_task_depose((arm_2d_op_core_t *)ptThis);
    return tResult;
}

__WEAK
arm_fsm_rt_t __arm_2d_issue_sub_task_copy_origin_masks(
                                        arm_2d_op_cp_t *ptThis,
                                        __arm_2d_tile_param_t *ptSource,
                                        __arm_2d_tile_param_t *ptOrigin,
                                        __arm_2d_tile_param_t *ptOriginMask,
                                        __arm_2d_tile_param_t *ptTarget,
                                        __arm_2d_tile_param_t *ptTargetMask,
                                        arm_2d_size_t * __RESTRICT ptCopySize)
{
    arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
    __arm_2d_sub_task_t *ptTask = &(__arm_2d_sub_task_t){
        .ptOP = (arm_2d_op_core_t *)ptThis,
        .Param.tCopyOrigMask = {
            .use_as____arm_2d_param_copy_orig_t = {
                .use_as____arm_2d_param_copy_t = {
                    .tSource        = *ptSource,
                    .tTarget        = *ptTarget,
                    .tCopySize      = *ptCopySize,
                },
                
                .tOrigin        = *ptOrigin,
            },
            .tOrigMask = *ptOriginMask,
            .tDesMask  = *ptTargetMask,
        },
    };
    
    /* call default software implementation */
    ARM_2D_RUN_DEFAULT(0,__arm_2d_io_func_t );

    __arm_2d_sub_task_depose((arm_2d_op_core_t *)ptThis);
    return tResult;
}

/*----------------------------------------------------------------------------*
 * Region Calculation                                                         *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(1,2)
static const arm_2d_tile_t * __arm_2d_tile_region_caculator( 
                                            const arm_2d_tile_t *ptTile,
                                            __arm_2d_tile_param_t *ptOut,
                                            uint_fast8_t *pchPixelLenInBit,
                                            bool bAllowEnforcedColour,
                                            uint32_t wMode)
{
    arm_2d_region_t tValidRegion;                                                     
    int32_t nOffset = 0;
    uint8_t *pchBuffer = NULL;                                            
                                    
    assert(NULL != ptTile);
    assert(NULL != ptOut);
    

    
    //memset(ptOut, 0, sizeof(__arm_2d_tile_param_t));
    
    ptTile = __arm_2d_tile_get_root(  ptTile,                           
                                    &tValidRegion,                   
                                    NULL,
                                    &ptOut->ptDerivedResource);                
    
    bool bDerivedResource = false;
    
    if (NULL != ptOut->ptDerivedResource) {
        if (!ptOut->ptDerivedResource->tInfo.bIsRoot) {
            bDerivedResource = true;
        }
    }
    
    if (NULL != ptTile) {    

        //! check if enforced colour is allowed
        if (bAllowEnforcedColour) {
            if (ptTile->tInfo.bHasEnforcedColour) {
                *pchPixelLenInBit = _BV(ptTile->tInfo.tColourInfo.u3ColourSZ);
            }
        }
        
        uint_fast8_t chPixelLenInBit = *pchPixelLenInBit;
    
        arm_2d_location_t tOffset = tValidRegion.tLocation;
        
        /* root tile can have offset location */
        tOffset.iX -= ptTile->tRegion.tLocation.iX;
        tOffset.iY -= ptTile->tRegion.tLocation.iY;
        
        if ((wMode) & ARM_2D_CP_MODE_X_MIRROR) {                             
            tOffset.iX = 0;                                                     
        }                                                                       
        if ((wMode) & ARM_2D_CP_MODE_Y_MIRROR) {                             
            tOffset.iY = 0;                                                     
        }

        if (chPixelLenInBit >= 8) {
            nOffset = ptTile->tRegion.tSize.iWidth * tOffset.iY + tOffset.iX;
            pchBuffer = ptTile->pchBuffer  
                      + (nOffset * chPixelLenInBit >> 3);            
        } else {
        
            nOffset = tOffset.iX;
        
            pchBuffer = ptTile->pchBuffer 
                      + ((ptTile->tRegion.tSize.iWidth 
                         *  chPixelLenInBit + 7) >> 3) * tOffset.iY
                      + ((tOffset.iX * chPixelLenInBit) >> 3);
        }
                 
        ptOut->iStride = ptTile->tRegion.tSize.iWidth;
        ptOut->nOffset = nOffset;
        ptOut->pBuffer = pchBuffer;
        ptOut->tValidRegion = tValidRegion;
        ptOut->tColour.chScheme = ptTile->tColourInfo.chScheme;
        ptOut->bDerivedResource = bDerivedResource;
        ptOut->bInvalid = false;
    }

    return ptTile;
}

ARM_NONNULL(1,2)
static
arm_fsm_rt_t __arm_2d_tile_process( arm_2d_op_t *ptThis,
                                    const arm_2d_tile_t *ptTarget)
{
    assert(NULL != ptTarget);
    assert(OP_CORE.ptOp->Info.Colour.u3ColourSZ >= ARM_2D_COLOUR_SZ_8BIT);
    
    arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;

    uint_fast8_t chPixelLenInBit = _BV(OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    __arm_2d_tile_param_t tTileParam;
    __arm_2d_tile_region_caculator( 
                                ptTarget, 
                                &tTileParam,
                                &chPixelLenInBit,
                                OP_CORE.ptOp->Info.Param.bAllowEnforcedColour,
                                0);

    tResult = __arm_2d_issue_sub_task_tile_process( ptThis, &tTileParam); 
    
    return tResult;
}


static void __arm_2d_source_side_tile_mirror_preprocess(
                                        const arm_2d_tile_t *ptTile,
                                        __arm_2d_tile_param_t *ptTileParam,
                                        uint_fast8_t chPixelLenInBit,
                                        const arm_2d_size_t *ptActualSize,
                                        uint32_t wMode)
{
    //! right and/or bottom alignment
    arm_2d_location_t tOffset = {0};
    
    if (ptTileParam->bDerivedResource) {
        //!< treat the content inside the valid region as the one to be mirrored
        //! right alignment 
        if (wMode & ARM_2D_CP_MODE_X_MIRROR) {
        
            tOffset.iX = ptTileParam->ptDerivedResource->tRegion.tLocation.iX;
            tOffset.iX +=  ( ptTileParam->ptDerivedResource->tRegion.tLocation.iX
                           + ptTileParam->ptDerivedResource->tRegion.tSize.iWidth)
                       -   (  ptTileParam->tValidRegion.tLocation.iX
                           +  ptActualSize->iWidth);

            ptTileParam->tValidRegion.tLocation.iX = tOffset.iX;
        }
        
        //! bottom alignment 
        if (wMode & ARM_2D_CP_MODE_Y_MIRROR) {
            tOffset.iY = ptTileParam->ptDerivedResource->tRegion.tLocation.iY;
            tOffset.iY +=  ( ptTileParam->ptDerivedResource->tRegion.tLocation.iY
                           + ptTileParam->ptDerivedResource->tRegion.tSize.iHeight)
                       -   (  ptTileParam->tValidRegion.tLocation.iY
                           +  ptActualSize->iHeight);
            ptTileParam->tValidRegion.tLocation.iY = tOffset.iY;
        }
    } else {

        //!< treat valid region as a logic region to indicate the area for mirroring.
        //! right alignment 
        if (wMode & ARM_2D_CP_MODE_X_MIRROR) {
            tOffset.iX = ptTile->tRegion.tSize.iWidth 
                           - (  ptTileParam->tValidRegion.tLocation.iX 
                             +  ptActualSize->iWidth);
            ptTileParam->tValidRegion.tLocation.iX = tOffset.iX;
        }
        
        //! bottom alignment 
        if (wMode & ARM_2D_CP_MODE_Y_MIRROR) {
            tOffset.iY = ptTile->tRegion.tSize.iHeight 
                           - (  ptTileParam->tValidRegion.tLocation.iY 
                             +  ptActualSize->iHeight);
            ptTileParam->tValidRegion.tLocation.iY = tOffset.iY;
        }
    }
              
    if (chPixelLenInBit >= 8) {
        
        ptTileParam->nOffset += ( tOffset.iY * ptTile->tRegion.tSize.iWidth 
                    + tOffset.iX);
        
        ptTileParam->pBuffer = ptTile->pchBuffer 
                  + (ptTileParam->nOffset * chPixelLenInBit >> 3);
    } else {
        ptTileParam->nOffset += tOffset.iX;
    
        (*(uintptr_t *)&(ptTileParam->pBuffer)) += 
                  + ((ptTile->tRegion.tSize.iWidth 
                     *  chPixelLenInBit + 7) >> 3) * tOffset.iY
                  + ((tOffset.iX * chPixelLenInBit) >> 3);
    }
}

ARM_NONNULL(1,2)
static
arm_fsm_rt_t __arm_2d_region_calculator(  arm_2d_op_cp_t *ptThis,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_tile_t *ptTargetMask,
                                            uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);
    //assert(OP_CORE.ptOp->Info.Colour.u3ColourSZ >= ARM_2D_COLOUR_SZ_8BIT);
    
    arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
    uint_fast8_t chTargetPixelLenInBit = _BV(OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    uint_fast8_t chSourcePixelLenInBit = chTargetPixelLenInBit;
    uint_fast8_t chOriginPixelLenInBit = chTargetPixelLenInBit;
    uint_fast8_t chSourceMaskPixelLenInBit = 8;
    
    __arm_2d_tile_param_t tSourceTileParam;
    __arm_2d_tile_param_t tSourceMaskParam;
    __arm_2d_tile_param_t tTargetTileParam;
    __arm_2d_tile_param_t tTargetMaskParam;
    __arm_2d_tile_param_t tOriginTileParam;
    arm_2d_tile_t tSourceMask;
    arm_2d_tile_t tTargetMask;
    
    //const arm_2d_tile_t *ptTargetMask = NULL;
    //const arm_2d_tile_t *ptSourceMask = NULL;
    
    const arm_2d_tile_t *ptOrigin = NULL;
    
    if (OP_CORE.ptOp->Info.Param.bHasOrigin) {
        arm_2d_op_src_orig_t *ptOP = (arm_2d_op_src_orig_t *)ptThis;
        //uint_fast8_t chOriginPixelLenInBit = chSourcePixelLenInBit;
        ptOrigin = __arm_2d_tile_region_caculator( 
                                ptOP->Origin.ptTile, 
                                &tOriginTileParam,
                                &chOriginPixelLenInBit,
                                OP_CORE.ptOp->Info.Param.bAllowEnforcedColour,
                                wMode); 
                                
        if (NULL == ptOrigin) {
            return (arm_fsm_rt_t)ARM_2D_ERR_OUT_OF_REGION;
        }
    }

    ptSource = __arm_2d_tile_region_caculator( 
                                ptSource, 
                                &tSourceTileParam,
                                &chSourcePixelLenInBit,
                                OP_CORE.ptOp->Info.Param.bAllowEnforcedColour,
                                wMode); 
                                
     
    
    ptTarget = __arm_2d_tile_region_caculator( 
                                ptTarget, 
                                &tTargetTileParam,
                                &chTargetPixelLenInBit,
                                false,
                                0);

    if (NULL == ptSource || NULL == ptTarget) {
        return (arm_fsm_rt_t)ARM_2D_ERR_OUT_OF_REGION;
    }
    
    
    if (!OP_CORE.ptOp->Info.Param.bHasOrigin) {                                 //!< no origin 
        if (OP_CORE.ptOp->Info.Param.bHasSrcMask) {
            arm_2d_op_src_msk_t *ptOP = (arm_2d_op_src_msk_t *)ptThis;  
            
            ptSourceMask = ptOP->Mask.ptSourceSide;
            
            if (NULL != ptSourceMask) {
                ptSourceMask = arm_2d_tile_generate_child( 
                                            ptSourceMask,
                                            &tSourceTileParam.tValidRegion,
                                            &tSourceMask,
                                            false);

                ptSourceMask = __arm_2d_tile_region_caculator( 
                                ptSourceMask, 
                                &tSourceMaskParam,
                                &chSourceMaskPixelLenInBit,
                                true,
                                wMode); 
            }
        }
        
        if (OP_CORE.ptOp->Info.Param.bHasDesMask) {
            arm_2d_op_src_msk_t *ptOP = (arm_2d_op_src_msk_t *)ptThis;
            ptTargetMask = ptOP->Mask.ptTargetSide;
            if (NULL != ptTargetMask) {
                uint_fast8_t chTargetMaskPixelLenInBit = 8;
                
                do {
                    arm_2d_region_t tTempRegion= {
                        .tSize = ptThis->Target.ptTile->tRegion.tSize,
                    };
                    
                    arm_2d_get_absolute_location(ptThis->Target.ptTile,
                                                 &tTempRegion.tLocation);
                    
                    tTempRegion.tLocation.iX 
                        = tTargetTileParam.tValidRegion.tLocation.iX 
                        - tTempRegion.tLocation.iX;

                    tTempRegion.tSize.iWidth
                        = tTargetTileParam.tValidRegion.tSize.iWidth 
                        - tTempRegion.tSize.iWidth;
                
                    
                    arm_2d_region_t tNewTargetMaskRegion = ptTargetMask->tRegion;
                
                    tNewTargetMaskRegion.tLocation.iX += tTempRegion.tLocation.iX;
                    tNewTargetMaskRegion.tSize.iWidth += tTempRegion.tSize.iWidth;
                    
                    // when the target mask is not 1-horizontal line mask
                    if (ptTargetMask->tRegion.tSize.iHeight != 1 ) {
                        tTempRegion.tLocation.iY 
                            = tTargetTileParam.tValidRegion.tLocation.iY 
                            - tTempRegion.tLocation.iY;
                    
                        tTempRegion.tSize.iHeight
                            = tTargetTileParam.tValidRegion.tSize.iHeight 
                            - tTempRegion.tSize.iHeight;
                    
                        
                        tNewTargetMaskRegion.tLocation.iY += tTempRegion.tLocation.iY;
                        tNewTargetMaskRegion.tSize.iHeight += tTempRegion.tSize.iHeight;
                    }
                
                    ptTargetMask = arm_2d_tile_generate_child( 
                                            ptTargetMask,
                                            &tNewTargetMaskRegion,
                                            &tTargetMask,
                                            false);
                                            
                } while(0);
                
                
                ptTargetMask = __arm_2d_tile_region_caculator( 
                                ptTargetMask, 
                                &tTargetMaskParam,
                                &chTargetMaskPixelLenInBit,
                                true,
                                0); 
            }
        }
    } else {                                                                    //!< has origin
        if (OP_CORE.ptOp->Info.Param.bHasSrcMask) {
            arm_2d_op_src_orig_msk_t *ptOP = (arm_2d_op_src_orig_msk_t *)ptThis;  
            
            ptSourceMask = ptOP->Mask.ptOriginSide;
            
            if (NULL != ptSourceMask) {
                ptSourceMask = arm_2d_tile_generate_child( 
                                            ptSourceMask,
                                            &tOriginTileParam.tValidRegion,
                                            &tSourceMask,
                                            false);

                ptSourceMask = __arm_2d_tile_region_caculator( 
                                ptSourceMask, 
                                &tSourceMaskParam,
                                &chSourceMaskPixelLenInBit,
                                true,
                                wMode); 
            }
        }
        
        if (OP_CORE.ptOp->Info.Param.bHasDesMask) {
            arm_2d_op_src_orig_msk_t *ptOP = (arm_2d_op_src_orig_msk_t *)ptThis;
            ptTargetMask = ptOP->Mask.ptTargetSide;
            if (NULL != ptTargetMask) {
                uint_fast8_t chTargetMaskPixelLenInBit = 8;
                
                do {
                    arm_2d_region_t tTempRegion= {
                        .tSize = ptThis->Target.ptTile->tRegion.tSize,
                    };
                    
                    arm_2d_get_absolute_location(ptThis->Target.ptTile,
                                                 &tTempRegion.tLocation);
                    
                    tTempRegion.tLocation.iX 
                        = tTargetTileParam.tValidRegion.tLocation.iX 
                        - tTempRegion.tLocation.iX;

                    tTempRegion.tSize.iWidth
                        = tTargetTileParam.tValidRegion.tSize.iWidth 
                        - tTempRegion.tSize.iWidth;
                
                    
                    arm_2d_region_t tNewTargetMaskRegion = ptTargetMask->tRegion;
                
                    tNewTargetMaskRegion.tLocation.iX += tTempRegion.tLocation.iX;
                    tNewTargetMaskRegion.tSize.iWidth += tTempRegion.tSize.iWidth;
                    
                    // when the target mask is not 1-horizontal line mask
                    if (ptTargetMask->tRegion.tSize.iHeight != 1 ) {
                        tTempRegion.tLocation.iY 
                            = tTargetTileParam.tValidRegion.tLocation.iY 
                            - tTempRegion.tLocation.iY;
                    
                        tTempRegion.tSize.iHeight
                            = tTargetTileParam.tValidRegion.tSize.iHeight 
                            - tTempRegion.tSize.iHeight;
                    
                        
                        tNewTargetMaskRegion.tLocation.iY += tTempRegion.tLocation.iY;
                        tNewTargetMaskRegion.tSize.iHeight += tTempRegion.tSize.iHeight;
                    }
                
                    ptTargetMask = arm_2d_tile_generate_child( 
                                            ptTargetMask,
                                            &tNewTargetMaskRegion,
                                            &tTargetMask,
                                            false);
                                            
                } while(0);
                
                
                ptTargetMask = __arm_2d_tile_region_caculator( 
                                ptTargetMask, 
                                &tTargetMaskParam,
                                &chTargetMaskPixelLenInBit,
                                true,
                                0); 
            }
        }
    }
    
    arm_2d_size_t tActualSize = {
        .iWidth = MIN(  tSourceTileParam.tValidRegion.tSize.iWidth, 
                        tTargetTileParam.tValidRegion.tSize.iWidth),
        .iHeight = MIN( tSourceTileParam.tValidRegion.tSize.iHeight, 
                        tTargetTileParam.tValidRegion.tSize.iHeight),
    };

    /* trim source valid region */
    tSourceTileParam.tValidRegion.tSize = tActualSize;

    if (wMode & ARM_2D_CP_MODE_FILL) {                                          //!< tiling (tile fill) operation

        if (OP_CORE.ptOp->Info.Param.bHasOrigin) {
            /*! \brief masks are not supported in fill with origin mode */
            assert(!OP_CORE.ptOp->Info.Param.bHasSrcMask);
            assert(!OP_CORE.ptOp->Info.Param.bHasDesMask);
            
            //! handle mirroring
            do {
                __arm_2d_source_side_tile_mirror_preprocess(
                                            ptOrigin,
                                            &tOriginTileParam,
                                            chOriginPixelLenInBit,
                                            &tOriginTileParam.tValidRegion.tSize,
                                            wMode);

                /*! \note NOT SUPPORTED YET
                if (    OP_CORE.ptOp->Info.Param.bHasSrcMask
                   &&   (NULL != ptSourceMask)) {
                    __arm_2d_source_side_tile_mirror_preprocess(
                                            ptSourceMask,
                                            &tSourceMaskParam,
                                            chSourceMaskPixelLenInBit,
                                            &tSourceMaskParam.tValidRegion.tSize,
                                            wMode);
                }
                */
            } while(0);

            /* load virtual resource if any */
            do {
                arm_2d_err_t tErr = 
                    __load_virtual_resource(ptOrigin, &tOriginTileParam);
                if (tErr != ARM_2D_ERR_NONE) {
                    return (arm_fsm_rt_t)tErr;
                }
            } while(0);

            tResult = __arm_2d_issue_sub_task_fill_origin(
                                                    ptThis,
                                                    &tSourceTileParam,
                                                    &tOriginTileParam,
                                                    &tTargetTileParam);
        } else {

            //! handle mirroring
            do {
                __arm_2d_source_side_tile_mirror_preprocess(
                                            ptSource,
                                            &tSourceTileParam,
                                            chSourcePixelLenInBit,
                                            &tSourceTileParam.tValidRegion.tSize,
                                            wMode);
            
                if (    OP_CORE.ptOp->Info.Param.bHasSrcMask
                   &&   (NULL != ptSourceMask)) {
                    __arm_2d_source_side_tile_mirror_preprocess(
                                            ptSourceMask,
                                            &tSourceMaskParam,
                                            chSourceMaskPixelLenInBit,
                                            &tSourceMaskParam.tValidRegion.tSize,
                                            wMode);
                }
            } while(0);

            /* trim source mask valid region */
            if (NULL != ptSourceMask) {
                tSourceMaskParam.tValidRegion.tSize.iWidth = 
                    MIN(tSourceMaskParam.tValidRegion.tSize.iWidth, 
                        tSourceTileParam.tValidRegion.tSize.iWidth);
                tSourceMaskParam.tValidRegion.tSize.iHeight = 
                    MIN(tSourceMaskParam.tValidRegion.tSize.iHeight, 
                        tSourceTileParam.tValidRegion.tSize.iHeight);
            }

            if (NULL != ptTargetMask) {
                tTargetMaskParam.tValidRegion.tSize.iWidth = 
                    MIN(tTargetMaskParam.tValidRegion.tSize.iWidth, 
                        tTargetTileParam.tValidRegion.tSize.iWidth);
                tTargetMaskParam.tValidRegion.tSize.iHeight = 
                    MIN(tTargetMaskParam.tValidRegion.tSize.iHeight, 
                        tTargetTileParam.tValidRegion.tSize.iHeight);
            }

            /* load virtual resource if any */
            do {
                arm_2d_err_t tErr = 
                    __load_virtual_resource(ptSource, &tSourceTileParam);
                if (tErr != ARM_2D_ERR_NONE) {
                    return (arm_fsm_rt_t)tErr;
                }

                tErr = __load_virtual_resource(ptSourceMask, &tSourceMaskParam);
                if (tErr != ARM_2D_ERR_NONE) {
                    return (arm_fsm_rt_t)tErr;
                }

                tErr = __load_virtual_resource(ptTargetMask, &tTargetMaskParam);
                if (tErr != ARM_2D_ERR_NONE) {
                    return (arm_fsm_rt_t)tErr;
                }
            } while(0);

            if (    (OP_CORE.ptOp->Info.Param.bHasSrcMask)
               ||   (OP_CORE.ptOp->Info.Param.bHasDesMask)){
               
                tResult = __arm_2d_issue_sub_task_fill_with_mask( 
                            ptThis, 
                            &tSourceTileParam,
                            ((NULL != ptSourceMask) ? &tSourceMaskParam : NULL),
                            &tTargetTileParam,
                            ((NULL != ptTargetMask) ? &tTargetMaskParam : NULL)
                        );
            } else {
                tResult = __arm_2d_issue_sub_task_fill( ptThis, 
                                                        &tSourceTileParam,
                                                        &tTargetTileParam);
            }
        }
    } else {                                                                    //!< normal tile copy operation

        if (OP_CORE.ptOp->Info.Param.bHasOrigin) {
            //! handle mirroring
            do {
                __arm_2d_source_side_tile_mirror_preprocess(
                                                        ptOrigin,
                                                        &tOriginTileParam,
                                                        chOriginPixelLenInBit,
                                                        &tActualSize,
                                                        wMode);
                                                            
                if (    OP_CORE.ptOp->Info.Param.bHasSrcMask
                   &&   (NULL != ptSourceMask)) {
                    arm_2d_size_t tMaskActualSize = {
                        .iWidth = MIN(tActualSize.iWidth, 
                                      tSourceMaskParam.tValidRegion.tSize.iWidth),
                        .iHeight = MIN(tActualSize.iHeight, 
                                      tSourceMaskParam.tValidRegion.tSize.iHeight),
                    };
                    __arm_2d_source_side_tile_mirror_preprocess(
                                                        ptSourceMask,
                                                        &tSourceMaskParam,
                                                        chSourceMaskPixelLenInBit,
                                                        &tMaskActualSize,
                                                        wMode);
                }
            } while(0);

//            /* trim source valid region */
//            tSourceTileParam.tValidRegion.tSize = tActualSize;

            /* trim source mask valid region */
            if (NULL != ptTargetMask) {
                tTargetMaskParam.tValidRegion.tSize.iWidth = 
                    MIN(tTargetMaskParam.tValidRegion.tSize.iWidth, 
                        tTargetTileParam.tValidRegion.tSize.iWidth);
                tTargetMaskParam.tValidRegion.tSize.iHeight = 
                    MIN(tTargetMaskParam.tValidRegion.tSize.iHeight, 
                        tTargetTileParam.tValidRegion.tSize.iHeight);
            }

            /* load virtual resource if any */
            do {
                arm_2d_err_t tErr = 
                    __load_virtual_resource(ptOrigin, &tOriginTileParam);
                if (tErr != ARM_2D_ERR_NONE) {
                    return (arm_fsm_rt_t)tErr;
                }
                
                tErr = __load_virtual_resource(ptSourceMask, &tSourceMaskParam);
                if (tErr != ARM_2D_ERR_NONE) {
                    return (arm_fsm_rt_t)tErr;
                }

                tErr = __load_virtual_resource(ptTargetMask, &tTargetMaskParam);
                if (tErr != ARM_2D_ERR_NONE) {
                    return (arm_fsm_rt_t)tErr;
                }
            } while(0);

            if (    (OP_CORE.ptOp->Info.Param.bHasSrcMask)
               ||   (OP_CORE.ptOp->Info.Param.bHasDesMask)){
                tResult = __arm_2d_issue_sub_task_copy_origin_masks( 
                            ptThis, 
                            &tSourceTileParam,
                            &tOriginTileParam,
                            ((NULL != ptSourceMask) ? &tSourceMaskParam : NULL),
                            &tTargetTileParam,
                            ((NULL != ptTargetMask) ? &tTargetMaskParam : NULL),
                            &tActualSize);
            } else {
                tResult = __arm_2d_issue_sub_task_copy_origin( 
                                                        ptThis, 
                                                        &tSourceTileParam,
                                                        &tOriginTileParam,
                                                        &tTargetTileParam,
                                                        &tActualSize);
            }
        } else {



            //! handle mirroring
            do {
                __arm_2d_source_side_tile_mirror_preprocess(
                                                        ptSource,
                                                        &tSourceTileParam,
                                                        chSourcePixelLenInBit,
                                                        &tActualSize,
                                                        wMode);
                                                            
                if (    OP_CORE.ptOp->Info.Param.bHasSrcMask
                   &&   (NULL != ptSourceMask)) {
                    arm_2d_size_t tMaskActualSize = {
                        .iWidth = MIN(tActualSize.iWidth, 
                                      tSourceMaskParam.tValidRegion.tSize.iWidth),
                        .iHeight = MIN(tActualSize.iHeight, 
                                      tSourceMaskParam.tValidRegion.tSize.iHeight),
                    };
                    __arm_2d_source_side_tile_mirror_preprocess(
                                                        ptSourceMask,
                                                        &tSourceMaskParam,
                                                        chSourceMaskPixelLenInBit,
                                                        &tMaskActualSize,
                                                        wMode);
                }
            } while(0);

//            /* trim source valid region */
//            tSourceTileParam.tValidRegion.tSize = tActualSize;

            if (NULL != ptSourceMask) {
                tSourceMaskParam.tValidRegion.tSize.iWidth = 
                    MIN(tSourceMaskParam.tValidRegion.tSize.iWidth, 
                        tSourceTileParam.tValidRegion.tSize.iWidth);
                tSourceMaskParam.tValidRegion.tSize.iHeight = 
                    MIN(tSourceMaskParam.tValidRegion.tSize.iHeight, 
                        tSourceTileParam.tValidRegion.tSize.iHeight);
            }

            if (NULL != ptTargetMask) {
                tTargetMaskParam.tValidRegion.tSize.iWidth = 
                    MIN(tTargetMaskParam.tValidRegion.tSize.iWidth, 
                        tTargetTileParam.tValidRegion.tSize.iWidth);
                tTargetMaskParam.tValidRegion.tSize.iHeight = 
                    MIN(tTargetMaskParam.tValidRegion.tSize.iHeight, 
                        tTargetTileParam.tValidRegion.tSize.iHeight);
            }

            /* load virtual resource if any */
            do {
                arm_2d_err_t tErr = 
                    __load_virtual_resource(ptSource, &tSourceTileParam);
                if (tErr != ARM_2D_ERR_NONE) {
                    return (arm_fsm_rt_t)tErr;
                }
                
                tErr = __load_virtual_resource(ptSourceMask, &tSourceMaskParam);
                if (tErr != ARM_2D_ERR_NONE) {
                    return (arm_fsm_rt_t)tErr;
                }

                tErr = __load_virtual_resource(ptTargetMask, &tTargetMaskParam);
                if (tErr != ARM_2D_ERR_NONE) {
                    return (arm_fsm_rt_t)tErr;
                }
            } while(0);

            if (    (OP_CORE.ptOp->Info.Param.bHasSrcMask)
               ||   (OP_CORE.ptOp->Info.Param.bHasDesMask)){
                tResult = __arm_2d_issue_sub_task_copy_with_mask( 
                            ptThis, 
                            &tSourceTileParam,
                            ((NULL != ptSourceMask) ? &tSourceMaskParam : NULL),
                            &tTargetTileParam,
                            ((NULL != ptTargetMask) ? &tTargetMaskParam : NULL),
                            &tActualSize);
            } else {
                tResult = __arm_2d_issue_sub_task_copy( ptThis, 
                                                        &tSourceTileParam,
                                                        &tTargetTileParam,
                                                        &tActualSize);
            }
        }
    }

    return tResult;
}


static
arm_fsm_rt_t __tile_clipped_pave(
                            arm_2d_op_cp_t *ptThis,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            arm_2d_region_t *ptClippedRegion,
                            uint32_t wMode)
{

    arm_2d_tile_t tTempSourceTile = {0};
    arm_2d_tile_t tTargetTile = {0};

    arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_OUT_OF_REGION;
    do {
        arm_2d_region_t tempRegion = {
            .tLocation = this.Source.ptTile->tRegion.tLocation,
            .tSize = ptRegion->tSize,
        };
    
        tempRegion.tLocation.iX = -ptRegion->tLocation.iX;
        tempRegion.tLocation.iY = -ptRegion->tLocation.iY;
        tempRegion.tSize.iWidth += ptRegion->tLocation.iX;
        tempRegion.tSize.iHeight += ptRegion->tLocation.iY;
    
    
        if (NULL == arm_2d_tile_generate_child( this.Source.ptTile, 
                                    &tempRegion, 
                                    &tTempSourceTile, 
                                    true)) {
            break;
        };
        
        if (NULL != ptClippedRegion) {
            *ptClippedRegion = tTempSourceTile.tRegion;
        }

        //! [Modify][Target.ptTile]
        //if (NULL == arm_2d_tile_generate_child( this.Target.ptTile, 
        if (NULL == arm_2d_tile_generate_child( //this.use_as__arm_2d_op_core_t.Runtime.ptTargetTile,
                                                ptTarget,
                                                ptRegion, 
                                                &tTargetTile, 
                                                true)) {
            break;
        }


    #if 0
        switch (OP_CORE.ptOp->Info.Colour.u3ColourSZ) {
            case ARM_2D_COLOUR_SZ_8BIT:
            case ARM_2D_COLOUR_SZ_16BIT:
            case ARM_2D_COLOUR_SZ_32BIT:
                tResult = __arm_2d_region_calculator( ptThis, 
                                                    &tTempSourceTile, 
                                                    &tTargetTile, 
                                                    wMode);
                break;
            default:
                tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
                break;
        }
    #else
        if (OP_CORE.ptOp->Info.Colour.u3ColourSZ >= ARM_2D_COLOUR_SZ_8BIT) {
            tResult = __arm_2d_region_calculator( ptThis, 
                                    &tTempSourceTile, 
                                    NULL,   //!< source mask
                                    &tTargetTile, 
                                    NULL,   //!< target mask
                                    wMode);
        } else {
            tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
        }
    #endif
    } while(0);

    return tResult;
}

static arm_fsm_rt_t __tile_non_negtive_location_pave(
                                                arm_2d_op_cp_t *ptThis,
                                                const arm_2d_tile_t *ptSource,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion,
                                                uint32_t wMode)
{
    arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_OUT_OF_REGION;
    do {
        arm_2d_tile_t tTile = {0};
        if (NULL == arm_2d_tile_generate_child(ptTarget, ptRegion, &tTile, true)) {
            break;
        }
    #if 0
        switch (OP_CORE.ptOp->Info.Colour.u3ColourSZ) {
            case ARM_2D_COLOUR_SZ_8BIT:
            case ARM_2D_COLOUR_SZ_16BIT:
            case ARM_2D_COLOUR_SZ_32BIT:
                tResult = __arm_2d_region_calculator(ptThis, ptSource, &tTile, wMode);
                break;
            default:
                tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
                break;
        }
    #else
        if (OP_CORE.ptOp->Info.Colour.u3ColourSZ >= ARM_2D_COLOUR_SZ_8BIT) {
            tResult = __arm_2d_region_calculator( ptThis, 
                                                    ptSource, 
                                                    NULL,       //!< source mask
                                                    &tTile, 
                                                    NULL,       //!< target mask
                                                    wMode);
        } else {
            tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
        }
    #endif
    } while(0);

    return tResult;
}


__WEAK 
bool __arm_2d_op_ensure_resource(   arm_2d_op_core_t *ptOP, 
                                    uint_fast16_t hwRequired)
{
    ARM_2D_UNUSED(hwRequired);
    ARM_2D_UNUSED(ptOP);
    return true;
}

static
arm_fsm_rt_t __arm_2d_op_frontend_region_process( arm_2d_op_core_t *ptOP)
{
   ARM_2D_IMPL(arm_2d_op_t, ptOP)

    arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_OUT_OF_REGION;;
    arm_2d_tile_t tTile = {0};
    arm_2d_region_t tDrawRegion = {0};
    arm_2d_region_t tTargetRegion = {0};
    arm_2d_tile_t *ptTarget = NULL;

    if (!__arm_2d_op_ensure_resource(ptOP, 1)) {
        //! insufficient resources, ask users to try again
        return arm_fsm_rt_wait_for_res;
    }
    
    
    if (NULL == this.Target.ptRegion) {
        //! use target region instead
        this.Target.ptRegion = &(this.Target.ptTile->tRegion);
        
        tTargetRegion.tSize = (*this.Target.ptRegion).tSize;
    } else {

        tTargetRegion = *this.Target.ptRegion;
    }

    //! handle the offset of the target tile
    do {
        //arm_2d_tile_t tValidTargetTile;
        arm_2d_region_t tValidRegion;
        arm_2d_location_t tOffset;
        if (NULL == arm_2d_tile_get_root(this.Target.ptTile, &tValidRegion, &tOffset)) {
            //! nothing to draw, actually
            return (arm_fsm_rt_t)ARM_2D_ERR_OUT_OF_REGION;
        }

        //! calculate the valid region in the view of the target tile
        tValidRegion.tLocation = tOffset;
        
        //! [Modify][Target.ptTile]
        //this.Target.ptTile = arm_2d_tile_generate_child(  this.Target.ptTile,
        //this.use_as__arm_2d_op_core_t.Runtime.ptTargetTile 
        
        ptTarget = arm_2d_tile_generate_child(   this.Target.ptTile,
                                            &tValidRegion,
                                            &tTile,
                                            true);
        //! [Modify][Target.ptTile]
        //assert(NULL != this.Target.ptTile);
        //assert(NULL != this.use_as__arm_2d_op_core_t.Runtime.ptTargetTile);
        assert(NULL != ptTarget);
        tTargetRegion.tLocation.iX -= tOffset.iX;
        tTargetRegion.tLocation.iY -= tOffset.iY;

    } while(0);

    //! [Modify][Target.ptTile]
    //tDrawRegion.tSize = this.Target.ptTile->tRegion.tSize;
    //tDrawRegion.tSize 
    //    = this.use_as__arm_2d_op_core_t.Runtime.ptTargetTile->tRegion.tSize;
    tDrawRegion.tSize = ptTarget->tRegion.tSize;
    if (!arm_2d_region_intersect(   &tDrawRegion, 
                                    &tTargetRegion, 
                                    &tDrawRegion)) {
        return (arm_fsm_rt_t)ARM_2D_ERR_OUT_OF_REGION;
    }


    //! drawing
    do {
        arm_2d_tile_t tTempTile = {0};
        
        //! [Modify][Target.ptTile]
        //if (NULL == arm_2d_tile_generate_child( this.Target.ptTile, 
        if (NULL == arm_2d_tile_generate_child( 
                            //this.use_as__arm_2d_op_core_t.Runtime.ptTargetTile,
                            ptTarget,
                            &tTargetRegion, 
                            &tTempTile, true)) {
            break;
        }
    #if 0
        switch (OP_CORE.ptOp->Info.Colour.u3ColourSZ) {
            case ARM_2D_COLOUR_SZ_8BIT:
            case ARM_2D_COLOUR_SZ_16BIT:
            case ARM_2D_COLOUR_SZ_32BIT:
                tResult = __arm_2d_region_calculator(ptThis, ptSource, &tTile, wMode);
                break;
            default:
                tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
                break;
        }
    #else
        if (OP_CORE.ptOp->Info.Colour.u3ColourSZ >= ARM_2D_COLOUR_SZ_8BIT) {
            tResult = __arm_2d_tile_process( ptThis,
                                            &tTempTile);
        } else {
            tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
        }
    #endif
    } while(0);

    return tResult;
}

static
arm_fsm_rt_t __arm_2d_op_frontend_control( arm_2d_op_core_t *ptOP)
{
    /*TODO: add support for control operations*/
    return (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
}

static
arm_fsm_rt_t __arm_2d_op_frontend_region_process_with_src( arm_2d_op_core_t *ptOP)
{
    ARM_2D_IMPL(arm_2d_op_src_t, ptOP)

    arm_fsm_rt_t tResult;
    arm_2d_tile_t tTile = {0};
    arm_2d_region_t tDrawRegion = {0};
    arm_2d_region_t tTargetRegion = {0};
    const arm_2d_tile_t *ptTarget = NULL;

    if (!__arm_2d_op_ensure_resource(ptOP, 4)) {
        //! insufficient resources, ask users to try again
        return arm_fsm_rt_wait_for_res;
    }
    
    
    if (NULL == this.Target.ptRegion) {
        //! use target region instead
        this.Target.ptRegion = &(this.Target.ptTile->tRegion);
        
        tTargetRegion.tSize = (*this.Target.ptRegion).tSize;
    } else {

        tTargetRegion = *this.Target.ptRegion;
    }

    //! handle the offset of the target tile
    do {
        //arm_2d_tile_t tValidTargetTile;
        
        arm_2d_region_t tValidRegion;
        arm_2d_location_t tOffset;
        if (NULL == arm_2d_tile_get_root(this.Target.ptTile, &tValidRegion, &tOffset)) {
            //! nothing to draw, actually
            return (arm_fsm_rt_t)ARM_2D_ERR_OUT_OF_REGION;
        }

        //! calculate the valid region in the view of the target tile
        tValidRegion.tLocation = tOffset;
        //if (tOffset.iX != 0 || tOffset.iY != 0) {
            //!! [Modify][Target.ptTile]
            //this.Target.ptTile = arm_2d_tile_generate_child(  this.Target.ptTile,
            //this.use_as__arm_2d_op_core_t.Runtime.ptTargetTile 
            ptTarget = arm_2d_tile_generate_child(  this.Target.ptTile,
                                                    &tValidRegion,
                                                    &tTile,
                                                    true);
            //!! [Modify][Target.ptTile]
            //assert(NULL != this.Target.ptTile);
            //assert(NULL != this.use_as__arm_2d_op_core_t.Runtime.ptTargetTile);
            assert(NULL != ptTarget);
            tTargetRegion.tLocation.iX -= tOffset.iX;
            tTargetRegion.tLocation.iY -= tOffset.iY;
        //}

    } while(false);



    if ( this.wMode & ARM_2D_CP_MODE_FILL) {
        /* quickly ignore non visiable area, only for FILL mode */
        if (tTargetRegion.tLocation.iX < 0) {
            int_fast16_t iX = tTargetRegion.tLocation.iX;
            
            tTargetRegion.tLocation.iX %= this.Source.ptTile->tRegion.tSize.iWidth;

            //! calculate the delta
            iX = tTargetRegion.tLocation.iX - iX;

            //! update the width to make sure the range won't be expanded mistakenly
            tTargetRegion.tSize.iWidth -= iX;
        }

        if (tTargetRegion.tLocation.iY < 0) {
            int_fast16_t iY = tTargetRegion.tLocation.iY;
            tTargetRegion.tLocation.iY %= this.Source.ptTile->tRegion.tSize.iHeight;

            //! calculate the delta
            iY = tTargetRegion.tLocation.iY - iY;

            //! update the width to make sure the range won't be expanded mistakenly
            tTargetRegion.tSize.iHeight -= iY;
        }
    }

    //!! [Modify][Target.ptTile]
    //tDrawRegion.tSize = this.Target.ptTile->tRegion.tSize;
    //tDrawRegion.tSize = this.use_as__arm_2d_op_core_t.Runtime.ptTargetTile->tRegion.tSize;
    tDrawRegion.tSize = ptTarget->tRegion.tSize;
    if (!arm_2d_region_intersect(   &tDrawRegion, 
                                    &tTargetRegion, 
                                    &tDrawRegion)) {
        //! no overlapping
        return (arm_fsm_rt_t)ARM_2D_ERR_OUT_OF_REGION;
    }

    if (    (tTargetRegion.tLocation.iX < 0) 
        ||  (tTargetRegion.tLocation.iY < 0)) {
        /*! draw the top left corner */
        /*
          HOW IT WORKS:

           source tile (with negative relative location)
          +------------------------------------+
          |                                    |
          |     +------------------------------+-- ... --+
          |     |//////////////////////////////|         |
          |     |//ACTUAL DRAWING PART/////////|         |
          |     |//////////////////////////////|         |
          +-----+------------------------------+-- ... --|
                |                              |         |
               ...                            ...       ...
                |                              |         |
                +------------------------------+-- ... --+
         */
        arm_2d_region_t tClippdRegion;
        tResult = __tile_clipped_pave(  &this,
                                        ptTarget,
                                        &tTargetRegion,
                                        &tClippdRegion,
                                        this.wMode & ~ARM_2D_CP_MODE_FILL);
                                        
        if (tResult < 0) {
            return tResult;
        }

        if (this.wMode & ARM_2D_CP_MODE_FILL) {                               /*!< fill mode */
            /*! draw the partial header */
            /*
            HOW IT WORKS:

            source tile (with negative relative location)
            +------------------+------------------------------------+-- ... --+
            |                  |    IGNORED HEADER PART             | repeat  |
            |  +---------------+------------------------------------+-- ... --+
            |  |               |////////////////////////////////////|/////////|
            |  |Clipped Region |////ACTUAL DRAWING PART/////////////|/repeat//|
            |  |               |////////////////////////////////////|/////////|
            +--+---------------+------------------------------------+-- ... --+
            |                                                                 |
            |                                                                 |
            +---------------------------------------------------------- ... --+
            */

            if  (tClippdRegion.tSize.iWidth < tDrawRegion.tSize.iWidth) {
                //! something left to draw

                arm_2d_region_t tHeaderRegion = tDrawRegion;
                tHeaderRegion.tSize.iWidth  = tDrawRegion.tSize.iWidth 
                                            - tClippdRegion.tSize.iWidth;
                tHeaderRegion.tLocation.iX += tClippdRegion.tSize.iWidth;
                tHeaderRegion.tSize.iHeight = tClippdRegion.tSize.iHeight 
                                            - tTargetRegion.tLocation.iY;
                tHeaderRegion.tLocation.iY = tTargetRegion.tLocation.iY;

                tResult = __tile_clipped_pave(  &this,
                                                ptTarget,
                                                &tHeaderRegion,
                                                NULL,
                                                this.wMode);
                                    
                //arm_2d_region_t tClippedRegion;
                if (tResult < 0) {
                    return tResult;
                }
            }



            /*! draw the partial column */
            /*
            HOW IT WORKS:

            source tile (with negative relative location)
            +------------------------------------+
            |                                    |
            |     +------------------------------+-- ... --+
            |     |                              |         |
            |     |                              |         |
            |     |                              |         |
            +-----+------------------------------+         |
            |     |//////////////////////////////|         |
            |     |//////////////////////////////|         |
            |     |////ACTUAL DRAWING COLUMN/////|         |
            |     |//////////////////////////////|         |
            |     |//////////////////////////////|         |
            +-----+------------------------------+         |
            |     |//////////////////////////////|         |
           ...   ...///repeat////////////////////|         |
            |     |//////////////////////////////|         |
            +-----+------------------------------+-- ... --+
            */

            if  (tClippdRegion.tSize.iHeight < tDrawRegion.tSize.iHeight) {
                //! something left to draw

                arm_2d_region_t tFirstColumnRegion = tDrawRegion;
                tFirstColumnRegion.tSize.iHeight = tDrawRegion.tSize.iHeight - tClippdRegion.tSize.iHeight;
                tFirstColumnRegion.tLocation.iY += tClippdRegion.tSize.iHeight;
                tFirstColumnRegion.tSize.iWidth = tClippdRegion.tSize.iWidth 
                                                - tTargetRegion.tLocation.iX;
                tFirstColumnRegion.tLocation.iX = tTargetRegion.tLocation.iX;

                tResult = __tile_clipped_pave(  &this,
                                                ptTarget,
                                                &tFirstColumnRegion,
                                                NULL,
                                                this.wMode);
                                    
                if (tResult < 0) {
                    return tResult;
                }
            }


            /*! draw the normal non-negitive part */
            /*
            HOW IT WORKS:

            source tile (with negative relative location)
            +------------------------------------+
            |                                    |
            |     +------------------------------+-- ... --+
            |     |                              |         |
            |     |                              |         |
            |     |                              |         |
            +-----+------------------------------+-- ... --|
                  |                              |/////////|
                  |                              |/////////|
                 ...                            ...///////...
                  |                              |/////////|
                  |                              |/////////|
                  +------------------------------+-- ... --+
            */

            if (    (tClippdRegion.tSize.iWidth < tDrawRegion.tSize.iWidth)
                &&  (tClippdRegion.tSize.iHeight < tDrawRegion.tSize.iHeight)) {

                arm_2d_region_t tNonNegRegion = tDrawRegion;
                tNonNegRegion.tSize.iWidth = tDrawRegion.tSize.iWidth - tClippdRegion.tSize.iWidth;
                tNonNegRegion.tSize.iHeight = tDrawRegion.tSize.iHeight - tClippdRegion.tSize.iHeight;
                tNonNegRegion.tLocation.iX += tClippdRegion.tSize.iWidth;
                tNonNegRegion.tLocation.iY += tClippdRegion.tSize.iHeight;

                tResult = __tile_non_negtive_location_pave( &this,
                                                            this.Source.ptTile,
                                                            
                                                            //! [Modify][Target.ptTile]
                                                            //this.Target.ptTile,
                                                            //this.use_as__arm_2d_op_core_t.Runtime.ptTargetTile,
                                                            ptTarget,
                                                            &tNonNegRegion,
                                                            this.wMode);
                if (tResult < 0) {
                    return tResult;
                }
            }
        }
    } else {
        tResult = __tile_non_negtive_location_pave( &this,
                                                    this.Source.ptTile,
                                                    
                                                    //! [Modify][Target.ptTile]
                                                    //this.Target.ptTile,
                                                    //this.use_as__arm_2d_op_core_t.Runtime.ptTargetTile,
                                                    ptTarget,
                                                    &tTargetRegion,
                                                    this.wMode);
    }

    return tResult;
}


/*----------------------------------------------------------------------------*
 * Frontend                                                                   *
 *----------------------------------------------------------------------------*/

arm_fsm_rt_t __arm_2d_op_depose(arm_2d_op_core_t *ptThis, 
                                arm_fsm_rt_t tResult)
{
    //arm_irq_safe 
    {
        this.tResult = tResult;
        this.Status.tValue = 
            ((tResult < 0) ? __ARM_2D_OP_STATUS_IO_ERROR_msk : 0)       |
            __ARM_2D_OP_STATUS_CPL_msk                                  ;
    }
    return tResult;
}

__WEAK
arm_fsm_rt_t __arm_2d_op_frontend_on_leave( arm_2d_op_core_t *ptThis, 
                                            arm_fsm_rt_t tResult)
{
    return __arm_2d_op_depose(ptThis, tResult);
}

static void __arm_2d_op_use_default_frame_buffer(arm_2d_op_core_t *ptOP)
{
    ARM_2D_IMPL(arm_2d_op_t, ptOP);
    
    if ((intptr_t)-1 == (intptr_t)this.Target.ptTile) {
        this.Target.ptTile = ARM_2D_CTRL.ptDefaultFrameBuffer;
    }
}

 
arm_fsm_rt_t __arm_2d_op_frontend_op_decoder(arm_2d_op_core_t *ptThis)
{
    arm_fsm_rt_t tResult;
    
    //! decode operation
    switch (this.ptOp->Info.Param.chValue & 
                (   ARM_2D_OP_INFO_PARAM_HAS_SOURCE 
                //|   ARM_2D_OP_INFO_PARAM_HAS_ORIGIN
                |   ARM_2D_OP_INFO_PARAM_HAS_TARGET
                //|   ARM_2D_OP_INFO_PARAM_HAS_SOURCE_MASK
                //|   ARM_2D_OP_INFO_PARAM_HAS_TARGET_MASK
                )) {
                
    case (   ARM_2D_OP_INFO_PARAM_HAS_SOURCE 
         |   ARM_2D_OP_INFO_PARAM_HAS_TARGET):
        __arm_2d_op_use_default_frame_buffer(ptThis);
        /* operaion process target tile based on source tile */
        tResult = __arm_2d_op_frontend_region_process_with_src(ptThis);
        break;
        
    case ARM_2D_OP_INFO_PARAM_HAS_TARGET:
        __arm_2d_op_use_default_frame_buffer(ptThis);
        /* operation which only processes target tile */
        tResult = __arm_2d_op_frontend_region_process(ptThis);
        break;
        
    default:
        /* control operation */
        tResult = __arm_2d_op_frontend_control(ptThis);
    }
    
    if (ARM_2D_ERR_OUT_OF_REGION == tResult) {                                 
        if (ARM_2D_RUNTIME_FEATURE.TREAT_OUT_OF_RANGE_AS_COMPLETE) {
            //! nothing to draw
            tResult = arm_fsm_rt_cpl;
        } 
    }
    
    return tResult;
}


__WEAK
/*! 
 * \brief wait asynchronouse operation complete
 * \retval true sync up with operation
 * \retval false operation is busy
 */
bool arm_2d_op_wait_async(arm_2d_op_core_t *ptOP)
{
    return true;
}


__WEAK
/*! \brief sync up with operation 
 *! \retval true operation is busy
 *! \retval false operation isn't busy
 */
bool __arm_2d_op_acquire(arm_2d_op_core_t *ptOP)
{
    ARM_2D_IMPL(arm_2d_op_core_t, ptOP)
    bool bResult = !this.Status.bIsBusy;
    
    arm_irq_safe {
        if (bResult) {
            //! initialize operation
            do {
                this.tResult = arm_fsm_rt_async;
                this.Status.tValue = 0;                                                 //! reset status
                this.Status.bIsBusy = true;                                             //! set busy flag
            } while(0);
        }
    }

    return bResult;
}

__WEAK
arm_fsm_rt_t __arm_2d_op_frontend(arm_2d_op_core_t *ptThis)
{
    arm_fsm_rt_t tResult;
    
    tResult = __arm_2d_op_frontend_op_decoder(ptThis);
    
    return  __arm_2d_op_frontend_on_leave(ptThis, tResult);
}

arm_fsm_rt_t __arm_2d_op_invoke(arm_2d_op_core_t *ptOP)
{
    ARM_2D_IMPL(arm_2d_op_core_t, ptOP)
    
#if 0
    if (this.Status.bIsBusy) {
        return arm_fsm_rt_on_going;
    }

    //! initialize operation
    do {
        this.tResult = arm_fsm_rt_async;
        this.Status.tValue = 0;                                                 //! reset status
        this.Status.bIsBusy = true;                                             //! set busy flag
    } while(0);
#endif

    return  __arm_2d_op_frontend(ptThis);
}


/*! 
 *  \brief initialise the arm-2d core service
 */
void __arm_2d_init(void)
{
    /*! initialise arm-2d control block */
    memset(&ARM_2D_CTRL, 0, sizeof(struct __arm_2d_op_control));
}

/*!
 * \brief set the default frame buffer
 * \param ptFrameBuffer the new frame buffer, 
 * \note  Passing NULL means using no default framebuffer
 * \return arm_2d_tile_t* the address of the old frame buffer
 */
arm_2d_tile_t *arm_2d_set_default_frame_buffer(const arm_2d_tile_t *ptFrameBuffer)
{
    arm_2d_tile_t *ptOldBuffer = NULL;
    
    arm_irq_safe {
        ptOldBuffer = ARM_2D_CTRL.ptDefaultFrameBuffer;
        ARM_2D_CTRL.ptDefaultFrameBuffer = (arm_2d_tile_t *)ptFrameBuffer;
    }
    
    return ptOldBuffer;
}

/*! 
 * \brief get the default frame buffer
 * \return arm_2d_tile_t* the address of the default frame buffer
 */
arm_2d_tile_t *arm_2d_get_default_frame_buffer(void)
{
    return ARM_2D_CTRL.ptDefaultFrameBuffer;
}

/*! 
 * \brief attach a user param (which could be a pointer) to specified OP
 * \param ptOP the address of the target OP (NULL means using the default OP)
 * \param pUserParam a user parameter (it can be used as a pointer)
 */
void arm_2d_set_user_param(arm_2d_op_core_t *ptOP, uintptr_t pUserParam)
{
    ARM_2D_IMPL(arm_2d_op_core_t, ptOP);
    
    this.pUserParam = pUserParam;
}

/*! 
   \brief get the status of a specified OP, 
   \param ptOP the address of the target OP (NULL means using the default OP)
   \return arm_2d_op_status_t the operation status
 */
/*
    usually, it is used after calling arm_2d_op_wait_async(). 
    E.g.
    //! wait for previous operation complete
    do {
        arm_2d_op_wait_async();
        arm_2d_op_status_t tStatus = arm_2d_get_op_status();
        if (tStatus.bIOError) {
            //! error detected
            ...
        } else if (tStatus.bOpCpl) {
            break;
        }
    } while(true);
 */
arm_2d_op_status_t arm_2d_get_op_status(arm_2d_op_core_t *ptOP)
{
    ARM_2D_IMPL(arm_2d_op_core_t, ptOP)

    volatile arm_2d_op_status_t *ptStatus 
        = (volatile arm_2d_op_status_t *)&(this.Status);
        
    return *ptStatus;
}


__WEAK
/*! 
 * \brief arm-2d pixel pipeline task entery
 * \note  This function is *TRHEAD-SAFE*
 * \param ptTask the address of an arm-2d task control block
 * \retval arm_fsm_rt_cpl The sub-task FIFO is empty, the caller can wait for a 
 *         semaphore set by arm_2d_notif_sub_task_fifo_task_arrive()
 * \retval arm_fsm_rt_on_going The arm_2d_task yields 
 * \retval arm_fsm_rt_async You shouldn't see this value
 * \retval arm_fsm_rt_wait_for_obj hardware accelerator wants to sync-up with applications.
 * \retval (<0) Serious error is detected.
 */
arm_fsm_rt_t arm_2d_task(arm_2d_task_t *ptTask)
{
    ARM_2D_UNUSED(ptTask);
    return arm_fsm_rt_cpl;
}

/*----------------------------------------------------------------------------*
 * Utilieis                                                                   *
 *----------------------------------------------------------------------------*/

arm_2d_err_t  __arm_mask_validate(  const arm_2d_tile_t *ptSource,
                                    const arm_2d_tile_t *ptSrcMask,
                                    const arm_2d_tile_t *ptTarget,
                                    const arm_2d_tile_t *ptDesMask,
                                    uint32_t wMode)
{
    ARM_2D_UNUSED(wMode);
    
    if (NULL != ptSrcMask) {
        //! valid source mask tile
        if (0 == ptSrcMask->bHasEnforcedColour) {
            return ARM_2D_ERR_INVALID_PARAM;
        } else if ( (ARM_2D_COLOUR_SZ_8BIT != ptSrcMask->tColourInfo.u3ColourSZ)
        #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
               &&   (ARM_2D_CHANNEL_8in32 != ptSrcMask->tColourInfo.chScheme)
        #endif
               ) {
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
        #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
               &&   (ARM_2D_CHANNEL_8in32 != ptDesMask->tColourInfo.chScheme)
        #endif
               ) {
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


/*----------------------------------------------------------------------------*
 * Low Level IO Interfaces                                                    *
 *----------------------------------------------------------------------------*/

const __arm_2d_op_info_t ARM_2D_OP_BARRIER = {
    .Info = {
        //! control operation
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_BARRIER,
        
        .LowLevelIO = {
            .ptCopyLike = NULL, 
            .ptFillLike = NULL,
        },
    },
};
    

#ifdef   __cplusplus
}
#endif

#endif
