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
 * Title:        __arm_2d_copy_helium.inc
 * Description:  c code template for copy like operations
 *
 * $Date:        12 July 2022
 * $Revision:    v1.0.0
 *
 * -------------------------------------------------------------------- */

#ifndef __API_COLOUR
#   error You have to define __API_COLOUR before using this c template
#endif
#ifndef __API_INT_TYPE
#   error You have to define the __API_INT_TYPE before using this c template
#endif
#ifndef __API_INT_TYPE_BIT_NUM
#   error You have to define the __API_INT_TYPE_BIT_NUM before using this c template
#endif

#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#define ____ARM_2D_FUNC(__NAME, __COLOUR)  __MVE_WRAPPER(__arm_2d_impl_##__COLOUR##_##__NAME)
#define ___ARM_2D_FUNC(__NAME, __COLOUR)   ____ARM_2D_FUNC(__NAME, __COLOUR)
#define __ARM_2D_FUNC(__NAME)   ___ARM_2D_FUNC(__NAME, __API_COLOUR)


#undef ____ARM_2D_TYPE
#undef ___ARM_2D_TYPE
#undef __ARM_2D_TYPE
#define ____ARM_2D_TYPE(__NAME, __COLOUR)  arm_2d_##__COLOUR##_##__NAME
#define ___ARM_2D_TYPE(__NAME, __COLOUR)   ____ARM_2D_TYPE(__NAME, __COLOUR)
#define __ARM_2D_TYPE(__NAME)   ___ARM_2D_TYPE(__NAME, __API_COLOUR)




/*----------------------------------------------------------------------------*
 * Paving                                                                     *
 *----------------------------------------------------------------------------*/

__OVERRIDE_WEAK
void __ARM_2D_FUNC(2x2_paving)(const __API_INT_TYPE *__RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                const arm_2d_size_t * __RESTRICT ptSourceSize,
                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                uint16_t tilePairRows,
                                uint16_t tilePairCols)
{
    __ARM_2D_PAVING_2x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}


__OVERRIDE_WEAK
void __ARM_2D_FUNC(2x2_paving_x_mirror)(const __API_INT_TYPE *__RESTRICT pSourceBase,
                                        int16_t iSourceStride,
                                        const arm_2d_size_t * __RESTRICT ptSourceSize,
                                        __API_INT_TYPE * __RESTRICT pTargetBase,
                                        int16_t iTargetStride,
                                        uint16_t tilePairRows,
                                        uint16_t tilePairCols)
{
    __ARM_2D_PAVING_2x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR,
                        PAVING_X_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}


__OVERRIDE_WEAK
void __ARM_2D_FUNC(2x2_paving_y_mirror)(const __API_INT_TYPE *__RESTRICT pSourceBase,
                                        int16_t iSourceStride,
                                        const arm_2d_size_t * __RESTRICT ptSourceSize,
                                        __API_INT_TYPE * __RESTRICT pTargetBase,
                                        int16_t iTargetStride,
                                        uint16_t tilePairRows,
                                        uint16_t tilePairCols)
{
    __ARM_2D_PAVING_2x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(2x2_paving_xy_mirror)(const __API_INT_TYPE *__RESTRICT pSourceBase,
                                            int16_t iSourceStride,
                                            const arm_2d_size_t * __RESTRICT ptSourceSize,
                                            __API_INT_TYPE * __RESTRICT pTargetBase,
                                            int16_t iTargetStride,
                                            uint16_t tilePairRows,
                                            uint16_t tilePairCols)
{
    __ARM_2D_PAVING_2x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}


__OVERRIDE_WEAK

void __ARM_2D_FUNC(1x2_paving)( const __API_INT_TYPE * __RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                const arm_2d_size_t * __RESTRICT ptSourceSize,
                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                uint32_t destWidth,
                                uint16_t tilePairRows)
{
    __ARM_2D_PAVING_1x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(1x2_paving_x_mirror)(const __API_INT_TYPE * __RESTRICT pSourceBase,
                                        int16_t iSourceStride,
                                        const arm_2d_size_t * __RESTRICT ptSourceSize,
                                        __API_INT_TYPE * __RESTRICT pTargetBase,
                                        int16_t iTargetStride,
                                        uint32_t destWidth,
                                        uint16_t tilePairRows)
{
    __ARM_2D_PAVING_1x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR,
                        PAVING_X_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(1x2_paving_y_mirror)(const __API_INT_TYPE * __RESTRICT pSourceBase,
                                       int16_t iSourceStride,
                                       const arm_2d_size_t * __RESTRICT ptSourceSize,
                                       __API_INT_TYPE * __RESTRICT pTargetBase,
                                       int16_t iTargetStride,
                                       uint32_t destWidth,
                                       uint16_t tilePairRows)
{
    __ARM_2D_PAVING_1x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}


__OVERRIDE_WEAK
void __ARM_2D_FUNC(1x2_paving_xy_mirror)(const __API_INT_TYPE * __RESTRICT pSourceBase,
                                       int16_t iSourceStride,
                                       const arm_2d_size_t * __RESTRICT ptSourceSize,
                                       __API_INT_TYPE * __RESTRICT pTargetBase,
                                       int16_t iTargetStride,
                                       uint32_t destWidth,
                                       uint16_t tilePairRows)
{
    __ARM_2D_PAVING_1x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}


__OVERRIDE_WEAK

void __ARM_2D_FUNC(2x1_paving)( const __API_INT_TYPE * __RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                const arm_2d_size_t * __RESTRICT ptSourceSize,
                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                uint16_t tilePairCols,
                                uint16_t destHeight)
{
    __ARM_2D_PAVING_2x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(2x1_paving_x_mirror)(const __API_INT_TYPE * __RESTRICT pSourceBase,
                                        int16_t iSourceStride,
                                        const arm_2d_size_t * __RESTRICT ptSourceSize,
                                        __API_INT_TYPE * __RESTRICT pTargetBase,
                                        int16_t iTargetStride,
                                        uint16_t tilePairCols,
                                        uint16_t destHeight)
{
    __ARM_2D_PAVING_2x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR,
                        PAVING_X_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(2x1_paving_y_mirror)(const __API_INT_TYPE * __RESTRICT pSourceBase,
                                        int16_t iSourceStride,
                                        const arm_2d_size_t * __RESTRICT ptSourceSize,
                                        __API_INT_TYPE * __RESTRICT pTargetBase,
                                        int16_t iTargetStride,
                                        uint16_t tilePairCols,
                                        uint16_t destHeight)
{
    __ARM_2D_PAVING_2x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}


__OVERRIDE_WEAK
void __ARM_2D_FUNC(2x1_paving_xy_mirror)(const __API_INT_TYPE * __RESTRICT pSourceBase,
                                        int16_t iSourceStride,
                                        const arm_2d_size_t * __RESTRICT ptSourceSize,
                                        __API_INT_TYPE * __RESTRICT pTargetBase,
                                        int16_t iTargetStride,
                                        uint16_t tilePairCols,
                                        uint16_t destHeight)
{
    __ARM_2D_PAVING_2x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}



__OVERRIDE_WEAK

void __ARM_2D_FUNC(1x1_paving)(const __API_INT_TYPE * __RESTRICT pSource,
                               int16_t iSourceStride,
                               __API_INT_TYPE * __RESTRICT pTarget,
                               int16_t iTargetStride,
                               const arm_2d_size_t * __RESTRICT ptSrcCopySize,
                               const arm_2d_size_t * __RESTRICT ptDstCopySize)
{
    __ARM_2D_PAVING_1x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(1x1_paving_x_mirror)(
                                        const __API_INT_TYPE * __RESTRICT pSource,
                                        int16_t iSourceStride,
                                        __API_INT_TYPE * __RESTRICT pTarget,
                                        int16_t iTargetStride,
                                        const arm_2d_size_t * __RESTRICT ptSrcCopySize,
                                        const arm_2d_size_t * __RESTRICT ptDstCopySize)
{
    __ARM_2D_PAVING_1x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR, PAVING_X_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}


__OVERRIDE_WEAK
void __ARM_2D_FUNC(1x1_paving_y_mirror)(
                                        const __API_INT_TYPE * __RESTRICT pSource,
                                        int16_t iSourceStride,
                                        __API_INT_TYPE * __RESTRICT pTarget,
                                        int16_t iTargetStride,
                                        const arm_2d_size_t * __RESTRICT ptSrcCopySize,
                                        const arm_2d_size_t * __RESTRICT ptDstCopySize)
{
    __ARM_2D_PAVING_1x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSrcCopySize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}


__OVERRIDE_WEAK
void __ARM_2D_FUNC(1x1_paving_xy_mirror)(const __API_INT_TYPE * __RESTRICT pSource,
                                           int16_t iSourceStride,
                                           __API_INT_TYPE * __RESTRICT pTarget,
                                           int16_t iTargetStride,
                                           const arm_2d_size_t * __RESTRICT ptSrcCopySize,
                                           const arm_2d_size_t * __RESTRICT ptDstCopySize)
{
    __ARM_2D_PAVING_1x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSrcCopySize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM));
}


/*----------------------------------------------------------------------------*
 * Paving with colour masking                                                 *
 *----------------------------------------------------------------------------*/
__OVERRIDE_WEAK

void __ARM_2D_FUNC(cl_key_2x2_paving)(const __API_INT_TYPE *__RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                const arm_2d_size_t * __RESTRICT ptSourceSize,
                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                uint16_t tilePairRows,
                                uint16_t tilePairCols,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_2x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}


__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_2x2_paving_x_mirror)(
                                const __API_INT_TYPE *__RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                const arm_2d_size_t * __RESTRICT ptSourceSize,
                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                uint16_t tilePairRows,
                                uint16_t tilePairCols,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_2x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR,
                        PAVING_X_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}


__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_2x2_paving_y_mirror)(
                                const __API_INT_TYPE *__RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                const arm_2d_size_t * __RESTRICT ptSourceSize,
                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                uint16_t tilePairRows,
                                uint16_t tilePairCols,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_2x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_2x2_paving_xy_mirror)(
                                const __API_INT_TYPE *__RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                const arm_2d_size_t * __RESTRICT ptSourceSize,
                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                uint16_t tilePairRows,
                                uint16_t tilePairCols,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_2x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}


__OVERRIDE_WEAK

void __ARM_2D_FUNC(cl_key_1x2_paving)(
                                const __API_INT_TYPE * __RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                const arm_2d_size_t * __RESTRICT ptSourceSize,
                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                uint32_t destWidth,
                                uint16_t tilePairRows,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_1x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_1x2_paving_x_mirror)(
                                const __API_INT_TYPE * __RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                const arm_2d_size_t * __RESTRICT ptSourceSize,
                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                uint32_t destWidth,
                                uint16_t tilePairRows,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_1x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR,
                        PAVING_X_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_1x2_paving_y_mirror)(
                                const __API_INT_TYPE * __RESTRICT pSourceBase,
                               int16_t iSourceStride,
                               const arm_2d_size_t * __RESTRICT ptSourceSize,
                               __API_INT_TYPE * __RESTRICT pTargetBase,
                               int16_t iTargetStride,
                               uint32_t destWidth,
                               uint16_t tilePairRows,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_1x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}


__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_1x2_paving_xy_mirror)(
                                const __API_INT_TYPE * __RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                const arm_2d_size_t * __RESTRICT ptSourceSize,
                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                uint32_t destWidth,
                                uint16_t tilePairRows,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_1x2(__API_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}


__OVERRIDE_WEAK

void __ARM_2D_FUNC(cl_key_2x1_paving)(
                                const __API_INT_TYPE * __RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                const arm_2d_size_t * __RESTRICT ptSourceSize,
                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                uint16_t tilePairCols,
                                uint16_t destHeight,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_2x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_2x1_paving_x_mirror)(
                                const __API_INT_TYPE * __RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                const arm_2d_size_t * __RESTRICT ptSourceSize,
                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                uint16_t tilePairCols,
                                uint16_t destHeight,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_2x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR,
                        PAVING_X_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_2x1_paving_y_mirror)(
                                const __API_INT_TYPE * __RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                const arm_2d_size_t * __RESTRICT ptSourceSize,
                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                uint16_t tilePairCols,
                                uint16_t destHeight,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_2x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}


__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_2x1_paving_xy_mirror)(
                                const __API_INT_TYPE * __RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                const arm_2d_size_t * __RESTRICT ptSourceSize,
                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                uint16_t tilePairCols,
                                uint16_t destHeight,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_2x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}



__OVERRIDE_WEAK

void __ARM_2D_FUNC(cl_key_1x1_paving)(
                                const __API_INT_TYPE * __RESTRICT pSource,
                                int16_t iSourceStride,
                                __API_INT_TYPE * __RESTRICT pTarget,
                                int16_t iTargetStride,
                                const arm_2d_size_t * __RESTRICT ptSrcCopySize,
                                const arm_2d_size_t * __RESTRICT ptDstCopySize,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_1x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_1x1_paving_x_mirror)(
                                const __API_INT_TYPE * __RESTRICT pSource,
                                int16_t iSourceStride,
                                __API_INT_TYPE * __RESTRICT pTarget,
                                int16_t iTargetStride,
                                const arm_2d_size_t * __RESTRICT ptSrcCopySize,
                                const arm_2d_size_t * __RESTRICT ptDstCopySize,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_1x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR,
                        PAVING_X_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}


__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_1x1_paving_y_mirror)(
                                const __API_INT_TYPE * __RESTRICT pSource,
                                int16_t iSourceStride,
                                __API_INT_TYPE * __RESTRICT pTarget,
                                int16_t iTargetStride,
                                const arm_2d_size_t * __RESTRICT ptSrcCopySize,
                                const arm_2d_size_t * __RESTRICT ptDstCopySize,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_1x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSrcCopySize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}


__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_1x1_paving_xy_mirror)(const __API_INT_TYPE * __RESTRICT pSource,
                               int16_t iSourceStride,
                               __API_INT_TYPE * __RESTRICT pTarget,
                               int16_t iTargetStride,
                               const arm_2d_size_t * __RESTRICT ptSrcCopySize,
                               const arm_2d_size_t * __RESTRICT ptDstCopySize,
                                __API_INT_TYPE Colour)
{
    __ARM_2D_PAVING_1x1(__API_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSrcCopySize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_INT_TYPE_BIT_NUM),
                        CMP_CL_MSK(__API_INT_TYPE_BIT_NUM, Colour));
}



/*----------------------------------------------------------------------------*
 * Copy with colour masking                                                   *
 *----------------------------------------------------------------------------*/



__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_copy)(
                            __API_INT_TYPE *__RESTRICT pSource,
                            int16_t iSourceStride,
                            __API_INT_TYPE *__RESTRICT pTarget,
                            int16_t iTargetStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            __API_INT_TYPE Colour)
{
    __ARM_2D_FUNC(cl_key_1x1_paving)(   pSource,
                                        iSourceStride,
                                        pTarget,
                                        iTargetStride,
                                        ptCopySize,
                                        ptCopySize,
                                        Colour);
}


__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_copy_y_mirror)(
                                        __API_INT_TYPE * __RESTRICT pSource,
                                        int16_t iSourceStride,
                                        __API_INT_TYPE * __RESTRICT pTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t * __RESTRICT ptCopySize,
                                        __API_INT_TYPE Colour)
{
    __ARM_2D_FUNC(cl_key_1x1_paving_y_mirror)(  pSource,
                                                iSourceStride,
                                                pTarget,
                                                iTargetStride,
                                                ptCopySize,
                                                ptCopySize,
                                                Colour);
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_copy_x_mirror)(
                                        __API_INT_TYPE * __RESTRICT pSource,
                                        int16_t iSourceStride,
                                        __API_INT_TYPE * __RESTRICT pTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t * __RESTRICT ptCopySize,
                                        __API_INT_TYPE Colour)
{
    __ARM_2D_FUNC(cl_key_1x1_paving_x_mirror)(  pSource,
                                                iSourceStride,
                                                pTarget,
                                                iTargetStride,
                                                ptCopySize,
                                                ptCopySize,
                                                Colour);
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(cl_key_copy_xy_mirror)(
                                        __API_INT_TYPE * __RESTRICT pSource,
                                        int16_t iSourceStride,
                                        __API_INT_TYPE * __RESTRICT pTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t * __RESTRICT ptCopySize,
                                        __API_INT_TYPE Colour)
{
    __ARM_2D_FUNC(cl_key_1x1_paving_xy_mirror)( pSource,
                                                iSourceStride,
                                                pTarget,
                                                iTargetStride,
                                                ptCopySize,
                                                ptCopySize,
                                                Colour);

}

/*----------------------------------------------------------------------------*
 * Copy                                                                       *
 *----------------------------------------------------------------------------*/

#if __API_INT_TYPE_BIT_NUM == 8

__OVERRIDE_WEAK
void __ARM_2D_FUNC(copy)(   __API_INT_TYPE *__RESTRICT pSource,
                            int16_t iSourceStride,
                            __API_INT_TYPE *__RESTRICT pTarget,
                            int16_t iTargetStride,
                            arm_2d_size_t *__RESTRICT ptCopySize)
{
    __ARM_2D_FUNC(1x1_paving)( pSource,
                               iSourceStride,
                               pTarget,
                               iTargetStride,
                               ptCopySize,
                               ptCopySize);
}

#endif

/*----------------------------------------------------------------------------*
 * Copy with mirroring                                                        *
 *----------------------------------------------------------------------------*/

__OVERRIDE_WEAK
void __ARM_2D_FUNC(copy_y_mirror)(  __API_INT_TYPE * __RESTRICT pSource,
                                    int16_t iSourceStride,
                                    __API_INT_TYPE * __RESTRICT pTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptCopySize)
{
    __ARM_2D_FUNC(1x1_paving_y_mirror)( pSource,
                                        iSourceStride,
                                        pTarget,
                                        iTargetStride,
                                        ptCopySize,
                                        ptCopySize);
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(copy_x_mirror)(__API_INT_TYPE * __RESTRICT pSource,
                                  int16_t iSourceStride,
                                  __API_INT_TYPE * __RESTRICT pTarget,
                                  int16_t iTargetStride,
                                  arm_2d_size_t * __RESTRICT ptCopySize)
{
    __ARM_2D_FUNC(1x1_paving_x_mirror)( pSource,
                                        iSourceStride,
                                        pTarget,
                                        iTargetStride,
                                        ptCopySize,
                                        ptCopySize);
}

__OVERRIDE_WEAK
void __ARM_2D_FUNC(copy_xy_mirror)( __API_INT_TYPE * __RESTRICT pSource,
                                    int16_t iSourceStride,
                                    __API_INT_TYPE * __RESTRICT pTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptCopySize)
{
    __ARM_2D_FUNC(1x1_paving_xy_mirror)(pSource,
                                        iSourceStride,
                                        pTarget,
                                        iTargetStride,
                                        ptCopySize,
                                        ptCopySize);

}



#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#undef __API_COLOUR
#undef __API_INT_TYPE
#undef __API_INT_TYPE_BIT_NUM
#undef ____ARM_2D_TYPE
#undef ___ARM_2D_TYPE
#undef __ARM_2D_TYPE

