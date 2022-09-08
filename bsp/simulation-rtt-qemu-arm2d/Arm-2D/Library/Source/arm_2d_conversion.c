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
 * Title:        arm-2d_draw.c
 * Description:  APIs for colour format conversion
 *
 * $Date:        08. Aug 2022
 * $Revision:    V.1.0.2
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
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wswitch-enum"
#   pragma clang diagnostic ignored "-Wswitch"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

void __arm_2d_impl_gray8_to_rgb565( uint8_t *__RESTRICT pchSourceBase,
                                    int16_t iSourceStride,
                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize);

void __arm_2d_impl_cccn888_to_rgb565(uint32_t *__RESTRICT pwSource,
                                    int16_t iSourceStride,
                                    uint16_t *__RESTRICT phwTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize);

void __arm_2d_impl_rgb565_to_cccn888(uint16_t *__RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize);

void __arm_2d_impl_gray8_to_cccn888(uint8_t *__RESTRICT pchSourceBase,
                                    int16_t iSourceStride,
                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize);

void __arm_2d_impl_rgb565_to_gray8( uint16_t *__RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize);

void __arm_2d_impl_cccn888_to_gray8(uint32_t *__RESTRICT pwSource,
                                    int16_t iSourceStride,
                                    uint8_t *__RESTRICT pchwTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize);
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


/*----------------------------------------------------------------------------*
 * Convert Colour format                                                      *
 *----------------------------------------------------------------------------*/

/*!
 * \brief convert the colour format of a given tile to rgb888
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[out] ptTarget the output tile (holding a buffer)
 * \return arm_fsm_rt_t the operation result
 */
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_convert_colour_to_rgb888(  arm_2d_op_cl_convt_t *ptOP,
                                                const arm_2d_tile_t *ptSource,
                                                const arm_2d_tile_t *ptTarget)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_cl_convt_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_CONVERT_TO_RGB888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = NULL;
    this.Source.ptTile = ptSource;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

/*!
 * \brief convert the colour format of a given tile to rgb565
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[out] ptTarget the output tile (holding a buffer)
 * \return arm_fsm_rt_t the operation result
 */
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_convert_colour_to_rgb565(  arm_2d_op_cl_convt_t *ptOP,
                                                const arm_2d_tile_t *ptSource,
                                                const arm_2d_tile_t *ptTarget)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_cl_convt_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_CONVERT_TO_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = NULL;
    this.Source.ptTile = ptSource;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

/*!
 * \brief convert the colour format of a given tile to gray8
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[out] ptTarget the output tile (holding a buffer)
 * \return arm_fsm_rt_t the operation result
 */
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_convert_colour_to_gray8(   arm_2d_op_cl_convt_t *ptOP,
                                                const arm_2d_tile_t *ptSource,
                                                const arm_2d_tile_t *ptTarget)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_cl_convt_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_CONVERT_TO_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = NULL;
    this.Source.ptTile = ptSource;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

arm_fsm_rt_t __arm_2d_sw_convert_colour_to_gray8(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cl_convt_t, ptTask->ptOP);

    if (!this.Source.ptTile->bHasEnforcedColour) {
        return (arm_fsm_rt_t)ARM_2D_ERR_MISSING_PARAM;
    }


    switch ( this.Source.ptTile->tInfo.tColourInfo.u3ColourSZ) {
        case ARM_2D_COLOUR_8BIT:
            /* no need to convert, return cpl directly */
            break;
        case ARM_2D_COLOUR_SZ_16BIT:
            __arm_2d_impl_rgb565_to_gray8(  ptTask->Param.tCopy.tSource.pBuffer,
                                            ptTask->Param.tCopy.tSource.iStride,
                                            ptTask->Param.tCopy.tTarget.pBuffer,
                                            ptTask->Param.tCopy.tTarget.iStride,
                                            &(ptTask->Param.tCopy.tCopySize));
            break;
        case ARM_2D_COLOUR_SZ_32BIT:
            __arm_2d_impl_cccn888_to_gray8( ptTask->Param.tCopy.tSource.pBuffer,
                                            ptTask->Param.tCopy.tSource.iStride,
                                            ptTask->Param.tCopy.tTarget.pBuffer,
                                            ptTask->Param.tCopy.tTarget.iStride,
                                            &(ptTask->Param.tCopy.tCopySize));
            break;
        default:
            return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    }

    return arm_fsm_rt_cpl;
}


arm_fsm_rt_t __arm_2d_sw_convert_colour_to_rgb565(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cl_convt_t, ptTask->ptOP);

    if (!this.Source.ptTile->bHasEnforcedColour) {
        return (arm_fsm_rt_t)ARM_2D_ERR_MISSING_PARAM;
    }


    switch ( this.Source.ptTile->tInfo.tColourInfo.u3ColourSZ) {
        case ARM_2D_COLOUR_8BIT:
            __arm_2d_impl_gray8_to_rgb565(  ptTask->Param.tCopy.tSource.pBuffer,
                                            ptTask->Param.tCopy.tSource.iStride,
                                            ptTask->Param.tCopy.tTarget.pBuffer,
                                            ptTask->Param.tCopy.tTarget.iStride,
                                            &(ptTask->Param.tCopy.tCopySize));
            break;
        case ARM_2D_COLOUR_SZ_16BIT:
            /* no need to convert, return cpl directly */
            break;
        case ARM_2D_COLOUR_SZ_32BIT:
            __arm_2d_impl_cccn888_to_rgb565( ptTask->Param.tCopy.tSource.pBuffer,
                                            ptTask->Param.tCopy.tSource.iStride,
                                            ptTask->Param.tCopy.tTarget.pBuffer,
                                            ptTask->Param.tCopy.tTarget.iStride,
                                            &(ptTask->Param.tCopy.tCopySize));
            break;
        default:
            return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_sw_convert_colour_to_rgb888(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cl_convt_t, ptTask->ptOP);

    if (!this.Source.ptTile->bHasEnforcedColour) {
        return (arm_fsm_rt_t)ARM_2D_ERR_MISSING_PARAM;
    }


    switch ( this.Source.ptTile->tInfo.tColourInfo.u3ColourSZ) {
        case ARM_2D_COLOUR_8BIT:
            __arm_2d_impl_gray8_to_cccn888(  ptTask->Param.tCopy.tSource.pBuffer,
                                            ptTask->Param.tCopy.tSource.iStride,
                                            ptTask->Param.tCopy.tTarget.pBuffer,
                                            ptTask->Param.tCopy.tTarget.iStride,
                                            &(ptTask->Param.tCopy.tCopySize));
            break;
        case ARM_2D_COLOUR_SZ_16BIT:
            __arm_2d_impl_rgb565_to_cccn888( ptTask->Param.tCopy.tSource.pBuffer,
                                            ptTask->Param.tCopy.tSource.iStride,
                                            ptTask->Param.tCopy.tTarget.pBuffer,
                                            ptTask->Param.tCopy.tTarget.iStride,
                                            &(ptTask->Param.tCopy.tCopySize));
            break;
        case ARM_2D_COLOUR_SZ_32BIT:
            /* no need to convert, return cpl directly */
            break;
        default:
            return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    }

    return arm_fsm_rt_cpl;
}

/*----------------------------------------------------------------------------*
 * Accelerable Low Level APIs                                                 *
 *----------------------------------------------------------------------------*/

__WEAK
void __arm_2d_impl_gray8_to_rgb565( uint8_t *__RESTRICT pchSourceBase,
                                    int16_t iSourceStride,
                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint8_t *__RESTRICT pchSource = pchSourceBase;
        uint16_t *__RESTRICT phwTarget = phwTargetBase;

        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++) {
            __arm_2d_color_fast_rgb_t      hwTargetPixel;

            uint_fast8_t chColour = *pchSource++;

            hwTargetPixel.R = chColour;   
            hwTargetPixel.G = chColour;  
            hwTargetPixel.B = chColour;   

            *phwTarget++ = __arm_2d_rgb565_pack(&hwTargetPixel);
        }

        pchSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_cccn888_to_rgb565(uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,
                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint32_t *__RESTRICT pwSource = pwSourceBase;
        uint16_t *__RESTRICT phwTarget = phwTargetBase;

        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++) {
            arm_2d_color_bgra8888_t wSrcPixel;
            __arm_2d_color_fast_rgb_t      hwTargetPixel;

            wSrcPixel.tValue = *pwSource++;
            hwTargetPixel.R = wSrcPixel.u8R;
            hwTargetPixel.G = wSrcPixel.u8G;
            hwTargetPixel.B = wSrcPixel.u8B;

            *phwTarget++ = __arm_2d_rgb565_pack(&hwTargetPixel);
        }

        pwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_gray8_to_cccn888(uint8_t *__RESTRICT pchSourceBase,
                                    int16_t iSourceStride,
                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint8_t *__RESTRICT pchSource = pchSourceBase;
        uint32_t       *__RESTRICT pwTarget = pwTargetBase;

        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++) {

            uint_fast8_t chPixel = *pchSource++;
            arm_2d_color_bgra8888_t wTargetPixel;

            wTargetPixel.u8R = chPixel;
            wTargetPixel.u8G = chPixel;
            wTargetPixel.u8B = chPixel;
            wTargetPixel.u8A = 0xFF;
            *pwTarget++ = wTargetPixel.tValue;
        }

        pchSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_rgb565_to_cccn888(uint16_t *__RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint16_t *__RESTRICT phwSource = phwSourceBase;
        uint32_t       *__RESTRICT pwTarget = pwTargetBase;

        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++) {
            __arm_2d_color_fast_rgb_t      hwSrcPixel;
            arm_2d_color_bgra8888_t wTargetPixel;

            __arm_2d_rgb565_unpack(*phwSource++, &hwSrcPixel);
            wTargetPixel.u8R = hwSrcPixel.R;
            wTargetPixel.u8G = hwSrcPixel.G;
            wTargetPixel.u8B = hwSrcPixel.B;
            wTargetPixel.u8A = 0xFF;
            *pwTarget++ = wTargetPixel.tValue;
        }

        phwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;
    }
}



__WEAK
void __arm_2d_impl_rgb565_to_gray8( uint16_t *__RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint16_t *__RESTRICT phwSource = phwSourceBase;
        uint8_t       *__RESTRICT pchTarget = pchTargetBase;

        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++) {
            __arm_2d_color_fast_rgb_t      hwSrcPixel;
            uint_fast16_t hwPixel = 0;

            __arm_2d_rgb565_unpack(*phwSource++, &hwSrcPixel);
            hwPixel += hwSrcPixel.R;
            hwPixel += hwSrcPixel.G;
            hwPixel += hwSrcPixel.B;

            *pchTarget++ = hwPixel / 3;
        }

        phwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_cccn888_to_gray8(uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,
                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint32_t *__RESTRICT pwSource = pwSourceBase;
        uint8_t *__RESTRICT pchTarget = pchTargetBase;

        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++) {
            arm_2d_color_bgra8888_t wSrcPixel;
            uint_fast16_t hwPixel = 0;

            wSrcPixel.tValue = *pwSource++;
            hwPixel += wSrcPixel.u8R;
            hwPixel += wSrcPixel.u8G;
            hwPixel += wSrcPixel.u8B;

            *pchTarget++ = hwPixel / 3;
        }

        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;
    }
}

/*----------------------------------------------------------------------------*
 * Low Level IO Interfaces                                                    *
 *----------------------------------------------------------------------------*/
__WEAK
def_low_lv_io(  __ARM_2D_IO_COLOUR_CONVERT_TO_GRAY8, 
                __arm_2d_sw_convert_colour_to_gray8);
__WEAK
def_low_lv_io(  __ARM_2D_IO_COLOUR_CONVERT_TO_RGB565, 
                __arm_2d_sw_convert_colour_to_rgb565);
__WEAK
def_low_lv_io(  __ARM_2D_IO_COLOUR_CONVERT_TO_RGB888, 
                __arm_2d_sw_convert_colour_to_rgb888);


const __arm_2d_op_info_t ARM_2D_OP_CONVERT_TO_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COLOUR_FORMAT_CONVERSION,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COLOUR_CONVERT_TO_GRAY8),
            .ptFillLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_CONVERT_TO_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COLOUR_FORMAT_CONVERSION,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COLOUR_CONVERT_TO_RGB565),
            .ptFillLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_CONVERT_TO_RGB888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COLOUR_FORMAT_CONVERSION,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COLOUR_CONVERT_TO_RGB888),
            .ptFillLike = NULL,
        },
    },
};

#ifdef   __cplusplus
}
#endif
