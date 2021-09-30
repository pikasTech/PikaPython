/*
 * Copyright (C) 2010-2020 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        arm-2d_rotation.c
 * Description:  APIs for tile rotation
 *
 * $Date:        29 April 2021
 * $Revision:    V.0.1.0
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
#   pragma clang diagnostic push
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
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wfloat-conversion"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wundef"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
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

/*----------------------------------------------------------------------------*
 * Code Template                                                              *
 *----------------------------------------------------------------------------*/


#define __API_PIXEL_AVERAGE_RESULT_RGB565()                     \
    (   tPixel.R >>= 8,                                         \
        tPixel.G >>= 8,                                         \
        tPixel.B >>= 8,                                         \
        __arm_2d_rgb565_pack(&tPixel));

#define __API_PIXEL_AVERAGE_RESULT_RGB888()                     \
    (   tPixel.R >>= 8,                                         \
        tPixel.G >>= 8,                                         \
        tPixel.B >>= 8,                                         \
        __arm_2d_rgb888_pack(&tPixel));

#define __API_COLOUR                rgb565
#define __API_INT_TYPE              uint16_t
#define __API_PIXEL_BLENDING        __ARM_2D_PIXEL_BLENDING_RGB565
#define __API_PIXEL_AVERAGE         __ARM_2D_PIXEL_AVERAGE_RGB565
#define __API_PIXEL_AVERAGE_RESULT  __API_PIXEL_AVERAGE_RESULT_RGB565
#include "__arm_2d_rotate.inc"

#define __API_COLOUR                rgb888
#define __API_INT_TYPE              uint32_t
#define __API_PIXEL_BLENDING        __ARM_2D_PIXEL_BLENDING_RGB888
#define __API_PIXEL_AVERAGE         __ARM_2D_PIXEL_AVERAGE_RGB888
#define __API_PIXEL_AVERAGE_RESULT  __API_PIXEL_AVERAGE_RESULT_RGB888

#include "__arm_2d_rotate.inc"

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if 0

/* faster atan(y/x) float version */
static
float32_t __atan2_f32(float32_t y, float32_t x)
{
    float32_t       xabs = fabsf(x);
    float32_t       yabs = fabsf(y);
    float32_t       atan2est, div;

    if (xabs >= yabs) {
        /* division is in the [-1 +1] range */
        div = yabs / (xabs + EPS_ATAN2);
        atan2est = FAST_ATAN_F32_1(div, div);
    } else {
        /* division is in the ]1 x*1e5] range */
        div = xabs / (yabs + EPS_ATAN2);
        atan2est = PI / 2 - FAST_ATAN_F32_1(div, div);
    }
    /* append sign */
    return copysignf(1.0f, y) * copysignf(1.0f, x) * atan2est;
}

static
void __arm_2d_rotate_get_rotated_corner(const arm_2d_location_t *ptLocation,
                                            const arm_2d_location_t *ptCenter,
                                            float fAngle,
                                            arm_2d_point_float_t *ptOutBuffer)
{
    int16_t         iX = ptLocation->iX - ptCenter->iX;
    int16_t         iY = ptLocation->iY - ptCenter->iY;

    float           cosAngle = arm_cos_f32(fAngle);
    float           sinAngle = arm_sin_f32(fAngle);

    ptOutBuffer->fY = (iY * cosAngle + iX * sinAngle + ptCenter->iY);
    ptOutBuffer->fX = (-iY * sinAngle + iX * cosAngle + ptCenter->iX);

}
#endif

#if __ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__

static
void __arm_2d_rotate_regression(arm_2d_size_t * __RESTRICT ptCopySize,
                                            arm_2d_location_t * pSrcPoint,
                                            float fAngle,
                                            arm_2d_location_t * tOffset,
                                            arm_2d_location_t * center,
                                            arm_2d_rot_linear_regr_t regrCoefs[]
    )
{
#define ONE_BY_2PI_Q31      341782637.0f
#define TO_Q16(x)           ((x) << 16)

    int_fast16_t        iHeight = ptCopySize->iHeight;
    int_fast16_t        iWidth = ptCopySize->iWidth;
    q31_t               invHeightFx = 0x7fffffff / (iHeight - 1);
    int32_t             AngleFx = lroundf(fAngle * ONE_BY_2PI_Q31);
    q31_t               cosAngleFx = arm_cos_q31(AngleFx);
    q31_t               sinAngleFx = arm_sin_q31(AngleFx);
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
        __QDADD(__QDADD(centerQ16.Y, MULTFX(iYQ16, cosAngleFx)), MULTFX(iXQ16, sinAngleFx));
    tPointCornerFx[0][0].X =
        __QDSUB(__QDADD(centerQ16.X, MULTFX(iXQ16, cosAngleFx)), MULTFX(iYQ16, sinAngleFx));


    /* ((iWidth - 1),0) corner */
    tmp.X = srcPointQ16.X + 0 + tOffsetQ16.X + TO_Q16(iWidth - 1);
    iXQ16 = tmp.X - centerQ16.X;

    tPointCornerFx[1][0].Y =
        __QDADD(__QDADD(centerQ16.Y, MULTFX(iYQ16, cosAngleFx)), MULTFX(iXQ16, sinAngleFx));
    tPointCornerFx[1][0].X =
        __QDSUB(__QDADD(centerQ16.X, MULTFX(iXQ16, cosAngleFx)), MULTFX(iYQ16, sinAngleFx));


    /* ((iWidth - 1),(iHeight - 1)) corner */
    tmp.Y = srcPointQ16.Y + tOffsetQ16.Y + TO_Q16(iHeight - 1);
    iYQ16 = tmp.Y - centerQ16.Y;

    tPointCornerFx[1][1].Y =
        __QDADD(__QDADD(centerQ16.Y, MULTFX(iYQ16, cosAngleFx)), MULTFX(iXQ16, sinAngleFx));
    tPointCornerFx[1][1].X =
        __QDSUB(__QDADD(centerQ16.X, MULTFX(iXQ16, cosAngleFx)), MULTFX(iYQ16, sinAngleFx));


    /* (0,(iHeight - 1)) corner */
    tmp.X = srcPointQ16.X + 0 + tOffsetQ16.X;
    iXQ16 = tmp.X - centerQ16.X;

    tPointCornerFx[0][1].Y =
        __QDADD(__QDADD(centerQ16.Y, MULTFX(iYQ16, cosAngleFx)), MULTFX(iXQ16, sinAngleFx));
    tPointCornerFx[0][1].X =
        __QDSUB(__QDADD(centerQ16.X, MULTFX(iXQ16, cosAngleFx)), MULTFX(iYQ16, sinAngleFx));


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
}

#else

static
void __arm_2d_rotate_regression(arm_2d_size_t * __RESTRICT ptCopySize,
                                            arm_2d_location_t * pSrcPoint,
                                            float fAngle,
                                            arm_2d_location_t * tOffset,
                                            arm_2d_location_t * ptCenter,
                                            arm_2d_rot_linear_regr_t regrCoefs[])
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    float           invHeight = 1.0f / (float) (iHeight - 1);
    float           cosAngle = arm_cos_f32(fAngle);
    float           sinAngle = arm_sin_f32(fAngle);
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


ARM_NONNULL(1,2,4)
static
arm_2d_point_float_t *__arm_2d_rotate_point(const arm_2d_location_t *ptLocation,
                                            const arm_2d_location_t *ptCenter,
                                            float fAngle,
                                            arm_2d_point_float_t *ptOutBuffer)
{
    int16_t iX = ptLocation->iX - ptCenter->iX;
    int16_t iY = ptLocation->iY - ptCenter->iY;

    float fX,fY;

    float           cosAngle = arm_cos_f32(fAngle);
    float           sinAngle = arm_sin_f32(fAngle);

    fY = (iY * cosAngle + iX * sinAngle + ptCenter->iY);
    fX = (-iY * sinAngle + iX * cosAngle + ptCenter->iX);


#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_ROTATION_FOR_PERFORMANCE__)
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


static arm_2d_err_t __arm_2d_rotate_preprocess_source(arm_2d_op_rotate_t *ptThis)
{
    arm_2d_tile_t *ptSource = this.Source.ptTile;

    memset(ptSource, 0, sizeof(*ptSource));

    ptSource->tInfo = this.Origin.ptTile->tInfo;
    ptSource->bIsRoot = true;
    ptSource->pchBuffer = NULL;                 //!< special case

    arm_2d_region_t tOrigValidRegion;
    if (NULL == arm_2d_tile_get_root(this.Origin.ptTile, &tOrigValidRegion, NULL)) {
        return ARM_2D_ERR_OUT_OF_REGION;
    }

    //! angle validation
    this.tRotate.fAngle = fmodf(this.tRotate.fAngle, ARM_2D_ANGLE(360));

    //! calculate the source region
    do {
        arm_2d_point_float_t tPoint;

        arm_2d_location_t tTopLeft = {.iX = INT16_MAX, .iY = INT16_MAX};
        arm_2d_location_t tBottomRight = {.iX = INT16_MIN, .iY = INT16_MIN};

        //! Top Left
        arm_2d_location_t tCornerPoint = tOrigValidRegion.tLocation;
        __arm_2d_rotate_point(  &tCornerPoint,
                                &this.tRotate.tCenter,
                                this.tRotate.fAngle,
                                &tPoint);

        do {
            tTopLeft.iX = MIN(tTopLeft.iX, tPoint.fX);
            tTopLeft.iY = MIN(tTopLeft.iY, tPoint.fY);

            tBottomRight.iX = MAX(tBottomRight.iX, tPoint.fX);
            tBottomRight.iY = MAX(tBottomRight.iY, tPoint.fY);
        } while(0);

        //! Bottom Left
        tCornerPoint.iY += tOrigValidRegion.tSize.iHeight - 1;
        __arm_2d_rotate_point(  &tCornerPoint,
                                &this.tRotate.tCenter,
                                this.tRotate.fAngle,
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

        __arm_2d_rotate_point(  &tCornerPoint,
                                &this.tRotate.tCenter,
                                this.tRotate.fAngle,
                                &tPoint);

        do {
            tTopLeft.iX = MIN(tTopLeft.iX, tPoint.fX);
            tTopLeft.iY = MIN(tTopLeft.iY, tPoint.fY);

            tBottomRight.iX = MAX(tBottomRight.iX, tPoint.fX);
            tBottomRight.iY = MAX(tBottomRight.iY, tPoint.fY);
        } while(0);

        //! Bottom Right
        tCornerPoint.iY += tOrigValidRegion.tSize.iHeight - 1;
        __arm_2d_rotate_point(  &tCornerPoint,
                                &this.tRotate.tCenter,
                                this.tRotate.fAngle,
                                &tPoint);

        do {
            tTopLeft.iX = MIN(tTopLeft.iX, tPoint.fX);
            tTopLeft.iY = MIN(tTopLeft.iY, tPoint.fY);

            tBottomRight.iX = MAX(tBottomRight.iX, tPoint.fX);
            tBottomRight.iY = MAX(tBottomRight.iY, tPoint.fY);
        } while(0);

        //! calculate the region
        this.tRotate.tDummySourceOffset = tTopLeft;

        ptSource->tRegion.tSize.iHeight = tBottomRight.iY - tTopLeft.iY + 1;
        ptSource->tRegion.tSize.iWidth = tBottomRight.iX - tTopLeft.iX + 1;

        //this.tRotate.tTargetRegion.tSize = ptSource->tRegion.tSize;
    } while(0);

    return ARM_2D_ERR_NONE;
}


static void __arm_2d_rotate_preprocess_target(arm_2d_op_rotate_t *ptThis)
{
    this.tRotate.tTargetRegion.tSize = this.Source.ptTile->tRegion.tSize;

    arm_2d_region_t *ptTargetRegion = this.Target.ptRegion;
    if (NULL == ptTargetRegion) {
        ptTargetRegion = &this.Target.ptTile->tRegion;
    }
    this.Target.ptRegion = &this.tRotate.tTargetRegion;

    this.tRotate.tTargetRegion.tLocation = ptTargetRegion->tLocation;

    //! align with the specified center point
    do {

        arm_2d_location_t tOffset = {
            .iX = this.tRotate.tCenter.iX - this.tRotate.tDummySourceOffset.iX,
            .iY = this.tRotate.tCenter.iY - this.tRotate.tDummySourceOffset.iY,
        };

        arm_2d_location_t tTargetCenter = {
            .iX = ptTargetRegion->tSize.iWidth >> 1,
            .iY = ptTargetRegion->tSize.iHeight >> 1,
        };

        tOffset.iX = tTargetCenter.iX - tOffset.iX;
        tOffset.iY = tTargetCenter.iY - tOffset.iY;

        this.tRotate.tTargetRegion.tLocation.iX += tOffset.iX;
        this.tRotate.tTargetRegion.tLocation.iY += tOffset.iY;

    } while(0);
}


ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_rotation_prepare(
                                            arm_2d_op_rotate_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            uint16_t hwFillColour)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_rotate_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_ROTATE_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tRotate.fAngle = fAngle;
    this.tRotate.tCenter = tCentre;
    this.tRotate.Mask.hwColour = hwFillColour;

    return __arm_2d_rotate_preprocess_source(ptThis);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb888_tile_rotation_prepare(
                                            arm_2d_op_rotate_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            uint32_t wFillColour)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_rotate_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_ROTATE_RGB888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tRotate.fAngle = fAngle;
    this.tRotate.tCenter = tCentre;
    this.tRotate.Mask.hwColour = wFillColour;

    return __arm_2d_rotate_preprocess_source(ptThis);
}

arm_fsm_rt_t __arm_2d_rgb565_sw_rotate(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_rotate_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    __arm_2d_impl_rgb565_rotate(&(ptTask->Param.tCopyOrig),
                                &this.tRotate);


    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb888_sw_rotate(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_rotate_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);


    __arm_2d_impl_rgb888_rotate(&(ptTask->Param.tCopyOrig),
                                &this.tRotate);

    return arm_fsm_rt_cpl;
}


ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_rotation_with_alpha_prepare(
                                        arm_2d_op_rotate_alpha_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        uint16_t hwFillColour,
                                        uint_fast8_t chRatio)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_rotate_alpha_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_ROTATE_WITH_ALPHA_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tRotate.fAngle = fAngle;
    this.tRotate.tCenter = tCentre;
    this.tRotate.Mask.hwColour = hwFillColour;
    this.chRatio = chRatio;

    return __arm_2d_rotate_preprocess_source((arm_2d_op_rotate_t *)ptThis);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb888_tile_rotation_with_alpha_prepare(
                                        arm_2d_op_rotate_alpha_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        uint32_t wFillColour,
                                        uint_fast8_t chRatio)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_rotate_alpha_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_ROTATE_WITH_ALPHA_RGB888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tRotate.fAngle = fAngle;
    this.tRotate.tCenter = tCentre;
    this.tRotate.Mask.wColour = wFillColour;
    this.chRatio = chRatio;

    return __arm_2d_rotate_preprocess_source((arm_2d_op_rotate_t *)ptThis);
}

arm_fsm_rt_t __arm_2d_rgb565_sw_rotate_with_alpha(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_rotate_alpha_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    __arm_2d_impl_rgb565_rotate_alpha(  &(ptTask->Param.tCopyOrig),
                                        &this.tRotate,
                                        this.chRatio);


    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb888_sw_rotate_with_alpha(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_rotate_alpha_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    __arm_2d_impl_rgb888_rotate_alpha(&(ptTask->Param.tCopyOrig),
                                        &this.tRotate,
                                        this.chRatio);

    return arm_fsm_rt_cpl;
}


arm_fsm_rt_t arm_2dp_tile_rotate(arm_2d_op_rotate_t *ptOP,
                                 const arm_2d_tile_t *ptTarget,
                                 const arm_2d_region_t *ptRegion)
{
    ARM_2D_IMPL(arm_2d_op_rotate_t, ptOP);

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;

    __arm_2d_rotate_preprocess_target(ptThis);
    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
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



#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_warning 174,177,188,68,513,144
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif
