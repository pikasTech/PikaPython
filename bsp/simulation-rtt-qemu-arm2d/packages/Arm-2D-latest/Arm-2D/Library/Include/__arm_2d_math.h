/******************************************************************************
 * @file     arm_2d_math.h
 * @brief    Public header file for Arm-2D Library
 * @version  V1.0.0
 * @date     16. June 2021
 ******************************************************************************/
/*
 * Copyright (c) 2010-2020 Arm Limited or its affiliates. All rights reserved.
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

#ifndef __ARM_2D_MATH_H__
#define __ARM_2D_MATH_H__

/*============================ INCLUDES ======================================*/


/*! \note this header file is not standalone, and please always use it after
 *!       #include "arm_2d_features.h"
 *!       #include "arm_2d_utils.h"
 *!       in your c source file if you insist to use it explicity.
 */

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   pragma clang diagnostic ignored "-Wundef"
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpedantic"
#endif

/*! \note arm-2d relies on CMSIS 5.8.0 and above.
 */
#include <arm_math.h>

#if     defined(__IS_COMPILER_ARM_COMPILER_5__)                                 \
    &&  defined(__ARM_2D_HAS_HELIUM__) && __ARM_2D_HAS_HELIUM__
#   warning 'Arm Compiler 5 doesn\'t support Armv8.1-M architecture, please use Arm Compiler 5 instead. If you insist using Arm Compiler 5, __ARM_2D_HAS_HELIUM__ is forced to 0.'
#   undef __ARM_2D_HAS_HELIUM__
#   define __ARM_2D_HAS_HELIUM__            0
#endif


#if defined(__ARM_2D_HAS_HELIUM__) && __ARM_2D_HAS_HELIUM__
#include <arm_mve.h>
#else
// if MVE is not defined, use float type for bilinear interpolation
typedef float float16_t;
#endif

#if __ARM_2D_HAS_DSP__

#if defined(__IS_COMPILER_ARM_COMPILER_5__)
#   include "armdsp.h"
#else
#   include <arm_acle.h>
#endif

#endif


#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*----------------------------------------------------------------------------*
 * Math                                                                       *
 *----------------------------------------------------------------------------*/
#ifndef MAX
#   define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#   define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif

#ifndef _BV
#   define _BV(__BIT)       (1 << (__BIT))
#endif

#ifndef ABS
#   define ABS(x) ((x) > 0 ? (x) : -(x))
#endif

#undef MULTFX
/* 32 bit multiplication with high part extraction */
#define MULTFX(x,y)         (q31_t)((q63_t)((q63_t) (x) * (q63_t)(y)) >> 32)

/* Q16 multiplication */
#undef MUL_Q16
#define MUL_Q16(x,y)         (q31_t)((q63_t)((q63_t) (x) * (q63_t)(y)) >> 16)


#define ARM_PIX_SCLTYP(sz)     ARM_CONNECT2(ARM_CONNECT2(uint, sz), _t)

#define ARM_2D_ANGLE(__ANGLE)  ((float)((float)(__ANGLE) * 3.1416926f / 180.0f))

#if __ARM_2D_HAS_DSP__

#undef __QDADD
#undef __QDSUB
#define __QDADD(X, Y)     __qadd(X, __qdbl(Y))
#define __QDSUB(X, Y)     __qsub(X, __qdbl(Y))

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#elif defined(__ARM_2D_CFG_UNSAFE_NO_SATURATION_IN_FIXED_POINT_FOR_PERFROMANCE__)
/*
 * @brief C custom defined QDADD
 */
__STATIC_FORCEINLINE int32_t __QDADD(
    int32_t x,
    int32_t y)
{
    return x + y * 2;
}
/*
 * @brief C custom defined QDSUB
 */
__STATIC_FORCEINLINE int32_t __QDSUB(
    int32_t x,
    int32_t y)
{
    return x - y * 2;
}
#else

/*
 * @brief C custom defined QDADD
 */
__STATIC_FORCEINLINE int32_t __QDADD(
    int32_t x,
    int32_t y)
{
    return ((int32_t)(clip_q63_to_q31((q63_t)x + (q63_t)y*2)));
}
/*
 * @brief C custom defined QDSUB
 */
__STATIC_FORCEINLINE int32_t __QDSUB(
    int32_t x,
    int32_t y)
{
    return ((int32_t)(clip_q63_to_q31((q63_t)x - (q63_t)2*y)));
}

#endif



#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif


#endif  /* end of __ARM_2D_MATH_H__ */

