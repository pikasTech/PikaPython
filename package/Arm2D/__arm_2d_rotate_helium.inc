
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
 * Title:        __arm_2d_rotate_helium.inc
 * Description:  c code template for rotation
 *
 * $Date:        22. Sept 2020
 * $Revision:    V.1.0.0
 *
 * -------------------------------------------------------------------- */

#ifndef __API_INT_TYPE_BIT_NUM
#   error You have to define __API_INT_TYPE_BIT_NUM before using this c template
#endif
#ifndef __API_COLOUR
#   error You have to define __API_COLOUR before using this c template
#endif


#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#define ____ARM_2D_FUNC(__NAME, __COLOUR)  __arm_2d_impl_##__COLOUR##_##__NAME
#define ___ARM_2D_FUNC(__NAME, __COLOUR)   ____ARM_2D_FUNC(__NAME, __COLOUR)
#define __ARM_2D_FUNC(__NAME)   ___ARM_2D_FUNC(__NAME, __API_COLOUR)



#define __API_INT_TYPE   ARM_PIX_SCLTYP(__API_INT_TYPE_BIT_NUM)

#define MASK_COLOR(sz)  (sz == 8) ? ptInfo->Mask.chColour : ((sz == 16) ?  ptInfo->Mask.hwColour :  ptInfo->Mask.wColour)


#if !__ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__


__OVERRIDE_WEAK
void __ARM_2D_FUNC(rotate)( __arm_2d_param_copy_orig_t *ptParam,
                            __arm_2d_rotate_info_t *ptInfo)
{
    int32_t    iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int32_t    iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int32_t    iTargetStride =
        ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE     *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    __API_INT_TYPE     *pOrigin = ptParam->tOrigin.pBuffer;
    int32_t             iOrigStride = ptParam->tOrigin.iStride;
    __API_INT_TYPE      MaskColour = MASK_COLOR(__API_INT_TYPE_BIT_NUM);
    float32_t           fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset =
        ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    arm_2d_location_t  *pCenter = &(ptInfo->tCenter);

    float32_t           invIWidth = iWidth > 1 ? 1.0f / (float32_t) (iWidth - 1) : __LARGEINVF32;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
#if __API_INT_TYPE_BIT_NUM == 16
    bool            gatherLoadIdxOverflow;
    gatherLoadIdxOverflow =
#endif
        __arm_2d_rotate_regression(&ptParam->use_as____arm_2d_param_copy_t.tCopySize,
                                   &SrcPt, fAngle, &tOffset, pCenter, iOrigStride,
                                   regrCoefs);


    /* slopes between 1st and last columns */
    float32_t       slopeY, slopeX;

    slopeY = (float32_t) (regrCoefs[1].interceptY - regrCoefs[0].interceptY) * invIWidth;
    slopeX = (float32_t) (regrCoefs[1].interceptX - regrCoefs[0].interceptX) * invIWidth;

#if __API_INT_TYPE_BIT_NUM == 16
    if (!gatherLoadIdxOverflow) {
#endif
        for (int32_t y = 0; y < iHeight; y++) {

            /* 1st column estimates (intercepts for regression in X direction */
            float32_t       colFirstY = regrCoefs[0].slopeY * y + regrCoefs[0].interceptY;
            float32_t       colFirstX = regrCoefs[0].slopeX * y + regrCoefs[0].interceptX;

            int32_t         nbVecElts = iWidth;
            float16x8_t     vX = vcvtq_f16_s16((int16x8_t) vidupq_n_u16(0, 1));
            __API_INT_TYPE       *pTargetBaseCur = pTargetBase;

            while (nbVecElts > 0) {
                arm_2d_point_f16x8_t tPointV;

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
                    __ARM_2D_FUNC(get_pixel_colour)(&tPointV,
                                                          &ptParam->tOrigin.tValidRegion,
                                                          pOrigin,
                                                          iOrigStride,
                                                          pTargetBaseCur, MaskColour,
                                                          nbVecElts);

                pTargetBaseCur += 8;
                vX += 8.0f16;
                nbVecElts -= 8;
            }
            pTargetBase += iTargetStride;
        }
#if __API_INT_TYPE_BIT_NUM == 16
    } else {
        for (int32_t y = 0; y < iHeight; y++) {

            /* 1st column estimates (intercepts for regression in X direction */
            float32_t       colFirstY = regrCoefs[0].slopeY * y + regrCoefs[0].interceptY;
            float32_t       colFirstX = regrCoefs[0].slopeX * y + regrCoefs[0].interceptX;
            int32_t         nbVecElts = iWidth;
            float16x8_t     vX = vcvtq_f16_s16((int16x8_t) vidupq_n_u16(0, 1));
            uint16_t       *pTargetBaseCur = pTargetBase;

            while (nbVecElts > 0) {
                arm_2d_point_f16x8_t tPointV;

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
                __arm_2d_impl_rgb565_get_pixel_colour_offs_compensated(&tPointV,
                                                                       &ptParam->tOrigin.
                                                                       tValidRegion,
                                                                       pOrigin,
                                                                       iOrigStride,
                                                                       pTargetBaseCur,
                                                                       MaskColour,
                                                                       nbVecElts);

                pTargetBaseCur += 8;
                vX += 8.0f16;
                nbVecElts -= 8;
            }
            pTargetBase += iTargetStride;
        }
    }
#endif
}


__OVERRIDE_WEAK
void __ARM_2D_FUNC(rotate_alpha)(   __arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_rotate_info_t *ptInfo,
                                    uint_fast8_t chRatio)
{
    int32_t         iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int32_t         iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int32_t         iTargetStride =
        ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE     *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    __API_INT_TYPE     *pOrigin = ptParam->tOrigin.pBuffer;
    int32_t             iOrigStride = ptParam->tOrigin.iStride;
    __API_INT_TYPE      MaskColour = MASK_COLOR(__API_INT_TYPE_BIT_NUM);
    float32_t           fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset =
        ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    arm_2d_location_t  *pCenter = &(ptInfo->tCenter);

    uint16_t            hwRatioCompl = 256 - chRatio;
    float32_t           invIWidth = iWidth > 1 ? 1.0f / (float32_t) (iWidth - 1) : __LARGEINVF32;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
#if __API_INT_TYPE_BIT_NUM == 16
    bool            gatherLoadIdxOverflow;
    gatherLoadIdxOverflow =
#endif
        __arm_2d_rotate_regression(&ptParam->use_as____arm_2d_param_copy_t.tCopySize,
                                   &SrcPt, fAngle, &tOffset, pCenter, iOrigStride,
                                   regrCoefs);

    /* slopes between 1st and last columns */
    float32_t       slopeY, slopeX;

    slopeY = (float32_t) (regrCoefs[1].interceptY - regrCoefs[0].interceptY) * invIWidth;
    slopeX = (float32_t) (regrCoefs[1].interceptX - regrCoefs[0].interceptX) * invIWidth;

#if __API_INT_TYPE_BIT_NUM == 16
    if (!gatherLoadIdxOverflow) {
#endif
        for (int32_t y = 0; y < iHeight; y++) {
            /* 1st column estimates (intercepts for regression in X direction */
            float32_t       colFirstY =
                 (regrCoefs[0].slopeY * y + regrCoefs[0].interceptY);
            float32_t       colFirstX =
                 (regrCoefs[0].slopeX * y + regrCoefs[0].interceptX);

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
                __ARM_2D_FUNC(get_pixel_colour_with_alpha)(&tPointV,
                                                             &ptParam->tOrigin.
                                                             tValidRegion,
                                                             pOrigin, iOrigStride,
                                                             pTargetBaseCur,
                                                             MaskColour,
                                                             hwRatioCompl,
                                                             nbVecElts);
                pTargetBaseCur += 8;
                vX += 8.0f16;
                nbVecElts -= 8;
            }
            pTargetBase += iTargetStride;
        }
#if __API_INT_TYPE_BIT_NUM == 16
    } else {

        /*
            Large image / Large origin offsets
            Gather load 16-bit could overflow
                - Y offset needs to be shifted down to avoid overflow
                - 16-bit gather loads base address is incremented

            Needs to be done in the inner loop.
            In the case of steep slopes, taking the minimum between the Y extrema could still generate overflows
        */
        for (int32_t y = 0; y < iHeight; y++) {
            /* 1st column estimates (intercepts for regression in X direction */
            float32_t       colFirstY =
                 (regrCoefs[0].slopeY * y + regrCoefs[0].interceptY);
            float32_t       colFirstX =
                 (regrCoefs[0].slopeX * y + regrCoefs[0].interceptX);

            int32_t         nbVecElts = iWidth;
            float16x8_t     vX = vcvtq_f16_s16((int16x8_t) vidupq_n_u16(0, 1));
            uint16_t       *pTargetBaseCur = pTargetBase;

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
                __arm_2d_impl_rgb565_get_pixel_colour_with_alpha_offs_compensated(&tPointV,
                                                                        &ptParam->tOrigin.
                                                                        tValidRegion,
                                                                        pOrigin,
                                                                        iOrigStride,
                                                                        pTargetBaseCur,
                                                                        MaskColour,
                                                                        hwRatioCompl,
                                                                        nbVecElts);
                pTargetBaseCur += 8;
                vX += 8.0f16;
                nbVecElts -= 8;
            }
            pTargetBase += iTargetStride;
        }
    }
#endif
}


#else /* __ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__ */

__OVERRIDE_WEAK
void __ARM_2D_FUNC(rotate)(   __arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_rotate_info_t *ptInfo)
{
    int32_t         iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int32_t         iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;
    int32_t         iTargetStride =
        ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE  *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    __API_INT_TYPE  *pOrigin = ptParam->tOrigin.pBuffer;
    int32_t         iOrigStride = ptParam->tOrigin.iStride;
    __API_INT_TYPE   MaskColour = MASK_COLOR(__API_INT_TYPE_BIT_NUM);
    float32_t        fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset =
        ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    arm_2d_location_t *pCenter = &(ptInfo->tCenter);
    q31_t           invIWidth = (iWidth > 1) ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t    regrCoefs[2];
    arm_2d_location_t           SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
#if __API_INT_TYPE_BIT_NUM == 16
    bool            gatherLoadIdxOverflow;

    gatherLoadIdxOverflow =
#endif
        __arm_2d_rotate_regression(&ptParam->use_as____arm_2d_param_copy_t.tCopySize,
                                   &SrcPt, fAngle, &tOffset, pCenter, iOrigStride,
                                   regrCoefs);


    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

#if __API_INT_TYPE_BIT_NUM == 16
    if (!gatherLoadIdxOverflow) {

#endif
    for (int32_t y = 0; y < iHeight; y++) {

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

                __ARM_2D_FUNC(get_pixel_colour)(&tPointV,
                                                  &ptParam->tOrigin.tValidRegion,
                                                  pOrigin,
                                                  iOrigStride,
                                                  pTargetBaseCur, MaskColour,
                                                  nbVecElts);

            pTargetBaseCur += 8;
            vX += ((1<<6) * 8);
            nbVecElts -= 8;
        }
        pTargetBase += iTargetStride;
        }
#if __API_INT_TYPE_BIT_NUM == 16
    /* RGB565 specific */
    }    else {
        for (int32_t y = 0; y < iHeight; y++) {

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
            __API_INT_TYPE     *pTargetBaseCur = pTargetBase;

            /* Q9.6 coversion */
            vX = SET_Q6INT(vX);

            while (nbVecElts > 0) {
                arm_2d_point_s16x8_t tPointV;

                tPointV.X = vqdmulhq_n_s16(vX, slopeX);
                tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

                tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
                tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

                __arm_2d_impl_rgb565_get_pixel_colour_offs_compensated(&tPointV,
                                                                       &ptParam->tOrigin.
                                                                       tValidRegion,
                                                                       pOrigin,
                                                                       iOrigStride,
                                                                       pTargetBaseCur,
                                                                       MaskColour,
                                                                       nbVecElts);

                pTargetBaseCur += 8;
                vX += SET_Q6INT(8);
                nbVecElts -= 8;
            }
            pTargetBase += iTargetStride;
        }
    }
#endif
}




__OVERRIDE_WEAK
void __ARM_2D_FUNC(rotate_alpha)(   __arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_rotate_info_t *ptInfo,
                                    uint_fast8_t chRatio)
{
    int32_t         iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int32_t         iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int32_t         iTargetStride =
        ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE       *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    __API_INT_TYPE       *pOrigin = ptParam->tOrigin.pBuffer;
    int32_t         iOrigStride = ptParam->tOrigin.iStride;
    __API_INT_TYPE        MaskColour = MASK_COLOR(__API_INT_TYPE_BIT_NUM);
    float           fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset =
        ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    arm_2d_location_t *pCenter = &(ptInfo->tCenter);

    uint16_t        hwRatioCompl = 256 - chRatio;
    q31_t           invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
#if __API_INT_TYPE_BIT_NUM == 16
    bool            gatherLoadIdxOverflow;
    gatherLoadIdxOverflow =
#endif
        __arm_2d_rotate_regression(&ptParam->use_as____arm_2d_param_copy_t.tCopySize,
                                   &SrcPt, fAngle, &tOffset, pCenter, iOrigStride,
                                   regrCoefs);


    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

#if __API_INT_TYPE_BIT_NUM == 16
    if (!gatherLoadIdxOverflow) {
#endif
        for (int32_t y = 0; y < iHeight; y++) {
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
            __API_INT_TYPE *pTargetBaseCur = pTargetBase;

            /* Q9.6 coversion */
            vX = SET_Q6INT(vX);

            while (nbVecElts > 0) {
                /* interpolation */
                arm_2d_point_s16x8_t tPointV;

                tPointV.X = vqdmulhq_n_s16(vX, slopeX);
                tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

                tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
                tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

                __ARM_2D_FUNC(get_pixel_colour_with_alpha)(&tPointV,
                                                             &ptParam->tOrigin.tValidRegion,
                                                             pOrigin, iOrigStride,
                                                             pTargetBaseCur,
                                                             MaskColour, hwRatioCompl,
                                                             nbVecElts);
                pTargetBaseCur += 8;
                vX += SET_Q6INT(8);
                nbVecElts -= 8;
            }
            pTargetBase += iTargetStride;
        }
#if __API_INT_TYPE_BIT_NUM == 16
    } else {
        /*
           Large image / Large origin offsets
           Gather load 16-bit could overflow
           - Y offset needs to be shifted down to avoid overflow
           - 16-bit gather loads base address is incremented

           Needs to be done in the inner loop.
           In the case of steep slopes, taking the minimum between the Y extrema could still generate overflows
         */
        for (int32_t y = 0; y < iHeight; y++) {
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
            uint16_t       *pTargetBaseCur = pTargetBase;

            /* Q9.6 coversion */
            vX = SET_Q6INT(vX);

            while (nbVecElts > 0) {
                /* interpolation */
                arm_2d_point_s16x8_t tPointV;

                tPointV.X = vqdmulhq_n_s16(vX, slopeX);
                tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

                tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
                tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

                __arm_2d_impl_rgb565_get_pixel_colour_with_alpha_offs_compensated
                    (&tPointV, &ptParam->tOrigin.tValidRegion, pOrigin, iOrigStride,
                     pTargetBaseCur, MaskColour, hwRatioCompl, nbVecElts);

                pTargetBaseCur += 8;
                vX += SET_Q6INT(8);
                nbVecElts -= 8;
            }
            pTargetBase += iTargetStride;
        }
    }
#endif
}


#endif /* __ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__ */


#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#undef __API_COLOUR
#undef __API_INT_TYPE
#undef __API_INT_TYPE_BIT_NUM
