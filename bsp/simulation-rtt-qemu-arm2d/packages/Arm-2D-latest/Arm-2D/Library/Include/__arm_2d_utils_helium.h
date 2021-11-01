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
 * Title:        arm-2d_utils_helium.h
 * Description:  Provides helium utility routines
 *
 * $Date:        20. May 2021
 * $Revision:    V 0.0.1
 *
 * Target Processor:  Cortex-M cores with Helium
 *
 * -------------------------------------------------------------------- */



#ifndef __ARM_2D_UTILS_HELIUM_H__
#define __ARM_2D_UTILS_HELIUM_H__


#if __ARM_2D_HAS_HELIUM_INTEGER__ == 1

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include <arm_math.h>

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

__STATIC_FORCEINLINE
void __arm_2d_rgb565_unpack_single_vec(uint16x8_t in,
                                            uint16x8_t * R, uint16x8_t * G, uint16x8_t * B)
{
    uint16x8_t      vecMaskR = vdupq_n_u16(0x001f);
    uint16x8_t      vecMaskG = vdupq_n_u16(0x003f);

    *R = (in & vecMaskR) * 8;
    *B = ((in >> 11)) * 8;
    *G = ((in >> 5) & vecMaskG) * 4;
}


__STATIC_FORCEINLINE
uint16x8_t __arm_2d_rgb565_pack_single_vec(uint16x8_t R, uint16x8_t G, uint16x8_t B)
{
    uint16x8_t      vecMaskBpck = vdupq_n_u16(0x00f8);
    uint16x8_t      vecMaskGpck = vdupq_n_u16(0x00fc);

    uint16x8_t      vOut = vorrq(vshrq(R, 3),
                                 vmulq(vandq(G, vecMaskGpck), 8));

    vOut = vorrq(vOut, vmulq(vandq(B, vecMaskBpck), 256));

    return vOut;
}

__STATIC_FORCEINLINE
uint16x8_t __arm_2d_rgb565_alpha_blending_single_vec(
                                            uint16x8_t      hwSource1,
                                            uint16x8_t      hwSource2,
                                            uint_fast8_t    chRatio)
{
    uint16_t        ratio1x8 = (256 - chRatio) * 8;
    uint16_t        ratio1x4 = (256 - chRatio) * 4;
    uint16_t        ratio2x8 = (chRatio) * 8;
    uint16_t        ratio2x4 = (chRatio) * 4;
    uint16x8_t      vecMaskR = vdupq_n_u16(0x001f);
    uint16x8_t      vecMaskG = vdupq_n_u16(0x003f);
    uint16x8_t      vecMaskBpck = vdupq_n_u16(0x00f8);
    uint16x8_t      vecMaskGpck = vdupq_n_u16(0x00fc);
    uint16x8_t      vecR0, vecB0, vecG0;
    uint16x8_t      vecR1, vecB1, vecG1;

    /* unpack 1st stream */
    vecR0 = hwSource1 & vecMaskR;
    vecB0 = hwSource1 >> 11;
    vecG0 = hwSource1 >> 5;
    vecG0 = vecG0 & vecMaskG;


    /* unpack 2nd stream */
    vecR1 = hwSource2 & vecMaskR;
    vecB1 = hwSource2 >> 11;
    vecG1 = hwSource2 >> 5;
    vecG1 = vecG1 & vecMaskG;


    /* merge */
    vecR0 = vecR0 * ratio1x8 + vecR1 * ratio2x8;
    vecR0 = vecR0 >> 8;

    vecG0 = vecG0 * ratio1x4 + vecG1 * ratio2x4;
    vecG0 = vecG0 >> 8;

    vecB0 = vecB0 * ratio1x8 + vecB1 * ratio2x8;
    vecB0 = vecB0 >> 8;


    /* pack */
    return vecR0 >> 3 | vmulq((vecG0 & vecMaskGpck), 8)
        | vmulq((vecB0 & vecMaskBpck), 256);
}


__STATIC_FORCEINLINE
uint16x8_t __arm_2d_rgb565_blending_opacity_single_vec(
                                            uint16x8_t      hwSource1,
                                            uint16x8_t      hwSource2,
                                            uint16x8_t      vecHwOpacity)
{
    uint16x8_t      vecAlpha = vsubq_u16(vdupq_n_u16(256), vecHwOpacity);
    uint16x8_t      vecR, vecG, vecB;
    uint16x8_t      vecSrcR, vecSrcG, vecSrcB;

    /* unpack sources */
    __arm_2d_rgb565_unpack_single_vec(hwSource1, &vecR, &vecG, &vecB);
    __arm_2d_rgb565_unpack_single_vec(hwSource2, &vecSrcR, &vecSrcG, &vecSrcB);

    /* merge */
    vecR = vecR * vecHwOpacity + vecSrcR * vecAlpha;
    vecR = vecR >> 8;

    vecG = vecG * vecHwOpacity + vecSrcG * vecAlpha;
    vecG = vecG >> 8;

    vecB = vecB * vecHwOpacity + vecSrcB * vecAlpha;
    vecB = vecB >> 8;

    /* pack */
    return __arm_2d_rgb565_pack_single_vec(vecR, vecG, vecB);
}


__STATIC_FORCEINLINE
uint16x8_t __rgb888_alpha_blending_direct_single_vec(
                                        uint16x8_t      wSource1,  /* widened input bytes */
                                        uint16x8_t      wSource2,  /* widened input bytes */
                                        uint_fast8_t    chRatio)
{
    uint16_t        chRatioCompl = 256 - (uint16_t) chRatio;
    uint16x8_t      vecOut;

    vecOut = vmulq_n_u16(wSource1, (uint16_t) chRatio);
    vecOut = vmlaq_n_u16(vecOut, wSource2, chRatioCompl);

    /* widened output */
    return vecOut >> 8;
}



/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef   __cplusplus
}
#endif

#endif // (ARM_MATH_HELIUM) || defined(ARM_MATH_MVEF) || defined(ARM_MATH_MVEI)

#endif // __ARM_2D_UTILS_HELIUM_H__

