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
 * Title:        arm-2d_transform_helium.c
 * Description:  Acceleration extensions using Helium.
 *
 * $Date:        12. July 2022
 * $Revision:    V.0.1.5
 *
 * Target Processor:  Cortex-M cores with Helium
 *
 * -------------------------------------------------------------------- */



#define __ARM_2D_IMPL__

#include "arm_2d.h"
#include "__arm_2d_impl.h"

#ifdef __ARM_2D_COMPILATION_UNIT
#undef __ARM_2D_COMPILATION_UNIT

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




#include "__arm_2d_math_helium.h"
#include "__arm_2d_utils_helium.h"
#ifdef   __cplusplus
extern "C" {
#endif




static
mve_pred16_t arm_2d_is_point_vec_inside_region_s16(const arm_2d_region_t * ptRegion,
                                               const arm_2d_point_s16x8_t * ptPoint)
{
    mve_pred16_t    p0 = vcmpgeq(ptPoint->X, ptRegion->tLocation.iX);
    p0 = vcmpgeq_m(ptPoint->Y, ptRegion->tLocation.iY, p0);
    p0 = vcmpltq_m(ptPoint->X, ptRegion->tLocation.iX + ptRegion->tSize.iWidth, p0);
    p0 = vcmpltq_m(ptPoint->Y, ptRegion->tLocation.iY + ptRegion->tSize.iHeight, p0);

    return p0;
}

static
mve_pred16_t arm_2d_is_point_vec_inside_region_s32(const arm_2d_region_t * ptRegion,
                                               const arm_2d_point_s32x4_t * ptPoint)
{
    mve_pred16_t    p0 = vcmpgeq_n_s32(ptPoint->X, ptRegion->tLocation.iX);
    p0 = vcmpgeq_m_n_s32(ptPoint->Y, ptRegion->tLocation.iY, p0);
    p0 = vcmpltq_m_n_s32(ptPoint->X, ptRegion->tLocation.iX + ptRegion->tSize.iWidth, p0);
    p0 = vcmpltq_m_n_s32(ptPoint->Y, ptRegion->tLocation.iY + ptRegion->tSize.iHeight, p0);

    return p0;
}


/**
  @brief         return 3 vector of 16-bit channels (8-bit widened) taken from a memory reference
  @param[in]     pMem           pointer to packed 8-bit channel
  @param[out]    R              vector of 16-bit widened R channel
  @param[out]    G              vector of 16-bit widened G channel
  @param[out]    B              vector of 16-bit widened B channel
 */
void __arm_2d_unpack_rgb888_from_mem(const uint8_t * pMem, uint16x8_t * R, uint16x8_t * G,
                                     uint16x8_t * B)
{
    uint16x8_t      sg = vidupq_n_u16(0, 4);

    *R = vldrbq_gather_offset_u16(pMem, sg);
    *G = vldrbq_gather_offset_u16(pMem + 1, sg);
    *B = vldrbq_gather_offset_u16(pMem + 2, sg);
}

/**
  @brief         interleave 3 x 16-bit widened vectors into 8-bit memory reference
                 (4th channel untouched)
  @param[in]     pMem           pointer to packed 8-bit channel
  @param[in]     R              vector of 16-bit widened R channel
  @param[in]     G              vector of 16-bit widened G channel
  @param[in]     B              vector of 16-bit widened B channel
 */
void __arm_2d_pack_rgb888_to_mem(uint8_t * pMem, uint16x8_t R, uint16x8_t G, uint16x8_t B)
{
    uint16x8_t      sg = vidupq_n_u16(0, 4);

    vstrbq_scatter_offset_u16(pMem,     sg, vminq(R, vdupq_n_u16(255)));
    vstrbq_scatter_offset_u16(pMem + 1, sg, vminq(G, vdupq_n_u16(255)));
    vstrbq_scatter_offset_u16(pMem + 2, sg, vminq(B, vdupq_n_u16(255)));
    //vstrbq_scatter_offset_u16(pMem + 3, sg, vdupq_n_u16(0));
}



/* selector for 16-bit vector gather load */
/* FAR version is been used when offsets involved are above 65535 16-bit words */
#define FAR_OFFSET   _FAR
#define NEAR_OFFSET


/**
  unpack vectors of 8-bit widened pixels read from a input 2D coordinates if fits inside the region of
  interest or alternative target pixel if content matches color mask
  (vector of packed pixels & region of interest name implicit and fixed to respectively
  vTarget and ptOrigValidRegion)
  Update global predictor tracking region fit & color mask comparison.
 */
#define __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR(/* inputs */                              \
                                                      vecX, vecY, pOrigin, ptOrigValidRegion,       \
                                                      iOrigStride, MaskColour, vTarget, predTail,   \
                                                      /* outputs */                                 \
                                                      vPixVal, predGlb)                             \
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
        /* combine 2 predicates set to true if point is in the region & values */                   \
        /*  different from color mask */                                                            \
        p = vcmpneq_m_n_u16(vPixVal, MaskColour, p);                                                \
        predGlb |= p;                                                                               \
        vPixVal = vpselq_u16(vPixVal, vTarget, p);




/**
  unpack vectors of pixels read from a input 2D coordinates if fits inside the region of
  interest or alternative target pixel if content matches color mask
  (vector of packed pixels & region of interest name implicit and fixed to respectively
  vTarget and ptOrigValidRegion)
  Update global predictor tracking region fit & color mask comparison.
 */

#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK(/* inputs */                                          \
                                                    vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,   \
                                                    MaskColour, vTarget, predTail,                         \
                                                    /* outputs */                                          \
                                                    ptVal, predGlb)                                        \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                              \
        /* set vector predicate if point is inside the region */                                           \
        mve_pred16_t    p =                                                                                \
            arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);                             \
                                                                                                           \
        /* prepare vector of point offsets */                                                              \
        uint16x8_t      ptOffs = vPoint.X + vPoint.Y * iOrigStride;                                        \
         /* retrieve all point values */                                                                   \
        ptVal =                                                                                            \
            vldrhq_gather_shifted_offset_z_u16(pOrigin, ptOffs, predTail & p);                             \
                                                                                                           \
        /* combine 2 predicates set to true if point is in the region & values different from color mask */\
        p = vcmpneq_m_n_u16(ptVal, MaskColour, p);                                                         \
        predGlb |= p;                                                                                      \
        ptVal = vpselq_u16(ptVal, vTarget, p);



#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT(/* inputs */                                                 \
                                              vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,         \
                                              MaskColour, vTarget, predTail,                               \
                                              /* outputs */                                                \
                                              vPixValR, vPixValG, vPixValB, predGlb)                       \
        uint16x8_t      ptVal;                                                                             \
                                                                                                           \
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK(vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,  \
                                   MaskColour, vTarget, predTail, ptVal, predGlb)                          \
        /* expand channels */                                                                              \
        __arm_2d_rgb565_unpack_single_vec(ptVal, &vPixValR, &vPixValG, &vPixValB);





/**
  Same as above but use offset compensation during gather load.
  unpack vectors of pixels read from a input 2D coordinates if fits inside the region of
  interest or alternative target pixel if content matches color mask
  (vector of packed pixels & region of interest name implicit and fixed to respectively
  vTarget and ptOrigValidRegion)
  Update global predictor tracking region fit & color mask comparison.
 */

#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_FAR(                                                  \
                                                /* inputs */                                               \
                                                vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,       \
                                                MaskColour, vTarget, predTail,                             \
                                                /* outputs */                                              \
                                                ptVal, predGlb)                                            \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                              \
        /* set vector predicate if point is inside the region */                                           \
        mve_pred16_t    p =                                                                                \
            arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);                             \
        /* prepare vector of point offsets */                                                              \
        int16_t         correctionOffset = vminvq_s16(INT16_MAX, vPoint.Y) - 1;                            \
        uint16x8_t      ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;                   \
                                                                                                           \
        /* base pointer update to compensate offset */                                                     \
        uint16_t       *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride);                     \
        /* retrieve all point values */                                                                    \
        ptVal =                                                                                            \
            vldrhq_gather_shifted_offset_z_u16(pOriginCorrected, ptOffs, predTail & p);                    \
                                                                                                           \
        /* combine 2 predicates set to true if point is in the region & values different from color mask */\
        p = vcmpneq_m_n_u16(ptVal, MaskColour, p);                                                         \
        predGlb |= p;                                                                                      \
        ptVal = vpselq_u16(ptVal, vTarget, p);


#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_FAR(/* inputs */                                             \
                                                  vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,     \
                                                  MaskColour, vTarget, predTail,                           \
                                                  /* outputs */                                            \
                                                  vPixValR, vPixValG, vPixValB, predGlb)                   \
        uint16x8_t      ptVal;                                                                             \
                                                                                                           \
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_FAR(vecX, vecY, pOrigin, ptOrigValidRegion,           \
                        iOrigStride, MaskColour, vTarget, predTail, ptVal, predGlb);                       \
                                                                                                           \
        /* expand channels */                                                                              \
        __arm_2d_rgb565_unpack_single_vec(ptVal, &vPixValR, &vPixValG, &vPixValB);


/**
  unpack vectors of 32-bit pixels read from a input 2D coordinates if fits inside the region of
  interest or alternative target pixel if content matches color mask
  16-bit vector processed in 2 parts because of 32-bit requirements, so handles 8 x 32-bit vectors
  (vectors of packed pixels & region of interest name implicit and fixed to respectively
  vTargetLo, vectorHi and ptOrigValidRegion)
  Update 2 global predictors tracking region fit & color mask comparison for 1st and 2nd half.
 */

#define __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_NOUNPK(/* inputs */ \
                                                    vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,   \
                                                    MaskColour, vTargetLo, vTargetHi,                      \
                                                    predTailLo, predTailHi,                                \
                                                    /* outputs */                                          \
                                                    pointLo, pointHi, predGlbLo, predGlbHi)                \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                              \
        arm_2d_point_s32x4_t tPointLo, tPointHi;                                                           \
        ARM_ALIGN(8) uint32_t scratch32[32];                                                               \
        int16_t        *pscratch16 = (int16_t *) scratch32;                                                \
                                                                                                           \
        /* split 16-bit point vector into 2 x 32-bit vectors */                                            \
        vst1q(pscratch16, vPoint.X);                                                                       \
        tPointLo.X = vldrhq_s32(pscratch16);                                                               \
        tPointHi.X = vldrhq_s32(pscratch16 + 4);                                                           \
                                                                                                           \
        vst1q(pscratch16, vPoint.Y);                                                                       \
        tPointLo.Y = vldrhq_s32(pscratch16);                                                               \
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);                                                           \
                                                                                                           \
        /* 1st half */                                                                                     \
                                                                                                           \
        /* set vector predicate if point is inside the region */                                           \
        mve_pred16_t    p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);           \
        /* prepare vector of point offsets */                                                              \
        uint32x4_t      ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;                                    \
                                                                                                           \
        /* retrieve all point values */                                                                    \
        pointLo = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLo & p);                     \
                                                                                                           \
        /* combine 2 predicates set to true if point is in the region & values different from color mask */\
        p = vcmpneq_m_n_u32(pointLo, MaskColour, p);                                                       \
        predGlbLo |= p;                                                                                    \
        pointLo = vpselq_u32(pointLo, vTargetLo, p);                                                       \
                                                                                                           \
                                                                                                           \
        /* 2nd half */                                                                                     \
                                                                                                           \
        /* set vector predicate if point is inside the region */                                           \
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);                           \
        /* prepare vector of point offsets */                                                              \
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;                                                    \
                                                                                                           \
        /* retrieve all point values */                                                                    \
        pointHi = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHi & p);                     \
                                                                                                           \
        /* combine 2 predicates set to true if point is in the region & values different from color mask */\
        p = vcmpneq_m_n_u32(pointHi, MaskColour, p);                                                       \
        predGlbHi |= p;                                                                                    \
        pointHi = vpselq_u32(pointHi, vTargetHi, p);


#define __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT(/* inputs */                                                 \
                                                vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,       \
                                                MaskColour, vTargetLo, vTargetHi, predTailLo, predTailHi,  \
                                              /* outputs */                                                \
                                                 vPixValR, vPixValG, vPixValB, predGlbLo, predGlbHi )      \
                                                                                                           \
        uint32x4_t      pointLo, pointHi;                                                                  \
                                                                                                           \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_NOUNPK(vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,  \
                 MaskColour, vTargetLo, vTargetHi, predTailLo, predTailHi,                                 \
                                                   pointLo, pointHi, predGlbLo, predGlbHi)                 \
                                                                                                           \
        /* expand channels */                                                                              \
        vst1q(scratch32, pointLo);                                                                         \
        vst1q(scratch32 + 4, pointHi);                                                                     \
                                                                                                           \
        __arm_2d_unpack_rgb888_from_mem((uint8_t *) scratch32, &vPixValR, &vPixValG, &vPixValB);




#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__

#define __ARM2D_AVG_NEIGHBR_GRAY8_PIX(/* inputs */ \
                                      ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,         \
                                      MaskColour, vTarget, predTail,                                      \
                                      /* outputs */                                                       \
                                      predGlb, vAvgPixel)                                                 \
                                                                                                          \
    uint16x8_t      ptVal8;                                                                               \
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
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR(vXi, vYi, pOrigin, ptOrigValidRegion,           \
                                    iOrigStride, MaskColour, vTarget, predTail, ptVal8, predGlb);         \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC(vAvgPixel, ptVal8, vAreaBL);                                              \
    }                                                                                                     \
                                                                                                          \
    /* Bottom Right averaging */                                                                          \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR(vaddq_n_s16(vXi, 1), vYi, pOrigin,              \
                ptOrigValidRegion, iOrigStride, MaskColour, vTarget, predTail, ptVal8, predGlb);          \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPixel, ptVal8, vAreaBR);                                          \
    }                                                                                                     \
                                                                                                          \
    /* Top Left averaging */                                                                              \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR(vXi, vaddq_n_s16(vYi, 1), pOrigin,              \
                 ptOrigValidRegion, iOrigStride, MaskColour, vTarget, predTail, ptVal8, predGlb);         \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPixel, ptVal8, vAreaTL);                                          \
    }                                                                                                     \
                                                                                                          \
    /* Top Right averaging */                                                                             \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR(vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1),       \
            pOrigin, ptOrigValidRegion, iOrigStride, MaskColour, vTarget, predTail, ptVal8, predGlb);     \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPixel, ptVal8, vAreaTR);                                          \
    }



#define __ARM2D_AVG_NEIGHBR_RGB565_PIX(/* inputs */                                                        \
                             far, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,              \
                             MaskColour, vTarget, predTail,                                                \
                             /* outputs */                                                                 \
                             predGlb, vAvgPixelR, vAvgPixelG, vAvgPixelB)                                  \
                                                                                                           \
    uint16x8_t      R, G, B;                                                                               \
    __typeof__ (vAvgPixelR)     vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                        \
                                                                                                           \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                  \
                                                                                                           \
                                                                                                           \
    /*                                                                                                     \
     * accumulate / average over the 4 neigbouring pixels                                                  \
     */                                                                                                    \
                                                                                                           \
    /* Bottom Left averaging */                                                                            \
    {                                                                                                      \
        ARM_CONNECT2(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT, far)(vXi, vYi, pOrigin, ptOrigValidRegion,     \
        iOrigStride, MaskColour,vTarget, predTail, R, G, B, predGlb);                                      \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBL);                       \
    }                                                                                                      \
                                                                                                           \
    /* Bottom Right averaging */                                                                           \
    {                                                                                                      \
        ARM_CONNECT2(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT, far)(vaddq_n_s16(vXi, 1), vYi,                 \
             pOrigin, ptOrigValidRegion, iOrigStride, MaskColour, vTarget, predTail, R, G, B, predGlb);    \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBR);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Left averaging */                                                                               \
    {                                                                                                      \
        ARM_CONNECT2(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT, far)(vXi, vaddq_n_s16(vYi, 1),                 \
                 pOrigin, ptOrigValidRegion, iOrigStride, MaskColour, vTarget, predTail, R, G, B, predGlb);\
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTL);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Right averaging */                                                                              \
    {                                                                                                      \
        ARM_CONNECT2(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT, far)(vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1), \
                 pOrigin, ptOrigValidRegion, iOrigStride, MaskColour, vTarget, predTail, R, G, B, predGlb);\
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTR);                   \
    }


#define __ARM2D_AVG_NEIGHBR_CCCN888_PIX(/* inputs */                                                       \
                                        ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,        \
                                        MaskColour, vTargetLo, vTargetHi, predTailLow, predTailHigh,       \
                                        /* outputs */                                                      \
                                        predGlbLo, predGlbHi,                                              \
                                        vAvgPixelR, vAvgPixelG, vAvgPixelB)                                \
                                                                                                           \
    uint16x8_t      R, G, B;                                                                               \
    __typeof__ (vAvgPixelR)     vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                        \
                                                                                                           \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                  \
                                                                                                           \
    /*                                                                                                     \
     * accumulate / average over the 4 neigbouring pixels                                                  \
     */                                                                                                    \
                                                                                                           \
    /* Bottom Left averaging */                                                                            \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT(vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,           \
            MaskColour, vTargetLo, vTargetHi, predTailLow, predTailHigh, R, G, B, predGlbLo, predGlbHi);   \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBL);                       \
    }                                                                                                      \
                                                                                                           \
    /* Bottom Right averaging */                                                                           \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT(vaddq_n_s16(vXi, 1), vYi, pOrigin, ptOrigValidRegion,        \
                               iOrigStride, MaskColour, vTargetLo, vTargetHi, predTailLow, predTailHigh,   \
                                                                        R, G, B, predGlbLo, predGlbHi);    \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBR);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Left averaging */                                                                               \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT(vXi, vaddq_n_s16(vYi, 1), pOrigin, ptOrigValidRegion,        \
                               iOrigStride, MaskColour, vTargetLo, vTargetHi, predTailLow, predTailHigh,   \
                                                                        R, G, B, predGlbLo, predGlbHi);    \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTL);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Right averaging */                                                                              \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT(vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1), pOrigin,           \
                                         ptOrigValidRegion, iOrigStride, MaskColour, vTargetLo, vTargetHi, \
                                       predTailLow, predTailHigh, R, G, B, predGlbLo, predGlbHi);          \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTR);                   \
    }


#endif

#define __ARM_2D_8BIT_RGB_MIX(/* inputs / outputs */                                                       \
                            vR_InOut, vG_InOut, vB_InOut,                                                  \
                            /* inputs */                                                                   \
                            C1,                                                                            \
                            vR2, vG2, vB2, C2)                                                             \
    vR_InOut = vqaddq(vR_InOut * C1, vR2 * C2);                                                            \
    vR_InOut = vR_InOut >> 8;                                                                              \
                                                                                                           \
    vG_InOut = vqaddq(vG_InOut * C1, vG2 * C2);                                                            \
    vG_InOut = vG_InOut >> 8;                                                                              \
                                                                                                           \
    vB_InOut = vqaddq(vB_InOut * C1, vB2 * C2);                                                            \
    vB_InOut = vB_InOut >> 8;


/**
  Alpha blending of a packed vector with 3 vectors of single R, G & B channels
 */


#define __ARM_2D_BLEND_RGB565_TARGET_RGBVEC(/* inputs */                                                   \
                                            hwOpacity, vPackedTarget, vAvgR, vAvgG, vAvgB,                 \
                                            /* output */                                                   \
                                            vBlended)                                                      \
    uint16x8_t      vTargetR, vTargetG, vTargetB;                                                          \
                                                                                                           \
    __arm_2d_rgb565_unpack_single_vec(vTarget, &vTargetR, &vTargetG, &vTargetB);                           \
                                                                                                           \
    uint16_t        transp = 256 - hwOpacity;                                                              \
    /* merge */                                                                                            \
    __ARM_2D_8BIT_RGB_MIX(vAvgR, vAvgG, vAvgB, hwOpacity,                                                  \
                                    vTargetR, vTargetG, vTargetB, transp);                                 \
                                                                                                           \
    vBlended = __arm_2d_rgb565_pack_single_vec(vAvgR, vAvgG, vAvgB);




#if     __ARM_2D_HAS_HELIUM_FLOAT__                                             \
    && !__ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__

#define __CALIB             0.009f16

/**
  Scale Gray8 channel
 */
#define __ARM_2D_SCALE_GRAY8VEC(/* output */                                            \
                                vAvgPix,                                                \
                                /* inputs */                                            \
                                vPtVal, vAreaScal)                                      \
        vAvgPix = vAreaScal * vcvtq_f16_u16(vPtVal);

/**
  Scale Gray8 channel with accumulation
 */
#define __ARM_2D_SCALE_GRAY8VEC_ACC(/* input / output */                                \
                                vAvgPix,                                                \
                                /* inputs */                                            \
                                vPtVal, vAreaScal)                                      \
        vAvgPix += vAreaScal * vcvtq_f16_u16(vPtVal);


/**
  Scale R, G & B channels
 */
#define __ARM_2D_SCALE_RGBVEC(/* outputs */                                             \
                                vAvgPixelR, vAvgPixelG, vAvgPixelB,                     \
                              /* inputs */                                              \
                                R, G, B, vScal)                                         \
        vAvgPixelR = vScal * vcvtq_f16_u16(R);                                          \
        vAvgPixelG = vScal * vcvtq_f16_u16(G);                                          \
        vAvgPixelB = vScal * vcvtq_f16_u16(B);


/**
  Scale R, G & B channels with accumulation
 */

#define __ARM_2D_SCALE_RGBVEC_ACC(/* inputs / outputs */                                \
                                   vAvgPixelR, vAvgPixelG, vAvgPixelB,                  \
                                  /* inputs */                                          \
                                   R, G, B, vScal)                                      \
        vAvgPixelR += vScal * vcvtq_f16_u16(R);                                         \
        vAvgPixelG += vScal * vcvtq_f16_u16(G);                                         \
        vAvgPixelB += vScal * vcvtq_f16_u16(B);



#define __ARM_2D_GET_POINT_COORD(point)     vcvtq_s16_f16(point)
#define __ARM_2D_CONVERT_TO_PIX_TYP(x)      vcvtq_u16_f16(x)

#define PIX_VEC_TYP                         float16x8_t
#define ARM_2D_POINT_VEC                    arm_2d_point_f16x8_t
#define AREA_UNITY                          1.0f16


static
bool __arm_2d_transform_regression(arm_2d_size_t * __RESTRICT ptCopySize,
                                    arm_2d_location_t * pSrcPoint,
                                    float fAngle,
                                    float fScale,
                                    arm_2d_location_t * tOffset,
                                    arm_2d_location_t * center,
                                    int32_t             iOrigStride,
                                    arm_2d_rot_linear_regr_t regrCoefs[]
    )
{
    int32_t    iHeight = ptCopySize->iHeight;
    int32_t    iWidth = ptCopySize->iWidth;
    float           invHeight = iHeight > 1 ? 1.0f / (float) (iHeight - 1) : __LARGEINVF32;
    arm_2d_point_s32x4_t vPointCornerI;
    int32x4_t       vCornerX = { 0, 1, 0, 1 };
    int32x4_t       vCornerY = { 0, 0, 1, 1 };
    float           cosAngle = arm_cos_f32(fAngle) * fScale;
    float           sinAngle = arm_sin_f32(fAngle) * fScale;
    arm_2d_point_float_t centerf;
    float           slopeX, slopeY;
    bool            gatherLoadIdxOverflow = 0;


    centerf.fX = (float) center->iX;
    centerf.fY = (float) center->iY;

    vPointCornerI.X = vdupq_n_s32(pSrcPoint->iX + tOffset->iX);
    vPointCornerI.X = vPointCornerI.X + vmulq_n_s32(vCornerX, (iWidth - 1));

    vPointCornerI.Y = vdupq_n_s32(pSrcPoint->iY + tOffset->iY);
    vPointCornerI.Y = vPointCornerI.Y + vmulq_n_s32(vCornerY, (iHeight - 1));

    /*
        Vector version of:

        int16_t         iX = ptLocation->iX - ptCenter->iX;
        int16_t         iY = ptLocation->iY - ptCenter->iY;

        float           cosAngle = arm_cos_f32(fAngle);
        float           sinAngle = arm_sin_f32(fAngle);

        ptOutBuffer->fY = (iY * cosAngle + iX * sinAngle + ptCenter->iY);
        ptOutBuffer->fX = (-iY * sinAngle + iX * cosAngle + ptCenter->iX);
    */

    arm_2d_point_f32x4_t vTmp, vPointCornerF;

    vTmp.X = vsubq_n_f32(vcvtq_f32_s32(vPointCornerI.X), centerf.fX);
    vTmp.Y = vsubq_n_f32(vcvtq_f32_s32(vPointCornerI.Y), centerf.fY);

    vPointCornerF.X = vmulq_n_f32(vTmp.X, cosAngle) - vmulq_n_f32(vTmp.Y, sinAngle);
    vPointCornerF.X = vaddq_n_f32(vPointCornerF.X, centerf.fX);

    vPointCornerF.Y = vmulq_n_f32(vTmp.X, sinAngle) + vmulq_n_f32(vTmp.Y, cosAngle);
    vPointCornerF.Y = vaddq_n_f32(vPointCornerF.Y, centerf.fY);

    /*
       Check whether rotated index offsets could exceed 16-bit limits
       used in subsequent gather loads
       This will occur for parts of large images (e.g. 320*200)
       To avoid unconditional penalties for small/medium images,
       returns a speculative overflow allowing to handle large offsets.
    */
    float32_t maxY = vmaxnmvq(0.0f, vPointCornerF.Y);

    if((iOrigStride * maxY) > (float)(UINT16_MAX))
        gatherLoadIdxOverflow = true;


    /* interpolation in Y direction for 1st elements column */
    slopeX = (vPointCornerF.X[2] - vPointCornerF.X[0]) * invHeight;
    slopeY = (vPointCornerF.Y[2] - vPointCornerF.Y[0]) * invHeight;

    regrCoefs[0].slopeY = slopeY;
    regrCoefs[0].slopeX = slopeX;
    regrCoefs[0].interceptY = vPointCornerF.Y[0];
    regrCoefs[0].interceptX = vPointCornerF.X[0];


    /* interpolation in Y direction for the last elements column */
    slopeX = (vPointCornerF.X[3] - vPointCornerF.X[1]) * invHeight;
    slopeY = (vPointCornerF.Y[3] - vPointCornerF.Y[1]) * invHeight;

    regrCoefs[1].slopeY = slopeY;
    regrCoefs[1].slopeX = slopeX;
    regrCoefs[1].interceptY = vPointCornerF.Y[1];
    regrCoefs[1].interceptX = vPointCornerF.X[1];

    return gatherLoadIdxOverflow;
}

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__

#define __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint,                        \
                                           vAreaTR, vAreaTL, vAreaBR, vAreaBL)  \
    float16x8_t     vOne = vdupq_n_f16(1.0f);                                   \
                                                                                \
    vXi = vsubq_m_n_s16(vXi, vXi, 1, vcmpltq_n_f16(ptPoint->X, 0));             \
    vYi = vsubq_m_n_s16(vYi, vYi, 1, vcmpltq_n_f16(ptPoint->Y, 0));             \
                                                                                \
    float16x8_t     vWX = ptPoint->X - vcvtq_f16_s16(vXi);                      \
    float16x8_t     vWY = ptPoint->Y - vcvtq_f16_s16(vYi);                      \
                                                                                \
    /* combination of Bottom / Top & Left / Right areas contributions */        \
    vAreaTR = vWX * vWY;                                                        \
    vAreaTL = (vOne - vWX) * vWY;                                               \
    vAreaBR = vWX * (vOne - vWY);                                               \
    vAreaBL = (vOne - vWX) * (vOne - vWY);

#endif


#else /* __ARM_2D_HAS_HELIUM_FLOAT__ && ! __ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__ */

/* extra calibration removed in fixed-point code since offset is lower than Q9.6 representation */

#define ONE_BY_2PI_Q31      341782637.0f
#define ONE_Q16             UINT16_MAX
#define ARSHIFT(x, shift)   (shift > 0 ? x >> shift : x << (-shift))
#define TO_Q16(x)           ((x) << 16)
#define GET_Q6INT(x)        ((x) >> 6)
#define SET_Q6INT(x)        ((x) << 6)

/**
  Scale Gray8 channel
 */
#define __ARM_2D_SCALE_GRAY8VEC(/* output */                                            \
                                vAvgPix,                                                \
                                /* inputs */                                            \
                                vPtVal, vAreaScal)                                      \
        vAvgPix = vrmulhq_u16(vAreaScal, vPtVal);

/**
  Scale Gray8 channel with accumulation
 */
#define __ARM_2D_SCALE_GRAY8VEC_ACC(/* input / output */                                \
                                vAvgPix,                                                \
                                /* inputs */                                            \
                                vPtVal, vAreaScal)                                      \
        vAvgPix = vqaddq(vAvgPix, vrmulhq_u16(vAreaScal, vPtVal));


/**
  Scale R, G & B channels
 */
#define __ARM_2D_SCALE_RGBVEC(/* outputs */                                             \
                                vAvgPixelR, vAvgPixelG, vAvgPixelB,                     \
                              /* inputs */                                              \
                                R, G, B, vScal)                                         \
        vAvgPixelR = vrmulhq_u16(vScal, R);                                             \
        vAvgPixelG = vrmulhq_u16(vScal, G);                                             \
        vAvgPixelB = vrmulhq_u16(vScal, B);

/**
  Scale R, G & B channels with accumulation
 */
#define __ARM_2D_SCALE_RGBVEC_ACC(/* inputs / outputs */                                \
                                   vAvgPixelR, vAvgPixelG, vAvgPixelB,                  \
                                  /* inputs */                                          \
                                   R, G, B, vScal)                                      \
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vScal, R));                         \
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vScal, G));                         \
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vScal, B));



#define __ARM_2D_GET_POINT_COORD(point)     GET_Q6INT(point)
#define __ARM_2D_CONVERT_TO_PIX_TYP(x)      (x)

#define PIX_VEC_TYP                         uint16x8_t
#define ARM_2D_POINT_VEC                    arm_2d_point_s16x8_t
#define AREA_UNITY                          vdupq_n_u16(ONE_Q16)


#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__


#define __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint,                                \
                                           vAreaTR, vAreaTL, vAreaBR, vAreaBL)          \
    int16x8_t       vOne = vdupq_n_s16(SET_Q6INT(1));                                   \
                                                                                        \
    /*vXi = vsubq_m_n_s16(vXi, vXi, 1, vcmpltq_n_s16(ptPoint->X, 0));  */               \
    /*vYi = vsubq_m_n_s16(vYi, vYi, 1, vcmpltq_n_s16(ptPoint->Y, 0));  */               \
                                                                                        \
    int16x8_t       vWX = ptPoint->X - SET_Q6INT(vXi);                                  \
    int16x8_t       vWY = ptPoint->Y - SET_Q6INT(vYi);                                  \
                                                                                        \
    /* combination of Bottom / Top & Left / Right areas contributions */                \
    vAreaTR = vmulq_u16(vWX, vWY);                                                      \
    vAreaTL = vmulq_u16((vOne - vWX), vWY);                                             \
    vAreaBR = vmulq_u16(vWX, (vOne - vWY));                                             \
    vAreaBL = vmulq_u16((vOne - vWX), (vOne - vWY));                                    \
                                                                                        \
    /* Q16 conversion */                                                                \
    vAreaTR = vqshlq_n_u16(vAreaTR, 4);                                                 \
    vAreaTL = vqshlq_n_u16(vAreaTL, 4);                                                 \
    vAreaBR = vqshlq_n_u16(vAreaBR, 4);                                                 \
    vAreaBL = vqshlq_n_u16(vAreaBL, 4);


#endif

static
bool __arm_2d_transform_regression(arm_2d_size_t * __RESTRICT ptCopySize,
                                            arm_2d_location_t * pSrcPoint,
                                            float fAngle,
                                            float fScale,
                                            arm_2d_location_t * tOffset,
                                            arm_2d_location_t * center,
                                            int32_t             iOrigStride,
                                            arm_2d_rot_linear_regr_t regrCoefs[]
    )
{
    int_fast16_t        iHeight = ptCopySize->iHeight;
    int_fast16_t        iWidth = ptCopySize->iWidth;
    q31_t               invHeightFx = iHeight > 1 ? INT32_MAX / (iHeight - 1) : INT32_MAX;
    int32_t             AngleFx = lroundf(fAngle * ONE_BY_2PI_Q31);
    int32_t             ScaleFx = (int32_t)((float)fScale * (float)TO_Q16(1));
    q31_t               cosAngleFx = MULTFX(arm_cos_q31(AngleFx), ScaleFx);
    q31_t               sinAngleFx = MULTFX(arm_sin_q31(AngleFx), ScaleFx);
    arm_2d_point_fx_t   tPointCornerFx[2][2];
    arm_2d_point_fx_t   centerQ16;
    arm_2d_point_fx_t   srcPointQ16;
    arm_2d_point_fx_t   tOffsetQ16;
    arm_2d_point_fx_t   tmp;
    int32_t             iXQ16, iYQ16;
    bool                gatherLoadIdxOverflow = 0;

    /* Q16 conversion */
    centerQ16.X = TO_Q16(center->iX);
    centerQ16.Y = TO_Q16(center->iY);

    srcPointQ16.X = TO_Q16(pSrcPoint->iX);
    srcPointQ16.Y = TO_Q16(pSrcPoint->iY);

    tOffsetQ16.X = TO_Q16(tOffset->iX);
    tOffsetQ16.Y = TO_Q16(tOffset->iY);


    /* (0,0) corner */
    tmp.X = srcPointQ16.X + 0 + tOffsetQ16.X;
    tmp.Y = srcPointQ16.Y + 0 + tOffsetQ16.Y;

    iXQ16 = tmp.X - centerQ16.X;
    iYQ16 = tmp.Y - centerQ16.Y;

    tPointCornerFx[0][0].Y =
        __QDADD(__QDADD(centerQ16.Y, MUL_Q16(iYQ16, cosAngleFx)),
                MUL_Q16(iXQ16, sinAngleFx));
    tPointCornerFx[0][0].X =
        __QDSUB(__QDADD(centerQ16.X, MUL_Q16(iXQ16, cosAngleFx)),
                MUL_Q16(iYQ16, sinAngleFx));


    /* ((iWidth - 1),0) corner */
    tmp.X = srcPointQ16.X + 0 + tOffsetQ16.X + TO_Q16(iWidth - 1);
    iXQ16 = tmp.X - centerQ16.X;

    tPointCornerFx[1][0].Y =
        __QDADD(__QDADD(centerQ16.Y, MUL_Q16(iYQ16, cosAngleFx)),
                MUL_Q16(iXQ16, sinAngleFx));
    tPointCornerFx[1][0].X =
        __QDSUB(__QDADD(centerQ16.X, MUL_Q16(iXQ16, cosAngleFx)),
                MUL_Q16(iYQ16, sinAngleFx));


    /* ((iWidth - 1),(iHeight - 1)) corner */
    tmp.Y = srcPointQ16.Y + tOffsetQ16.Y + TO_Q16(iHeight - 1);
    iYQ16 = tmp.Y - centerQ16.Y;

    tPointCornerFx[1][1].Y =
        __QDADD(__QDADD(centerQ16.Y, MUL_Q16(iYQ16, cosAngleFx)),
                MUL_Q16(iXQ16, sinAngleFx));
    tPointCornerFx[1][1].X =
        __QDSUB(__QDADD(centerQ16.X, MUL_Q16(iXQ16, cosAngleFx)),
                MUL_Q16(iYQ16, sinAngleFx));


    /* (0,(iHeight - 1)) corner */
    tmp.X = srcPointQ16.X + 0 + tOffsetQ16.X;
    iXQ16 = tmp.X - centerQ16.X;

    tPointCornerFx[0][1].Y =
        __QDADD(__QDADD(centerQ16.Y, MUL_Q16(iYQ16, cosAngleFx)),
                MUL_Q16(iXQ16, sinAngleFx));
    tPointCornerFx[0][1].X =
        __QDSUB(__QDADD(centerQ16.X, MUL_Q16(iXQ16, cosAngleFx)),
                MUL_Q16(iYQ16, sinAngleFx));

    /*
       Check whether rotated index offsets could exceed 16-bit limits
       used in subsequent gather loads
       This will occur for parts of large images (e.g. 320*200)
       To avoid unconditional penalties for small/medium images,
       returns a speculative overflow allowing to handle large offsets.
    */
    int32_t maxY = MAX(MAX
                        (MAX(tPointCornerFx[0][0].Y, tPointCornerFx[0][1].Y),
                            tPointCornerFx[1][0].Y),
                                tPointCornerFx[1][1].Y);

    if(MULTFX(TO_Q16(iOrigStride), maxY) > UINT16_MAX)
        gatherLoadIdxOverflow = true;


    /* regression */
    int32_t           slopeXFx, slopeYFx;

    /* interpolation in Y direction for 1st elements column */
    slopeXFx = MULTFX((tPointCornerFx[0][1].X - tPointCornerFx[0][0].X), invHeightFx);
    slopeYFx = MULTFX((tPointCornerFx[0][1].Y - tPointCornerFx[0][0].Y), invHeightFx);

    regrCoefs[0].slopeY = slopeYFx * 2;
    regrCoefs[0].slopeX = slopeXFx * 2;
    regrCoefs[0].interceptY = tPointCornerFx[0][0].Y;
    regrCoefs[0].interceptX = tPointCornerFx[0][0].X;


    /* interpolation in Y direction for the last elements column */
    slopeXFx = MULTFX((tPointCornerFx[1][1].X - tPointCornerFx[1][0].X), invHeightFx);
    slopeYFx = MULTFX((tPointCornerFx[1][1].Y - tPointCornerFx[1][0].Y), invHeightFx);

    regrCoefs[1].slopeY = slopeYFx* 2;
    regrCoefs[1].slopeX = slopeXFx* 2;
    regrCoefs[1].interceptY = tPointCornerFx[1][0].Y;
    regrCoefs[1].interceptX = tPointCornerFx[1][0].X;

    return gatherLoadIdxOverflow;
}
#endif // __ARM_2D_HAS_HELIUM_FLOAT__



static
void __MVE_WRAPPER( __arm_2d_impl_gray8_get_pixel_colour)(ARM_2D_POINT_VEC * ptPoint,
                                          arm_2d_region_t * ptOrigValidRegion,
                                          uint8_t * pOrigin,
                                          int16_t iOrigStride,
                                          uint8_t * pTarget, uint8_t MaskColour, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_u16(pTarget);

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;
    /* accumulated pixel vectors */
    uint16x8_t      vDstPixel;


#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX(ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                                      MaskColour, vTarget, predTail, predGlb, vAvgPixel);

        vDstPixel = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

        /* saturate to 8-bit */
        vDstPixel = vminq(vDstPixel, vdupq_n_u16(255));
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR(vXi, vYi, pOrigin, ptOrigValidRegion,
                                                          iOrigStride, MaskColour, vTarget, predTail,
                                                          vDstPixel, predGlb);
    }
#endif

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vDstPixel, vTarget, predGlb);

    vstrbq_p_u16(pTarget, vTarget, predTail);
}




static
void __MVE_WRAPPER( __arm_2d_impl_gray8_get_pixel_colour_with_alpha)(ARM_2D_POINT_VEC * ptPoint,
                                                     arm_2d_region_t * ptOrigValidRegion,
                                                     uint8_t * pOrigin,
                                                     int16_t iOrigStride,
                                                     uint8_t * pTarget,
                                                     uint8_t MaskColour,
                                                     uint_fast16_t hwOpacity, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_u16(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vDstPixel;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX(ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                                      MaskColour, vTarget, predTail, predGlb, vAvgPixel);

        vDstPixel = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR(vXi, vYi, pOrigin, ptOrigValidRegion,
                                                          iOrigStride, MaskColour, vTarget, predTail,
                                                          vDstPixel, predGlb);
    }
#endif

    /* blending */
    uint16_t        hwTransparency = 256 - hwOpacity;
    uint16x8_t      vBlended =
        vqaddq(vDstPixel * (uint16_t) hwOpacity, vTarget * hwTransparency) >> 8;

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vstrbq_p_u16(pTarget, vTarget, predTail);
}


static 
void __MVE_WRAPPER( __arm_2d_impl_rgb565_get_pixel_colour)(ARM_2D_POINT_VEC * ptPoint,
                                           arm_2d_region_t * ptOrigValidRegion,
                                           uint16_t * pOrigin,
                                           int16_t iOrigStride,
                                           uint16_t * pTarget, uint16_t MaskColour, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vDstPixel;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX(NEAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                                       iOrigStride, MaskColour, vTarget, predTail, predGlb, vAvgPixelR,
                                       vAvgPixelG, vAvgPixelB);

        /* pack */
        vDstPixel = __arm_2d_rgb565_pack_single_vec(__ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR),
                                                            __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG),
                                                            __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB));
    }
#else
    {

        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK(vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                                        MaskColour, vTarget, predTail, vDstPixel, predGlb );
    }
#endif

    vTarget = vpselq_u16(vDstPixel, vTarget, predGlb);

    /* update target pixels */
    vst1q_p(pTarget, vTarget, predTail);
}




static
void __MVE_WRAPPER( __arm_2d_impl_rgb565_get_pixel_colour_offs_compensated)(ARM_2D_POINT_VEC * ptPoint,
                                           arm_2d_region_t * ptOrigValidRegion,
                                           uint16_t * pOrigin,
                                           int16_t iOrigStride,
                                           uint16_t * pTarget, uint16_t MaskColour, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vDstPixel;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX(FAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                                       iOrigStride, MaskColour, vTarget, predTail, predGlb, vAvgPixelR,
                                       vAvgPixelG, vAvgPixelB);

        /* pack */
        vDstPixel = __arm_2d_rgb565_pack_single_vec(__ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR),
                                                            __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG),
                                                            __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB));
    }
#else
    {

        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_FAR(vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                                        MaskColour, vTarget, predTail, vDstPixel, predGlb);
    }
#endif

    vTarget = vpselq_u16(vDstPixel, vTarget, predGlb);

    /* update target pixels */
    vst1q_p(pTarget, vTarget, predTail);
}



static
void __MVE_WRAPPER( __arm_2d_impl_rgb565_get_pixel_colour_with_alpha)(ARM_2D_POINT_VEC * ptPoint,
                                                      arm_2d_region_t * ptOrigValidRegion,
                                                      uint16_t * pOrigin,
                                                      int16_t iOrigStride,
                                                      uint16_t * pTarget,
                                                      uint16_t MaskColour,
                                                      uint_fast16_t hwOpacity, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vAvgR, vAvgG, vAvgB;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX(NEAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                                       iOrigStride, MaskColour, vTarget, predTail, predGlb, vAvgPixelR,
                                       vAvgPixelG, vAvgPixelB);

        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {

        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT(vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                                        MaskColour, vTarget, predTail, vAvgR, vAvgG, vAvgB, predGlb );
    }
#endif
    /* blending */

    uint16x8_t      vBlended;

    __ARM_2D_BLEND_RGB565_TARGET_RGBVEC((uint16_t) hwOpacity, vTarget, vAvgR, vAvgG, vAvgB,
                                        vBlended);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(pTarget, vTarget, predTail);
}


static
void __MVE_WRAPPER( __arm_2d_impl_rgb565_get_pixel_colour_with_alpha_offs_compensated)(ARM_2D_POINT_VEC * ptPoint,
                                                      arm_2d_region_t * ptOrigValidRegion,
                                                      uint16_t * pOrigin,
                                                      int16_t iOrigStride,
                                                      uint16_t * pTarget,
                                                      uint16_t MaskColour,
                                                      uint_fast16_t hwOpacity, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vAvgR, vAvgG, vAvgB;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX(FAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                                       iOrigStride, MaskColour, vTarget, predTail, predGlb, vAvgPixelR,
                                       vAvgPixelG, vAvgPixelB);

        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_FAR(vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                                        MaskColour, vTarget, predTail, vAvgR, vAvgG, vAvgB, predGlb );
    }
#endif
    /* blending */

    uint16x8_t      vBlended;

    __ARM_2D_BLEND_RGB565_TARGET_RGBVEC((uint16_t) hwOpacity, vTarget, vAvgR, vAvgG, vAvgB,
                                        vBlended);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(pTarget, vTarget, predTail);
}




static
void __MVE_WRAPPER( __arm_2d_impl_cccn888_get_pixel_colour)(ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint32_t * pOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pTarget, uint32_t MaskColour, int16_t elts)
{

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    uint32x4_t      vTargetLo = vld1q(pTarget);
    uint32x4_t      vTargetHi = vld1q(pTarget + 4);
    mve_pred16_t    predTailLow = vctp32q(elts);
    mve_pred16_t    predTailHigh = elts - 4 > 0 ? vctp32q(elts - 4) : 0;

    /* predicate accumulators */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlbLo = 0, predGlbHi = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;


        __ARM2D_AVG_NEIGHBR_CCCN888_PIX(ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                                        MaskColour, vTargetLo, vTargetHi, predTailLow, predTailHigh,
                                        predGlbLo, predGlbHi,
                                        vAvgPixelR, vAvgPixelG, vAvgPixelB);

        {
            ARM_ALIGN(8)    uint32_t scratch32[32];
            /* pack */
            __arm_2d_pack_rgb888_to_mem((uint8_t *) scratch32, __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR),
                                        __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG), __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB));

            uint32x4_t      TempPixel = vld1q(scratch32);

            /* select between target pixel, averaged pixed */
            TempPixel = vpselq_u32(TempPixel, vTargetLo, predGlbLo);

            vst1q_p(pTarget, TempPixel, predTailLow);

            TempPixel = vld1q(scratch32 + 4);

            /* select between target pixel, averaged pixed */
            TempPixel = vpselq_u32(TempPixel, vTargetHi, predGlbHi);

            vst1q_p(pTarget + 4, TempPixel, predTailHigh);
        }
    }
#else
    {
        uint32x4_t      pointLo, pointHi;

        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_NOUNPK(vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
            MaskColour, vTargetLo, vTargetHi, predTailLow, predTailHigh, pointLo, pointHi, predGlbLo, predGlbHi);

        uint32x4_t vPixel = vpselq_u32(pointLo, vTargetLo, predGlbLo);

        vst1q_p(pTarget, vPixel, predTailLow);

        vPixel = vpselq_u32(pointHi, vTargetHi, predGlbHi);

        vst1q_p(pTarget + 4, vPixel, predTailHigh);
    }
#endif
}



static
void __MVE_WRAPPER( __arm_2d_impl_cccn888_get_pixel_colour_with_alpha)(ARM_2D_POINT_VEC * ptPoint,
                                                       arm_2d_region_t * ptOrigValidRegion,
                                                       uint32_t * pOrigin,
                                                       int16_t iOrigStride,
                                                       uint32_t * pTarget,
                                                       uint32_t MaskColour,
                                                       uint_fast16_t hwOpacity, int16_t elts)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    uint32x4_t      vTargetLo = vld1q(pTarget);
    uint32x4_t      vTargetHi = vld1q(pTarget + 4);
    mve_pred16_t    predTailLow = vctp32q(elts);
    mve_pred16_t    predTailHigh = elts - 4 > 0 ? vctp32q(elts - 4) : 0;
    uint16x8_t      vAvgR, vAvgG, vAvgB;


    /* predicate accumulators */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlbLo = 0, predGlbHi = 0;


#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)  &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_CCCN888_PIX(ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                                        MaskColour, vTargetLo, vTargetHi, predTailLow, predTailHigh,
                                        predGlbLo, predGlbHi,
                                        vAvgPixelR, vAvgPixelG,
                                        vAvgPixelB);
        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {
        uint16x8_t      R, G, B;

        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT(vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride, MaskColour, vTargetLo,
                                              vTargetHi, predTailLow, predTailHigh, R, G, B, predGlbLo, predGlbHi);

        vAvgR = R;
        vAvgG = G;
        vAvgB = B;
    }
#endif


    /* alpha blending */
    uint16x8_t      vTargetR, vTargetG, vTargetB;
    uint16_t        transp = 256 - hwOpacity;
    ARM_ALIGN(8)    uint32_t scratch32[32];

    __arm_2d_unpack_rgb888_from_mem((const uint8_t *) pTarget, &vTargetR, &vTargetG, &vTargetB);

    /* merge */
    __ARM_2D_8BIT_RGB_MIX(vAvgR, vAvgG, vAvgB, (uint16_t) hwOpacity,
                                    vTargetR, vTargetG, vTargetB, transp);

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


/*
 * Src mask variants expansion
 *
 * - transform_with_src_chn_mask_and_opacity
 * - transform_with_src_mask_and_opacity
 * - transform_with_src_chn_mask
 * - transform_with_src_mask
 */


#define __API_INT_TYPE_BIT_NUM      8
#define __API_COLOUR                ARM_2D_M_COLOUR_GRAY8
#define __API_COLOUR_NAME           gray8


#include "__arm_2d_transform_helium.inc"

#define __API_INT_TYPE_BIT_NUM      16
#define __API_COLOUR                ARM_2D_M_COLOUR_RGB565
#define __API_COLOUR_NAME           rgb565

#include "__arm_2d_transform_helium.inc"

#define __API_INT_TYPE_BIT_NUM      32
#define __API_COLOUR                ARM_2D_M_COLOUR_CCCN888
#define __API_COLOUR_NAME           cccn888

#include "__arm_2d_transform_helium.inc"


#ifdef   __cplusplus
}
#endif

#endif // __ARM_2D_HAS_HELIUM__

#endif // __ARM_2D_COMPILATION_UNIT
