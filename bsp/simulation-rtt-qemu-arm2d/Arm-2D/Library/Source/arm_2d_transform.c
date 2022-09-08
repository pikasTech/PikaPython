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
 * Title:        arm-2d_transform.c
 * Description:  APIs for tile transform
 *
 * $Date:        20 May 2022
 * $Revision:    V.1.0.3
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */


/*============================ INCLUDES ======================================*/
#define __ARM_2D_IMPL__

#include "arm_2d.h"
#include "__arm_2d_impl.h"
#include "math.h"


#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
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
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wfloat-conversion"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wundef"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144,1296
#elif defined(__IS_COMPILER_IAR__)
#   pragma diag_suppress=Pa093
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#endif

#include <arm_math.h>

/*============================ MACROS ========================================*/
#undef __PI
#define __PI                3.1415926f

#define __CALIB             0.009f
/* faster ATAN */
#define FAST_ATAN_F32_1(x, xabs)    \
                            (x * (PI / 4.0f) + 0.273f * x * (1.0f - xabs))
#define EPS_ATAN2           1e-5f


#define TO_Q16(x)           ((int32_t)(x) << 16)

/*----------------------------------------------------------------------------*
 * Code Template                                                              *
 *----------------------------------------------------------------------------*/

#define __API_PIXEL_AVERAGE_RESULT_GRAY8()                      \
    (   tPixel >> 8)

#define __API_PIXEL_AVERAGE_RESULT_RGB565()                     \
    (   tPixel.R >>= 8,                                         \
        tPixel.G >>= 8,                                         \
        tPixel.B >>= 8,                                         \
        __arm_2d_rgb565_pack(&tPixel))

#define __API_PIXEL_AVERAGE_RESULT_CCCN888()                    \
    (   tPixel.R >>= 8,                                         \
        tPixel.G >>= 8,                                         \
        tPixel.B >>= 8,                                         \
        tPixel.A = *((uint8_t *)pTarget + 3),                   \
        __arm_2d_cccn888_pack(&tPixel))


#define __API_COLOUR                gray8
#define __API_INT_TYPE              uint8_t
#define __API_INT_TYPE_BIT_NUM      16
#define __API_PIXEL_AVERAGE_INIT()  uint16_t tPixel = 0;
#define __API_PIXEL_BLENDING        __ARM_2D_PIXEL_BLENDING_OPA_GRAY8
#define __API_PIXEL_AVERAGE         __ARM_2D_PIXEL_AVERAGE_GRAY8
#define __API_PIXEL_AVERAGE_RESULT  __API_PIXEL_AVERAGE_RESULT_GRAY8
#include "__arm_2d_transform.inc"

#define __API_COLOUR                rgb565
#define __API_INT_TYPE              uint16_t
#define __API_INT_TYPE_BIT_NUM      16
#define __API_PIXEL_BLENDING        __ARM_2D_PIXEL_BLENDING_OPA_RGB565
#define __API_PIXEL_AVERAGE         __ARM_2D_PIXEL_AVERAGE_RGB565
#define __API_PIXEL_AVERAGE_RESULT  __API_PIXEL_AVERAGE_RESULT_RGB565
#include "__arm_2d_transform.inc"

#define __API_COLOUR                cccn888
#define __API_INT_TYPE              uint32_t
#define __API_INT_TYPE_BIT_NUM      32
#define __API_PIXEL_BLENDING        __ARM_2D_PIXEL_BLENDING_OPA_CCCN888
#define __API_PIXEL_AVERAGE         __ARM_2D_PIXEL_AVERAGE_CCCN888
#define __API_PIXEL_AVERAGE_RESULT  __API_PIXEL_AVERAGE_RESULT_CCCN888

#include "__arm_2d_transform.inc"

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * Utilities                                                                  *
 *----------------------------------------------------------------------------*/
__arm_2d_point_adj_alpha_t
__arm_2d_point_get_adjacent_alpha_fp(arm_2d_point_float_t *ptPoint)
{
    assert(NULL != ptPoint);
    float x = ptPoint->fX - (int32_t)ptPoint->fX;
    float y = ptPoint->fY - (int32_t)ptPoint->fY;

    int16_t iXSign = x < 0 ;
    int16_t iYSign = y < 0 ;

    __arm_2d_point_adj_alpha_t tResult = {
        .tMatrix = {
            [0] = {
                .tOffset = {
                    .iX = -iXSign,
                    .iY = -iYSign,
                },
            #if 0
                .chAlpha = (uint8_t)(
                                ((float)(1-iXSign)  - (float)x)     //!< x
                            *   ((float)(1-iYSign)  - (float)y)     //!< y
                            *   256.0f
                            ),
            #endif
            },
            [1] = {
                .tOffset = {
                    .iX = -iXSign + 1,
                    .iY = -iYSign,
                },
                .chAlpha = (uint8_t)(
                                ((float)iXSign      + (float)x)     //!< x
                            *   ((float)(1-iYSign)  - (float)y)     //!< y
                            *   256.0f
                            ),
            },
            [2] = {
                .tOffset = {
                    .iX = -iXSign,
                    .iY = -iYSign + 1,
                },
                .chAlpha = (uint8_t)(
                                ((float)(1-iXSign)  - (float)x)     //!< x
                            *   ((float)iYSign      + (float)y)     //!< y
                            *   256.0f
                            ),
            },
            [3] = {
                .tOffset = {
                    .iX = -iXSign + 1,
                    .iY = -iYSign +1,
                },
                .chAlpha = (uint8_t)(
                                ((float)iXSign      + (float)x)     //!< x
                            *   ((float)iYSign      + (float)y)     //!< y
                            *   256.0f
                            ),
            },
        },
    };
#if 1
    tResult.tMatrix[0].chAlpha  = 256
                                - tResult.tMatrix[1].chAlpha
                                - tResult.tMatrix[2].chAlpha
                                - tResult.tMatrix[3].chAlpha;
#endif
    return tResult;
}

__arm_2d_point_adj_alpha_t
__arm_2d_point_get_adjacent_alpha_q16(arm_2d_point_fx_t *ptPoint)
{
    assert(NULL != ptPoint);
    int32_t x = ptPoint->X  & 0xFFFF;
    int32_t y = ptPoint->Y  & 0xFFFF;

    x |= ((ptPoint->X < 0) * 0xFFFF0000);
    y |= ((ptPoint->Y < 0) * 0xFFFF0000);

    int_fast16_t iXSign = x < 0;// ? 1 : 0;
    int_fast16_t iYSign = y < 0;// ? 1 : 0;

    __arm_2d_point_adj_alpha_t tResult = {
        .tMatrix = {
            [0] = {
                .tOffset = {
                    .iX = -iXSign,
                    .iY = -iYSign,
                },
            #if 0
                .chAlpha = (uint8_t)__USAT(
             MUL_Q16(MUL_Q16(   (TO_Q16(1-iXSign)   - x)        //!< x
                            ,   (TO_Q16(1-iYSign)   - y))       //!< y
                            ,   TO_Q16(256)
                            ) >> 16, 8),
            #endif
            },
            [1] = {
                .tOffset = {
                    .iX = -iXSign + 1,
                    .iY = -iYSign,
                },
                .chAlpha = (uint8_t)__USAT(
             MUL_Q16(MUL_Q16(   (TO_Q16(iXSign)     + x)        //!< x
                            ,   (TO_Q16(1-iYSign)   - y))       //!< y
                            ,   TO_Q16(256)
                            ) >> 16, 8),
            },
            [2] = {
                .tOffset = {
                    .iX = -iXSign,
                    .iY = -iYSign + 1,
                },
                .chAlpha = (uint8_t)__USAT(
             MUL_Q16(MUL_Q16(   (TO_Q16(1-iXSign)   - x)        //!< x
                            ,   (TO_Q16(iYSign)     + y))       //!< y
                            ,   TO_Q16(256)
                            ) >> 16, 8),
            },
            [3] = {
                .tOffset = {
                    .iX = -iXSign + 1,
                    .iY = -iYSign +1,
                },
                .chAlpha = (uint8_t)__USAT(
             MUL_Q16(MUL_Q16(   (TO_Q16(iXSign)     + x)        //!< x
                            ,   (TO_Q16(iYSign)     + y))       //!< y
                            ,   TO_Q16(256)
                            ) >> 16, 8),
            },
        },
    };
#if 1
    tResult.tMatrix[0].chAlpha  = 256
                                - tResult.tMatrix[1].chAlpha
                                - tResult.tMatrix[2].chAlpha
                                - tResult.tMatrix[3].chAlpha;
#endif

    return tResult;
}



#if __ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__

static
void __arm_2d_transform_regression(arm_2d_size_t * __RESTRICT ptCopySize,
                                arm_2d_location_t * pSrcPoint,
                                float fAngle,
                                float fScale,
                                arm_2d_location_t * tOffset,
                                arm_2d_location_t * center,
                                arm_2d_rot_linear_regr_t regrCoefs[])
{
#define ONE_BY_2PI_Q31      341782637.0f
//#define TO_Q16(x)           ((x) << 16)

    int_fast16_t        iHeight = ptCopySize->iHeight;
    int_fast16_t        iWidth = ptCopySize->iWidth;
    q31_t               invHeightFx;

    if (1 == iHeight) {
        invHeightFx = 0x7fffffff;
    } else {
        invHeightFx = 0x7fffffff / (iHeight - 1);
    }

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


    /* regression */
    int32_t           slopeXFx, slopeYFx;

    /* interpolation in Y direction for 1st elements column */
    slopeXFx = MULTFX(  (tPointCornerFx[0][1].X - tPointCornerFx[0][0].X),
                        invHeightFx);
    slopeYFx = MULTFX((tPointCornerFx[0][1].Y - tPointCornerFx[0][0].Y),
                        invHeightFx);

    regrCoefs[0].slopeY = slopeYFx * 2;
    regrCoefs[0].slopeX = slopeXFx * 2;
    regrCoefs[0].interceptY = tPointCornerFx[0][0].Y;
    regrCoefs[0].interceptX = tPointCornerFx[0][0].X;


    /* interpolation in Y direction for the last elements column */
    slopeXFx = MULTFX(  (tPointCornerFx[1][1].X - tPointCornerFx[1][0].X),
                        invHeightFx);
    slopeYFx = MULTFX(  (tPointCornerFx[1][1].Y - tPointCornerFx[1][0].Y),
                        invHeightFx);

    regrCoefs[1].slopeY = slopeYFx* 2;
    regrCoefs[1].slopeX = slopeXFx* 2;
    regrCoefs[1].interceptY = tPointCornerFx[1][0].Y;
    regrCoefs[1].interceptX = tPointCornerFx[1][0].X;
}

#else

static
void __arm_2d_transform_regression(arm_2d_size_t * __RESTRICT ptCopySize,
                                arm_2d_location_t * pSrcPoint,
                                float fAngle,
                                float fScale,
                                arm_2d_location_t * tOffset,
                                arm_2d_location_t * ptCenter,
                                arm_2d_rot_linear_regr_t regrCoefs[])
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    float           invHeight;

    if (1 == iHeight ) {
        invHeight = __LARGEINVF32;
    } else {
        invHeight = 1.0f / (float) (iHeight - 1);
    }

    float           cosAngle = arm_cos_f32(fAngle) * fScale;
    float           sinAngle = arm_sin_f32(fAngle) * fScale;
    arm_2d_location_t tSrcPoint;
    arm_2d_point_float_t tPointCorner[2][2];
    int16_t         iX, iY;

    tSrcPoint.iX = pSrcPoint->iX + 0 + tOffset->iX;
    tSrcPoint.iY = pSrcPoint->iY + 0 + tOffset->iY;

    iX = tSrcPoint.iX - ptCenter->iX;
    iY = tSrcPoint.iY - ptCenter->iY;

    tPointCorner[0][0].fY = (iY * cosAngle + iX * sinAngle + ptCenter->iY);
    tPointCorner[0][0].fX = (-iY * sinAngle + iX * cosAngle + ptCenter->iX);

    tSrcPoint.iX = pSrcPoint->iX + (iWidth - 1) + tOffset->iX;
    iX = tSrcPoint.iX - ptCenter->iX;

    tPointCorner[1][0].fY = (iY * cosAngle + iX * sinAngle + ptCenter->iY);
    tPointCorner[1][0].fX = (-iY * sinAngle + iX * cosAngle + ptCenter->iX);

    tSrcPoint.iY = pSrcPoint->iY + (iHeight - 1) + tOffset->iY;
    iY = tSrcPoint.iY - ptCenter->iY;

    tPointCorner[1][1].fY = (iY * cosAngle + iX * sinAngle + ptCenter->iY);
    tPointCorner[1][1].fX = (-iY * sinAngle + iX * cosAngle + ptCenter->iX);

    tSrcPoint.iX = pSrcPoint->iX + 0 + tOffset->iX;
    iX = tSrcPoint.iX - ptCenter->iX;

    tPointCorner[0][1].fY = (iY * cosAngle + iX * sinAngle + ptCenter->iY);
    tPointCorner[0][1].fX = (-iY * sinAngle + iX * cosAngle + ptCenter->iX);

    float           slopeX, slopeY;

    /* interpolation in Y direction for 1st elements column */
    slopeX = (tPointCorner[0][1].fX - tPointCorner[0][0].fX) * invHeight;
    slopeY = (tPointCorner[0][1].fY - tPointCorner[0][0].fY) * invHeight;

    regrCoefs[0].slopeY = slopeY;
    regrCoefs[0].slopeX = slopeX;
    regrCoefs[0].interceptY = tPointCorner[0][0].fY;
    regrCoefs[0].interceptX = tPointCorner[0][0].fX;


    /* interpolation in Y direction for the last elements column */
    slopeX = (tPointCorner[1][1].fX - tPointCorner[1][0].fX) * invHeight;
    slopeY = (tPointCorner[1][1].fY - tPointCorner[1][0].fY) * invHeight;

    regrCoefs[1].slopeY = slopeY;
    regrCoefs[1].slopeX = slopeX;
    regrCoefs[1].interceptY = tPointCorner[1][0].fY;
    regrCoefs[1].interceptX = tPointCorner[1][0].fX;
}

#endif


ARM_NONNULL(1,2,5)
static
arm_2d_point_float_t *__arm_2d_transform_point(
                                            const arm_2d_location_t *ptLocation,
                                            const arm_2d_location_t *ptCenter,
                                            float fAngle,
                                            float fScale,
                                            arm_2d_point_float_t *ptOutBuffer)
{
    int16_t iX = ptLocation->iX - ptCenter->iX;
    int16_t iY = ptLocation->iY - ptCenter->iY;

    float fX,fY;

    float           cosAngle = arm_cos_f32(fAngle) * fScale;
    float           sinAngle = arm_sin_f32(fAngle) * fScale;

    fY = (iY * cosAngle + iX * sinAngle + ptCenter->iY);
    fX = (-iY * sinAngle + iX * cosAngle + ptCenter->iX);


#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
    if (fX > 0) {
        ptOutBuffer->fX = fX + __CALIB;
    } else {
        ptOutBuffer->fX = fX - __CALIB;
    }
    if (fY > 0) {
        ptOutBuffer->fY = fY + __CALIB;
    } else {
        ptOutBuffer->fY = fY - __CALIB;
    }
#else
    ptOutBuffer->fX = fX;
    ptOutBuffer->fY = fY;
#endif

    return ptOutBuffer;
}


static arm_2d_err_t __arm_2d_transform_preprocess_source(
                                        arm_2d_op_trans_t *ptThis,
                                        __arm_2d_transform_info_t *ptTransform)
{
    arm_2d_tile_t *ptSource = (arm_2d_tile_t *)this.Source.ptTile;

    memset(ptSource, 0, sizeof(*ptSource));

    ptSource->tInfo = this.Origin.ptTile->tInfo;
    ptSource->bIsRoot = true;
    ptSource->pchBuffer = NULL;                 //!< special case

    arm_2d_region_t tOrigValidRegion;
    if (NULL == arm_2d_tile_get_root(this.Origin.ptTile, &tOrigValidRegion, NULL)) {
        return ARM_2D_ERR_OUT_OF_REGION;
    }

    //! angle validation
    ptTransform->fAngle = fmodf(ptTransform->fAngle, ARM_2D_ANGLE(360));
    if (0.0f == ptTransform->fScale) {
        ptTransform->fScale = 1.0f;
    }

    //! calculate the source region
    do {
        arm_2d_point_float_t tPoint;

        arm_2d_location_t tTopLeft = {.iX = INT16_MAX, .iY = INT16_MAX};
        arm_2d_location_t tBottomRight = {.iX = INT16_MIN, .iY = INT16_MIN};

        //! Top Left
        arm_2d_location_t tCornerPoint = tOrigValidRegion.tLocation;
        __arm_2d_transform_point(  &tCornerPoint,
                                &ptTransform->tCenter,
                                ptTransform->fAngle,
                                ptTransform->fScale,
                                &tPoint);

        do {
            tTopLeft.iX = MIN(tTopLeft.iX, tPoint.fX);
            tTopLeft.iY = MIN(tTopLeft.iY, tPoint.fY);

            tBottomRight.iX = MAX(tBottomRight.iX, tPoint.fX);
            tBottomRight.iY = MAX(tBottomRight.iY, tPoint.fY);
        } while(0);

        //! Bottom Left
        tCornerPoint.iY += tOrigValidRegion.tSize.iHeight - 1;
        __arm_2d_transform_point(  &tCornerPoint,
                                &ptTransform->tCenter,
                                ptTransform->fAngle,
                                ptTransform->fScale,
                                &tPoint);

        do {
            tTopLeft.iX = MIN(tTopLeft.iX, tPoint.fX);
            tTopLeft.iY = MIN(tTopLeft.iY, tPoint.fY);

            tBottomRight.iX = MAX(tBottomRight.iX, tPoint.fX);
            tBottomRight.iY = MAX(tBottomRight.iY, tPoint.fY);
        } while(0);

        //! Top Right
        tCornerPoint = tOrigValidRegion.tLocation;
        tCornerPoint.iX += tOrigValidRegion.tSize.iWidth - 1;

        __arm_2d_transform_point(  &tCornerPoint,
                                &ptTransform->tCenter,
                                ptTransform->fAngle,
                                ptTransform->fScale,
                                &tPoint);

        do {
            tTopLeft.iX = MIN(tTopLeft.iX, tPoint.fX);
            tTopLeft.iY = MIN(tTopLeft.iY, tPoint.fY);

            tBottomRight.iX = MAX(tBottomRight.iX, tPoint.fX);
            tBottomRight.iY = MAX(tBottomRight.iY, tPoint.fY);
        } while(0);

        //! Bottom Right
        tCornerPoint.iY += tOrigValidRegion.tSize.iHeight - 1;
        __arm_2d_transform_point(  &tCornerPoint,
                                &ptTransform->tCenter,
                                ptTransform->fAngle,
                                ptTransform->fScale,
                                &tPoint);

        do {
            tTopLeft.iX = MIN(tTopLeft.iX, tPoint.fX);
            tTopLeft.iY = MIN(tTopLeft.iY, tPoint.fY);

            tBottomRight.iX = MAX(tBottomRight.iX, tPoint.fX);
            tBottomRight.iY = MAX(tBottomRight.iY, tPoint.fY);
        } while(0);

        //! calculate the region
        ptTransform->tDummySourceOffset = tTopLeft;

        ptSource->tRegion.tSize.iHeight = tBottomRight.iY - tTopLeft.iY + 1;
        ptSource->tRegion.tSize.iWidth = tBottomRight.iX - tTopLeft.iX + 1;

        ptTransform->fScale = 1.0f / ptTransform->fScale;

        //ptTransform->tTargetRegion.tSize = ptSource->tRegion.tSize;
    } while(0);

    return ARM_2D_ERR_NONE;
}


static void __arm_2d_transform_preprocess_target(
                                        arm_2d_op_trans_t *ptThis,
                                        const arm_2d_location_t *ptTargetCentre)
{
    __arm_2d_transform_info_t *ptTransform
        = (__arm_2d_transform_info_t *)
            (   (uintptr_t)ptThis
            +   this.use_as__arm_2d_op_core_t.ptOp->Info.chInClassOffset);

    //! the following code is correct. DO NOT modify it unless you 100% sure.
    ptTransform->Target.tRegion.tSize = this.Source.ptTile->tRegion.tSize;

#if 0  //!< please keep this code for understanding the original meaning
    arm_2d_region_t tTargetRegion = {0};
    if (NULL != this.Target.ptRegion) {
        tTargetRegion = *this.Target.ptRegion;
    } else {
        tTargetRegion.tSize = this.Target.ptTile->tRegion.tSize;
    }
#else
    //! equivalent code
    assert(NULL == this.Target.ptRegion);

    arm_2d_region_t tTargetRegion = {
        .tSize = this.Target.ptTile->tRegion.tSize,
    };
#endif

    this.Target.ptRegion = &ptTransform->Target.tRegion;

    ptTransform->Target.tRegion.tLocation = tTargetRegion.tLocation;

    //! align with the specified center point
    do {

        arm_2d_location_t tOffset = {
            .iX = ptTransform->tCenter.iX - ptTransform->tDummySourceOffset.iX,
            .iY = ptTransform->tCenter.iY - ptTransform->tDummySourceOffset.iY,
        };

        if (NULL == ptTargetCentre) {
            arm_2d_location_t tTargetCenter = {
                .iX = tTargetRegion.tSize.iWidth >> 1,
                .iY = tTargetRegion.tSize.iHeight >> 1,
            };

            tOffset.iX = tTargetCenter.iX - tOffset.iX;
            tOffset.iY = tTargetCenter.iY - tOffset.iY;
        } else {
            tOffset.iX = ptTargetCentre->iX - tOffset.iX;
            tOffset.iY = ptTargetCentre->iY - tOffset.iY;
        }
        ptTransform->Target.tRegion.tLocation.iX += tOffset.iX;
        ptTransform->Target.tRegion.tLocation.iY += tOffset.iY;

    } while(0);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_gray8_tile_transform_prepare(
                                            arm_2d_op_trans_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale,
                                            uint_fast8_t chFillColour)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScale = fScale;
    this.tTransform.tCenter = tCentre;
    this.tTransform.Mask.hwColour = chFillColour;

    return __arm_2d_transform_preprocess_source(ptThis, &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_transform_prepare(
                                            arm_2d_op_trans_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale,
                                            uint_fast16_t hwFillColour)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScale = fScale;
    this.tTransform.tCenter = tCentre;
    this.tTransform.Mask.hwColour = hwFillColour;

    return __arm_2d_transform_preprocess_source(ptThis, &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_tile_transform_prepare(
                                            arm_2d_op_trans_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale,
                                            uint32_t wFillColour)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScale = fScale;
    this.tTransform.tCenter = tCentre;
    this.tTransform.Mask.wColour = wFillColour;

    return __arm_2d_transform_preprocess_source(ptThis, &this.tTransform);
}

arm_fsm_rt_t __arm_2d_gray8_sw_transform(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_8BIT == OP_CORE.ptOp->Info.Colour.chScheme);

    __arm_2d_impl_gray8_transform(  &(ptTask->Param.tCopyOrig),
                                    &this.tTransform);


    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_transform(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    __arm_2d_impl_rgb565_transform( &(ptTask->Param.tCopyOrig),
                                    &this.tTransform);

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_transform(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);


    __arm_2d_impl_cccn888_transform(&(ptTask->Param.tCopyOrig),
                                    &this.tTransform);

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_gray8_tile_transform_with_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast8_t chFillColour,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_OPACITY_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScale = fScale;
    this.tTransform.tCenter = tCentre;
    this.tTransform.Mask.chColour = chFillColour;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}


ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_transform_with_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast16_t hwFillColour,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_OPACITY_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScale = fScale;
    this.tTransform.tCenter = tCentre;
    this.tTransform.Mask.hwColour = hwFillColour;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_tile_transform_with_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint32_t wFillColour,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_OPACITY_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScale = fScale;
    this.tTransform.tCenter = tCentre;
    this.tTransform.Mask.wColour = wFillColour;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

arm_fsm_rt_t __arm_2d_gray8_sw_transform_with_alpha(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);

    if (255 == this.chOpacity) {
        __arm_2d_impl_gray8_transform(  &(ptTask->Param.tCopyOrig),
                                        &this.tTransform);
    } else {
        __arm_2d_impl_gray8_transform_with_opacity(  &(ptTask->Param.tCopyOrig),
                                            &this.tTransform,
                                            this.chOpacity);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_transform_with_alpha(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    if (255 == this.chOpacity) {
        __arm_2d_impl_rgb565_transform( &(ptTask->Param.tCopyOrig),
                                        &this.tTransform);
    } else {
        __arm_2d_impl_rgb565_transform_with_opacity(   &(ptTask->Param.tCopyOrig),
                                                &this.tTransform,
                                                this.chOpacity);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_transform_with_alpha(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (255 == this.chOpacity) {
        __arm_2d_impl_cccn888_transform(&(ptTask->Param.tCopyOrig),
                                        &this.tTransform);
    } else {
        __arm_2d_impl_cccn888_transform_with_opacity(  &(ptTask->Param.tCopyOrig),
                                                &this.tTransform,
                                                this.chOpacity);
    }
    return arm_fsm_rt_cpl;
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_tile_transform(arm_2d_op_trans_t *ptOP,
                                 const arm_2d_tile_t *ptTarget,
                                 const arm_2d_region_t *ptRegion,
                                 const arm_2d_location_t *ptTargetCentre)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_trans_t, ptOP);
    arm_2d_location_t tTargetCentre;

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    if (NULL != ptRegion) {
        __arm_2d_transform_info_t *ptTransform
            = (__arm_2d_transform_info_t *)
                (   (uintptr_t)ptThis
                +   this.use_as__arm_2d_op_core_t.ptOp->Info.chInClassOffset);


        this.Target.ptTile = arm_2d_tile_generate_child(
                                                    ptTarget,
                                                    ptRegion,
                                                    &ptTransform->Target.tTile,
                                                    false);
        if (NULL == this.Target.ptTile) {
            arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_OUT_OF_REGION;
            if (ARM_2D_RUNTIME_FEATURE.TREAT_OUT_OF_RANGE_AS_COMPLETE) {
                tResult = arm_fsm_rt_cpl;
            }

            return __arm_2d_op_depose((arm_2d_op_core_t *)ptThis, tResult);
        }

        if (NULL != ptTargetCentre) {
            tTargetCentre.iX = ptTargetCentre->iX - ptRegion->tLocation.iX;
            tTargetCentre.iY = ptTargetCentre->iY - ptRegion->tLocation.iY;

            ptTargetCentre = &tTargetCentre;
        }
    } else {
        this.Target.ptTile = ptTarget;
        //this.Target.ptRegion = ptRegion;
    }

    this.Target.ptRegion = NULL;

    __arm_2d_transform_preprocess_target(   ptThis, ptTargetCentre);
    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}



ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_gray8_tile_transform_with_src_mask_prepare(
                                            arm_2d_op_trans_msk_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(ptSource,
                                            ptSourceMask,
                                            NULL,
                                            NULL,
                                            0);
    if (tErr < 0) {
        return tErr;
    }

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScale = fScale;
    this.tTransform.tCenter = tCentre;
    //this.tTransform.Mask.hwColour = chFillColour;
    this.Mask.ptOriginSide = ptSourceMask;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_rgb565_tile_transform_with_src_mask_prepare(
                                            arm_2d_op_trans_msk_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(ptSource,
                                            ptSourceMask,
                                            NULL,
                                            NULL,
                                            0);
    if (tErr < 0) {
        return tErr;
    }

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScale = fScale;
    this.tTransform.tCenter = tCentre;
    //this.tTransform.Mask.hwColour = hwFillColour;
    this.Mask.ptOriginSide = ptSourceMask;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_cccn888_tile_transform_with_src_mask_prepare(
                                            arm_2d_op_trans_msk_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(ptSource,
                                            ptSourceMask,
                                            NULL,
                                            NULL,
                                            0);
    if (tErr < 0) {
        return tErr;
    }

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScale = fScale;
    this.tTransform.tCenter = tCentre;
    //this.tTransform.Mask.hwColour = wFillColour;
    this.Mask.ptOriginSide = ptSourceMask;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

arm_fsm_rt_t
__arm_2d_gray8_sw_transform_with_src_mask(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_msk_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_8BIT == OP_CORE.ptOp->Info.Colour.chScheme);

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptTask->Param.tCopyOrigMask.tOrigMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        __arm_2d_impl_gray8_transform_with_src_chn_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
    } else
#endif
    {
        __arm_2d_impl_gray8_transform_with_src_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t
__arm_2d_rgb565_sw_transform_with_src_mask(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_msk_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptTask->Param.tCopyOrigMask.tOrigMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        __arm_2d_impl_rgb565_transform_with_src_chn_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
    } else
#endif
    {
        __arm_2d_impl_rgb565_transform_with_src_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t
__arm_2d_cccn888_sw_transform_with_src_mask(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_msk_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptTask->Param.tCopyOrigMask.tOrigMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        __arm_2d_impl_cccn888_transform_with_src_chn_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
    } else
#endif
    {
        __arm_2d_impl_cccn888_transform_with_src_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
    }

    return arm_fsm_rt_cpl;
}



ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_gray8_tile_transform_with_src_mask_and_opacity_prepare(
                                            arm_2d_op_trans_msk_opa_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale,
                                            uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_msk_opa_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(ptSource,
                                            ptSourceMask,
                                            NULL,
                                            NULL,
                                            0);
    if (tErr < 0) {
        return tErr;
    }

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScale = fScale;
    this.tTransform.tCenter = tCentre;
    //this.tTransform.Mask.hwColour = chFillColour;
    this.Mask.ptOriginSide = ptSourceMask;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity_prepare(
                                            arm_2d_op_trans_msk_opa_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale,
                                            uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_msk_opa_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(ptSource,
                                            ptSourceMask,
                                            NULL,
                                            NULL,
                                            0);
    if (tErr < 0) {
        return tErr;
    }

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScale = fScale;
    this.tTransform.tCenter = tCentre;
    //this.tTransform.Mask.hwColour = hwFillColour;
    this.Mask.ptOriginSide = ptSourceMask;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity_prepare(
                                            arm_2d_op_trans_msk_opa_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale,
                                            uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_msk_opa_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(ptSource,
                                            ptSourceMask,
                                            NULL,
                                            NULL,
                                            0);
    if (tErr < 0) {
        return tErr;
    }

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScale = fScale;
    this.tTransform.tCenter = tCentre;
    //this.tTransform.Mask.hwColour = wFillColour;
    this.Mask.ptOriginSide = ptSourceMask;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

arm_fsm_rt_t
__arm_2d_gray8_sw_transform_with_src_mask_and_opacity(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_8BIT == OP_CORE.ptOp->Info.Colour.chScheme);

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptTask->Param.tCopyOrigMask.tOrigMask.tColour.chScheme);
#endif

    if (255 == this.chOpacity) {
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        if (bIsMaskChannel8In32) {
            __arm_2d_impl_gray8_transform_with_src_chn_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
        } else
    #endif
        {
            __arm_2d_impl_gray8_transform_with_src_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
        }
    } else {
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        if (bIsMaskChannel8In32) {
            __arm_2d_impl_gray8_transform_with_src_chn_mask_and_opacity(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform,
                                                this.chOpacity);
        } else
    #endif
        {
            __arm_2d_impl_gray8_transform_with_src_mask_and_opacity(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform,
                                                this.chOpacity);
        }
    }


    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t
__arm_2d_rgb565_sw_transform_with_src_mask_and_opacity(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptTask->Param.tCopyOrigMask.tOrigMask.tColour.chScheme);
#endif

    if (255 == this.chOpacity) {
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        if (bIsMaskChannel8In32) {
            __arm_2d_impl_rgb565_transform_with_src_chn_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
        } else
    #endif
        {
            __arm_2d_impl_rgb565_transform_with_src_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
        }
    } else {
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        if (bIsMaskChannel8In32) {
            __arm_2d_impl_rgb565_transform_with_src_chn_mask_and_opacity(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform,
                                                this.chOpacity);
        } else
    #endif
        {
            __arm_2d_impl_rgb565_transform_with_src_mask_and_opacity(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform,
                                                this.chOpacity);
        }
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t
__arm_2d_cccn888_sw_transform_with_src_mask_and_opacity(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptTask->Param.tCopyOrigMask.tOrigMask.tColour.chScheme);
#endif

    if (255 == this.chOpacity) {
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        if (bIsMaskChannel8In32) {
            __arm_2d_impl_cccn888_transform_with_src_chn_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
        } else
    #endif
        {
            __arm_2d_impl_cccn888_transform_with_src_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
        }
    } else {
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        if (bIsMaskChannel8In32) {
            __arm_2d_impl_cccn888_transform_with_src_chn_mask_and_opacity(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform,
                                                this.chOpacity);
        } else
    #endif
        {
            __arm_2d_impl_cccn888_transform_with_src_mask_and_opacity(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform,
                                                this.chOpacity);
        }
    }
    return arm_fsm_rt_cpl;
}



/*----------------------------------------------------------------------------*
 * Accelerable Low Level APIs                                                 *
 *----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*
 * Draw a point whose cordinates is stored as float point.                    *
 *----------------------------------------------------------------------------*/

#if 0
static arm_2d_region_t *__arm_2d_calculate_region(  const arm_2d_point_float_t *ptLocation,
                                                    arm_2d_region_t *ptRegion)
{
    assert(NULL != ptLocation);
    assert(NULL != ptRegion);

    /* +-----+-----+
     * |  P0 |  P1 |
     * +---- p ----+
     * |  P2 |  -- |
     * +-----+-----+
     */

    arm_2d_location_t tPoints[3];

    tPoints[0].iX = (int16_t)ptLocation->fX;
    tPoints[2].iX = (int16_t)ptLocation->fX;
    tPoints[1].iX = (int16_t)(ptLocation->fX + 0.99f);
    ptRegion->tSize.iWidth = tPoints[1].iX - tPoints[0].iX + 1;

    tPoints[0].iY = (int16_t)ptLocation->fY;
    tPoints[2].iY = (int16_t)ptLocation->fY;
    tPoints[1].iY = (int16_t)(ptLocation->fY + 0.99f);
    ptRegion->tSize.iHeight = tPoints[2].iY - tPoints[0].iY + 1;

    ptRegion->tLocation = tPoints[0];

    return ptRegion;
}
#endif



/*----------------------------------------------------------------------------*
 * Low Level IO Interfaces                                                    *
 *----------------------------------------------------------------------------*/

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_GRAY8,
                __arm_2d_gray8_sw_transform);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_RGB565,
                __arm_2d_rgb565_sw_transform);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_CCCN888,
                __arm_2d_cccn888_sw_transform);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_ALPHA_GRAY8,
                __arm_2d_gray8_sw_transform_with_alpha);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_ALPHA_RGB565,
                __arm_2d_rgb565_sw_transform_with_alpha);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_ALPHA_CCCN888,
                __arm_2d_cccn888_sw_transform_with_alpha);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_GRAY8,
                __arm_2d_gray8_sw_transform_with_src_mask);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_RGB565,
                __arm_2d_rgb565_sw_transform_with_src_mask);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_CCCN888,
                __arm_2d_cccn888_sw_transform_with_src_mask);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_GRAY8,
                __arm_2d_gray8_sw_transform_with_src_mask_and_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_RGB565,
                __arm_2d_rgb565_sw_transform_with_src_mask_and_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_CCCN888,
                __arm_2d_cccn888_sw_transform_with_src_mask_and_opacity);

const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM,
        .chInClassOffset    = offsetof(arm_2d_op_trans_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_GRAY8),
            .ptFillOrigLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM,
        .chInClassOffset    = offsetof(arm_2d_op_trans_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_RGB565),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM,
        .chInClassOffset    = offsetof(arm_2d_op_trans_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_CCCN888),
            .ptFillOrigLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_opa_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_ALPHA_GRAY8),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_opa_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_ALPHA_RGB565),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_OPACITY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_opa_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_ALPHA_CCCN888),
            .ptFillOrigLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
            .bHasSrcMask            = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_SOURCE_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_trans_msk_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_GRAY8),
            .ptFillOrigLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
            .bHasSrcMask            = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_SOURCE_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_trans_msk_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_RGB565),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
            .bHasSrcMask            = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_SOURCE_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_trans_msk_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_CCCN888),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
            .bHasSrcMask            = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_SOURCE_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_msk_opa_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_GRAY8),
            .ptFillOrigLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
            .bHasSrcMask            = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_SOURCE_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_msk_opa_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_RGB565),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
            .bHasSrcMask            = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_SOURCE_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_msk_opa_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_CCCN888),
            .ptFillOrigLike = NULL,
        },
    },
};


#ifdef   __cplusplus
}
#endif
