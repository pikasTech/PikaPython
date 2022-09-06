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
 * Title:        __arm_2d_meta_copy_with_masks_helium.inc
 * Description:  c code template for copy and fill like operations
 *
 * $Date:        12. July 2022
 * $Revision:    v1.0.1
 *
 * -------------------------------------------------------------------- */

#ifndef __API_MCWM_COLOUR
#   error You have to define __API_MCWM_COLOUR before using this c template
#endif

#include "__arm_2d_math_helium.h"


#if  CHECKPIXTYP(__API_MCWM_COLOUR, rgb565)
#define __API_MCWM_PIXEL_BIT_NUM        16
#define __API_MCWM_PIXEL_VECLOAD        vld1q
#define __API_MCWM_PIXEL_GATHVECLOAD    vldrhq_gather_shifted_offset
#define __API_MCWM_PIXEL_PVECSTORE      vst1q_p
#define __API_MCWM_PIXEL_BLENDING       __arm_2d_rgb565_blending_opacity_single_vec

#elif CHECKPIXTYP(__API_MCWM_COLOUR, gray8)
#define __API_MCWM_PIXEL_BIT_NUM        16 /* widening involved */
#define __API_MCWM_PIXEL_VECLOAD        vldrbq_u16
#define __API_MCWM_PIXEL_GATHVECLOAD    vldrbq_gather_offset_u16
#define __API_MCWM_PIXEL_PVECSTORE      vstrbq_p_u16
#define __API_MCWM_PIXEL_BLENDING       __arm_2d_gray8_blending_opacity_single_vec

#elif CHECKPIXTYP(__API_MCWM_COLOUR, cccn888)
#if !ENABLE_ALT_MASK_FILL
#define __API_MCWM_PIXEL_BIT_NUM        32
#define __API_MCWM_PIXEL_VECLOAD        vld1q
#define __API_MCWM_PIXEL_GATHVECLOAD    vldrwq_gather_shifted_offset
#define __API_MCWM_PIXEL_PVECSTORE      vst1q_p
#define __API_MCWM_PIXEL_BLENDING       __arm_2d_cccn888_blending_opacity_single_vec
#else
#define __API_MCWM_PIXEL_BIT_NUM        16 /* widening involved */
#define __API_MCWM_PIXEL_VECLOAD        vldrbq_u16
#define __API_MCWM_PIXEL_GATHVECLOAD    vldrbq_gather_offset_u16
#define __API_MCWM_PIXEL_PVECSTORE      vstrbq_p_u16
#define __API_MCWM_PIXEL_BLENDING       __arm_2d_gray8_blending_opacity_single_vec
#endif

#else
#error Unknown colour
#endif

#define __API_MCWM_PIXEL_VECTYP         ARM_PIX_VECTYP(__API_MCWM_PIXEL_BIT_NUM)
#define __API_MCWM_VEC_INCR             ARM_PIX_VECELT(__API_MCWM_PIXEL_BIT_NUM)

/*! disable this feature by default */
#ifndef __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
#   define __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING               0
#endif

#ifndef __API_MCWM_CFG_1_HORIZONTAL_LINE
#   define __API_MCWM_CFG_1_HORIZONTAL_LINE                     0
#endif

#ifndef __API_MCWM_CFG_CHANNEL_8in32_SUPPORT
#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT                 0
#endif

#ifndef __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE  0
#endif

#ifndef __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE  0
#endif


#undef ____MCWM_FUNC
#undef ___MCWM_FUNC
#undef __MCWM_FUNC



#ifndef __API_MCWM_OP_NAME
#   define ____MCWM_FUNC(__NAME, __COLOUR)                                      \
        __MVE_WRAPPER(__arm_2d_impl_##__COLOUR##_##__NAME)
#   define ___MCWM_FUNC(__NAME, __COLOUR)   ____MCWM_FUNC(__NAME, __COLOUR)
#else
#   define _____MCWM_FUNC(__OP_NAME, __NAME, __COLOUR)                          \
        __MVE_WRAPPER(__arm_2d_impl_##__COLOUR##_##__OP_NAME##_##__NAME)
#   define ____MCWM_FUNC(__OP_NAME, __NAME, __COLOUR)                           \
        _____MCWM_FUNC(__OP_NAME, __NAME, __COLOUR)
#   define ___MCWM_FUNC(__NAME, __COLOUR)                                       \
        ____MCWM_FUNC(__API_MCWM_OP_NAME, __NAME, __COLOUR)
#endif

#define __MCWM_FUNC(__NAME)   ___MCWM_FUNC(__NAME, __API_MCWM_COLOUR)


#undef ____MCWM_TYPE
#undef ___MCWM_TYPE
#undef __MCWM_TYPE

#ifndef __API_MCWM_OP_NAME
#   define ____MCWM_TYPE(__NAME, __COLOUR)  arm_2d_##__COLOUR##_##__NAME
#   define ___MCWM_TYPE(__NAME, __COLOUR)   ____MCWM_TYPE(__NAME, __COLOUR)
#else
#   define _____MCWM_TYPE(__OP_NAME, __NAME, __COLOUR)                        \
        arm_2d_##__COLOUR##_##__OP_NAME##_##__NAME
#   define ____MCWM_TYPE(__OP_NAME, __NAME, __COLOUR)                         \
        _____MCWM_TYPE(__OP_NAME, __NAME, __COLOUR)
#   define ___MCWM_TYPE(__NAME, __COLOUR)                                     \
        ____MCWM_TYPE(__API_MCWM_OP_NAME, __NAME, __COLOUR)
#endif


#define __MCWM_TYPE(__NAME)   ___MCWM_TYPE(__NAME, __API_MCWM_COLOUR)


/*----------------------------------------------------------------------------*
 * Fill with Mirroring (both masks)                                           *
 *----------------------------------------------------------------------------*/


__OVERRIDE_WEAK
void __MCWM_FUNC(masks_fill)(
                        __API_MCWM_INT_TYPE * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                        uint32_t * __RESTRICT ptSourceMaskBase,
                    #else
                        uint8_t * __RESTRICT ptSourceMaskBase,
                    #endif
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                        __API_MCWM_INT_TYPE *__RESTRICT ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                        uint32_t *__RESTRICT ptTargetMaskBase,
                    #else
                        uint8_t *__RESTRICT ptTargetMaskBase,
                    #endif
                        int16_t iTargetMaskStride,
                        arm_2d_size_t *__RESTRICT ptTargetMaskSize)
{
    __API_MCWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(256);

#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
    uint32_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#else
    uint8_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#endif

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        __API_MCWM_INT_TYPE *__RESTRICT ptSource = ptSourceBase;
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
        uint32_t *ptSourceMask = ptSourceMaskBase;
    #else
        uint8_t *ptSourceMask = ptSourceMaskBase;
    #endif

    #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
        int_fast16_t iSourceMaskY = 0;
    #endif

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            __API_MCWM_INT_TYPE *__RESTRICT ptTarget = ptTargetBase;
        #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            uint32_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #else
            uint8_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #endif

            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                __API_MCWM_INT_TYPE       *__RESTRICT ptSrc = ptSource;
                __API_MCWM_INT_TYPE       *__RESTRICT ptTargetCur = ptTarget;

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                        __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                uint32_t        curIncStride4Idx = 0;
                __API_MCWM_PIXEL_VECTYP      vIncStride4Offs =
                    ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
            #else
                uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
            #endif
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #else
                uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #endif
                int32_t         blkCnt = wLength;

                do {
                    __API_MCWM_PIXEL_VECTYP      vecTarget = __API_MCWM_PIXEL_VECLOAD(ptTargetCur);
                    __API_MCWM_PIXEL_VECTYP      vecSource = __API_MCWM_PIXEL_VECLOAD(ptSrc);
                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    __API_MCWM_PIXEL_VECTYP      vecSrcMsk =
                                    ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)(
                                                    (uint8_t const *)ptSrcMsk, vIncStride4Offs);
                    #else
                    __API_MCWM_PIXEL_VECTYP      vecSrcMsk = ARM_CONNECT2(vldrbq_u,
                                                            __API_MCWM_PIXEL_BIT_NUM)(ptSrcMsk);
                    #endif

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    __API_MCWM_PIXEL_VECTYP      vecTargetMask =
                                    ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)(
                                            (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
                    #else
                    __API_MCWM_PIXEL_VECTYP      vecTargetMask = ARM_CONNECT2(vldrbq_u,
                                                            __API_MCWM_PIXEL_BIT_NUM)(ptTargetMaskCur);
                    #endif

                    __API_MCWM_PIXEL_VECTYP      vecHwOpacity =
                        vsubq(v256, (vecSrcMsk * vecTargetMask) >> 8);

                    #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                    vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                        ARM_CONNECT2(vcmpeqq_n_u, __API_MCWM_PIXEL_BIT_NUM)(vecHwOpacity, 2));
                    #endif

                    vecTarget = __API_MCWM_PIXEL_BLENDING(
                                                    vecTarget, vecSource, vecHwOpacity);
                    /* tail predication */
                    __API_MCWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget,
                                            ARM_CONNECT3(vctp, __API_MCWM_PIXEL_BIT_NUM, q)(blkCnt));

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                        __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
                    #endif

                    #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    ptSrcMsk += __API_MCWM_VEC_INCR;
                    #endif

                    #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    ptTargetMaskCur += __API_MCWM_VEC_INCR;
                    #endif

                    ptTargetCur += __API_MCWM_VEC_INCR;
                    ptSrc += __API_MCWM_VEC_INCR;

                    blkCnt -= __API_MCWM_VEC_INCR;
                }
                while (blkCnt > 0);

                ptTarget += wLength;
                ptTargetMask += wLength;

                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);

            /*---------------- Height Loop End----------------*/
            ptSource += iSourceStride;
            ptTargetBase += iTargetStride;

        #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
            iSourceMaskY++;
            //! handle source mask
            if (    (iSourceMaskY >= ptSourceMaskSize->iHeight)
               ||   (iSourceMaskY >= ptSourceSize->iHeight)) {
                ptSourceMask = ptSourceMaskBase;
                iSourceMaskY = 0;
            } else {
                ptSourceMask += iSourceMaskStride;
            }
        #else
            ptSourceMask += iSourceMaskStride;
        #endif

        #if __API_MCWM_CFG_1_HORIZONTAL_LINE
            ptTargetMaskLineBase = ptTargetMaskBase;
        #else
            ptTargetMaskLineBase += iTargetMaskStride;
        #endif

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}




#if CHECKPIXTYP(__API_MCWM_COLOUR, cccn888) && ENABLE_ALT_MASK_FILL

/* Alternative 32-bit fill with mask */
/* Operates on individual cccn888 pixel channel */
/* requires slower scat/gather for channel extraction */
/* but allows faster processing with vector of 8 elements  */
/* slightly faster than version above but deviates from 8 & 16-bit template versions */

__OVERRIDE_WEAK
void __MCWM_FUNC(masks_fill_alt)(
                        __API_MCWM_INT_TYPE * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                        uint32_t * __RESTRICT ptSourceMaskBase,
                    #else
                        uint8_t * __RESTRICT ptSourceMaskBase,
                    #endif
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                        __API_MCWM_INT_TYPE *__RESTRICT ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                        uint32_t *__RESTRICT ptTargetMaskBase,
                    #else
                        uint8_t *__RESTRICT ptTargetMaskBase,
                    #endif
                        int16_t iTargetMaskStride,
                        arm_2d_size_t *__RESTRICT ptTargetMaskSize)
{
    __API_MCWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(256);

#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
    uint32_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#else
    uint8_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#endif

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        __API_MCWM_INT_TYPE *__RESTRICT ptSource = ptSourceBase;
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
        uint32_t *ptSourceMask = ptSourceMaskBase;
    #else
        uint8_t *ptSourceMask = ptSourceMaskBase;
    #endif

    #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
        int_fast16_t iSourceMaskY = 0;
    #endif

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            __API_MCWM_INT_TYPE *__RESTRICT ptTarget = ptTargetBase;
        #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            uint32_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #else
            uint8_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #endif

            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/


                /*
                 * chan 0 (scat / gath with 8x16 elements )
                 */
                __API_MCWM_INT_TYPE       *__RESTRICT ptSrc = ptSource;
                __API_MCWM_INT_TYPE       *__RESTRICT ptTargetCur = ptTarget;

                uint32_t        curIncStride4Idx = 0;
                __API_MCWM_PIXEL_VECTYP      vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);


            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
            #else
                uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
            #endif
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #else
                uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #endif
                int32_t         blkCnt = wLength;

                do {
                    __API_MCWM_PIXEL_VECTYP      vecTarget = vldrbq_gather_offset_u16((uint8_t*)ptTargetCur, vIncStride4Offs);
                    __API_MCWM_PIXEL_VECTYP      vecSource = vldrbq_gather_offset_u16((uint8_t*)ptSrc, vIncStride4Offs);

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    __API_MCWM_PIXEL_VECTYP      vecSrcMsk = vldrbq_gather_offset_u16(
                                                    (uint8_t const *)ptSrcMsk, vIncStride4Offs);
                    #else
                    __API_MCWM_PIXEL_VECTYP      vecSrcMsk = ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)(ptSrcMsk);
                    #endif

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    __API_MCWM_PIXEL_VECTYP      vecTargetMask = vldrbq_gather_offset_u16(
                                            (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
                    #else
                    __API_MCWM_PIXEL_VECTYP      vecTargetMask = ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)(ptTargetMaskCur);
                    #endif

                    __API_MCWM_PIXEL_VECTYP      vecHwOpacity =
                        vsubq(v256, (vecSrcMsk * vecTargetMask) >> 8);

                    #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                    vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                        ARM_CONNECT2(vcmpeqq_n_u, __API_MCWM_PIXEL_BIT_NUM)(vecHwOpacity, 2));
                    #endif

                    vecTarget = __arm_2d_gray8_blending_opacity_single_vec(
                                                    vecTarget, vecSource, vecHwOpacity);
                    /* tail predication */
                    vstrbq_scatter_offset_p_u16((uint8_t*)ptTargetCur, vIncStride4Offs,
                            vecTarget, ARM_CONNECT3(vctp, __API_MCWM_PIXEL_BIT_NUM, q)(blkCnt));

                    vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);

                    #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    ptSrcMsk += __API_MCWM_VEC_INCR;
                    #endif

                    #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    ptTargetMaskCur += __API_MCWM_VEC_INCR;
                    #endif

                    ptTargetCur += __API_MCWM_VEC_INCR;
                    ptSrc += __API_MCWM_VEC_INCR;

                    blkCnt -= __API_MCWM_VEC_INCR;
                }
                while (blkCnt > 0);


                /*
                 * chan 1 (scat / gath with 8x16 elements )
                 */
                ptSrc = ptSource;
                ptTargetCur = ptTarget;

                curIncStride4Idx = 0;
                vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                ptSrcMsk = ptSourceMask;
            #else
                ptSrcMsk = ptSourceMask;
            #endif
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                ptTargetMaskCur = ptTargetMask;
            #else
                ptTargetMaskCur = ptTargetMask;
            #endif
                blkCnt = wLength;

                do {
                    __API_MCWM_PIXEL_VECTYP      vecTarget = vldrbq_gather_offset_u16(((uint8_t*)ptTargetCur)+1, vIncStride4Offs);
                    __API_MCWM_PIXEL_VECTYP      vecSource = vldrbq_gather_offset_u16(((uint8_t*)ptSrc)+1, vIncStride4Offs);

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    __API_MCWM_PIXEL_VECTYP      vecSrcMsk = vldrbq_gather_offset_u16(
                                                    (uint8_t const *)ptSrcMsk, vIncStride4Offs);
                    #else
                    __API_MCWM_PIXEL_VECTYP      vecSrcMsk = ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)(ptSrcMsk);
                    #endif

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    __API_MCWM_PIXEL_VECTYP      vecTargetMask = vldrbq_gather_offset_u16(
                                            (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
                    #else
                    __API_MCWM_PIXEL_VECTYP      vecTargetMask = ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)(ptTargetMaskCur);
                    #endif

                    __API_MCWM_PIXEL_VECTYP      vecHwOpacity =
                        vsubq(v256, (vecSrcMsk * vecTargetMask) >> 8);

                    #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                    vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                        ARM_CONNECT2(vcmpeqq_n_u, __API_MCWM_PIXEL_BIT_NUM)(vecHwOpacity, 2));
                    #endif

                    vecTarget = __arm_2d_gray8_blending_opacity_single_vec(
                                                    vecTarget, vecSource, vecHwOpacity);
                    /* tail predication */
                    vstrbq_scatter_offset_p_u16(((uint8_t*)ptTargetCur)+1, vIncStride4Offs,
                            vecTarget, ARM_CONNECT3(vctp, __API_MCWM_PIXEL_BIT_NUM, q)(blkCnt));

                    vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);

                    #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    ptSrcMsk += __API_MCWM_VEC_INCR;
                    #endif

                    #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    ptTargetMaskCur += __API_MCWM_VEC_INCR;
                    #endif

                    ptTargetCur += __API_MCWM_VEC_INCR;
                    ptSrc += __API_MCWM_VEC_INCR;

                    blkCnt -= __API_MCWM_VEC_INCR;
                }
                while (blkCnt > 0);


                /*
                 * chan 2 (scat / gath with 8x16 elements )
                 */
                ptSrc = ptSource;
                ptTargetCur = ptTarget;

                curIncStride4Idx = 0;
                vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);


            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                ptSrcMsk = ptSourceMask;
            #else
                ptSrcMsk = ptSourceMask;
            #endif
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                ptTargetMaskCur = ptTargetMask;
            #else
                ptTargetMaskCur = ptTargetMask;
            #endif
                blkCnt = wLength;

                do {
                    __API_MCWM_PIXEL_VECTYP      vecTarget = vldrbq_gather_offset_u16(((uint8_t*)ptTargetCur)+2, vIncStride4Offs);
                    __API_MCWM_PIXEL_VECTYP      vecSource = vldrbq_gather_offset_u16(((uint8_t*)ptSrc)+2, vIncStride4Offs);

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    __API_MCWM_PIXEL_VECTYP      vecSrcMsk = vldrbq_gather_offset_u16(
                                                    (uint8_t const *)ptSrcMsk, vIncStride4Offs);
                    #else
                    __API_MCWM_PIXEL_VECTYP      vecSrcMsk = ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)(ptSrcMsk);
                    #endif

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    __API_MCWM_PIXEL_VECTYP      vecTargetMask = vldrbq_gather_offset_u16(
                                            (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
                    #else
                    __API_MCWM_PIXEL_VECTYP      vecTargetMask = ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)(ptTargetMaskCur);
                    #endif

                    __API_MCWM_PIXEL_VECTYP      vecHwOpacity =
                        vsubq(v256, (vecSrcMsk * vecTargetMask) >> 8);

                    #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                    vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                        ARM_CONNECT2(vcmpeqq_n_u, __API_MCWM_PIXEL_BIT_NUM)(vecHwOpacity, 2));
                    #endif

                    vecTarget = __arm_2d_gray8_blending_opacity_single_vec(
                                                    vecTarget, vecSource, vecHwOpacity);
                    /* tail predication */
                    vstrbq_scatter_offset_p_u16(((uint8_t*)ptTargetCur)+2, vIncStride4Offs,
                            vecTarget, ARM_CONNECT3(vctp, __API_MCWM_PIXEL_BIT_NUM, q)(blkCnt));

                    vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);

                    #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    ptSrcMsk += __API_MCWM_VEC_INCR;
                    #endif

                    #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    ptTargetMaskCur += __API_MCWM_VEC_INCR;
                    #endif

                    ptTargetCur += __API_MCWM_VEC_INCR;
                    ptSrc += __API_MCWM_VEC_INCR;

                    blkCnt -= __API_MCWM_VEC_INCR;
                }
                while (blkCnt > 0);


                ptTarget += wLength;
                ptTargetMask += wLength;

                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);

            /*---------------- Height Loop End----------------*/
            ptSource += iSourceStride;
            ptTargetBase += iTargetStride;

        #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
            iSourceMaskY++;
            //! handle source mask
            if (    (iSourceMaskY >= ptSourceMaskSize->iHeight)
               ||   (iSourceMaskY >= ptSourceSize->iHeight)) {
                ptSourceMask = ptSourceMaskBase;
                iSourceMaskY = 0;
            } else {
                ptSourceMask += iSourceMaskStride;
            }
        #else
            ptSourceMask += iSourceMaskStride;
        #endif

        #if __API_MCWM_CFG_1_HORIZONTAL_LINE
            ptTargetMaskLineBase = ptTargetMaskBase;
        #else
            ptTargetMaskLineBase += iTargetMaskStride;
        #endif

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

#endif



__OVERRIDE_WEAK
void __MCWM_FUNC(masks_fill_x_mirror)(
                        __API_MCWM_INT_TYPE * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                        uint32_t * __RESTRICT ptSourceMaskBase,
                    #else
                        uint8_t * __RESTRICT ptSourceMaskBase,
                    #endif
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                        __API_MCWM_INT_TYPE *__RESTRICT ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                        uint32_t *__RESTRICT ptTargetMaskBase,
                    #else
                        uint8_t *__RESTRICT ptTargetMaskBase,
                    #endif
                        int16_t iTargetMaskStride,
                        arm_2d_size_t *__RESTRICT ptTargetMaskSize)
{
    __API_MCWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(256);
    uint16_t        srcWidth = ptSourceSize->iWidth;
#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
    uint32_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#else
    uint8_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#endif

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        __API_MCWM_INT_TYPE *__RESTRICT ptSource = ptSourceBase;
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
        uint32_t *ptSourceMask = ptSourceMaskBase;
    #else
        uint8_t *ptSourceMask = ptSourceMaskBase;
    #endif

    #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
        int_fast16_t iSourceMaskY = 0;
    #endif

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            __API_MCWM_INT_TYPE *__RESTRICT ptTarget = ptTargetBase;

        #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            uint32_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #else
            uint8_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #endif

            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                __API_MCWM_INT_TYPE       *__RESTRICT ptSrc = ptSource;
                __API_MCWM_INT_TYPE       *__RESTRICT ptTargetCur = ptTarget;

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                uint32_t        curIncStride4Idx = 0;
                __API_MCWM_PIXEL_VECTYP      vIncStride4Offs =
                                ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
            #else
                uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
            #endif
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #else
                uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #endif

                uint32_t        curDecrStride1Idx = srcWidth - 1;
                int32_t         blkCnt = wLength;
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                uint32_t        curDecrStride4Idx = 4*(srcWidth - 1);
                __API_MCWM_PIXEL_VECTYP      vDecrStride4Offs =
                            ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
            #endif
                __API_MCWM_PIXEL_VECTYP      vDecrStride1Offs =
                            ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


                do {
                    __API_MCWM_PIXEL_VECTYP      vecTarget = __API_MCWM_PIXEL_VECLOAD(ptTargetCur);
                    __API_MCWM_PIXEL_VECTYP      vecSource =
                        __API_MCWM_PIXEL_GATHVECLOAD(ptSrc, vDecrStride1Offs);
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    __API_MCWM_PIXEL_VECTYP      vecSrcMsk =
                        ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)
                                        ((uint8_t *)ptSrcMsk, vDecrStride4Offs);
            #else
                    __API_MCWM_PIXEL_VECTYP      vecSrcMsk =
                        ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)
                                        (ptSrcMsk, vDecrStride1Offs);
            #endif
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    __API_MCWM_PIXEL_VECTYP      vecTargetMask =
                                ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)(
                                            (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
            #else
                    __API_MCWM_PIXEL_VECTYP      vecTargetMask =
                                ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)((uint8_t *)ptTargetMaskCur);
            #endif
                    __API_MCWM_PIXEL_VECTYP      vecHwOpacity =
                        vsubq(v256, (vecSrcMsk * vecTargetMask) >> 8);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                    vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                        ARM_CONNECT2(vcmpeqq_n_u, __API_MCWM_PIXEL_BIT_NUM)(vecHwOpacity, 2));
            #endif

                    vecTarget =
                        __API_MCWM_PIXEL_BLENDING(vecTarget, vecSource,
                                                                    vecHwOpacity);

                    /* tail predication */
                    __API_MCWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget,
                                        ARM_CONNECT3(vctp, __API_MCWM_PIXEL_BIT_NUM, q)(blkCnt));

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    vDecrStride4Offs = ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
            #endif
                    vDecrStride1Offs = ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #else
                    ptTargetMaskCur += __API_MCWM_VEC_INCR;
            #endif
                    ptTargetCur += __API_MCWM_VEC_INCR;

                    blkCnt -= __API_MCWM_VEC_INCR;
                }
                while (blkCnt > 0);


                ptTarget += wLength;
                ptTargetMask += wLength;
                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);

            /*---------------- Height Loop End----------------*/
            ptSource += iSourceStride;
            ptTargetBase += iTargetStride;

        #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
            iSourceMaskY++;
            //! handle source mask
            if (    (iSourceMaskY >= ptSourceMaskSize->iHeight)
               ||   (iSourceMaskY >= ptSourceSize->iHeight)) {
                ptSourceMask = ptSourceMaskBase;
                iSourceMaskY = 0;
            } else {
                ptSourceMask += iSourceMaskStride;
            }
        #else
            ptSourceMask += iSourceMaskStride;
        #endif

        #if __API_MCWM_CFG_1_HORIZONTAL_LINE
            ptTargetMaskLineBase = ptTargetMaskBase;
        #else
            ptTargetMaskLineBase += iTargetMaskStride;
        #endif

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}



__OVERRIDE_WEAK
void __MCWM_FUNC(masks_fill_y_mirror)(
                        __API_MCWM_INT_TYPE * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                        uint32_t * __RESTRICT ptSourceMaskBase,
                    #else
                        uint8_t * __RESTRICT ptSourceMaskBase,
                    #endif
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                        __API_MCWM_INT_TYPE *__RESTRICT ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                        uint32_t *__RESTRICT ptTargetMaskBase,
                    #else
                        uint8_t *__RESTRICT ptTargetMaskBase,
                    #endif
                        int16_t iTargetMaskStride,
                        arm_2d_size_t *__RESTRICT ptTargetMaskSize)
{
    __API_MCWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(256);

    assert(ptSourceSize->iHeight <= ptSourceMaskSize->iHeight);
    ptSourceMaskBase += iSourceMaskStride * (ptSourceSize->iHeight - 1);

#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
    uint32_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#else
    uint8_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#endif

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        __API_MCWM_INT_TYPE *__RESTRICT ptSource
            = ptSourceBase + iSourceStride * (ptSourceSize->iHeight - 1);

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
        uint32_t *ptSourceMask = ptSourceMaskBase;
    #else
        uint8_t *ptSourceMask = ptSourceMaskBase;
    #endif

    #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
        int_fast16_t iSourceMaskY = 0;
    #endif

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            __API_MCWM_INT_TYPE *__RESTRICT ptTarget = ptTargetBase;
        #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            uint32_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #else
            uint8_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #endif

            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                __API_MCWM_INT_TYPE       *__RESTRICT ptSrc = ptSource;
                __API_MCWM_INT_TYPE       *__RESTRICT ptTargetCur = ptTarget;

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                        __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                uint32_t        curIncStride4Idx = 0;
                __API_MCWM_PIXEL_VECTYP      vIncStride4Offs =
                        ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
            #else
                uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
            #endif
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #else
                uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #endif
                int32_t         blkCnt = wLength;

                do {
                    __API_MCWM_PIXEL_VECTYP      vecTarget = __API_MCWM_PIXEL_VECLOAD(ptTargetCur);
                    __API_MCWM_PIXEL_VECTYP      vecSource = __API_MCWM_PIXEL_VECLOAD(ptSrc);
                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    __API_MCWM_PIXEL_VECTYP      vecSrcMsk = ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)(
                                                    (uint8_t const *)ptSrcMsk, vIncStride4Offs);
                    #else
                    __API_MCWM_PIXEL_VECTYP      vecSrcMsk = ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)(ptSrcMsk);
                    #endif

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    __API_MCWM_PIXEL_VECTYP      vecTargetMask =
                                            ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)(
                                                            (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
                    #else
                    __API_MCWM_PIXEL_VECTYP      vecTargetMask = ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)(ptTargetMaskCur);
                    #endif

                    __API_MCWM_PIXEL_VECTYP      vecHwOpacity =
                        vsubq(v256, (vecSrcMsk * vecTargetMask) >> 8);

                    #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                    vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                        ARM_CONNECT2(vcmpeqq_n_u, __API_MCWM_PIXEL_BIT_NUM)(vecHwOpacity, 2));
                    #endif

                    vecTarget = __API_MCWM_PIXEL_BLENDING(
                                                    vecTarget, vecSource, vecHwOpacity);
                    /* tail predication */
                    __API_MCWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget, ARM_CONNECT3(vctp, __API_MCWM_PIXEL_BIT_NUM, q)(blkCnt));

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                        __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
                    #endif

                    #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    ptSrcMsk += __API_MCWM_VEC_INCR;
                    #endif

                    #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    ptTargetMaskCur += __API_MCWM_VEC_INCR;
                    #endif

                    ptTargetCur += __API_MCWM_VEC_INCR;
                    ptSrc += __API_MCWM_VEC_INCR;

                    blkCnt -= __API_MCWM_VEC_INCR;
                }
                while (blkCnt > 0);

                ptTarget += wLength;
                ptTargetMask += wLength;

                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);

            /*---------------- Height Loop End----------------*/
            ptSource -= iSourceStride;
            ptTargetBase += iTargetStride;

        #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
            iSourceMaskY++;
            //! handle source mask
            if (    (iSourceMaskY >= ptSourceMaskSize->iHeight)
               ||   (iSourceMaskY >= ptSourceSize->iHeight)) {
                ptSourceMask = ptSourceMaskBase;
                iSourceMaskY = 0;
            } else {
                ptSourceMask -= iSourceMaskStride;
            }
        #else
            ptSourceMask -= iSourceMaskStride;
        #endif

        #if __API_MCWM_CFG_1_HORIZONTAL_LINE
            ptTargetMaskLineBase = ptTargetMaskBase;
        #else
            ptTargetMaskLineBase += iTargetMaskStride;
        #endif

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}



__OVERRIDE_WEAK
void __MCWM_FUNC(masks_fill_xy_mirror)(
                        __API_MCWM_INT_TYPE * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                        uint32_t * __RESTRICT ptSourceMaskBase,
                    #else
                        uint8_t * __RESTRICT ptSourceMaskBase,
                    #endif
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                        __API_MCWM_INT_TYPE *__RESTRICT ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                        uint32_t *__RESTRICT ptTargetMaskBase,
                    #else
                        uint8_t *__RESTRICT ptTargetMaskBase,
                    #endif
                        int16_t iTargetMaskStride,
                        arm_2d_size_t *__RESTRICT ptTargetMaskSize)
{
    assert(ptSourceSize->iHeight <= ptSourceMaskSize->iHeight);
    ptSourceMaskBase += iSourceMaskStride * (ptSourceSize->iHeight - 1);

    __API_MCWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(256);
    __API_MCWM_INT_TYPE        srcWidth = ptSourceSize->iWidth;
#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
    uint32_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#else
    uint8_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#endif

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        __API_MCWM_INT_TYPE *__RESTRICT ptSource
            = ptSourceBase + iSourceStride * (ptSourceSize->iHeight - 1);

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
        uint32_t *ptSourceMask = ptSourceMaskBase;
    #else
        uint8_t *ptSourceMask = ptSourceMaskBase;
    #endif

    #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
        int_fast16_t iSourceMaskY = 0;
    #endif

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            __API_MCWM_INT_TYPE *__RESTRICT ptTarget = ptTargetBase;

        #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            uint32_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #else
            uint8_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #endif

            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                __API_MCWM_INT_TYPE       *__RESTRICT ptSrc = ptSource;
                __API_MCWM_INT_TYPE       *__RESTRICT ptTargetCur = ptTarget;

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                uint32_t        curIncStride4Idx = 0;
                __API_MCWM_PIXEL_VECTYP      vIncStride4Offs =
                                    ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
            #else
                uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
            #endif
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #else
                uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #endif

                uint32_t        curDecrStride1Idx = srcWidth - 1;
                int32_t         blkCnt = wLength;
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                uint32_t        curDecrStride4Idx = 4*(srcWidth - 1);
                __API_MCWM_PIXEL_VECTYP      vDecrStride4Offs =
                            ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
            #endif
                __API_MCWM_PIXEL_VECTYP      vDecrStride1Offs =
                            ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


                do {
                    __API_MCWM_PIXEL_VECTYP      vecTarget = __API_MCWM_PIXEL_VECLOAD(ptTargetCur);
                    __API_MCWM_PIXEL_VECTYP      vecSource =
                        __API_MCWM_PIXEL_GATHVECLOAD(ptSrc, vDecrStride1Offs);
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    __API_MCWM_PIXEL_VECTYP      vecSrcMsk =
                            ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)((uint8_t *)ptSrcMsk, vDecrStride4Offs);

            #else
                    __API_MCWM_PIXEL_VECTYP      vecSrcMsk =
                            ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)(ptSrcMsk, vDecrStride1Offs);
            #endif
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    __API_MCWM_PIXEL_VECTYP      vecTargetMask =
                            ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)(
                                            (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
            #else
                    __API_MCWM_PIXEL_VECTYP      vecTargetMask =
                            ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)((uint8_t *)ptTargetMaskCur);
            #endif
                    __API_MCWM_PIXEL_VECTYP      vecHwOpacity =
                        vsubq(v256, (vecSrcMsk * vecTargetMask) >> 8);

                    #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                    vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                        ARM_CONNECT2(vcmpeqq_n_u, __API_MCWM_PIXEL_BIT_NUM)(vecHwOpacity, 2));
                    #endif

                    vecTarget =
                        __API_MCWM_PIXEL_BLENDING(vecTarget, vecSource,
                                                                    vecHwOpacity);

                    /* tail predication */
                    __API_MCWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget, ARM_CONNECT3(vctp, __API_MCWM_PIXEL_BIT_NUM, q)(blkCnt));

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    vDecrStride4Offs = ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
            #endif
                    vDecrStride1Offs = ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #else
                    ptTargetMaskCur += __API_MCWM_VEC_INCR;
            #endif
                    ptTargetCur += __API_MCWM_VEC_INCR;

                    blkCnt -= __API_MCWM_VEC_INCR;
                }
                while (blkCnt > 0);


                ptTarget += wLength;
                ptTargetMask += wLength;
                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);

            /*---------------- Height Loop End----------------*/
            ptSource -= iSourceStride;
            ptTargetBase += iTargetStride;

        #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
            iSourceMaskY++;
            //! handle source mask
            if (    (iSourceMaskY >= ptSourceMaskSize->iHeight)
               ||   (iSourceMaskY >= ptSourceSize->iHeight)) {
                ptSourceMask = ptSourceMaskBase;
                iSourceMaskY = 0;
            } else {
                ptSourceMask -= iSourceMaskStride;
            }
        #else
            ptSourceMask -= iSourceMaskStride;
        #endif

        #if __API_MCWM_CFG_1_HORIZONTAL_LINE
            ptTargetMaskLineBase = ptTargetMaskBase;
        #else
            ptTargetMaskLineBase += iTargetMaskStride;
        #endif

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}




/*----------------------------------------------------------------------------*
 * Copy with Mirroring  (both masks)                                          *
 *----------------------------------------------------------------------------*/


__OVERRIDE_WEAK
void __MCWM_FUNC(masks_copy)(
                __API_MCWM_INT_TYPE * __RESTRICT pSourceBase,
                int16_t iSourceStride,

#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                uint32_t * __RESTRICT ptSourceMaskBase,
#else
                uint8_t * __RESTRICT ptSourceMaskBase,
#endif
                int16_t iSourceMaskStride,
                arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                __API_MCWM_INT_TYPE * __RESTRICT pTargetBase,
                int16_t iTargetStride,

#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                uint32_t *__RESTRICT ptTargetMaskBase,
#else
                uint8_t *__RESTRICT ptTargetMaskBase,
#endif
                int16_t iTargetMaskStride,
                arm_2d_size_t *__RESTRICT ptTargetMaskSize,

                arm_2d_size_t * __RESTRICT ptCopySize)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    __API_MCWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(256);

#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
    uint32_t *ptSourceMask = ptSourceMaskBase;
#else
    uint8_t *ptSourceMask = ptSourceMaskBase;
#endif
#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
    uint32_t *ptTargetMask = ptTargetMaskBase;
#else
    uint8_t *ptTargetMask = ptTargetMaskBase;
#endif

#if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
    int_fast16_t iSourceMaskY = 0;
#endif

    for (   int_fast16_t y = 0;
            y < iHeight;
            y++) {
        __API_MCWM_INT_TYPE       *__RESTRICT ptSrc = pSourceBase;
        __API_MCWM_INT_TYPE       *__RESTRICT ptTargetCur = pTargetBase;

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
        uint32_t        curIncStride4Idx = 0;
        __API_MCWM_PIXEL_VECTYP      vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
    #endif

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
        uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
    #else
        uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
    #endif
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
        uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #else
        uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #endif
        int32_t         blkCnt = iWidth;

        do {
            __API_MCWM_PIXEL_VECTYP      vecTarget = __API_MCWM_PIXEL_VECLOAD(ptTargetCur);
            __API_MCWM_PIXEL_VECTYP      vecSource = __API_MCWM_PIXEL_VECLOAD(ptSrc);
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            __API_MCWM_PIXEL_VECTYP      vecSrcMsk = ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)(
                                            (uint8_t const *)ptSrcMsk, vIncStride4Offs);
            #else
            __API_MCWM_PIXEL_VECTYP      vecSrcMsk = ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)(ptSrcMsk);
            #endif

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            __API_MCWM_PIXEL_VECTYP      vecTargetMask = ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)(
                                    (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
            #else
            __API_MCWM_PIXEL_VECTYP      vecTargetMask = ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)(ptTargetMaskCur);
            #endif

            __API_MCWM_PIXEL_VECTYP      vecHwOpacity =
                vsubq(v256, (vecSrcMsk * vecTargetMask) >> 8);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                    ARM_CONNECT2(vcmpeqq_n_u, __API_MCWM_PIXEL_BIT_NUM)(vecHwOpacity, 2));
            #endif

            vecTarget = __API_MCWM_PIXEL_BLENDING(
                                            vecTarget, vecSource, vecHwOpacity);
            /* tail predication */
            __API_MCWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget, ARM_CONNECT3(vctp, __API_MCWM_PIXEL_BIT_NUM, q)(blkCnt));

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif

            #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            ptSrcMsk += __API_MCWM_VEC_INCR;
            #endif

            #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            ptTargetMaskCur += __API_MCWM_VEC_INCR;
            #endif

            ptTargetCur += __API_MCWM_VEC_INCR;
            ptSrc += __API_MCWM_VEC_INCR;
            blkCnt -= __API_MCWM_VEC_INCR;
        }
        while (blkCnt > 0);

        pSourceBase += (iSourceStride);
        pTargetBase += (iTargetStride);

    #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
        //! handle source mask
        iSourceMaskY++;
        if (    (iSourceMaskY >= ptSourceMaskSize->iHeight)
           ||   (iSourceMaskY >= iHeight)) {
            ptSourceMask = ptSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            ptSourceMask += (iSourceMaskStride);
        }
    #else
        ptSourceMask += (iSourceMaskStride);
    #endif

    #if __API_MCWM_CFG_1_HORIZONTAL_LINE
        ptTargetMask = ptTargetMaskBase;
    #else
        ptTargetMask += (iTargetMaskStride);
    #endif
    }
}


__OVERRIDE_WEAK
void __MCWM_FUNC(masks_copy_x_mirror)(
                            __API_MCWM_INT_TYPE * __RESTRICT pSourceBase,
                            int16_t iSourceStride,

                        #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                            uint32_t * __RESTRICT ptSourceMaskBase,
                        #else
                            uint8_t * __RESTRICT ptSourceMaskBase,
                        #endif
                            int16_t iSourceMaskStride,
                            arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                            __API_MCWM_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                        uint32_t *__RESTRICT ptTargetMaskBase,
                    #else
                        uint8_t *__RESTRICT ptTargetMaskBase,
                    #endif
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptTargetMaskSize,

                            arm_2d_size_t * __RESTRICT ptCopySize)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    __API_MCWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(256);

#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
    uint32_t *ptSourceMask = ptSourceMaskBase;
#else
    uint8_t *ptSourceMask = ptSourceMaskBase;
#endif
#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
    uint32_t *ptTargetMask = ptTargetMaskBase;
#else
    uint8_t *ptTargetMask = ptTargetMaskBase;
#endif

#if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
    int_fast16_t iSourceMaskY = 0;
#endif

    for (   int_fast16_t y = 0;
            y < iHeight;
            y++) {

        __API_MCWM_INT_TYPE       *__RESTRICT ptSrc = pSourceBase;
        __API_MCWM_INT_TYPE       *__RESTRICT ptTargetCur = pTargetBase;

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
        uint32_t        curIncStride4Idx = 0;
        __API_MCWM_PIXEL_VECTYP      vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
    #endif

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
        uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
    #else
        uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
    #endif
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
        uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #else
        uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #endif

        uint32_t        curDecrStride1Idx = iWidth - 1;
        int32_t         blkCnt = iWidth;
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
        uint32_t        curDecrStride4Idx = 4*(iWidth - 1);
        __API_MCWM_PIXEL_VECTYP      vDecrStride4Offs =
                ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
    #endif
        __API_MCWM_PIXEL_VECTYP      vDecrStride1Offs =
                ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


        do {
            __API_MCWM_PIXEL_VECTYP      vecTarget = __API_MCWM_PIXEL_VECLOAD(ptTargetCur);
            __API_MCWM_PIXEL_VECTYP      vecSource =
                __API_MCWM_PIXEL_GATHVECLOAD(ptSrc, vDecrStride1Offs);
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            __API_MCWM_PIXEL_VECTYP      vecSrcMsk =
                ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)((uint8_t *)ptSrcMsk, vDecrStride4Offs);
    #else
            __API_MCWM_PIXEL_VECTYP      vecSrcMsk =
                ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)(ptSrcMsk, vDecrStride1Offs);
    #endif
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            __API_MCWM_PIXEL_VECTYP      vecTargetMask =
                ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)(
                                    (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
    #else
            __API_MCWM_PIXEL_VECTYP      vecTargetMask =
                ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)((uint8_t *)ptTargetMaskCur);
    #endif
            __API_MCWM_PIXEL_VECTYP      vecHwOpacity =
                vsubq(v256, (vecSrcMsk * vecTargetMask) >> 8);

    #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                    ARM_CONNECT2(vcmpeqq_n_u, __API_MCWM_PIXEL_BIT_NUM)(vecHwOpacity, 2));
    #endif

            vecTarget =
                __API_MCWM_PIXEL_BLENDING(vecTarget, vecSource,
                                                            vecHwOpacity);

            /* tail predication */
            __API_MCWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget, ARM_CONNECT3(vctp, __API_MCWM_PIXEL_BIT_NUM, q)(blkCnt));

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            vDecrStride4Offs = ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
    #endif
            vDecrStride1Offs = ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
    #else
            ptTargetMaskCur += __API_MCWM_VEC_INCR;
    #endif
            ptTargetCur += __API_MCWM_VEC_INCR;

            blkCnt -= __API_MCWM_VEC_INCR;
        }
        while (blkCnt > 0);


        pSourceBase += iSourceStride;
        pTargetBase += iTargetStride;

    #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
        //! handle source mask
        iSourceMaskY++;
        if (    (iSourceMaskY >= ptSourceMaskSize->iHeight)
           ||   (iSourceMaskY >= iHeight)) {
            ptSourceMask = ptSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            ptSourceMask += iSourceMaskStride;
        }
    #else
        ptSourceMask += iSourceMaskStride;
    #endif

    #if __API_MCWM_CFG_1_HORIZONTAL_LINE
        ptTargetMask = ptTargetMaskBase;
    #else
        ptTargetMask += (iTargetMaskStride);
    #endif

    }
}


__OVERRIDE_WEAK
void __MCWM_FUNC(masks_copy_y_mirror)(
                            __API_MCWM_INT_TYPE * __RESTRICT pSourceBase,
                            int16_t iSourceStride,

                        #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                            uint32_t * __RESTRICT ptSourceMaskBase,
                        #else
                            uint8_t * __RESTRICT ptSourceMaskBase,
                        #endif
                            int16_t iSourceMaskStride,
                            arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                            __API_MCWM_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,

                        #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                            uint32_t *__RESTRICT ptTargetMaskBase,
                        #else
                            uint8_t *__RESTRICT ptTargetMaskBase,
                        #endif
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptTargetMaskSize,

                            arm_2d_size_t * __RESTRICT ptCopySize)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    __API_MCWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(256);

    pSourceBase += iSourceStride * (ptCopySize->iHeight - 1);

    assert (ptCopySize->iHeight <= ptSourceMaskSize->iHeight);
    ptSourceMaskBase += iSourceMaskStride * (ptCopySize->iHeight - 1);

#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
    uint32_t *ptSourceMask = ptSourceMaskBase;
#else
    uint8_t *ptSourceMask = ptSourceMaskBase;
#endif
#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
    uint32_t *ptTargetMask = ptTargetMaskBase;
#else
    uint8_t *ptTargetMask = ptTargetMaskBase;
#endif

#if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
    int_fast16_t iSourceMaskY = 0;
#endif

    for (   int_fast16_t y = 0;
            y < iHeight;
            y++) {
        __API_MCWM_INT_TYPE       *__RESTRICT ptSrc = pSourceBase;
        __API_MCWM_INT_TYPE       *__RESTRICT ptTargetCur = pTargetBase;

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
        uint32_t        curIncStride4Idx = 0;
        __API_MCWM_PIXEL_VECTYP      vIncStride4Offs =
                ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
    #endif

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
        uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
    #else
        uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
    #endif
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
        uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #else
        uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #endif
        int32_t         blkCnt = iWidth;

        do {
            __API_MCWM_PIXEL_VECTYP      vecTarget = __API_MCWM_PIXEL_VECLOAD(ptTargetCur);
            __API_MCWM_PIXEL_VECTYP      vecSource = __API_MCWM_PIXEL_VECLOAD(ptSrc);
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            __API_MCWM_PIXEL_VECTYP      vecSrcMsk =
                    ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)(
                                            (uint8_t const *)ptSrcMsk, vIncStride4Offs);
            #else
            __API_MCWM_PIXEL_VECTYP      vecSrcMsk =
                    ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)(ptSrcMsk);
            #endif

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            __API_MCWM_PIXEL_VECTYP      vecTargetMask =
                    ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)(
                                    (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
            #else
            __API_MCWM_PIXEL_VECTYP      vecTargetMask =
                    ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)(ptTargetMaskCur);
            #endif

            __API_MCWM_PIXEL_VECTYP      vecHwOpacity =
                vsubq(v256, (vecSrcMsk * vecTargetMask) >> 8);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                    ARM_CONNECT2(vcmpeqq_n_u, __API_MCWM_PIXEL_BIT_NUM)(vecHwOpacity, 2));
            #endif

            vecTarget = __API_MCWM_PIXEL_BLENDING(
                                            vecTarget, vecSource, vecHwOpacity);
            /* tail predication */
            __API_MCWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget,
                    ARM_CONNECT3(vctp, __API_MCWM_PIXEL_BIT_NUM, q)(blkCnt));

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif

            #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            ptSrcMsk += __API_MCWM_VEC_INCR;
            #endif

            #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            ptTargetMaskCur += __API_MCWM_VEC_INCR;
            #endif

            ptTargetCur += __API_MCWM_VEC_INCR;
            ptSrc += __API_MCWM_VEC_INCR;

            blkCnt -= __API_MCWM_VEC_INCR;
        }
        while (blkCnt > 0);

        pSourceBase -= iSourceStride;
        pTargetBase += (iTargetStride);

    #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
        //! handle source mask
        iSourceMaskY++;
        if (    (iSourceMaskY >= ptSourceMaskSize->iHeight)
           ||   (iSourceMaskY >= iHeight)) {
            ptSourceMask = ptSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            ptSourceMask -= iSourceMaskStride;
        }
    #else
        ptSourceMask -= iSourceMaskStride;
    #endif

    #if __API_MCWM_CFG_1_HORIZONTAL_LINE
        ptTargetMask = ptTargetMaskBase;
    #else
        ptTargetMask += (iTargetMaskStride);
    #endif
    }

}


__OVERRIDE_WEAK
void __MCWM_FUNC(masks_copy_xy_mirror)(
                            __API_MCWM_INT_TYPE * __RESTRICT pSourceBase,
                            int16_t iSourceStride,

                        #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                            uint32_t * __RESTRICT ptSourceMaskBase,
                        #else
                            uint8_t * __RESTRICT ptSourceMaskBase,
                        #endif
                            int16_t iSourceMaskStride,
                            arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                            __API_MCWM_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,

                        #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                            uint32_t *__RESTRICT ptTargetMaskBase,
                        #else
                            uint8_t *__RESTRICT ptTargetMaskBase,
                        #endif
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptTargetMaskSize,

                            arm_2d_size_t * __RESTRICT ptCopySize)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    __API_MCWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(256);

    pSourceBase += iSourceStride * (ptCopySize->iHeight - 1);

    assert (ptCopySize->iHeight <= ptSourceMaskSize->iHeight);
    ptSourceMaskBase += iSourceMaskStride * (ptCopySize->iHeight - 1);


#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
    uint32_t *ptSourceMask = ptSourceMaskBase;
#else
    uint8_t *ptSourceMask = ptSourceMaskBase;
#endif

#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
    uint32_t *ptTargetMask = ptTargetMaskBase;
#else
    uint8_t *ptTargetMask = ptTargetMaskBase;
#endif

#if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
    int_fast16_t iSourceMaskY = 0;
#endif

    for (   int_fast16_t y = 0;
            y < iHeight;
            y++) {
        __API_MCWM_INT_TYPE       *__RESTRICT ptSrc = pSourceBase;
        __API_MCWM_INT_TYPE       *__RESTRICT ptTargetCur = pTargetBase;

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
        uint32_t        curIncStride4Idx = 0;
        __API_MCWM_PIXEL_VECTYP      vIncStride4Offs =
                ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
    #endif

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
        uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
    #else
        uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
    #endif
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
        uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #else
        uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #endif

        uint32_t        curDecrStride1Idx = iWidth - 1;
        int32_t         blkCnt = iWidth;
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
        uint32_t        curDecrStride4Idx = 4*(iWidth - 1);
        __API_MCWM_PIXEL_VECTYP      vDecrStride4Offs =
                ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
    #endif
        __API_MCWM_PIXEL_VECTYP      vDecrStride1Offs =
                ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


        do {
            __API_MCWM_PIXEL_VECTYP      vecTarget = __API_MCWM_PIXEL_VECLOAD(ptTargetCur);
            __API_MCWM_PIXEL_VECTYP      vecSource =
                __API_MCWM_PIXEL_GATHVECLOAD(ptSrc, vDecrStride1Offs);
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            __API_MCWM_PIXEL_VECTYP      vecSrcMsk =
                ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)((uint8_t *)ptSrcMsk, vDecrStride4Offs);
    #else
            __API_MCWM_PIXEL_VECTYP      vecSrcMsk =
                ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)(ptSrcMsk, vDecrStride1Offs);
    #endif
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            __API_MCWM_PIXEL_VECTYP      vecTargetMask =
                ARM_CONNECT2(vldrbq_gather_offset_u, __API_MCWM_PIXEL_BIT_NUM)(
                                    (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
    #else
            __API_MCWM_PIXEL_VECTYP      vecTargetMask =
                ARM_CONNECT2(vldrbq_u, __API_MCWM_PIXEL_BIT_NUM)((uint8_t *)ptTargetMaskCur);
    #endif
            __API_MCWM_PIXEL_VECTYP      vecHwOpacity =
                vsubq(v256, (vecSrcMsk * vecTargetMask) >> 8);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_MCWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                    ARM_CONNECT2(vcmpeqq_n_u, __API_MCWM_PIXEL_BIT_NUM)(vecHwOpacity, 2));
            #endif

            vecTarget =
                __API_MCWM_PIXEL_BLENDING(vecTarget, vecSource,
                                                            vecHwOpacity);

            /* tail predication */
            __API_MCWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget, ARM_CONNECT3(vctp, __API_MCWM_PIXEL_BIT_NUM, q)(blkCnt));

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            vDecrStride4Offs = ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
    #endif
            vDecrStride1Offs = ARM_CONNECT2(vddupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_MCWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
    #else
            ptTargetMaskCur += __API_MCWM_VEC_INCR;
    #endif
            ptTargetCur += __API_MCWM_VEC_INCR;

            blkCnt -= __API_MCWM_VEC_INCR;
        }
        while (blkCnt > 0);

        pSourceBase -= iSourceStride;
        pTargetBase += (iTargetStride);

    #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
        //! handle source mask
        iSourceMaskY++;
        if (    (iSourceMaskY >= ptSourceMaskSize->iHeight)
           ||   (iSourceMaskY >= iHeight)) {
            ptSourceMask = ptSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            ptSourceMask -= iSourceMaskStride;
        }
    #else
        ptSourceMask -= iSourceMaskStride;
    #endif

    #if __API_MCWM_CFG_1_HORIZONTAL_LINE
        ptTargetMask = ptTargetMaskBase;
    #else
        ptTargetMask += (iTargetMaskStride);
    #endif
    }

}


#undef masks_fill
#undef masks_fill_x_mirror
#undef masks_fill_y_mirror
#undef masks_fill_xy_mirror
#undef masks_fill_mirror


#undef masks_copy
#undef masks_copy_x_mirror
#undef masks_copy_y_mirror
#undef masks_copy_xy_mirror
#undef masks_copy_mirror


#undef __API_MCWM_COPY_LIKE_OP_NAME
#undef __API_MCWM_OP_NAME
#undef ____MCWM_FUNC
#undef ___MCWM_FUNC
#undef __MCWM_FUNC
#undef __API_MCWM_COLOUR
#undef ____MCWM_TYPE
#undef ___MCWM_TYPE
#undef __MCWM_TYPE
#undef __API_MCWM_PIXEL_BIT_NUM
#undef __API_MCWM_PIXEL_VECLOAD
#undef __API_MCWM_PIXEL_PVECSTORE
#undef __API_MCWM_PIXEL_BLENDING
#undef __API_MCWM_PIXEL_GATHVECLOAD
#undef __API_MCWM_VEC_INCR
#undef __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
#undef __API_MCWM_CFG_1_HORIZONTAL_LINE
#undef __API_MCWM_CFG_CHANNEL_8in32_SUPPORT
#undef __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
#undef __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
