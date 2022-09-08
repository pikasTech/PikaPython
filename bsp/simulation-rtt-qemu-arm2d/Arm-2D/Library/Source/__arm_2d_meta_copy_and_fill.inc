
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
 * Title:        __arm_2d_meta_copy_and_fill.inc
 * Description:  c code template for copy and fill like operations
 *
 * $Date:        13. Sept 2021
 * $Revision:    V.1.0.0
 *
 * -------------------------------------------------------------------- */
 
#ifndef __API_MCAF_COLOUR
#   error You have to define __API_MCAF_COLOUR before using this c template
#endif
#ifndef __API_MCAF_INT_TYPE
#   error You have to define the __API_MCAF_INT_TYPE before using this c template
#endif
#ifndef __API_MCAF_INT_TYPE_BIT_NUM
#   error You have to define the __API_MCAF_INT_TYPE_BIT_NUM before using this c template
#endif

#ifndef __API_MCAF_EXTRA_PARAM
#   define __API_MCAF_EXTRA_PARAM
#endif


#undef ____MCAF_FUNC
#undef ___MCAF_FUNC
#undef __MCAF_FUNC


#ifndef __API_MCAF_OP_NAME
#   define ____MCAF_FUNC(__NAME, __COLOUR)                                      \
        __arm_2d_impl_##__COLOUR##_##__NAME
#   define ___MCAF_FUNC(__NAME, __COLOUR)   ____MCAF_FUNC(__NAME, __COLOUR) 
#else
#   define _____MCAF_FUNC(__OP_NAME, __NAME, __COLOUR)                          \
        __arm_2d_impl_##__COLOUR##_##__OP_NAME##_##__NAME
#   define ____MCAF_FUNC(__OP_NAME, __NAME, __COLOUR)                           \
        _____MCAF_FUNC(__OP_NAME, __NAME, __COLOUR)
#   define ___MCAF_FUNC(__NAME, __COLOUR)                                       \
        ____MCAF_FUNC(__API_MCAF_OP_NAME, __NAME, __COLOUR)
#endif



#define __MCAF_FUNC(__NAME)   ___MCAF_FUNC(__NAME, __API_MCAF_COLOUR)


#undef ____MCAF_TYPE
#undef ___MCAF_TYPE
#undef __MCAF_TYPE

#ifndef __API_MCAF_OP_NAME
#   define ____MCAF_TYPE(__NAME, __COLOUR)  arm_2d_##__COLOUR##_##__NAME
#   define ___MCAF_TYPE(__NAME, __COLOUR)   ____MCAF_TYPE(__NAME, __COLOUR) 
#else
#   define _____MCAF_TYPE(__OP_NAME, __NAME, __COLOUR)                          \
        arm_2d_##__COLOUR##_##__OP_NAME##_##__NAME
#   define ____MCAF_TYPE(__OP_NAME, __NAME, __COLOUR)                           \
        _____MCAF_TYPE(__OP_NAME, __NAME, __COLOUR)
#   define ___MCAF_TYPE(__NAME, __COLOUR)                                       \
        ____MCAF_TYPE(__API_MCAF_OP_NAME, __NAME, __COLOUR) 
#endif


#define __MCAF_TYPE(__NAME)   ___MCAF_TYPE(__NAME, __API_MCAF_COLOUR)

/*----------------------------------------------------------------------------*
 * Accelerable Low Level APIs                                                 *
 *----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*
 * Fill with mirroring                                                        *
 *----------------------------------------------------------------------------*/

void __MCAF_FUNC(fill_x_mirror)(  __API_MCAF_INT_TYPE * __RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t * __RESTRICT ptSourceSize,
                                    __API_MCAF_INT_TYPE * __RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptTargetSize
                                    
                                    __API_MCAF_EXTRA_PARAM
                                    );

void __MCAF_FUNC(fill_y_mirror)( __API_MCAF_INT_TYPE * __RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t * __RESTRICT ptSourceSize,
                                    __API_MCAF_INT_TYPE * __RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptTargetSize
                                    
                                    __API_MCAF_EXTRA_PARAM
                                    );

void __MCAF_FUNC(fill_xy_mirror)( __API_MCAF_INT_TYPE * __RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t * __RESTRICT ptSourceSize,
                                    __API_MCAF_INT_TYPE * __RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptTargetSize
                                    
                                    __API_MCAF_EXTRA_PARAM
                                    );

static
void __MCAF_FUNC(fill_generic)(   
                        const __API_MCAF_INT_TYPE * __RESTRICT pSourceBase,
                        int16_t iSourceStride,
                        const arm_2d_size_t * __RESTRICT ptSourceSize,
                        __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                        int16_t iTargetStride,
                        const arm_2d_size_t * __RESTRICT ptTargetSize,
                        const __MCAF_TYPE(paving_fct_t) * __RESTRICT pPavFct
                                    
                        __API_MCAF_EXTRA_PARAM
                        )
{
#ifndef __API_MCAF_EXTRA_EXPRESSION
    __ARM_2D_MEM_FILL_GENERIC(  pSourceBase, 
                                iSourceStride,
                                ptSourceSize, 
                                pTargetBase, 
                                iTargetStride, 
                                ptTargetSize,
                                pPavFct
                                );
#else
    __ARM_2D_MEM_FILL_GENERIC(  pSourceBase, 
                                iSourceStride,
                                ptSourceSize, 
                                pTargetBase, 
                                iTargetStride, 
                                ptTargetSize,
                                pPavFct,
                                
                                __API_MCAF_EXTRA_EXPRESSION
                                );
#endif
 
}


/*----------------------------------------------------------------------------*
 * Copy with mirroring                                                        *
 *----------------------------------------------------------------------------*/
void __MCAF_FUNC(copy_x_mirror)( __API_MCAF_INT_TYPE *__RESTRICT phwSource,
                                    int16_t iSourceStride,
                                    __API_MCAF_INT_TYPE *__RESTRICT phwTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize
                                    
                                    __API_MCAF_EXTRA_PARAM
                                    );   
                                        
void __MCAF_FUNC(copy_y_mirror)( __API_MCAF_INT_TYPE *__RESTRICT phwSource,
                                    int16_t iSourceStride,
                                    __API_MCAF_INT_TYPE *__RESTRICT phwTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize
                                    
                                    __API_MCAF_EXTRA_PARAM
                                    );

void __MCAF_FUNC(copy_xy_mirror)( __API_MCAF_INT_TYPE *__RESTRICT phwSource,
                                    int16_t iSourceStride,
                                    __API_MCAF_INT_TYPE *__RESTRICT phwTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize
                                    
                                    __API_MCAF_EXTRA_PARAM
                                    );

/*----------------------------------------------------------------------------*
 * Paving                                                                     *
 *----------------------------------------------------------------------------*/
__WEAK 
void __MCAF_FUNC(2x2_paving)(const __API_MCAF_INT_TYPE *__RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                const arm_2d_size_t * __RESTRICT ptSourceSize,
                                __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                uint16_t tilePairRows,
                                uint16_t tilePairCols
                                    
                                __API_MCAF_EXTRA_PARAM
                                )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_2x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_2x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}


__WEAK void __MCAF_FUNC(2x2_paving_x_mirror)(
                            const __API_MCAF_INT_TYPE *__RESTRICT pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t * __RESTRICT ptSourceSize,
                            __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,
                            uint16_t tilePairRows,
                            uint16_t tilePairCols
                                    
                            __API_MCAF_EXTRA_PARAM
                            )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_2x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR,
                        PAVING_X_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_2x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR,
                        PAVING_X_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}


__WEAK void __MCAF_FUNC(2x2_paving_y_mirror)(
                            const __API_MCAF_INT_TYPE *__RESTRICT pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t * __RESTRICT ptSourceSize,
                            __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,
                            uint16_t tilePairRows,
                            uint16_t tilePairCols
                                    
                            __API_MCAF_EXTRA_PARAM
                            )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_2x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_2x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}

__WEAK void __MCAF_FUNC(2x2_paving_xy_mirror)(
                            const __API_MCAF_INT_TYPE *__RESTRICT pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t * __RESTRICT ptSourceSize,
                            __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,
                            uint16_t tilePairRows,
                            uint16_t tilePairCols
                                    
                            __API_MCAF_EXTRA_PARAM
                            )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_2x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_2x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}


__WEAK 
void __MCAF_FUNC(1x2_paving)( 
                            const __API_MCAF_INT_TYPE * __RESTRICT pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t * __RESTRICT ptSourceSize,
                            __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,
                            uint32_t destWidth,
                            uint16_t tilePairRows
                                
                            __API_MCAF_EXTRA_PARAM
                            )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_1x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_1x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}

__WEAK void __MCAF_FUNC(1x2_paving_x_mirror)(
                            const __API_MCAF_INT_TYPE * __RESTRICT pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t * __RESTRICT ptSourceSize,
                            __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,
                            uint32_t destWidth,
                            uint16_t tilePairRows
                                    
                            __API_MCAF_EXTRA_PARAM
                            )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_1x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR,
                        PAVING_X_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_1x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR,
                        PAVING_X_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}

__WEAK void __MCAF_FUNC(1x2_paving_y_mirror)(
                            const __API_MCAF_INT_TYPE * __RESTRICT pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t * __RESTRICT ptSourceSize,
                            __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,
                            uint32_t destWidth,
                            uint16_t tilePairRows
                        
                            __API_MCAF_EXTRA_PARAM
                            )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_1x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_1x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}


__WEAK void __MCAF_FUNC(1x2_paving_xy_mirror)(
                            const __API_MCAF_INT_TYPE * __RESTRICT pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t * __RESTRICT ptSourceSize,
                            __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,
                            uint32_t destWidth,
                            uint16_t tilePairRows

                            __API_MCAF_EXTRA_PARAM
                            )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_1x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_1x2(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}


__WEAK 
void __MCAF_FUNC(2x1_paving)( 
                            const __API_MCAF_INT_TYPE * __RESTRICT pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t * __RESTRICT ptSourceSize,
                            __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,
                            uint16_t tilePairCols,
                            uint16_t destHeight

                            __API_MCAF_EXTRA_PARAM
                            )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_2x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_2x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}

__WEAK void __MCAF_FUNC(2x1_paving_x_mirror)(
                            const __API_MCAF_INT_TYPE * __RESTRICT pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t * __RESTRICT ptSourceSize,
                            __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,
                            uint16_t tilePairCols,
                            uint16_t destHeight

                            __API_MCAF_EXTRA_PARAM
                            )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_2x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR,
                        PAVING_X_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_2x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR,
                        PAVING_X_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}

__WEAK void __MCAF_FUNC(2x1_paving_y_mirror)(
                            const __API_MCAF_INT_TYPE * __RESTRICT pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t * __RESTRICT ptSourceSize,
                            __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,
                            uint16_t tilePairCols,
                            uint16_t destHeight

                            __API_MCAF_EXTRA_PARAM
                            )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_2x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_2x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}


__WEAK void __MCAF_FUNC(2x1_paving_xy_mirror)(
                            const __API_MCAF_INT_TYPE * __RESTRICT pSourceBase,
                            int16_t iSourceStride,
                            const arm_2d_size_t * __RESTRICT ptSourceSize,
                            __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,
                            uint16_t tilePairCols,
                            uint16_t destHeight

                            __API_MCAF_EXTRA_PARAM
                            )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_2x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_2x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSourceSize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}



__WEAK 
void __MCAF_FUNC(1x1_paving)(const __API_MCAF_INT_TYPE * __RESTRICT pSource,
                               int16_t iSourceStride,
                               __API_MCAF_INT_TYPE * __RESTRICT pTarget,
                               int16_t iTargetStride,
                               const arm_2d_size_t * __RESTRICT ptSrcCopySize,
                               const arm_2d_size_t * __RESTRICT ptDstCopySize
                                    
                                __API_MCAF_EXTRA_PARAM
                                )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_1x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_1x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_DIRECT_START_OFFS(_, _),
                        PAVING_DIRECT_READ_DIR,
                        PAVING_DIRECT_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_DIRECT_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}

__WEAK void __MCAF_FUNC(1x1_paving_x_mirror)(
                                const __API_MCAF_INT_TYPE * __RESTRICT pSource,
                                int16_t iSourceStride,
                                __API_MCAF_INT_TYPE * __RESTRICT pTarget,
                                int16_t iTargetStride,
                                const arm_2d_size_t * __RESTRICT ptSrcCopySize,
                                const arm_2d_size_t * __RESTRICT ptDstCopySize

                                __API_MCAF_EXTRA_PARAM
                                )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_1x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR, PAVING_X_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_1x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_X_MIRROR_START_OFFS(_, _),
                        PAVING_X_MIRROR_READ_DIR, PAVING_X_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_X_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}


__WEAK void __MCAF_FUNC(1x1_paving_y_mirror)(
                                const __API_MCAF_INT_TYPE * __RESTRICT pSource,
                                int16_t iSourceStride,
                                __API_MCAF_INT_TYPE * __RESTRICT pTarget,
                                int16_t iTargetStride,
                                const arm_2d_size_t * __RESTRICT ptSrcCopySize,
                                const arm_2d_size_t * __RESTRICT ptDstCopySize

                                __API_MCAF_EXTRA_PARAM
                                )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_1x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSrcCopySize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_1x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_Y_MIRROR_START_OFFS(iSourceStride, ptSrcCopySize->iHeight),
                        PAVING_Y_MIRROR_READ_DIR,
                        PAVING_Y_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_Y_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}


__WEAK void __MCAF_FUNC(1x1_paving_xy_mirror)(
                                const __API_MCAF_INT_TYPE * __RESTRICT pSource,
                                int16_t iSourceStride,
                                __API_MCAF_INT_TYPE * __RESTRICT pTarget,
                                int16_t iTargetStride,
                                const arm_2d_size_t * __RESTRICT ptSrcCopySize,
                                const arm_2d_size_t * __RESTRICT ptDstCopySize

                                __API_MCAF_EXTRA_PARAM
                                )
{
#ifndef __API_MCAF_EXTRA_PAVE_EXPRESSION
    __ARM_2D_PAVING_1x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSrcCopySize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM));
#else
    __ARM_2D_PAVING_1x1(__API_MCAF_INT_TYPE_BIT_NUM,
                        PAVING_XY_MIRROR_START_OFFS(iSourceStride, ptSrcCopySize->iHeight),
                        PAVING_XY_MIRROR_READ_DIR,
                        PAVING_XY_MIRROR_SETUP_COPY(__API_MCAF_INT_TYPE_BIT_NUM),
                        PAVING_XY_MIRROR_LOAD_PATTERN(__API_MCAF_INT_TYPE_BIT_NUM),
                                
                        __API_MCAF_EXTRA_PAVE_EXPRESSION);
#endif
}


/*----------------------------------------------------------------------------*
 * Copy                                                                       *
 *----------------------------------------------------------------------------*/




__WEAK 
void __MCAF_FUNC(copy_y_mirror)(  __API_MCAF_INT_TYPE * __RESTRICT pSource,
                                    int16_t iSourceStride,
                                    __API_MCAF_INT_TYPE * __RESTRICT pTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptCopySize
                                    
                                    __API_MCAF_EXTRA_PARAM
                                    )
{
#ifndef __API_MCAF_EXTRA_EXPRESSION
    __MCAF_FUNC(1x1_paving_y_mirror)(   pSource,
                                        iSourceStride,
                                        pTarget,
                                        iTargetStride,
                                        ptCopySize,
                                        ptCopySize);
#else
    __MCAF_FUNC(1x1_paving_y_mirror)(   pSource,
                                        iSourceStride,
                                        pTarget,
                                        iTargetStride,
                                        ptCopySize,
                                        ptCopySize,
                                
                                        __API_MCAF_EXTRA_EXPRESSION);
#endif
}

__WEAK 
void __MCAF_FUNC(copy_x_mirror)(__API_MCAF_INT_TYPE * __RESTRICT pSource,
                                  int16_t iSourceStride,
                                  __API_MCAF_INT_TYPE * __RESTRICT pTarget,
                                  int16_t iTargetStride,
                                  arm_2d_size_t * __RESTRICT ptCopySize
                                    
                                  __API_MCAF_EXTRA_PARAM
                                  )
{
#ifndef __API_MCAF_EXTRA_EXPRESSION
    __MCAF_FUNC(1x1_paving_x_mirror)( pSource,
                                        iSourceStride,
                                        pTarget,
                                        iTargetStride,
                                        ptCopySize,
                                        ptCopySize);
#else
    __MCAF_FUNC(1x1_paving_x_mirror)( pSource,
                                        iSourceStride,
                                        pTarget,
                                        iTargetStride,
                                        ptCopySize,
                                        ptCopySize,
                                
                                        __API_MCAF_EXTRA_EXPRESSION);
#endif
}

__WEAK 
void __MCAF_FUNC(copy_xy_mirror)( __API_MCAF_INT_TYPE * __RESTRICT pSource,
                                    int16_t iSourceStride,
                                    __API_MCAF_INT_TYPE * __RESTRICT pTarget,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptCopySize
                                    
                                    __API_MCAF_EXTRA_PARAM
                                    )
{
#ifndef __API_MCAF_EXTRA_EXPRESSION
    __MCAF_FUNC(1x1_paving_xy_mirror)(  pSource,
                                        iSourceStride,
                                        pTarget,
                                        iTargetStride,
                                        ptCopySize,
                                        ptCopySize);
#else
    __MCAF_FUNC(1x1_paving_xy_mirror)(  pSource,
                                        iSourceStride,
                                        pTarget,
                                        iTargetStride,
                                        ptCopySize,
                                        ptCopySize,
                                
                                        __API_MCAF_EXTRA_EXPRESSION);
#endif
}





/*----------------------------------------------------------------------------*
 * Fill                                                                       *
 *----------------------------------------------------------------------------*/

__WEAK 
void __MCAF_FUNC(fill_x_mirror)(  __API_MCAF_INT_TYPE * __RESTRICT pSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t * __RESTRICT ptSourceSize,
                                    __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptTargetSize

                                    __API_MCAF_EXTRA_PARAM
                                    )
{
    static __MCAF_TYPE(paving_fct_t) pav_func = {
        .pav_2x2 = &__MCAF_FUNC(2x2_paving_x_mirror),
        .pav_1x2 = &__MCAF_FUNC(1x2_paving_x_mirror),
        .pav_2x1 = &__MCAF_FUNC(2x1_paving_x_mirror),
        .pav_1x1 = &__MCAF_FUNC(1x1_paving_x_mirror)
    };

#ifndef __API_MCAF_EXTRA_EXPRESSION
    __MCAF_FUNC(fill_generic)(pSourceBase,
                                iSourceStride,
                                ptSourceSize,
                                pTargetBase,
                                iTargetStride, ptTargetSize, &pav_func);
#else
    __MCAF_FUNC(fill_generic)(pSourceBase,
                                iSourceStride,
                                ptSourceSize,
                                pTargetBase,
                                iTargetStride, 
                                ptTargetSize, 
                                &pav_func,
                                
                                __API_MCAF_EXTRA_EXPRESSION);
#endif
}

__WEAK void __MCAF_FUNC(fill_y_mirror)(
                                    __API_MCAF_INT_TYPE * __RESTRICT pSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t * __RESTRICT ptSourceSize,
                                    __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptTargetSize

                                    __API_MCAF_EXTRA_PARAM
                                    )
{
    static __MCAF_TYPE(paving_fct_t) pav_func = {
        .pav_2x2 = &__MCAF_FUNC(2x2_paving_y_mirror),
        .pav_1x2 = &__MCAF_FUNC(1x2_paving_y_mirror),
        .pav_2x1 = &__MCAF_FUNC(2x1_paving_y_mirror),
        .pav_1x1 = &__MCAF_FUNC(1x1_paving_y_mirror)
    };
    
#ifndef __API_MCAF_EXTRA_EXPRESSION
    __MCAF_FUNC(fill_generic)(  pSourceBase,
                                iSourceStride,
                                ptSourceSize,
                                pTargetBase,
                                iTargetStride, 
                                ptTargetSize, 
                                &pav_func);
#else
    __MCAF_FUNC(fill_generic)(  pSourceBase,
                                iSourceStride,
                                ptSourceSize,
                                pTargetBase,
                                iTargetStride, 
                                ptTargetSize, 
                                &pav_func,
                                
                                __API_MCAF_EXTRA_EXPRESSION);
#endif
}


__WEAK void __MCAF_FUNC(fill_xy_mirror)(
                                    __API_MCAF_INT_TYPE * __RESTRICT pSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t * __RESTRICT ptSourceSize,
                                    __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptTargetSize
                                    
                                    __API_MCAF_EXTRA_PARAM
                                    )
{
    static __MCAF_TYPE(paving_fct_t) pav_func = {
        .pav_2x2 = &__MCAF_FUNC(2x2_paving_xy_mirror),
        .pav_1x2 = &__MCAF_FUNC(1x2_paving_xy_mirror),
        .pav_2x1 = &__MCAF_FUNC(2x1_paving_xy_mirror),
        .pav_1x1 = &__MCAF_FUNC(1x1_paving_xy_mirror)
    };

#ifndef __API_MCAF_EXTRA_EXPRESSION
    __MCAF_FUNC(fill_generic)(pSourceBase,
                                iSourceStride,
                                ptSourceSize,
                                pTargetBase,
                                iTargetStride, 
                                ptTargetSize, 
                                &pav_func);
#else
    __MCAF_FUNC(fill_generic)(pSourceBase,
                                iSourceStride,
                                ptSourceSize,
                                pTargetBase,
                                iTargetStride, 
                                ptTargetSize, 
                                &pav_func,
                                
                                __API_MCAF_EXTRA_EXPRESSION);
#endif
}



/*----------------------------------------------------------------------------*
 * APIs used by external source code                                          *
 *----------------------------------------------------------------------------*/

/* kept here for references
__WEAK
void __MCAF_FUNC(copy)(   __API_INT_TYPE *__RESTRICT pSource,
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
*/
extern
void __MCAF_FUNC(copy)(   __API_INT_TYPE *__RESTRICT pSource,
                            int16_t iSourceStride,
                            __API_INT_TYPE *__RESTRICT pTarget,
                            int16_t iTargetStride,
                            arm_2d_size_t *__RESTRICT ptCopySize
                                    
                            __API_MCAF_EXTRA_PARAM
                            );

__WEAK
void __MCAF_FUNC(copy_mirror)( __API_MCAF_INT_TYPE * __RESTRICT pSource,
                                 int16_t iSourceStride,
                                 __API_MCAF_INT_TYPE * __RESTRICT pTarget,
                                 int16_t iTargetStride,
                                 arm_2d_size_t * __RESTRICT ptCopySize,
                                 uint32_t wMode
                                    
                                __API_MCAF_EXTRA_PARAM
                                )
{
#ifndef __API_MCAF_EXTRA_EXPRESSION
    switch (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
      case ARM_2D_CP_MODE_X_MIRROR:
          __MCAF_FUNC(copy_x_mirror)( pSource, iSourceStride,
                                        pTarget, iTargetStride, 
                                        ptCopySize);
          break;
      case ARM_2D_CP_MODE_Y_MIRROR:
          __MCAF_FUNC(copy_y_mirror)( pSource, iSourceStride,
                                        pTarget, iTargetStride, 
                                        ptCopySize);
          break;
      case ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR:
          __MCAF_FUNC(copy_xy_mirror)(pSource, iSourceStride,
                                        pTarget, iTargetStride, 
                                        ptCopySize);
          break;
      default:
          assert(false);        /*! this should not happen */
          //break;
    }
#else
    switch (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
      case ARM_2D_CP_MODE_X_MIRROR:
          __MCAF_FUNC(copy_x_mirror)( pSource, iSourceStride,
                                        pTarget, iTargetStride, 
                                        ptCopySize,
                                
                                        __API_MCAF_EXTRA_EXPRESSION);
          break;
      case ARM_2D_CP_MODE_Y_MIRROR:
          __MCAF_FUNC(copy_y_mirror)( pSource, iSourceStride,
                                        pTarget, iTargetStride, 
                                        ptCopySize,
                                
                                        __API_MCAF_EXTRA_EXPRESSION);
          break;
      case ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR:
          __MCAF_FUNC(copy_xy_mirror)(pSource, iSourceStride,
                                        pTarget, iTargetStride, 
                                        ptCopySize,
                                
                                        __API_MCAF_EXTRA_EXPRESSION);
          break;
      default:
          assert(false);        /*! this should not happen */
          //break;
    }
#endif
}


__WEAK 
void __MCAF_FUNC(fill)(__API_MCAF_INT_TYPE * __RESTRICT pSourceBase,
                         int16_t iSourceStride,
                         arm_2d_size_t * __RESTRICT ptSourceSize,
                         __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                         int16_t iTargetStride,
                         arm_2d_size_t * __RESTRICT ptTargetSize
                                    
                         __API_MCAF_EXTRA_PARAM
                         )
{
    static const __MCAF_TYPE(paving_fct_t) pav_func = {
        .pav_2x2 = &__MCAF_FUNC(2x2_paving),
        .pav_1x2 = &__MCAF_FUNC(1x2_paving),
        .pav_2x1 = &__MCAF_FUNC(2x1_paving),
        .pav_1x1 = &__MCAF_FUNC(1x1_paving)
    };

#ifndef __API_MCAF_EXTRA_EXPRESSION
    __MCAF_FUNC(fill_generic)(pSourceBase,
                                iSourceStride,
                                ptSourceSize,
                                pTargetBase,
                                iTargetStride, 
                                ptTargetSize, 
                                &pav_func);
#else
    __MCAF_FUNC(fill_generic)(pSourceBase,
                                iSourceStride,
                                ptSourceSize,
                                pTargetBase,
                                iTargetStride, 
                                ptTargetSize, 
                                &pav_func,
                                
                                __API_MCAF_EXTRA_EXPRESSION);
#endif
}

__WEAK
void __MCAF_FUNC(fill_mirror)( __API_MCAF_INT_TYPE * __RESTRICT pSourceBase,
                                 int16_t iSourceStride,
                                 arm_2d_size_t * __RESTRICT ptSourceSize,
                                 __API_MCAF_INT_TYPE * __RESTRICT pTargetBase,
                                 int16_t iTargetStride,
                                 arm_2d_size_t * __RESTRICT ptTargetSize,
                                 uint32_t wMode
                                    
                                 __API_MCAF_EXTRA_PARAM
                                 )
{
#ifndef __API_MCAF_EXTRA_EXPRESSION
    switch (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
        case ARM_2D_CP_MODE_X_MIRROR:
            __MCAF_FUNC(fill_x_mirror)(
                    pSourceBase, iSourceStride, ptSourceSize,
                    pTargetBase, iTargetStride, ptTargetSize);
            break;
        case ARM_2D_CP_MODE_Y_MIRROR:
            __MCAF_FUNC(fill_y_mirror)(
                    pSourceBase, iSourceStride, ptSourceSize,
                    pTargetBase, iTargetStride, ptTargetSize);
            break;
        case ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR :
            __MCAF_FUNC(fill_xy_mirror)(
                    pSourceBase, iSourceStride, ptSourceSize, 
                    pTargetBase, iTargetStride, ptTargetSize);
            break;
        default:
            assert(false);  /*! this should not happen */
            //break;
    }
#else
    switch (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
        case ARM_2D_CP_MODE_X_MIRROR:
            __MCAF_FUNC(fill_x_mirror)(
                    pSourceBase, iSourceStride, ptSourceSize,
                    pTargetBase, iTargetStride, ptTargetSize,
                                
                    __API_MCAF_EXTRA_EXPRESSION);
            break;
        case ARM_2D_CP_MODE_Y_MIRROR:
            __MCAF_FUNC(fill_y_mirror)(
                    pSourceBase, iSourceStride, ptSourceSize,
                    pTargetBase, iTargetStride, ptTargetSize,
                                
                    __API_MCAF_EXTRA_EXPRESSION);
            break;
        case ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR :
            __MCAF_FUNC(fill_xy_mirror)(
                    pSourceBase, iSourceStride, ptSourceSize, 
                    pTargetBase, iTargetStride, ptTargetSize,
                                
                    __API_MCAF_EXTRA_EXPRESSION);
            break;
        default:
            assert(false);  /*! this should not happen */
            //break;
    }
#endif
}

#undef __API_MCAF_EXTRA_PAVE_EXPRESSION
#undef __API_MCAF_COPY_LIKE_OP_NAME
#undef __API_MCAF_EXTRA_EXPRESSION
#undef __API_MCAF_EXTRA_PARAM
#undef __API_MCAF_OP_NAME
#undef ____MCAF_FUNC
#undef ___MCAF_FUNC
#undef __MCAF_FUNC
#undef __API_MCAF_COLOUR
#undef __API_MCAF_INT_TYPE
#undef __API_MCAF_INT_TYPE_BIT_NUM
#undef ____MCAF_TYPE
#undef ___MCAF_TYPE
#undef __MCAF_TYPE
