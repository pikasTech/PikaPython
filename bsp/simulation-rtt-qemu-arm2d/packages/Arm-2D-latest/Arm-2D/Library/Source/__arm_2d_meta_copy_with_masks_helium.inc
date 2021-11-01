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
 * Title:        __arm_2d_meta_copy_with_masks_helium.inc
 * Description:  c code template for copy and fill like operations
 *
 * $Date:        06. Oct 2021
 * $Revision:    V.1.0.0
 *
 * -------------------------------------------------------------------- */

#ifndef __API_MCWM_COLOUR
#   error You have to define __API_MCWM_COLOUR before using this c template
#endif

#if  __API_MCWM_COLOUR != rgb565
#error Unknown colour
#endif


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
        __arm_2d_impl_##__COLOUR##_##__NAME
#   define ___MCWM_FUNC(__NAME, __COLOUR)   ____MCWM_FUNC(__NAME, __COLOUR)
#else
#   define _____MCWM_FUNC(__OP_NAME, __NAME, __COLOUR)                          \
        __arm_2d_impl_##__COLOUR##_##__OP_NAME##_##__NAME
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


void __MCWM_FUNC(masks_fill)(
                        uint16_t * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                        uint32_t * __RESTRICT ptSourceMaskBase,
                    #else
                        uint8_t * __RESTRICT ptSourceMaskBase,
                    #endif
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                        uint16_t *__RESTRICT ptTargetBase,
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
    uint16x8_t      v256 = vdupq_n_u16(256);

#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
    uint32_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#else
    uint8_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#endif

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        uint16_t *__RESTRICT ptSource = ptSourceBase;
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
        uint32_t *ptSourceMask = ptSourceMaskBase;
    #else
        uint8_t *ptSourceMask = ptSourceMaskBase;
    #endif

    #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
        int_fast16_t iSourceMaskY = 0;
    #endif

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            uint16_t *__RESTRICT ptTarget = ptTargetBase;
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

                uint16_t       *__RESTRICT ptSrc = ptSource;
                uint16_t       *__RESTRICT ptTargetCur = ptTarget;

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                        __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                uint32_t        curIncStride4Idx = 0;
                uint16x8_t      vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
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
                    uint16x8_t      vecTarget = vld1q(ptTargetCur);
                    uint16x8_t      vecSource = vld1q(ptSrc);
                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    uint16x8_t      vecSrcMsk = vldrbq_gather_offset_u16(
                                                    (uint8_t const *)ptSrcMsk, vIncStride4Offs);
                    #else
                    uint16x8_t      vecSrcMsk = vldrbq_u16(ptSrcMsk);
                    #endif

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    uint16x8_t      vecTargetMask = vldrbq_gather_offset_u16(
                                            (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
                    #else
                    uint16x8_t      vecTargetMask = vldrbq_u16(ptTargetMaskCur);
                    #endif

                    uint16x8_t      vecHwOpacity =
                        vsubq_u16(v256, (vecSrcMsk * vecTargetMask) >> 8);

                    vecTarget = __arm_2d_rgb565_blending_opacity_single_vec(
                                                    vecTarget, vecSource, vecHwOpacity);
                    /* tail predication */
                    vst1q_p_u16(ptTargetCur, vecTarget, vctp16q(blkCnt));

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                        __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
                    #endif

                    #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    ptSrcMsk += 8;
                    #endif

                    #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    ptTargetMaskCur += 8;
                    #endif

                    ptTargetCur += 8;
                    ptSrc += 8;

                    blkCnt -= 8;
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
void __MCWM_FUNC(masks_fill_x_mirror)(
                        uint16_t * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                        uint32_t * __RESTRICT ptSourceMaskBase,
                    #else
                        uint8_t * __RESTRICT ptSourceMaskBase,
                    #endif
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                        uint16_t *__RESTRICT ptTargetBase,
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
    uint16x8_t      v256 = vdupq_n_u16(256);
    uint16_t        srcWidth = ptSourceSize->iWidth;
#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
    uint32_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#else
    uint8_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#endif

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        uint16_t *__RESTRICT ptSource = ptSourceBase;
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
        uint32_t *ptSourceMask = ptSourceMaskBase;
    #else
        uint8_t *ptSourceMask = ptSourceMaskBase;
    #endif

    #if __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
        int_fast16_t iSourceMaskY = 0;
    #endif

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            uint16_t *__RESTRICT ptTarget = ptTargetBase;

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

                uint16_t       *__RESTRICT ptSrc = ptSource;
                uint16_t       *__RESTRICT ptTargetCur = ptTarget;

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                uint32_t        curIncStride4Idx = 0;
                uint16x8_t      vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
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
                uint16x8_t      vDecrStride4Offs = vddupq_wb_u16(&curDecrStride4Idx, 4);
            #endif
                uint16x8_t      vDecrStride1Offs = vddupq_wb_u16(&curDecrStride1Idx, 1);


                do {
                    uint16x8_t      vecTarget = vld1q(ptTargetCur);
                    uint16x8_t      vecSource =
                        vldrhq_gather_shifted_offset(ptSrc, vDecrStride1Offs);
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    uint16x8_t      vecSrcMsk =
                        vldrbq_gather_offset_u16((uint8_t *)ptSrcMsk, vDecrStride4Offs);
            #else
                    uint16x8_t      vecSrcMsk =
                        vldrbq_gather_offset_u16(ptSrcMsk, vDecrStride1Offs);
            #endif
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    uint16x8_t      vecTargetMask = vldrbq_gather_offset_u16(
                                            (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
            #else
                    uint16x8_t      vecTargetMask = vldrbq_u16((uint8_t *)ptTargetMaskCur);
            #endif
                    uint16x8_t      vecHwOpacity =
                        vsubq_u16(v256, (vecSrcMsk * vecTargetMask) >> 8);

                    vecTarget =
                        __arm_2d_rgb565_blending_opacity_single_vec(vecTarget, vecSource,
                                                                    vecHwOpacity);

                    /* tail predication */
                    vst1q_p_u16(ptTargetCur, vecTarget, vctp16q(blkCnt));

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    vDecrStride4Offs = vddupq_wb_u16(&curDecrStride4Idx, 4);
            #endif
                    vDecrStride1Offs = vddupq_wb_u16(&curDecrStride1Idx, 1);


            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
            #else
                    ptTargetMaskCur += 8;
            #endif
                    ptTargetCur += 8;

                    blkCnt -= 8;
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
                        uint16_t * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                        uint32_t * __RESTRICT ptSourceMaskBase,
                    #else
                        uint8_t * __RESTRICT ptSourceMaskBase,
                    #endif
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                        uint16_t *__RESTRICT ptTargetBase,
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
    uint16x8_t      v256 = vdupq_n_u16(256);

    assert(ptSourceSize->iHeight <= ptSourceMaskSize->iHeight);
    ptSourceMaskBase += iSourceMaskStride * (ptSourceSize->iHeight - 1);

#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
    uint32_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#else
    uint8_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#endif

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        uint16_t *__RESTRICT ptSource
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

            uint16_t *__RESTRICT ptTarget = ptTargetBase;
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

                uint16_t       *__RESTRICT ptSrc = ptSource;
                uint16_t       *__RESTRICT ptTargetCur = ptTarget;

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                        __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                uint32_t        curIncStride4Idx = 0;
                uint16x8_t      vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
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
                    uint16x8_t      vecTarget = vld1q(ptTargetCur);
                    uint16x8_t      vecSource = vld1q(ptSrc);
                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    uint16x8_t      vecSrcMsk = vldrbq_gather_offset_u16(
                                                    (uint8_t const *)ptSrcMsk, vIncStride4Offs);
                    #else
                    uint16x8_t      vecSrcMsk = vldrbq_u16(ptSrcMsk);
                    #endif

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    uint16x8_t      vecTargetMask = vldrbq_gather_offset_u16(
                                            (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
                    #else
                    uint16x8_t      vecTargetMask = vldrbq_u16(ptTargetMaskCur);
                    #endif

                    uint16x8_t      vecHwOpacity =
                        vsubq_u16(v256, (vecSrcMsk * vecTargetMask) >> 8);

                    vecTarget = __arm_2d_rgb565_blending_opacity_single_vec(
                                                    vecTarget, vecSource, vecHwOpacity);
                    /* tail predication */
                    vst1q_p_u16(ptTargetCur, vecTarget, vctp16q(blkCnt));

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                        __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
                    #endif

                    #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    ptSrcMsk += 8;
                    #endif

                    #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    ptTargetMaskCur += 8;
                    #endif

                    ptTargetCur += 8;
                    ptSrc += 8;

                    blkCnt -= 8;
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
                        uint16_t * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                        uint32_t * __RESTRICT ptSourceMaskBase,
                    #else
                        uint8_t * __RESTRICT ptSourceMaskBase,
                    #endif
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                        uint16_t *__RESTRICT ptTargetBase,
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

    uint16x8_t      v256 = vdupq_n_u16(256);
    uint16_t        srcWidth = ptSourceSize->iWidth;
#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
    uint32_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#else
    uint8_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#endif

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        uint16_t *__RESTRICT ptSource
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
            uint16_t *__RESTRICT ptTarget = ptTargetBase;

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

                uint16_t       *__RESTRICT ptSrc = ptSource;
                uint16_t       *__RESTRICT ptTargetCur = ptTarget;

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                uint32_t        curIncStride4Idx = 0;
                uint16x8_t      vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
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
                uint16x8_t      vDecrStride4Offs = vddupq_wb_u16(&curDecrStride4Idx, 4);
            #endif
                uint16x8_t      vDecrStride1Offs = vddupq_wb_u16(&curDecrStride1Idx, 1);


                do {
                    uint16x8_t      vecTarget = vld1q(ptTargetCur);
                    uint16x8_t      vecSource =
                        vldrhq_gather_shifted_offset(ptSrc, vDecrStride1Offs);
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    uint16x8_t      vecSrcMsk =
                        vldrbq_gather_offset_u16((uint8_t *)ptSrcMsk, vDecrStride4Offs);
            #else
                    uint16x8_t      vecSrcMsk =
                        vldrbq_gather_offset_u16(ptSrcMsk, vDecrStride1Offs);
            #endif
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    uint16x8_t      vecTargetMask = vldrbq_gather_offset_u16(
                                            (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
            #else
                    uint16x8_t      vecTargetMask = vldrbq_u16((uint8_t *)ptTargetMaskCur);
            #endif
                    uint16x8_t      vecHwOpacity =
                        vsubq_u16(v256, (vecSrcMsk * vecTargetMask) >> 8);

                    vecTarget =
                        __arm_2d_rgb565_blending_opacity_single_vec(vecTarget, vecSource,
                                                                    vecHwOpacity);

                    /* tail predication */
                    vst1q_p_u16(ptTargetCur, vecTarget, vctp16q(blkCnt));

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                    vDecrStride4Offs = vddupq_wb_u16(&curDecrStride4Idx, 4);
            #endif
                    vDecrStride1Offs = vddupq_wb_u16(&curDecrStride1Idx, 1);


            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
                    vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
            #else
                    ptTargetMaskCur += 8;
            #endif
                    ptTargetCur += 8;

                    blkCnt -= 8;
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
                uint16_t * __RESTRICT pSourceBase,
                int16_t iSourceStride,

#if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                uint32_t * __RESTRICT ptSourceMaskBase,
#else
                uint8_t * __RESTRICT ptSourceMaskBase,
#endif
                int16_t iSourceMaskStride,
                arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                uint16_t * __RESTRICT pTargetBase,
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
    uint16x8_t      v256 = vdupq_n_u16(256);

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
        uint16_t       *__RESTRICT ptSrc = pSourceBase;
        uint16_t       *__RESTRICT ptTargetCur = pTargetBase;

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
        uint32_t        curIncStride4Idx = 0;
        uint16x8_t      vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
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
            uint16x8_t      vecTarget = vld1q(ptTargetCur);
            uint16x8_t      vecSource = vld1q(ptSrc);
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            uint16x8_t      vecSrcMsk = vldrbq_gather_offset_u16(
                                            (uint8_t const *)ptSrcMsk, vIncStride4Offs);
            #else
            uint16x8_t      vecSrcMsk = vldrbq_u16(ptSrcMsk);
            #endif

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            uint16x8_t      vecTargetMask = vldrbq_gather_offset_u16(
                                    (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
            #else
            uint16x8_t      vecTargetMask = vldrbq_u16(ptTargetMaskCur);
            #endif

            uint16x8_t      vecHwOpacity =
                vsubq_u16(v256, (vecSrcMsk * vecTargetMask) >> 8);

            vecTarget = __arm_2d_rgb565_blending_opacity_single_vec(
                                            vecTarget, vecSource, vecHwOpacity);
            /* tail predication */
            vst1q_p_u16(ptTargetCur, vecTarget, vctp16q(blkCnt));

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
            #endif

            #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            ptSrcMsk += 8;
            #endif

            #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            ptTargetMaskCur += 8;
            #endif

            ptTargetCur += 8;
            ptSrc += 8;
            blkCnt -= 8;
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
                            uint16_t * __RESTRICT pSourceBase,
                            int16_t iSourceStride,

                        #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                            uint32_t * __RESTRICT ptSourceMaskBase,
                        #else
                            uint8_t * __RESTRICT ptSourceMaskBase,
                        #endif
                            int16_t iSourceMaskStride,
                            arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                            uint16_t * __RESTRICT pTargetBase,
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
    uint16x8_t      v256 = vdupq_n_u16(256);

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

        uint16_t       *__RESTRICT ptSrc = pSourceBase;
        uint16_t       *__RESTRICT ptTargetCur = pTargetBase;

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
        uint32_t        curIncStride4Idx = 0;
        uint16x8_t      vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
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
        uint16x8_t      vDecrStride4Offs = vddupq_wb_u16(&curDecrStride4Idx, 4);
    #endif
        uint16x8_t      vDecrStride1Offs = vddupq_wb_u16(&curDecrStride1Idx, 1);


        do {
            uint16x8_t      vecTarget = vld1q(ptTargetCur);
            uint16x8_t      vecSource =
                vldrhq_gather_shifted_offset(ptSrc, vDecrStride1Offs);
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            uint16x8_t      vecSrcMsk =
                vldrbq_gather_offset_u16((uint8_t *)ptSrcMsk, vDecrStride4Offs);
    #else
            uint16x8_t      vecSrcMsk =
                vldrbq_gather_offset_u16(ptSrcMsk, vDecrStride1Offs);
    #endif
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            uint16x8_t      vecTargetMask = vldrbq_gather_offset_u16(
                                    (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
    #else
            uint16x8_t      vecTargetMask = vldrbq_u16((uint8_t *)ptTargetMaskCur);
    #endif
            uint16x8_t      vecHwOpacity =
                vsubq_u16(v256, (vecSrcMsk * vecTargetMask) >> 8);

            vecTarget =
                __arm_2d_rgb565_blending_opacity_single_vec(vecTarget, vecSource,
                                                            vecHwOpacity);

            /* tail predication */
            vst1q_p_u16(ptTargetCur, vecTarget, vctp16q(blkCnt));

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            vDecrStride4Offs = vddupq_wb_u16(&curDecrStride4Idx, 4);
    #endif
            vDecrStride1Offs = vddupq_wb_u16(&curDecrStride1Idx, 1);


    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
    #else
            ptTargetMaskCur += 8;
    #endif
            ptTargetCur += 8;

            blkCnt -= 8;
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
                            uint16_t * __RESTRICT pSourceBase,
                            int16_t iSourceStride,

                        #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                            uint32_t * __RESTRICT ptSourceMaskBase,
                        #else
                            uint8_t * __RESTRICT ptSourceMaskBase,
                        #endif
                            int16_t iSourceMaskStride,
                            arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                            uint16_t * __RESTRICT pTargetBase,
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
    uint16x8_t      v256 = vdupq_n_u16(256);

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
        uint16_t       *__RESTRICT ptSrc = pSourceBase;
        uint16_t       *__RESTRICT ptTargetCur = pTargetBase;

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
        uint32_t        curIncStride4Idx = 0;
        uint16x8_t      vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
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
            uint16x8_t      vecTarget = vld1q(ptTargetCur);
            uint16x8_t      vecSource = vld1q(ptSrc);
            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            uint16x8_t      vecSrcMsk = vldrbq_gather_offset_u16(
                                            (uint8_t const *)ptSrcMsk, vIncStride4Offs);
            #else
            uint16x8_t      vecSrcMsk = vldrbq_u16(ptSrcMsk);
            #endif

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            uint16x8_t      vecTargetMask = vldrbq_gather_offset_u16(
                                    (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
            #else
            uint16x8_t      vecTargetMask = vldrbq_u16(ptTargetMaskCur);
            #endif

            uint16x8_t      vecHwOpacity =
                vsubq_u16(v256, (vecSrcMsk * vecTargetMask) >> 8);

            vecTarget = __arm_2d_rgb565_blending_opacity_single_vec(
                                            vecTarget, vecSource, vecHwOpacity);
            /* tail predication */
            vst1q_p_u16(ptTargetCur, vecTarget, vctp16q(blkCnt));

            #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE || \
                __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
            #endif

            #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            ptSrcMsk += 8;
            #endif

            #if !__API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            ptTargetMaskCur += 8;
            #endif

            ptTargetCur += 8;
            ptSrc += 8;

            blkCnt -= 8;
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
                            uint16_t * __RESTRICT pSourceBase,
                            int16_t iSourceStride,

                        #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                            uint32_t * __RESTRICT ptSourceMaskBase,
                        #else
                            uint8_t * __RESTRICT ptSourceMaskBase,
                        #endif
                            int16_t iSourceMaskStride,
                            arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                            uint16_t * __RESTRICT pTargetBase,
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
    uint16x8_t      v256 = vdupq_n_u16(256);

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
        uint16_t       *__RESTRICT ptSrc = pSourceBase;
        uint16_t       *__RESTRICT ptTargetCur = pTargetBase;

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
        uint32_t        curIncStride4Idx = 0;
        uint16x8_t      vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
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
        uint16x8_t      vDecrStride4Offs = vddupq_wb_u16(&curDecrStride4Idx, 4);
    #endif
        uint16x8_t      vDecrStride1Offs = vddupq_wb_u16(&curDecrStride1Idx, 1);


        do {
            uint16x8_t      vecTarget = vld1q(ptTargetCur);
            uint16x8_t      vecSource =
                vldrhq_gather_shifted_offset(ptSrc, vDecrStride1Offs);
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            uint16x8_t      vecSrcMsk =
                vldrbq_gather_offset_u16((uint8_t *)ptSrcMsk, vDecrStride4Offs);
    #else
            uint16x8_t      vecSrcMsk =
                vldrbq_gather_offset_u16(ptSrcMsk, vDecrStride1Offs);
    #endif
    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            uint16x8_t      vecTargetMask = vldrbq_gather_offset_u16(
                                    (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
    #else
            uint16x8_t      vecTargetMask = vldrbq_u16((uint8_t *)ptTargetMaskCur);
    #endif
            uint16x8_t      vecHwOpacity =
                vsubq_u16(v256, (vecSrcMsk * vecTargetMask) >> 8);

            vecTarget =
                __arm_2d_rgb565_blending_opacity_single_vec(vecTarget, vecSource,
                                                            vecHwOpacity);

            /* tail predication */
            vst1q_p_u16(ptTargetCur, vecTarget, vctp16q(blkCnt));

    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            vDecrStride4Offs = vddupq_wb_u16(&curDecrStride4Idx, 4);
    #endif
            vDecrStride1Offs = vddupq_wb_u16(&curDecrStride1Idx, 1);


    #if __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
            vIncStride4Offs = vidupq_wb_u16(&curIncStride4Idx, 4);
    #else
            ptTargetMaskCur += 8;
    #endif
            ptTargetCur += 8;

            blkCnt -= 8;
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
#undef __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING
#undef __API_MCWM_CFG_1_HORIZONTAL_LINE
#undef __API_MCWM_CFG_CHANNEL_8in32_SUPPORT
#undef __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
#undef __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
