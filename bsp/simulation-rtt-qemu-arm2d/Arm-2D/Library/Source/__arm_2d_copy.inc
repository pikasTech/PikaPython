
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
 * Title:        __arm_2d_copy.inc
 * Description:  c code template for copy like operations
 *
 * $Date:        14. April 2020
 * $Revision:    V.1.0.0
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
#define ____ARM_2D_FUNC(__NAME, __COLOUR)  __arm_2d_impl_##__COLOUR##_##__NAME
#define ___ARM_2D_FUNC(__NAME, __COLOUR)   ____ARM_2D_FUNC(__NAME, __COLOUR) 
#define __ARM_2D_FUNC(__NAME)   ___ARM_2D_FUNC(__NAME, __API_COLOUR)


#undef ____ARM_2D_TYPE
#undef ___ARM_2D_TYPE
#undef __ARM_2D_TYPE
#define ____ARM_2D_TYPE(__NAME, __COLOUR)  arm_2d_##__COLOUR##_##__NAME
#define ___ARM_2D_TYPE(__NAME, __COLOUR)   ____ARM_2D_TYPE(__NAME, __COLOUR) 
#define __ARM_2D_TYPE(__NAME)   ___ARM_2D_TYPE(__NAME, __API_COLOUR)



/*----------------------------------------------------------------------------*
 * Accelerable Low Level APIs                                                 *
 *----------------------------------------------------------------------------*/

#if 0
/*----------------------------------------------------------------------------*
 * Fill with mirroring                                                        *
 *----------------------------------------------------------------------------*/

void __ARM_2D_FUNC(fill_x_mirror)(  __API_INT_TYPE * __RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t * __RESTRICT ptSourceSize,
                                    __API_INT_TYPE * __RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptTargetSize);

void __ARM_2D_FUNC(fill_y_mirror)( __API_INT_TYPE * __RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t * __RESTRICT ptSourceSize,
                                    __API_INT_TYPE * __RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptTargetSize);

void __ARM_2D_FUNC(fill_xy_mirror)( __API_INT_TYPE * __RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t * __RESTRICT ptSourceSize,
                                    __API_INT_TYPE * __RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptTargetSize);

static
void __ARM_2D_FUNC(fill_generic)(   
                        const __API_INT_TYPE * __RESTRICT pSourceBase,
                        int16_t iSourceStride,
                        const arm_2d_size_t * __RESTRICT ptSourceSize,
                        __API_INT_TYPE * __RESTRICT pTargetBase,
                        int16_t iTargetStride,
                        const arm_2d_size_t * __RESTRICT ptTargetSize,
                        const __ARM_2D_TYPE(paving_fct_t) * __RESTRICT pPavFct)
{
    __ARM_2D_MEM_FILL_GENERIC(  pSourceBase, 
                                iSourceStride,
                                ptSourceSize, 
                                pTargetBase, 
                                iTargetStride, 
                                ptTargetSize,
                                pPavFct);
}


/*----------------------------------------------------------------------------*
 * Copy with mirroring                                                        *
 *----------------------------------------------------------------------------*/
void __ARM_2D_FUNC(copy_x_mirror)( __API_INT_TYPE *__RESTRICT phwSource,
                                    int16_t iSourceStride,
                                    __API_INT_TYPE *__RESTRICT phwTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize);   
                                        
void __ARM_2D_FUNC(copy_y_mirror)( __API_INT_TYPE *__RESTRICT phwSource,
                                    int16_t iSourceStride,
                                    __API_INT_TYPE *__RESTRICT phwTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize);

void __ARM_2D_FUNC(copy_xy_mirror)( __API_INT_TYPE *__RESTRICT phwSource,
                                    int16_t iSourceStride,
                                    __API_INT_TYPE *__RESTRICT phwTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize);
                                    
/*----------------------------------------------------------------------------*
 * Paving                                                                     *
 *----------------------------------------------------------------------------*/
__WEAK 
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


__WEAK void __ARM_2D_FUNC(2x2_paving_x_mirror)(const __API_INT_TYPE *__RESTRICT pSourceBase,
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


__WEAK void __ARM_2D_FUNC(2x2_paving_y_mirror)(const __API_INT_TYPE *__RESTRICT pSourceBase,
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

__WEAK void __ARM_2D_FUNC(2x2_paving_xy_mirror)(const __API_INT_TYPE *__RESTRICT pSourceBase,
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


__WEAK 
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

__WEAK void __ARM_2D_FUNC(1x2_paving_x_mirror)(const __API_INT_TYPE * __RESTRICT pSourceBase,
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

__WEAK void __ARM_2D_FUNC(1x2_paving_y_mirror)(const __API_INT_TYPE * __RESTRICT pSourceBase,
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


__WEAK void __ARM_2D_FUNC(1x2_paving_xy_mirror)(const __API_INT_TYPE * __RESTRICT pSourceBase,
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


__WEAK 
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

__WEAK void __ARM_2D_FUNC(2x1_paving_x_mirror)(const __API_INT_TYPE * __RESTRICT pSourceBase,
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

__WEAK void __ARM_2D_FUNC(2x1_paving_y_mirror)(const __API_INT_TYPE * __RESTRICT pSourceBase,
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


__WEAK void __ARM_2D_FUNC(2x1_paving_xy_mirror)(const __API_INT_TYPE * __RESTRICT pSourceBase,
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



__WEAK 
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

__WEAK void __ARM_2D_FUNC(1x1_paving_x_mirror)(
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


__WEAK void __ARM_2D_FUNC(1x1_paving_y_mirror)(
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


__WEAK void __ARM_2D_FUNC(1x1_paving_xy_mirror)(const __API_INT_TYPE * __RESTRICT pSource,
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
 * Copy                                                                       *
 *----------------------------------------------------------------------------*/


__WEAK
void __ARM_2D_FUNC(copy)(   __API_INT_TYPE *__RESTRICT pSource,
                            int16_t iSourceStride,
                            __API_INT_TYPE *__RESTRICT pTarget,
                            int16_t iTargetStride,
                            arm_2d_size_t *__RESTRICT ptCopySize)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
        memcpy(pTarget, pSource, ptCopySize->iWidth * sizeof(*pSource));
        pSource += iSourceStride;
        pTarget += iTargetStride;
    }
}


__WEAK
void __ARM_2D_FUNC(copy_mirror)( __API_INT_TYPE * __RESTRICT pSource,
                                 int16_t iSourceStride,
                                 __API_INT_TYPE * __RESTRICT pTarget,
                                 int16_t iTargetStride,
                                 arm_2d_size_t * __RESTRICT ptCopySize,
                                 uint32_t wMode)
{
    switch (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
      case ARM_2D_CP_MODE_X_MIRROR:
          __ARM_2D_FUNC(copy_x_mirror)( pSource, iSourceStride,
                                        pTarget, iTargetStride, 
                                        ptCopySize);
          break;
      case ARM_2D_CP_MODE_Y_MIRROR:
          __ARM_2D_FUNC(copy_y_mirror)( pSource, iSourceStride,
                                        pTarget, iTargetStride, 
                                        ptCopySize);
          break;
      case ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR:
          __ARM_2D_FUNC(copy_xy_mirror)(pSource, iSourceStride,
                                        pTarget, iTargetStride, 
                                        ptCopySize);
          break;
      default:
          assert(false);        /*! this should not happen */
          //break;
    }
}

__WEAK 
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

__WEAK 
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

__WEAK 
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



/*----------------------------------------------------------------------------*
 * Fill                                                                       *
 *----------------------------------------------------------------------------*/
__WEAK 
void __ARM_2D_FUNC(fill)(__API_INT_TYPE * __RESTRICT pSourceBase,
                         int16_t iSourceStride,
                         arm_2d_size_t * __RESTRICT ptSourceSize,
                         __API_INT_TYPE * __RESTRICT pTargetBase,
                         int16_t iTargetStride,
                         arm_2d_size_t * __RESTRICT ptTargetSize)
{
    static const __ARM_2D_TYPE(paving_fct_t) pav_func = {
        .pav_2x2 = &__ARM_2D_FUNC(2x2_paving),
        .pav_1x2 = &__ARM_2D_FUNC(1x2_paving),
        .pav_2x1 = &__ARM_2D_FUNC(2x1_paving),
        .pav_1x1 = &__ARM_2D_FUNC(1x1_paving)
    };

    __ARM_2D_FUNC(fill_generic)(pSourceBase,
                                iSourceStride,
                                ptSourceSize,
                                pTargetBase,
                                iTargetStride, 
                                ptTargetSize, 
                                &pav_func);
}

__WEAK
void __ARM_2D_FUNC(fill_mirror)( __API_INT_TYPE * __RESTRICT pSourceBase,
                                 int16_t iSourceStride,
                                 arm_2d_size_t * __RESTRICT ptSourceSize,
                                 __API_INT_TYPE * __RESTRICT pTargetBase,
                                 int16_t iTargetStride,
                                 arm_2d_size_t * __RESTRICT ptTargetSize,
                                 uint32_t wMode)
{
    switch (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
        case ARM_2D_CP_MODE_X_MIRROR:
            __ARM_2D_FUNC(fill_x_mirror)(
                    pSourceBase, iSourceStride, ptSourceSize,
                    pTargetBase, iTargetStride, ptTargetSize);
            break;
        case ARM_2D_CP_MODE_Y_MIRROR:
            __ARM_2D_FUNC(fill_y_mirror)(
                    pSourceBase, iSourceStride, ptSourceSize,
                    pTargetBase, iTargetStride, ptTargetSize);
            break;
        case ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR :
            __ARM_2D_FUNC(fill_xy_mirror)(
                    pSourceBase, iSourceStride, ptSourceSize, 
                    pTargetBase, iTargetStride, ptTargetSize);
            break;
        default:
            assert(false);  /*! this should not happen */
            //break;
    }
}

__WEAK 
void __ARM_2D_FUNC(fill_x_mirror)(__API_INT_TYPE * __RESTRICT pSourceBase,
                                      int16_t iSourceStride,
                                      arm_2d_size_t * __RESTRICT ptSourceSize,
                                      __API_INT_TYPE * __RESTRICT pTargetBase,
                                      int16_t iTargetStride,
                                      arm_2d_size_t * __RESTRICT ptTargetSize)
{
    static __ARM_2D_TYPE(paving_fct_t) pav_func = {
        .pav_2x2 = &__ARM_2D_FUNC(2x2_paving_x_mirror),
        .pav_1x2 = &__ARM_2D_FUNC(1x2_paving_x_mirror),
        .pav_2x1 = &__ARM_2D_FUNC(2x1_paving_x_mirror),
        .pav_1x1 = &__ARM_2D_FUNC(1x1_paving_x_mirror)
    };

    __ARM_2D_FUNC(fill_generic)(pSourceBase,
                                iSourceStride,
                                ptSourceSize,
                                pTargetBase,
                                iTargetStride, ptTargetSize, &pav_func);
}

__WEAK void __ARM_2D_FUNC(fill_y_mirror)(__API_INT_TYPE * __RESTRICT pSourceBase,
                                      int16_t iSourceStride,
                                      arm_2d_size_t * __RESTRICT ptSourceSize,
                                      __API_INT_TYPE * __RESTRICT pTargetBase,
                                      int16_t iTargetStride,
                                      arm_2d_size_t * __RESTRICT ptTargetSize)
{
    static __ARM_2D_TYPE(paving_fct_t) pav_func = {
        .pav_2x2 = &__ARM_2D_FUNC(2x2_paving_y_mirror),
        .pav_1x2 = &__ARM_2D_FUNC(1x2_paving_y_mirror),
        .pav_2x1 = &__ARM_2D_FUNC(2x1_paving_y_mirror),
        .pav_1x1 = &__ARM_2D_FUNC(1x1_paving_y_mirror)
    };

    __ARM_2D_FUNC(fill_generic)(pSourceBase,
                                iSourceStride,
                                ptSourceSize,
                                pTargetBase,
                                iTargetStride, ptTargetSize, &pav_func);
}


__WEAK void __ARM_2D_FUNC(fill_xy_mirror)(__API_INT_TYPE * __RESTRICT pSourceBase,
                                       int16_t iSourceStride,
                                       arm_2d_size_t * __RESTRICT ptSourceSize,
                                       __API_INT_TYPE * __RESTRICT pTargetBase,
                                       int16_t iTargetStride,
                                       arm_2d_size_t * __RESTRICT ptTargetSize)
{
    static __ARM_2D_TYPE(paving_fct_t) pav_func = {
        .pav_2x2 = &__ARM_2D_FUNC(2x2_paving_xy_mirror),
        .pav_1x2 = &__ARM_2D_FUNC(1x2_paving_xy_mirror),
        .pav_2x1 = &__ARM_2D_FUNC(2x1_paving_xy_mirror),
        .pav_1x1 = &__ARM_2D_FUNC(1x1_paving_xy_mirror)
    };

    __ARM_2D_FUNC(fill_generic)(pSourceBase,
                                iSourceStride,
                                ptSourceSize,
                                pTargetBase,
                                iTargetStride, ptTargetSize, &pav_func);
}

/*----------------------------------------------------------------------------*
 * Copy with colour maksing and mirroring                                     *
 *----------------------------------------------------------------------------*/
void __ARM_2D_FUNC(copy_colour_keying_x_mirror)( 
                                    __API_INT_TYPE *__RESTRICT phwSource,
                                    int16_t iSourceStride,
                                    __API_INT_TYPE *__RESTRICT phwTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    __API_INT_TYPE Colour);   
                                        
void __ARM_2D_FUNC(copy_colour_keying_y_mirror)( 
                                    __API_INT_TYPE *__RESTRICT phwSource,
                                    int16_t iSourceStride,
                                    __API_INT_TYPE *__RESTRICT phwTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    __API_INT_TYPE Colour);   

void __ARM_2D_FUNC(copy_colour_keying_xy_mirror)( 
                                    __API_INT_TYPE *__RESTRICT phwSource,
                                    int16_t iSourceStride,
                                    __API_INT_TYPE *__RESTRICT phwTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    __API_INT_TYPE Colour);   
                                        


/*----------------------------------------------------------------------------*
 * Fill with colour maksing and mirroring                                     *
 *----------------------------------------------------------------------------*/
 
 void __ARM_2D_FUNC(fill_colour_keying_x_mirror)(  
                                    __API_INT_TYPE * __RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t * __RESTRICT ptSourceSize,
                                    __API_INT_TYPE * __RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptTargetSize,
                                    __API_INT_TYPE Colour);

void __ARM_2D_FUNC(fill_colour_keying_y_mirror)( 
                                    __API_INT_TYPE * __RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t * __RESTRICT ptSourceSize,
                                    __API_INT_TYPE * __RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptTargetSize,
                                    __API_INT_TYPE Colour);

void __ARM_2D_FUNC(fill_colour_keying_xy_mirror)( 
                                    __API_INT_TYPE * __RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t * __RESTRICT ptSourceSize,
                                    __API_INT_TYPE * __RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptTargetSize,
                                    __API_INT_TYPE Colour);

static
void __ARM_2D_FUNC(fill_colour_keying_generic)(   
                const __API_INT_TYPE * __RESTRICT pSourceBase,
                int16_t iSourceStride,
                const arm_2d_size_t * __RESTRICT ptSourceSize,
                __API_INT_TYPE * __RESTRICT pTargetBase,
                int16_t iTargetStride,
                const arm_2d_size_t * __RESTRICT ptTargetSize,
                const __ARM_2D_TYPE(cl_key_paving_fct_t) * __RESTRICT pPavFct,
                uint_fast16_t hwColour)
{
    __ARM_2D_MEM_FILL_GENERIC(  pSourceBase, 
                                iSourceStride,
                                ptSourceSize, 
                                pTargetBase, 
                                iTargetStride, 
                                ptTargetSize,
                                pPavFct,
                                hwColour);
}


/*----------------------------------------------------------------------------*
 * Paving with colour masking                                                 *
 *----------------------------------------------------------------------------*/
__WEAK 
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
                        (in != Colour));
}


__WEAK void __ARM_2D_FUNC(cl_key_2x2_paving_x_mirror)(
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
                        (in != Colour));
}


__WEAK void __ARM_2D_FUNC(cl_key_2x2_paving_y_mirror)(
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
                        (in != Colour));
}

__WEAK void __ARM_2D_FUNC(cl_key_2x2_paving_xy_mirror)(
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
                        (in != Colour));
}


__WEAK 
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
                        (in != Colour));
}

__WEAK void __ARM_2D_FUNC(cl_key_1x2_paving_x_mirror)(
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
                        (in != Colour));
}

__WEAK void __ARM_2D_FUNC(cl_key_1x2_paving_y_mirror)(
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
                        (in != Colour));
}


__WEAK void __ARM_2D_FUNC(cl_key_1x2_paving_xy_mirror)(
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
                        (in != Colour));
}


__WEAK 
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
                        (in != Colour));
}

__WEAK void __ARM_2D_FUNC(cl_key_2x1_paving_x_mirror)(
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
                        (in != Colour));
}

__WEAK void __ARM_2D_FUNC(cl_key_2x1_paving_y_mirror)(
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
                        (in != Colour));
}


__WEAK void __ARM_2D_FUNC(cl_key_2x1_paving_xy_mirror)(
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
                        (in != Colour));
}



__WEAK 
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
                        (in != Colour));
}

__WEAK void __ARM_2D_FUNC(cl_key_1x1_paving_x_mirror)(
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
                        (in != Colour));
}


__WEAK void __ARM_2D_FUNC(cl_key_1x1_paving_y_mirror)(
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
                        (in != Colour));
}


__WEAK void __ARM_2D_FUNC(cl_key_1x1_paving_xy_mirror)(const __API_INT_TYPE * __RESTRICT pSource,
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
                        (in != Colour));
}


/*----------------------------------------------------------------------------*
 * Copy with colour masking                                                   *
 *----------------------------------------------------------------------------*/


__WEAK
void __ARM_2D_FUNC(cl_key_copy)(   
                            __API_INT_TYPE *__RESTRICT pSource,
                            int16_t iSourceStride,
                            __API_INT_TYPE *__RESTRICT pTarget,
                            int16_t iTargetStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            __API_INT_TYPE Colour)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++){
            if (pSource[x] == Colour) {
                continue;
            }
            pTarget[x] = pSource[x];
        }
        pSource += iSourceStride;
        pTarget += iTargetStride;
    }
}


__WEAK
void __ARM_2D_FUNC(cl_key_copy_mirror)( 
                                        __API_INT_TYPE * __RESTRICT pSource,
                                        int16_t iSourceStride,
                                        __API_INT_TYPE * __RESTRICT pTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t * __RESTRICT ptCopySize,
                                        uint32_t wMode,
                                        __API_INT_TYPE Colour)
{
    switch (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
      case ARM_2D_CP_MODE_X_MIRROR:
          __ARM_2D_FUNC(copy_colour_keying_x_mirror)( 
                                        pSource, iSourceStride,
                                        pTarget, iTargetStride, 
                                        ptCopySize,
                                        Colour);
          break;
      case ARM_2D_CP_MODE_Y_MIRROR:
          __ARM_2D_FUNC(copy_colour_keying_y_mirror)( pSource, iSourceStride,
                                        pTarget, iTargetStride, 
                                        ptCopySize,
                                        Colour);
          break;
      case ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR:
          __ARM_2D_FUNC(copy_colour_keying_xy_mirror)(pSource, iSourceStride,
                                        pTarget, iTargetStride, 
                                        ptCopySize,
                                        Colour);
          break;
      default:
          assert(false);        /*! this should not happen */
          //break;
    }
}

__WEAK 
void __ARM_2D_FUNC(copy_colour_keying_y_mirror)(   
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

__WEAK 
void __ARM_2D_FUNC(copy_colour_keying_x_mirror)(
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

__WEAK 
void __ARM_2D_FUNC(copy_colour_keying_xy_mirror)( 
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
 * Fill with Colour Masking                                                   *
 *----------------------------------------------------------------------------*/

__WEAK 
void __ARM_2D_FUNC(cl_key_fill)(__API_INT_TYPE * __RESTRICT pSourceBase,
                         int16_t iSourceStride,
                         arm_2d_size_t * __RESTRICT ptSourceSize,
                         __API_INT_TYPE * __RESTRICT pTargetBase,
                         int16_t iTargetStride,
                         arm_2d_size_t * __RESTRICT ptTargetSize,
                         __API_INT_TYPE Colour)
{

    static const __ARM_2D_TYPE(cl_key_paving_fct_t) pav_func = {
        .pav_2x2 = &__ARM_2D_FUNC(cl_key_2x2_paving),
        .pav_1x2 = &__ARM_2D_FUNC(cl_key_1x2_paving),
        .pav_2x1 = &__ARM_2D_FUNC(cl_key_2x1_paving),
        .pav_1x1 = &__ARM_2D_FUNC(cl_key_1x1_paving),
    };

    __ARM_2D_FUNC(fill_colour_keying_generic)( pSourceBase,
                                                iSourceStride,
                                                ptSourceSize,
                                                pTargetBase,
                                                iTargetStride, 
                                                ptTargetSize, 
                                                &pav_func,
                                                Colour);

}

__WEAK
void __ARM_2D_FUNC(cl_key_fill_mirror)( 
                                 __API_INT_TYPE * __RESTRICT pSourceBase,
                                 int16_t iSourceStride,
                                 arm_2d_size_t * __RESTRICT ptSourceSize,
                                 __API_INT_TYPE * __RESTRICT pTargetBase,
                                 int16_t iTargetStride,
                                 arm_2d_size_t * __RESTRICT ptTargetSize,
                                 uint32_t wMode,
                                 __API_INT_TYPE Colour)
{
    switch (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
        case ARM_2D_CP_MODE_X_MIRROR:
            __ARM_2D_FUNC(fill_colour_keying_x_mirror)(
                    pSourceBase, iSourceStride, ptSourceSize,
                    pTargetBase, iTargetStride, ptTargetSize,
                    Colour);
            break;
        case ARM_2D_CP_MODE_Y_MIRROR:
            __ARM_2D_FUNC(fill_colour_keying_y_mirror)(
                    pSourceBase, iSourceStride, ptSourceSize,
                    pTargetBase, iTargetStride, ptTargetSize,
                    Colour);
            break;
        case ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR :
            __ARM_2D_FUNC(fill_colour_keying_xy_mirror)(
                    pSourceBase, iSourceStride, ptSourceSize, 
                    pTargetBase, iTargetStride, ptTargetSize,
                    Colour);
            break;
        default:
            assert(false);  /*! this should not happen */
            //break;
    }
}

__WEAK 
void __ARM_2D_FUNC(fill_colour_keying_x_mirror)(__API_INT_TYPE * __RESTRICT pSourceBase,
                                      int16_t iSourceStride,
                                      arm_2d_size_t * __RESTRICT ptSourceSize,
                                      __API_INT_TYPE * __RESTRICT pTargetBase,
                                      int16_t iTargetStride,
                                      arm_2d_size_t * __RESTRICT ptTargetSize,
                                    __API_INT_TYPE Colour)
{
    static __ARM_2D_TYPE(cl_key_paving_fct_t) pav_func = {
        .pav_2x2 = &__ARM_2D_FUNC(cl_key_2x2_paving_x_mirror),
        .pav_1x2 = &__ARM_2D_FUNC(cl_key_1x2_paving_x_mirror),
        .pav_2x1 = &__ARM_2D_FUNC(cl_key_2x1_paving_x_mirror),
        .pav_1x1 = &__ARM_2D_FUNC(cl_key_1x1_paving_x_mirror)
    };

    __ARM_2D_FUNC(fill_colour_keying_generic)( pSourceBase,
                                                iSourceStride,
                                                ptSourceSize,
                                                pTargetBase,
                                                iTargetStride, 
                                                ptTargetSize, 
                                                &pav_func,
                                                Colour);
}

__WEAK void __ARM_2D_FUNC(fill_colour_keying_y_mirror)(
                                        __API_INT_TYPE * __RESTRICT pSourceBase,
                                        int16_t iSourceStride,
                                        arm_2d_size_t * __RESTRICT ptSourceSize,
                                        __API_INT_TYPE * __RESTRICT pTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t * __RESTRICT ptTargetSize,
                                        __API_INT_TYPE Colour)
{
    static __ARM_2D_TYPE(cl_key_paving_fct_t) pav_func = {
        .pav_2x2 = &__ARM_2D_FUNC(cl_key_2x2_paving_y_mirror),
        .pav_1x2 = &__ARM_2D_FUNC(cl_key_1x2_paving_y_mirror),
        .pav_2x1 = &__ARM_2D_FUNC(cl_key_2x1_paving_y_mirror),
        .pav_1x1 = &__ARM_2D_FUNC(cl_key_1x1_paving_y_mirror)
    };

    __ARM_2D_FUNC(fill_colour_keying_generic)( pSourceBase,
                                                iSourceStride,
                                                ptSourceSize,
                                                pTargetBase,
                                                iTargetStride, 
                                                ptTargetSize, 
                                                &pav_func,
                                                Colour);
}


__WEAK void __ARM_2D_FUNC(fill_colour_keying_xy_mirror)(
                                        __API_INT_TYPE * __RESTRICT pSourceBase,
                                        int16_t iSourceStride,
                                        arm_2d_size_t * __RESTRICT ptSourceSize,
                                        __API_INT_TYPE * __RESTRICT pTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t * __RESTRICT ptTargetSize,
                                        __API_INT_TYPE Colour)
{
    static __ARM_2D_TYPE(cl_key_paving_fct_t) pav_func = {
        .pav_2x2 = &__ARM_2D_FUNC(cl_key_2x2_paving_xy_mirror),
        .pav_1x2 = &__ARM_2D_FUNC(cl_key_1x2_paving_xy_mirror),
        .pav_2x1 = &__ARM_2D_FUNC(cl_key_2x1_paving_xy_mirror),
        .pav_1x1 = &__ARM_2D_FUNC(cl_key_1x1_paving_xy_mirror)
    };

    __ARM_2D_FUNC(fill_colour_keying_generic)( pSourceBase,
                                                iSourceStride,
                                                ptSourceSize,
                                                pTargetBase,
                                                iTargetStride, 
                                                ptTargetSize, 
                                                &pav_func,
                                                Colour);
}

#endif


/*----------------------------------------------------------------------------*
 * Copy and Fill                                                              *
 *----------------------------------------------------------------------------*/

#define __API_MCAF_COLOUR                   __API_COLOUR
#define __API_MCAF_INT_TYPE                 __API_INT_TYPE
#define __API_MCAF_INT_TYPE_BIT_NUM         __API_INT_TYPE_BIT_NUM


#include "__arm_2d_meta_copy_and_fill.inc"


__WEAK
void __ARM_2D_FUNC(copy)(   __API_INT_TYPE *__RESTRICT pSource,
                            int16_t iSourceStride,
                            __API_INT_TYPE *__RESTRICT pTarget,
                            int16_t iTargetStride,
                            arm_2d_size_t *__RESTRICT ptCopySize)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
        memcpy(pTarget, pSource, ptCopySize->iWidth * sizeof(*pSource));
        pSource += iSourceStride;
        pTarget += iTargetStride;
    }
}



/*----------------------------------------------------------------------------*
 * Copy and Fill with Colour Masking                                          *
 *----------------------------------------------------------------------------*/

#define __API_MCAF_COLOUR                   __API_COLOUR
#define __API_MCAF_INT_TYPE                 __API_INT_TYPE
#define __API_MCAF_INT_TYPE_BIT_NUM         __API_INT_TYPE_BIT_NUM

#define __API_MCAF_OP_NAME                  cl_key
#define __API_MCAF_EXTRA_PARAM              ,__API_MCAF_INT_TYPE Colour
#define __API_MCAF_EXTRA_EXPRESSION         Colour
#define __API_MCAF_EXTRA_PAVE_EXPRESSION    (in != Colour)

#include "__arm_2d_meta_copy_and_fill.inc"

__WEAK
void __ARM_2D_FUNC(cl_key_copy)(   
                            __API_INT_TYPE *__RESTRICT pSource,
                            int16_t iSourceStride,
                            __API_INT_TYPE *__RESTRICT pTarget,
                            int16_t iTargetStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            __API_INT_TYPE Colour)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++){
            if (pSource[x] == Colour) {
                continue;
            }
            pTarget[x] = pSource[x];
        }
        pSource += iSourceStride;
        pTarget += iTargetStride;
    }
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
