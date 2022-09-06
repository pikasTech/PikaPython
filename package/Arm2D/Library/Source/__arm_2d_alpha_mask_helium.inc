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
 * Title:        __arm_2d_alpha_mask_helium.inc
 * Description:  c code template for copy and fill like operations
 *
 * $Date:        12. July 2022
 * $Revision:    v1.0.1
 *
 * -------------------------------------------------------------------- */

#ifndef __API_CAFWM_COLOUR
#   error You have to define __API_CAFWM_COLOUR before using this c template
#endif

#define PIXTYP_IS_gray8         1
#define PIXTYP_IS_rgb565        2
#define PIXTYP_IS_cccn888       3

#define CHECKPIXTYP(NAME1, NAME2) CHECKPIXTYP_impl(NAME1, NAME2)
#define CHECKPIXTYP_impl(NAME1, NAME2) PIXTYP_IS_ ## NAME1 == PIXTYP_IS_ ## NAME2

#define CHECKPIXTYP_NOT(NAME1, NAME2) CHECKPIXTYP_NOT_impl(NAME1, NAME2)
#define CHECKPIXTYP_NOT_impl(NAME1, NAME2) PIXTYP_IS_ ## NAME1 != PIXTYP_IS_ ## NAME2


#if  CHECKPIXTYP(__API_CAFWM_COLOUR, rgb565)
#define __API_CAFWM_INT_TYPE                    uint16_t
#define __API_CAFWM_PIXEL_BIT_NUM        16
#define __API_CAFWM_PIXEL_VECLOAD        vld1q
#define __API_CAFWM_PIXEL_GATHVECLOAD    vldrhq_gather_shifted_offset
#define __API_CAFWM_PIXEL_PVECSTORE      vst1q_p
#define __API_CAFWM_PIXEL_BLENDING       __arm_2d_rgb565_blending_opacity_single_vec

#elif CHECKPIXTYP(__API_CAFWM_COLOUR, gray8)
#define __API_CAFWM_INT_TYPE                    uint8_t
#define __API_CAFWM_PIXEL_BIT_NUM        16 /* widening involved */
#define __API_CAFWM_PIXEL_VECLOAD        vldrbq_u16
#define __API_CAFWM_PIXEL_GATHVECLOAD    vldrbq_gather_offset_u16
#define __API_CAFWM_PIXEL_PVECSTORE      vstrbq_p_u16
#define __API_CAFWM_PIXEL_BLENDING       __arm_2d_gray8_blending_opacity_single_vec

#elif CHECKPIXTYP(__API_CAFWM_COLOUR, cccn888)
#define __API_CAFWM_INT_TYPE                    uint32_t
#if !ENABLE_ALT_MASK_FILL
#define __API_CAFWM_PIXEL_BIT_NUM        32
#define __API_CAFWM_PIXEL_VECLOAD        vld1q
#define __API_CAFWM_PIXEL_GATHVECLOAD    vldrwq_gather_shifted_offset
#define __API_CAFWM_PIXEL_PVECSTORE      vst1q_p
#define __API_CAFWM_PIXEL_BLENDING       __arm_2d_cccn888_blending_opacity_single_vec
#else
#define __API_CAFWM_PIXEL_BIT_NUM        16 /* widening involved */
#define __API_CAFWM_PIXEL_VECLOAD        vldrbq_u16
#define __API_CAFWM_PIXEL_GATHVECLOAD    vldrbq_gather_offset_u16
#define __API_CAFWM_PIXEL_PVECSTORE      vstrbq_p_u16
#define __API_CAFWM_PIXEL_BLENDING       __arm_2d_gray8_blending_opacity_single_vec
#endif

#else
#error Unknown colour
#endif

#define __API_CAFWM_PIXEL_VECTYP         ARM_PIX_VECTYP(__API_CAFWM_PIXEL_BIT_NUM)
#define __API_CAFWM_VEC_INCR             ARM_PIX_VECELT(__API_CAFWM_PIXEL_BIT_NUM)

/*! disable this feature by default */
#ifndef __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
#   define __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING      0
#endif

#ifndef __API_CAFWM_CFG_1_HORIZONTAL_LINE
#   define __API_CAFWM_CFG_1_HORIZONTAL_LINE            0
#endif

#ifndef __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
#   define __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT        0
#endif


#if __API_CAFWM_CFG_1_HORIZONTAL_LINE && !__API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
//! rename functions for 'c8bit' 1 horizontal line target mask
#   define masks_fill                  src_msk_1h_des_msk_fill
#   define masks_fill_x_mirror         src_msk_1h_des_msk_fill_x_mirror
#   define masks_fill_y_mirror         src_msk_1h_des_msk_fill_y_mirror
#   define masks_fill_xy_mirror        src_msk_1h_des_msk_fill_xy_mirror
#   define masks_fill_mirror           src_msk_1h_des_msk_fill_mirror

#   define des_msk_fill                1h_des_msk_fill
#   define des_msk_fill_x_mirror       1h_des_msk_fill_x_mirror
#   define des_msk_fill_y_mirror       1h_des_msk_fill_y_mirror
#   define des_msk_fill_xy_mirror      1h_des_msk_fill_xy_mirror
#   define des_msk_fill_mirror         1h_des_msk_fill_mirror


#   define masks_copy                  src_msk_1h_des_msk_copy
#   define masks_copy_x_mirror         src_msk_1h_des_msk_copy_x_mirror
#   define masks_copy_y_mirror         src_msk_1h_des_msk_copy_y_mirror
#   define masks_copy_xy_mirror        src_msk_1h_des_msk_copy_xy_mirror
#   define masks_copy_mirror           src_msk_1h_des_msk_copy_mirror

#   define des_msk_copy                1h_des_msk_copy
#   define des_msk_copy_x_mirror       1h_des_msk_copy_x_mirror
#   define des_msk_copy_y_mirror       1h_des_msk_copy_y_mirror
#   define des_msk_copy_xy_mirror      1h_des_msk_copy_xy_mirror
#   define des_msk_copy_mirror         1h_des_msk_copy_mirror

#elif __API_CAFWM_CFG_1_HORIZONTAL_LINE && __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
//! rename functions for '8in32 channel' 1 horizontal line target mask

#   error Do NOT Support this combination:  __API_CAFWM_CFG_1_HORIZONTAL_LINE=1 \
 and __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT=1 !

#elif !__API_CAFWM_CFG_1_HORIZONTAL_LINE && __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
//! rename functions for '8in32 channel'

#   define des_msk_fill                des_chn_msk_fill
#   define des_msk_fill_x_mirror       des_chn_msk_fill_x_mirror
#   define des_msk_fill_y_mirror       des_chn_msk_fill_y_mirror
#   define des_msk_fill_xy_mirror      des_chn_msk_fill_xy_mirror
#   define des_msk_fill_mirror         des_chn_msk_fill_mirror

#   define des_msk_copy                des_chn_msk_copy
#   define des_msk_copy_x_mirror       des_chn_msk_copy_x_mirror
#   define des_msk_copy_y_mirror       des_chn_msk_copy_y_mirror
#   define des_msk_copy_xy_mirror      des_chn_msk_copy_xy_mirror
#   define des_msk_copy_mirror         des_chn_msk_copy_mirror

#   define src_msk_fill                src_chn_msk_fill
#   define src_msk_fill_x_mirror       src_chn_msk_fill_x_mirror
#   define src_msk_fill_y_mirror       src_chn_msk_fill_y_mirror
#   define src_msk_fill_xy_mirror      src_chn_msk_fill_xy_mirror
#   define src_msk_fill_mirror         src_chn_msk_fill_mirror

#   define src_msk_copy                src_chn_msk_copy
#   define src_msk_copy_x_mirror       src_chn_msk_copy_x_mirror
#   define src_msk_copy_y_mirror       src_chn_msk_copy_y_mirror
#   define src_msk_copy_xy_mirror      src_chn_msk_copy_xy_mirror
#   define src_msk_copy_mirror         src_chn_msk_copy_mirror

#endif



#undef ____CAFWM_FUNC
#undef ___CAFWM_FUNC
#undef __CAFWM_FUNC



#ifndef __API_CAFWM_OP_NAME
#   define ____CAFWM_FUNC(__NAME, __COLOUR)                                      \
        __MVE_WRAPPER(__arm_2d_impl_##__COLOUR##_##__NAME)
#   define ___CAFWM_FUNC(__NAME, __COLOUR)   ____CAFWM_FUNC(__NAME, __COLOUR)
#else
#   define _____CAFWM_FUNC(__OP_NAME, __NAME, __COLOUR)                          \
        __MVE_WRAPPER(__arm_2d_impl_##__COLOUR##_##__OP_NAME##_##__NAME)
#   define ____CAFWM_FUNC(__OP_NAME, __NAME, __COLOUR)                           \
        _____CAFWM_FUNC(__OP_NAME, __NAME, __COLOUR)
#   define ___CAFWM_FUNC(__NAME, __COLOUR)                                       \
        ____CAFWM_FUNC(__API_CAFWM_OP_NAME, __NAME, __COLOUR)
#endif

#define __CAFWM_FUNC(__NAME)   ___CAFWM_FUNC(__NAME, __API_CAFWM_COLOUR)


#undef ____CAFWM_TYPE
#undef ___CAFWM_TYPE
#undef __CAFWM_TYPE

#ifndef __API_CAFWM_OP_NAME
#   define ____CAFWM_TYPE(__NAME, __COLOUR)  arm_2d_##__COLOUR##_##__NAME
#   define ___CAFWM_TYPE(__NAME, __COLOUR)   ____CAFWM_TYPE(__NAME, __COLOUR)
#else
#   define _____CAFWM_TYPE(__OP_NAME, __NAME, __COLOUR)                        \
        arm_2d_##__COLOUR##_##__OP_NAME##_##__NAME
#   define ____CAFWM_TYPE(__OP_NAME, __NAME, __COLOUR)                         \
        _____CAFWM_TYPE(__OP_NAME, __NAME, __COLOUR)
#   define ___CAFWM_TYPE(__NAME, __COLOUR)                                     \
        ____CAFWM_TYPE(__API_CAFWM_OP_NAME, __NAME, __COLOUR)
#endif


#define __CAFWM_TYPE(__NAME)   ___CAFWM_TYPE(__NAME, __API_CAFWM_COLOUR)

/*----------------------------------------------------------------------------*
 * Fill with Mirroring (both masks)                                           *
 *----------------------------------------------------------------------------*/



#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT

/*! \note source mask only */
#   define __API_MCWM_COLOUR                   __API_CAFWM_COLOUR
#   define __API_MCWM_INT_TYPE                 __API_CAFWM_INT_TYPE

#   if defined(__API_CAFWM_OP_NAME)
#       define __API_MCWM_OP_NAME              __API_CAFWM_OP_NAME
#   endif

#   define __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING                               \
                __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
#   define __API_MCWM_CFG_1_HORIZONTAL_LINE                         0
#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT                     1

#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE      1
#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE      0

#   define masks_fill                  src_chn_msk_des_msk_fill
#   define masks_fill_x_mirror         src_chn_msk_des_msk_fill_x_mirror
#   define masks_fill_y_mirror         src_chn_msk_des_msk_fill_y_mirror
#   define masks_fill_xy_mirror        src_chn_msk_des_msk_fill_xy_mirror
#   define masks_fill_mirror           src_chn_msk_des_msk_fill_mirror

#   define masks_copy                  src_chn_msk_des_msk_copy
#   define masks_copy_x_mirror         src_chn_msk_des_msk_copy_x_mirror
#   define masks_copy_y_mirror         src_chn_msk_des_msk_copy_y_mirror
#   define masks_copy_xy_mirror        src_chn_msk_des_msk_copy_xy_mirror
#   define masks_copy_mirror           src_chn_msk_des_msk_copy_mirror


#   include "__arm_2d_meta_copy_with_masks_helium.inc"



/*! \note source mask only */
#   define __API_MCWM_COLOUR                   __API_CAFWM_COLOUR
#   define __API_MCWM_INT_TYPE                 __API_CAFWM_INT_TYPE

#   if defined(__API_CAFWM_OP_NAME)
#       define __API_MCWM_OP_NAME              __API_CAFWM_OP_NAME
#   endif

#   define __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING                               \
                __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING

#   define __API_MCWM_CFG_1_HORIZONTAL_LINE                         1
#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT                     1

#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE      1
#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE      0

#   define masks_fill                  src_chn_msk_1h_des_msk_fill
#   define masks_fill_x_mirror         src_chn_msk_1h_des_msk_fill_x_mirror
#   define masks_fill_y_mirror         src_chn_msk_1h_des_msk_fill_y_mirror
#   define masks_fill_xy_mirror        src_chn_msk_1h_des_msk_fill_xy_mirror
#   define masks_fill_mirror           src_chn_msk_1h_des_msk_fill_mirror

#   define masks_copy                  src_chn_msk_1h_des_msk_copy
#   define masks_copy_x_mirror         src_chn_msk_1h_des_msk_copy_x_mirror
#   define masks_copy_y_mirror         src_chn_msk_1h_des_msk_copy_y_mirror
#   define masks_copy_xy_mirror        src_chn_msk_1h_des_msk_copy_xy_mirror
#   define masks_copy_mirror           src_chn_msk_1h_des_msk_copy_mirror


#   include "__arm_2d_meta_copy_with_masks_helium.inc"


/*! \note des mask only */
#   define __API_MCWM_COLOUR                   __API_CAFWM_COLOUR
#   define __API_MCWM_INT_TYPE                 __API_CAFWM_INT_TYPE

#   if defined(__API_CAFWM_OP_NAME)
#       define __API_MCWM_OP_NAME              __API_CAFWM_OP_NAME
#   endif

#   define __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING                               \
                __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
#   define __API_MCWM_CFG_1_HORIZONTAL_LINE                         0
#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT                     1

#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE      0
#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE      1

#   define masks_fill                  src_msk_des_chn_msk_fill
#   define masks_fill_x_mirror         src_msk_des_chn_msk_fill_x_mirror
#   define masks_fill_y_mirror         src_msk_des_chn_msk_fill_y_mirror
#   define masks_fill_xy_mirror        src_msk_des_chn_msk_fill_xy_mirror
#   define masks_fill_mirror           src_msk_des_chn_msk_fill_mirror

#   define masks_copy                  src_msk_des_chn_msk_copy
#   define masks_copy_x_mirror         src_msk_des_chn_msk_copy_x_mirror
#   define masks_copy_y_mirror         src_msk_des_chn_msk_copy_y_mirror
#   define masks_copy_xy_mirror        src_msk_des_chn_msk_copy_xy_mirror
#   define masks_copy_mirror           src_msk_des_chn_msk_copy_mirror


#   include "__arm_2d_meta_copy_with_masks_helium.inc"


/*! \note both masks */
#   define __API_MCWM_COLOUR                   __API_CAFWM_COLOUR
#   define __API_MCWM_INT_TYPE                 __API_CAFWM_INT_TYPE

#   if defined(__API_CAFWM_OP_NAME)
#       define __API_MCWM_OP_NAME              __API_CAFWM_OP_NAME
#   endif

#   define __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING                               \
                __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
#   define __API_MCWM_CFG_1_HORIZONTAL_LINE                         0
#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT                     1

#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE      1
#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE      1

#   define masks_fill                  src_chn_msk_des_chn_msk_fill
#   define masks_fill_x_mirror         src_chn_msk_des_chn_msk_fill_x_mirror
#   define masks_fill_y_mirror         src_chn_msk_des_chn_msk_fill_y_mirror
#   define masks_fill_xy_mirror        src_chn_msk_des_chn_msk_fill_xy_mirror
#   define masks_fill_mirror           src_chn_msk_des_chn_msk_fill_mirror

#   define masks_copy                  src_chn_msk_des_chn_msk_copy
#   define masks_copy_x_mirror         src_chn_msk_des_chn_msk_copy_x_mirror
#   define masks_copy_y_mirror         src_chn_msk_des_chn_msk_copy_y_mirror
#   define masks_copy_xy_mirror        src_chn_msk_des_chn_msk_copy_xy_mirror
#   define masks_copy_mirror           src_chn_msk_des_chn_msk_copy_mirror


#   include "__arm_2d_meta_copy_with_masks_helium.inc"

#else

#   define __API_MCWM_COLOUR                   __API_CAFWM_COLOUR
#   define __API_MCWM_INT_TYPE                 __API_CAFWM_INT_TYPE

#   if defined(__API_CAFWM_OP_NAME)
#       define __API_MCWM_OP_NAME              __API_CAFWM_OP_NAME
#   endif

#   define __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING                               \
                __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
#   define __API_MCWM_CFG_1_HORIZONTAL_LINE                                     \
                __API_CAFWM_CFG_1_HORIZONTAL_LINE
#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT                     0

#   include "__arm_2d_meta_copy_with_masks_helium.inc"

#endif




/*----------------------------------------------------------------------------*
 * Fill with Mirroring (target mask only)                                     *
 *----------------------------------------------------------------------------*/

__OVERRIDE_WEAK
void __CAFWM_FUNC(des_msk_fill)(
                        __API_CAFWM_INT_TYPE * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                        __API_CAFWM_INT_TYPE *__RESTRICT ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,

                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                        uint32_t *__RESTRICT ptTargetMaskBase,
                    #else
                        uint8_t *__RESTRICT ptTargetMaskBase,
                    #endif
                        int16_t iTargetMaskStride,
                        arm_2d_size_t *__RESTRICT ptTargetMaskSize)
{
    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);

#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#else
    uint8_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#endif

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        __API_CAFWM_INT_TYPE *__RESTRICT ptSource = ptSourceBase;

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            __API_CAFWM_INT_TYPE *__RESTRICT ptTarget = ptTargetBase;
        #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            uint32_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #else
            uint8_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #endif

            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = ptSource;
                __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = ptTarget;

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t        curIncStride4Idx = 0;
                __API_CAFWM_PIXEL_VECTYP      vIncStride4Offs =
                    ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #else
                uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #endif
                int32_t         blkCnt = wLength;

                do {
                    __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
                    __API_CAFWM_PIXEL_VECTYP      vecSource = __API_CAFWM_PIXEL_VECLOAD(ptSrc);

                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    __API_CAFWM_PIXEL_VECTYP      vecTargetMask =
                                    ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)(
                                            (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
                    #else
                    __API_CAFWM_PIXEL_VECTYP      vecTargetMask = ARM_CONNECT2(vldrbq_u,
                                                            __API_CAFWM_PIXEL_BIT_NUM)(ptTargetMaskCur);
                    #endif

                    __API_CAFWM_PIXEL_VECTYP      vecHwOpacity = vsubq(v256, vecTargetMask);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                    vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                        ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

                    vecTarget = __API_CAFWM_PIXEL_BLENDING(
                                                    vecTarget, vecSource, vecHwOpacity);
                    /* tail predication */
                    __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget,
                                            ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
                    #endif


                    #if !__API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    ptTargetMaskCur += __API_CAFWM_VEC_INCR;
                    #endif

                    ptTargetCur += __API_CAFWM_VEC_INCR;
                    ptSrc += __API_CAFWM_VEC_INCR;

                    blkCnt -= __API_CAFWM_VEC_INCR;
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


        #if __API_CAFWM_CFG_1_HORIZONTAL_LINE
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
void __CAFWM_FUNC(des_msk_fill_x_mirror)(
                        __API_CAFWM_INT_TYPE * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                        __API_CAFWM_INT_TYPE *__RESTRICT ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,

                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                        uint32_t *__RESTRICT ptTargetMaskBase,
                    #else
                        uint8_t *__RESTRICT ptTargetMaskBase,
                    #endif
                        int16_t iTargetMaskStride,
                        arm_2d_size_t *__RESTRICT ptTargetMaskSize)
{
    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);
    uint16_t        srcWidth = ptSourceSize->iWidth;
#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#else
    uint8_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#endif

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        __API_CAFWM_INT_TYPE *__RESTRICT ptSource = ptSourceBase;

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            __API_CAFWM_INT_TYPE *__RESTRICT ptTarget = ptTargetBase;

        #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            uint32_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #else
            uint8_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #endif

            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = ptSource;
                __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = ptTarget;

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t        curIncStride4Idx = 0;
                __API_CAFWM_PIXEL_VECTYP      vIncStride4Offs =
                                ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #else
                uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #endif

                uint32_t        curDecrStride1Idx = srcWidth - 1;
                int32_t         blkCnt = wLength;
                __API_CAFWM_PIXEL_VECTYP      vDecrStride1Offs =
                            ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);

                do {
                    __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
                    __API_CAFWM_PIXEL_VECTYP      vecSource =
                        __API_CAFWM_PIXEL_GATHVECLOAD(ptSrc, vDecrStride1Offs);
            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    __API_CAFWM_PIXEL_VECTYP      vecTargetMask =
                                ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)(
                                            (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
            #else
                    __API_CAFWM_PIXEL_VECTYP      vecTargetMask =
                                ARM_CONNECT2(vldrbq_u, __API_CAFWM_PIXEL_BIT_NUM)((uint8_t *)ptTargetMaskCur);
            #endif
                    __API_CAFWM_PIXEL_VECTYP      vecHwOpacity = vsubq(v256, vecTargetMask);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                    vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                        ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

                    vecTarget =
                        __API_CAFWM_PIXEL_BLENDING(vecTarget, vecSource,
                                                                    vecHwOpacity);

                    /* tail predication */
                    __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget,
                                        ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

                    vDecrStride1Offs = ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #else
                    ptTargetMaskCur += __API_CAFWM_VEC_INCR;
            #endif
                    ptTargetCur += __API_CAFWM_VEC_INCR;

                    blkCnt -= __API_CAFWM_VEC_INCR;
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


        #if __API_CAFWM_CFG_1_HORIZONTAL_LINE
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
void __CAFWM_FUNC(des_msk_fill_y_mirror)(
                        __API_CAFWM_INT_TYPE * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                        __API_CAFWM_INT_TYPE *__RESTRICT ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,

                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                        uint32_t *__RESTRICT ptTargetMaskBase,
                    #else
                        uint8_t *__RESTRICT ptTargetMaskBase,
                    #endif
                        int16_t iTargetMaskStride,
                        arm_2d_size_t *__RESTRICT ptTargetMaskSize)
{
    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);


#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#else
    uint8_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#endif

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        __API_CAFWM_INT_TYPE *__RESTRICT ptSource
            = ptSourceBase + iSourceStride * (ptSourceSize->iHeight - 1);

    #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
        int_fast16_t iSourceMaskY = 0;
    #endif

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            __API_CAFWM_INT_TYPE *__RESTRICT ptTarget = ptTargetBase;
        #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            uint32_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #else
            uint8_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #endif

            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = ptSource;
                __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = ptTarget;

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t        curIncStride4Idx = 0;
                __API_CAFWM_PIXEL_VECTYP      vIncStride4Offs =
                        ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #else
                uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #endif
                int32_t         blkCnt = wLength;

                do {
                    __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
                    __API_CAFWM_PIXEL_VECTYP      vecSource = __API_CAFWM_PIXEL_VECLOAD(ptSrc);

                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    __API_CAFWM_PIXEL_VECTYP      vecTargetMask =
                                            ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)(
                                                            (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
                    #else
                    __API_CAFWM_PIXEL_VECTYP      vecTargetMask = ARM_CONNECT2(vldrbq_u, __API_CAFWM_PIXEL_BIT_NUM)(ptTargetMaskCur);
                    #endif

                    __API_CAFWM_PIXEL_VECTYP      vecHwOpacity = vsubq(v256, vecTargetMask);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                    vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                        ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

                    vecTarget = __API_CAFWM_PIXEL_BLENDING(
                                                    vecTarget, vecSource, vecHwOpacity);
                    /* tail predication */
                    __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget, ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
                    #endif

                    #if !__API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    ptTargetMaskCur += __API_CAFWM_VEC_INCR;
                    #endif

                    ptTargetCur += __API_CAFWM_VEC_INCR;
                    ptSrc += __API_CAFWM_VEC_INCR;

                    blkCnt -= __API_CAFWM_VEC_INCR;
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


        #if __API_CAFWM_CFG_1_HORIZONTAL_LINE
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
void __CAFWM_FUNC(des_msk_fill_xy_mirror)(
                        __API_CAFWM_INT_TYPE * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                        __API_CAFWM_INT_TYPE *__RESTRICT ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,

                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                        uint32_t *__RESTRICT ptTargetMaskBase,
                    #else
                        uint8_t *__RESTRICT ptTargetMaskBase,
                    #endif
                        int16_t iTargetMaskStride,
                        arm_2d_size_t *__RESTRICT ptTargetMaskSize)
{
    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);
    __API_CAFWM_INT_TYPE        srcWidth = ptSourceSize->iWidth;
#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#else
    uint8_t *__RESTRICT ptTargetMaskLineBase = ptTargetMaskBase;
#endif

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        __API_CAFWM_INT_TYPE *__RESTRICT ptSource
            = ptSourceBase + iSourceStride * (ptSourceSize->iHeight - 1);

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            __API_CAFWM_INT_TYPE *__RESTRICT ptTarget = ptTargetBase;

        #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            uint32_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #else
            uint8_t *__RESTRICT ptTargetMask = ptTargetMaskLineBase;
        #endif

            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = ptSource;
                __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = ptTarget;

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t        curIncStride4Idx = 0;
                __API_CAFWM_PIXEL_VECTYP      vIncStride4Offs =
                                    ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #else
                uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
            #endif

                uint32_t        curDecrStride1Idx = srcWidth - 1;
                int32_t         blkCnt = wLength;
                __API_CAFWM_PIXEL_VECTYP      vDecrStride1Offs =
                            ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


                do {
                    __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
                    __API_CAFWM_PIXEL_VECTYP      vecSource =
                        __API_CAFWM_PIXEL_GATHVECLOAD(ptSrc, vDecrStride1Offs);
            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    __API_CAFWM_PIXEL_VECTYP      vecTargetMask =
                            ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)(
                                            (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
            #else
                    __API_CAFWM_PIXEL_VECTYP      vecTargetMask =
                            ARM_CONNECT2(vldrbq_u, __API_CAFWM_PIXEL_BIT_NUM)((uint8_t *)ptTargetMaskCur);
            #endif
                    __API_CAFWM_PIXEL_VECTYP      vecHwOpacity = vsubq(v256, vecTargetMask);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                    vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                        ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

                    vecTarget =
                        __API_CAFWM_PIXEL_BLENDING(vecTarget, vecSource,
                                                                    vecHwOpacity);

                    /* tail predication */
                    __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget, ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

                    vDecrStride1Offs = ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #else
                    ptTargetMaskCur += __API_CAFWM_VEC_INCR;
            #endif
                    ptTargetCur += __API_CAFWM_VEC_INCR;

                    blkCnt -= __API_CAFWM_VEC_INCR;
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

        #if __API_CAFWM_CFG_1_HORIZONTAL_LINE
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
 * Fill with Mirroring (src mask only)                                        *
 *----------------------------------------------------------------------------*/
#if !__API_CAFWM_CFG_1_HORIZONTAL_LINE
__OVERRIDE_WEAK
void __CAFWM_FUNC(src_msk_fill)(
                        __API_CAFWM_INT_TYPE * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                        uint32_t * __RESTRICT ptSourceMaskBase,
                    #else
                        uint8_t * __RESTRICT ptSourceMaskBase,
                    #endif
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                        __API_CAFWM_INT_TYPE *__RESTRICT ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize)
{
    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        __API_CAFWM_INT_TYPE *__RESTRICT ptSource = ptSourceBase;
    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t *ptSourceMask = ptSourceMaskBase;
    #else
        uint8_t *ptSourceMask = ptSourceMaskBase;
    #endif

    #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
        int_fast16_t iSourceMaskY = 0;
    #endif

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            __API_CAFWM_INT_TYPE *__RESTRICT ptTarget = ptTargetBase;

            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = ptSource;
                __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = ptTarget;

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t        curIncStride4Idx = 0;
                __API_CAFWM_PIXEL_VECTYP      vIncStride4Offs =
                    ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
            #else
                uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
            #endif
                int32_t         blkCnt = wLength;

                do {
                    __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
                    __API_CAFWM_PIXEL_VECTYP      vecSource = __API_CAFWM_PIXEL_VECLOAD(ptSrc);
                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    __API_CAFWM_PIXEL_VECTYP      vecSrcMsk =
                                    ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)(
                                                    (uint8_t const *)ptSrcMsk, vIncStride4Offs);
                    #else
                    __API_CAFWM_PIXEL_VECTYP      vecSrcMsk = ARM_CONNECT2(vldrbq_u,
                                                            __API_CAFWM_PIXEL_BIT_NUM)(ptSrcMsk);
                    #endif


                    __API_CAFWM_PIXEL_VECTYP      vecHwOpacity = vsubq(v256, vecSrcMsk);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                    vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                        ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

                    vecTarget = __API_CAFWM_PIXEL_BLENDING(
                                                    vecTarget, vecSource, vecHwOpacity);
                    /* tail predication */
                    __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget,
                                            ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
                    #endif

                    #if !__API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    ptSrcMsk += __API_CAFWM_VEC_INCR;
                    #endif


                    ptTargetCur += __API_CAFWM_VEC_INCR;
                    ptSrc += __API_CAFWM_VEC_INCR;

                    blkCnt -= __API_CAFWM_VEC_INCR;
                }
                while (blkCnt > 0);

                ptTarget += wLength;

                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);

            /*---------------- Height Loop End----------------*/
            ptSource += iSourceStride;
            ptTargetBase += iTargetStride;

        #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
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


            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}




__OVERRIDE_WEAK
void __CAFWM_FUNC(src_msk_fill_x_mirror)(
                        __API_CAFWM_INT_TYPE * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                        uint32_t * __RESTRICT ptSourceMaskBase,
                    #else
                        uint8_t * __RESTRICT ptSourceMaskBase,
                    #endif
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                        __API_CAFWM_INT_TYPE *__RESTRICT ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize)
{
    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);
    uint16_t        srcWidth = ptSourceSize->iWidth;

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        __API_CAFWM_INT_TYPE *__RESTRICT ptSource = ptSourceBase;
    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t *ptSourceMask = ptSourceMaskBase;
    #else
        uint8_t *ptSourceMask = ptSourceMaskBase;
    #endif

    #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
        int_fast16_t iSourceMaskY = 0;
    #endif

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            __API_CAFWM_INT_TYPE *__RESTRICT ptTarget = ptTargetBase;

            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = ptSource;
                __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = ptTarget;


            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
            #else
                uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
            #endif

                uint32_t        curDecrStride1Idx = srcWidth - 1;
                int32_t         blkCnt = wLength;
            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t        curDecrStride4Idx = 4*(srcWidth - 1);
                __API_CAFWM_PIXEL_VECTYP      vDecrStride4Offs =
                            ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
            #endif
                __API_CAFWM_PIXEL_VECTYP      vDecrStride1Offs =
                            ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


                do {
                    __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
                    __API_CAFWM_PIXEL_VECTYP      vecSource =
                        __API_CAFWM_PIXEL_GATHVECLOAD(ptSrc, vDecrStride1Offs);
            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    __API_CAFWM_PIXEL_VECTYP      vecSrcMsk =
                        ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)
                                        ((uint8_t *)ptSrcMsk, vDecrStride4Offs);
            #else
                    __API_CAFWM_PIXEL_VECTYP      vecSrcMsk =
                        ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)
                                        (ptSrcMsk, vDecrStride1Offs);
            #endif
                    __API_CAFWM_PIXEL_VECTYP      vecHwOpacity = vsubq(v256, vecSrcMsk);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                    vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                        ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

                    vecTarget =
                        __API_CAFWM_PIXEL_BLENDING(vecTarget, vecSource,
                                                                    vecHwOpacity);

                    /* tail predication */
                    __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget,
                                        ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    vDecrStride4Offs = ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
            #endif
                    vDecrStride1Offs = ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


                    ptTargetCur += __API_CAFWM_VEC_INCR;

                    blkCnt -= __API_CAFWM_VEC_INCR;
                }
                while (blkCnt > 0);


                ptTarget += wLength;
                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);

            /*---------------- Height Loop End----------------*/
            ptSource += iSourceStride;
            ptTargetBase += iTargetStride;

        #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
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


            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__OVERRIDE_WEAK
void __CAFWM_FUNC(src_msk_fill_y_mirror)(
                        __API_CAFWM_INT_TYPE * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                        uint32_t * __RESTRICT ptSourceMaskBase,
                    #else
                        uint8_t * __RESTRICT ptSourceMaskBase,
                    #endif
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                        __API_CAFWM_INT_TYPE *__RESTRICT ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize)
{
    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);

    assert(ptSourceSize->iHeight <= ptSourceMaskSize->iHeight);
    ptSourceMaskBase += iSourceMaskStride * (ptSourceSize->iHeight - 1);


    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        __API_CAFWM_INT_TYPE *__RESTRICT ptSource
            = ptSourceBase + iSourceStride * (ptSourceSize->iHeight - 1);

    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t *ptSourceMask = ptSourceMaskBase;
    #else
        uint8_t *ptSourceMask = ptSourceMaskBase;
    #endif

    #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
        int_fast16_t iSourceMaskY = 0;
    #endif

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            __API_CAFWM_INT_TYPE *__RESTRICT ptTarget = ptTargetBase;

            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = ptSource;
                __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = ptTarget;

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t        curIncStride4Idx = 0;
                __API_CAFWM_PIXEL_VECTYP      vIncStride4Offs =
                        ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
            #else
                uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
            #endif
                int32_t         blkCnt = wLength;

                do {
                    __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
                    __API_CAFWM_PIXEL_VECTYP      vecSource = __API_CAFWM_PIXEL_VECLOAD(ptSrc);
                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    __API_CAFWM_PIXEL_VECTYP      vecSrcMsk = ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)(
                                                    (uint8_t const *)ptSrcMsk, vIncStride4Offs);
                    #else
                    __API_CAFWM_PIXEL_VECTYP      vecSrcMsk = ARM_CONNECT2(vldrbq_u, __API_CAFWM_PIXEL_BIT_NUM)(ptSrcMsk);
                    #endif


                    __API_CAFWM_PIXEL_VECTYP      vecHwOpacity = vsubq(v256, vecSrcMsk);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                    vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                        ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

                    vecTarget = __API_CAFWM_PIXEL_BLENDING(
                                                    vecTarget, vecSource, vecHwOpacity);
                    /* tail predication */
                    __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget, ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
                    #endif

                    #if !__API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    ptSrcMsk += __API_CAFWM_VEC_INCR;
                    #endif


                    ptTargetCur += __API_CAFWM_VEC_INCR;
                    ptSrc += __API_CAFWM_VEC_INCR;

                    blkCnt -= __API_CAFWM_VEC_INCR;
                }
                while (blkCnt > 0);

                ptTarget += wLength;

                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);

            /*---------------- Height Loop End----------------*/
            ptSource -= iSourceStride;
            ptTargetBase += iTargetStride;

        #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
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

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__OVERRIDE_WEAK
void __CAFWM_FUNC(src_msk_fill_xy_mirror)(
                        __API_CAFWM_INT_TYPE * __RESTRICT ptSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,

                    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                        uint32_t * __RESTRICT ptSourceMaskBase,
                    #else
                        uint8_t * __RESTRICT ptSourceMaskBase,
                    #endif
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                        __API_CAFWM_INT_TYPE *__RESTRICT ptTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize)
{
    assert(ptSourceSize->iHeight <= ptSourceMaskSize->iHeight);
    ptSourceMaskBase += iSourceMaskStride * (ptSourceSize->iHeight - 1);

    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);
    __API_CAFWM_INT_TYPE        srcWidth = ptSourceSize->iWidth;

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        //! reset source
        __API_CAFWM_INT_TYPE *__RESTRICT ptSource
            = ptSourceBase + iSourceStride * (ptSourceSize->iHeight - 1);

    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t *ptSourceMask = ptSourceMaskBase;
    #else
        uint8_t *ptSourceMask = ptSourceMaskBase;
    #endif

    #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
        int_fast16_t iSourceMaskY = 0;
    #endif

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            __API_CAFWM_INT_TYPE *__RESTRICT ptTarget = ptTargetBase;

            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = ptSource;
                __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = ptTarget;

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
            #else
                uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
            #endif

                uint32_t        curDecrStride1Idx = srcWidth - 1;
                int32_t         blkCnt = wLength;
            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t        curDecrStride4Idx = 4*(srcWidth - 1);
                __API_CAFWM_PIXEL_VECTYP      vDecrStride4Offs =
                            ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
            #endif
                __API_CAFWM_PIXEL_VECTYP      vDecrStride1Offs =
                            ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


                do {
                    __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
                    __API_CAFWM_PIXEL_VECTYP      vecSource =
                        __API_CAFWM_PIXEL_GATHVECLOAD(ptSrc, vDecrStride1Offs);
            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    __API_CAFWM_PIXEL_VECTYP      vecSrcMsk =
                            ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)((uint8_t *)ptSrcMsk, vDecrStride4Offs);

            #else
                    __API_CAFWM_PIXEL_VECTYP      vecSrcMsk =
                            ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)(ptSrcMsk, vDecrStride1Offs);
            #endif
                    __API_CAFWM_PIXEL_VECTYP      vecHwOpacity = vsubq(v256, vecSrcMsk);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
                    vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                        ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

                    vecTarget =
                        __API_CAFWM_PIXEL_BLENDING(vecTarget, vecSource,
                                                                    vecHwOpacity);

                    /* tail predication */
                    __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget, ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    vDecrStride4Offs = ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
            #endif
                    vDecrStride1Offs = ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


                    ptTargetCur += __API_CAFWM_VEC_INCR;

                    blkCnt -= __API_CAFWM_VEC_INCR;
                }
                while (blkCnt > 0);


                ptTarget += wLength;
                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);

            /*---------------- Height Loop End----------------*/
            ptSource -= iSourceStride;
            ptTargetBase += iTargetStride;

        #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
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

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}


#endif






/*----------------------------------------------------------------------------*
 * Copy with Mirroring  (target mask only)                                    *
 *----------------------------------------------------------------------------*/

__OVERRIDE_WEAK
void __CAFWM_FUNC(des_msk_copy)(
                            __API_CAFWM_INT_TYPE * __RESTRICT pSourceBase,
                            int16_t iSourceStride,

                            __API_CAFWM_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,

                        #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                            uint32_t * __RESTRICT ptTargetMaskBase,
                        #else
                            uint8_t * __RESTRICT ptTargetMaskBase,
                        #endif
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptTargetMaskSize,

                            arm_2d_size_t * __RESTRICT ptCopySize)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);

#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *ptTargetMask = ptTargetMaskBase;
#else
    uint8_t *ptTargetMask = ptTargetMaskBase;
#endif


    for (   int_fast16_t y = 0;
            y < iHeight;
            y++) {

    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t        curIncStride4Idx = 0;
        __API_CAFWM_PIXEL_VECTYP      vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
    #endif

        __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = pSourceBase;
        __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = pTargetBase;


    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #else
        uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #endif
        int32_t         blkCnt = iWidth;

        do {
            __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
            __API_CAFWM_PIXEL_VECTYP      vecSource = __API_CAFWM_PIXEL_VECLOAD(ptSrc);

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            __API_CAFWM_PIXEL_VECTYP      vecTargetMask =
                        ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)(
                                    (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
            #else
            __API_CAFWM_PIXEL_VECTYP      vecTargetMask =
                        ARM_CONNECT2(vldrbq_u, __API_CAFWM_PIXEL_BIT_NUM)(ptTargetMaskCur);
            #endif

            __API_CAFWM_PIXEL_VECTYP      vecHwOpacity = vsubq(v256, vecTargetMask);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

            vecTarget = __API_CAFWM_PIXEL_BLENDING(
                                            vecTarget, vecSource, vecHwOpacity);
            /* tail predication */
            __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget,
                        ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif

            #if !__API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            ptTargetMaskCur += __API_CAFWM_VEC_INCR;
            #endif

            ptTargetCur += __API_CAFWM_VEC_INCR;
            ptSrc += __API_CAFWM_VEC_INCR;
            blkCnt -= __API_CAFWM_VEC_INCR;
        }
        while (blkCnt > 0);

        pSourceBase += (iSourceStride);
        pTargetBase += (iTargetStride);


    #if __API_CAFWM_CFG_1_HORIZONTAL_LINE
        ptTargetMask = ptTargetMaskBase;
    #else
        ptTargetMask += (iTargetMaskStride);
    #endif
    }
}





__OVERRIDE_WEAK
void __CAFWM_FUNC(des_msk_copy_x_mirror)(
                                    __API_CAFWM_INT_TYPE * __RESTRICT pSourceBase,
                                    int16_t iSourceStride,

                                    __API_CAFWM_INT_TYPE * __RESTRICT pTargetBase,
                                    int16_t iTargetStride,

                                #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                                    uint32_t * __RESTRICT ptTargetMaskBase,
                                #else
                                    uint8_t * __RESTRICT ptTargetMaskBase,
                                #endif
                                    int16_t iTargetMaskStride,
                                    arm_2d_size_t *__RESTRICT ptTargetMaskSize,

                                    arm_2d_size_t * __RESTRICT ptCopySize)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);

#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *ptTargetMask = ptTargetMaskBase;
#else
    uint8_t *ptTargetMask = ptTargetMaskBase;
#endif

#if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
    int_fast16_t iSourceMaskY = 0;
#endif

    for (   int_fast16_t y = 0;
            y < iHeight;
            y++) {

        __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = pSourceBase;
        __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = pTargetBase;

    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t        curIncStride4Idx = 0;
        __API_CAFWM_PIXEL_VECTYP      vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
    #endif

    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #else
        uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #endif

        uint32_t        curDecrStride1Idx = iWidth - 1;
        int32_t         blkCnt = iWidth;
        __API_CAFWM_PIXEL_VECTYP      vDecrStride1Offs =
                ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


        do {
            __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
            __API_CAFWM_PIXEL_VECTYP      vecSource =
                __API_CAFWM_PIXEL_GATHVECLOAD(ptSrc, vDecrStride1Offs);
    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            __API_CAFWM_PIXEL_VECTYP      vecTargetMask =
                ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)(
                                    (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
    #else
            __API_CAFWM_PIXEL_VECTYP      vecTargetMask =
                ARM_CONNECT2(vldrbq_u, __API_CAFWM_PIXEL_BIT_NUM)((uint8_t *)ptTargetMaskCur);
    #endif
            __API_CAFWM_PIXEL_VECTYP      vecHwOpacity = vsubq(v256, vecTargetMask);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

            vecTarget =
                __API_CAFWM_PIXEL_BLENDING(vecTarget, vecSource,
                                                            vecHwOpacity);

            /* tail predication */
            __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget, ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

            vDecrStride1Offs = ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
    #else
            ptTargetMaskCur += __API_CAFWM_VEC_INCR;
    #endif
            ptTargetCur += __API_CAFWM_VEC_INCR;

            blkCnt -= __API_CAFWM_VEC_INCR;
        }
        while (blkCnt > 0);


        pSourceBase += iSourceStride;
        pTargetBase += iTargetStride;


    #if __API_CAFWM_CFG_1_HORIZONTAL_LINE
        ptTargetMask = ptTargetMaskBase;
    #else
        ptTargetMask += (iTargetMaskStride);
    #endif

    }
}


__OVERRIDE_WEAK
void __CAFWM_FUNC(des_msk_copy_y_mirror)(
                                    __API_CAFWM_INT_TYPE * __RESTRICT pSourceBase,
                                    int16_t iSourceStride,

                                    __API_CAFWM_INT_TYPE * __RESTRICT pTargetBase,
                                    int16_t iTargetStride,

                                #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                                    uint32_t * __RESTRICT ptTargetMaskBase,
                                #else
                                    uint8_t * __RESTRICT ptTargetMaskBase,
                                #endif
                                    int16_t iTargetMaskStride,
                                    arm_2d_size_t *__RESTRICT ptTargetMaskSize,

                                    arm_2d_size_t * __RESTRICT ptCopySize)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);

    pSourceBase += iSourceStride * (ptCopySize->iHeight - 1);


#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *ptTargetMask = ptTargetMaskBase;
#else
    uint8_t *ptTargetMask = ptTargetMaskBase;
#endif

#if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
    int_fast16_t iSourceMaskY = 0;
#endif

    for (   int_fast16_t y = 0;
            y < iHeight;
            y++) {
        __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = pSourceBase;
        __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = pTargetBase;

    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t        curIncStride4Idx = 0;
        __API_CAFWM_PIXEL_VECTYP      vIncStride4Offs =
                ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
    #endif

    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #else
        uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #endif
        int32_t         blkCnt = iWidth;

        do {
            __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
            __API_CAFWM_PIXEL_VECTYP      vecSource = __API_CAFWM_PIXEL_VECLOAD(ptSrc);

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            __API_CAFWM_PIXEL_VECTYP      vecTargetMask =
                    ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)(
                                    (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
            #else
            __API_CAFWM_PIXEL_VECTYP      vecTargetMask =
                    ARM_CONNECT2(vldrbq_u, __API_CAFWM_PIXEL_BIT_NUM)(ptTargetMaskCur);
            #endif

            __API_CAFWM_PIXEL_VECTYP      vecHwOpacity = vsubq(v256, vecTargetMask);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

            vecTarget = __API_CAFWM_PIXEL_BLENDING(
                                            vecTarget, vecSource, vecHwOpacity);
            /* tail predication */
            __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget,
                    ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif


            #if !__API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            ptTargetMaskCur += __API_CAFWM_VEC_INCR;
            #endif

            ptTargetCur += __API_CAFWM_VEC_INCR;
            ptSrc += __API_CAFWM_VEC_INCR;

            blkCnt -= __API_CAFWM_VEC_INCR;
        }
        while (blkCnt > 0);

        pSourceBase -= iSourceStride;
        pTargetBase += (iTargetStride);


    #if __API_CAFWM_CFG_1_HORIZONTAL_LINE
        ptTargetMask = ptTargetMaskBase;
    #else
        ptTargetMask += (iTargetMaskStride);
    #endif
    }

}


__OVERRIDE_WEAK
void __CAFWM_FUNC(des_msk_copy_xy_mirror)(
                                    __API_CAFWM_INT_TYPE * __RESTRICT pSourceBase,
                                    int16_t iSourceStride,

                                    __API_CAFWM_INT_TYPE * __RESTRICT pTargetBase,
                                    int16_t iTargetStride,

                                #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                                    uint32_t * __RESTRICT ptTargetMaskBase,
                                #else
                                    uint8_t * __RESTRICT ptTargetMaskBase,
                                #endif
                                    int16_t iTargetMaskStride,
                                    arm_2d_size_t *__RESTRICT ptTargetMaskSize,

                                    arm_2d_size_t * __RESTRICT ptCopySize)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);

    pSourceBase += iSourceStride * (ptCopySize->iHeight - 1);


#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *ptTargetMask = ptTargetMaskBase;
#else
    uint8_t *ptTargetMask = ptTargetMaskBase;
#endif

#if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
    int_fast16_t iSourceMaskY = 0;
#endif

    for (   int_fast16_t y = 0;
            y < iHeight;
            y++) {
        __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = pSourceBase;
        __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = pTargetBase;

    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t        curIncStride4Idx = 0;
        __API_CAFWM_PIXEL_VECTYP      vIncStride4Offs =
                ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
    #endif

    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t       *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #else
        uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;
    #endif

        uint32_t        curDecrStride1Idx = iWidth - 1;
        int32_t         blkCnt = iWidth;
        __API_CAFWM_PIXEL_VECTYP      vDecrStride1Offs =
                ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


        do {
            __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
            __API_CAFWM_PIXEL_VECTYP      vecSource =
                __API_CAFWM_PIXEL_GATHVECLOAD(ptSrc, vDecrStride1Offs);
    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            __API_CAFWM_PIXEL_VECTYP      vecTargetMask =
                ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)(
                                    (uint8_t const *)ptTargetMaskCur, vIncStride4Offs);
    #else
            __API_CAFWM_PIXEL_VECTYP      vecTargetMask =
                ARM_CONNECT2(vldrbq_u, __API_CAFWM_PIXEL_BIT_NUM)((uint8_t *)ptTargetMaskCur);
    #endif
            __API_CAFWM_PIXEL_VECTYP      vecHwOpacity = vsubq(v256, vecTargetMask);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

            vecTarget =
                __API_CAFWM_PIXEL_BLENDING(vecTarget, vecSource,
                                                            vecHwOpacity);

            /* tail predication */
            __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget, ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

            vDecrStride1Offs = ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
    #else
            ptTargetMaskCur += __API_CAFWM_VEC_INCR;
    #endif
            ptTargetCur += __API_CAFWM_VEC_INCR;

            blkCnt -= __API_CAFWM_VEC_INCR;
        }
        while (blkCnt > 0);

        pSourceBase -= iSourceStride;
        pTargetBase += (iTargetStride);


    #if __API_CAFWM_CFG_1_HORIZONTAL_LINE
        ptTargetMask = ptTargetMaskBase;
    #else
        ptTargetMask += (iTargetMaskStride);
    #endif
    }

}



/*----------------------------------------------------------------------------*
 * Copy with Mirroring  (src mask only)                                       *
 *----------------------------------------------------------------------------*/
#if !__API_CAFWM_CFG_1_HORIZONTAL_LINE
__OVERRIDE_WEAK
void __CAFWM_FUNC(src_msk_copy)(
                            __API_CAFWM_INT_TYPE * __RESTRICT pSourceBase,
                            int16_t iSourceStride,

                        #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                            uint32_t * __RESTRICT ptSourceMaskBase,
                        #else
                            uint8_t * __RESTRICT ptSourceMaskBase,
                        #endif
                            int16_t iSourceMaskStride,
                            arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                            __API_CAFWM_INT_TYPE * __RESTRICT pTargetBase,
                            int16_t iTargetStride,

                            arm_2d_size_t * __RESTRICT ptCopySize)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);

#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *ptSourceMask = ptSourceMaskBase;
#else
    uint8_t *ptSourceMask = ptSourceMaskBase;
#endif

#if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
    int_fast16_t iSourceMaskY = 0;
#endif

    for (   int_fast16_t y = 0;
            y < iHeight;
            y++) {
        __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = pSourceBase;
        __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = pTargetBase;

    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t        curIncStride4Idx = 0;
        __API_CAFWM_PIXEL_VECTYP      vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
    #endif

    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
    #else
        uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
    #endif
        int32_t         blkCnt = iWidth;

        do {
            __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
            __API_CAFWM_PIXEL_VECTYP      vecSource = __API_CAFWM_PIXEL_VECLOAD(ptSrc);
            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            __API_CAFWM_PIXEL_VECTYP      vecSrcMsk = ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)(
                                            (uint8_t const *)ptSrcMsk, vIncStride4Offs);
            #else
            __API_CAFWM_PIXEL_VECTYP      vecSrcMsk = ARM_CONNECT2(vldrbq_u, __API_CAFWM_PIXEL_BIT_NUM)(ptSrcMsk);
            #endif

            __API_CAFWM_PIXEL_VECTYP      vecHwOpacity = vsubq(v256, vecSrcMsk);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

            vecTarget = __API_CAFWM_PIXEL_BLENDING(
                                            vecTarget, vecSource, vecHwOpacity);
            /* tail predication */
            __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget, ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif

            #if !__API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            ptSrcMsk += __API_CAFWM_VEC_INCR;
            #endif


            ptTargetCur += __API_CAFWM_VEC_INCR;
            ptSrc += __API_CAFWM_VEC_INCR;
            blkCnt -= __API_CAFWM_VEC_INCR;
        }
        while (blkCnt > 0);

        pSourceBase += (iSourceStride);
        pTargetBase += (iTargetStride);

    #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
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

    }
}


__OVERRIDE_WEAK
void __CAFWM_FUNC(src_msk_copy_x_mirror)(
                                    __API_CAFWM_INT_TYPE * __RESTRICT pSourceBase,
                                    int16_t iSourceStride,

                                #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                                    uint32_t * __RESTRICT ptSourceMaskBase,
                                #else
                                    uint8_t * __RESTRICT ptSourceMaskBase,
                                #endif
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                                    __API_CAFWM_INT_TYPE * __RESTRICT pTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t * __RESTRICT ptCopySize)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);

#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *ptSourceMask = ptSourceMaskBase;
#else
    uint8_t *ptSourceMask = ptSourceMaskBase;
#endif

#if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
    int_fast16_t iSourceMaskY = 0;
#endif

    for (   int_fast16_t y = 0;
            y < iHeight;
            y++) {

        __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = pSourceBase;
        __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = pTargetBase;


    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
    #else
        uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
    #endif

        uint32_t        curDecrStride1Idx = iWidth - 1;
        int32_t         blkCnt = iWidth;
    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t        curDecrStride4Idx = 4*(iWidth - 1);
        __API_CAFWM_PIXEL_VECTYP      vDecrStride4Offs =
                ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
    #endif
        __API_CAFWM_PIXEL_VECTYP      vDecrStride1Offs =
                ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


        do {
            __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
            __API_CAFWM_PIXEL_VECTYP      vecSource =
                __API_CAFWM_PIXEL_GATHVECLOAD(ptSrc, vDecrStride1Offs);
    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            __API_CAFWM_PIXEL_VECTYP      vecSrcMsk =
                ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)((uint8_t *)ptSrcMsk, vDecrStride4Offs);
    #else
            __API_CAFWM_PIXEL_VECTYP      vecSrcMsk =
                ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)(ptSrcMsk, vDecrStride1Offs);
    #endif
            __API_CAFWM_PIXEL_VECTYP      vecHwOpacity =  vsubq(v256, vecSrcMsk);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

            vecTarget =
                __API_CAFWM_PIXEL_BLENDING(vecTarget, vecSource,
                                                            vecHwOpacity);

            /* tail predication */
            __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget, ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            vDecrStride4Offs = ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
    #endif
            vDecrStride1Offs = ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


            ptTargetCur += __API_CAFWM_VEC_INCR;

            blkCnt -= __API_CAFWM_VEC_INCR;
        }
        while (blkCnt > 0);


        pSourceBase += iSourceStride;
        pTargetBase += iTargetStride;

    #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
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

    }
}


__OVERRIDE_WEAK
void __CAFWM_FUNC(src_msk_copy_y_mirror)(
                                    __API_CAFWM_INT_TYPE * __RESTRICT pSourceBase,
                                    int16_t iSourceStride,

                                #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                                    uint32_t * __RESTRICT ptSourceMaskBase,
                                #else
                                    uint8_t * __RESTRICT ptSourceMaskBase,
                                #endif
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                                    __API_CAFWM_INT_TYPE * __RESTRICT pTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t * __RESTRICT ptCopySize)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);

    pSourceBase += iSourceStride * (ptCopySize->iHeight - 1);

    assert (ptCopySize->iHeight <= ptSourceMaskSize->iHeight);
    ptSourceMaskBase += iSourceMaskStride * (ptCopySize->iHeight - 1);

#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *ptSourceMask = ptSourceMaskBase;
#else
    uint8_t *ptSourceMask = ptSourceMaskBase;
#endif

#if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
    int_fast16_t iSourceMaskY = 0;
#endif

    for (   int_fast16_t y = 0;
            y < iHeight;
            y++) {
        __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = pSourceBase;
        __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = pTargetBase;

    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t        curIncStride4Idx = 0;
        __API_CAFWM_PIXEL_VECTYP      vIncStride4Offs =
                ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
    #endif

    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
    #else
        uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
    #endif
        int32_t         blkCnt = iWidth;

        do {
            __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
            __API_CAFWM_PIXEL_VECTYP      vecSource = __API_CAFWM_PIXEL_VECLOAD(ptSrc);
            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            __API_CAFWM_PIXEL_VECTYP      vecSrcMsk =
                    ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)(
                                            (uint8_t const *)ptSrcMsk, vIncStride4Offs);
            #else
            __API_CAFWM_PIXEL_VECTYP      vecSrcMsk =
                    ARM_CONNECT2(vldrbq_u, __API_CAFWM_PIXEL_BIT_NUM)(ptSrcMsk);
            #endif


            __API_CAFWM_PIXEL_VECTYP      vecHwOpacity = vsubq(v256, vecSrcMsk);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

            vecTarget = __API_CAFWM_PIXEL_BLENDING(
                                            vecTarget, vecSource, vecHwOpacity);
            /* tail predication */
            __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget,
                    ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            vIncStride4Offs = ARM_CONNECT2(vidupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curIncStride4Idx, 4);
            #endif

            #if !__API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            ptSrcMsk += __API_CAFWM_VEC_INCR;
            #endif


            ptTargetCur += __API_CAFWM_VEC_INCR;
            ptSrc += __API_CAFWM_VEC_INCR;

            blkCnt -= __API_CAFWM_VEC_INCR;
        }
        while (blkCnt > 0);

        pSourceBase -= iSourceStride;
        pTargetBase += (iTargetStride);

    #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
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
    }
}


__OVERRIDE_WEAK
void __CAFWM_FUNC(src_msk_copy_xy_mirror)(
                                    __API_CAFWM_INT_TYPE * __RESTRICT pSourceBase,
                                    int16_t iSourceStride,

                                #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                                    uint32_t * __RESTRICT ptSourceMaskBase,
                                #else
                                    uint8_t * __RESTRICT ptSourceMaskBase,
                                #endif
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                                    __API_CAFWM_INT_TYPE * __RESTRICT pTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t * __RESTRICT ptCopySize)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    __API_CAFWM_PIXEL_VECTYP      v256 = ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(256);

    pSourceBase += iSourceStride * (ptCopySize->iHeight - 1);

    assert (ptCopySize->iHeight <= ptSourceMaskSize->iHeight);
    ptSourceMaskBase += iSourceMaskStride * (ptCopySize->iHeight - 1);


#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *ptSourceMask = ptSourceMaskBase;
#else
    uint8_t *ptSourceMask = ptSourceMaskBase;
#endif


#if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
    int_fast16_t iSourceMaskY = 0;
#endif

    for (   int_fast16_t y = 0;
            y < iHeight;
            y++) {
        __API_CAFWM_INT_TYPE       *__RESTRICT ptSrc = pSourceBase;
        __API_CAFWM_INT_TYPE       *__RESTRICT ptTargetCur = pTargetBase;


    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t       *__RESTRICT ptSrcMsk = ptSourceMask;
    #else
        uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
    #endif
        uint32_t        curDecrStride1Idx = iWidth - 1;
        int32_t         blkCnt = iWidth;
    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t        curDecrStride4Idx = 4*(iWidth - 1);
        __API_CAFWM_PIXEL_VECTYP      vDecrStride4Offs =
                ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
    #endif
        __API_CAFWM_PIXEL_VECTYP      vDecrStride1Offs =
                ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);


        do {
            __API_CAFWM_PIXEL_VECTYP      vecTarget = __API_CAFWM_PIXEL_VECLOAD(ptTargetCur);
            __API_CAFWM_PIXEL_VECTYP      vecSource =
                __API_CAFWM_PIXEL_GATHVECLOAD(ptSrc, vDecrStride1Offs);
    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            __API_CAFWM_PIXEL_VECTYP      vecSrcMsk =
                ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)((uint8_t *)ptSrcMsk, vDecrStride4Offs);
    #else
            __API_CAFWM_PIXEL_VECTYP      vecSrcMsk =
                ARM_CONNECT2(vldrbq_gather_offset_u, __API_CAFWM_PIXEL_BIT_NUM)(ptSrcMsk, vDecrStride1Offs);
    #endif
            __API_CAFWM_PIXEL_VECTYP      vecHwOpacity = vsubq(v256, vecSrcMsk);

            #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vecHwOpacity = vpselq(ARM_CONNECT2(vdupq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(0),vecHwOpacity,
                ARM_CONNECT2(vcmpeqq_n_u, __API_CAFWM_PIXEL_BIT_NUM)(vecHwOpacity, 1));
            #endif

            vecTarget =
                __API_CAFWM_PIXEL_BLENDING(vecTarget, vecSource,
                                                            vecHwOpacity);

            /* tail predication */
            __API_CAFWM_PIXEL_PVECSTORE(ptTargetCur, vecTarget, ARM_CONNECT3(vctp, __API_CAFWM_PIXEL_BIT_NUM, q)(blkCnt));

    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            vDecrStride4Offs = ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride4Idx, 4);
    #endif
            vDecrStride1Offs = ARM_CONNECT2(vddupq_wb_u, __API_CAFWM_PIXEL_BIT_NUM)(&curDecrStride1Idx, 1);

            ptTargetCur += __API_CAFWM_VEC_INCR;

            blkCnt -= __API_CAFWM_VEC_INCR;
        }
        while (blkCnt > 0);

        pSourceBase -= iSourceStride;
        pTargetBase += (iTargetStride);

    #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
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
    }
}




#endif

#undef masks_fill
#undef masks_fill_x_mirror
#undef masks_fill_y_mirror
#undef masks_fill_xy_mirror
#undef masks_fill_mirror

#undef des_msk_fill
#undef des_msk_fill_x_mirror
#undef des_msk_fill_y_mirror
#undef des_msk_fill_xy_mirror
#undef des_msk_fill_mirror


#undef masks_copy
#undef masks_copy_x_mirror
#undef masks_copy_y_mirror
#undef masks_copy_xy_mirror
#undef masks_copy_mirror

#undef des_msk_copy
#undef des_msk_copy_x_mirror
#undef des_msk_copy_y_mirror
#undef des_msk_copy_xy_mirror
#undef des_msk_copy_mirror

#undef src_msk_fill
#undef src_msk_fill_x_mirror
#undef src_msk_fill_y_mirror
#undef src_msk_fill_xy_mirror
#undef src_msk_fill_mirror

#undef src_msk_copy
#undef src_msk_copy_x_mirror
#undef src_msk_copy_y_mirror
#undef src_msk_copy_xy_mirror
#undef src_msk_copy_mirror


#undef __API_CAFWM_COPY_LIKE_OP_NAME
#undef __API_CAFWM_OP_NAME
#undef __API_CAFWM_PIXEL_BLENDING
#undef ____CAFWM_FUNC
#undef ___CAFWM_FUNC
#undef __CAFWM_FUNC
#undef __API_CAFWM_COLOUR
#undef __API_CAFWM_INT_TYPE
#undef __API_CAFWM_PIXEL_BIT_NUM
#undef __API_CAFWM_PIXEL_VECLOAD
#undef __API_CAFWM_PIXEL_GATHVECLOAD
#undef __API_CAFWM_PIXEL_PVECSTORE
#undef ____CAFWM_TYPE
#undef ___CAFWM_TYPE
#undef __CAFWM_TYPE
#undef __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
#undef __API_CAFWM_CFG_1_HORIZONTAL_LINE
#undef __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT

