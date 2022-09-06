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
 * Title:        arm-2d_math_helium.h
 * Description:  Provides helium math routines
 *
 * $Date:        29. Sep 2021
 * $Revision:    V 0.0.2
 *
 * Target Processor:  Cortex-M cores with Helium
 *
 * -------------------------------------------------------------------- */



#ifndef __ARM_2D_MATH_HELIUM_H__
#define __ARM_2D_MATH_HELIUM_H__


#if defined(__ARM_2D_HAS_HELIUM__) && __ARM_2D_HAS_HELIUM__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include <arm_math.h>

#if __ARM_2D_HAS_HELIUM_FLOAT__
#include <arm_math_f16.h>
#endif

#ifdef   __cplusplus
extern "C" {
#endif

#define vec_rgb8_t             uint8x16_t
#define vec_rgb16_t            uint16x8_t
#define vec_rgb32_t            uint32x4_t

#define ARM_PIX_SCLTYP(sz)     ARM_CONNECT2(ARM_CONNECT2(uint, sz), _t)
#define ARM_PIX_VECTYP(sz)     ARM_CONNECT2(ARM_CONNECT2(vec_rgb,sz), _t)

#define vld8                 vldrb
#define vld16                vldrh
#define vld32                vldrw
#define vst8                 vstrb
#define vst16                vstrh
#define vst32                vstrw

#define ARM_VLD_ASM(sz)       ARM_CONNECT2(vld, sz)
#define ARM_VST_ASM(sz)       ARM_CONNECT2(vst, sz)

#define ARM_VLD1_ASM(sz)      ARM_TO_STRING(ARM_VLD_ASM(sz).ARM_CONNECT2(u,sz))
#define ARM_VST1_ASM(sz)      ARM_TO_STRING(ARM_VST_ASM(sz).ARM_CONNECT2(u,sz))
#define ARM_VLD1Z_ASM(sz)     ARM_TO_STRING(ARM_VLD_ASM(sz)t.ARM_CONNECT2(u,sz))
#define ARM_VST1P_ASM(sz)     ARM_TO_STRING(ARM_VST_ASM(sz)t.ARM_CONNECT2(u,sz))
#define ARM_VLDWID_ASM(sz, wid)    ARM_TO_STRING(ARM_VLD_ASM(sz).ARM_CONNECT2(u,wid))
#define ARM_VSTNRW_ASM(sz, nrw)    ARM_TO_STRING(ARM_VLD_ASM(sz).ARM_CONNECT2(u,nrw))


/* number of vector elements */
#define ARM_PIX_VECELT(sz)     (128/sz)

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/




#if __ARM_2D_HAS_HELIUM_FLOAT__

#define __PIF16                3.14159265358979F16
#define __CALIBF16             0.009f16
/* avoid 0 division */
/* should not be too small as inverse could grows to F16 infinite 65e3 when */
/* multiplied with numerator */
#define __EPSF16               1e-03f16

#define __LARGEINVF32          100.0f

#define __PI_2_F16             (__PIF16/2.0f16)
#define INV_NEWTON_INIT_F16     0x7773

#define INFINITY_F16           ((float16_t)__builtin_inf())


#define ATANF_LUT                   \
	-0.0443265554792128,	/*p7*/  \
	-0.3258083974640975,	/*p3*/  \
	+0.1555786518463281,	/*p5*/  \
	+0.9997878412794807  	/*p1*/  \

extern const float16_t sinTable_f16[FAST_MATH_TABLE_SIZE + 1];
extern const float16_t atanf_lut_f16[4];

#define INVSQRT_MAGIC_F16           0x59ba      /*  ( 0x1ba = 0x3759df >> 13) */

/* 2D point floating point vector types */
typedef struct arm_2d_point_f32x4_t {
    float32x4_t X;
    float32x4_t Y;
} arm_2d_point_f32x4_t;

typedef struct arm_2d_point_f16x8_t {
    float16x8_t     X;
    float16x8_t     Y;
} arm_2d_point_f16x8_t;

#endif

/* 2D point integer point vector types */
typedef struct arm_2d_point_s16x8_t {
    int16x8_t       X;
    int16x8_t       Y;
} arm_2d_point_s16x8_t;

typedef struct arm_2d_point_s32x4_t {
    int32x4_t       X;
    int32x4_t       Y;
} arm_2d_point_s32x4_t;



#if __ARM_2D_HAS_HELIUM_FLOAT__ == 1

__STATIC_FORCEINLINE  float16x8_t vsin_f16(
    float16x8_t vecIn)
{
    q15x8_t         vecMask = vdupq_n_s16(0x1ff);
    float16x8_t     vecOne = vdupq_n_f16(1.0f16);
    float16x8_t     vecTmpFlt0, vecTmpFlt1;
    float16x8_t     vecSin0, vecSin1;
    q15x8_t         vecTmpFx;


    /*
     * input x is in radians
     * Scale the input to [0 1] range from [0 2*PI]
     * divide input by 2*pi (add 0.25 (pi/2) to read sine table)
     * in = x * 0.159154943092f + 0.25f
     */
    vecTmpFlt0 = vmulq(vecIn, (float16_t) (1.0 / (2.0 * PI)));
    /*
     * Calculation of floor value of input
     */
    vecTmpFx = vcvtmq_s16_f16(vecTmpFlt0);
    vecTmpFlt1 = vcvtq_f16_s16(vecTmpFx);
    /*
     * Map input value to [0 1]
     */
    vecTmpFlt1 = vecTmpFlt0 - vecTmpFlt1;
    /*
     * Calculation of index of the table
     * findex = (float16_t) FAST_MATH_TABLE_SIZE * in;
     */
    vecTmpFlt0 = vecTmpFlt1 * (float16_t) FAST_MATH_TABLE_SIZE;
    /*
     * index = ((uint16_t)findex) & 0x1ff;
     */
    vecTmpFx = vcvtq_s16_f16(vecTmpFlt0);
    vecTmpFx = vandq(vecTmpFx, vecMask);
    /*
     * fractional value calculation
     * fract = findex - (float16_t) index;
     */
    vecTmpFlt1 = vcvtq_f16_s16(vecTmpFx);
    vecTmpFlt0 = vecTmpFlt0 - vecTmpFlt1;
    /*
     * Read two nearest values of input value from the cos table
     * a = sinTable_f16[index];
     */
    vecSin0 = vldrhq_gather_shifted_offset_f16(sinTable_f16, vecTmpFx);
    /*
     * b = sinTable_f16[index+1];
     */
    vecTmpFx = vecTmpFx + 1;
    vecSin1 = vldrhq_gather_shifted_offset_f16(sinTable_f16, vecTmpFx);
    /*
     * 1.0f - fract
     */
    vecTmpFlt1 = vecOne - vecTmpFlt0;
    /*
     * fract * b;
     */
    vecTmpFlt0 = vecTmpFlt0 * vecSin1;
    /*
     * Linear interpolation process
     * cosVal = (1.0f-fract)*a + fract*b;
     */
    vecTmpFlt0 = vfmaq(vecTmpFlt0, vecSin0, vecTmpFlt1);

    // set 'tiny' values to x (F16 precision limit)
    vecTmpFlt0 = vpselq(vecIn, vecTmpFlt0, vcmpleq(vabsq(vecIn), 0.05f16));

    return vecTmpFlt0;
}



__STATIC_FORCEINLINE float16x8_t vcos_f16(
    float16x8_t x)
{
    return vsin_f16(x + __PI_2_F16);
}



/* fast inverse approximation (4x newton) */
__STATIC_INLINE float16x8_t vrecip_hiprec_f16(
    float16x8_t x)
{
    q15x8_t         m;
    float16x8_t         b;
    any16x8_t       xinv;
    float16x8_t         ax = vabsq(x);

    xinv.f = ax;

    m = 0x03c00 - (xinv.i & 0x07c00);
    xinv.i = xinv.i + m;
    xinv.f = 1.41176471f16 - 0.47058824f16 * xinv.f;
    xinv.i = xinv.i + m;

    b = 2.0f16 - xinv.f * ax;
    xinv.f = xinv.f * b;

    b = 2.0f16 - xinv.f * ax;
    xinv.f = xinv.f * b;

    b = 2.0f16 - xinv.f * ax;
    xinv.f = xinv.f * b;

    b = 2.0f16 - xinv.f * ax;
    xinv.f = xinv.f * b;

    xinv.f = vdupq_m(xinv.f, INFINITY_F16, vcmpeqq(x, 0.0f));
    /*
     * restore sign
     */
    xinv.f = vnegq_m(xinv.f, xinv.f, vcmpltq(x, 0.0f));

    return xinv.f;
}

__STATIC_FORCEINLINE float16x8_t vrecip_lowprec_f16(float16x8_t vecIn)
{
    float16x8_t       vecSx, vecW;
    any16x8_t     r;

    vecSx = vabsq(vecIn);
    r.f = vecSx;

    r.i = vdupq_n_u16(INV_NEWTON_INIT_F16) - r.i;

    vecW = vmulq(vecSx, r.f);
    vecW = vsubq(vdupq_n_f16((float16_t)2), vecW);

    r.f = vmulq(r.f, vecW);

    r.f = vdupq_m(r.f, (float16_t)INFINITY, vcmpeqq(vecIn, (float16_t)0));
    /*
    * restore sign
    */
    r.f = vnegq_m(r.f, r.f, vcmpltq_n_f16(vecIn, (float16_t)0));

    return r.f;
}



__STATIC_FORCEINLINE float16x8_t vatan_f16(
    float16x8_t x)
{
    float16x8_t         a, b, r;
    float16x8_t         xx;
    any16x8_t           xinv , ax;

    ax.f = vabsq(x);
    //fast inverse approximation (2x newton)
    xinv.f = vrecip_lowprec_f16( ax.f );

    //if |x| > 1.0 -> ax = -1/ax, r = pi/2
    xinv.f = xinv.f + ax.f;

    // a = (c > 1.0f);
    a = vdupq_n_f16(0.0f16);
    a = vdupq_m(a, 1.0f16, vcmpgtq(ax.f, 1.0f16));

    ax.f = ax.f - a * xinv.f;
    r = a * (float16_t)__PI_2_F16;

    //polynomial evaluation
    xx = ax.f * ax.f;
    a = (atanf_lut_f16[0] * ax.f) * xx + (atanf_lut_f16[2] * ax.f);
    b = (atanf_lut_f16[1] * ax.f) * xx + (atanf_lut_f16[3] * ax.f);
    xx = xx * xx;
    b = b + a * xx;
    r = r + b;

    //if x < 0 -> r = -r
    r = vnegq_m(r, r, vcmpltq(x, 0.0f16));
    // set 'tiny' values to x (F16 precision limit)
    r = vpselq(x, r, vcmpleq(vabsq(x), 0.05f16));

    return r;
}


__STATIC_INLINE float16x8_t vdiv_f16(
    float16x8_t num, float16x8_t den)
{
    return vmulq(num, vrecip_hiprec_f16(den));
}




__STATIC_FORCEINLINE float16x8_t visqrtf_f16(
    float16x8_t vecIn)
{
    int16x8_t       vecNewtonInit = vdupq_n_s16(INVSQRT_MAGIC_F16);
    float16x8_t     vecOneHandHalf = vdupq_n_f16(1.5f16);
    float16x8_t     vecHalf;
    int16x8_t       vecTmpInt;
    float16x8_t     vecTmpFlt, vecTmpFlt1;


    vecHalf = vmulq(vecIn, (float16_t) 0.5f16);
    /*
     * cast input float vector to integer and right shift by 1
     */
    vecTmpInt = vshrq((int16x8_t) vecIn, 1);
    /*
     * INVSQRT_MAGIC - ((vec_q16_t)vecIn >> 1)
     */
    vecTmpInt = vsubq(vecNewtonInit, vecTmpInt);

    /*
     *------------------------------
     * 1st iteration
     *------------------------------
     * (1.5f-xhalf*x*x)
     */
    vecTmpFlt1 = vmulq((float16x8_t) vecTmpInt, (float16x8_t) vecTmpInt);
    vecTmpFlt1 = vmulq(vecTmpFlt1, vecHalf);
    vecTmpFlt1 = vsubq(vecOneHandHalf, vecTmpFlt1);
    /*
     * x = x*(1.5f-xhalf*x*x);
     */
    vecTmpFlt = vmulq((float16x8_t) vecTmpInt, vecTmpFlt1);

    /*
     *------------------------------
     * 2nd iteration
     *------------------------------
     */
    vecTmpFlt1 = vmulq(vecTmpFlt, vecTmpFlt);
    vecTmpFlt1 = vmulq(vecTmpFlt1, vecHalf);
    vecTmpFlt1 = vsubq(vecOneHandHalf, vecTmpFlt1);
    vecTmpFlt = vmulq(vecTmpFlt, vecTmpFlt1);

    /*
     * set negative values to nan
     */
    vecTmpFlt = vdupq_m(vecTmpFlt, (float16_t) NAN, vcmpltq(vecIn, (float16_t) 0.0f16));
    vecTmpFlt =
        vdupq_m(vecTmpFlt, (float16_t) INFINITY, vcmpeqq(vecIn, (float16_t) 0.0f16));

    return vecTmpFlt;
}


__STATIC_FORCEINLINE float16x8_t vsqrtf_f16(
    float16x8_t vecIn)
{
    float16x8_t         vecDst;

    /* inverse square root unsing 2 newton iterations */
    vecDst = visqrtf_f16(vecIn);
    vecDst = vdupq_m(vecDst, 0.0f, vcmpeqq(vecIn, 0.0f));
    vecDst = vecDst * vecIn;
    return vecDst;
}

#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef   __cplusplus
}
#endif

#endif // (ARM_MATH_HELIUM) || defined(ARM_MATH_MVEF) || defined(ARM_MATH_MVEI)

#endif // __ARM_2D_MATH_HELIUM_H__

