/*
 * Copyright (C) 2022 Arm Limited or its affiliates. All rights reserved.
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
 * $Date:        18. July 2022
 * $Revision:    V.1.0.3
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_FEATURES_H__
#define __ARM_2D_FEATURES_H__

/*============================ INCLUDES ======================================*/


#ifdef   __cplusplus
extern "C" {
#endif

/*!
 * \addtogroup gKernel 1 Kernel
 * @{
 */

#ifndef __ARM_2D_SKIP_CFG_HEADER__
#   ifndef __ARM_2D_USER_CFG_HEADER__
#       include "arm_2d_cfg.h"
#   else
#       include __ARM_2D_USER_CFG_HEADER__
#   endif
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
#   if  !defined(__ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__) ||                        \
        __ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__ < 4
#       define __ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__      4
#   endif
#endif

#undef __ARM_2D_HAS_FPU__
#if defined(__ARM_FP)
#define __ARM_2D_HAS_FPU__                              1
#else
#define __ARM_2D_HAS_FPU__                              0
#endif

#undef __ARM_2D_HAS_DSP__
#if defined(__ARM_FEATURE_DSP) && __ARM_FEATURE_DSP
#define __ARM_2D_HAS_DSP__                              1
#else
#define __ARM_2D_HAS_DSP__                              0
#endif

#ifndef __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
#   ifdef __ARM_2D_HAS_INTERPOLATION_ROTATION__
#   warning __ARM_2D_HAS_INTERPOLATION_ROTATION__ is deprecated, please use __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ instead.
/*! \brief  __ARM_2D_HAS_INTERPOLATION_ROTATION__ is deprecated 
 *!         add this for backward compatible
 */
#       define __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__                              \
                __ARM_2D_HAS_INTERPOLATION_ROTATION__
#   else
#       define __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__        0
#   endif
#endif


/*! \note DO NOT define macro __ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__ unless
 *!       you sure about what you are doing.
 */
#if !__ARM_2D_HAS_FPU__
#   undef __ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__
#   define __ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__   1
#elif   !__ARM_2D_HAS_HELIUM__                                                  \
    &&  !defined(__ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__)
    /*! \note For Armv7-m processors and Armv8-m processors that have no Helium
     *!       extension but only FPU, fixed point rotation is faster than the
     *!       float point rotation even if FPU can accelerate float point
     *!       operations.
     */
#   define __ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__   1
#endif

#if __ARM_2D_HAS_HELIUM_INTEGER__ && !__ARM_2D_HAS_HELIUM_FLOAT__
#   undef __ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__
#   define __ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__   1
#endif

#ifndef __ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__
#   define __ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__   1
#endif

/*! \note In your application, if you do need to use RGBA8888 for some resources
 *!       and you want to use colour channels (e.g. the alpha channel) in mask 
 *!       related APIs, please set this macro to 1 in your project.
 */
#ifndef __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
#   define __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__  1
#endif

/*----------------------------------------------------------------------------*
 * Unsafe configurations                                                      *
 *----------------------------------------------------------------------------*
 * Following macro switches are used to improve performance with aggressive   *
 * methods which might cause errors or distortions in some cases.             *
 * Those macros are undefined by defaults. Please use with cautions.          *
 *----------------------------------------------------------------------------*
 *                                                                            *
 * 1. __ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__                         *
 *    This option is used to remove calibration in angle computations to gain *
 *    a better performance, small error might be noticible for angles like    *
 *    90, 180, 270 etc.                                                       *
 *                                                                            *
 * 2. __ARM_2D_CFG_UNSAFE_NO_SATURATION_IN_FIXED_POINT__                      *
 *    This option is used to speed up M-cores without DSP support             *
 *    It skips saturation in the QADD/QDADD/QDSUB involved in the rotation.   *
 *    The chances of overflow remain low as elements involved are using       *
 *    non-accumulating Q15.16 format and integer parts are in the range of    *
 *    the screen size providing enough margin.                                *
 *                                                                            *
 * 3. __ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__                     *
 *    When define this macro, alpha value 0xFF will not be treated as opaque  *
 *    in mask related operations you can barely see the background. Defining  *
 *    this macro can get a big performance uplift.                            * 
 *----------------------------------------------------------------------------*/


/*! \note __ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_ROTATION_FOR_PERFORMANCE__ is 
 *        deprecated. 
 *        Please use __ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__ instead.
 */
#ifndef __ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__
#   ifdef  __ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_ROTATION_FOR_PERFORMANCE__

#       warning __ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_ROTATION_FOR_PERFORMANCE__\
 is deprecated, please use __ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__\
 instead.

#       define __ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__                  \
                __ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_ROTATION_FOR_PERFORMANCE__
#   endif
#endif

/*! \note __ARM_2D_CFG_UNSAFE_NO_SATURATION_IN_FIXED_POINT_FOR_PERFROMANCE__ is 
 *        deprecated. 
 *        Please use __ARM_2D_CFG_UNSAFE_NO_SATURATION_IN_FIXED_POINT__ instead.
 */
#ifndef __ARM_2D_CFG_UNSAFE_NO_SATURATION_IN_FIXED_POINT__
#   ifdef  __ARM_2D_CFG_UNSAFE_NO_SATURATION_IN_FIXED_POINT_FOR_PERFROMANCE__
#       warning __ARM_2D_CFG_UNSAFE_NO_SATURATION_IN_FIXED_POINT_FOR_PERFROMANCE__\
 is deprecated, please use __ARM_2D_CFG_UNSAFE_NO_SATURATION_IN_FIXED_POINT__\
 instead.

#       define  __ARM_2D_CFG_UNSAFE_NO_SATURATION_IN_FIXED_POINT__              \
            __ARM_2D_CFG_UNSAFE_NO_SATURATION_IN_FIXED_POINT_FOR_PERFROMANCE__
#endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*! @} */

#ifdef   __cplusplus
}
#endif

#endif
