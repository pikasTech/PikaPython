
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
 * Title:        __arm_2d_draw_pattern_helium.inc
 * Description:  Helium code template for drawing pattern
 *
 * $Date:        29. sep 2021
 * $Revision:    V.0.0.3
 *
 * -------------------------------------------------------------------- */


#ifndef __API_COLOUR
#   error You have to define __API_COLOUR before using this c template
#endif
#ifndef __API_ELT_SZ
#   error You have to define the __API_ELT_SZ before using this c template
#endif

#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#define ____ARM_2D_FUNC(__NAME, __COLOUR)  __arm_2d_impl_##__COLOUR##_##__NAME
#define ___ARM_2D_FUNC(__NAME, __COLOUR)   ____ARM_2D_FUNC(__NAME, __COLOUR)
#define __ARM_2D_FUNC(__NAME)   ___ARM_2D_FUNC(__NAME, __API_COLOUR)



#if __API_ELT_SZ != 32

__OVERRIDE_WEAK
void __ARM_2D_FUNC(draw_pattern_fg_only)(uint8_t *__RESTRICT pchSourceBase,
                                         int32_t  iOffset,
                                         int16_t iSourceStride,
                                         ARM_PIX_SCLTYP(__API_ELT_SZ) *__RESTRICT pTargetBase,
                                         int16_t iTargetStride,
                                         arm_2d_size_t *__RESTRICT ptCopySize,
                                         ARM_PIX_SCLTYP(__API_ELT_SZ) ForeColour)
{
    //! get in byte offset
    iOffset &= 0x07;
    iSourceStride = (iSourceStride + 7) & ~0x07;

    /* deduces offset vector from iOffset for gather loading */
    ARM_PIX_VECTYP(__API_ELT_SZ)      offS =
            vld1q(ARM_CONNECT2(__draw_pattern_src_incr_, __API_COLOUR) + iOffset);
    /* deduces bitmask vector with wrap from iOffset */
    ARM_PIX_VECTYP(__API_ELT_SZ)      vBitMask =
            vld1q(ARM_CONNECT2(__draw_pattern_src_bitmask_, __API_COLOUR) + iOffset);


    if (ptCopySize->iWidth <= ARM_PIX_VECELT(__API_ELT_SZ)) {
        /* small width specialization */
        /* no inner loop */
        mve_pred16_t    p = ARM_CONNECT2(ARM_CONNECT2(vctp, __API_ELT_SZ), q)(ptCopySize->iWidth);

#ifdef USE_MVE_INTRINSICS
        for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
            ARM_PIX_VECTYP(__API_ELT_SZ)      vchSrc = vldrbq_gather_offset(pchSourceBase, offS);
            ARM_PIX_VECTYP(__API_ELT_SZ)      vTarg = vld1q(pTargetBase);

            vchSrc = vchSrc & vBitMask;
            vTarg = vdupq_m(vTarg, ForeColour, vcmpneq(vchSrc, 0));
            vst1q_p(pTargetBase, vTarg, p);

            pchSourceBase += (iSourceStride >> 3);
            pTargetBase += iTargetStride;
        }
#else
        ARM_PIX_VECTYP(__API_ELT_SZ) vForeG = ARM_CONNECT2(vdupq_n_u, __API_ELT_SZ)(ForeColour);

        __asm volatile(
            ".p2align 2                                             \n"
            "   wls             lr, %[iHeight], 1f                  \n"
            "2:                                                     \n"
            /* widened vector load  */
            ARM_VLDWID_ASM(8,__API_ELT_SZ) \
                              " q0, [%[src], %[offS]]               \n"
            "   vand            q0, q0, %[bitmask]                  \n"

            "   vcmp.i" TO_STRING(__API_ELT_SZ)  \
                              " eq, q0, zr                          \n"
            /* contigous vector load  */
            ARM_VLD1_ASM(__API_ELT_SZ) \
                              " q0, [%[target]]                     \n"
            "   vpsel           q0, q0,%[ForeG]                     \n"
            "   vmsr            P0, %[p]                            \n"
            "   vpst                                                \n"
            /* predicated contigous vector store  */
            ARM_VST1P_ASM(__API_ELT_SZ) \
                              " q0,[%[target]]                      \n"

            "   add             %[src], %[src], %[srcStride]        \n"
            "   add             %[target], %[target], %[targStride] \n"

            "   le               lr, 2b                             \n"
            "1:                                                     \n"

            : [src] "+r"(pchSourceBase),  [target] "+r" (pTargetBase)
            : [bitmask] "t"   (vBitMask),[ForeG] "t"(vForeG),
              [srcStride] "r" (iSourceStride >> 3),
              [targStride] "r" (iTargetStride * (__API_ELT_SZ/8)),
              [iHeight] "r" (ptCopySize->iHeight),
              [offS] "t" (offS), [p] "r" (p)
            : "q0", "memory", "lr");
#endif
    } else {
        for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
            int32_t             cnt = ptCopySize->iWidth;
            uint8_t            *pchSourceBaseCur = pchSourceBase;
            ARM_PIX_SCLTYP(__API_ELT_SZ) *pTargetBaseCur = pTargetBase;

#ifdef USE_MVE_INTRINSICS
            while (cnt > 0) {
                mve_pred16_t    p = ARM_CONNECT2(ARM_CONNECT2(vctp, __API_ELT_SZ), q)(cnt);
                ARM_PIX_VECTYP(__API_ELT_SZ)      vchSrc =
                    vldrbq_gather_offset_z(pchSourceBaseCur, offS, p);
                ARM_PIX_VECTYP(__API_ELT_SZ)      vTarg = vld1q_z(pTargetBaseCur, p);

                vchSrc = vandq_x(vchSrc, vBitMask, p);
                vTarg = vdupq_m(vTarg, ForeColour, vcmpneq_m(vchSrc, 0, p));

                vst1q_p(pTargetBaseCur, vTarg, p);

                pTargetBaseCur += ARM_PIX_VECELT(__API_ELT_SZ);
                pchSourceBaseCur += ARM_PIX_VECELT(__API_ELT_SZ) / 8;
                cnt -= ARM_PIX_VECELT(__API_ELT_SZ);
            }
#else
            ARM_PIX_VECTYP(__API_ELT_SZ) vForeG = ARM_CONNECT2(vdupq_n_u, __API_ELT_SZ)(ForeColour);

            __asm volatile(
                ".p2align 2                                             \n"
                "   wlstp."TO_STRING(__API_ELT_SZ) " lr, %[cnt], 1f     \n"
                "2:                                                     \n"
                /* widened vector load  */
                ARM_VLDWID_ASM(8,__API_ELT_SZ) \
                                  " q0, [%[src], %[offS]]               \n"

                "   vand            q0, q0, %[bitmask]                  \n"
                "   add             %[src], %[src], %[incrSrc]          \n"

                "   vpt.i" TO_STRING(__API_ELT_SZ)  \
                                  " ne, q0, zr                          \n"

                /* predicated contigous vector store  */
                ARM_VST1P_ASM(__API_ELT_SZ) \
                                  " %[ForeG],[%[target]], #16           \n"

                "   letp               lr, 2b                           \n"
                "1:                                                     \n"

                : [src] "+r"(pchSourceBaseCur),  [target] "+r" (pTargetBaseCur)
                : [bitmask] "t"   (vBitMask),[ForeG] "t"(vForeG),
                  [cnt] "r" (cnt), [incrSrc] "i" (ARM_PIX_VECELT(__API_ELT_SZ) / 8),
                  [offS] "t" (offS)
                : "q0", "memory", "lr");
#endif
            pchSourceBase += (iSourceStride >> 3);
            pTargetBase += iTargetStride;
        }
    }
}



__OVERRIDE_WEAK
void __ARM_2D_FUNC(draw_pattern_no_bg_comp)(
                                            uint8_t *__RESTRICT pchSourceBase,
                                            int32_t  iOffset,
                                            int16_t iSourceStride,
                                            ARM_PIX_SCLTYP(__API_ELT_SZ) *__RESTRICT pTargetBase,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *__RESTRICT ptCopySize)
{
    //! get in byte offset
    iOffset &= 0x07;
    iSourceStride = (iSourceStride + 7) & ~0x07;

    /* deduces offset vector from iOffset for gather loading */
    ARM_PIX_VECTYP(__API_ELT_SZ)      offS =
            vld1q(ARM_CONNECT2(__draw_pattern_src_incr_, __API_COLOUR) + iOffset);
    /* deduces bitmask vector with wrap from iOffset */
    ARM_PIX_VECTYP(__API_ELT_SZ)      vBitMask =
            vld1q(ARM_CONNECT2(__draw_pattern_src_bitmask_, __API_COLOUR) + iOffset);

    if (ptCopySize->iWidth <= ARM_PIX_VECELT(__API_ELT_SZ)) {
        /* small width specialization */
        /* no inner loop */
        mve_pred16_t    p = ARM_CONNECT2(ARM_CONNECT2(vctp, __API_ELT_SZ), q)(ptCopySize->iWidth);

#ifdef USE_MVE_INTRINSICS
        for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
            ARM_PIX_VECTYP(__API_ELT_SZ)      vchSrc = vldrbq_gather_offset(pchSourceBase, offS);
            ARM_PIX_VECTYP(__API_ELT_SZ)      vTarg = vld1q(pTargetBase);

            vchSrc = vchSrc & vBitMask;
            vTarg = vpselq(~vTarg, vTarg, vcmpneq(vchSrc, 0));

            vst1q_p(pTargetBase, vTarg, p);

            pchSourceBase += (iSourceStride >> 3);
            pTargetBase += iTargetStride;
        }
#else

        __asm volatile(
            ".p2align 2                                             \n"
            "   wls             lr, %[iHeight], 1f                  \n"
            "2:                                                     \n"
            /* widened vector load  */
            ARM_VLDWID_ASM(8,__API_ELT_SZ) \
                              " q0, [%[src], %[offS]]               \n"
            "   vand            q0, q0, %[bitmask]                  \n"

            ARM_VLD1_ASM(__API_ELT_SZ) \
                              " q1, [%[target]]                     \n"

            "   vpt.i" TO_STRING(__API_ELT_SZ)  \
                              " ne, q0, zr                          \n"
            "   vmvnt           q1, q1                              \n"

            "   vmsr            P0, %[p]                            \n"
            "   vpst                                                \n"
            /* predicated contigous vector store  */
            ARM_VST1P_ASM(__API_ELT_SZ) \
                              " q1,[%[target]]                      \n"

            "   add             %[src], %[src], %[srcStride]        \n"
            "   add             %[target], %[target], %[targStride] \n"

            "   le               lr, 2b                             \n"
            "1:                                                     \n"

            : [src] "+r"(pchSourceBase),  [target] "+r" (pTargetBase)
            : [bitmask] "t"   (vBitMask),
              [srcStride] "r" (iSourceStride >> 3),
              [targStride] "r" (iTargetStride*(__API_ELT_SZ/8)),
              [iHeight] "r" (ptCopySize->iHeight),
              [offS] "t" (offS), [p] "r" (p)
            : "q0", "q1", "memory", "lr");
#endif


    } else {

        for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
            int32_t             cnt = ptCopySize->iWidth;
            uint8_t            *pchSourceBaseCur = pchSourceBase;
            ARM_PIX_SCLTYP(__API_ELT_SZ) *pTargetBaseCur = pTargetBase;

#ifdef USE_MVE_INTRINSICS
            while (cnt > 0) {
                mve_pred16_t    p = ARM_CONNECT2(ARM_CONNECT2(vctp, __API_ELT_SZ), q)(cnt);
                ARM_PIX_VECTYP(__API_ELT_SZ)      vchSrc =
                    vldrbq_gather_offset_z(pchSourceBaseCur, offS, p);
                ARM_PIX_VECTYP(__API_ELT_SZ)      vTarg = vld1q_z(pTargetBaseCur, p);

                vchSrc = vandq_x(vchSrc, vBitMask, p);
                vTarg = vpselq(vmvnq_x(vTarg, p), vTarg, vcmpneq_m(vchSrc, 0, p));

                vst1q_p(pTargetBaseCur, vTarg, p);

                pTargetBaseCur += ARM_PIX_VECELT(__API_ELT_SZ);
                pchSourceBaseCur += ARM_PIX_VECELT(__API_ELT_SZ) / 8;
                cnt -= ARM_PIX_VECELT(__API_ELT_SZ);
            }
#else

            __asm volatile(
                ".p2align 2                                             \n"
                "   wlstp."TO_STRING(__API_ELT_SZ) \
                                  " lr, %[cnt], 1f                      \n"
                "2:                                                     \n"
                /* widened vector load  */
                ARM_VLDWID_ASM(8,__API_ELT_SZ) \
                                  " q0, [%[src], %[offS]]               \n"

                "   vand            q0, q0, %[bitmask]                  \n"
                "   add             %[src], %[src], %[incrSrc]          \n"

                ARM_VLD1_ASM(__API_ELT_SZ) \
                                  " q1, [%[target]]                     \n"

                "   vpt.i" TO_STRING(__API_ELT_SZ)  \
                                  " ne, q0, zr                          \n"
                "   vmvnt           q1, q1                              \n"
                /* contigous vector store  */
                ARM_VST1_ASM(__API_ELT_SZ) \
                                  " q1 ,[%[target]], #16                \n"

                "   letp            lr, 2b                              \n"
                "1:                                                     \n"

                : [src] "+r"(pchSourceBaseCur),  [target] "+r" (pTargetBaseCur)
                : [bitmask] "t"   (vBitMask),
                  [cnt] "r" (cnt), [incrSrc] "i" (ARM_PIX_VECELT(__API_ELT_SZ) / 8),
                  [offS] "t" (offS)
                : "q0", "q1", "memory", "lr");
#endif

            pchSourceBase += (iSourceStride >> 3);
            pTargetBase += iTargetStride;
        }
    }
}



__OVERRIDE_WEAK
void __ARM_2D_FUNC(draw_pattern_bg_only)(   uint8_t *__RESTRICT pchSourceBase,
                                            int32_t  iOffset,
                                            int16_t iSourceStride,
                                            ARM_PIX_SCLTYP(__API_ELT_SZ) *__RESTRICT pTargetBase,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *__RESTRICT ptCopySize,
                                            ARM_PIX_SCLTYP(__API_ELT_SZ) BackColour)
{
    //! get in byte offset
    iOffset &= 0x07;
    iSourceStride = (iSourceStride + 7) & ~0x07;

    /* deduces offset vector from iOffset for gather loading */
    ARM_PIX_VECTYP(__API_ELT_SZ)      offS =
            vld1q(ARM_CONNECT2(__draw_pattern_src_incr_, __API_COLOUR) + iOffset);
    /* deduces bitmask vector with wrap from iOffset */
    ARM_PIX_VECTYP(__API_ELT_SZ)      vBitMask =
            vld1q(ARM_CONNECT2(__draw_pattern_src_bitmask_, __API_COLOUR) + iOffset);

    if (ptCopySize->iWidth <= ARM_PIX_VECELT(__API_ELT_SZ)) {
        /* small width specialization */
        /* no inner loop */
        mve_pred16_t    p = ARM_CONNECT2(ARM_CONNECT2(vctp, __API_ELT_SZ), q)(ptCopySize->iWidth);

#ifdef USE_MVE_INTRINSICS
        for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
            ARM_PIX_VECTYP(__API_ELT_SZ)      vchSrc = vldrbq_gather_offset(pchSourceBase, offS);
            ARM_PIX_VECTYP(__API_ELT_SZ)      vTarg = vld1q(pTargetBase);

            vchSrc = vchSrc & vBitMask;
            vTarg = vdupq_m(vTarg, BackColour, vcmpeqq(vchSrc, 0));

            vst1q_p(pTargetBase, vTarg, p);

            pchSourceBase += (iSourceStride >> 3);
            pTargetBase += iTargetStride;
        }
#else
        ARM_PIX_VECTYP(__API_ELT_SZ) vBackG = ARM_CONNECT2(vdupq_n_u, __API_ELT_SZ)(BackColour);

        __asm volatile(
            ".p2align 2                                             \n"
            "   wls             lr, %[iHeight], 1f                  \n"
            "2:                                                     \n"
            /* widened vector load  */
            ARM_VLDWID_ASM(8,__API_ELT_SZ) \
                              " q0, [%[src], %[offS]]               \n"
            "   vand            q0, q0, %[bitmask]                  \n"

            "   vcmp.i" TO_STRING(__API_ELT_SZ)  \
                              " eq, q0, zr                          \n"
            /* contigous vector load  */
            ARM_VLD1_ASM(__API_ELT_SZ) \
                              " q0, [%[target]]                     \n"
            "   vpsel           q0, q0,%[vBackG]                    \n"
            "   vmsr            P0, %[p]                            \n"
            "   vpst                                                \n"
            /* predicated contigous vector store  */
            ARM_VST1P_ASM(__API_ELT_SZ) \
                              " q0,[%[target]]                      \n"

            "   add             %[src], %[src], %[srcStride]        \n"
            "   add             %[target], %[target], %[targStride] \n"

            "   le               lr, 2b                             \n"
            "1:                                                     \n"

            : [src] "+r"(pchSourceBase),  [target] "+r" (pTargetBase)
            : [bitmask] "t"   (vBitMask),[vBackG] "t"(vBackG),
              [srcStride] "r" (iSourceStride >> 3),
              [targStride] "r" (iTargetStride*(__API_ELT_SZ/8)),
              [iHeight] "r" (ptCopySize->iHeight),
              [offS] "t" (offS), [p] "r" (p)
            : "q0", "memory", "lr");
#endif


    } else {
        for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
            int32_t             cnt = ptCopySize->iWidth;
            uint8_t            *pchSourceBaseCur = pchSourceBase;
            ARM_PIX_SCLTYP(__API_ELT_SZ) *pTargetBaseCur = pTargetBase;

#ifdef USE_MVE_INTRINSICS
            while (cnt > 0) {
                mve_pred16_t    p = ARM_CONNECT2(ARM_CONNECT2(vctp, __API_ELT_SZ), q)(cnt);
                ARM_PIX_VECTYP(__API_ELT_SZ)      vchSrc =
                    vldrbq_gather_offset_z(pchSourceBaseCur, offS, p);
                ARM_PIX_VECTYP(__API_ELT_SZ)      vTarg = vld1q_z(pTargetBaseCur, p);

                vchSrc = vandq_x(vchSrc, vBitMask, p);
                vTarg = vdupq_m(vTarg, BackColour, vcmpeqq_m(vchSrc, 0, p));

                vst1q_p(pTargetBaseCur, vTarg, p);

                pTargetBaseCur += ARM_PIX_VECELT(__API_ELT_SZ);
                pchSourceBaseCur += ARM_PIX_VECELT(__API_ELT_SZ) / 8;
                cnt -= ARM_PIX_VECELT(__API_ELT_SZ);
            }
#else
            ARM_PIX_VECTYP(__API_ELT_SZ) vBackG = ARM_CONNECT2(vdupq_n_u, __API_ELT_SZ)(BackColour);

            __asm volatile(
                ".p2align 2                                             \n"
                "   wlstp."TO_STRING(__API_ELT_SZ) " lr, %[cnt], 1f     \n"
                "2:                                                     \n"
                /* widened vector load  */
                ARM_VLDWID_ASM(8,__API_ELT_SZ) \
                                  " q0, [%[src], %[offS]]               \n"

                "   vand            q0, q0, %[bitmask]                  \n"
                "   add             %[src], %[src], %[incrSrc]          \n"

                "   vpt.i" TO_STRING(__API_ELT_SZ)  \
                                  " ne, q0, zr                          \n"

                /* predicated contigous vector store  */
                ARM_VST1P_ASM(__API_ELT_SZ) \
                                  " %[vBackG],[%[target]], #16           \n"

                "   letp               lr, 2b                           \n"
                "1:                                                     \n"

                : [src] "+r"(pchSourceBaseCur),  [target] "+r" (pTargetBaseCur)
                : [bitmask] "t"   (vBitMask),[vBackG] "t"(vBackG),
                  [cnt] "r" (cnt), [incrSrc] "i" (ARM_PIX_VECELT(__API_ELT_SZ) / 8),
                  [offS] "t" (offS)
                : "q0", "memory", "lr");
#endif

            pchSourceBase += (iSourceStride >> 3);
            pTargetBase += iTargetStride;
        }
    }
}



__OVERRIDE_WEAK
void __ARM_2D_FUNC(draw_pattern_bg_fg)( uint8_t *__RESTRICT pchSourceBase,
                                        int32_t  iOffset,
                                        int16_t iSourceStride,
                                        ARM_PIX_SCLTYP(__API_ELT_SZ) *__RESTRICT pTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        ARM_PIX_SCLTYP(__API_ELT_SZ) ForeColour,
                                        ARM_PIX_SCLTYP(__API_ELT_SZ) BackColour)
{
    //! get in byte offset
    iOffset &= 0x07;
    iSourceStride = (iSourceStride + 7) & ~0x07;

    /* deduces offset vector from iOffset for gather loading */
    ARM_PIX_VECTYP(__API_ELT_SZ)      offS =
            vld1q(ARM_CONNECT2(__draw_pattern_src_incr_, __API_COLOUR) + iOffset);
    /* deduces bitmask vector with wrap from iOffset */
    ARM_PIX_VECTYP(__API_ELT_SZ)      vBitMask =
            vld1q(ARM_CONNECT2(__draw_pattern_src_bitmask_, __API_COLOUR) + iOffset);
    ARM_PIX_VECTYP(__API_ELT_SZ)      vFgColor =
                ARM_CONNECT2(vdupq_n_u, __API_ELT_SZ)(ForeColour);
    ARM_PIX_VECTYP(__API_ELT_SZ)      vBgColor =
                ARM_CONNECT2(vdupq_n_u, __API_ELT_SZ)(BackColour);

    if (ptCopySize->iWidth <= ARM_PIX_VECELT(__API_ELT_SZ)) {
        /* small width specialization */
        /* no inner loop */
        mve_pred16_t    p = ARM_CONNECT2(ARM_CONNECT2(vctp, __API_ELT_SZ), q)(ptCopySize->iWidth);

#ifdef USE_MVE_INTRINSICS
        for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
            ARM_PIX_VECTYP(__API_ELT_SZ)      vchSrc = vldrbq_gather_offset(pchSourceBase, offS);
            ARM_PIX_VECTYP(__API_ELT_SZ)      vTarg;

            vchSrc = vchSrc & vBitMask;
            vTarg =
                vpselq(vFgColor, vBgColor, vcmpneq(vchSrc, 0));

            vst1q_p(pTargetBase, vTarg, p);

            pchSourceBase += (iSourceStride >> 3);
            pTargetBase += iTargetStride;
        }
#else

        __asm volatile(
            ".p2align 2                                             \n"
            "   wls             lr, %[iHeight], 1f                  \n"
            "2:                                                     \n"
            /* widened vector load  */
            ARM_VLDWID_ASM(8,__API_ELT_SZ) \
                              " q0, [%[src], %[offS]]               \n"
            "   vand            q0, q0, %[bitmask]                  \n"

            "   vcmp.i" TO_STRING(__API_ELT_SZ)  \
                              " eq, q0, zr                          \n"
            "   vpsel           q0, %[vBackG], %[vForeG]            \n"
            "   vmsr            P0, %[p]                            \n"
            "   vpst                                                \n"
            /* predicated contigous vector store  */
            ARM_VST1P_ASM(__API_ELT_SZ) \
                              " q0,[%[target]]                      \n"

            "   add             %[src], %[src], %[srcStride]        \n"
            "   add             %[target], %[target], %[targStride] \n"

            "   le               lr, 2b                             \n"
            "1:                                                     \n"

            : [src] "+r"(pchSourceBase),  [target] "+r" (pTargetBase)
            : [bitmask] "t"   (vBitMask),[vBackG] "t"(vBgColor),
              [vForeG] "t" (vFgColor),
              [srcStride] "r" (iSourceStride >> 3),
              [targStride] "r" (iTargetStride*(__API_ELT_SZ/8)),
              [iHeight] "r" (ptCopySize->iHeight),
              [offS] "t" (offS), [p] "r" (p)
            : "q0", "memory", "lr");
#endif


    } else {
        for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
            int32_t             cnt = ptCopySize->iWidth;
            uint8_t            *pchSourceBaseCur = pchSourceBase;
            ARM_PIX_SCLTYP(__API_ELT_SZ) *pTargetBaseCur = pTargetBase;

#ifdef USE_MVE_INTRINSICS
            while (cnt > 0) {
                mve_pred16_t    p = ARM_CONNECT2(ARM_CONNECT2(vctp, __API_ELT_SZ), q)(cnt);
                ARM_PIX_VECTYP(__API_ELT_SZ)      vchSrc =
                    vldrbq_gather_offset_z(pchSourceBaseCur, offS, p);
                ARM_PIX_VECTYP(__API_ELT_SZ)      vTarg;

                vchSrc = vandq_x(vchSrc, vBitMask, p);
                vTarg =
                    vpselq(vFgColor, vBgColor, vcmpneq_m(vchSrc, 0, p));

                vst1q_p(pTargetBaseCur, vTarg, p);

                pTargetBaseCur += ARM_PIX_VECELT(__API_ELT_SZ);
                pchSourceBaseCur += ARM_PIX_VECELT(__API_ELT_SZ) / 8;
                cnt -= ARM_PIX_VECELT(__API_ELT_SZ);
            }
#else
            __asm volatile(
                ".p2align 2                                             \n"
                "   wlstp."TO_STRING(__API_ELT_SZ) " lr, %[cnt], 1f     \n"
                "2:                                                     \n"
                /* widened vector load  */
                ARM_VLDWID_ASM(8,__API_ELT_SZ) \
                                  " q0, [%[src], %[offS]]               \n"

                "   vand            q0, q0, %[bitmask]                  \n"
                "   add             %[src], %[src], %[incrSrc]          \n"

                "   vcmp.i" TO_STRING(__API_ELT_SZ)  \
                                  " eq, q0, zr                          \n"
                "   vpsel           q0, %[vBackG], %[vForeG]            \n"

                ARM_VST1_ASM(__API_ELT_SZ) \
                                  " q0, [%[target]], #16                \n"

                "   letp               lr, 2b                           \n"
                "1:                                                     \n"

                : [src] "+r"(pchSourceBaseCur),  [target] "+r" (pTargetBaseCur)
                : [bitmask] "t"   (vBitMask),[vBackG] "t"(vBgColor),
                  [vForeG] "t" (vFgColor),
                  [cnt] "r" (cnt), [incrSrc] "i" (ARM_PIX_VECELT(__API_ELT_SZ) / 8),
                  [offS] "t" (offS)
                : "q0", "memory", "lr");
#endif
            pchSourceBase += (iSourceStride >> 3);
            pTargetBase += iTargetStride;
        }
    }
}



__OVERRIDE_WEAK
void __ARM_2D_FUNC(draw_pattern_bg_comp)(   uint8_t *__RESTRICT pchSourceBase,
                                            int32_t  iOffset,
                                            int16_t iSourceStride,
                                            ARM_PIX_SCLTYP(__API_ELT_SZ) *__RESTRICT pTargetBase,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *__RESTRICT ptCopySize,
                                            ARM_PIX_SCLTYP(__API_ELT_SZ) BackColour)
{
    //! get in byte offset
    iOffset &= 0x07;
    iSourceStride = (iSourceStride + 7) & ~0x07;

    /* deduces offset vector from iOffset for gather loading */
    ARM_PIX_VECTYP(__API_ELT_SZ)      offS =
            vld1q(ARM_CONNECT2(__draw_pattern_src_incr_, __API_COLOUR) + iOffset);
    /* deduces bitmask vector with wrap from iOffset */
    ARM_PIX_VECTYP(__API_ELT_SZ)      vBitMask =
            vld1q(ARM_CONNECT2(__draw_pattern_src_bitmask_, __API_COLOUR) + iOffset);
    ARM_PIX_VECTYP(__API_ELT_SZ)      vBgColor =
            ARM_CONNECT2(vdupq_n_u, __API_ELT_SZ)(BackColour);

    if (ptCopySize->iWidth <= ARM_PIX_VECELT(__API_ELT_SZ)) {
        /* small width specialization */
        /* no inner loop */
        mve_pred16_t    p = ARM_CONNECT2(ARM_CONNECT2(vctp, __API_ELT_SZ), q)(ptCopySize->iWidth);

#ifdef USE_MVE_INTRINSICS
        for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
            ARM_PIX_VECTYP(__API_ELT_SZ)      vchSrc = vldrbq_gather_offset(pchSourceBase, offS);
            ARM_PIX_VECTYP(__API_ELT_SZ)      vTarg = vld1q(pTargetBase);

            vchSrc = vchSrc & vBitMask;
            vTarg = vpselq(vmvnq(vTarg), vBgColor, vcmpneq(vchSrc, 0));

            vst1q_p(pTargetBase, vTarg, p);

            pchSourceBase += (iSourceStride >> 3);
            pTargetBase += iTargetStride;
        }
#else
        __asm volatile(
            ".p2align 2                                             \n"
            "   wls             lr, %[iHeight], 1f                  \n"
            "2:                                                     \n"
            /* widened vector load  */
            ARM_VLDWID_ASM(8,__API_ELT_SZ) \
                              " q0, [%[src], %[offS]]               \n"
            "   vand            q0, q0, %[bitmask]                  \n"

            ARM_VLD1_ASM(__API_ELT_SZ) \
                              " q1, [%[target]]                     \n"

            "   vmvn            q1, q1                              \n"

            "   vcmp.i" TO_STRING(__API_ELT_SZ)  \
                              " eq, q0, zr                          \n"
            "   vpsel           q1, %[vBackG], q1                   \n"

            "   vmsr            P0, %[p]                            \n"
            "   vpst                                                \n"
            /* predicated contigous vector store  */
            ARM_VST1P_ASM(__API_ELT_SZ) \
                              " q1,[%[target]]                      \n"

            "   add             %[src], %[src], %[srcStride]        \n"
            "   add             %[target], %[target], %[targStride] \n"

            "   le               lr, 2b                             \n"
            "1:                                                     \n"

            : [src] "+r"(pchSourceBase),  [target] "+r" (pTargetBase)
            : [bitmask] "t"   (vBitMask), [vBackG] "t" (vBgColor),
              [srcStride] "r" (iSourceStride >> 3),
              [targStride] "r" (iTargetStride*(__API_ELT_SZ/8)),
              [iHeight] "r" (ptCopySize->iHeight),
              [offS] "t" (offS), [p] "r" (p)
            : "q0", "q1", "memory", "lr");
#endif
    } else {
        for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
            int32_t             cnt = ptCopySize->iWidth;
            uint8_t            *pchSourceBaseCur = pchSourceBase;
            ARM_PIX_SCLTYP(__API_ELT_SZ) *pTargetBaseCur = pTargetBase;

#ifdef USE_MVE_INTRINSICS
            while (cnt > 0) {
                mve_pred16_t    p = ARM_CONNECT2(ARM_CONNECT2(vctp, __API_ELT_SZ), q)(cnt);
                ARM_PIX_VECTYP(__API_ELT_SZ)      vchSrc =
                    vldrbq_gather_offset_z(pchSourceBaseCur, offS, p);
                ARM_PIX_VECTYP(__API_ELT_SZ)      vTarg = vld1q_z(pTargetBaseCur, p);

                vchSrc = vandq_x(vchSrc, vBitMask, p);
                vTarg =
                    vpselq(vmvnq_x(vTarg, p), vBgColor, vcmpneq_m(vchSrc, 0, p));

                vst1q_p(pTargetBaseCur, vTarg, p);

                pTargetBaseCur += ARM_PIX_VECELT(__API_ELT_SZ);
                pchSourceBaseCur += ARM_PIX_VECELT(__API_ELT_SZ) / 8;
                cnt -= ARM_PIX_VECELT(__API_ELT_SZ);
            }
#else
            __asm volatile(
                ".p2align 2                                             \n"
                "   wlstp."TO_STRING(__API_ELT_SZ) " lr, %[cnt], 1f     \n"
                "2:                                                     \n"
                /* widened vector load  */
                ARM_VLDWID_ASM(8,__API_ELT_SZ) \
                                  " q0, [%[src], %[offS]]               \n"

                "   vand            q0, q0, %[bitmask]                  \n"
                "   add             %[src], %[src], %[incrSrc]          \n"

                ARM_VLD1_ASM(__API_ELT_SZ) \
                                  " q1, [%[target]]                     \n"

                "   vmvn            q1, q1                              \n"

                "   vcmp.i" TO_STRING(__API_ELT_SZ)  \
                                  " eq, q0, zr                          \n"
                "   vpsel           q1, %[vBackG], q1                   \n"

                ARM_VST1_ASM(__API_ELT_SZ) \
                                  " q1, [%[target]], #16                \n"

                "   letp               lr, 2b                           \n"
                "1:                                                     \n"

                : [src] "+r"(pchSourceBaseCur),  [target] "+r" (pTargetBaseCur)
                : [bitmask] "t"   (vBitMask), [vBackG] "t" (vBgColor),
                  [cnt] "r" (cnt), [incrSrc] "i" (ARM_PIX_VECELT(__API_ELT_SZ) / 8),
                  [offS] "t" (offS)
                : "q0", "q1", "memory", "lr");
#endif
            pchSourceBase += (iSourceStride >> 3);
            pTargetBase += iTargetStride;
        }
    }
}

#else /* __API_ELT_SZ != 32 */

#define GENERIC_RGB32_DRAW_PATTERN(SELECTOR)                                            \
    if (ptCopySize->iWidth <= 4) {                                                      \
        /* very tall width case */                                                      \
        /* only bottom parts of gather load and bitmask needed */                       \
        /* no inner loop */                                                             \
        for (int32_t y = 0; y < ptCopySize->iHeight; y++) {                             \
            mve_pred16_t    p = vctp32q(ptCopySize->iWidth);                            \
            uint32x4_t      vchSrc = vldrbq_gather_offset_u32(pchSourceBase, offSLo);   \
            uint32x4_t      vTarg = vld1q(pTargetBase);                                 \
                                                                                        \
            vchSrc = vandq(vchSrc, vBitMaskLo);                                         \
            vTarg = SELECTOR(vcmpneq_n_u32(vchSrc, 0));                                 \
                                                                                        \
            vst1q_p(pTargetBase, vTarg, p);                                             \
                                                                                        \
            pchSourceBase += (iSourceStride >> 3);                                      \
            pTargetBase += iTargetStride;                                               \
        }                                                                               \
    } else if (ptCopySize->iWidth <= 8) {                                               \
        /* bottom and partial upper parts of gather load and bitmask needed */          \
        /* no inner loop */                                                             \
        for (int32_t y = 0; y < ptCopySize->iHeight; y++) {                             \
            mve_pred16_t    p = vctp32q(ptCopySize->iWidth - 4);                        \
            uint32x4_t      vchSrc = vldrbq_gather_offset_u32(pchSourceBase, offSLo);   \
            uint32x4_t      vTarg = vld1q(pTargetBase);                                 \
                                                                                        \
            vchSrc = vandq(vchSrc, vBitMaskLo);                                         \
            vTarg = SELECTOR(vcmpneq_n_u32(vchSrc, 0));                                 \
                                                                                        \
            vst1q(pTargetBase, vTarg);                                                  \
                                                                                        \
            vchSrc = vldrbq_gather_offset_u32(pchSourceBase, offSHi);                   \
            vTarg = vld1q(pTargetBase + 4);                                             \
                                                                                        \
            vchSrc = vandq(vchSrc, vBitMaskHi);                                         \
            vTarg = SELECTOR(vcmpneq_n_u32(vchSrc, 0));                                 \
                                                                                        \
            vst1q_p(pTargetBase + 4, vTarg, p);                                         \
                                                                                        \
            pchSourceBase += (iSourceStride >> 3);                                      \
            pTargetBase += iTargetStride;                                               \
        }                                                                               \
    } else {                                                                            \
        /* generic case */                                                              \
        for (int32_t y = 0; y < ptCopySize->iHeight; y++) {                             \
            int32_t         cnt = ptCopySize->iWidth;                                   \
            uint8_t        *pchSourceBaseCur = pchSourceBase;                           \
            uint32_t       *pTargetBaseCur = pTargetBase;                               \
                                                                                        \
            while (cnt >= 8) {                                                          \
                uint32x4_t      vchSrc =                                                \
                    vldrbq_gather_offset_u32(pchSourceBaseCur, offSLo);                 \
                uint32x4_t      vTarg = vld1q(pTargetBaseCur);                          \
                                                                                        \
                vchSrc = vandq(vchSrc, vBitMaskLo);                                     \
                vTarg = SELECTOR(vcmpneq_n_u32(vchSrc, 0));                             \
                                                                                        \
                vst1q(pTargetBaseCur, vTarg);                                           \
                pTargetBaseCur += 4;                                                    \
                                                                                        \
                vchSrc = vldrbq_gather_offset_u32(pchSourceBaseCur, offSHi);            \
                vTarg = vld1q(pTargetBaseCur);                                          \
                                                                                        \
                vchSrc = vandq(vchSrc, vBitMaskHi);                                     \
                vTarg = SELECTOR(vcmpneq_n_u32(vchSrc, 0));                             \
                                                                                        \
                vst1q(pTargetBaseCur, vTarg);                                           \
                pTargetBaseCur += 4;                                                    \
                                                                                        \
                pchSourceBaseCur += 1;                                                  \
                cnt -= 8;                                                               \
            }                                                             \
                                                                                        \
            /* tail */                                                                  \
            if (cnt > 4) {                                                              \
                /* bottom part + upper residual parts */                                \
                uint32x4_t      vchSrc =                                                \
                    vldrbq_gather_offset_u32(pchSourceBaseCur, offSLo);                 \
                uint32x4_t      vTarg = vld1q(pTargetBaseCur);                          \
                cnt -= 4;                                                               \
                mve_pred16_t    p = vctp32q(cnt);                                       \
                                                                                        \
                vchSrc = vandq(vchSrc, vBitMaskLo);                                     \
                vTarg = SELECTOR(vcmpneq_n_u32(vchSrc, 0));                             \
                                                                                        \
                vst1q(pTargetBaseCur, vTarg);                                           \
                pTargetBaseCur += 4;                                                    \
                                                                                        \
                vchSrc = vldrbq_gather_offset_u32(pchSourceBaseCur, offSHi);            \
                vTarg = vld1q(pTargetBaseCur);                                          \
                                                                                        \
                vchSrc = vandq(vchSrc, vBitMaskHi);                                     \
                vTarg = SELECTOR(vcmpneq_n_u32(vchSrc, 0));                             \
                                                                                        \
                vst1q_p(pTargetBaseCur, vTarg, p);                                      \
            } else if (cnt > 0) {                                                       \
                /* bottom part residual */                                              \
                uint32x4_t      vchSrc =                                                \
                    vldrbq_gather_offset_u32(pchSourceBaseCur, offSLo);                 \
                uint32x4_t      vTarg = vld1q(pTargetBaseCur);                          \
                mve_pred16_t    p = vctp32q(cnt);                                       \
                                                                                        \
                vchSrc = vandq(vchSrc, vBitMaskLo);                                     \
                vTarg = SELECTOR(vcmpneq_n_u32(vchSrc, 0));                             \
                                                                                        \
                vst1q_p(pTargetBaseCur, vTarg, p);                                      \
            }                                                                           \
                                                                                        \
            pchSourceBase += (iSourceStride >> 3);                                      \
            pTargetBase += iTargetStride;                                               \
        }                                                                               \
    }

__OVERRIDE_WEAK
void __arm_2d_impl_rgb32_draw_pattern_fg_only(uint8_t *__RESTRICT pchSourceBase,
                                         int32_t  iOffset,
                                         int16_t iSourceStride,
                                         uint32_t *__RESTRICT pTargetBase,
                                         int16_t iTargetStride,
                                         arm_2d_size_t *__RESTRICT ptCopySize,
                                         uint32_t hwForeColour)
{
    //! get in byte offset
    iOffset &= 0x07;
    iSourceStride = (iSourceStride + 7) & ~0x07;

    /* deduces offset vector from iOffset for gather loading */
    /* hold  8 contiguous values into 2 32-bit vector pair */
    uint32x4_t      offSLo = vld1q(__draw_pattern_src_incr_rgb32 + iOffset);
    uint32x4_t      offSHi = vld1q(__draw_pattern_src_incr_rgb32 + iOffset + 4);
    /* deduces bitmask vector with wrap from iOffset */
    /* hold  8 contiguous values into 2 32-bit vector pair */
    uint32x4_t      vBitMaskLo = vld1q(__draw_pattern_src_bitmask_rgb32 + iOffset);
    uint32x4_t      vBitMaskHi = vld1q(__draw_pattern_src_bitmask_rgb32 + iOffset + 4);

#define SEL_FG_ONLY(p)          vdupq_m_n_u32(vTarg, hwForeColour, p);

    GENERIC_RGB32_DRAW_PATTERN(SEL_FG_ONLY);
}



__OVERRIDE_WEAK
void __arm_2d_impl_rgb32_draw_pattern_no_bg_comp(uint8_t *__RESTRICT pchSourceBase,
                                         int32_t  iOffset,
                                         int16_t iSourceStride,
                                         uint32_t *__RESTRICT pTargetBase,
                                         int16_t iTargetStride,
                                         arm_2d_size_t *__RESTRICT ptCopySize)
{
    //! get in byte offset
    iOffset &= 0x07;
    iSourceStride = (iSourceStride + 7) & ~0x07;

    /* deduces offset vector from iOffset for gather loading */
    /* hold  8 contiguous values into 2 32-bit vector pair */
    uint32x4_t      offSLo = vld1q(__draw_pattern_src_incr_rgb32 + iOffset);
    uint32x4_t      offSHi = vld1q(__draw_pattern_src_incr_rgb32 + iOffset + 4);
    /* deduces bitmask vector with wrap from iOffset */
    /* hold  8 contiguous values into 2 32-bit vector pair */
    uint32x4_t      vBitMaskLo = vld1q(__draw_pattern_src_bitmask_rgb32 + iOffset);
    uint32x4_t      vBitMaskHi = vld1q(__draw_pattern_src_bitmask_rgb32 + iOffset + 4);

#define SEL_NO_BG_COMP(p)               vpselq(~vTarg, vTarg, p);

    GENERIC_RGB32_DRAW_PATTERN(SEL_NO_BG_COMP);

}





__OVERRIDE_WEAK
void __arm_2d_impl_rgb32_draw_pattern_bg_only(uint8_t *__RESTRICT pchSourceBase,
                                         int32_t  iOffset,
                                         int16_t iSourceStride,
                                         uint32_t *__RESTRICT pTargetBase,
                                         int16_t iTargetStride,
                                         arm_2d_size_t *__RESTRICT ptCopySize,
                                         uint32_t hwBackColour)
{
    //! get in byte offset
    iOffset &= 0x07;
    iSourceStride = (iSourceStride + 7) & ~0x07;

    /* deduces offset vector from iOffset for gather loading */
    /* hold  8 contiguous values into 2 32-bit vector pair */
    uint32x4_t      offSLo = vld1q(__draw_pattern_src_incr_rgb32 + iOffset);
    uint32x4_t      offSHi = vld1q(__draw_pattern_src_incr_rgb32 + iOffset + 4);
    /* deduces bitmask vector with wrap from iOffset */
    /* hold  8 contiguous values into 2 32-bit vector pair */
    uint32x4_t      vBitMaskLo = vld1q(__draw_pattern_src_bitmask_rgb32 + iOffset);
    uint32x4_t      vBitMaskHi = vld1q(__draw_pattern_src_bitmask_rgb32 + iOffset + 4);

#define SEL_BG_ONLY(p)          vdupq_m_n_u32(vTarg, hwBackColour, p);

    GENERIC_RGB32_DRAW_PATTERN(SEL_BG_ONLY);

}




__OVERRIDE_WEAK
void __arm_2d_impl_rgb32_draw_pattern_bg_fg(uint8_t *__RESTRICT pchSourceBase,
                                         int32_t  iOffset,
                                         int16_t iSourceStride,
                                         uint32_t *__RESTRICT pTargetBase,
                                         int16_t iTargetStride,
                                         arm_2d_size_t *__RESTRICT ptCopySize,
                                         uint32_t hwForeColour,
                                         uint32_t hwBackColour)
{
    //! get in byte offset
    iOffset &= 0x07;
    iSourceStride = (iSourceStride + 7) & ~0x07;

    /* deduces offset vector from iOffset for gather loading */
    /* hold  8 contiguous values into 2 32-bit vector pair */
    uint32x4_t      offSLo = vld1q(__draw_pattern_src_incr_rgb32 + iOffset);
    uint32x4_t      offSHi = vld1q(__draw_pattern_src_incr_rgb32 + iOffset + 4);
    /* deduces bitmask vector with wrap from iOffset */
    /* hold  8 contiguous values into 2 32-bit vector pair */
    uint32x4_t      vBitMaskLo = vld1q(__draw_pattern_src_bitmask_rgb32 + iOffset);
    uint32x4_t      vBitMaskHi = vld1q(__draw_pattern_src_bitmask_rgb32 + iOffset + 4);
    uint32x4_t      vFgColor = vdupq_n_u32(hwForeColour);
    uint32x4_t      vBgColor = vdupq_n_u32(hwBackColour);

#define SEL_BG_FG(p)            vpselq(vFgColor, vBgColor, p);

    GENERIC_RGB32_DRAW_PATTERN(SEL_BG_FG);

}




__OVERRIDE_WEAK
void __arm_2d_impl_rgb32_draw_pattern_bg_comp(uint8_t *__RESTRICT pchSourceBase,
                                         int32_t  iOffset,
                                         int16_t iSourceStride,
                                         uint32_t *__RESTRICT pTargetBase,
                                         int16_t iTargetStride,
                                         arm_2d_size_t *__RESTRICT ptCopySize,
                                         uint32_t hwBackColour)
{
    //! get in byte offset
    iOffset &= 0x07;
    iSourceStride = (iSourceStride + 7) & ~0x07;

    /* deduces offset vector from iOffset for gather loading */
    /* hold  8 contiguous values into 2 32-bit vector pair */
    uint32x4_t      offSLo = vld1q(__draw_pattern_src_incr_rgb32 + iOffset);
    uint32x4_t      offSHi = vld1q(__draw_pattern_src_incr_rgb32 + iOffset + 4);
    /* deduces bitmask vector with wrap from iOffset */
    /* hold  8 contiguous values into 2 32-bit vector pair */
    uint32x4_t      vBitMaskLo = vld1q(__draw_pattern_src_bitmask_rgb32 + iOffset);
    uint32x4_t      vBitMaskHi = vld1q(__draw_pattern_src_bitmask_rgb32 + iOffset + 4);
    uint32x4_t      vBgColor = vdupq_n_u32(hwBackColour);

#define SEL_BG_COMP(p)          vpselq(vmvnq(vTarg), vBgColor, p);

    GENERIC_RGB32_DRAW_PATTERN(SEL_BG_COMP);

}

#endif


#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#undef __API_COLOUR
#undef __API_ELT_SZ
