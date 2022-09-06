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
 * $Date:        20. May 2022
 * $Revision:    V 0.0.2
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

#if defined(__ARM_2D_HAS_CDE__) && __ARM_2D_HAS_CDE__
#include "__arm_2d_cde.h"
#endif

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/


#if defined(__MVE_DEBUG__) && __MVE_DEBUG__
#   define ____MVE_WRAPPER(__FUNC)       __FUNC##_mve
#else
#   define ____MVE_WRAPPER(__FUNC)       __FUNC
#endif

#define __MVE_WRAPPER(__FUNC)       ____MVE_WRAPPER(__FUNC)



#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
/* set vecAlpha value to 0 when equal to the compensated value */
/* (=1 or 2 when involving 2 alpha = 255 multiplications)      */

/* 16-bit vector */
#define ALPHA_255_COMP_VEC16(vecAlpha, compVal)   vecAlpha = \
                vpselq(vdupq_n_u16(256), vecAlpha, vcmpeqq_n_u16(vecAlpha, compVal))

/* 32-bit vector */
#define ALPHA_255_COMP_VEC32(vecAlpha, compVal)   vecAlpha = \
                vpselq(vdupq_n_u32(256), vecAlpha, vcmpeqq_n_u32(vecAlpha, compVal))

#else
#define ALPHA_255_COMP_VEC16(vecAlpha, compVal)
#define ALPHA_255_COMP_VEC32(vecAlpha, compVal)
#endif


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


#if !defined(__ARM_2D_HAS_CDE__) || __ARM_2D_HAS_CDE__ == 0

__STATIC_FORCEINLINE
uint16x8_t __arm_2d_rgb565_blending_scal_opacity_single_vec(
                                            uint16x8_t      Source1,
                                            uint16x8_t      Source2,
                                            uint_fast16_t hwRatio)
{
    /* scalar ratio allows extra optimization */
    uint16_t        ratio1x8 = (256 - hwRatio) * 8;
    uint16_t        ratio1x4 = (256 - hwRatio) * 4;
    uint16_t        ratio2x8 = (hwRatio) * 8;
    uint16_t        ratio2x4 = (hwRatio) * 4;
    uint16x8_t      vecMaskR = vdupq_n_u16(0x001f);
    uint16x8_t      vecMaskG = vdupq_n_u16(0x003f);
    uint16x8_t      vecMaskBpck = vdupq_n_u16(0x00f8);
    uint16x8_t      vecMaskGpck = vdupq_n_u16(0x00fc);
    uint16x8_t      vecR0, vecB0, vecG0;
    uint16x8_t      vecR1, vecB1, vecG1;

    /* unpack 1st stream */
    vecR0 = Source1 & vecMaskR;
    vecB0 = Source1 >> 11;
    vecG0 = Source1 >> 5;
    vecG0 = vecG0 & vecMaskG;


    /* unpack 2nd stream */
    vecR1 = Source2 & vecMaskR;
    vecB1 = Source2 >> 11;
    vecG1 = Source2 >> 5;
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
                                            uint16x8_t      Source1,
                                            uint16x8_t      Source2,
                                            uint16x8_t      vecHwOpacity)
{
    uint16x8_t      vecAlpha = vsubq(vdupq_n_u16(256), vecHwOpacity);
    uint16x8_t      vecR, vecG, vecB;
    uint16x8_t      vecSrcR, vecSrcG, vecSrcB;

    /* unpack sources */
    __arm_2d_rgb565_unpack_single_vec(Source1, &vecR, &vecG, &vecB);
    __arm_2d_rgb565_unpack_single_vec(Source2, &vecSrcR, &vecSrcG, &vecSrcB);

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

#endif


__STATIC_FORCEINLINE
uint16x8_t __arm_2d_gray8_blending_opacity_single_vec(
                                            uint16x8_t      Source1,
                                            uint16x8_t      Source2,
                                            uint16x8_t      vecHwOpacity)
{
    uint16x8_t      vecAlpha = vsubq(vdupq_n_u16(256), vecHwOpacity);

    return  vaddq(
                vmulq(Source1, vecHwOpacity),
                vmulq(Source2, vecAlpha))  >> 8;
}



__STATIC_FORCEINLINE
uint32x4_t __arm_2d_cccn888_blending_opacity_single_vec_alt(
                                            uint32x4_t      Source1,
                                            uint32x4_t      Source2,
                                            uint32x4_t      vecHwOpacity)
{
    uint32x4_t      vecAlpha = vsubq(vdupq_n_u32(256), vecHwOpacity);
    uint32x4_t      acc;

    acc = ((Source1 & 0xff) * vecHwOpacity + (Source2 & 0xff) * vecAlpha) >> 8;
    acc |=
        ((((Source1 >> 8) & 0xff) * vecHwOpacity +
          ((Source2 >> 8) & 0xff) * vecAlpha) >> 8) << 8;
    acc |=
        ((((Source1 >> 16) & 0xff) * vecHwOpacity +
          ((Source2 >> 16) & 0xff) * vecAlpha) >> 8) << 16;
    /* preserve Source1 alpha */
    acc |= Source1 & 0xff000000;

    return acc;
}



__STATIC_FORCEINLINE
uint32x4_t __arm_2d_cccn888_blending_opacity_single_vec(
                                            uint32x4_t      Source1,
                                            uint32x4_t      Source2,
                                            uint32x4_t      vecHwOpacity)
{
    uint32x4_t      vecAlpha = vsubq(vdupq_n_u32(256), vecHwOpacity);

    /* expand opacity / alpha from byte to packed 32-bit*/
    /* {0x000000a0 0x000000a1 0x000000a2 0x000000a3}
       => {0x00a0a0a0 0x00a1a1a1 0x00a2a2a2 0x00a3a3a3}
     */
    uint32x4_t      vecAlphaExp = (vecAlpha & 0xff) * 0x00010101;
    uint32x4_t      vecHwOpacityExp = (vecHwOpacity & 0xff) * 0x00010101;


    uint32x4_t      blendB = vaddq_u16(vmullbq_int_u8((uint8x16_t) Source1,
                                                      (uint8x16_t) vecHwOpacityExp),
                                       vmullbq_int_u8((uint8x16_t) Source2,
                                                      (uint8x16_t) vecAlphaExp));
    uint32x4_t      blendT = vaddq_u16(vmulltq_int_u8((uint8x16_t) Source1,
                                                      (uint8x16_t) vecHwOpacityExp),
                                       vmulltq_int_u8((uint8x16_t) Source2,
                                                      (uint8x16_t) vecAlphaExp));

    int8x16_t       target = vshrnbq_n_s16(vuninitializedq_u8(), blendB, 8);
    target = vshrntq_n_s16(target, blendT, 8);

    target = vpselq_u32(Source1, target, vcmpeqq_n_u32(vecHwOpacity, 0x100));
    target = vpselq_u32(Source2, target, vcmpeqq_n_u32(vecHwOpacity, 0));

    /* preserve Source1 alpha */
    target = (int8x16_t) ((Source1 & 0xff000000) | ((uint32x4_t) target & 0x00ffffff));
    return target;
}

__STATIC_FORCEINLINE
uint16x8_t __rgb888_alpha_blending_direct_single_vec(
                                        uint16x8_t      wSource1,  /* widened input bytes */
                                        uint16x8_t      wSource2,  /* widened input bytes */
                                        uint_fast16_t   hwRatio)
{
    uint16_t        transp = 256 - (uint16_t) hwRatio;
    uint16x8_t      vecOut;

    vecOut = vmulq_n_u16(wSource1, (uint16_t) hwRatio);
    vecOut = vmlaq_n_u16(vecOut, wSource2, transp);

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

