/*
 * Copyright (C) 2021 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        #include "arm_2d_features.h"
 * Description:  Public header file to indicate features avaialble for this
 *               arm-2d library variant.
 *
 * $Date:        29. Jan 2020
 * $Revision:    V.0.5.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_FEATURES_H__
#define __ARM_2D_FEATURES_H__

/*============================ INCLUDES ======================================*/


#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#undef __ARM_2D_HAS_HELIUM__
#undef __ARM_2D_HAS_HELIUM_INTEGER__
#undef __ARM_2D_HAS_HELIUM_FLOAT__

#if defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE
#   define __ARM_2D_HAS_HELIUM__                        1
#   define __ARM_2D_HAS_HELIUM_INTEGER__                1
#   if (__ARM_FEATURE_MVE & 2)
#       define __ARM_2D_HAS_HELIUM_FLOAT__              1
#   else
#       define __ARM_2D_HAS_HELIUM_FLOAT__              0
#   endif
#else
#   define __ARM_2D_HAS_HELIUM__                        0
#   define __ARM_2D_HAS_HELIUM_INTEGER__                0
#   define __ARM_2D_HAS_HELIUM_FLOAT__                  0
#endif


#ifndef __ARM_2D_HAS_CDE__
#   define __ARM_2D_HAS_CDE__                           0
#endif

#ifndef __ARM_2D_HAS_HW_ACC__
#   define __ARM_2D_HAS_HW_ACC__                        0
#endif
#if defined(__ARM_2D_HAS_HW_ACC__) && __ARM_2D_HAS_HW_ACC__
#   if defined(__ARM_2D_HAS_ASYNC__) && !__ARM_2D_HAS_ASYNC__
#       warning As long as __ARM_2D_HAS_HW_ACC__ is set to 1,\
 __ARM_2D_HAS_ASYNC__ is forced to 1. Since you set __ARM_2D_HAS_ASYNC__ to\
 0, please remove your macro definition for __ARM_2D_HAS_ASYNC__ to avoid this\
 warning.
#   endif
#   undef __ARM_2D_HAS_ASYNC__
#   define __ARM_2D_HAS_ASYNC__                         1
#endif

#ifndef __ARM_2D_HAS_ASYNC__
#   define __ARM_2D_HAS_ASYNC__                         1
#endif
#if defined(__ARM_2D_HAS_ASYNC__) &&  __ARM_2D_HAS_ASYNC__
#   if  !defined(__ARM_2D_DEFAULT_SUB_TASK_POOL_SIZE) ||                        \
        __ARM_2D_DEFAULT_SUB_TASK_POOL_SIZE < 4
#       define __ARM_2D_DEFAULT_SUB_TASK_POOL_SIZE      4
#   endif
#endif

#undef __ARM_2D_HAS_FPU__
#if defined(__ARM_FP)
#define __ARM_2D_HAS_FPU__                              1
#else
#define __ARM_2D_HAS_FPU__                              0
#endif

#undef __ARM_2D_HAS_DSP__
#if defined(__ARM_FEATURE_DSP)
#define __ARM_2D_HAS_DSP__                              1
#else
#define __ARM_2D_HAS_DSP__                              0
#endif


#ifndef __ARM_2D_HAS_INTERPOLATION_ROTATION__
#   define __ARM_2D_HAS_INTERPOLATION_ROTATION__        0
#endif


/*! \note DO NOT define macro __ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__ unless
 *!       you sure about what you are doing.
 */
#if !__ARM_2D_HAS_FPU__
#   undef __ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__
#   define __ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__   1
#elif   !__ARM_2D_HAS_HELIUM__                                                  \
    &&  !defined(__ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__)
    /*! \note For Armv7-m processors and Armv8-m processors that have no Helium
     *!       extension but only FPU, fixed point rotation is faster than the
     *!       float point rotation even if FPU can accelerate float point
     *!       operations.
     */
#   define __ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__   1
#endif

#if __ARM_2D_HAS_HELIUM_INTEGER__ && !__ARM_2D_HAS_HELIUM_FLOAT__
#   undef __ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__
#   define __ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__   1
#endif

#ifndef __ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__
#   define __ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__   0
#endif

/*! \note In your application, if you do need to use RGBA8888 for some resources
 *!       and you want to use colour channels (e.g. the alpha channel) in mask 
 *!       related APIs, please set this macro to 1 in your project.
 */
#ifndef __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
#   define __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__ 0
#endif

/*----------------------------------------------------------------------------*
 * Unsafe configurations                                                      *
 *----------------------------------------------------------------------------*
 * Following macro switches are used to improve performance with aggressive   *
 * methods which might cause errors or distortions in some cases.             *
 * Those macros are undefined by defaults. Please use with cautions.          *
 *----------------------------------------------------------------------------*
 *                                                                            *
 * 1. __ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_ROTATION_FOR_PERFORMANCE__          *
 *    This option is used to remove calibration in angle computations to gain *
 *    a better performance, small error might be noticible for angles like    *
 *    90, 180, 270 etc.                                                       *
 *                                                                            *
 * 2. __ARM_2D_CFG_UNSAFE_NO_SATURATION_IN_FIXED_POINT_FOR_PERFROMANCE__      *
 *    This option is used to speed up M-cores without DSP support             *
 *    It skips saturation in the QADD/QDADD/QDSUB involved in the rotation.   *
 *    The chances of overflow remain low as elements involved are using       *
 *    non-accumulating Q15.16 format and integer parts are in the range of    *
 *    the screen size providing enough margin.                                *
 *----------------------------------------------------------------------------*/

/*============================ MACROFIED FUNCTIONS ===========================*/

#if defined(__ARM_2D_HAS_CDE__) && !__ARM_2D_HAS_CDE__
#   define __arm_2d_cde_init()
#endif

#if defined(__ARM_2D_HAS_HELIUM__) && !__ARM_2D_HAS_HELIUM__
#   define __arm_2d_helium_init()
#endif

#if defined(__ARM_2D_HAS_HW_ACC__) && !__ARM_2D_HAS_HW_ACC__
#   define __arm_2d_acc_init()
#endif

#if defined(__ARM_2D_HAS_ASYNC__) && !__ARM_2D_HAS_ASYNC__
#   define __arm_2d_async_init()
#endif


#undef arm_2d_init
#define arm_2d_init()                                                           \
        do {                                                                    \
            __arm_2d_init();                                                    \
            __arm_2d_async_init();                                              \
            __arm_2d_helium_init();                                             \
            __arm_2d_cde_init();                                                \
            __arm_2d_acc_init();                                                \
        } while(0)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef   __cplusplus
}
#endif

#endif
