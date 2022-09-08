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
 * Title:        arm-2d_helium.c
 * Description:  Acceleration extensions using Helium.
 *
 * $Date:        03. Aug 2022
 * $Revision:    V.0.13.6
 *
 * Target Processor:  Cortex-M cores with Helium
 *
 * -------------------------------------------------------------------- */

#define __ARM_2D_IMPL__

#include "arm_2d.h"
#include "__arm_2d_impl.h"

#if defined(__ARM_2D_HAS_HELIUM__) && __ARM_2D_HAS_HELIUM__

#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
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
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wvector-conversion"
#   pragma clang diagnostic ignored "-Wundef"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#endif


/*============================ INCLUDES ======================================*/

#include "__arm_2d_paving_helium.h"
#include "__arm_2d_math_helium.h"
#include "__arm_2d_utils_helium.h"
#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

/*!
 * \brief initialise the helium acceleration
 */
void __arm_2d_helium_init(void)
{
    /* even if this is empty, do not remove it */
}


/*----------------------------------------------------------------------------*
 * Code Template for tile operations                                                             *
 *----------------------------------------------------------------------------*/

#define __API_COLOUR                c8bit
#define __API_INT_TYPE              uint8_t
#define __API_INT_TYPE_BIT_NUM      8

#include "__arm_2d_copy_helium.inc"

#define __API_COLOUR                rgb16
#define __API_INT_TYPE              uint16_t
#define __API_INT_TYPE_BIT_NUM      16

#include "__arm_2d_copy_helium.inc"


#define __API_COLOUR                rgb32
#define __API_INT_TYPE              uint32_t
#define __API_INT_TYPE_BIT_NUM      32

#include "__arm_2d_copy_helium.inc"


#define __ARM_2D_COMPILATION_UNIT
#include "__arm_2d_transform_helium.c"

#define __ARM_2D_COMPILATION_UNIT
#include "__arm_2d_conversion_helium.c"

/*----------------------------------------------------------------------------*
 * Helper
 *----------------------------------------------------------------------------*/

__OVERRIDE_WEAK
void __MVE_WRAPPER(arm_2d_helper_swap_rgb16)(uint16_t *phwBuffer, uint32_t wCount)
{
    if (0 == wCount) {
        return ;
    }

    // aligned (2)
    assert((((uintptr_t) phwBuffer) & 0x01) == 0);

    // src not aligned to 32-bit
    // (helium supports unaligned vector load & store but with extra cycle penalty)
    if ((((uintptr_t) phwBuffer) & 0x03) == 0x02) {
        // handle the leading pixel
        uint32_t wTemp = *phwBuffer;
        *phwBuffer++ = (uint16_t)__REV16(wTemp);
        wCount--;
    }

#ifdef USE_MVE_INTRINSICS
        do {
            mve_pred16_t    tailPred = vctp16q(wCount);
            uint16x8_t      rgb16vec = vld1q_z(phwBuffer, tailPred);

            rgb16vec = (uint16x8_t)vrev16q_m_u8(rgb16vec, rgb16vec, tailPred);

            vst1q_p(phwBuffer , rgb16vec , tailPred);

            phwBuffer += 8;
            wCount -= 8;
        }
        while ((int32_t)wCount > 0);

#else
    __asm volatile(
            ".p2align 2                                             \n"
            "   wlstp.16                lr, %[wCount], 1f           \n"
            "2:                                                     \n"

            "   vldrh.u16               q0, [%[phwBuffer]]          \n"
            "   vrev16.8                q0, q0                      \n"
            "   vstrh.u16               q0, [%[phwBuffer]], #16     \n"
            "   letp                    lr, 2b                      \n"
            "1:                                                     \n"

            : [phwBuffer] "+r"(phwBuffer)
            : [wCount] "r" (wCount)
            :"q0", "lr", "memory");
#endif
}



/*----------------------------------------------------------------------------*
 * Specialized Copy Routines                                                  *
 *----------------------------------------------------------------------------*/


static
void __arm_copy_16_mve_narrow(  uint16_t *phwSource,
                                                int16_t iSourceStride,
                                                uint16_t *phwTarget,
                                                int16_t iTargetStride,
                                                arm_2d_size_t *ptCopySize)
{
#ifdef USE_MVE_INTRINSICS
    for (int32_t x = 0; x < ptCopySize->iWidth; x++) {
        uint16x8_t      srcStr = vidupq_u16((uint32_t) 0, 1);
        uint16x8_t      dstStr = vidupq_u16((uint32_t) 0, 1);

        srcStr = srcStr * iSourceStride;
        dstStr = dstStr * iTargetStride;

        for (int32_t y = 0; y < ptCopySize->iHeight / 8; y++) {
            uint16x8_t      in = vldrhq_gather_shifted_offset_u16(phwSource, srcStr);
            srcStr = vaddq_n_u16(srcStr, (8 * iSourceStride));

            vstrhq_scatter_shifted_offset_u16(phwTarget, dstStr, in);
            dstStr = vaddq_n_u16(dstStr, (8 * iTargetStride));
        }

        phwSource++;
        phwTarget++;
    }
#else
    __asm volatile(
        "   clrm                {r2, r4}                               \n"
        "   vidup.u16           q0, r2, #1                             \n"
        "   vmul.i16            q2, q0, %[iSourceStride]               \n"
        "   vidup.u16           q1, r4, #1                             \n"
        "   vmul.i16            q3, q1, %[iTargetStride]               \n"

        "3:                                                            \n"
        /* outer loop, iterates over columns */
        /* size = ptCopySize->iWidth */
        "   vmov                q0, q2                                 \n"
        "   vmov                q1, q3                                 \n"

        /* inner loop, iterates over rows (size = ptCopySize->iHeight) */
        "   wlstp.16            lr, %[iHeight], 1f                     \n"
        ".p2align 2                                                    \n"
        "2:                                                            \n"
        "   vldrh.u16           q4, [%[phwSource], q0, uxtw #1]        \n"
        "   vadd.i16            q0, q0, %[iSourceStridex8]             \n"
        "   vstrh.16            q4, [%[phwTarget], q1, uxtw #1]        \n"
        "   vadd.i16            q1, q1, %[iTargetStridex8]             \n"
        "   letp                lr, 2b                                 \n"
        "1:                                                            \n"

        "   add.n               %[phwSource], #2                       \n"
        "   add.n               %[phwTarget], #2                       \n"
        "   subs                %[iWidth], #1                          \n"
        "   bne                 3b                                     \n"

        : [phwTarget] "+r"(phwTarget),  [phwSource] "+r"(phwSource)
        : [iHeight] "r"(ptCopySize->iHeight), [iWidth] "r" (ptCopySize->iWidth),
          [iSourceStride] "r" (iSourceStride),[iSourceStridex8] "r" (iSourceStride*8),
          [iTargetStride] "r" (iTargetStride),[iTargetStridex8] "r" (iTargetStride*8)
        : "r2", "r4", "q0", "q1", "q2", "q3", "q4", "memory", "r14", "cc");
#endif
}

static
void __arm_copy_32_mve_narrow(   uint32_t *pwSource,
                                                int32_t iSourceStride,
                                                uint32_t *pwTarget,
                                                int32_t iTargetStride,
                                                arm_2d_size_t *ptCopySize)
{
#ifdef USE_MVE_INTRINSICS
    for (int_fast32_t x = 0; x < ptCopySize->iWidth; x++) {
        uint32x4_t      srcStr = vidupq_u32((uint32_t) 0, 1);
        uint32x4_t      dstStr = vidupq_u32((uint32_t) 0, 1);

        srcStr = srcStr * iSourceStride;
        dstStr = dstStr * iTargetStride;

        for (int_fast32_t y = 0; y < ptCopySize->iHeight / 4; y++) {
            uint32x4_t      in = vldrwq_gather_shifted_offset_u32(pwSource, srcStr);
            srcStr = vaddq_n_u32(srcStr, (4 * iSourceStride));

            vstrwq_scatter_shifted_offset_u32(pwTarget, dstStr, in);
            dstStr = vaddq_n_u32(dstStr, (4 * iTargetStride));
        }

        pwSource++;
        pwTarget++;
    }
#else
    __asm volatile(

        "   clrm                {r2, r4}                              \n"
        "   vidup.u32           q0, r2, #1                            \n"
        "   vmul.i32            q2, q0, %[iSourceStride]              \n"
        "   vidup.u32           q1, r4, #1                            \n"
        "   vmul.i32            q3, q1, %[iTargetStride]              \n"

        "3:                                                           \n"
        /* outer loop, iterates over columns */
        /* size = ptCopySize->iWidth */
        "   vmov                q0, q2                                \n"
        "   vmov                q1, q3                                \n"

        /* inner loop, iterates over rows (size = ptCopySize->iHeight) */
        "   wlstp.32            lr, %[iHeight], 1f                    \n"
        ".p2align 2                                                   \n"
        "2:                                                           \n"
        "   vldrw.u32           q4, [%[pwSource], q0, uxtw #2]        \n"
        "   vadd.i32            q0, q0, %[iSourceStridex4]            \n"
        "   vstrw.32            q4, [%[pwTarget], q1, uxtw #2]        \n"
        "   vadd.i32            q1, q1, %[iTargetStridex4]            \n"
        "   letp                lr, 2b                                \n"
        "1:                                                           \n"

        "   add.n               %[pwSource], #4                       \n"
        "   add.n               %[pwTarget], #4                       \n"
        "   subs                %[iWidth], #1                         \n"
        "   bne                 3b                                    \n"

        : [pwTarget] "+r"(pwTarget),  [pwSource] "+r"(pwSource)
        : [iHeight] "r"(ptCopySize->iHeight), [iWidth] "r" (ptCopySize->iWidth),
          [iSourceStride] "r" (iSourceStride),[iSourceStridex4] "r" (iSourceStride*4),
          [iTargetStride] "r" (iTargetStride),[iTargetStridex4] "r" (iTargetStride*4)
        : "r2", "r4", "q0", "q1", "q2", "q3", "q4", "memory", "r14", "cc");
#endif
}




__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_rgb16_copy)(   uint16_t *phwSource,
                                int16_t iSourceStride,
                                uint16_t *phwTarget,
                                int16_t iTargetStride,
                                arm_2d_size_t *ptCopySize)
{
    /*
     * 16-bit Narrow copy case:
     * use column copy with scatter / gather
     */
    if(ptCopySize->iWidth <= 4) {
        __arm_copy_16_mve_narrow(phwSource,
                                    iSourceStride,
                                    phwTarget,
                                    iTargetStride,
                                    ptCopySize);

    } else  if((((uint32_t)phwSource & 3) == 0) && (((uint32_t)phwTarget & 3) == 0)
        && ((iSourceStride & 3) == 0) && ((iTargetStride & 3) ==0)) {
        /*
         * source / dst & strides are 64-bit aligned
         * use scalar LDRD/STRD, faster than back to back vector VLDR/VSTR on M55
         */
        __asm volatile(
            "3:                                                          \n"

            "   mov                 r0, %[phwSource]                     \n"
            "   mov                 r1, %[phwTarget]                     \n"

            /* scalar version faster (no DTCM bank conflict)*/
            "   wls                 lr, %[iWidth], 1f                    \n"
            ".p2align 2                                                  \n"
            "2:                                                          \n"
            "   ldrd                r2, r3, [r0], #8                     \n"
            "   strd                r2, r3, [r1], #8                     \n"
            "   le                  lr, 2b                               \n"
            "1:                                                          \n"
           // tail
            "   wls                 lr, %[iWidthTail], 1f                \n"
            ".p2align 2                                                  \n"
            "2:                                                          \n"
            "   ldrh                r2, [r0], #2                         \n"
            "   strh                r2, [r1], #2                         \n"
            "   le                  lr, 2b                               \n"
            "1:                                                          \n"

            "   add                 %[phwSource], %[iSourceStride]       \n"
            "   add                 %[phwTarget], %[iTargetStride]       \n"
            "   subs                %[iHeight], #1                       \n"
            "   bne                 3b                                   \n"

            : [phwTarget] "+r"(phwTarget),  [phwSource] "+r"(phwSource)
            : [iHeight] "r"(ptCopySize->iHeight), [iWidth] "r" (ptCopySize->iWidth/4),
              [iWidthTail] "r" (ptCopySize->iWidth & 3),
              [iSourceStride] "r" (iSourceStride*sizeof(uint16_t)),
              [iTargetStride] "r" (iTargetStride*sizeof(uint16_t))
            : "r0", "r1", "r2", "r3", "q0", "memory", "r14", "cc"
            );
        }
        else
        {
        /*
         * generic column major 16-bit 2D copy
         */
        int32_t iWidth = ptCopySize->iWidth;
        int32_t iHeight = ptCopySize->iHeight;

        __asm volatile(
            "   mov                 r2, %[iHeight]                  \n"
            "3:                                                     \n"

            "   mov                 r0, %[phwSource]                \n"
            "   mov                 r1, %[phwTarget]                \n"

            "   wlstp.16            lr, %[iWidth], 1f               \n"
            ".p2align 2                                             \n"
            "2:                                                     \n"
            "   vldrh.u16           q0, [r0], #16                   \n"
            "   vstrh.16            q0, [r1], #16                   \n"
            "   letp                lr, 2b                          \n"
            "1:                                                     \n"
            "   add                 %[phwSource], %[iSourceStride]  \n"
            "   add                 %[phwTarget], %[iTargetStride]  \n"
            "   subs                r2, #1                          \n"
            "   bne                 3b                              \n"

            : [phwTarget] "+r"(phwTarget),  [phwSource] "+r"(phwSource)
            : [iHeight] "r"(iHeight), [iWidth] "r" (iWidth),
              [iSourceStride] "r" (iSourceStride*sizeof(uint16_t)),
              [iTargetStride] "r" (iTargetStride*sizeof(uint16_t))
            : "r0", "r1", "r2", "q0", "memory", "r14", "cc");
    }
}

__OVERRIDE_WEAK
 void __MVE_WRAPPER( __arm_2d_impl_rgb32_copy)( uint32_t *pwSource,
                                int16_t iSourceStride,
                                uint32_t *pwTarget,
                                int16_t iTargetStride,
                                arm_2d_size_t *ptCopySize)
{
    if(ptCopySize->iWidth <= 2) {
        /*
         * 32-bit Narrow copy case:
         * use column copy with scatter / gather
         */
        __arm_copy_32_mve_narrow(pwSource,
                                    iSourceStride,
                                    pwTarget,
                                    iTargetStride,
                                    ptCopySize);

    } else  if((((uint32_t)pwSource & 3) == 0) && (((uint32_t)pwTarget & 3) == 0)
        && ((iSourceStride & 3) == 0) && ((iTargetStride & 3) ==0)) {
        /*
         * source / dst & strides are 64-bit aligned
         * use scalar LDRD/STRD, faster than back to back vector VLDR/VSTR on M55
         */
        __asm volatile(
            "3:                                                          \n"
            "   mov                 r0, %[pwSource]                      \n"
            "   mov                 r1, %[pwTarget]                      \n"

            /* scalar version faster (no DTCM bank conflict)*/
            "   wls                 lr, %[iWidth], 1f                    \n"
            ".p2align 2                                                  \n"
            "2:                                                          \n"
            "   ldrd                r2, r3, [r0], #8                     \n"
            "   strd                r2, r3, [r1], #8                     \n"
            "   le                  lr, 2b                               \n"
            "1:                                                          \n"
           // tail
            "   wls                 lr, %[iWidthTail], 1f                \n"
            ".p2align 2                                                  \n"
            "2:                                                          \n"
            "   ldr                 r2, [r0], #4                         \n"
            "   str                 r2, [r1], #4                         \n"
            "   le                  lr, 2b                               \n"
            "1:                                                          \n"

            "   add                 %[pwSource], %[iSourceStride]        \n"
            "   add                 %[pwTarget], %[iTargetStride]        \n"
            "   subs                %[iHeight], #1                       \n"
            "   bne                 3b                                   \n"

            : [pwTarget] "+r"(pwTarget),  [pwSource] "+r"(pwSource)
            : [iHeight] "r"(ptCopySize->iHeight), [iWidth] "r" (ptCopySize->iWidth/2),
              [iWidthTail] "r" (ptCopySize->iWidth & 1),
              [iSourceStride] "r" (iSourceStride*sizeof(uint32_t)),
              [iTargetStride] "r" (iTargetStride*sizeof(uint32_t))
            : "r0", "r1", "r2", "r3", "q0", "memory", "r14", "cc"
            );
        }
        else
        {
        /*
         * generic column major 32-bit 2D copy
         */
        __asm volatile(
            "3:                                                     \n"

            "   mov                 r0, %[pwSource]                 \n"
            "   mov                 r1, %[pwTarget]                 \n"

            "   wlstp.32            lr, %[iWidth], 1f               \n"
            ".p2align 2                                             \n"
            "2:                                                     \n"
            "   vldrw.32           q0, [r0], #16                    \n"
            "   vstrw.32            q0, [r1], #16                   \n"
            "   letp                lr, 2b                          \n"
            "1:                                                     \n"
            "   add                 %[pwSource], %[iSourceStride]   \n"
            "   add                 %[pwTarget], %[iTargetStride]   \n"
            "   subs                %[iHeight], #1                  \n"
            "   bne                 3b                              \n"

            : [pwTarget] "+r"(pwTarget),  [pwSource] "+r"(pwSource)
            : [iHeight] "r"(ptCopySize->iHeight), [iWidth] "r" (ptCopySize->iWidth),
              [iSourceStride] "r" (iSourceStride*sizeof(uint32_t)),
              [iTargetStride] "r" (iTargetStride*sizeof(uint32_t))
            : "r0", "r1", "q0", "memory", "r14", "cc");
        }
}

/*----------------------------------------------------------------------------*
 * alpha blending                                                             *
 *----------------------------------------------------------------------------*/

__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_gray8_alpha_blending)(uint8_t * __RESTRICT pSourceBase,
                                        int16_t iSourceStride,
                                        uint8_t * __RESTRICT pTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t * __RESTRICT ptCopySize,
                                        uint_fast16_t hwRatio)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif
    uint16_t        hwRatioCompl = 256 - hwRatio;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        const uint8_t *pSource = pSourceBase;
        uint8_t       *pTarget = pTargetBase;
        int32_t         blkCnt = iWidth;

#ifdef USE_MVE_INTRINSICS
        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);
            uint16x8_t      vecTgt = vldrbq_z_u16(pTarget, tailPred);
            uint16x8_t      vecSrc = vldrbq_z_u16(pSource, tailPred);

            vecTgt = vmulq_x(vecTgt, hwRatioCompl, tailPred);
            vecTgt = vmlaq_m(vecTgt, vecSrc, hwRatio, tailPred);
            vecTgt  = vecTgt  >> 8;

            vstrbq_p_u16(pTarget , vecTgt , tailPred);

            pSource += 8;
            pTarget += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

#else
        __asm volatile(
            "   vldrb.u16               q0, [%[pTarget]]             \n"
            ".p2align 2                                              \n"
            "   wls                     lr, %[loopCnt], 1f           \n"
            "2:                                                      \n"
            "   vmul.u16                q0, q0, %[hwRatioCompl]      \n"
            "   vldrb.u16               q1, [%[pSource]], #8         \n"
            "   vmla.s16                q0, q1, %[hwRatio]           \n"
            "   vldrb.u16               q2, [%[pTarget], #8]         \n"
            "   vshr.u16                q0, q0, #8                   \n"
            "   vstrb.u16               q0, [%[pTarget]], #8         \n"
            "   vmul.u16                q2, q2, %[hwRatioCompl]      \n"
            "   vldrb.u16               q1, [%[pSource]], #8         \n"
            "   vmla.s16                q2, q1, %[hwRatio]           \n"
            "   vldrb.u16               q0, [%[pTarget], #8]         \n"
            "   vshr.u16                q2, q2, #8                   \n"
            "   vstrb.u16               q2, [%[pTarget]], #8         \n"
            "   le                      lr, 2b                       \n"
            "1:                                                      \n"
            /* tail */
            "   wlstp.16                lr, %[tail], 1f              \n"
            "2:                                                      \n"
            "   vmul.u16                q0, q0, %[hwRatioCompl]      \n"
            "   vldrb.u16               q1, [%[pSource]], #8         \n"
            "   vmla.s16                q0, q1, %[hwRatio]           \n"
            "   vshr.u16                q1, q0, #8                   \n"
            "   vldrb.u16               q0, [%[pTarget], #8]         \n"
            "   vstrb.u16               q1, [%[pTarget]], #8         \n"
            "   letp                    lr, 2b                       \n"
            "1:                                                      \n"

            : [pTarget] "+r"(pTarget),  [pSource] "+r" (pSource)
            : [hwRatio] "r" (hwRatio), [hwRatioCompl] "r" (hwRatioCompl),
              [loopCnt] "r"(blkCnt/16), [tail] "r"(blkCnt & 0xf)
            :"q0", "q1", "q2", "memory", "r14");
#endif
        pSourceBase += (iSourceStride);
        pTargetBase += (iTargetStride);
   }
}

__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_gray8_alpha_blending_colour_keying)(uint8_t * __RESTRICT pSourceBase,
                                                      int16_t iSourceStride,
                                                      uint8_t * __RESTRICT pTargetBase,
                                                      int16_t iTargetStride,
                                                      arm_2d_size_t *
                                                      __RESTRICT ptCopySize,
                                                      uint_fast16_t hwRatio,
                                                      uint8_t Colour)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif
    uint16_t        hwRatioCompl = 256 - hwRatio;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        const uint8_t *pSource = pSourceBase;
        uint8_t       *pTarget = pTargetBase;
        int32_t         blkCnt = iWidth;

#ifdef USE_MVE_INTRINSICS
        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);
            uint16x8_t      vecTgt = vldrbq_z_u16(pTarget, tailPred);
            uint16x8_t      vecSrc = vldrbq_z_u16(pSource, tailPred);

            vecTgt = vmulq_x(vecTgt, hwRatioCompl, tailPred);
            vecTgt = vmlaq_m(vecTgt, vecSrc, hwRatio, tailPred);
            vecTgt  = vecTgt  >> 8;

            vstrbq_p_u16(pTarget , vecTgt ,
                vcmpneq_m_n_u16(vecSrc, (uint16_t)Colour, tailPred));

            pSource += 8;
            pTarget += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

#else
        __asm volatile(
            "   vldrb.u16               q0, [%[pTarget]]             \n"
            ".p2align 2                                              \n"
            "   wls                     lr, %[loopCnt], 1f           \n"
            "2:                                                      \n"
            "   vmul.u16                q0, q0, %[hwRatioCompl]      \n"
            "   vldrb.u16               q1, [%[pSource]], #8         \n"
            "   vmla.s16                q0, q1, %[hwRatio]           \n"
            "   vldrb.u16               q2, [%[pTarget], #8]         \n"
            "   vshr.u16                q0, q0, #8                   \n"
            "   vpt.u16                 ne, q1, %[Colour]            \n"
            "   vstrbt.u16              q0, [%[pTarget]], #8         \n"
            "   vmul.u16                q2, q2, %[hwRatioCompl]      \n"
            "   vldrb.u16               q1, [%[pSource]], #8         \n"
            "   vmla.s16                q2, q1, %[hwRatio]           \n"
            "   vldrb.u16               q0, [%[pTarget], #8]         \n"
            "   vshr.u16                q2, q2, #8                   \n"
            "   vpt.u16                 ne, q1, %[Colour]            \n"
            "   vstrbt.u16              q2, [%[pTarget]], #8         \n"
            "   le                      lr, 2b                       \n"
            "1:                                                      \n"
            /* tail */
            "   wlstp.16                lr, %[tail], 1f              \n"
            "2:                                                      \n"
            "   vmul.u16                q0, q0, %[hwRatioCompl]      \n"
            "   vldrb.u16               q1, [%[pSource]], #8         \n"
            "   vmla.s16                q0, q1, %[hwRatio]           \n"
            "   vshr.u16                q2, q0, #8                   \n"
            "   vldrb.u16               q0, [%[pTarget], #8]         \n"
            "   vpt.u16                 ne, q1, %[Colour]            \n"
            "   vstrbt.u16              q2, [%[pTarget]], #8         \n"
            "   letp                    lr, 2b                       \n"
            "1:                                                      \n"

            : [pTarget] "+r"(pTarget),  [pSource] "+r" (pSource)
            : [hwRatio] "r" (hwRatio), [hwRatioCompl] "r" (hwRatioCompl),
              [loopCnt] "r"(blkCnt/16), [tail] "r"(blkCnt & 0xf),
              [Colour] "r" (Colour)
            :"q0", "q1", "q2", "memory", "r14");
#endif
        pSourceBase += (iSourceStride);
        pTargetBase += (iTargetStride);
   }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_gray8_colour_filling_with_opacity)(uint8_t * __restrict pTargetBase,
                                                     int16_t iTargetStride,
                                                     arm_2d_size_t *
                                                     __restrict ptCopySize,
                                                     uint8_t Colour,
                                                     uint_fast16_t hwRatio)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif

    uint16_t        hwRatioCompl = 256 - hwRatio;

    uint16x8_t      vecSrc = vdupq_n_u16(Colour);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t       *pTarget = pTargetBase;
        int32_t        blkCnt = iWidth;

#ifdef USE_MVE_INTRINSICS
        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);
            uint16x8_t      vecTgt = vldrbq_z_u16(pTarget, tailPred);

            vecTgt = vmulq_x(vecTgt, hwRatioCompl, tailPred);
            vecTgt = vmlaq_m(vecTgt, vecSrc, hwRatio, tailPred);
            vecTgt  = vecTgt  >> 8;

            vstrbq_p_u16(pTarget , vecTgt , tailPred);

            pTarget += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

#else
        __asm volatile(
            "   vldrb.u16               q0, [%[pTarget]]             \n"
            "   vmul.u16                q0, q0, %[hwRatioCompl]      \n"
            ".p2align 2                                              \n"
            "   wls                     lr, %[loopCnt], 1f           \n"
            "2:                                                      \n"

            "   vmla.s16                q0, %[vecSrc], %[hwRatio]    \n"
            "   vldrb.u16               q2, [%[pTarget], #8]         \n"
            "   vshr.u16                q0, q0, #8                   \n"
            "   vmul.u16                q2, q2, %[hwRatioCompl]      \n"
            "   vstrb.u16               q0, [%[pTarget]], #8         \n"
            "   vmla.s16                q2, %[vecSrc], %[hwRatio]    \n"
            "   vldrb.u16               q0, [%[pTarget], #8]         \n"
            "   vshr.u16                q2, q2, #8                   \n"
            "   vmul.u16                q0, q0, %[hwRatioCompl]      \n"
            "   vstrb.u16               q2, [%[pTarget]], #8         \n"
            "   le                      lr, 2b                       \n"
            "1:                                                      \n"
            /* tail */
            "   wlstp.16                lr, %[tail], 1f              \n"
            "2:                                                      \n"

            "   vmla.s16                q0, %[vecSrc], %[hwRatio]    \n"
            "   vshr.u16                q2, q0, #8                   \n"
            "   vldrb.u16               q0, [%[pTarget], #8]         \n"
            "   vmul.u16                q0, q0, %[hwRatioCompl]      \n"
            "   vstrb.u16               q2, [%[pTarget]], #8         \n"
            "   letp                    lr, 2b                       \n"
            "1:                                                      \n"

            : [pTarget] "+r"(pTarget)
            : [hwRatio] "r" (hwRatio), [hwRatioCompl] "r" (hwRatioCompl),
              [loopCnt] "r"(blkCnt/16), [tail] "r"(blkCnt & 0xf),
              [vecSrc] "t" (vecSrc)
            :"q0", "q2", "memory", "r14");
#endif
        pTargetBase += (iTargetStride);
   }
}





__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_rgb565_alpha_blending)(   uint16_t *phwSourceBase,
                                            int16_t iSourceStride,
                                            uint16_t *phwTargetBase,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *ptCopySize,
                                            uint_fast16_t hwRatio)
{
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif

#ifdef USE_MVE_INTRINSICS
    int32_t         blkCnt;
    uint16_t        ratio1x8 = hwRatio * 8;
    uint16_t        ratio1x4 = hwRatio * 4;
    uint16_t        ratio2x8 = (256 - hwRatio) * 8;
    uint16_t        ratio2x4 = (256 - hwRatio) * 4;

    uint16x8_t      vecMaskR = vdupq_n_u16(0x001f);
    uint16x8_t      vecMaskG = vdupq_n_u16(0x003f);
    uint16x8_t      vecMaskBpck = vdupq_n_u16(0x00f8);
    uint16x8_t      vecMaskGpck = vdupq_n_u16(0x00fc);


    for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
        const uint16_t *phwSource = phwSourceBase;
        uint16_t       *phwTarget = phwTargetBase;

        blkCnt = ptCopySize->iWidth;
        do {

            uint16x8_t      vecIn;
            uint16x8_t      vecR0, vecB0, vecG0;
            uint16x8_t      vecR1, vecB1, vecG1;

            /* unpack 1st stream */
            vecIn = vld1q(phwSource);
            vecR0 = vecIn & vecMaskR;

            vecB0 = vecIn >> 11;

            vecG0 = vecIn >> 5;
            vecG0 = vecG0 & vecMaskG;


            /* unpack 2nd stream */
            vecIn = vld1q(phwTarget);
            vecR1 = vecIn & vecMaskR;

            vecB1 = vecIn >> 11;

            vecG1 = vecIn >> 5;
            vecG1 = vecG1 & vecMaskG;


            /* merge */
            vecR0 = vecR0 * ratio1x8 + vecR1 * ratio2x8;
            vecR0 = vecR0 >> 8;

            vecG0 = vecG0 * ratio1x4 + vecG1 * ratio2x4;
            vecG0 = vecG0 >> 8;

            vecB0 = vecB0 * ratio1x8 + vecB1 * ratio2x8;
            vecB0 = vecB0 >> 8;


            /* pack */
            uint16x8_t      vOut = vecR0 >> 3 | vmulq((vecG0 & vecMaskGpck), 8)
                | vmulq((vecB0 & vecMaskBpck), 256);


            vst1q(phwTarget, vOut);

            phwSource += 8;
            phwTarget += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

        phwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;
    }

#else /* USE_MVE_INTRINSICS  */


    uint16_t        ratio1x8 = hwRatio * 8;
    uint16_t        ratio1x4 = hwRatio * 4;
    uint16_t        ratio2x8 = (256 - hwRatio) * 8;
    uint16_t        ratio2x4 = (256 - hwRatio) * 4;
    uint16x8_t      vecMaskR = vdupq_n_u16(0x001f);
    uint16x8_t      vecMaskG = vdupq_n_u16(0x003f);
    uint16x8_t      vecMaskBpck = vdupq_n_u16(0x00f8);
    uint32_t        iWidth = ptCopySize->iWidth;
    int32_t         row = ptCopySize->iHeight;
    uint16x8_t      scratch[1];

    vst1q((uint16_t *)scratch, vdupq_n_u16(0x00fc));

    do {
        const uint16_t *pSource = phwSourceBase;
        uint16_t       *pTarget = phwTargetBase;
        register unsigned loopCnt  __asm("lr");
        loopCnt = iWidth;

    __asm volatile(
        ".p2align 2                                              \n"
        "   vldrh.u16               q4, [%[pTarget]]             \n"
        "   vldrh.u16               q5, [%[pSource]], #16        \n"

        "   wlstp.16                lr, %[loopCnt], 1f           \n"
        "2:                                                      \n"
        // B target extraction
        // right shift by 5 (x 1/32) for M55 friendly
        // IV / Mul pipe interleaving
        "   vqdmulh.s16             q2, q4, %[rshft5]            \n"
        "   vand                    q7, q4, %[vecMaskR]          \n"

        "   vmul.i16                q6, q7, %[ratio2x8]          \n"
        // B source extraction
        "   vand                    q7, q5, %[vecMaskR]          \n"
        // B mix
        "   vmla.s16                q6, q7, %[ratio1x8]          \n"
        // G extraction
        "   vand                    q2, q2, %[vecMaskG]          \n"
        "   vshr.u16                q7, q5, #5                   \n"
        "   vmul.i16                q2, q2, %[ratio2x4]          \n"
        // G extraction
        "   vand                    q7, q7, %[vecMaskG]          \n"
        // G mix
        "   vmla.s16                q2, q7, %[ratio1x4]          \n"
        // R extraction
        "   vshr.u16                q4, q4, #11                  \n"
        "   vmul.i16                q7, q4, %[ratio2x8]          \n"
        // R extraction
        "   vshr.u16                q5, q5, #11                  \n"
        // R mix
        "   vmla.s16                q7, q5, %[ratio1x8]          \n"

        "   vshr.u16                q2, q2, #8                   \n"
        "   vldrh.16                q5, [%[scratch]]             \n"

        "   vand                    q2, q2, q5                   \n"
        // vmulq((vecG0 & 0x00fc), 8)
        "   vmul.i16                q2, q2, %[eight]             \n"
        "   vshr.u16                q4, q7, #8                   \n"
        // schedule next source load
        "   vldrh.u16               q5, [%[pSource]], #16        \n"
        "   vand                    q7, q4, %[vecMaskBpck]       \n"
        // pack R & G
        // vmulq((vecG0 & vecMaskGpck), 8) + vmulq((vecR0 & vecMaskRpck), 256)
        "   vmla.s16                q2, q7, %[twofiftysix]       \n"
        // downshift B ((vecB0 >> 8) >> 3)
        "   vshr.u16                q7, q6, #11                  \n"
        // schedule next target load (pre offset as target not imcrementred so far)
        "   vldrh.u16               q4, [%[pTarget], #16]        \n"
        // pack blue with R&G
        "   vorr                    q2, q2, q7                   \n"

        "   vstrh.16                q2, [%[pTarget]], #16        \n"
        "   letp                    lr, 2b                       \n"
        "1:                                                      \n"

        : [pSource] "+r"(pSource),  [pTarget] "+r" (pTarget), [loopCnt] "+r"(loopCnt)
        : [vecMaskR] "t" (vecMaskR), [vecMaskG] "t" (vecMaskG),
          [vecMaskBpck] "t" (vecMaskBpck),
          [ratio1x8] "r" (ratio1x8), [ratio2x8] "r" (ratio2x8),
          [ratio1x4] "r" (ratio1x4), [ratio2x4] "r" (ratio2x4),
          [eight] "r" (8), [twofiftysix] "r" (256), [rshft5] "r" (1024), [scratch] "r" (scratch)
        : "q2", "q4", "q5", "q6", "q7", "memory" );

        phwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;
    } while (--row);
#endif /* USE_MVE_INTRINSICS */
}






__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_rgb565_colour_filling_with_opacity)(
                                        uint16_t *__RESTRICT pTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint16_t Colour,
                                        uint_fast16_t hwRatio)
{

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif

#ifdef USE_MVE_INTRINSICS
    int32_t         blkCnt;
    uint16_t        ratio1x8 = hwRatio * 8;
    uint16_t        ratio1x4 = hwRatio * 4;
    uint16_t        ratio2x8 = (256 - hwRatio) * 8;
    uint16_t        ratio2x4 = (256 - hwRatio) * 4;

    uint16x8_t      vecMaskR = vdupq_n_u16(0x001f);
    uint16x8_t      vecMaskG = vdupq_n_u16(0x003f);
    uint16x8_t      vecMaskBpck = vdupq_n_u16(0x00f8);
    uint16x8_t      vecMaskGpck = vdupq_n_u16(0x00fc);
    uint16x8_t      vecIn;
    uint16x8_t      vecColorR, vecColorB, vecColorG;

    /* unpack color & scale */
    vecIn = vdupq_n_u16(Colour);
    vecColorR = (vecIn & vecMaskR) * ratio1x8;

    vecColorB = (vecIn >> 11) * ratio1x8;

    vecColorG = vecIn >> 5;
    vecColorG = (vecColorG & vecMaskG) * ratio1x4;

    for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
        uint16_t       *phwTarget = pTargetBase;

        blkCnt = ptCopySize->iWidth;
        do {
            uint16x8_t      vecR0, vecB0, vecG0;
            uint16x8_t      vecR1, vecB1, vecG1;

            /* unpack stream */
            vecIn = vld1q(phwTarget);
            vecR1 = vecIn & vecMaskR;

            vecB1 = vecIn >> 11;

            vecG1 = vecIn >> 5;
            vecG1 = vecG1 & vecMaskG;


            /* merge */
            vecR0 = vecColorR + vecR1 * ratio2x8;
            vecR0 = vecR0 >> 8;

            vecG0 = vecColorG + vecG1 * ratio2x4;
            vecG0 = vecG0 >> 8;

            vecB0 = vecColorB + vecB1 * ratio2x8;
            vecB0 = vecB0 >> 8;

            /* pack */
            uint16x8_t      vOut = vecR0 >> 3 | vmulq((vecG0 & vecMaskGpck), 8)
                | vmulq((vecB0 & vecMaskBpck), 256);

            vst1q(phwTarget, vOut);

            phwTarget += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

        pTargetBase += iTargetStride;
    }

#else /* USE_MVE_INTRINSICS  */

    uint16_t        ratio1x8 = hwRatio * 8;
    uint16_t        ratio1x4 = hwRatio * 4;
    uint16_t        ratio2x8 = (256 - hwRatio) * 8;
    uint16_t        ratio2x4 = (256 - hwRatio) * 4;
    uint16x8_t      vecMaskR = vdupq_n_u16(0x001f);
    uint16x8_t      vecMaskG = vdupq_n_u16(0x003f);
    uint16x8_t      vecMaskBpck = vdupq_n_u16(0x00f8);
    uint16x8_t      vecColorR, vecColorB, vecColorG;
    uint16x8_t      scratch[4];

    /* unpack color */
    uint16x8_t vecIn = vdupq_n_u16(Colour);
    vecColorR = vecIn & vecMaskR;
    vecColorB = vecIn >> 11;
    vecColorG = vecIn >> 5;
    vecColorG = vecColorG & vecMaskG;
    vst1q((uint16_t*)scratch, vecColorR * ratio1x8);
    vst1q((uint16_t*)&scratch[1], vecColorB * ratio1x8);
    vst1q((uint16_t*)&scratch[2], vecColorG * ratio1x4);
    vst1q((uint16_t*)&scratch[3], vdupq_n_u16(0x00fc));

    int32_t         row = ptCopySize->iHeight;
    do {

        uint16_t       *phwTarget = pTargetBase;
        register unsigned loopCnt  __asm("lr");
        loopCnt = ptCopySize->iWidth;

    __asm volatile(
        "   vldrh.u16               q4, [%[phwTarget]]           \n"

        "   wlstp.16                lr, %[loopCnt], 1f           \n"
        ".p2align 2                                              \n"
        "2:                                                      \n"
        // B target extraction
        "   vand                    q7, q4, %[vecMaskR]          \n"
        "   vldrh.u16               q6, [%[scratch]]             \n"
        "   vshr.u16                q2, q4, #5                   \n"

        // B mix
        "   vmla.s16                q6, q7, %[ratio2x8]          \n"
        // G extraction
        "   vand                    q7, q2, %[vecMaskG]          \n"

        // G extraction
        "   vldrh.u16               q2, [%[scratch], #32]        \n"
        // G mix
        "   vmla.s16                q2, q7, %[ratio2x4]          \n"

        "   vshr.u16                q4, q4, #11                  \n"
        // R extraction
        "   vldrh.u16               q7, [%[scratch], #16]        \n"
        "   vshr.u16                q2, q2, #8                   \n"
        // R mix
        "   vmla.s16                q7, q4, %[ratio2x8]          \n"
        "   vshr.u16                q4, q7, #8                   \n"

        // load duplicated 0xfc mask
        "   vldrh.u16               q7, [%[scratch], #48]        \n"
        "   vand                    q2, q2, q7                   \n"

        "   vmul.i16                q2, q2, %[eight]             \n"
        "   vand                    q7, q4, %[vecMaskBpck]       \n"

        // pack R & G
        "   vmla.s16                q2, q7, %[twofiftysix]       \n"
        // downshift B ((vecB0 >> 8) >> 3)
        "   vshr.u16                q7, q6, #11                  \n"
        // schedule next target load
        "   vldrh.u16               q4, [%[phwTarget], #16]      \n"
        // pack blue with R&G
        "   vorr                    q2, q2, q7                   \n"
        "   vstrh.16                q2, [%[phwTarget]], #16      \n"
        "   letp                    lr, 2b                       \n"
        "1:                                                      \n"
        : [phwTarget] "+r" (phwTarget), [loopCnt] "+r"(loopCnt)
        : [vecMaskR] "t" (vecMaskR), [vecMaskG] "t" (vecMaskG),
          [vecMaskBpck] "t" (vecMaskBpck),
          [ratio2x8] "r" (ratio2x8), [ratio2x4] "r" (ratio2x4),
          [eight] "r" (8), [twofiftysix] "r" (256), [scratch] "r" (scratch)
        : "q2", "q4", "q5", "q6", "q7", "memory" );

        pTargetBase += iTargetStride;
    } while (--row);

#endif /* USE_MVE_INTRINSICS */

}


__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_rgb565_alpha_blending_colour_keying)(
                                                uint16_t * __RESTRICT phwSource,
                                                int16_t         iSourceStride,
                                                uint16_t * __RESTRICT phwTarget,
                                                int16_t         iTargetStride,
                                                arm_2d_size_t * __RESTRICT ptCopySize,
                                                uint_fast16_t hwRatio,
                                                uint16_t   hwColour)
{
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif

#ifdef USE_MVE_INTRINSICS
    uint32_t        iHeight = ptCopySize->iHeight;
    uint32_t        iWidth = ptCopySize->iWidth;

    int32_t         blkCnt;
    uint16_t        ratio1x8 = hwRatio * 8;
    uint16_t        ratio1x4 = hwRatio * 4;
    uint16_t        ratio2x8 = (256 - hwRatio) * 8;
    uint16_t        ratio2x4 = (256 - hwRatio) * 4;

    uint16x8_t      vecMaskR = vdupq_n_u16(0x001f);
    uint16x8_t      vecMaskG = vdupq_n_u16(0x003f);
    uint16x8_t      vecMaskBpck = vdupq_n_u16(0x00f8);
    uint16x8_t      vecMaskGpck = vdupq_n_u16(0x00fc);

    for (uint32_t y = 0; y < iHeight; y++) {
        // - inconditional blending + predicated dst update
        const uint16_t *pSource = phwSource;
        uint16_t       *pTarget = phwTarget;
        blkCnt = iWidth >> 3;

        while (blkCnt > 0) {
            uint16x8_t      vecInSrc, vecInDst;
            uint16x8_t      vecR0, vecB0, vecG0;
            uint16x8_t      vecR1, vecB1, vecG1;

            /* unpack 1st stream */
            vecInSrc = vld1q(pSource);
            vecR0 = vandq(vecInSrc, vecMaskR);
            vecB0 = vshrq(vecInSrc, 11);
            vecG0 = vshrq(vecInSrc, 5);
            vecG0 = vandq(vecG0, vecMaskG);

            /* unpack 2nd stream */
            vecInDst = vld1q(pTarget);
            vecR1 = vandq(vecInDst, vecMaskR);
            vecB1 = vshrq(vecInDst, 11);
            vecG1 = vshrq(vecInDst, 5);
            vecG1 = vandq(vecG1, vecMaskG);

            /* merge */
            vecR0 = vmlaq(vmulq(vecR0, ratio1x8), vecR1, ratio2x8);
            vecR0 = vshrq(vecR0, 8);
            vecG0 = vmlaq(vmulq(vecG0, ratio1x4), vecG1, ratio2x4);
            vecG0 = vshrq(vecG0, 8);
            vecB0 = vmlaq(vmulq(vecB0, ratio1x8), vecB1, ratio2x8);
            vecB0 = vshrq(vecB0, 8);

            /* pack */
            uint16x8_t      vOut = vorrq(vshrq(vecR0, 3),
                                         vmulq(vandq(vecG0, vecMaskGpck), 8));

            vOut = vorrq(vOut, vmulq(vandq(vecB0, vecMaskBpck), 256));

            vst1q_p(pTarget, vOut, vcmpneq_n_s16(vecInSrc, hwColour));

            pSource += 8;
            pTarget += 8;
            blkCnt--;

        }

        blkCnt = iWidth & 7;
        if (blkCnt > 0U) {
            uint16x8_t      vecInSrc, vecInDst;
            uint16x8_t      vecR0, vecB0, vecG0;
            uint16x8_t      vecR1, vecB1, vecG1;

            /* unpack 1st stream */
            vecInSrc = vld1q(pSource);
            vecR0 = vandq(vecInSrc, vecMaskR);
            vecB0 = vshrq(vecInSrc, 11);
            vecG0 = vshrq(vecInSrc, 5);
            vecG0 = vandq(vecG0, vecMaskG);

            /* unpack 2nd stream */
            vecInDst = vld1q(pTarget);
            vecR1 = vandq(vecInDst, vecMaskR);
            vecB1 = vshrq(vecInDst, 11);
            vecG1 = vshrq(vecInDst, 5);
            vecG1 = vandq(vecG1, vecMaskG);

            /* merge */
            vecR0 = vmlaq(vmulq(vecR0, ratio1x8), vecR1, ratio2x8);
            vecR0 = vshrq(vecR0, 8);
            vecG0 = vmlaq(vmulq(vecG0, ratio1x4), vecG1, ratio2x4);
            vecG0 = vshrq(vecG0, 8);
            vecB0 = vmlaq(vmulq(vecB0, ratio1x8), vecB1, ratio2x8);
            vecB0 = vshrq(vecB0, 8);

            /* pack */
            uint16x8_t      vOut = vorrq(vshrq(vecR0, 3),
                                         vmulq(vandq(vecG0, vecMaskGpck), 8));

            vOut = vorrq(vOut,
                    vmulq(vandq(vecB0, vecMaskBpck), 256));

            vst1q_p(pTarget, vOut,
                    vcmpneq_m_n_s16(vecInSrc, hwColour, vctp16q(blkCnt)));

        }

        phwSource += iSourceStride;
        phwTarget += iTargetStride;
    }
#else
    uint32_t        iHeight = ptCopySize->iHeight;
    uint32_t        iWidth = ptCopySize->iWidth;

    uint16_t        ratio1x8 = hwRatio * 8;
    uint16_t        ratio1x4 = hwRatio * 4;
    uint16_t        ratio2x8 = (256 - hwRatio) * 8;
    uint16_t        ratio2x4 = (256 - hwRatio) * 4;

    uint16x8_t      vecMaskR = vdupq_n_u16(0x001f);
    uint16x8_t      vecMaskG = vdupq_n_u16(0x003f);
    uint16x8_t      vecMaskBpck = vdupq_n_u16(0x00f8);
    uint16x8_t      scratch[1];

    vst1q((uint16_t *)scratch, vdupq_n_u16(0x00fc));

    for (uint32_t y = 0; y < iHeight; y++) {

        const uint16_t *pSource = phwSource;
        uint16_t       *pTarget = phwTarget;
        register unsigned loopCnt  __asm("lr");
        loopCnt = iWidth;

    __asm volatile(
        ".p2align 2                                              \n"
        "   vldrh.u16               q4, [%[pTarget]]             \n"
        "   vldrh.u16               q5, [%[pSource]], #16        \n"
        "   vand                    q7, q4, %[vecMaskR]          \n"
        "   wlstp.16                lr, %[loopCnt], 1f           \n"
        "2:                                                      \n"
        // B target extraction
        "   vshr.u16                q2, q4, #5                   \n"
        "   vmul.i16                q6, q7, %[ratio2x8]          \n"
        // B source extraction
        "   vand                    q7, q5, %[vecMaskR]          \n"
        // B mix
        "   vmla.s16                q6, q7, %[ratio1x8]          \n"
        // G extraction
        "   vand                    q2, q2, %[vecMaskG]          \n"
        "   vshr.u16                q7, q5, #5                   \n"
        "   vmul.i16                q2, q2, %[ratio2x4]          \n"
        // G extraction
        "   vand                    q7, q7, %[vecMaskG]          \n"
        // G mix
        "   vmla.s16                q2, q7, %[ratio1x4]          \n"
        // R extraction
        "   vshr.u16                q4, q4, #11                  \n"
        "   vmul.i16                q7, q4, %[ratio2x8]          \n"
        // R extraction
        "   vshr.u16                q5, q5, #11                  \n"
        // R mix
        "   vmla.s16                q7, q5, %[ratio1x8]          \n"

        "   vshr.u16                q2, q2, #8                   \n"
        "   vldrh.16                q5, [%[scratch]]             \n"

        "   vand                    q2, q2, q5                   \n"
        // vmulq((vecG0 & 0x00fc), 8)
        "   vmul.i16                q2, q2, %[eight]             \n"
        "   vshr.u16                q4, q7, #8                   \n"
        // schedule next source load
        "   vldrh.u16               q5, [%[pSource]], #16        \n"
        "   vand                    q7, q4, %[vecMaskBpck]       \n"
        // pack R & G
        // vmulq((vecG0 & vecMaskGpck), 8) + vmulq((vecR0 & vecMaskRpck), 256)
        "   vmla.s16                q2, q7, %[twofiftysix]       \n"
        // downshift B ((vecB0 >> 8) >> 3)
        "   vshr.u16                q7, q6, #11                  \n"
        // schedule next target load (pre offset as target not imcrementred so far)
        "   vldrh.u16               q4, [%[pTarget], #16]        \n"
        // pack blue with R&G
        "   vorr                    q2, q2, q7                   \n"
        "   vldrh.u16               q6, [%[pSource], #-32]       \n"
        "   vand                    q7, q4, %[vecMaskR]          \n"
        "   vpt.u16                 ne, q6, %[hwColour]          \n"
        "   vstrht.16               q2, [%[pTarget]], #16        \n"
        "   letp                    lr, 2b                       \n"
        "1:                                                      \n"

        : [pSource] "+r"(pSource),  [pTarget] "+r" (pTarget), [loopCnt] "+r"(loopCnt)
        : [vecMaskR] "t" (vecMaskR), [vecMaskG] "t" (vecMaskG),
          [vecMaskBpck] "t" (vecMaskBpck),
          [ratio1x8] "r" (ratio1x8), [ratio2x8] "r" (ratio2x8),
          [ratio1x4] "r" (ratio1x4), [ratio2x4] "r" (ratio2x4),
          [eight] "r" (8), [twofiftysix] "r" (256), [hwColour] "r" (hwColour), [scratch] "r" (scratch)
        : "q2", "q4", "q5", "q6", "q7", "memory" );

        phwSource += (iSourceStride);
        phwTarget += (iTargetStride);
    }
#endif
}




__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_cccn888_alpha_blending)(   uint32_t *pwSourceBase,
                                            int16_t iSourceStride,
                                            uint32_t *pwTargetBase,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *ptCopySize,
                                            uint_fast16_t hwRatio)
{
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif
    uint16_t        hwRatioCompl = 256 - (uint16_t) hwRatio;

#ifdef USE_MVE_INTRINSICS


    int32_t         blkCnt;
    int32_t         row = ptCopySize->iHeight;

    while (row > 0) {

        const uint32_t *pwSource = pwSourceBase;
        uint32_t       *pwTarget = pwTargetBase;
        /* byte extraction into 16-bit vector */
        uint16x8_t      vecSrc = vldrbq_u16((const uint8_t *)pwSource);
        uint16x8_t      vecTrg = vldrbq_u16((const uint8_t *)pwTarget);

        pwSource += 2;
        blkCnt = ptCopySize->iWidth;

        while (blkCnt > 0) {
            vstrbq_u16((const uint8_t *)pwTarget,
                       vmlaq(vmulq(vecSrc, hwRatio), vecTrg, hwRatioCompl) >> 8);

            pwTarget += 2;

            vecSrc = vldrbq_u16((const uint8_t *)pwSource);
            vecTrg = vldrbq_u16((const uint8_t *)pwTarget);
            pwSource += 2;
            blkCnt -= 2;
        }

        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;
        row--;
    }
#else

    register unsigned blkCnt  __asm("lr");
    int32_t row = ptCopySize->iHeight;

    while(row > 0)
    {
        blkCnt = ptCopySize->iWidth*4;
        const uint32_t *pwSource = pwSourceBase;
        uint32_t *pwTarget = pwTargetBase;

        __asm volatile(
            "   vldrb.u16               q0, [%[pwSource]], #8          \n"
            "   vldrb.u16               q1, [%[pwTarget]]              \n"

            "   wlstp.16                lr, %[loopCnt], 1f             \n"
            "2:                                                        \n"
            "   vmul.u16                q2, q0, %[hwRatio]             \n"
            "   vldrb.u16               q0, [%[pwSource]], #8          \n"
            "   vmla.s16                q2, q1, %[hwRatioCompl]        \n"
            "   vldrb.u16               q1, [%[pwTarget], #8]          \n"
            "   vshr.u16                q2, q2, #8                     \n"
            "   vstrb.16                q2, [%[pwTarget]], #8          \n"
            "   letp                    lr, 2b                         \n"
            "1:                                                        \n"

            : [pwSource] "+l"(pwSource),  [pwTarget] "+l"(pwTarget),
              [loopCnt] "+r"(blkCnt)
            : [hwRatio] "r" (hwRatio), [hwRatioCompl] "r" (hwRatioCompl)
            : "q0", "q1", "q2", "memory" );

        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;
        row--;
    }
#endif
}




__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_cccn888_colour_filling_with_opacity)(
                                        uint32_t *__RESTRICT pTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint32_t Colour,
                                        uint_fast16_t hwRatio)
{
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif
    uint16_t        hwRatioCompl = 256 - (uint16_t) hwRatio;

#ifdef USE_MVE_INTRINSICS
    int32_t         blkCnt;
    int32_t         row = ptCopySize->iHeight;
    uint32_t        scratch[2];
    uint16x8_t      vColor;

    scratch[0] = scratch[1] = Colour;
    vColor = vldrbq_u16((uint8_t *) scratch);
    vColor = vColor * (uint16_t)hwRatio;

    while (row > 0) {
        uint32_t       *pTarget = pTargetBase;
        blkCnt = ptCopySize->iWidth;

        while (blkCnt > 0) {
            /* byte extraction into 16-bit vector */
            uint16x8_t      vecTrg = vldrbq_u16((uint8_t *)pTarget);

            vstrbq_u16((uint8_t *)pTarget, vmlaq(vColor, vecTrg, hwRatioCompl) >> 8);

            pTarget += 2;
            blkCnt -= 2;
        }
        pTargetBase += iTargetStride;
        row--;
    }
#else /* USE_MVE_INTRINSICS  */

    int32_t         blkCnt;
    int32_t         row = ptCopySize->iHeight;
    uint32_t        scratch[2];
    uint16x8_t      vColor;

    scratch[0] = scratch[1] = Colour;
    vColor = vldrbq_u16((uint8_t *) scratch);
    vColor = vColor * (uint16_t)hwRatio;

    while (row > 0) {
        uint32_t       *pTarget = pTargetBase;
        blkCnt = ptCopySize->iWidth*4;

    __asm volatile(
        /* preload */
        "   vldrb.u16               q1, [%[pTarget]]               \n"

        "   wlstp.16                lr, %[loopCnt], 1f             \n"
        ".p2align 2                                                \n"
        "2:                                                        \n"
        "   vmov                    q2, %[vColor]                  \n"
        "   vmla.s16                q2, q1, %[hwRatioCompl]        \n"
        "   vldrb.u16               q1, [%[pTarget], #8]           \n"
        "   vshr.u16                q2, q2, #8                     \n"
        "   vstrb.16                q2, [%[pTarget]], #8           \n"
        "   letp                    lr, 2b                         \n"
        "1:                                                        \n"
        : [pTarget] "+l"(pTarget)
        : [loopCnt] "r"(blkCnt), [hwRatioCompl] "r" (hwRatioCompl), [vColor] "t" (vColor)
        : "q0", "q1", "q2", "memory" );

        pTargetBase += iTargetStride;
        row--;
    }

#endif /* USE_MVE_INTRINSICS */
}

__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_cccn888_alpha_blending_colour_keying)(uint32_t * __RESTRICT pSourceBase,
                                                       int16_t iSourceStride,
                                                       uint32_t * __RESTRICT pTargetBase,
                                                       int16_t iTargetStride,
                                                       arm_2d_size_t *
                                                       __RESTRICT ptCopySize,
                                                       uint_fast16_t hwRatio,
                                                       uint32_t Colour)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif
    uint16_t        hwRatioCompl = 256 - hwRatio;

    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint32_t *pSource = pSourceBase;
        uint32_t       *pTarget = pTargetBase;

#ifdef USE_MVE_INTRINSICS
        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    p = vctp32q(blkCnt);

            uint8x16_t      vSrc8 = vld1q_z(pSource, p);
            uint8x16_t      vTrg8 = vld1q_z(pTarget, p);

            /* 16-bit expansion A/G src pixels */
            uint16x8_t      vSrc16b = vmovlbq_x(vSrc8, p);
            /* 16-bit expansion R/B src pixels */
            uint16x8_t      vSrc16t = vmovltq_x(vSrc8, p);

            /* 16-bit expansion A/G target pixels */
            uint16x8_t      vTrg16b = vmovlbq_x(vTrg8, p);
            /* 16-bit expansion R/B target pixels */
            uint16x8_t      vTrg16t = vmovltq_x(vTrg8, p);

            /* A/G blending */
            int16x8_t       vecOutb = vmlaq_m(vmulq_x(vSrc16b, hwRatio, p), vTrg16b, hwRatioCompl, p);
            /* R/B blending */
            int16x8_t       vecOutt = vmlaq_m(vmulq_x(vSrc16t, hwRatio, p), vTrg16t, hwRatioCompl, p);

            /* merge into 8-bit vector */
            int8x16_t       vecOut8 = vuninitializedq_s8();

            vecOut8 = vqshrnbq_m_n_s16(vecOut8, vecOutb, 8, p);
            vecOut8 = vqshrntq_m_n_s16(vecOut8, vecOutt, 8, p);

            // update if (*pSourceBase != Colour)
            vst1q_p_u32(pTarget, (uint32x4_t) vecOut8,
                        vcmpneq_m_n_u32((uint32x4_t) vSrc8, Colour, p));

            pSource += 4;
            pTarget += 4;
            blkCnt -= 4;
        }
        while (blkCnt > 0);

#else // USE_MVE_INTRINSICS

    __asm volatile (
        ".p2align 2                                 \n"
        /* preload uint32x4_t target vector */
        "   vldrw.u32       q2, [%[targ]]           \n"

        "   wlstp.32        lr, %[loopCnt], 1f      \n"
        "2:                                         \n"
        /* 16-bit expansion A/G target pixels */
        "   vmovlb.u8       q3, q2                  \n"
        "   vldrw.u32       q0, [%[src]], #16       \n"
        /* 16-bit expansion A/G source pixels */
        "   vmovlb.u8       q1, q0                  \n"
        "   vmul.i16        q1, q1, %[ratio]        \n"
        /* 16-bit expansion R/B target pixels */
        "   vmovlt.u8       q2, q2                  \n"
        /* A/G blending */
        "   vmla.s16        q1, q3, %[ratioCmp]     \n"
        /* 16-bit expansion R/B source pixels */
        "   vmovlt.u8       q3, q0                  \n"
        "   vmul.i16        q3, q3, %[ratio]        \n"
        /* merge A/G into 8-bit vector */
        "   vqshrnb.s16     q1, q1, #8              \n"
        /* R/B blending */
        "   vmla.s16        q3, q2, %[ratioCmp]     \n"
        /* preload next target */
        "   vldrw.u32       q2, [%[targ], #16]      \n"
        /* merge R/B into 8-bit vector */
        "   vqshrnt.s16     q1, q3, #8              \n"
        /* update if (*pSourceBase != Colour) */
        "   vpt.i32         ne, q0, %[color]        \n"
        "   vstrwt.32       q1, [%[targ]], #16      \n"
        "   letp            lr, 2b                  \n"
        "1:                                         \n"
        :[targ] "+r" (pTarget), [src] "+r" (pSource)
        :[loopCnt] "r" (iWidth), [ratio] "r" (hwRatio),
         [ratioCmp] "r" (hwRatioCompl), [color] "r" (Colour)
        :"r14", "q0", "q1", "q2", "q3", "memory");
#endif
        pSourceBase += (iSourceStride);
        pTargetBase += (iTargetStride);
    }
}



__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_rgb565_alpha_blending_direct)(const uint16_t *phwSource,
                                                const uint16_t *phwBackground,
                                                uint16_t *phwDestination,
                                                uint32_t wPixelCount,
                                                uint_fast16_t hwRatio)
{
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif

#ifdef USE_MVE_INTRINSICS
    int32_t         blkCnt;
    uint16_t        ratio1x8 = hwRatio * 8;
    uint16_t        ratio1x4 = hwRatio * 4;
    uint16_t        ratio2x8 = (256 - hwRatio) * 8;
    uint16_t        ratio2x4 = (256 - hwRatio) * 4;

    uint16x8_t      vecMaskR = vdupq_n_u16(0x001f);
    uint16x8_t      vecMaskG = vdupq_n_u16(0x003f);
    uint16x8_t      vecMaskBpck = vdupq_n_u16(0x00f8);
    uint16x8_t      vecMaskGpck = vdupq_n_u16(0x00fc);

    blkCnt = wPixelCount;
    do {

        uint16x8_t      vecIn;
        uint16x8_t      vecR0, vecB0, vecG0;
        uint16x8_t      vecR1, vecB1, vecG1;

        /* unpack 1st stream */
        vecIn = vld1q(phwSource);
        phwSource += 8;

        vecR0 = vecIn & vecMaskR;

        vecB0 = vecIn >> 11;

        vecG0 = vecIn >> 5;
        vecG0 = vecG0 & vecMaskG;


        /* unpack 2nd stream */
        vecIn = vld1q(phwBackground);
        phwBackground += 8;

        vecR1 = vecIn & vecMaskR;

        vecB1 = vecIn >> 11;

        vecG1 = vecIn >> 5;
        vecG1 = vecG1 & vecMaskG;


        /* merge */
        vecR0 = vecR0 * ratio1x8 + vecR1 * ratio2x8;
        vecR0 = vecR0 >> 8;

        vecG0 = vecG0 * ratio1x4 + vecG1 * ratio2x4;
        vecG0 = vecG0 >> 8;

        vecB0 = vecB0 * ratio1x8 + vecB1 * ratio2x8;
        vecB0 = vecB0 >> 8;


        /* pack */
        uint16x8_t      vOut =
            vecR0 >> 3 | vmulq((vecG0 & vecMaskGpck), 8)
            | vmulq((vecB0 & vecMaskBpck), 256);

        vst1q(phwDestination, vOut);
        phwDestination += 8;

        blkCnt -= 8;
    }
    while (blkCnt > 0);

#else /* USE_MVE_INTRINSICS */

    uint16_t        ratio1x8 = hwRatio * 8;
    uint16_t        ratio1x4 = hwRatio * 4;
    uint16_t        ratio2x8 = (256 - hwRatio) * 8;
    uint16_t        ratio2x4 = (256 - hwRatio) * 4;
    uint16x8_t      vecMaskR = vdupq_n_u16(0x001f);
    uint16x8_t      vecMaskG = vdupq_n_u16(0x003f);
    uint16x8_t      vecMaskBpck = vdupq_n_u16(0x00f8);

     register unsigned loopCnt  __asm("lr") = (wPixelCount);

    __asm volatile(

        "   vldrh.u16               q4, [%[in2]], #16            \n"
        "   vmov.i16                q6, #0x00fc                  \n"
        "   vstrw.32                q6, [sp]                     \n"
        "   vldrh.u16               q5, [%[in1]], #16            \n"
        "   wlstp.16                lr, %[loopCnt], 1f           \n"
        "2:                                                      \n"

        "   vand                    q6, q4, %[vecMaskR]          \n"
        "   vmul.i16                q6, q6, %[ratio2x8]          \n"
        "   vshr.u16                q2, q4, #5                   \n"
        "   vand                    q7, q5, %[vecMaskR]          \n"
        "   vmla.s16                q6, q7, %[ratio1x8]          \n"
        "   vand                    q2, q2, %[vecMaskG]          \n"
        "   vshr.u16                q7, q5, #5                   \n"
        "   vmul.i16                q2, q2, %[ratio2x4]          \n"
        "   vand                    q7, q7, %[vecMaskG]          \n"
        "   vmla.s16                q2, q7, %[ratio1x4]          \n"
        "   vshr.u16                q4, q4, #11                  \n"
        "   vmul.i16                q7, q4, %[ratio2x8]          \n"
        "   vshr.u16                q5, q5, #11                  \n"
        "   vshr.u16                q2, q2, #8                   \n"
        "   vmla.s16                q7, q5, %[ratio1x8]          \n"

        //  "   vmov.i16                 q6, #0x00fc  \n"
        "   vshr.u16                q7, q7, #8                   \n"
        //  "   vmov.i16                 q6, #0x00fc  \n"
        /* load 0x00fc instead of mov for better overlap opportunity */
        "   vldrw.u32               q4, [sp]                     \n"
        "   vand                    q2, q2, q4                   \n"
        "   vmul.i16                q2, q2, %[eight]             \n"
        "   vand                    q4, q7, %[vecMaskBpck]       \n" // Q7 = vecB0
        "   vldrh.u16               q5, [%[in1]], #16            \n"
        "   vmla.s16                q2, q4, %[twofiftysix]       \n"
        // (vecR0 >> 3) >> 8
        "   vshr.u16                q6, q6, #11                  \n"
        "   vldrh.u16               q4, [%[in2]], #16            \n"
        "   vorr                    q2, q2, q6                   \n"
        "   vstrh.16                q2, [%[out]], #16            \n"
        "   letp                    lr, 2b                       \n"
        "1:                                                      \n"

        : [in1] "+r"(phwSource),  [in2] "+r"(phwBackground),
          [out] "+r" (phwDestination), [loopCnt] "+r"(loopCnt)
        : [vecMaskR] "t" (vecMaskR), [vecMaskG] "t" (vecMaskG),
          [vecMaskBpck] "t" (vecMaskBpck),
          [ratio1x8] "r" (ratio1x8), [ratio2x8] "r" (ratio2x8),
          [ratio1x4] "r" (ratio1x4), [ratio2x4] "r" (ratio2x4),
          [eight] "r" (8), [twofiftysix] "r" (256)
        : "q2", "q4", "q5", "q6", "q7", "memory" );

#endif
}

__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_cccn888_alpha_blending_direct)(const uint32_t *pwSource,
                                                const uint32_t *pwBackground,
                                                uint32_t *pwDestination,
                                                uint32_t wPixelCount,
                                                uint_fast16_t hwRatio)
{
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif
    uint16_t        hwRatioCompl = 256 - hwRatio;

#ifdef USE_MVE_INTRINSICS
    int32_t         blkCnt;

    uint16x8_t      vecSrc, vecBckg;


    vecSrc = vldrbq_u16((uint8_t const *) pwSource);
    pwSource += 2;
    vecBckg = vldrbq_u16((uint8_t const *) pwBackground);
    pwBackground += 2;


    blkCnt = wPixelCount;
    do {
        uint16x8_t      vecOut;

        vecOut = vmulq_n_u16(vecSrc, (uint16_t) hwRatio);
        vecSrc = vldrbq_u16((uint8_t const *) pwSource);
        pwSource += 2;

        vecOut = vmlaq_n_u16(vecOut, vecBckg, hwRatioCompl);
        vecBckg = vldrbq_u16((uint8_t const *) pwBackground);
        pwBackground += 2;

        vecOut = vecOut >> 8;

        vstrbq_u16((uint8_t *) pwDestination, vecOut);
        pwDestination += 2;

        blkCnt -= 2;
    }
    while (blkCnt > 0);

#else /* USE_MVE_INTRINSICS */
    register unsigned blkCnt  __asm("lr") = (wPixelCount * 4);

    __asm volatile(
        "   vldrb.u16               q0, [%[pwSource]], #8          \n"
        "   vldrb.u16               q1, [%[pwBackg]], #8           \n"

        "   wlstp.16                lr, %[loopCnt], 1f             \n"
        "2:                                                        \n"
        "   vmul.u16                q2, q0, %[hwRatio]             \n"
        "   vldrb.u16               q0, [%[pwSource]], #8          \n"
        "   vmla.s16                q2, q1, %[hwRatioCompl]        \n"
        "   vldrb.u16               q1, [%[pwBackg]], #8           \n"
        "   vshr.u16                q2, q2, #8                     \n"
        "   vstrb.16                q2, [%[pwDest]], #8            \n"
        "   letp                    lr, 2b                         \n"
        "1:                                                        \n"

        : [pwSource] "+l"(pwSource),  [pwBackg] "+l"(pwBackground),
          [pwDest] "+l" (pwDestination), [loopCnt] "+r"(blkCnt)
        : [hwRatio] "r" (hwRatio), [hwRatioCompl] "r" (hwRatioCompl)
        : "q0", "q1", "q2", "memory" );
#endif

}



/* rgb8_draw_pattern helpers */

/*
 * enable to pick gather load offset based on initial offset
 * e.g. if iOffset = 3
 * will get {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2}
 */
static uint8_t __draw_pattern_src_incr_c8bit[32] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3
};

/*
 * enable to pick vector bitmask based on initial offset
 * e.g. if iOffset = 3
 * will get {8, 16, 32, 64, 128, 1, 2, 4, 8, 16, 32, 64, 128, 1, 2, 4}
 */

static  uint8_t __draw_pattern_src_bitmask_c8bit[32] = {
    1, 2, 4, 8, 16, 32, 64, 128,
    1, 2, 4, 8, 16, 32, 64, 128,
    1, 2, 4, 8, 16, 32, 64, 128,
    1, 2, 4, 8, 16, 32, 64, 128,
};


/* rgb16_draw_pattern helpers */

/*
 * enable to pick gather load offset based on initial offset
 * e.g. if iOffset = 3
 * will get {0, 0, 0, 0, 0, 1, 1, 1}
 */
static uint16_t __draw_pattern_src_incr_rgb16[16] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1
};

/*
 * enable to pick vector bitmask based on initial offset
 * e.g. if iOffset = 3
 * will get {8, 16, 32, 64, 128, 1, 2, 4}
 */

static  uint16_t __draw_pattern_src_bitmask_rgb16[16] = {
    1, 2, 4, 8, 16, 32, 64, 128,
    1, 2, 4, 8, 16, 32, 64, 128,
};


/* rgb32_draw_pattern helpers */

static uint32_t __draw_pattern_src_incr_rgb32[16] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1
};


static  uint32_t __draw_pattern_src_bitmask_rgb32[16] = {
    1, 2, 4, 8, 16, 32, 64, 128,
    1, 2, 4, 8, 16, 32, 64, 128,
};



/*! adding support with c code template */
#define __API_COLOUR        c8bit
#define __API_ELT_SZ        8
#include "__arm_2d_draw_pattern_helium.inc"


#define __API_COLOUR        rgb16
#define __API_ELT_SZ        16
#include "__arm_2d_draw_pattern_helium.inc"


#define __API_COLOUR        rgb32
#define __API_ELT_SZ        32
#include "__arm_2d_draw_pattern_helium.inc"



#define __API_COLOUR        c8bit
#define __API_ELT_SZ        8
#include "__arm_2d_fill_colour_helium.inc"


#define __API_COLOUR        rgb16
#define __API_ELT_SZ        16
#include "__arm_2d_fill_colour_helium.inc"


#define __API_COLOUR        rgb32
#define __API_ELT_SZ        32
#include "__arm_2d_fill_colour_helium.inc"


/**
  8-bit pixel color fill alpha/channel mask with/without opacity MVE intrinsic generator
   - TRGT_LOAD is a contigous / strided target load function
        C8BIT_TRGT_LOAD / C8BIT_TRGT_LOAD_STRIDE
  - STRIDE is an optional vector of offset for gather load
  - SCAL_OPACITY is extra alpha scaling function
        C8BIT_SCAL_OPACITY_NONE / C8BIT_SCAL_OPACITY
  - OPACITY is an optinal 8-bit vector with duplicated opacity values
  (need vector format to be used with VMULH.U8)
  - ALPHA_SZ, alpha chan width (1 or 4 for resp. 8 or 32-bit type)

  Macro assumes pTarget8/ pAlpha are already setup
 */




#define C8BIT_COLOUR_FILLING_MASK_INNER_MVE(TRGT_LOAD, STRIDE, SCAL_OPACITY,                   \
                                                             OPACITY, ALPHA_SZ, COMPVAL)       \
        int32_t         blkCnt = iWidth;                                                       \
        do {                                                                                   \
            mve_pred16_t    tailPred = vctp16q(blkCnt);                                        \
                                                                                               \
            uint16x8_t      vecTarget = vldrbq_z_u16(pTarget8, tailPred);                      \
            uint16x8_t      vecTransp = TRGT_LOAD(pAlpha, STRIDE, tailPred);                   \
                                                                                               \
            vecTransp = SCAL_OPACITY(vecTransp, OPACITY, tailPred);                            \
                                                                                               \
            ALPHA_255_COMP_VEC16(vecTransp, COMPVAL);                                                \
                                                                                               \
            uint16x8_t      vecAlpha = vsubq_x_u16(v256, vecTransp, tailPred);                 \
                                                                                               \
            vecTarget = vmulq_x(vecTarget, vecAlpha, tailPred);                                \
            vecTarget = vmlaq_m(vecTarget, vecTransp, (uint16_t) Colour, tailPred);            \
            vecTarget = vecTarget >> 8;                                                        \
                                                                                               \
            vstrbq_p_u16(pTarget8, vecTarget, tailPred);                                       \
                                                                                               \
            pAlpha += (8 * ALPHA_SZ);                                                          \
            pTarget8 += 8;                                                                     \
            blkCnt -= 8;                                                                       \
        }                                                                                      \
        while (blkCnt > 0);


/**
  RGB565 pixel color fill alpha/channel mask with/without opacity MVE intrinsic generator
   - TRGT_LOAD is a contigous / strided target load function
        RGB565_TRGT_LOAD / RGB565_TRGT_LOAD_STRIDE
  - STRIDE is an optional vector of offset for gather load
  - SCAL_OPACITY is extra alpha scaling function
        RGB565_SCAL_OPACITY_NONE / RGB565_SCAL_OPACITY
  - OPACITY is an optinal 8-bit vector with duplicated opacity values
  (need vector format to be used with VMULH.U8)
  - P_ALPHA, 8-bit or 32-bit alpha chan. pointer
  - ALPHA_SZ, alpha chan width (1 or 4 for resp. 8 or 32-bit type)

  Does not generate a tail-predicated loop as relying on pack/unpack functions.
  Predication is only applied in the final stage during pixel store.
 */

#define RGB565_COLOUR_FILLING_MASK_MVE(TRGT_LOAD, STRIDE, SCAL_OPACITY, OPACITY,               \
                                                            P_ALPHA,  ALPHA_SZ, COMPVAL)       \
    uint16x8_t      v256 = vdupq_n_u16(256);                                                   \
                                                                                               \
    for (int_fast16_t y = 0; y < iHeight; y++) {                                               \
        const uint8_t  *pAlpha = (const uint8_t *)P_ALPHA;                                     \
        uint16_t       *pCurTarget = pTarget;                                                  \
        int32_t         blkCnt = iWidth;                                                       \
                                                                                               \
        do {                                                                                   \
            uint16x8_t      vecTarget = vld1q(pCurTarget);                                     \
            uint16x8_t      vecTransp = TRGT_LOAD(pAlpha, STRIDE);                             \
            vecTransp = SCAL_OPACITY(vecTransp, OPACITY);                                      \
                                                                                               \
            ALPHA_255_COMP_VEC16(vecTransp, COMPVAL);                                                \
                                                                                               \
            uint16x8_t      vecAlpha = vsubq_u16(v256, vecTransp);                             \
            uint16x8_t      vecR, vecG, vecB;                                                  \
                                                                                               \
            __arm_2d_rgb565_unpack_single_vec(vecTarget, &vecR, &vecG, &vecB);                 \
                                                                                               \
            /* blending using alpha vector weights */                                          \
            vecR = vmulq(vecR, vecAlpha);                                                      \
            vecR = vmlaq(vecR, vecTransp, (uint16_t) tSrcPix.R);                               \
            vecR = vecR >> 8;                                                                  \
                                                                                               \
            vecG = vmulq(vecG, vecAlpha);                                                      \
            vecG = vmlaq(vecG, vecTransp, (uint16_t) tSrcPix.G);                               \
            vecG = vecG >> 8;                                                                  \
                                                                                               \
            vecB = vmulq(vecB, vecAlpha);                                                      \
            vecB = vmlaq(vecB, vecTransp, (uint16_t) tSrcPix.B);                               \
            vecB = vecB >> 8;                                                                  \
                                                                                               \
            vecTarget = __arm_2d_rgb565_pack_single_vec(vecR, vecG, vecB);                     \
                                                                                               \
            /* tail predication */                                                             \
            vst1q_p_u16(pCurTarget, vecTarget, vctp16q(blkCnt));                               \
                                                                                               \
            pAlpha += (8 *  ALPHA_SZ);                                                         \
            pCurTarget += 8;                                                                   \
            blkCnt -= 8;                                                                       \
        }                                                                                      \
        while (blkCnt > 0);                                                                    \
                                                                                               \
        P_ALPHA += (iAlphaStride);                                                             \
        pTarget += (iTargetStride);                                                            \
    }


/**
  CCCN888 pixel color fill alpha/channel mask with/without opacity MVE intrinsic generator
   - TRGT_LOAD is a contigous / strided target load function
        CCCN888_TRGT_LOAD / CCCN888_TRGT_LOAD_STRIDE
  - STRIDE is an optional vector of offset for gather load
  - SCAL_OPACITY is extra alpha scaling function
        CCCN888_SCAL_OPACITY_NONE / CCCN888_SCAL_OPACITY
  - OPACITY is an optinal 8-bit vector with duplicated opacity values
  (need vector format to be used with VMULH.U8)
  - ALPHA_SZ, alpha chan width (1 or 4 for resp. 8 or 32-bit type)

  Macro assumes pTargetCh0/1/2 & pAlpha are already setup
 */

#define CCCN888_COLOUR_FILLING_MASK_INNER_MVE(TRGT_LOAD, STRIDE, SCAL_OPACITY,                 \
                                                                  OPACITY, ALPHA_SZ, COMPVAL)  \
        int32_t         blkCnt = iWidth;                                                       \
                                                                                               \
        do {                                                                                   \
            mve_pred16_t    tailPred = vctp16q(blkCnt);                                        \
                                                                                               \
            /* expand chan0, chan1, chan2 */                                                   \
            uint16x8_t      vecTargetC0 = vldrbq_gather_offset_z_u16(pTargetCh0, vStride4Offs, \
                tailPred);                                                                     \
            uint16x8_t      vecTargetC1 = vldrbq_gather_offset_z_u16(pTargetCh1, vStride4Offs, \
                tailPred);                                                                     \
            uint16x8_t      vecTargetC2 = vldrbq_gather_offset_z_u16(pTargetCh2, vStride4Offs, \
                tailPred);                                                                     \
            uint16x8_t      vecTransp = TRGT_LOAD(pAlpha, STRIDE, tailPred);                   \
                                                                                               \
            vecTransp = SCAL_OPACITY(vecTransp, OPACITY, tailPred);                            \
                                                                                               \
            ALPHA_255_COMP_VEC16(vecTransp, COMPVAL);                                          \
                                                                                               \
            uint16x8_t      vecAlpha = vsubq_x_u16(v256, vecTransp, tailPred);                 \
                                                                                               \
                                                                                               \
            /*  scale ch0 vector with alpha vector */                                          \
            vecTargetC0 = vmulq_x(vecTargetC0, vecAlpha, tailPred);                            \
            /*  blend ch0 vector with input ch0 color*/                                        \
            vecTargetC0 = vmlaq_m(vecTargetC0, vecTransp, (uint16_t) c0, tailPred);            \
            vecTargetC0 = vecTargetC0 >> 8;                                                    \
                                                                                               \
            /* repeat for ch1 and ch2 */                                                       \
            vecTargetC1 = vmulq_x(vecTargetC1, vecAlpha, tailPred);                            \
            vecTargetC1 = vmlaq_m(vecTargetC1, vecTransp, (uint16_t) c1, tailPred);            \
            vecTargetC1 = vecTargetC1 >> 8;                                                    \
                                                                                               \
            vecTargetC2 = vmulq_x(vecTargetC2, vecAlpha, tailPred);                            \
            vecTargetC2 = vmlaq_m(vecTargetC2, vecTransp, (uint16_t) c2, tailPred);            \
            vecTargetC2 = vecTargetC2 >> 8;                                                    \
                                                                                               \
            /* store and merge chan0, chan1, chan2 */                                          \
            vstrbq_scatter_offset_p_u16(pTargetCh0, vStride4Offs, vecTargetC0, tailPred);      \
            vstrbq_scatter_offset_p_u16(pTargetCh1, vStride4Offs, vecTargetC1, tailPred);      \
            vstrbq_scatter_offset_p_u16(pTargetCh2, vStride4Offs, vecTargetC2, tailPred);      \
                                                                                               \
            pAlpha += 8 * ALPHA_SZ;                                                            \
            pTargetCh0 += 8*4;                                                                 \
            pTargetCh1 += 8*4;                                                                 \
            pTargetCh2 += 8*4;                                                                 \
            blkCnt -= 8;                                                                       \
        }                                                                                      \
        while (blkCnt > 0);



#define C8BIT_TRGT_LOAD(base, stride, pred)             vldrbq_z_u16(base, pred)
#define C8BIT_TRGT_LOAD_STRIDE(base, stride, pred)      vldrbq_gather_offset_z_u16(base, stride, pred);
#define C8BIT_SCAL_OPACITY_NONE(transp, opac, pred)     transp
#define C8BIT_SCAL_OPACITY(transp, opac, pred)          (uint16x8_t) vmulhq_x((uint8x16_t) transp, opac, pred)

#define RGB565_TRGT_LOAD(base, stride)                  vldrbq_u16(base)
#define RGB565_TRGT_LOAD_STRIDE(base, stride)           vldrbq_gather_offset_u16(base, stride);
#define RGB565_SCAL_OPACITY_NONE(transp, opac)          transp
#define RGB565_SCAL_OPACITY(transp, opac)               (uint16x8_t) vmulhq((uint8x16_t) transp, opac)

#define CCCN888_TRGT_LOAD(base, stride, pred)           vldrbq_z_u16(base, pred)
#define CCCN888_TRGT_LOAD_STRIDE(base, stride, pred)    vldrbq_gather_offset_z_u16(base, stride, pred);
#define CCCN888_SCAL_OPACITY_NONE(transp, opac, pred)   transp
#define CCCN888_SCAL_OPACITY(transp, opac, pred)        (uint16x8_t) vmulhq_x((uint8x16_t) transp, opac, pred)



__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_gray8_colour_filling_mask)(uint8_t * __RESTRICT pTarget,
                                                       int16_t iTargetStride,
                                                       uint8_t * __RESTRICT pchAlpha,
                                                       int16_t iAlphaStride,
                                                       arm_2d_size_t * __RESTRICT ptCopySize,
                                                       uint8_t Colour)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    uint16x8_t      v256 = vdupq_n_u16(256);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint8_t * pAlpha = pchAlpha;
        uint8_t *       pTarget8 = pTarget;

#ifdef USE_MVE_INTRINSICS
        C8BIT_COLOUR_FILLING_MASK_INNER_MVE(C8BIT_TRGT_LOAD, _,
                                        C8BIT_SCAL_OPACITY_NONE, _, 1, 255);
#else
        register unsigned blkCnt __asm("lr");
        blkCnt = iWidth;

    __asm volatile(
            "vecAlphaCompl            .req q2                        \n"

            ".p2align 2                                              \n"
            "   vldrb.u16               q0, [%[pTarget]]             \n"
            "   vldrb.u16               q1, [%[pAlpha]], #8          \n"
            "   wlstp.16                lr, %[loopCnt], 1f           \n"
            "2:                                                      \n"
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            /* if alpha == 255, boost to 256 */
            "   vpt.i16                 eq, q1, %[alph255]           \n"
            "   vmovt.i16               q1, #256                     \n"
#endif

            "   vsub.i16                vecAlphaCompl, %[vec256], q1 \n"
            "   vmul.u16                q3, q0, vecAlphaCompl        \n"
            "   vldrb.u16               q0, [%[pTarget], #8]         \n"
            "   vmla.u16                q3, q1, %[Colour]            \n"
            "   vldrb.u16               q1, [%[pAlpha]], #8          \n"
            "   vshr.u16                q3, q3, #8                   \n"
            "   vstrb.u16               q3, [%[pTarget]], #8         \n"
            "   letp                    lr, 2b                       \n"
            "1:                                                      \n"

            " .unreq vecAlphaCompl                                   \n"
            : [pTarget] "+l"(pTarget8),  [pAlpha] "+l" (pAlpha), [loopCnt] "+r"(blkCnt)
            :[vec256] "t"   (v256),[Colour] "r"(Colour)
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            ,[alph255] "r" (255)
#endif
            :"q0", "q1", "q2", "q3", "memory");
#endif
        pchAlpha += (iAlphaStride);
        pTarget += (iTargetStride);
    }
}

__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_gray8_colour_filling_mask_opacity)(uint8_t * __RESTRICT pTarget,
                                                               int16_t iTargetStride,
                                                               uint8_t * __RESTRICT pchAlpha,
                                                               int16_t iAlphaStride,
                                                               arm_2d_size_t *
                                                               __RESTRICT ptCopySize,
                                                               uint8_t Colour,
                                                               uint_fast16_t hwOpacity)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    uint8x16_t      vOpacity = vdupq_n_u8(hwOpacity);
    uint16x8_t      v256 = vdupq_n_u16(256);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint8_t * pAlpha = pchAlpha;
        uint8_t *       pTarget8 = pTarget;

#ifdef USE_MVE_INTRINSICS
        C8BIT_COLOUR_FILLING_MASK_INNER_MVE(C8BIT_TRGT_LOAD, _,
                                        C8BIT_SCAL_OPACITY, vOpacity, 1, 254);
#else
        register unsigned blkCnt __asm("lr");
        blkCnt = iWidth;

    __asm volatile(
        "vecAlphaCompl            .req q2                        \n"

        ".p2align 2                                              \n"
        "   vldrb.u16               q0, [%[pTarget]]             \n"
        "   vldrb.u16               q1, [%[pAlpha]], #8          \n"
        "   vmulh.u8                q1, q1, %[vOpacity]          \n"
        "   wlstp.16                lr, %[loopCnt], 1f           \n"
        "2:                                                      \n"

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        /* if vOpacity == 254, boost to 256 */
        "   vpt.i16                 eq, q1, %[opa254]            \n"
        "   vmovt.i16               q1, #256                     \n"
#endif

        "   vsub.i16                vecAlphaCompl, %[vec256], q1 \n"
        "   vmul.u16                q3, q0, vecAlphaCompl        \n"
        "   vldrb.u16               q0, [%[pTarget], #8]         \n"
        "   vmla.u16                q3, q1, %[Colour]            \n"
        "   vldrb.u16               q1, [%[pAlpha]], #8          \n"
        "   vmulh.u8                q1, q1, %[vOpacity]          \n"
        "   vshr.u16                q3, q3, #8                   \n"
        "   vstrb.u16               q3, [%[pTarget]], #8         \n"
        "   letp                    lr, 2b                       \n"
        "1:                                                      \n"

        " .unreq vecAlphaCompl                                   \n"
        : [pTarget] "+l"(pTarget8),  [pAlpha] "+l" (pAlpha), [loopCnt] "+r"(blkCnt)
        :[vec256] "t"   (v256),[Colour] "r"(Colour),[vOpacity] "t"(vOpacity)
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
         ,[opa254] "r" (254)
#endif
        :"q0", "q1", "q2", "q3", "memory");

#endif
        pchAlpha += (iAlphaStride);
        pTarget += (iTargetStride);
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_gray8_colour_filling_channel_mask)(uint8_t * __RESTRICT pTarget,
                                                         int16_t iTargetStride,
                                                         uint32_t * __RESTRICT pwAlpha,
                                                         int16_t iAlphaStride,
                                                         arm_2d_size_t * __RESTRICT ptCopySize,
                                                         uint8_t Colour)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    uint16x8_t      v256 = vdupq_n_u16(256);
    uint16x8_t      vStride4Offs = vidupq_n_u16(0, 4);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint8_t  *pAlpha = (const uint8_t *)pwAlpha;
        uint8_t *       pTarget8 = pTarget;

#ifdef USE_MVE_INTRINSICS
        C8BIT_COLOUR_FILLING_MASK_INNER_MVE(C8BIT_TRGT_LOAD_STRIDE, vStride4Offs,
                                        C8BIT_SCAL_OPACITY_NONE, _, 4, 255);
#else
        register unsigned blkCnt __asm("lr");
        blkCnt = iWidth;

    __asm volatile(
        "vecAlphaCompl            .req q2                        \n"

        ".p2align 2                                              \n"
        "   vldrb.u16               q0, [%[pTarget]]             \n"
        "   vldrb.u16               q1, [%[pAlpha], %[str4Offs]] \n"
        "   wlstp.16                lr, %[loopCnt], 1f           \n"
        "2:                                                      \n"
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        /* if alpha == 255, boost to 256 */
        "   vpt.i16                 eq, q1, %[alph255]           \n"
        "   vmovt.i16               q1, #256                     \n"
#endif

        "   add                     %[pAlpha], %[pAlpha], #(8*4) \n"
        "   vsub.i16                vecAlphaCompl, %[vec256], q1 \n"
        "   vmul.u16                q3, q0, vecAlphaCompl        \n"
        "   vldrb.u16               q0, [%[pTarget], #8]         \n"
        "   vmla.s16                q3, q1, %[Colour]            \n"
        "   vldrb.u16               q1, [%[pAlpha], %[str4Offs]] \n"
        "   vshr.u16                q3, q3, #8                   \n"
        "   vstrb.u16               q3, [%[pTarget]], #8         \n"
        "   letp                    lr, 2b                       \n"
        "1:                                                      \n"

        " .unreq vecAlphaCompl                                   \n"
        : [pTarget] "+l"(pTarget8),  [pAlpha] "+r" (pAlpha), [loopCnt] "+r"(blkCnt)
        :[vec256] "t"   (v256),[Colour] "r"(Colour),[str4Offs] "t"(vStride4Offs)
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        ,[alph255] "r" (255)
#endif
        :"q0", "q1", "q2", "q3", "memory");

#endif
        pwAlpha += (iAlphaStride);
        pTarget += (iTargetStride);
    }
}




__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_gray8_colour_filling_channel_mask_opacity)(uint8_t * __RESTRICT pTarget,
                                                                 int16_t iTargetStride,
                                                                 uint32_t * __RESTRICT pwAlpha,
                                                                 int16_t iAlphaStride,
                                                                 arm_2d_size_t *
                                                                 __RESTRICT ptCopySize,
                                                                 uint8_t Colour,
                                                                 uint_fast16_t hwOpacity)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    uint8x16_t      vOpacity = vdupq_n_u8(hwOpacity);
    uint16x8_t      v256 = vdupq_n_u16(256);
    uint16x8_t      vStride4Offs = vidupq_n_u16(0, 4);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint8_t  *pAlpha = (const uint8_t *)pwAlpha;
        uint8_t        *pTarget8 = pTarget;

#ifdef USE_MVE_INTRINSICS
        C8BIT_COLOUR_FILLING_MASK_INNER_MVE(C8BIT_TRGT_LOAD_STRIDE, vStride4Offs,
                                        C8BIT_SCAL_OPACITY, vOpacity, 4, 254);
#else
        register unsigned blkCnt __asm("lr");
        blkCnt = iWidth;

    __asm volatile(
        "vecAlphaCompl            .req q2                        \n"

        ".p2align 2                                              \n"
        "   vldrb.u16               q0, [%[pTarget]]             \n"
        "   vldrb.u16               q1, [%[pAlpha], %[str4Offs]] \n"
        "   vmulh.u8                q1, q1, %[vOpacity]          \n"
        "   wlstp.16                lr, %[loopCnt], 1f           \n"
        "2:                                                      \n"

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        /* if vOpacity == 254, boost to 256 */
        "   vpt.i16                 eq, q1, %[opa254]            \n"
        "   vmovt.i16               q1, #256                     \n"
#endif

        "   add                     %[pAlpha], %[pAlpha], #(8*4) \n"

        "   vsub.i16                vecAlphaCompl, %[vec256], q1 \n"
        "   vmul.u16                q3, q0, vecAlphaCompl        \n"
        "   vldrb.u16               q0, [%[pTarget], #8]         \n"
        "   vmla.s16                q3, q1, %[Colour]            \n"
        "   vldrb.u16               q1, [%[pAlpha], %[str4Offs]] \n"
        "   vmulh.u8                q1, q1, %[vOpacity]          \n"
        "   vshr.u16                q3, q3, #8                   \n"
        "   vstrb.u16               q3, [%[pTarget]], #8         \n"
        "   letp                    lr, 2b                       \n"
        "1:                                                      \n"

        " .unreq vecAlphaCompl                                   \n"
        : [pTarget] "+l"(pTarget8),  [pAlpha] "+r" (pAlpha), [loopCnt] "+r"(blkCnt)
        :[vec256] "t"   (v256),[Colour] "r"(Colour),[vOpacity] "t"(vOpacity),
         [str4Offs] "t"(vStride4Offs)
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
         ,[opa254] "r" (254)
#endif
        :"q0", "q1", "q2", "q3", "memory");

#endif
        pwAlpha += (iAlphaStride);
        pTarget += (iTargetStride);
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_rgb565_colour_filling_mask)(uint16_t * __RESTRICT pTarget,
                                                    int16_t iTargetStride,
                                                    uint8_t * __RESTRICT pchAlpha,
                                                    int16_t iAlphaStride,
                                                    arm_2d_size_t * __RESTRICT ptCopySize,
                                                    uint16_t Colour)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    __arm_2d_color_fast_rgb_t tSrcPix;

    __arm_2d_rgb565_unpack(*(&Colour), &tSrcPix);

#ifdef USE_MVE_INTRINSICS
    RGB565_COLOUR_FILLING_MASK_MVE( RGB565_TRGT_LOAD, _,
                                    RGB565_SCAL_OPACITY_NONE, _, pchAlpha, 1, 255);
#else
    /* RGB565 pack/unpack Masks */
    /* use memory rather than vmov to optimize Helium operations interleaving */
    uint16x8_t scratch[5];

    // Unpacking Mask Red
    vst1q((uint16_t*)&scratch[0], vdupq_n_u16(0x1f));
    // Unpacking Mask Green
    vst1q((uint16_t*)&scratch[1], vdupq_n_u16(0x3f));
    // packing Mask Green
    vst1q((uint16_t*)&scratch[2], vdupq_n_u16(0xfc));
    // packing Mask Blue
    vst1q((uint16_t*)&scratch[3], vdupq_n_u16(0xf8));

    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint8_t  *pAlpha = pchAlpha;
        uint16_t       *pCurTarget = pTarget;
        register unsigned blkCnt __asm("lr");
        blkCnt = iWidth;

       __asm volatile  (
            ".p2align 2                                            \n"
            /* load scheduling */
            "   vldrh.u16               q0, [%[pTarget]]           \n"
            "   vmov.i16                q7, #0x0100                \n"
            "   vldrb.u16               q1, [%[pAlpha]], #8        \n"

            "   wlstp.16                lr, %[loopCnt], 1f         \n"
            "2:                                                    \n"

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            /* if alpha == 255, boost to 256 */
            "   vpt.i16                 eq, q1, %[alph255]         \n"
            "   vmovt.i16               q1, #256                   \n"
#endif

            // vecAlpha
            "   vsub.i16                q2, q7, q1                 \n"

            /*  RGB565 unpack          */

            /* vecAlpha * 4 for G channel upscale */
            "   vmul.i16                q2, q2, %[four]            \n"
            /* G channel extract */
            "   vshr.u16                q5, q0, #5                 \n"

            /* load Unpacking Mask for R channel */
            "   vldrh.u16               q7, [%[scratch], #(0*16)]  \n"
            "   vand                    q4, q0, q7                 \n"

            /* load Unpacking Mask for G channel */
            "   vldrh.u16               q7, [%[scratch], #(1*16)]  \n"
            "   vand                    q5, q5, q7                 \n"
            /*  scale G vector with alpha vector */
            "   vmul.u16                q5, q5, q2                 \n"

            /* B channel */
            "   vshr.u16                q6, q0, #11                \n"

            /*  blend G vector with input G color*/
            "   vmla.s16                q5, q1, %[G]               \n"

            /* vecAlpha * 8 for R & B  upscale */
            "   vshl.i16                q2, q2, #1                 \n"

            /*  scale R vector with alpha vec */
            "   vmul.u16                q4, q4, q2                 \n"

            "   vshr.u16                q5, q5, #8                 \n"

            /*  blend R vector with input R color*/
            "   vmla.s16                q4, q1, %[B]               \n"

            /* load packing Mask for G channel */
            "   vldrh.u16               q7, [%[scratch], #(2*16)]  \n"

            /*  scale B vector with alpha vector */
            "   vmul.u16                q6, q6, q2                 \n"
            "   vand                    q5, q5, q7                 \n"

            /*  blend B vector with input B color*/
            "   vmla.s16                q6, q1, %[R]               \n"

            /* load packing Mask for B channel */
            "   vldrh.u16               q7, [%[scratch], #(3*16)]  \n"

            "   vshr.u16                q6, q6, #8                 \n"

           /* RGB 565 pack */

            /* (G & 0x00fc), 8) */
            "   vmul.i16                q5, q5, %[eight]           \n"

            /* (B & 0x00f8) */
            "   vand                    q6, q6, q7                 \n"

            /* load next alpha vector */
            "   vldrb.u16               q1, [%[pAlpha]], #8        \n"
            "   vmov.i16                q7, #0x0100                \n"

            /* pack G & B */
            "   vmla.s16                q5, q6, %[twofiftysix]     \n"
            /* combined (R >> 8) >> 3 */
            "   vshr.u16                q4, q4, #11                \n"

            /* load next target */
            "   vldrh.u16               q0, [%[pTarget], #16]      \n"

            /*  pack R */
            "   vorr                    q4, q4, q5                 \n"
            "   vstrh.16                q4, [%[pTarget]], #16      \n"
            "   letp                    lr, 2b                     \n"
            "1:                                                    \n"
            :[pTarget]"+l"(pCurTarget),[pAlpha] "+l"(pAlpha),[loopCnt] "+r"(blkCnt)
            :[Colour] "r"(Colour), [eight] "r" (8), [four] "r" (4),
            [R] "r" (tSrcPix.R), [G] "r" (tSrcPix.G), [B] "r" (tSrcPix.B),
            [twofiftysix] "r" (256), [scratch] "r" (scratch)
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            ,[alph255] "r" (255)
#endif
            :"q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "memory");

        pchAlpha += (iAlphaStride);
        pTarget += (iTargetStride);
    }
#endif
}



__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_rgb565_colour_filling_mask_opacity)(uint16_t * __RESTRICT pTarget,
                                                    int16_t iTargetStride,
                                                    uint8_t * __RESTRICT pchAlpha,
                                                    int16_t iAlphaStride,
                                                    arm_2d_size_t * __RESTRICT ptCopySize,
                                                    uint16_t Colour,
                                                    uint_fast16_t hwOpacity)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    uint8x16_t      vOpacity = vdupq_n_u8(hwOpacity);

    __arm_2d_color_fast_rgb_t tSrcPix;

    __arm_2d_rgb565_unpack(*(&Colour), &tSrcPix);

#ifdef USE_MVE_INTRINSICS
    RGB565_COLOUR_FILLING_MASK_MVE( RGB565_TRGT_LOAD, _,
                                    RGB565_SCAL_OPACITY, vOpacity, pchAlpha, 1, 254);
#else
    /* RGB565 pack/unpack Masks + opacity */
    /* use memory rather than vmov to optimize Helium operations interleaving */
    uint16x8_t scratch[6];

    // Unpacking Mask Red
    vst1q((uint16_t*)&scratch[0], vdupq_n_u16(0x1f));
    // Unpacking Mask Green
    vst1q((uint16_t*)&scratch[1], vdupq_n_u16(0x3f));
    // packing Mask Green
    vst1q((uint16_t*)&scratch[2], vdupq_n_u16(0xfc));
    // packing Mask Blue
    vst1q((uint16_t*)&scratch[3], vdupq_n_u16(0xf8));
    // opacity
    vst1q((uint16_t*)&scratch[4], (uint16x8_t)vOpacity);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint8_t  *pAlpha = pchAlpha;
        uint16_t       *pCurTarget = pTarget;
        register unsigned blkCnt __asm("lr");
        blkCnt = iWidth;

       __asm volatile  (
            ".p2align 2                                            \n"
            /* load scheduling */
            "   vldrh.u16               q0, [%[pTarget]]           \n"
            "   vmov.i16                q7, #0x0100                \n"
            "   vldrb.u16               q1, [%[pAlpha]], #8        \n"
            /* opacity vector */
            "   vldrh.u16               q6, [%[scratch], #(4*16)]  \n"
            "   vmulh.u8                q1, q1, q6                 \n"

            "   wlstp.16                lr, %[loopCnt], 1f         \n"
            "2:                                                    \n"

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            /* if vOpacity == 254, boost to 256 */
            "   vpt.i16                 eq, q1, %[opa254]          \n"
            "   vmovt.i16               q1, #256                   \n"
#endif
            // vecAlpha
            "   vsub.i16                q2, q7, q1                 \n"

            /*  RGB565 unpack          */

            /* vecAlpha * 4 for G channel upscale */
            "   vmul.i16                q2, q2, %[four]            \n"
            /* G channel extract */
            "   vshr.u16                q5, q0, #5                 \n"

            /* load Unpacking Mask for R channel */
            "   vldrh.u16               q7, [%[scratch], #(0*16)]  \n"
            "   vand                    q4, q0, q7                 \n"

            /* load Unpacking Mask for G channel */
            "   vldrh.u16               q7, [%[scratch], #(1*16)]  \n"
            "   vand                    q5, q5, q7                 \n"
            /*  scale G vector with alpha vector */
            "   vmul.u16                q5, q5, q2                 \n"

            /* B channel */
            "   vshr.u16                q6, q0, #11                \n"

            /*  blend G vector with input G color*/
            "   vmla.s16                q5, q1, %[G]               \n"

            /* vecAlpha * 8 for R & B  upscale */
            "   vshl.i16                q2, q2, #1                 \n"

            /*  scale R vector with alpha vec */
            "   vmul.u16                q4, q4, q2                 \n"

            "   vshr.u16                q5, q5, #8                 \n"

            /*  blend R vector with input R color*/
            "   vmla.s16                q4, q1, %[B]               \n"

            /* load packing Mask for G channel */
            "   vldrh.u16               q7, [%[scratch], #(2*16)]  \n"

            /*  scale B vector with alpha vector */
            "   vmul.u16                q6, q6, q2                 \n"
            "   vand                    q5, q5, q7                 \n"

            /*  blend B vector with input B color*/
            "   vmla.s16                q6, q1, %[R]               \n"

            /* load packing Mask for B channel */
            "   vldrh.u16               q7, [%[scratch], #(3*16)]  \n"

            "   vshr.u16                q6, q6, #8                 \n"

           /* RGB 565 pack */

            /* (G & 0x00fc), 8) */
            "   vmul.i16                q5, q5, %[eight]           \n"

            /* (B & 0x00f8) */
            "   vand                    q6, q6, q7                 \n"

            /* load next alpha vector */
            "   vldrb.u16               q1, [%[pAlpha]], #8        \n"
            "   vmov.i16                q7, #0x0100                \n"

            /* pack G & B */
            "   vmla.s16                q5, q6, %[twofiftysix]     \n"
            /* reload opacity and scale alpha */
            "   vldrh.u16               q6, [%[scratch], #(4*16)]  \n"
            "   vmulh.u8                q1, q1, q6                 \n"

            /* combined (R >> 8) >> 3 */
            "   vshr.u16                q4, q4, #11                \n"

            /* load next target */
            "   vldrh.u16               q0, [%[pTarget], #16]      \n"

            /*  pack R */
            "   vorr                    q4, q4, q5                 \n"
            "   vstrh.16                q4, [%[pTarget]], #16      \n"
            "   letp                    lr, 2b                     \n"
            "1:                                                    \n"
            :[pTarget]"+r"(pCurTarget),[pAlpha] "+l"(pAlpha),[loopCnt] "+r"(blkCnt)
            :[Colour] "r"(Colour), [eight] "r" (8), [four] "r" (4),
            [R] "r" (tSrcPix.R), [G] "r" (tSrcPix.G), [B] "r" (tSrcPix.B),
            [twofiftysix] "r" (256), [scratch] "r" (scratch)
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
             ,[opa254] "r" (254)
#endif
            :"q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "memory");

        pchAlpha += (iAlphaStride);
        pTarget += (iTargetStride);
    }
#endif
}


__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_rgb565_colour_filling_channel_mask)(uint16_t * __RESTRICT pTarget,
                                                      int16_t iTargetStride,
                                                      uint32_t * __RESTRICT pwAlpha,
                                                      int16_t iAlphaStride,
                                                      arm_2d_size_t * __RESTRICT ptCopySize,
                                                      uint16_t Colour)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    uint16x8_t      vStride4Offs = vidupq_n_u16(0, 4);
    __arm_2d_color_fast_rgb_t tSrcPix;

    __arm_2d_rgb565_unpack(*(&Colour), &tSrcPix);

#ifdef USE_MVE_INTRINSICS
    RGB565_COLOUR_FILLING_MASK_MVE(RGB565_TRGT_LOAD_STRIDE, vStride4Offs,
                                            RGB565_SCAL_OPACITY_NONE, _, pwAlpha, 4, 1);
#else
    /* RGB565 pack/unpack Masks  */
    /* use memory rather than vmov to optimize Helium operations interleaving */
    uint16x8_t scratch[4];

    // Unpacking Mask Red
    vst1q((uint16_t*)&scratch[0], vdupq_n_u16(0x1f));
    // Unpacking Mask Green
    vst1q((uint16_t*)&scratch[1], vdupq_n_u16(0x3f));
    // packing Mask Green
    vst1q((uint16_t*)&scratch[2], vdupq_n_u16(0xfc));
    // packing Mask Blue
    vst1q((uint16_t*)&scratch[3], vdupq_n_u16(0xf8));

    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint32_t  *pAlpha = pwAlpha;
        uint16_t       *pCurTarget = pTarget;
        register unsigned blkCnt __asm("lr");
        blkCnt = iWidth;

       __asm volatile  (
            ".p2align 2                                            \n"
            /* load scheduling */
            "   vldrh.u16               q0, [%[pTarget]]           \n"

            "   vmov.i16                q7, #0x0100                \n"
            "   vldrb.u16               q1, [%[pAlpha],%[str4Offs]]\n"

            "   wlstp.16                lr, %[loopCnt], 1f         \n"
            "2:                                                    \n"
            "   add                     %[pAlpha], %[pAlpha],#(8*4)\n"
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            /* if alpha == 255, boost to 256 */
            "   vpt.i16                 eq, q1, %[alph255]         \n"
            "   vmovt.i16               q1, #256                   \n"
#endif
            // vecAlpha
            "   vsub.i16                q2, q7, q1                 \n"

            /*  RGB565 unpack          */

            /* vecAlpha * 4 for G channel upscale */
            "   vmul.i16                q2, q2, %[four]            \n"
            /* G channel extract */
            "   vshr.u16                q5, q0, #5                 \n"

            /* load Unpacking Mask for R channel */
            "   vldrh.u16               q7, [%[scratch], #(0*16)]  \n"
            "   vand                    q4, q0, q7                 \n"

            /* load Unpacking Mask for G channel */
            "   vldrh.u16               q7, [%[scratch], #(1*16)]  \n"
            "   vand                    q5, q5, q7                 \n"
            /*  scale G vector with alpha vector */
            "   vmul.u16                q5, q5, q2                 \n"

            /* B channel */
            "   vshr.u16                q6, q0, #11                \n"

            /*  blend G vector with input G color*/
            "   vmla.s16                q5, q1, %[G]               \n"

            /* vecAlpha * 8 for R & B  upscale */
            "   vshl.i16                q2, q2, #1                 \n"

            /*  scale R vector with alpha vec */
            "   vmul.u16                q4, q4, q2                 \n"

            "   vshr.u16                q5, q5, #8                 \n"

            /*  blend R vector with input R color*/
            "   vmla.s16                q4, q1, %[B]               \n"

            /* load packing Mask for G channel */
            "   vldrh.u16               q7, [%[scratch], #(2*16)]  \n"

            /*  scale B vector with alpha vector */
            "   vmul.u16                q6, q6, q2                 \n"
            "   vand                    q5, q5, q7                 \n"

            /*  blend B vector with input B color*/
            "   vmla.s16                q6, q1, %[R]               \n"

            /* load packing Mask for B channel */
            "   vldrh.u16               q7, [%[scratch], #(3*16)]  \n"

            "   vshr.u16                q6, q6, #8                 \n"

           /* RGB 565 pack */

            /* (G & 0x00fc), 8) */
            "   vmul.i16                q5, q5, %[eight]           \n"

            /* (B & 0x00f8) */
            "   vand                    q6, q6, q7                 \n"

            /* load next alpha vector */
            "   vldrb.u16               q1, [%[pAlpha],%[str4Offs]]\n"

            "   vmov.i16                q7, #0x0100                \n"

            /* pack G & B */
            "   vmla.s16                q5, q6, %[twofiftysix]     \n"
            /* combined (R >> 8) >> 3 */
            "   vshr.u16                q4, q4, #11                \n"

            /* load next target */
            "   vldrh.u16               q0, [%[pTarget], #16]      \n"

            /*  pack R */
            "   vorr                    q4, q4, q5                 \n"
            "   vstrh.16                q4, [%[pTarget]], #16      \n"
            "   letp                    lr, 2b                     \n"
            "1:                                                    \n"
            :[pTarget]"+r"(pCurTarget),[pAlpha] "+r"(pAlpha),[loopCnt] "+r"(blkCnt)
            :[Colour] "r"(Colour), [eight] "r" (8), [four] "r" (4),
            [R] "r" (tSrcPix.R), [G] "r" (tSrcPix.G), [B] "r" (tSrcPix.B),
            [twofiftysix] "r" (256), [scratch] "r" (scratch), [str4Offs] "t"(vStride4Offs)
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            ,[alph255] "r" (255)
#endif
            :"q0", "q1", "q2", "q4", "q5", "q6", "q7", "memory");

        pwAlpha += (iAlphaStride);
        pTarget += (iTargetStride);
    }
#endif
}


__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_rgb565_colour_filling_channel_mask_opacity)(uint16_t * __RESTRICT pTarget,
                                                              int16_t iTargetStride,
                                                              uint32_t * __RESTRICT pwAlpha,
                                                              int16_t iAlphaStride,
                                                              arm_2d_size_t * __RESTRICT ptCopySize,
                                                              uint16_t Colour,
                                                              uint_fast16_t hwOpacity)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    uint16x8_t      vStride4Offs = vidupq_n_u16(0, 4);
    uint8x16_t      vOpacity = vdupq_n_u8(hwOpacity);
    __arm_2d_color_fast_rgb_t tSrcPix;

    __arm_2d_rgb565_unpack(*(&Colour), &tSrcPix);

#ifdef USE_MVE_INTRINSICS

    RGB565_COLOUR_FILLING_MASK_MVE(RGB565_TRGT_LOAD_STRIDE, vStride4Offs,
                                            RGB565_SCAL_OPACITY, vOpacity, pwAlpha, 4, 2);
#else
    /* RGB565 pack/unpack Masks + opacity */
    /* use memory rather than vmov to optimize Helium operations interleaving */
    uint16x8_t scratch[5];

    // Unpacking Mask Red
    vst1q((uint16_t*)&scratch[0], vdupq_n_u16(0x1f));
    // Unpacking Mask Green
    vst1q((uint16_t*)&scratch[1], vdupq_n_u16(0x3f));
    // packing Mask Green
    vst1q((uint16_t*)&scratch[2], vdupq_n_u16(0xfc));
    // packing Mask Blue
    vst1q((uint16_t*)&scratch[3], vdupq_n_u16(0xf8));
    // opacity
    vst1q((uint16_t*)&scratch[4], (uint16x8_t)vOpacity);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint32_t  *pAlpha = pwAlpha;
        uint16_t       *pCurTarget = pTarget;
        register unsigned blkCnt __asm("lr");
        blkCnt = iWidth;

       __asm volatile  (
            ".p2align 2                                            \n"
            /* load scheduling */
            "   vldrh.u16               q0, [%[pTarget]]           \n"

            "   vmov.i16                q7, #0x0100                \n"
            "   vldrb.u16               q1, [%[pAlpha],%[str4Offs]]\n"
            /* opacity vector */
            "   vldrh.u16               q6, [%[scratch], #(4*16)]  \n"
            "   vmulh.u8                q1, q1, q6                 \n"

            "   wlstp.16                lr, %[loopCnt], 1f         \n"
            "2:                                                    \n"
            "   add                     %[pAlpha], %[pAlpha],#(8*4)\n"
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            /* if vOpacity == 254, boost to 256 */
            "   vpt.i16                 eq, q1, %[opa254]          \n"
            "   vmovt.i16               q1, #256                   \n"
#endif
            // vecAlpha
            "   vsub.i16                q2, q7, q1                 \n"

            /*  RGB565 unpack          */

            /* vecAlpha * 4 for G channel upscale */
            "   vmul.i16                q2, q2, %[four]            \n"
            /* G channel extract */
            "   vshr.u16                q5, q0, #5                 \n"

            /* load Unpacking Mask for R channel */
            "   vldrh.u16               q7, [%[scratch], #(0*16)]  \n"
            "   vand                    q4, q0, q7                 \n"

            /* load Unpacking Mask for G channel */
            "   vldrh.u16               q7, [%[scratch], #(1*16)]  \n"
            "   vand                    q5, q5, q7                 \n"
            /*  scale G vector with alpha vector */
            "   vmul.u16                q5, q5, q2                 \n"

            /* B channel */
            "   vshr.u16                q6, q0, #11                \n"

            /*  blend G vector with input G color*/
            "   vmla.s16                q5, q1, %[G]               \n"

            /* vecAlpha * 8 for R & B  upscale */
            "   vshl.i16                q2, q2, #1                 \n"

            /*  scale R vector with alpha vec */
            "   vmul.u16                q4, q4, q2                 \n"

            "   vshr.u16                q5, q5, #8                 \n"

            /*  blend R vector with input R color*/
            "   vmla.s16                q4, q1, %[B]               \n"

            /* load packing Mask for G channel */
            "   vldrh.u16               q7, [%[scratch], #(2*16)]  \n"

            /*  scale B vector with alpha vector */
            "   vmul.u16                q6, q6, q2                 \n"
            "   vand                    q5, q5, q7                 \n"

            /*  blend B vector with input B color*/
            "   vmla.s16                q6, q1, %[R]               \n"

            /* load packing Mask for B channel */
            "   vldrh.u16               q7, [%[scratch], #(3*16)]  \n"

            "   vshr.u16                q6, q6, #8                 \n"

           /* RGB 565 pack */

            /* (G & 0x00fc), 8) */
            "   vmul.i16                q5, q5, %[eight]           \n"

            /* (B & 0x00f8) */
            "   vand                    q6, q6, q7                 \n"

            /* load next alpha vector */
            "   vldrb.u16               q1, [%[pAlpha],%[str4Offs]]\n"

            "   vmov.i16                q7, #0x0100                \n"

            /* pack G & B */
            "   vmla.s16                q5, q6, %[twofiftysix]     \n"
            /* combined (R >> 8) >> 3 */
            "   vldrh.u16               q6, [%[scratch], #(4*16)]  \n"
            "   vmulh.u8                q1, q1, q6                 \n"
            "   vshr.u16                q4, q4, #11                \n"

            /* load next target */
            "   vldrh.u16               q0, [%[pTarget], #16]      \n"

            /*  pack R */
            "   vorr                    q4, q4, q5                 \n"
            "   vstrh.16                q4, [%[pTarget]], #16      \n"
            "   letp                    lr, 2b                     \n"
            "1:                                                    \n"
            :[pTarget]"+r"(pCurTarget),[pAlpha] "+r"(pAlpha),[loopCnt] "+r"(blkCnt)
            :[Colour] "r"(Colour), [eight] "r" (8), [four] "r" (4),
            [R] "r" (tSrcPix.R), [G] "r" (tSrcPix.G), [B] "r" (tSrcPix.B),
            [twofiftysix] "r" (256), [scratch] "r" (scratch), [str4Offs] "t"(vStride4Offs)
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
             ,[opa254] "r" (254)
#endif
            :"q0", "q1", "q2", "q4", "q5", "q6", "q7", "memory");

        pwAlpha += (iAlphaStride);
        pTarget += (iTargetStride);
    }
#endif
}



__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_cccn888_colour_filling_mask)(uint32_t * __RESTRICT pTarget,
                                                     int16_t iTargetStride,
                                                     uint8_t * __RESTRICT pchAlpha,
                                                     int16_t iAlphaStride,
                                                     arm_2d_size_t * __RESTRICT ptCopySize,
                                                     uint32_t Colour)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    uint16x8_t      v256 = vdupq_n_u16(256);
    uint16x8_t      vStride4Offs = vidupq_n_u16(0, 4);
    uint16_t        c0, c1, c2;

    c0 = Colour & 0xff;
    c1 = (Colour >> 8) & 0xff;
    c2 = (Colour >> 16) & 0xff;

    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint8_t * pAlpha = pchAlpha;
        uint8_t *       pTargetCh0 = (uint8_t*)pTarget;
        uint8_t *       pTargetCh1 = pTargetCh0 + 1;
        uint8_t *       pTargetCh2 = pTargetCh0 + 2;

#ifdef USE_MVE_INTRINSICS

        CCCN888_COLOUR_FILLING_MASK_INNER_MVE(CCCN888_TRGT_LOAD, _,
                                        CCCN888_SCAL_OPACITY_NONE, _, 1, 255);
#else

        register unsigned blkCnt __asm("lr");
        blkCnt = iWidth;

    __asm volatile(
            "vecAlphaCompl            .req q2                             \n"

            ".p2align 2                                                   \n"
            /* expand chan0 */
            "   vldrb.u16               q0, [%[pTargetCh0], %[str4Offs]]  \n"
            "   vldrb.u16               q1, [%[pAlpha]], #8               \n"

            "   wlstp.16                lr, %[loopCnt], 1f                \n"
            "2:                                                           \n"

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            /* if alpha == 255, boost to 256 */
            "   vpt.i16                 eq, q1, %[alph255]                \n"
            "   vmovt.i16               q1, #256                          \n"
#endif

            "   vsub.i16                vecAlphaCompl, %[vec256], q1      \n"
            /*  scale ch0 vector with alpha vector */
            "   vmul.u16                q3, q0, vecAlphaCompl             \n"

            /* expand chan1 */
            "   vldrb.u16               q0, [%[pTargetCh1], %[str4Offs]]  \n"
            /*  blend ch0 vector with input ch0 color*/
            "   vmla.s16                q3, q1, %[c0]                     \n"
            "   vshr.u16                q3, q3, #8                        \n"

            "   vstrb.u16               q3, [%[pTargetCh0], %[str4Offs]]  \n"

            /*  scale ch1 vector with alpha vector */
            "   vmul.u16                q3, q0, vecAlphaCompl             \n"

            /* expand chan2 */
            "   vldrb.u16               q0, [%[pTargetCh2], %[str4Offs]]  \n"
            /*  blend ch1 vector with input ch1 color*/
            "   vmla.s16                q3, q1, %[c1]                     \n"
            "   vshr.u16                q3, q3, #8                        \n"
            "   vstrb.u16               q3, [%[pTargetCh1], %[str4Offs]]  \n"

            "   adds                    %[pTargetCh0], #32                \n"
            "   adds                    %[pTargetCh1], #32                \n"

            /*  scale ch2 vector with alpha vector */
            "   vmul.u16                q3, q0, vecAlphaCompl             \n"
            "   vldrb.u16               q0, [%[pTargetCh0], %[str4Offs]]  \n"

            /*  blend ch2 vector with input ch2 color*/
            "   vmla.s16                q3, q1, %[c2]                     \n"
            "   vldrb.u16               q1, [%[pAlpha]], #8               \n"

            "   vshr.u16                q3, q3, #8                        \n"
            "   vstrb.u16               q3, [%[pTargetCh2], %[str4Offs]]  \n"

            "   add.w                   %[pTargetCh2], %[pTargetCh2], #32 \n"

            "   letp                    lr, 2b                            \n"
            "1:                                                           \n"

            " .unreq vecAlphaCompl                                        \n"

            :[pTargetCh0] "+r"(pTargetCh0),  [pTargetCh1] "+r"(pTargetCh1),
             [pTargetCh2] "+r"(pTargetCh2), [pAlpha] "+l" (pAlpha), [loopCnt] "+r"(blkCnt)
            :[vec256] "t" (v256),[str4Offs] "t" (vStride4Offs),
             [c0] "r"(c0), [c1] "r"(c1), [c2] "r"(c2)
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            ,[alph255] "r" (255)
#endif
            :"q0", "q1", "q2", "q3", "memory", "cc");

#endif
        pchAlpha += (iAlphaStride);
        pTarget += (iTargetStride);
    }
}



__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_cccn888_colour_filling_mask_opacity)(uint32_t * __RESTRICT pTarget,
                                                             int16_t iTargetStride,
                                                             uint8_t * __RESTRICT pchAlpha,
                                                             int16_t iAlphaStride,
                                                             arm_2d_size_t * __RESTRICT ptCopySize,
                                                             uint32_t Colour,
                                                             uint_fast16_t hwOpacity)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    uint16x8_t      v256 = vdupq_n_u16(256);
    uint16x8_t      vStride4Offs = vidupq_n_u16(0, 4);
    uint8x16_t      vOpacity = vdupq_n_u8(hwOpacity);
    uint16_t        c0, c1, c2;

    c0 = Colour & 0xff;
    c1 = (Colour >> 8) & 0xff;
    c2 = (Colour >> 16) & 0xff;

    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint8_t * pAlpha = pchAlpha;
        uint8_t *       pTargetCh0 = (uint8_t*)pTarget;
        uint8_t *       pTargetCh1 = pTargetCh0 + 1;
        uint8_t *       pTargetCh2 = pTargetCh0 + 2;

#ifdef USE_MVE_INTRINSICS

        CCCN888_COLOUR_FILLING_MASK_INNER_MVE(CCCN888_TRGT_LOAD, _,
                                        CCCN888_SCAL_OPACITY, vOpacity, 1, 254);
#else

        register unsigned blkCnt __asm("lr");
        blkCnt = iWidth;

    __asm volatile(
            "vecAlphaCompl            .req q2                             \n"

            ".p2align 2                                                   \n"
            /* expand chan0 */
            "   vldrb.u16               q0, [%[pTargetCh0], %[str4Offs]]  \n"
            "   vldrb.u16               q1, [%[pAlpha]], #8               \n"
            "   vmulh.u8                q1, q1, %[vOpacity]               \n"

            "   wlstp.16                lr, %[loopCnt], 1f                \n"
            "2:                                                           \n"

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            /* if vOpacity == 254, boost to 256 */
            "   vpt.i16                 eq, q1, %[opa254]                 \n"
            "   vmovt.i16               q1, #256                          \n"
#endif

            "   vsub.i16                vecAlphaCompl, %[vec256], q1      \n"
            /*  scale ch0 vector with alpha vector */
            "   vmul.u16                q3, q0, vecAlphaCompl             \n"

            /* expand chan1 */
            "   vldrb.u16               q0, [%[pTargetCh1], %[str4Offs]]  \n"
            /*  blend ch0 vector with input ch0 color*/
            "   vmla.s16                q3, q1, %[c0]                     \n"
            "   vshr.u16                q3, q3, #8                        \n"

            "   vstrb.u16               q3, [%[pTargetCh0], %[str4Offs]]  \n"

            /*  scale ch1 vector with alpha vector */
            "   vmul.u16                q3, q0, vecAlphaCompl             \n"

            /* expand chan2 */
            "   vldrb.u16               q0, [%[pTargetCh2], %[str4Offs]]  \n"
            /*  blend ch1 vector with input ch1 color*/
            "   vmla.s16                q3, q1, %[c1]                     \n"
            "   vshr.u16                q3, q3, #8                        \n"
            "   vstrb.u16               q3, [%[pTargetCh1], %[str4Offs]]  \n"

            "   adds                    %[pTargetCh0], #32                \n"
            "   adds                    %[pTargetCh1], #32                \n"

            /*  scale ch2 vector with alpha vector */
            "   vmul.u16                q3, q0, vecAlphaCompl             \n"
            "   vldrb.u16               q0, [%[pTargetCh0], %[str4Offs]]  \n"

            /*  blend ch2 vector with input ch2 color*/
            "   vmla.s16                q3, q1, %[c2]                     \n"
            "   vldrb.u16               q1, [%[pAlpha]], #8               \n"
            "   vmulh.u8                q1, q1, %[vOpacity]               \n"

            "   vshr.u16                q3, q3, #8                        \n"
            "   vstrb.u16               q3, [%[pTargetCh2], %[str4Offs]]  \n"

            "   add.w                   %[pTargetCh2], %[pTargetCh2], #32 \n"

            "   letp                    lr, 2b                            \n"
            "1:                                                           \n"

            :[pTargetCh0] "+r"(pTargetCh0),  [pTargetCh1] "+r"(pTargetCh1),
             [pTargetCh2] "+r"(pTargetCh2), [pAlpha] "+l" (pAlpha), [loopCnt] "+r"(blkCnt)
            :[vec256] "t" (v256),[str4Offs] "t" (vStride4Offs),
             [vOpacity] "t"(vOpacity),
             [c0] "r"(c0), [c1] "r"(c1), [c2] "r"(c2)
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
             ,[opa254] "r" (254)
#endif
            :"q0", "q1", "q2", "q3", "memory", "cc");

#endif
        pchAlpha += (iAlphaStride);
        pTarget += (iTargetStride);
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_cccn888_colour_filling_channel_mask)(uint32_t * __RESTRICT pTarget,
                                                       int16_t iTargetStride,
                                                       uint32_t * __RESTRICT pwAlpha,
                                                       int16_t iAlphaStride,
                                                       arm_2d_size_t * __RESTRICT ptCopySize,
                                                       uint32_t Colour)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    uint16x8_t      v256 = vdupq_n_u16(256);
    uint16x8_t      vStride4Offs = vidupq_n_u16(0, 4);
    uint16_t        c0, c1, c2;

    c0 = Colour & 0xff;
    c1 = (Colour >> 8) & 0xff;
    c2 = (Colour >> 16) & 0xff;

    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint8_t *pAlpha = (const uint8_t *)pwAlpha;
        uint8_t *       pTargetCh0 = (uint8_t*)pTarget;
        uint8_t *       pTargetCh1 = pTargetCh0 + 1;
        uint8_t *       pTargetCh2 = pTargetCh0 + 2;

#ifdef USE_MVE_INTRINSICS

        CCCN888_COLOUR_FILLING_MASK_INNER_MVE(CCCN888_TRGT_LOAD_STRIDE, vStride4Offs,
                                        CCCN888_SCAL_OPACITY_NONE, _, 4, 255);
#else

        register unsigned blkCnt __asm("lr");
        blkCnt = iWidth;

    __asm volatile(
            "vecAlphaCompl            .req q2                             \n"

            ".p2align 2                                                   \n"
            /* expand chan0 */
            "   vldrb.u16               q0, [%[pTargetCh0], %[str4Offs]]  \n"
            "   vldrb.u16               q1, [%[pAlpha], %[str4Offs]]      \n"

            "   wlstp.16                lr, %[loopCnt], 1f                \n"
            "2:                                                           \n"

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            /* if alpha == 255, boost to 256 */
            "   vpt.i16                 eq, q1, %[alph255]                \n"
            "   vmovt.i16               q1, #256                          \n"
#endif
            "   vsub.i16                vecAlphaCompl, %[vec256], q1      \n"

            /*  scale ch0 vector with alpha vector */
            "   vmul.u16                q3, q0, vecAlphaCompl             \n"

            /* expand chan1 */
            "   vldrb.u16               q0, [%[pTargetCh1], %[str4Offs]]  \n"
            /*  blend ch0 vector with input ch0 color*/
            "   vmla.s16                q3, q1, %[c0]                     \n"
            "   vshr.u16                q3, q3, #8                        \n"

            "   vstrb.u16               q3, [%[pTargetCh0], %[str4Offs]]  \n"

            /*  scale ch1 vector with alpha vector */
            "   vmul.u16                q3, q0, vecAlphaCompl             \n"

            /* expand chan2 */
            "   vldrb.u16               q0, [%[pTargetCh2], %[str4Offs]]  \n"
            /*  blend ch1 vector with input ch1 color*/
            "   vmla.s16                q3, q1, %[c1]                     \n"
            "   vshr.u16                q3, q3, #8                        \n"
            "   vstrb.u16               q3, [%[pTargetCh1], %[str4Offs]]  \n"

            "   adds                    %[pAlpha], #32                    \n"
            "   adds                    %[pTargetCh0], #32                \n"


            /*  scale ch2 vector with alpha vector */
            "   vmul.u16                q3, q0, vecAlphaCompl             \n"
            "   vldrb.u16               q0, [%[pTargetCh0], %[str4Offs]]  \n"

            /*  blend ch2 vector with input ch2 color*/
            "   vmla.s16                q3, q1, %[c2]                     \n"
            "   vldrb.u16               q1, [%[pAlpha], %[str4Offs]]      \n"

            "   vshr.u16                q3, q3, #8                        \n"
            "   vstrb.u16               q3, [%[pTargetCh2], %[str4Offs]]  \n"

            "   adds                    %[pTargetCh1], #32                \n"
            "   adds                    %[pTargetCh2], #32                \n"

            "   letp                    lr, 2b                            \n"
            "1:                                                           \n"

            :[pTargetCh0] "+r"(pTargetCh0),  [pTargetCh1] "+r"(pTargetCh1),
             [pTargetCh2] "+r"(pTargetCh2), [pAlpha] "+r" (pAlpha), [loopCnt] "+r"(blkCnt)
            :[vec256] "t" (v256),[str4Offs] "t" (vStride4Offs),
             [c0] "r"(c0), [c1] "r"(c1), [c2] "r"(c2)
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            ,[alph255] "r" (255)
#endif
            :"q0", "q1", "q2", "q3", "memory", "cc");

#endif
        pwAlpha += (iAlphaStride);
        pTarget += (iTargetStride);
    }
}



__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_cccn888_colour_filling_channel_mask_opacity)(uint32_t * __RESTRICT pTarget,
                                                               int16_t iTargetStride,
                                                               uint32_t * __RESTRICT pwAlpha,
                                                               int16_t iAlphaStride,
                                                               arm_2d_size_t *
                                                               __RESTRICT ptCopySize,
                                                               uint32_t Colour,
                                                               uint_fast16_t hwOpacity)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    uint16x8_t      v256 = vdupq_n_u16(256);
    uint16x8_t      vStride4Offs = vidupq_n_u16(0, 4);
    uint8x16_t      vOpacity = vdupq_n_u8(hwOpacity);
    uint16_t        c0, c1, c2;

    c0 = Colour & 0xff;
    c1 = (Colour >> 8) & 0xff;
    c2 = (Colour >> 16) & 0xff;

    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint8_t  *pAlpha = (const uint8_t *)pwAlpha;
        uint8_t *       pTargetCh0 = (uint8_t*)pTarget;
        uint8_t *       pTargetCh1 = pTargetCh0 + 1;
        uint8_t *       pTargetCh2 = pTargetCh0 + 2;

#ifdef USE_MVE_INTRINSICS

        CCCN888_COLOUR_FILLING_MASK_INNER_MVE(CCCN888_TRGT_LOAD_STRIDE, vStride4Offs,
                                        CCCN888_SCAL_OPACITY, vOpacity, 4, 254);
#else

        register unsigned blkCnt __asm("lr");
        blkCnt = iWidth;

    __asm volatile(
            "vecAlphaCompl            .req q2                             \n"

            ".p2align 2                                                   \n"
            /* expand chan0 */
            "   vldrb.u16               q0, [%[pTargetCh0], %[str4Offs]]  \n"
            "   vldrb.u16               q1, [%[pAlpha], %[str4Offs]]      \n"
            "   vmulh.u8                q1, q1, %[vOpacity]               \n"

            "   wlstp.16                lr, %[loopCnt], 1f                \n"
            "2:                                                           \n"

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            /* if vOpacity == 254, boost to 256 */
            "   vpt.i16                 eq, q1, %[opa254]                 \n"
            "   vmovt.i16               q1, #256                          \n"
#endif

            "   vsub.i16                vecAlphaCompl, %[vec256], q1      \n"
            /*  scale ch0 vector with alpha vector */
            "   vmul.u16                q3, q0, vecAlphaCompl             \n"

            /* expand chan1 */
            "   vldrb.u16               q0, [%[pTargetCh1], %[str4Offs]]  \n"
            /*  blend ch0 vector with input ch0 color*/
            "   vmla.s16                q3, q1, %[c0]                     \n"
            "   vshr.u16                q3, q3, #8                        \n"

            "   vstrb.u16               q3, [%[pTargetCh0], %[str4Offs]]  \n"

            /*  scale ch1 vector with alpha vector */
            "   vmul.u16                q3, q0, vecAlphaCompl             \n"

            /* expand chan2 */
            "   vldrb.u16               q0, [%[pTargetCh2], %[str4Offs]]  \n"
            /*  blend ch1 vector with input ch1 color*/
            "   vmla.s16                q3, q1, %[c1]                     \n"
            "   vshr.u16                q3, q3, #8                        \n"
            "   vstrb.u16               q3, [%[pTargetCh1], %[str4Offs]]  \n"

            "   adds                    %[pAlpha], #32                    \n"
            "   adds                    %[pTargetCh0], #32                \n"


            /*  scale ch2 vector with alpha vector */
            "   vmul.u16                q3, q0, vecAlphaCompl              \n"
            "   vldrb.u16               q0, [%[pTargetCh0], %[str4Offs]]  \n"

            /*  blend ch2 vector with input ch2 color*/
            "   vmla.s16                q3, q1, %[c2]                     \n"
            "   vldrb.u16               q1, [%[pAlpha], %[str4Offs]]      \n"
            "   vmulh.u8                q1, q1, %[vOpacity]               \n"

            "   vshr.u16                q3, q3, #8                        \n"
            "   vstrb.u16               q3, [%[pTargetCh2], %[str4Offs]]  \n"

            "   adds                    %[pTargetCh1], #32                \n"
            "   adds                    %[pTargetCh2], #32                \n"

            "   letp                    lr, 2b                            \n"
            "1:                                                           \n"

            :[pTargetCh0] "+r"(pTargetCh0),  [pTargetCh1] "+r"(pTargetCh1),
             [pTargetCh2] "+r"(pTargetCh2), [pAlpha] "+r" (pAlpha), [loopCnt] "+r"(blkCnt)
            :[vec256] "t" (v256),[str4Offs] "t" (vStride4Offs), [vOpacity] "t"(vOpacity),
             [c0] "r"(c0), [c1] "r"(c1), [c2] "r"(c2)
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
             ,[opa254] "r" (254)
#endif
            :"q0", "q1", "q2", "q3", "memory", "cc");

#endif
        pwAlpha += (iAlphaStride);
        pTarget += (iTargetStride);
    }
}


/* use macro expansion of fill/copy with masking */

#define __API_CAFWM_COLOUR                      gray8

#include "__arm_2d_alpha_mask_helium.inc"

#define __API_CAFWM_CFG_1_HORIZONTAL_LINE       1
#define __API_CAFWM_COLOUR                      gray8

#include "__arm_2d_alpha_mask_helium.inc"

#define __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT   1
#define __API_CAFWM_COLOUR                      gray8

#include "__arm_2d_alpha_mask_helium.inc"



#define __API_CAFWM_COLOUR                      rgb565

#include "__arm_2d_alpha_mask_helium.inc"

#define __API_CAFWM_CFG_1_HORIZONTAL_LINE       1
#define __API_CAFWM_COLOUR                      rgb565

#include "__arm_2d_alpha_mask_helium.inc"

#define __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT   1
#define __API_CAFWM_COLOUR                      rgb565

#include "__arm_2d_alpha_mask_helium.inc"




#define __API_CAFWM_COLOUR                      cccn888

#include "__arm_2d_alpha_mask_helium.inc"

#define __API_CAFWM_CFG_1_HORIZONTAL_LINE       1
#define __API_CAFWM_COLOUR                      cccn888

#include "__arm_2d_alpha_mask_helium.inc"

#define __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT   1
#define __API_CAFWM_COLOUR                      cccn888

#include "__arm_2d_alpha_mask_helium.inc"



/*----------------------------------------------------------------------------*
 * Assembly Patches                                                           *
 *----------------------------------------------------------------------------*/

#if defined(__IS_COMPILER_GCC__) && __IS_COMPILER_GCC__

__OVERRIDE_WEAK
void ARM_2D_WRAP_FUNC(  __MVE_WRAPPER( __arm_2d_impl_rgb565_masks_fill))(
                         uint16_t * __RESTRICT ptSourceBase,
                         int16_t iSourceStride,
                         arm_2d_size_t * __RESTRICT ptSourceSize,
                         uint8_t * __RESTRICT pchSourceMaskBase,
                         int16_t iSourceMaskStride,
                         arm_2d_size_t * __RESTRICT ptSourceMaskSize,
                         uint16_t * __RESTRICT ptTargetBase,
                         int16_t iTargetStride,
                         arm_2d_size_t * __RESTRICT ptTargetSize,
                         uint8_t * __RESTRICT pchTargetMaskBase,
                         int16_t iTargetMaskStride,
                         arm_2d_size_t * __RESTRICT ptTargetMaskSize)
{
    uint8_t        *__RESTRICT pchTargetMaskLineBase = pchTargetMaskBase;
    uint16x8_t      v256 = vdupq_n_u16(256);

#ifndef USE_MVE_INTRINSICS
    uint16x8_t      scratch[5];

    /* vector of 256 avoiding use of vdup to increase overlap efficiency*/
    vst1q((uint16_t *) & scratch[0], v256);
    /* scratch[1] is temporary for blended Red chan. vector */

    /* Unpacking Mask Red */
    vst1q((uint16_t *) & scratch[2], vdupq_n_u16(0x00fc));
    /* B channel packing mask */
    vst1q((uint16_t *) & scratch[3], vdupq_n_u16(0xf800));
    /* G channel packing Mask */
    vst1q((uint16_t *) & scratch[4], vdupq_n_u16(0x07e0));

    /* use of fixed point mult instead of vshr to increase overlap efficiency */
    const int16_t   inv_2pow3 = 1 << (15 - 3);  /* 1/(2^3) in Q.15 */
#endif

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {
        uint16_t       *__RESTRICT ptSource = ptSourceBase;
        uint8_t        *pchSourceMask = pchSourceMaskBase;
    #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
        int_fast16_t    iSourceMaskY = 0;
    #endif

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            uint16_t       *__RESTRICT ptTarget = ptTargetBase;
            uint8_t        *__RESTRICT pchTargetMask = pchTargetMaskLineBase;
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                uint16_t       *__RESTRICT ptSrc = ptSource;
                uint8_t        *__RESTRICT pchSrcMsk = pchSourceMask;
                uint16_t       *__RESTRICT ptTargetCur = ptTarget;
                uint8_t        *__RESTRICT pchTargetMaskCur = pchTargetMask;

#ifdef USE_MVE_INTRINSICS
                int32_t         blkCnt = wLength;

                do {
                    uint16x8_t      vecTarget = vld1q(ptTargetCur);
                    uint16x8_t      vecSource = vld1q(ptSrc);
                    uint16x8_t      vecSrcMsk = vldrbq_u16(pchSrcMsk);
                    uint16x8_t      vecTargetMask = vldrbq_u16(pchTargetMaskCur);
                    uint16x8_t      vecHwOpacity =
                        vsubq_u16(v256, (vecSrcMsk * vecTargetMask) >> 8);

                    vecTarget = __arm_2d_rgb565_blending_opacity_single_vec(
                                                    vecTarget, vecSource, vecHwOpacity);
                    /* tail predication */
                    vst1q_p_u16(ptTargetCur, vecTarget, vctp16q(blkCnt));

                    pchSrcMsk += 8;
                    pchTargetMaskCur += 8;
                    ptTargetCur += 8;
                    ptSrc += 8;

                    blkCnt -= 8;
                }
                while (blkCnt > 0);

#else
                register unsigned blkCnt __asm("lr");
                blkCnt = wLength;

                __asm volatile  (
                    /* R & B mask */
                    "vecRBUnpackMask    .req q7                               \n"
                    "vecAlpha           .req q5                               \n"
                    "vecHwOpacity       .req q3                               \n"

                    /* preload */
                    "   vldrb.u16           q0, [%[pchSrcMsk]], #8            \n"
                    "   vmov.i16            vecRBUnpackMask, #0x00f8          \n"
                    "   vldrb.u16           q5, [%[pchTargetMask]], #8        \n"

                    ".p2align 2                                               \n"
                    "   wlstp.16            lr, %[loopCnt], 1f                \n"
                    "2:                                                       \n"
                    /* vecSrcMsk * vecTargetMask */
                    "   vmul.i16            q0, q5, q0                        \n"
                    "   vldrh.u16           q6, [%[ptTarget]]                 \n"
                    "   vshr.u16            vecAlpha, q0, #8                  \n"
                    /* 256-dup vector */
                    "   vldrh.u16           q1, [%[scratch], #(16*0)]         \n"
                    /* vecHwOpacity =
                          vsubq_u16(v256, (vecSrcMsk * vecTargetMask) >> 8) */
                    "   vsub.i16            vecHwOpacity, q1, vecAlpha        \n"
                    "   vldrh.u16           q1, [%[ptSrc]], #16               \n"
                    /* mimic vshl #3 */
                    "   vshl.u16            q0, q6, #3                        \n"
                    "   vmul.i16            q4, q1, %[eight]                  \n"
                    /* vecR extract and scale */
                    "   vand                q0, q0, vecRBUnpackMask           \n"
                    "   vmul.i16            q0, vecHwOpacity, q0              \n"
                    /* vecSrcR extract and scale */
                    "   vand                q4, q4, vecRBUnpackMask           \n"
                    "   vmul.i16            q4, vecAlpha, q4                  \n"
                    /* 0xfc G-mask */
                    "   vldrw.u32           q2, [%[scratch], #(16*2)]         \n"
                    "   vadd.i16            q4, q0, q4                        \n"
                    /* push blended R  */
                    "   vstrw.32            q4, [%[scratch], #(16*1)]         \n"
                    /*  mimic vshr.u16            q4, q6, #3 */
                    "   vqdmulh.s16         q4, q6, %[inv_2pow3]              \n"
                    "   vshr.u16            q0, q1, #3                        \n"
                    /* vecG extract and scale */
                    "   vand                q4, q4, q2                        \n"
                    "   vmul.i16            q4, vecHwOpacity, q4              \n"
                    /* vecSrcG extract and scale */
                    "   vand                q0, q0, q2                        \n"
                    "   vmul.i16            q2, vecAlpha, q0                  \n"
                    "   vshr.u16            q0, q1, #8                        \n"
                    /* blended G */
                    /* vadd.i16            q2, q4, q2
                       addition using vmla for more efficient overlap */
                    "   vmla.s16            q2, q4, %[one]                    \n"
                    /* vecB extract and scale */
                    "   vshr.u16            q4, q6, #8                        \n"
                    "   vand                q4, q4, vecRBUnpackMask           \n"
                    "   vmul.i16            q4, vecHwOpacity, q4              \n"
                    /* vecSrcB extract and scale */
                    "   vand                q0, q0, vecRBUnpackMask           \n"
                    "   vmul.i16            q0, vecAlpha, q0                  \n"

                    ".unreq vecAlpha                                          \n"
                    ".unreq vecHwOpacity                                      \n"
                    ".unreq vecRBUnpackMask                                   \n"

                    /* reload blended R */
                    "   vldrw.u32           q1, [%[scratch], #(16*1)]         \n"
                    /* blended B
                       vadd.i16            q0, q4, q0
                       addition using vmla for more efficient overlap */
                    "   vmla.s16            q0, q4, %[one]                    \n"
                    /* pack R */
                    "   vshr.u16            q3, q1, #11                       \n"
                    /* B channel packing mask 0xf800 */
                    "   vldrw.u32           q4, [%[scratch], #(16*3)]         \n"
                    "   vand                q0, q0, q4                        \n"
                    /* accumulate R & B */
                    "   vorr                q4, q0, q3                        \n"
                    /* G channel packing mask 0x07e0 */
                    "   vldrw.u32           q3, [%[scratch], #(16*4)]         \n"
                    "   vshr.u16            q2, q2, #5                        \n"
                    /* load next source mask */
                    "   vldrb.u16           q0, [%[pchSrcMsk]], #8            \n"
                    /* G channel masking */
                    "   vand                q2, q2, q3                        \n"
                    /* load next target mask */
                    "   vldrb.u16           q5, [%[pchTargetMask]], #8        \n"
                    /* pack G with R.B */
                    "   vorr                q4, q4, q2                        \n"
                    "   vstrh.16            q4, [%[ptTarget]], #16            \n"
                    "   letp                lr, 2b                            \n"
                    "1:                                                       \n"
                    :[ptTarget] "+r"(ptTargetCur),[ptSrc] "+r"(ptSrc),
                     [pchTargetMask] "+l"(pchTargetMaskCur),[pchSrcMsk] "+l"(pchSrcMsk),
                     [loopCnt] "+r"(blkCnt)
                    :[scratch] "r"  (scratch),[eight] "r"(8),[inv_2pow3] "r"(inv_2pow3),
                     [one] "r" (1)
                    :"q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "memory");

#endif
                ptTarget += wLength;
                pchTargetMask += wLength;

                wLengthLeft -= wLength;
            } while (wLengthLeft);

            ptSource += iSourceStride;
            ptTargetBase += iTargetStride;
        #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
            iSourceMaskY++;
            //! handle source mask
            if (    (iSourceMaskY >= ptSourceMaskSize->iHeight)
               ||   (iSourceMaskY >= ptSourceSize->iHeight)) {
                pchSourceMask = pchSourceMaskBase;
                iSourceMaskY = 0;
            } else {
                pchSourceMask += iSourceMaskStride;
            }
        #else
            pchSourceMask += iSourceMaskStride;
        #endif

            pchTargetMaskLineBase += iTargetMaskStride;

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}



__OVERRIDE_WEAK
void ARM_2D_WRAP_FUNC(  __MVE_WRAPPER( __arm_2d_impl_rgb565_src_msk_1h_des_msk_fill))(
                        uint16_t * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t * __RESTRICT ptSourceSize,
                        uint8_t * __RESTRICT pchSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t * __RESTRICT ptSourceMaskSize,
                        uint16_t * __RESTRICT ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t * __RESTRICT ptTargetSize,
                        uint8_t * __RESTRICT pchTargetMaskBase,
                        int16_t iTargetMaskStride,
                        arm_2d_size_t * __RESTRICT ptTargetMaskSize)
{
    uint8_t        *__RESTRICT pchTargetMaskLineBase = pchTargetMaskBase;
    uint16x8_t      v256 = vdupq_n_u16(256);

#ifndef USE_MVE_INTRINSICS
    uint16x8_t      scratch[5];

    /* vector of 256 avoiding use of vdup to increase overlap efficiency*/
    vst1q((uint16_t *) & scratch[0], v256);
    /* scratch[1] is temporary for blended Red chan. vector */

    /* Unpacking Mask Red */
    vst1q((uint16_t *) & scratch[2], vdupq_n_u16(0x00fc));
    /* B channel packing mask */
    vst1q((uint16_t *) & scratch[3], vdupq_n_u16(0xf800));
    /* G channel packing Mask */
    vst1q((uint16_t *) & scratch[4], vdupq_n_u16(0x07e0));

    /* use of fixed point mult instead of vshr to increase overlap efficiency */
    const int16_t   inv_2pow3 = 1 << (15 - 3);  /* 1/(2^3) in Q.15 */
#endif

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {
        uint16_t       *__RESTRICT ptSource = ptSourceBase;
        uint8_t        *pchSourceMask = pchSourceMaskBase;
    #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
        int_fast16_t    iSourceMaskY = 0;
    #endif

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            uint16_t       *__RESTRICT ptTarget = ptTargetBase;
            uint8_t        *__RESTRICT pchTargetMask = pchTargetMaskLineBase;
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                uint16_t       *__RESTRICT ptSrc = ptSource;
                uint8_t        *__RESTRICT pchSrcMsk = pchSourceMask;
                uint16_t       *__RESTRICT ptTargetCur = ptTarget;
                uint8_t        *__RESTRICT pchTargetMaskCur = pchTargetMask;

#ifdef USE_MVE_INTRINSICS
                int32_t         blkCnt = wLength;

                do {
                    uint16x8_t      vecTarget = vld1q(ptTargetCur);
                    uint16x8_t      vecSource = vld1q(ptSrc);
                    uint16x8_t      vecSrcMsk = vldrbq_u16(pchSrcMsk);
                    uint16x8_t      vecTargetMask = vldrbq_u16(pchTargetMaskCur);
                    uint16x8_t      vecHwOpacity =
                        vsubq_u16(v256, (vecSrcMsk * vecTargetMask) >> 8);

                    vecTarget = __arm_2d_rgb565_blending_opacity_single_vec(
                                                    vecTarget, vecSource, vecHwOpacity);
                    /* tail predication */
                    vst1q_p_u16(ptTargetCur, vecTarget, vctp16q(blkCnt));

                    pchSrcMsk += 8;
                    pchTargetMaskCur += 8;
                    ptTargetCur += 8;
                    ptSrc += 8;

                    blkCnt -= 8;
                }
                while (blkCnt > 0);

#else
                register unsigned blkCnt __asm("lr");
                blkCnt = wLength;

                __asm volatile  (
                    /* R & B mask */
                    "vecRBUnpackMask    .req q7                               \n"
                    "vecAlpha           .req q5                               \n"
                    "vecHwOpacity       .req q3                               \n"

                    /* preload */
                    "   vldrb.u16           q0, [%[pchSrcMsk]], #8            \n"
                    "   vmov.i16            vecRBUnpackMask, #0x00f8          \n"
                    "   vldrb.u16           q5, [%[pchTargetMask]], #8        \n"

                    ".p2align 2                                               \n"
                    "   wlstp.16            lr, %[loopCnt], 1f                \n"
                    "2:                                                       \n"
                    /* vecSrcMsk * vecTargetMask */
                    "   vmul.i16            q0, q5, q0                        \n"
                    "   vldrh.u16           q6, [%[ptTarget]]                 \n"
                    "   vshr.u16            vecAlpha, q0, #8                  \n"
                    /* 256-dup vector */
                    "   vldrh.u16           q1, [%[scratch], #(16*0)]         \n"
                    /* vecHwOpacity =
                          vsubq_u16(v256, (vecSrcMsk * vecTargetMask) >> 8) */
                    "   vsub.i16            vecHwOpacity, q1, vecAlpha        \n"
                    "   vldrh.u16           q1, [%[ptSrc]], #16               \n"
                    /* mimic vshl #3 */
                    "   vshl.u16            q0, q6, #3                        \n"
                    "   vmul.i16            q4, q1, %[eight]                  \n"
                    /* vecR extract and scale */
                    "   vand                q0, q0, vecRBUnpackMask           \n"
                    "   vmul.i16            q0, vecHwOpacity, q0              \n"
                    /* vecSrcR extract and scale */
                    "   vand                q4, q4, vecRBUnpackMask           \n"
                    "   vmul.i16            q4, vecAlpha, q4                  \n"
                    /* 0xfc G-mask */
                    "   vldrw.u32           q2, [%[scratch], #(16*2)]         \n"
                    "   vadd.i16            q4, q0, q4                        \n"
                    /* push blended R  */
                    "   vstrw.32            q4, [%[scratch], #(16*1)]         \n"
                    /*  mimic vshr.u16            q4, q6, #3 */
                    "   vqdmulh.s16         q4, q6, %[inv_2pow3]              \n"
                    "   vshr.u16            q0, q1, #3                        \n"
                    /* vecG extract and scale */
                    "   vand                q4, q4, q2                        \n"
                    "   vmul.i16            q4, vecHwOpacity, q4              \n"
                    /* vecSrcG extract and scale */
                    "   vand                q0, q0, q2                        \n"
                    "   vmul.i16            q2, vecAlpha, q0                  \n"
                    "   vshr.u16            q0, q1, #8                        \n"
                    /* blended G */
                    /* vadd.i16            q2, q4, q2
                       addition using vmla for more efficient overlap */
                    "   vmla.s16            q2, q4, %[one]                    \n"
                    /* vecB extract and scale */
                    "   vshr.u16            q4, q6, #8                        \n"
                    "   vand                q4, q4, vecRBUnpackMask           \n"
                    "   vmul.i16            q4, vecHwOpacity, q4              \n"
                    /* vecSrcB extract and scale */
                    "   vand                q0, q0, vecRBUnpackMask           \n"
                    "   vmul.i16            q0, vecAlpha, q0                  \n"

                    ".unreq vecAlpha                                          \n"
                    ".unreq vecHwOpacity                                      \n"
                    ".unreq vecRBUnpackMask                                   \n"

                    /* reload blended R */
                    "   vldrw.u32           q1, [%[scratch], #(16*1)]         \n"
                    /* blended B
                       vadd.i16            q0, q4, q0
                       addition using vmla for more efficient overlap */
                    "   vmla.s16            q0, q4, %[one]                    \n"
                    /* pack R */
                    "   vshr.u16            q3, q1, #11                       \n"
                    /* B channel packing mask 0xf800 */
                    "   vldrw.u32           q4, [%[scratch], #(16*3)]         \n"
                    "   vand                q0, q0, q4                        \n"
                    /* accumulate R & B */
                    "   vorr                q4, q0, q3                        \n"
                    /* G channel packing mask 0x07e0 */
                    "   vldrw.u32           q3, [%[scratch], #(16*4)]         \n"
                    "   vshr.u16            q2, q2, #5                        \n"
                    /* load next source mask */
                    "   vldrb.u16           q0, [%[pchSrcMsk]], #8            \n"
                    /* G channel masking */
                    "   vand                q2, q2, q3                        \n"
                    /* load next target mask */
                    "   vldrb.u16           q5, [%[pchTargetMask]], #8        \n"
                    /* pack G with R.B */
                    "   vorr                q4, q4, q2                        \n"
                    "   vstrh.16            q4, [%[ptTarget]], #16            \n"
                    "   letp                lr, 2b                            \n"
                    "1:                                                       \n"
                    :[ptTarget] "+r"(ptTargetCur),[ptSrc] "+r"(ptSrc),
                     [pchTargetMask] "+l"(pchTargetMaskCur),[pchSrcMsk] "+l"(pchSrcMsk),
                     [loopCnt] "+r"(blkCnt)
                    :[scratch] "r"  (scratch),[eight] "r"(8),[inv_2pow3] "r"(inv_2pow3),
                     [one] "r" (1)
                    :"q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "memory");

#endif
                ptTarget += wLength;
                pchTargetMask += wLength;

                wLengthLeft -= wLength;
            } while (wLengthLeft);

            ptSource += iSourceStride;
            ptTargetBase += iTargetStride;
        #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
            iSourceMaskY++;
            //! handle source mask
            if (    (iSourceMaskY >= ptSourceMaskSize->iHeight)
               ||   (iSourceMaskY >= ptSourceSize->iHeight)) {
                pchSourceMask = pchSourceMaskBase;
                iSourceMaskY = 0;
            } else {
                pchSourceMask += iSourceMaskStride;
            }
        #else
            pchSourceMask += iSourceMaskStride;
        #endif

            pchTargetMaskLineBase = pchTargetMaskBase;

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

#endif

#ifdef   __cplusplus
}
#endif

#endif // __ARM_2D_HAS_HELIUM__
