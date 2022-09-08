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
 * Title:        __arm_2d_meta_trans_with_masks_helium.inc
 * Description:  c code template for :
 *                  - transform_with_src_chn_mask_and_opacity
 *                  - transform_with_src_mask_and_opacity
 *                  - transform_with_src_chn_mask
 *                  - transform_with_src_mask
 *
 * $Date:        12. July 2022
 * $Revision:    V.1.0.4
 *
 * -------------------------------------------------------------------- */

#ifndef __API_MTWM_COLOUR
#   error You have to define __API_MTWM_COLOUR before using this c template
#endif
#ifndef __API_MTWM_COLOUR_NAME
#   error You have to define __API_MTWM_COLOUR_NAME before using this c template
#endif
#ifndef __API_MTWM_INT_TYPE
#   error You have to define the __API_MTWM_INT_TYPE before using this c template
#endif
#ifndef __API_MTWM_INT_TYPE_BIT_NUM
#   error You have to define the __API_MTWM_INT_TYPE_BIT_NUM before using this c template
#endif


/*! disable this feature by default */
#ifndef __API_MTWM_CFG_SUPPORT_SRC_MSK_WRAPING
#   define __API_MTWM_CFG_SUPPORT_SRC_MSK_WRAPING               0
#endif

//#ifndef __API_MTWM_CFG_1_HORIZONTAL_LINE
//#   define __API_MTWM_CFG_1_HORIZONTAL_LINE                     0
//#endif

//#ifndef __API_MTWM_CFG_CHANNEL_8in32_SUPPORT
//#   define __API_MTWM_CFG_CHANNEL_8in32_SUPPORT                 0
//#endif

#ifndef __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
#   define __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE  0
#endif

//#ifndef __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
//#   define __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE  0
//#endif

#ifndef __API_MTWM_CFG_SUPPORT_SOURCE_MASK
#   define __API_MTWM_CFG_SUPPORT_SOURCE_MASK                   0
#endif

//#ifndef __API_MTWM_CFG_SUPPORT_TARGET_MASK
//#   define __API_MTWM_CFG_SUPPORT_TARGET_MASK                   0
//#endif

#ifndef __API_MTWM_CFG_SUPPORT_OPACITY
#   define __API_MTWM_CFG_SUPPORT_OPACITY                       0
#endif

#undef ____MTWM_FUNC
#undef ___MTWM_FUNC
#undef __MTWM_FUNC


#if __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
    #define MASK_STRIDE_SCALE 4
#else
    #define MASK_STRIDE_SCALE 1
#endif

#if __API_MTWM_CFG_SUPPORT_OPACITY
    #define SCALE_BY_OPACITY(pixelAlpha, opa)   vPixelAlpha = (vPixelAlpha * opa) >> 8;
#else
    #define SCALE_BY_OPACITY(pixelAlpha, opa)
#endif


#ifndef __API_MTWM_OP_NAME
#   define ____MTWM_FUNC(__NAME, __COLOUR)                                      \
        __MVE_WRAPPER(__arm_2d_impl_##__COLOUR##_##__NAME)
#   define ___MTWM_FUNC(__NAME, __COLOUR)   ____MTWM_FUNC(__NAME, __COLOUR)
#else
#   define _____MTWM_FUNC(__OP_NAME, __NAME, __COLOUR)                          \
        __MVE_WRAPPER(__arm_2d_impl_##__COLOUR##_##__OP_NAME##_##__NAME)
#   define ____MTWM_FUNC(__OP_NAME, __NAME, __COLOUR)                           \
        _____MTWM_FUNC(__OP_NAME, __NAME, __COLOUR)
#   define ___MTWM_FUNC(__NAME, __COLOUR)                                       \
        ____MTWM_FUNC(__API_MTWM_OP_NAME, __NAME, __COLOUR)
#endif

#define __MTWM_FUNC(__NAME)   ___MTWM_FUNC(__NAME, __API_MTWM_COLOUR_NAME)


#undef ____MTWM_TYPE
#undef ___MTWM_TYPE
#undef __MTWM_TYPE

#ifndef __API_MTWM_OP_NAME
#   define ____MTWM_TYPE(__NAME, __COLOUR)  arm_2d_##__COLOUR##_##__NAME
#   define ___MTWM_TYPE(__NAME, __COLOUR)   ____MTWM_TYPE(__NAME, __COLOUR)
#else
#   define _____MTWM_TYPE(__OP_NAME, __NAME, __COLOUR)                          \
        arm_2d_##__COLOUR##_##__OP_NAME##_##__NAME
#   define ____MTWM_TYPE(__OP_NAME, __NAME, __COLOUR)                           \
        _____MTWM_TYPE(__OP_NAME, __NAME, __COLOUR)
#   define ___MTWM_TYPE(__NAME, __COLOUR)                                       \
        ____MTWM_TYPE(__API_MTWM_OP_NAME, __NAME, __COLOUR)
#endif


#define __MTWM_TYPE(__NAME)   ___MTWM_TYPE(__NAME, __API_MTWM_COLOUR)

/*============================ PROTOTYPES ====================================*/
extern
void __MTWM_FUNC(transform_with_mask)(
                                    #if __API_MTWM_CFG_SUPPORT_SOURCE_MASK      \
                                     || __API_MTWM_CFG_SUPPORT_TARGET_MASK
                                        __arm_2d_param_copy_orig_msk_t *ptThis,
                                    #else
                                        __arm_2d_param_copy_orig_t *ptParam,
                                    #endif
                                        __arm_2d_transform_info_t *ptInfo
                                    #if __API_MTWM_CFG_SUPPORT_OPACITY
                                       ,uint_fast16_t hwOpacity
                                    #endif
                                        );

/*============================ IMPLEMENTATION ================================*/


#if !__ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__

/**
  Scale Gray8 pixel vector with Area & Alpha
 */
#define __ARM_2D_SCALE_MASK_GRAY8VEC(/* outputs */                                                \
                                     vAvgPix, vTransp,                                            \
                                     /* inputs */                                                 \
                                     vPtVal, vAreaScal, vAlphaSc)                                 \
        float16x8_t vAlpha = vAreaScal * vcvtq_f16_u16(vAlphaSc) ;                                \
        vTransp = 256.0f16 * vAreaScal - vAlpha;                                                  \
        vAvgPix = vAlpha * vcvtq_f16_u16(vPtVal);

/**
  Scale Gray8 pixel vector with Area & Alpha. Accumulated version
 */
#define __ARM_2D_SCALE_MASK_GRAY8VEC_ACC(/* input / outputs */                                    \
                                     vAvgPix, vTransp,                                            \
                                     /* inputs */                                                 \
                                     vPtVal, vAreaScal, vAlphaSc)                                 \
        float16x8_t vAlpha = vAreaScal * vcvtq_f16_u16(vAlphaSc);                                 \
        vTransp += 256.0f16 * vAreaScal - vAlpha;                                                 \
        vAvgPix += vAlpha * vcvtq_f16_u16(vPtVal);


/**
  Scale R/G/B pixel vectors with Area & Alpha
 */
#define __ARM_2D_SCALE_MASK_RGBVEC(/* outputs */                                                  \
                                    vAvgPixelR, vAvgPixelG, vAvgPixelB, vAvgTransparency,         \
                                    /* inputs */                                                  \
                                    R, G, B, vScal, vAlphaSc)                                     \
        float16x8_t vAlpha = vScal * vcvtq_f16_u16(vAlphaSc) ;                                    \
        vAvgTransparency = 256.0f16 * vScal - vAlpha;                                             \
        vAvgPixelR = vAlpha * vcvtq_f16_u16(R);                                                   \
        vAvgPixelG = vAlpha * vcvtq_f16_u16(G);                                                   \
        vAvgPixelB = vAlpha * vcvtq_f16_u16(B);


/**
  Scale R/G/B pixel vectors with Area & Alpha. Accumulated version
 */
#define __ARM_2D_SCALE_MASK_RGBVEC_ACC(/* input / outputs */                                      \
                                       vAvgPixelR, vAvgPixelG, vAvgPixelB,vAvgTransparency,       \
                                       /* inputs */                                               \
                                       R, G, B,  vScal, vAlphaSc)                                 \
        float16x8_t vAlpha = vScal * vcvtq_f16_u16(vAlphaSc) ;                                    \
        vAvgTransparency += 256.0f16 * vScal - vAlpha;                                            \
        vAvgPixelR += vAlpha * vcvtq_f16_u16(R);                                                  \
        vAvgPixelG += vAlpha * vcvtq_f16_u16(G);                                                  \
        vAvgPixelB += vAlpha * vcvtq_f16_u16(B);


/**
  Mix Gray8 averaged pixel vector with transparency-scaled target vector
 */
#define __ARM_2D_BLEND_AVG_TARGET_GRAY8(vAvgPixel, vTarget, vAvgTransparency)                     \
                        vqaddq(vcvtq_u16_f16(vAvgPixel),                                          \
                               vcvtq_u16_f16(vAvgTransparency) * vTarget) >> 8;

/**
  Mix R, G, B averaged pixel vectors with transparency-scaled target vector
 */
#define __ARM_2D_BLEND_AVG_TARGET_RGB(/* input / outputs */                                       \
                             vAvgR, vAvgG, vAvgB,                                                 \
                             /* inputs */                                                         \
                             vTargetR, vTargetG, vTargetB, vAvgTrans)                             \
    vAvgR = vqaddq(vAvgR,  vTargetR * vAvgTrans);                                                 \
    vAvgR = vAvgR >> 8;                                                                           \
                                                                                                  \
    vAvgG = vqaddq(vAvgG, vTargetG * vAvgTrans);                                                  \
    vAvgG = vAvgG >> 8;                                                                           \
                                                                                                  \
    vAvgB = vqaddq(vAvgB, vTargetB * vAvgTrans);                                                  \
    vAvgB = vAvgB >> 8;


#else /* __ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__ */


/**
  Scale Gray8 pixel vector with Area & Alpha
 */
#define __ARM_2D_SCALE_MASK_GRAY8VEC(/* outputs */                                                \
                                     vAvgPix, vTransp,                                            \
                                     /* inputs */                                                 \
                                     vPtVal, vAreaScal, vAlphaSc)                                 \
        uint16x8_t vAlpha = vmulq_u16((vAreaScal >> 8),  vAlphaSc);                               \
        vTransp = vAreaScal - vAlpha;                                                             \
        vAvgPix = vrmulhq(vAlpha, vPtVal);

/**
  Scale Gray8 pixel vector with Area & Alpha. Accumulated version
 */
#define __ARM_2D_SCALE_MASK_GRAY8VEC_ACC(/* input / outputs */                                    \
                                     vAvgPix, vTransp,                                            \
                                     /* inputs */                                                 \
                                     vPtVal, vAreaScal, vAlphaSc)                                 \
        uint16x8_t vAlpha = vmulq_u16((vAreaScal >> 8), vAlphaSc);                                \
        vTransp = vqaddq(vTransp, vAreaScal - vAlpha);                                            \
        vAvgPix = vqaddq(vAvgPix, vrmulhq(vAlpha,  vPtVal));

/**
  Scale R/G/B pixel vectors with Area & Alpha
 */
#define __ARM_2D_SCALE_MASK_RGBVEC(/* outputs */                                                  \
                                    vAvgPixelR, vAvgPixelG, vAvgPixelB, vAvgTransparency,         \
                                    /* inputs */                                                  \
                                    R, G, B, vScal, vAlphaSc)                                     \
        uint16x8_t vAlpha = vmulq_u16((vScal >> 8), vAlphaSc);                                    \
        vAvgTransparency = vScal - vAlpha;                                                        \
        vAvgPixelR = vrmulhq_u16(vAlpha, R );                                                     \
        vAvgPixelG = vrmulhq_u16(vAlpha, G );                                                     \
        vAvgPixelB = vrmulhq_u16(vAlpha, B );


/**
  Scale R/G/B pixel vectors with Area & Alpha. Accumulated version
 */
#define __ARM_2D_SCALE_MASK_RGBVEC_ACC(/* input / outputs */                                      \
                                       vAvgPixelR, vAvgPixelG, vAvgPixelB,vAvgTransparency,       \
                                       /* inputs */                                               \
                                       R, G, B,  vScal, vAlphaSc)                                 \
        uint16x8_t vAlpha = vmulq_u16((vScal >> 8), vAlphaSc);                                    \
        vAvgTransparency = vqaddq(vAvgTransparency, vScal - vAlpha);                              \
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));                                  \
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));                                  \
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));


/**
  Mix Gray8 averaged pixel vector with transparency-scaled target vector
 */
#define __ARM_2D_BLEND_AVG_TARGET_GRAY8(vAvgPixel, vTarget, vAvgTransparency)                     \
                vminq(vAvgPixel + vrmulhq(vTarget, vAvgTransparency), vdupq_n_u16(255));


/**
  Mix R, G, B averaged pixel vectors with transparency-scaled target vector
 */
#define __ARM_2D_BLEND_AVG_TARGET_RGB(/* inputs / outputs */                                      \
                                 vAvgR, vAvgG, vAvgB,                                             \
                                 /* inputs */                                                     \
                                 vTargetR, vTargetG, vTargetB, vAvgTrans)                         \
    vAvgR = vqaddq(vAvgR,  vrmulhq(vTargetR, vAvgTrans));                                         \
    vAvgR = vminq(vAvgR, vdupq_n_u16(255));                                                       \
                                                                                                  \
    vAvgG = vqaddq(vAvgG,  vrmulhq(vTargetG, vAvgTrans));                                         \
    vAvgG = vminq(vAvgG, vdupq_n_u16(255));                                                       \
                                                                                                  \
    vAvgB = vqaddq(vAvgB,  vrmulhq(vTargetB, vAvgTrans));                                         \
    vAvgB = vminq(vAvgB, vdupq_n_u16(255));

#endif

#if __API_MTWM_COLOUR == ARM_2D_M_COLOUR_GRAY8

/**
  Unpack vectors of 8-bit widened pixels read from a input 2D coordinates if fits inside the region of
  interest
  Unpack vectors of 8-bit widened masks (alpha) read from a input 2D coordinates if fits inside the region of
  interest. Masks indexes are scaled based on input stride and exta scale for src chan. mask operarations
  Vector mask content is further scaled using input opacity
  Update global predictor tracking region fit & color mask comparison.
 */

#define __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_ARR_FAR(/* inputs */                              \
                                                           vecX, vecY,                              \
                                                           pOrigin, ptOrigValidRegion, iOrigStride, \
                                                           pMaskArr, maskStrd, maskScal,            \
                                                           opacity, predTail,                       \
                                                           /* outputs */                            \
                                                           vPixVal, vPixelAlpha,                    \
                                                           predGlb)                                 \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                       \
        /* set vector predicate if point is inside the region */                                    \
        mve_pred16_t    p =                                                                         \
            arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);                      \
        predGlb |= p;                                                                               \
        /* prepare vector of point offsets */                                                       \
        int16_t         correctionOffset = vminvq_s16(INT16_MAX, vPoint.Y) - 1;                     \
        uint16x8_t      ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;            \
                                                                                                    \
        /* base pointer update to compensate offset */                                              \
        uint8_t       *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride);               \
        /* retrieve all point values */                                                             \
        vPixVal =                                                                                   \
            vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p);                     \
                                                                                                    \
        uint16x8_t     maskOffs = maskScal * vPoint.X + (vPoint.Y - correctionOffset) * maskStrd;   \
        uint8_t       *pMaskCorrected = pMaskArr + (correctionOffset * maskStrd);                   \
        /* retrieve all mask values */                                                              \
        vPixelAlpha =                                                                               \
            vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p);                     \
                                                                                                    \
        SCALE_BY_OPACITY(vPixelAlpha, opacity);                                                     \
                                                                                                    \
        ALPHA_255_COMP_VEC16(vPixelAlpha, 255);



#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__

/* Compute averaged gray8 pixel 8-bit widenedvector and tranparency using 4 neighbouring pixel / masks */
/* Return predictors if vectors fit region of interest */
#define __ARM2D_AVG_NEIGHBR_GRAY8_PIX_MASK_ARR(/* inputs */                                               \
                                                 ptPoint, vXi, vYi,                                       \
                                                 Origin, ptOrigValidRegion, iOrigStride,                  \
                                                 pMaskArr, maskStride,                                    \
                                                 vTarget, opacity, predTail,                              \
                                                 /* outputs */                                            \
                                                 predGlb, vAvgPixel, vAvgTransparency)                    \
                                                                                                          \
    uint16x8_t      ptVal8, vPixelAlpha;                                                                  \
    /* combination of Bottom / Top & Left / Right areas contributions */                                  \
    __typeof__ (vAvgPixel)    vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                         \
                                                                                                          \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                 \
                                                                                                          \
    /*                                                                                                    \
     * accumulate / average over the 4 neigbouring pixels                                                 \
     */                                                                                                   \
                                                                                                          \
    /* Bottom Left averaging */                                                                           \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_ARR_FAR(vXi, vYi, pOrigin, ptOrigValidRegion,           \
                                                  iOrigStride, pMaskArr, maskStride, MASK_STRIDE_SCALE,   \
                                                  opacity, predTail,                                      \
                                                  ptVal8, vPixelAlpha, predGlb);                          \
                                                                                                          \
        __ARM_2D_SCALE_MASK_GRAY8VEC(vAvgPixel, vAvgTransparency, ptVal8, vAreaBL, vPixelAlpha);          \
    }                                                                                                     \
                                                                                                          \
    /* Bottom Right averaging */                                                                          \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_ARR_FAR(vaddq_n_s16(vXi, 1), vYi, pOrigin,              \
                                ptOrigValidRegion, iOrigStride, pMaskArr, maskStride, MASK_STRIDE_SCALE,  \
                                            opacity, predTail, ptVal8, vPixelAlpha, predGlb);             \
                                                                                                          \
        __ARM_2D_SCALE_MASK_GRAY8VEC_ACC(vAvgPixel, vAvgTransparency, ptVal8, vAreaBR, vPixelAlpha);      \
    }                                                                                                     \
                                                                                                          \
    /* Top Left averaging */                                                                              \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_ARR_FAR(vXi, vaddq_n_s16(vYi, 1), pOrigin,              \
                                ptOrigValidRegion, iOrigStride, pMaskArr,maskStride, MASK_STRIDE_SCALE,   \
                                opacity, predTail,                                                        \
                                ptVal8, vPixelAlpha, predGlb);                                            \
                                                                                                          \
        __ARM_2D_SCALE_MASK_GRAY8VEC_ACC(vAvgPixel, vAvgTransparency, ptVal8, vAreaTL, vPixelAlpha);      \
    }                                                                                                     \
                                                                                                          \
    /* Top Right averaging */                                                                             \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_ARR_FAR(vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1),       \
                        pOrigin, ptOrigValidRegion, iOrigStride, pMaskArr, maskStride, MASK_STRIDE_SCALE, \
                                                    opacity, predTail,ptVal8, vPixelAlpha, predGlb);      \
                                                                                                          \
        __ARM_2D_SCALE_MASK_GRAY8VEC_ACC(vAvgPixel, vAvgTransparency, ptVal8, vAreaTR, vPixelAlpha);      \
    }

#endif


#elif __API_MTWM_COLOUR == ARM_2D_M_COLOUR_RGB565

/**
  Unpack vectors of 8-bit widened R, G and B pixels read from a input 2D coordinates if fits inside the region of
  interest
  Unpack vectors of 8-bit widened masks (alpha) read from a input 2D coordinates if fits inside the region of
  interest. Masks indexes are scaled based on input stride and exta scale for src chan. mask operarations
  Vector mask content is further scaled using input opacity
  Update global predictor tracking region fit & color mask comparison.
 */
#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_ARRR_FAR(/* inputs */                                   \
                                                               vecX, vecY, pOrigin,                        \
                                                               ptOrigValidRegion, iOrigStride,             \
                                                               pMaskArr, maskStrd, maskScal,               \
                                                               opacity, predTail,                          \
                                                            /* outputs */                                  \
                                                               R, G, B,                                    \
                                                               vPixelAlpha, pGlb)                          \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                              \
        /* set vector predicate if point is inside the region */                                           \
        mve_pred16_t    p =                                                                                \
            arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);                             \
        pGlb |= p;                                                                                         \
        /* prepare vector of point offsets */                                                              \
        int16_t         correctionOffset = vminvq_s16(INT16_MAX, vPoint.Y) - 1;                            \
        uint16x8_t      ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;                   \
                                                                                                           \
        /* base pointer update to compensate offset */                                                     \
        uint16_t       *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride);                     \
        /* retrieve all point values */                                                                    \
        uint16x8_t      ptVal =                                                                            \
            vldrhq_gather_shifted_offset_z_u16(pOriginCorrected, ptOffs, predTail & p);                    \
                                                                                                           \
        /* expand channels */                                                                              \
        __arm_2d_rgb565_unpack_single_vec(ptVal, &R, &G, &B);                                              \
        uint16x8_t     maskOffs = maskScal * vPoint.X + (vPoint.Y - correctionOffset) * maskStrd;          \
        uint8_t       *pMaskCorrected = pMaskArr + (correctionOffset * maskStrd);                          \
        /* retrieve all mask values */                                                                     \
        vPixelAlpha =                                                                                      \
            vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p);                            \
                                                                                                           \
        SCALE_BY_OPACITY(vPixelAlpha, opacity);                                                            \
                                                                                                           \
        ALPHA_255_COMP_VEC16(vPixelAlpha, 255);


#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__

/* Computes averaged R, G, B 8-bit widened pixels vector and tranparency using 4 neighbouring pixel / masks */
/* Returns predictor if vectors fit region of interest */
#define __ARM2D_AVG_NEIGHBR_RGB565_PIX_MASK_ARR(ptPoint, vXi, vYi,                                        \
                                                        pOrigin, ptOrigValidRegion, iOrigStride,          \
                                                        pMaskArr, maskStride, vTarget,                    \
                                                        opacity, predTail,                                \
                                                        /* outputs */                                     \
                                                        predGlb,                                          \
                                                        vAvgPixelR, vAvgPixelG, vAvgPixelB,               \
                                                        vAvgTransparency)                                 \
                                                                                                          \
    uint16x8_t      R, G, B, vPixelAlpha;                                                                 \
    /* combination of Bottom / Top & Left / Right areas contributions */                                  \
    __typeof__ (vAvgPixelR)    vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                        \
                                                                                                          \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                 \
                                                                                                          \
                                                                                                          \
    /* Bottom Left averaging */                                                                           \
    {                                                                                                     \
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_ARRR_FAR(vXi, vYi, pOrigin, ptOrigValidRegion,         \
                                                            iOrigStride,                                  \
                                                            pMaskArr, maskStride, MASK_STRIDE_SCALE,      \
                                                            opacity, predTail,                            \
                                                            R, G, B, vPixelAlpha, predGlb);               \
                                                                                                          \
        __ARM_2D_SCALE_MASK_RGBVEC(vAvgPixelR, vAvgPixelG, vAvgPixelB, vAvgTransparency, R,               \
                                   G, B, vAreaBL, vPixelAlpha);                                           \
    }                                                                                                     \
                                                                                                          \
                                                                                                          \
    /* Bottom Right averaging */                                                                          \
    {                                                                                                     \
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_ARRR_FAR(vaddq_n_s16(vXi, 1), vYi,                     \
                                                                pOrigin, ptOrigValidRegion, iOrigStride,  \
                                                                pMaskArr,maskStride, MASK_STRIDE_SCALE,   \
                                                                opacity, predTail,                        \
                                                                R, G, B, vPixelAlpha,                     \
                                                                predGlb);                                 \
                                                                                                          \
        __ARM_2D_SCALE_MASK_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB,                                \
                                       vAvgTransparency, R, G, B, vAreaBR, vPixelAlpha);                  \
    }                                                                                                     \
                                                                                                          \
    /* Top Left averaging */                                                                              \
    {                                                                                                     \
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_ARRR_FAR(vXi, vaddq_n_s16(vYi, 1),                     \
                                                                pOrigin, ptOrigValidRegion, iOrigStride,  \
                                                                pMaskArr, maskStride, MASK_STRIDE_SCALE,  \
                                                                opacity, predTail,                        \
                                                                R, G, B, vPixelAlpha, predGlb);           \
                                                                                                          \
        __ARM_2D_SCALE_MASK_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB,                                \
                                       vAvgTransparency, R, G, B, vAreaTL, vPixelAlpha);                  \
    }                                                                                                     \
                                                                                                          \
    /* Top Right averaging */                                                                             \
    {                                                                                                     \
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_ARRR_FAR(vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1),     \
                                                                pOrigin, ptOrigValidRegion, iOrigStride,  \
                                                                pMaskArr, maskStride, MASK_STRIDE_SCALE,  \
                                                                opacity,predTail,                         \
                                                                R, G, B, vPixelAlpha, predGlb);           \
                                                                                                          \
        __ARM_2D_SCALE_MASK_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB,                                \
                                       vAvgTransparency, R, G, B, vAreaTR, vPixelAlpha);                  \
    }
#endif


#elif __API_MTWM_COLOUR == ARM_2D_M_COLOUR_CCCN888

/**
  Unpack vectors of 8-bit widened R, G and B pixels read from a input 2D coordinates if fits inside the region of
  interest. These are read from 2 adjacent 32-bit packed vectors hence 2 tail prediction masks are needed
  Unpack vectors of 8-bit widened masks (alpha) read from a input 2D coordinates if fits inside the region of
  interest. Masks indexes are scaled based on input stride and exta scale for src chan. mask operarations
  Vector mask content is further scaled using input opacity
  Update 2 global predictors tracking region fit for 1st and 2nd 32-bit vector.
 */

#define __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_ARR(/* inputs */                                       \
                                                        vecX, vecY,                                       \
                                                        pOrigin, ptOrigValidRegion, iOrigStride,          \
                                                        pMaskArr, maskStrd, maskScal, opacity,            \
                                                        predTailLow, predTailHigh,                        \
                                                       /* outputs */                                      \
                                                        R, G, B, vPixelAlpha, pGlbLo, pGlbHi)             \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                             \
        arm_2d_point_s32x4_t tPointLo, tPointHi;                                                          \
                                                                                                          \
        /* split 16-bit point vector into 2 x 32-bit vectors */                                           \
        vst1q(pscratch16, vPoint.X);                                                                      \
        tPointLo.X = vldrhq_s32(pscratch16);                                                              \
        tPointHi.X = vldrhq_s32(pscratch16 + 4);                                                          \
                                                                                                          \
        vst1q(pscratch16, vPoint.Y);                                                                      \
        tPointLo.Y = vldrhq_s32(pscratch16);                                                              \
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);                                                          \
                                                                                                          \
        /* 1st half */                                                                                    \
                                                                                                          \
        /* set vector predicate if point is inside the region */                                          \
        mve_pred16_t    p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);          \
        pGlbLo |= p;                                                                                      \
        /* prepare vector of point offsets */                                                             \
        uint32x4_t      ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;                                   \
                                                                                                          \
        /* retrieve all point values */                                                                   \
        uint32x4_t      ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);     \
                                                                                                          \
        vst1q(scratch32, ptVal);                                                                          \
                                                                                                          \
        uint32x4_t     maskOffs = maskScal * tPointLo.X + tPointLo.Y * maskStrd;                          \
        uint32x4_t      maskVal =                                                                         \
            vldrbq_gather_offset_z_u32(pMaskArr, maskOffs, predTailLow & p);                              \
                                                                                                          \
        vst1q(scratch32+8, maskVal);                                                                      \
                                                                                                          \
        /* 2nd half */                                                                                    \
                                                                                                          \
        /* set vector predicate if point is inside the region */                                          \
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);                          \
        pGlbHi |= p;                                                                                      \
        /* prepare vector of point offsets */                                                             \
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;                                                   \
                                                                                                          \
        /* retrieve all point values */                                                                   \
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);                    \
                                                                                                          \
        vst1q(scratch32 + 4, ptVal);                                                                      \
                                                                                                          \
        maskOffs = maskScal * tPointHi.X + tPointHi.Y * maskStrd;                                         \
         maskVal =                                                                                        \
        vldrbq_gather_offset_z_u32(pMaskArr, maskOffs, predTailHigh & p);                                 \
                                                                                                          \
        vst1q(scratch32+12, maskVal);                                                                     \
                                                                                                          \
        /* expand channels */                                                                             \
        __arm_2d_unpack_rgb888_from_mem((uint8_t *) scratch32, &R, &G, &B);                               \
                                                                                                          \
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *) &scratch32[8], vidupq_n_u16(0, 4));            \
                                                                                                          \
        SCALE_BY_OPACITY(vPixelAlpha, opacity);                                                           \
                                                                                                          \
        ALPHA_255_COMP_VEC16(vPixelAlpha, 255);

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__

/* compute averaged R, G, B 8-bit widened pixels vector and tranparency using 4 neighbouring pixel / masks */
/* Returns 2 predictors for top / bottom 32-bit vectors fitting region of interest */
#define __ARM2D_AVG_NEIGHBR_RGB888_PIX_MASK_ARR(ptPoint, vXi, vYi,                                       \
                                                    pOrigin, ptOrigValidRegion, iOrigStride,             \
                                                    pMaskArr, maskStride, vTarget,  opacity,             \
                                                    predTail,                                            \
                                                    /* outputs */                                        \
                                                    predGlbLo, predGlbHi,                                \
                                                    vAvgPixelR, vAvgPixelG, vAvgPixelB, vAvgTransparency)\
                                                                                                         \
    uint16x8_t      R, G, B, vPixelAlpha;                                                                \
    /* combination of Bottom / Top & Left / Right areas contributions */                                 \
    __typeof__ (vAvgPixelR)    vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                       \
                                                                                                         \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                \
                                                                                                         \
    /*                                                                                                   \
     * accumulate / average over the 4 neigbouring pixels                                                \
     */                                                                                                  \
                                                                                                         \
                                                                                                         \
    /* Bottom Left averaging */                                                                          \
    {                                                                                                    \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_ARR(vXi, vYi, pOrigin, ptOrigValidRegion,             \
                                                       iOrigStride, pMaskArr,                            \
                                                       maskStride, MASK_STRIDE_SCALE, opacity,           \
                                                       predTailLow, predTailHigh,                        \
                                                        R, G, B, vPixelAlpha, predGlbLo, predGlbHi);     \
                                                                                                         \
        __ARM_2D_SCALE_MASK_RGBVEC(vAvgPixelR, vAvgPixelG, vAvgPixelB, vAvgTransparency, R,              \
                                   G, B, vAreaBL, vPixelAlpha);                                          \
    }                                                                                                    \
                                                                                                         \
    /* Bottom Right averaging */                                                                         \
    {                                                                                                    \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_ARR(vaddq_n_s16(vXi, 1), vYi,                         \
                                                            pOrigin, ptOrigValidRegion, iOrigStride,     \
                                                            pMaskArr, maskStride, MASK_STRIDE_SCALE,     \
                                                            opacity, predTailLow, predTailHigh,          \
                                                            R, G, B, vPixelAlpha, predGlbLo, predGlbHi); \
                                                                                                         \
        __ARM_2D_SCALE_MASK_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB,                               \
                                       vAvgTransparency, R, G, B, vAreaBR, vPixelAlpha);                 \
    }                                                                                                    \
                                                                                                         \
    /* Top Left averaging */                                                                             \
    {                                                                                                    \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_ARR(vXi, vaddq_n_s16(vYi, 1),                         \
                                                       pOrigin, ptOrigValidRegion, iOrigStride,          \
                                                       pMaskArr, maskStride, MASK_STRIDE_SCALE,          \
                                                       opacity, predTailLow, predTailHigh,               \
                                                       R, G, B, vPixelAlpha, predGlbLo, predGlbHi);      \
                                                                                                         \
        __ARM_2D_SCALE_MASK_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB,                               \
                                       vAvgTransparency, R, G, B, vAreaTL, vPixelAlpha)                  \
    }                                                                                                    \
                                                                                                         \
    /* Top Right averaging */                                                                            \
    {                                                                                                    \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_ARR(vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1),         \
                                                        pOrigin, ptOrigValidRegion, iOrigStride,         \
                                                        pMaskArr, maskStride, MASK_STRIDE_SCALE,         \
                                                        opacity, predTailLow, predTailHigh,              \
                                                        R, G, B, vPixelAlpha, predGlbLo, predGlbHi);     \
                                                                                                         \
        __ARM_2D_SCALE_MASK_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB,                               \
                                       vAvgTransparency, R, G, B, vAreaTR, vPixelAlpha)                  \
    }

#endif

#endif


#if __API_MTWM_COLOUR == ARM_2D_M_COLOUR_GRAY8

static
void __MVE_WRAPPER(ARM_CONNECT2(__arm_2d_impl_gray8_, get_pixel_colour_mask)) (
                                                 ARM_2D_POINT_VEC * ptPoint,
                                                 arm_2d_region_t * ptOrigValidRegion,
                                                 uint8_t * pOrigin, int16_t iOrigStride,
                                                 uint8_t * pTarget,
                                    #if __API_MTWM_CFG_SUPPORT_SOURCE_MASK
                                                 uint8_t * pchOrigMask, int16_t iOrigmaskStride,
                                    #else
                                                 __API_INT_TYPE MaskColour,
                                    #endif
                                    #if __API_MTWM_CFG_SUPPORT_OPACITY
                                                 uint16_t hwOpacity,
                                    #endif
                                                 uint32_t elts)
{
    iOrigmaskStride *= MASK_STRIDE_SCALE;

    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_u16(pTarget);

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    /* accumulated pixel vectors */
    PIX_VEC_TYP     vAvgPixel;

    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;
    PIX_VEC_TYP     vAvgTransparency;


#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_MASK_ARR(ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                                           iOrigStride, pchOrigMask, iOrigmaskStride, vTarget,
                                           hwOpacity, predTail,
                                           predGlb, vAvgPixel, vAvgTransparency);
    }
#else
    {
        uint16x8_t      ptVal8, vPixelAlpha;

        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_ARR_FAR(vXi, vYi, pOrigin, ptOrigValidRegion,
                                                          iOrigStride, pchOrigMask, iOrigmaskStride,
                                                          MASK_STRIDE_SCALE, hwOpacity, predTail,
                                                          ptVal8, vPixelAlpha, predGlb);

        __ARM_2D_SCALE_MASK_GRAY8VEC(vAvgPixel, vAvgTransparency, ptVal8, AREA_UNITY, vPixelAlpha);

    }
#endif

    /* blending */
    uint16x8_t      vBlended = __ARM_2D_BLEND_AVG_TARGET_GRAY8(vAvgPixel, vTarget, vAvgTransparency);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vstrbq_p_u16(pTarget, vTarget, predTail);
}

#elif __API_MTWM_COLOUR == ARM_2D_M_COLOUR_RGB565

static
void __MVE_WRAPPER(ARM_CONNECT2(__arm_2d_impl_rgb565_, get_pixel_colour_mask))(ARM_2D_POINT_VEC * ptPoint,
                                                            arm_2d_region_t * ptOrigValidRegion,
                                                            uint16_t * pOrigin,
                                                            int16_t iOrigStride,
                                                            uint16_t * pTarget,
                                                    #if __API_MTWM_CFG_SUPPORT_SOURCE_MASK
                                                           uint8_t * pchOrigMask,
                                                           int16_t iOrigmaskStride,
                                                    #else
                                                        __API_INT_TYPE MaskColour,
                                                    #endif
                                                    #if __API_MTWM_CFG_SUPPORT_OPACITY
                                                           uint16_t hwOpacity,
                                                    #endif
                                                            uint32_t elts)
{
    iOrigmaskStride *= MASK_STRIDE_SCALE;

    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q(pTarget);
    PIX_VEC_TYP     vAvgTransparency;


    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    /* accumulated pixel vectors */
    PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;
    uint16x8_t      vAvgR, vAvgG, vAvgB, vAvgTrans;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__

    __ARM2D_AVG_NEIGHBR_RGB565_PIX_MASK_ARR(ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                                            iOrigStride, pchOrigMask, iOrigmaskStride, vTarget,
                                            hwOpacity, predTail, predGlb, vAvgPixelR,
                                            vAvgPixelG, vAvgPixelB, vAvgTransparency);

#else
    {
        uint16x8_t      R, G, B, vPixelAlpha;

        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_ARRR_FAR(vXi, vYi, pOrigin, ptOrigValidRegion,   iOrigStride,
                                                            pchOrigMask,
                                                            iOrigmaskStride, MASK_STRIDE_SCALE,
                                                            hwOpacity, predTail, R, G, B,
                                                            vPixelAlpha, predGlb);

        __ARM_2D_SCALE_MASK_RGBVEC(vAvgPixelR, vAvgPixelG, vAvgPixelB, vAvgTransparency, R,
                                   G, B, AREA_UNITY, vPixelAlpha);
    }
#endif

    vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
    vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
    vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    vAvgTrans = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgTransparency);

    /* blending */
    uint16x8_t      vBlended;
    uint16x8_t      vTargetR, vTargetG, vTargetB;

    __arm_2d_rgb565_unpack_single_vec(vTarget, &vTargetR, &vTargetG, &vTargetB);

    /* merge */
    __ARM_2D_BLEND_AVG_TARGET_RGB(vAvgR, vAvgG, vAvgB, vTargetR, vTargetG, vTargetB, vAvgTrans);

    vBlended = __arm_2d_rgb565_pack_single_vec(vAvgR, vAvgG, vAvgB);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(pTarget, vTarget, predTail);
}


#elif __API_MTWM_COLOUR == ARM_2D_M_COLOUR_CCCN888


static
void __MVE_WRAPPER(ARM_CONNECT2(__arm_2d_impl_cccn888_, get_pixel_colour_mask))(ARM_2D_POINT_VEC *ptPoint,
                                        arm_2d_region_t *ptOrigValidRegion,
                                        uint32_t *pOrigin,
                                        int16_t iOrigStride,
                                        uint32_t *pTarget,
                                        #if __API_MTWM_CFG_SUPPORT_SOURCE_MASK
                                               uint8_t * pchOrigMask,
                                               int16_t iOrigmaskStride,
                                        #else
                                            __API_INT_TYPE MaskColour,
                                        #endif
                                        #if __API_MTWM_CFG_SUPPORT_OPACITY
                                               uint16_t hwOpacity,
                                        #endif
                                        uint32_t elts
                                       )
{
    iOrigmaskStride *= MASK_STRIDE_SCALE;

    ARM_ALIGN(8) uint32_t scratch32[32];
    int16_t        *pscratch16 = (int16_t *) scratch32;
    uint32x4_t      vTargetLo = vld1q(pTarget);
    uint32x4_t      vTargetHi = vld1q(pTarget + 4);
    mve_pred16_t    predTailLow = vctp32q(elts);
    mve_pred16_t    predTailHigh = elts - 4 > 0 ? vctp32q(elts - 4) : 0;
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    /* accumulated pixel vectors */
    PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

    /* predicate accumulators */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlbLo = 0, predGlbHi = 0;
    PIX_VEC_TYP     vAvgTransparency;
    uint16x8_t      vAvgR, vAvgG, vAvgB, vAvgTrans;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)  &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    /*
     * accumulate / average over the 4 neigbouring pixels
     */
    __ARM2D_AVG_NEIGHBR_RGB888_PIX_MASK_ARR(ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                                            iOrigStride, pchOrigMask, iOrigmaskStride, vTarget,
                                            hwOpacity, predTail, predGlbLo, predGlbHi,
                                            vAvgPixelR, vAvgPixelG, vAvgPixelB, vAvgTransparency);
#else
    {
        uint16x8_t      R, G, B, vPixelAlpha;

        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_ARR(vXi, vYi, pOrigin, ptOrigValidRegion,
                                                       iOrigStride, pchOrigMask,
                                                       iOrigmaskStride, MASK_STRIDE_SCALE,
                                                       hwOpacity, predTailLow, predTailHigh,
                                                       R, G, B, vPixelAlpha, predGlbLo, predGlbHi);

        __ARM_2D_SCALE_MASK_RGBVEC(vAvgPixelR, vAvgPixelG, vAvgPixelB, vAvgTransparency, R,
                                   G, B, AREA_UNITY, vPixelAlpha);
    }
#endif

    vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
    vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
    vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    vAvgTrans = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgTransparency);

    /* alpha blending */
    uint16x8_t      vTargetR, vTargetG, vTargetB;

    __arm_2d_unpack_rgb888_from_mem((const uint8_t *) pTarget, &vTargetR, &vTargetG, &vTargetB);


    /* merge */
    __ARM_2D_BLEND_AVG_TARGET_RGB(vAvgR, vAvgG, vAvgB, vTargetR, vTargetG, vTargetB, vAvgTrans);

    /* pack */
    __arm_2d_pack_rgb888_to_mem((uint8_t *) scratch32, vAvgR, vAvgG, vAvgB);

    uint32x4_t      TempPixel = vld1q(scratch32);

    /* select between target pixel, averaged pixed */
    TempPixel = vpselq_u32(TempPixel, vTargetLo, predGlbLo);

    vst1q_p(pTarget, TempPixel, predTailLow);

    TempPixel = vld1q(scratch32 + 4);

    /* select between target pixel, averaged pixed */
    TempPixel = vpselq_u32(TempPixel, vTargetHi, predGlbHi);

    vst1q_p(pTarget + 4, TempPixel, predTailHigh);
}



#endif



#if !__ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__



__OVERRIDE_WEAK
void __MTWM_FUNC(transform_with_mask)(
                                    #if __API_MTWM_CFG_SUPPORT_SOURCE_MASK      \
                                     || __API_MTWM_CFG_SUPPORT_TARGET_MASK
                                        __arm_2d_param_copy_orig_msk_t *ptThis,
                                    #else
                                        __arm_2d_param_copy_orig_t *ptParam,
                                    #endif
                                        __arm_2d_transform_info_t *ptInfo
                                    #if __API_MTWM_CFG_SUPPORT_OPACITY
                                       ,uint_fast16_t hwOpacity
                                    #endif
                                        )
{

#if __API_MTWM_CFG_SUPPORT_SOURCE_MASK                                          \
 || __API_MTWM_CFG_SUPPORT_TARGET_MASK
    __arm_2d_param_copy_orig_t *ptParam =
        &(ptThis->use_as____arm_2d_param_copy_orig_t);
#endif

    int_fast16_t iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;

    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;

#if __API_MTWM_CFG_SUPPORT_SOURCE_MASK
    uint8_t *pOriginMask = this.tOrigMask.pBuffer;
    int_fast16_t iOrigMaskStride = this.tOrigMask.iStride;
#else
    __API_INT_TYPE MaskColour = ptInfo->Mask.hwColour;
#endif

#if __API_MTWM_CFG_SUPPORT_OPACITY
    hwOpacity += (hwOpacity == 255);
#endif

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    float           invIWidth = iWidth > 1 ? 1.0f / (float) (iWidth - 1) : __LARGEINVF32;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScale,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last cols */
    float           slopeY, slopeX;

    slopeY = (regrCoefs[1].interceptY - regrCoefs[0].interceptY) * invIWidth;
    slopeX = (regrCoefs[1].interceptX - regrCoefs[0].interceptX) * invIWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates (intercepts for regression in X direction */
        float           colFirstY = regrCoefs[0].slopeY * y + regrCoefs[0].interceptY;
        float           colFirstX = regrCoefs[0].slopeX * y + regrCoefs[0].interceptX;

        int32_t         nbVecElts = iWidth;
        float16x8_t     vX = vcvtq_f16_s16((int16x8_t) vidupq_n_u16(0, 1));
        __API_INT_TYPE       *pTargetBaseCur = pTargetBase;

        while (nbVecElts > 0) {
            arm_2d_point_f16x8_t tPointV;

            /* linear interpolation thru first & last columns */
            tPointV.X =
                vfmaq_n_f16(vdupq_n_f16(colFirstX), vX, slopeX);
            tPointV.Y =
                vfmaq_n_f16(vdupq_n_f16(colFirstY), vX, slopeY);

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_ROTATION_FOR_PERFORMANCE__)
            tPointV.X = vaddq_m_n_f16(tPointV.X, tPointV.X, __CALIB, vcmpgtq(tPointV.X, 0));
            tPointV.X = vsubq_m_n_f16(tPointV.X, tPointV.X, __CALIB, vcmpleq(tPointV.X, 0));

            tPointV.Y = vaddq_m_n_f16(tPointV.Y, tPointV.Y, __CALIB, vcmpgtq(tPointV.Y, 0));
            tPointV.Y = vsubq_m_n_f16(tPointV.Y, tPointV.Y, __CALIB, vcmpleq(tPointV.Y, 0));
#endif

            __ARM_2D_FUNC(get_pixel_colour_mask)(
                                &tPointV,
                                &ptParam->tOrigin.tValidRegion,
                                ptParam->tOrigin.pBuffer,
                                iOrigStride,
                                pTargetBaseCur,
                            #if __API_MTWM_CFG_SUPPORT_SOURCE_MASK
                                pOriginMask,
                                iOrigMaskStride,
                            #else
                                MaskColour,
                            #endif
                            #if __API_MTWM_CFG_SUPPORT_OPACITY
                                hwOpacity,
                            #endif
                                nbVecElts
                            );

                pTargetBaseCur += 8;
                vX += 8.0f16;
                nbVecElts -= 8;
        }
        pTargetBase += (iTargetStride);
    }
}


#else /* __ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__ */


__OVERRIDE_WEAK
void __MTWM_FUNC(transform_with_mask)(
                                    #if __API_MTWM_CFG_SUPPORT_SOURCE_MASK      \
                                     || __API_MTWM_CFG_SUPPORT_TARGET_MASK
                                        __arm_2d_param_copy_orig_msk_t *ptThis,
                                    #else
                                        __arm_2d_param_copy_orig_t *ptParam,
                                    #endif
                                        __arm_2d_transform_info_t *ptInfo
                                    #if __API_MTWM_CFG_SUPPORT_OPACITY
                                       ,uint_fast16_t hwOpacity
                                    #endif
                                        )
{
#if __API_MTWM_CFG_SUPPORT_SOURCE_MASK                                          \
 || __API_MTWM_CFG_SUPPORT_TARGET_MASK
    __arm_2d_param_copy_orig_t *ptParam =
        &(ptThis->use_as____arm_2d_param_copy_orig_t);
#endif

    int_fast16_t iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;

#if __API_MTWM_CFG_SUPPORT_SOURCE_MASK
    uint8_t *pOriginMask = this.tOrigMask.pBuffer;
    int_fast16_t iOrigMaskStride = this.tOrigMask.iStride;
#else
    __API_INT_TYPE MaskColour = ptInfo->Mask.hwColour;
#endif

#if __API_MTWM_CFG_SUPPORT_OPACITY
    hwOpacity += (hwOpacity == 255);
#endif

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    q31_t             invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScale,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);


    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t         nbVecElts = iWidth;
        int16x8_t       vX = (int16x8_t) vidupq_n_u16(0, 1);
        __API_INT_TYPE       *pTargetBaseCur = pTargetBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {
            arm_2d_point_s16x8_t tPointV;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);



            __ARM_2D_FUNC(get_pixel_colour_mask)(
                                &tPointV,
                                &ptParam->tOrigin.tValidRegion,
                                ptParam->tOrigin.pBuffer,
                                iOrigStride,
                                pTargetBaseCur,
                            #if __API_MTWM_CFG_SUPPORT_SOURCE_MASK
                                pOriginMask,
                                iOrigMaskStride,
                            #else
                                MaskColour,
                            #endif
                            #if __API_MTWM_CFG_SUPPORT_OPACITY
                                hwOpacity,
                            #endif
                                nbVecElts
                            );

            pTargetBaseCur += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }
        pTargetBase += iTargetStride;
    }
}



#endif  /* __ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__ */




#undef get_pixel_colour_mask
#undef transform_with_mask
#undef MASK_STRIDE_SCALE
#undef SCALE_BY_OPACITY

#undef __ARM2D_AVG_NEIGHBR_GRAY8_PIX_MASK_ARR
#undef __ARM2D_AVG_NEIGHBR_RGB565_PIX_MASK_ARR
#undef __ARM2D_AVG_NEIGHBR_RGB888_PIX_MASK_ARR


#undef __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_ARR_FAR
#undef __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_ARRR_FAR
#undef __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_ARR

#undef __ARM_2D_SCALE_MASK_GRAY8VEC
#undef __ARM_2D_SCALE_MASK_GRAY8VEC_ACC
#undef __ARM_2D_SCALE_MASK_RGBVEC
#undef __ARM_2D_SCALE_MASK_RGBVEC_ACC

#undef __API_MTWM_COPY_LIKE_OP_NAME
#undef __API_MTWM_OP_NAME
#undef __API_MTWM_PIXEL_BLENDING
#undef ____MTWM_FUNC
#undef ___MTWM_FUNC
#undef __MTWM_FUNC
#undef __API_MTWM_COLOUR
#undef __API_MTWM_COLOUR_NAME
#undef __API_MTWM_INT_TYPE
#undef __API_MTWM_INT_TYPE_BIT_NUM
#undef ____MTWM_TYPE
#undef ___MTWM_TYPE
#undef __MTWM_TYPE
#undef __API_MTWM_CFG_SUPPORT_SRC_MSK_WRAPING
#undef __API_MTWM_CFG_1_HORIZONTAL_LINE
#undef __API_MTWM_CFG_CHANNEL_8in32_SUPPORT
#undef __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
#undef __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
#undef __API_MTWM_CFG_SUPPORT_SOURCE_MASK
#undef __API_MTWM_CFG_SUPPORT_TARGET_MASK
#undef __API_MTWM_CFG_SUPPORT_OPACITY
