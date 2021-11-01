
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
 * Title:        __arm_2d_fill_colour.inc
 * Description:  c code template for drawing pattern
 *
 * $Date:        14. April 2020
 * $Revision:    V.1.0.0
 *
 * -------------------------------------------------------------------- */


#ifndef __API_COLOUR
#   error You have to define __API_COLOUR before using this c template
#endif
#ifndef __API_INT_TYPE
#   error You have to define the __API_INT_TYPE before using this c template
#endif
#ifndef __API_PIXEL_BLENDING
#   error You have to define __API_PIXEL_BLENDING before using this c template
#endif
#ifndef __API_PIXEL_AVERAGE
#   error You have to define __API_PIXEL_AVERAGE before using this c template
#endif
#ifndef __API_PIXEL_AVERAGE_RESULT
#   error You have to define __API_PIXEL_AVERAGE_RESULT before using this c template
#endif
#ifndef __API_PIXEL_AVERAGE_INIT
#   define __API_PIXEL_AVERAGE_INIT()   __arm_2d_color_fast_rgb_t tPixel = {0};
#endif

#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#define ____ARM_2D_FUNC(__NAME, __COLOUR)  __arm_2d_impl_##__COLOUR##_##__NAME
#define ___ARM_2D_FUNC(__NAME, __COLOUR)   ____ARM_2D_FUNC(__NAME, __COLOUR)
#define __ARM_2D_FUNC(__NAME)   ___ARM_2D_FUNC(__NAME, __API_COLOUR)


static
arm_2d_point_float_t *__arm_2d_rotate_point(const arm_2d_location_t *ptLocation,
                                            const arm_2d_location_t *ptCenter,
                                            float fAngle,
                                            arm_2d_point_float_t *ptOutBuffer);

static
void __arm_2d_rotate_regression(arm_2d_size_t * __RESTRICT ptCopySize,
                                            arm_2d_location_t * pSrcPoint,
                                            float fAngle,
                                            arm_2d_location_t * tOffset,
                                            arm_2d_location_t * ptCenter,
                                            arm_2d_rot_linear_regr_t regrCoefs[]);


extern
void __ARM_2D_FUNC(rotate)( __arm_2d_param_copy_orig_t *ptParam,
                            __arm_2d_rotate_info_t *ptInfo);

extern
void __ARM_2D_FUNC(rotate_alpha)(   __arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_rotate_info_t *ptInfo,
                                    uint_fast8_t chRatio);

#if !__ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__

__WEAK
__API_INT_TYPE __ARM_2D_FUNC(get_pixel_colour)(   arm_2d_point_float_t *ptPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            __API_INT_TYPE *pOrigin,
                                            int16_t iOrigStride,
                                            //arm_2d_location_t *ptTargetPoint,
                                            //arm_2d_region_t *ptTargetValidRegion,
                                            __API_INT_TYPE *pTarget,
                                            //int16_t iTargetSride,
                                            __API_INT_TYPE MaskColour)
{
#if     defined(__ARM_2D_HAS_INTERPOLATION_ROTATION__)                          \
    &&  __ARM_2D_HAS_INTERPOLATION_ROTATION__

    arm_2d_location_t tOriginLocation;

    tOriginLocation.iX = ptPoint->fX;
    tOriginLocation.iY = ptPoint->fY;

    __arm_2d_point_adj_alpha_t tAdjacentArray
        = __arm_2d_point_get_adjacent_alpha_fp(ptPoint);

    __API_PIXEL_AVERAGE_INIT();

    __API_INT_TYPE TempPixel;
    bool bIsInside = false;

    for (int_fast8_t n = 0; n < 4; n++) {
        uint8_t chAlpha = tAdjacentArray.tMatrix[n].chAlpha;
        arm_2d_location_t tTemp = {
            .iX = tOriginLocation.iX + tAdjacentArray.tMatrix[n].tOffset.iX,
            .iY = tOriginLocation.iY + tAdjacentArray.tMatrix[n].tOffset.iY,
        };
        TempPixel = (*pTarget);

        if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tTemp)) {

            __API_INT_TYPE OriginPixel = pOrigin[   tTemp.iY * iOrigStride
                                            +   tTemp.iX];

            if (OriginPixel != MaskColour) {
                bIsInside = true;
                TempPixel = OriginPixel;
            }
        }

        __API_PIXEL_AVERAGE(TempPixel, chAlpha);
    }

    if (bIsInside) {
        TempPixel = __API_PIXEL_AVERAGE_RESULT();
    } else {
        TempPixel = *pTarget;
    }

    return TempPixel;

#else
    arm_2d_location_t tOriginLocation;

    tOriginLocation.iX = ptPoint->fX;
    tOriginLocation.iY = ptPoint->fY;
    __API_INT_TYPE Pixel = *pTarget;

    if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tOriginLocation)) {
        __API_INT_TYPE Temp = pOrigin[   tOriginLocation.iY * iOrigStride
                                     +   tOriginLocation.iX];
        if (Temp != MaskColour) {
            Pixel = Temp;
        }
    }

    return Pixel;
#endif
}

__WEAK
void __ARM_2D_FUNC(rotate)( __arm_2d_param_copy_orig_t *ptParam,
                            __arm_2d_rotate_info_t *ptInfo)
{
    int_fast16_t iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;
    __API_INT_TYPE MaskColour = ptInfo->Mask.hwColour;

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    float           invIWidth = iWidth > 1 ? 1.0f / (float) (iWidth - 1) : 100.0f;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_rotate_regression(&ptParam->use_as____arm_2d_param_copy_t.tCopySize,
                               &SrcPt, fAngle, &tOffset, &(ptInfo->tCenter), regrCoefs);

    /* slopes between 1st and last cols */
    float           slopeY, slopeX;

    slopeY = (regrCoefs[1].interceptY - regrCoefs[0].interceptY) * invIWidth;
    slopeX = (regrCoefs[1].interceptX - regrCoefs[0].interceptX) * invIWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates (intercepts for regression in X direction */
        float           colFirstY = regrCoefs[0].slopeY * y + regrCoefs[0].interceptY;
        float           colFirstX = regrCoefs[0].slopeX * y + regrCoefs[0].interceptX;

        for (int_fast16_t x = 0; x < iWidth; x++) {
            arm_2d_point_float_t tPoint;

            /* linear interpolation thru first & last cols */
            tPoint.fX = colFirstX + slopeX * x;
            tPoint.fY = colFirstY + slopeY * x;

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_ROTATION_FOR_PERFORMANCE__)
            if (tPoint.fX > 0) {
                tPoint.fX += __CALIB;
            } else {
                tPoint.fX -= __CALIB;
            }
            if (tPoint.fY > 0) {
                tPoint.fY += __CALIB;
            } else {
                tPoint.fY -= __CALIB;
            }
        #endif

            *pTargetBase = __ARM_2D_FUNC(get_pixel_colour)(
                                &tPoint,
                                &ptParam->tOrigin.tValidRegion,
                                ptParam->tOrigin.pBuffer,
                                iOrigStride,

                                pTargetBase,
                                //iTargetStride,
                                MaskColour
                            );
            pTargetBase++;
        }
        //phwSourceBase += (iSourceStride - iWidth);
        pTargetBase += (iTargetStride - iWidth);
    }
}

#else /* __ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__ */

__WEAK
__API_INT_TYPE __ARM_2D_FUNC(get_pixel_colour)(
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            __API_INT_TYPE *pOrigin,
                                            int16_t iOrigStride,
                                            __API_INT_TYPE *pTarget,
                                            //int16_t iTargetSride,
                                            __API_INT_TYPE MaskColour)
{
#if     defined(__ARM_2D_HAS_INTERPOLATION_ROTATION__)                          \
    &&  __ARM_2D_HAS_INTERPOLATION_ROTATION__

    arm_2d_location_t tOriginLocation = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    __arm_2d_point_adj_alpha_t tAdjacentArray
        = __arm_2d_point_get_adjacent_alpha_q16(ptFxPoint);

    __API_PIXEL_AVERAGE_INIT();

    __API_INT_TYPE TempPixel;
    bool bIsInside = false;

    for (int_fast8_t n = 0; n < 4; n++) {
        uint8_t chAlpha = tAdjacentArray.tMatrix[n].chAlpha;
        arm_2d_location_t tTemp = {
            .iX = tOriginLocation.iX + tAdjacentArray.tMatrix[n].tOffset.iX,
            .iY = tOriginLocation.iY + tAdjacentArray.tMatrix[n].tOffset.iY,
        };
        TempPixel = (*pTarget);

        if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tTemp)) {

            __API_INT_TYPE Temp = pOrigin[   tTemp.iY * iOrigStride
                                            +   tTemp.iX];

            if (Temp != MaskColour) {
                bIsInside = true;
                TempPixel = Temp;
            }
        }

        __API_PIXEL_AVERAGE(TempPixel, chAlpha);
    }

    if (bIsInside) {
        TempPixel = __API_PIXEL_AVERAGE_RESULT();
    } else {
        TempPixel = *pTarget;
    }

    return TempPixel;
#else
    __API_INT_TYPE Pixel = *pTarget;
    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };
    if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tPoint)) {
        __API_INT_TYPE Temp = pOrigin[   tPoint.iY * iOrigStride
                                     +   tPoint.iX];
        if (Temp != MaskColour) {
            Pixel = Temp;
        }
    }

    return Pixel;
#endif
}

__WEAK
void __ARM_2D_FUNC(rotate)(   __arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_rotate_info_t *ptInfo)
{
    int_fast16_t iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    __API_INT_TYPE *pOrigin = ptParam->tOrigin.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;
    __API_INT_TYPE MaskColour = ptInfo->Mask.hwColour;
    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    q31_t             invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_rotate_regression(&ptParam->use_as____arm_2d_param_copy_t.tCopySize,
                               &SrcPt, fAngle, &tOffset, &(ptInfo->tCenter), regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        for (int_fast16_t x = 0; x < iWidth; x++) {
            arm_2d_point_fx_t tPointFast;

            tPointFast.X = __QDADD(colFirstX, slopeX * x);
            tPointFast.Y = __QDADD(colFirstY, slopeY * x);

#define __CALIBFX 590
        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_ROTATION_FOR_PERFORMANCE__)
            if (tPointFast.X > 0) {
                tPointFast.X += __CALIBFX;
            } else {
                tPointFast.X -= __CALIBFX;
            }
            if (tPointFast.Y > 0) {
                tPointFast.Y += __CALIBFX;
            } else {
                tPointFast.Y -= __CALIBFX;
            }
        #endif

            *pTargetBase = __ARM_2D_FUNC(get_pixel_colour)(
                                        &tPointFast,
                                        &ptParam->tOrigin.tValidRegion,
                                        pOrigin,
                                        iOrigStride,
                                        pTargetBase,
                                        //iTargetStride,
                                        MaskColour
                                    );
            pTargetBase++;
        }
        //phwSourceBase += (iSourceStride - iWidth);
        pTargetBase += (iTargetStride - iWidth);
    }
}

#endif



#if !__ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__
__WEAK
__API_INT_TYPE __ARM_2D_FUNC(get_pixel_colour_with_alpha)(
                                            arm_2d_point_float_t *ptPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            __API_INT_TYPE *pOrigin,
                                            int16_t iOrigStride,
                                            __API_INT_TYPE *pTarget,
                                            //int16_t iTargetSride,
                                            __API_INT_TYPE MaskColour,
                                            uint8_t chOpacity)
{
#if     defined(__ARM_2D_HAS_INTERPOLATION_ROTATION__)                          \
    &&  __ARM_2D_HAS_INTERPOLATION_ROTATION__
    arm_2d_location_t tOriginLocation;

    tOriginLocation.iX = ptPoint->fX;
    tOriginLocation.iY = ptPoint->fY;

    __arm_2d_point_adj_alpha_t tAdjacentArray
        = __arm_2d_point_get_adjacent_alpha_fp(ptPoint);

    __API_PIXEL_AVERAGE_INIT();

    __API_INT_TYPE TempPixel;
    bool bIsInside = false;

    for (int_fast8_t n = 0; n < 4; n++) {
        uint8_t chAlpha = tAdjacentArray.tMatrix[n].chAlpha;
        arm_2d_location_t tTemp = {
            .iX = tOriginLocation.iX + tAdjacentArray.tMatrix[n].tOffset.iX,
            .iY = tOriginLocation.iY + tAdjacentArray.tMatrix[n].tOffset.iY,
        };
        TempPixel = (*pTarget);

        if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tTemp)) {

            __API_INT_TYPE OriginPixel = pOrigin[   tTemp.iY * iOrigStride
                                            +   tTemp.iX];

            if (OriginPixel != MaskColour) {
                bIsInside = true;
                TempPixel = OriginPixel;
            }
        }

        __API_PIXEL_AVERAGE(TempPixel, chAlpha);
    }

    if (bIsInside) {
        TempPixel = *pTarget;
        __API_INT_TYPE tSourcPixel = __API_PIXEL_AVERAGE_RESULT();

        __API_PIXEL_BLENDING( &tSourcPixel, &TempPixel, chOpacity);
    } else {
        TempPixel = *pTarget;
    }

    return TempPixel;
#else
    arm_2d_location_t tOriginLocation;

    tOriginLocation.iX = ptPoint->fX;
    tOriginLocation.iY = ptPoint->fY;
    __API_INT_TYPE Pixel = *pTarget;

    if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tOriginLocation)) {
        __API_INT_TYPE Temp = pOrigin[   tOriginLocation.iY * iOrigStride
                                     +   tOriginLocation.iX];
        if (Temp != MaskColour) {
            //Pixel = Temp;

            __API_PIXEL_BLENDING( &Temp, &Pixel, chOpacity);
        }
    }

    return Pixel;
#endif
}
#else
__WEAK
__API_INT_TYPE __ARM_2D_FUNC(get_pixel_colour_with_alpha)(
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            __API_INT_TYPE *pOrigin,
                                            int16_t iOrigStride,
                                            __API_INT_TYPE *pTarget,
                                            //int16_t iTargetSride,
                                            __API_INT_TYPE MaskColour,
                                            uint8_t chOpacity)
{
#if     defined(__ARM_2D_HAS_INTERPOLATION_ROTATION__)                          \
    &&  __ARM_2D_HAS_INTERPOLATION_ROTATION__
    arm_2d_location_t tOriginLocation = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    __arm_2d_point_adj_alpha_t tAdjacentArray
        = __arm_2d_point_get_adjacent_alpha_q16(ptFxPoint);

    __API_PIXEL_AVERAGE_INIT();

    __API_INT_TYPE TempPixel;
    bool bIsInside = false;

    for (int_fast8_t n = 0; n < 4; n++) {
        uint8_t chAlpha = tAdjacentArray.tMatrix[n].chAlpha;
        arm_2d_location_t tTemp = {
            .iX = tOriginLocation.iX + tAdjacentArray.tMatrix[n].tOffset.iX,
            .iY = tOriginLocation.iY + tAdjacentArray.tMatrix[n].tOffset.iY,
        };
        TempPixel = (*pTarget);

        if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tTemp)) {

            __API_INT_TYPE Temp = pOrigin[   tTemp.iY * iOrigStride
                                            +   tTemp.iX];

            if (Temp != MaskColour) {
                bIsInside = true;
                TempPixel = Temp;
            }
        }

        __API_PIXEL_AVERAGE(TempPixel, chAlpha);
    }

    if (bIsInside) {
        TempPixel = *pTarget;
        __API_INT_TYPE tSourcPixel = __API_PIXEL_AVERAGE_RESULT();

        __API_PIXEL_BLENDING( &tSourcPixel, &TempPixel, chOpacity);
    } else {
        TempPixel = *pTarget;
    }

    return TempPixel;
#else

    __API_INT_TYPE Pixel = *pTarget;
    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };
    if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tPoint)) {
        __API_INT_TYPE Temp = pOrigin[   tPoint.iY * iOrigStride
                                     +   tPoint.iX];
        if (Temp != MaskColour) {
            __API_PIXEL_BLENDING( &Temp, &Pixel, chOpacity);
        }
    }

    return Pixel;
#endif
}
#endif


#if !__ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__
__WEAK
void __ARM_2D_FUNC(rotate_alpha)(   __arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_rotate_info_t *ptInfo,
                                    uint_fast8_t chRatio)
{
    int_fast16_t        iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t        iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t        iTargetStride =
        ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE     *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    __API_INT_TYPE     *pOrigin = ptParam->tOrigin.pBuffer;
    int_fast16_t        iOrigStride = ptParam->tOrigin.iStride;
    __API_INT_TYPE      MaskColour = ptInfo->Mask.hwColour;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset =
        ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    uint16_t            hwRatioCompl = 256 - chRatio;
    float               invIWidth = iWidth > 1 ? 1.0f / (float) (iWidth - 1) : 100.0f;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_rotate_regression(&ptParam->use_as____arm_2d_param_copy_t.tCopySize,
                               &SrcPt, fAngle, &tOffset, &(ptInfo->tCenter), regrCoefs);

    /* slopes between 1st and last cols */
    float           slopeY, slopeX;

    slopeY = (regrCoefs[1].interceptY - regrCoefs[0].interceptY) * invIWidth;
    slopeX = (regrCoefs[1].interceptX - regrCoefs[0].interceptX) * invIWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates (intercepts for regression in X direction */
        float           colFirstY = regrCoefs[0].slopeY * y + regrCoefs[0].interceptY;
        float           colFirstX = regrCoefs[0].slopeX * y + regrCoefs[0].interceptX;


        for (int_fast16_t x = 0; x < iWidth; x++) {
            arm_2d_point_float_t tPoint;

            /* linear interpolation thru first & last cols */
            tPoint.fX = colFirstX + slopeX * x;
            tPoint.fY = colFirstY + slopeY * x;

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_ROTATION_FOR_PERFORMANCE__)
            if (tPoint.fX > 0) {
                tPoint.fX += __CALIB;
            } else {
                tPoint.fX -= __CALIB;
            }
            if (tPoint.fY > 0) {
                tPoint.fY += __CALIB;
            } else {
                tPoint.fY -= __CALIB;
            }
        #endif

            *pTargetBase = __ARM_2D_FUNC(get_pixel_colour_with_alpha) (
                               &tPoint,
                               &ptParam->tOrigin.
                               tValidRegion,
                               pOrigin,
                               iOrigStride,
                               pTargetBase,
                               MaskColour,
                               hwRatioCompl);
            pTargetBase++;
        }
        pTargetBase += (iTargetStride - iWidth);
    }
}

#else /* __ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__ */

__WEAK
void __ARM_2D_FUNC(rotate_alpha)(   __arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_rotate_info_t *ptInfo,
                                    uint_fast8_t chRatio)
{
    int_fast16_t        iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t        iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t        iTargetStride =
        ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE     *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    __API_INT_TYPE     *pOrigin = ptParam->tOrigin.pBuffer;
    int_fast16_t        iOrigStride = ptParam->tOrigin.iStride;
    __API_INT_TYPE      MaskColour = ptInfo->Mask.hwColour;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset =
        ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    uint16_t            hwRatioCompl = 256 - chRatio;
    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_rotate_regression(&ptParam->use_as____arm_2d_param_copy_t.tCopySize,
                               &SrcPt, fAngle, &tOffset, &(ptInfo->tCenter), regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        for (int_fast16_t x = 0; x < iWidth; x++) {
            arm_2d_point_fx_t tPointFast;

            tPointFast.X = __QDADD(colFirstX, slopeX * x);
            tPointFast.Y = __QDADD(colFirstY, slopeY * x);

#define __CALIBFX 590

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_ROTATION_FOR_PERFORMANCE__)
            if (tPointFast.X > 0) {
                tPointFast.X += __CALIBFX;
            } else {
                tPointFast.X -= __CALIBFX;
            }
            if (tPointFast.Y > 0) {
                tPointFast.Y += __CALIBFX;
            } else {
                tPointFast.Y -= __CALIBFX;
            }
        #endif

            *pTargetBase = __ARM_2D_FUNC(get_pixel_colour_with_alpha) (
                            &tPointFast,
                            &ptParam->tOrigin.
                            tValidRegion,
                            pOrigin,
                            iOrigStride,
                            pTargetBase,
                            MaskColour,
                            hwRatioCompl);
            pTargetBase++;
        }
        pTargetBase += (iTargetStride - iWidth);
    }
}

#endif /* __ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__ */


#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#undef __API_COLOUR
#undef __API_INT_TYPE
#undef __API_PIXEL_BLENDING
#undef __API_PIXEL_AVERAGE
#undef __API_PIXEL_AVERAGE_RESULT
#undef __API_PIXEL_AVERAGE_INIT
