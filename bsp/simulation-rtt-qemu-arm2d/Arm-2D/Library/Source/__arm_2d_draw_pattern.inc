
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
 * Title:        __arm_2d_draw_pattern.inc
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

#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#define ____ARM_2D_FUNC(__NAME, __COLOUR)  __arm_2d_impl_##__COLOUR##_##__NAME
#define ___ARM_2D_FUNC(__NAME, __COLOUR)   ____ARM_2D_FUNC(__NAME, __COLOUR) 
#define __ARM_2D_FUNC(__NAME)   ___ARM_2D_FUNC(__NAME, __API_COLOUR)


__WEAK
void __ARM_2D_FUNC(draw_pattern_fg_only)(uint8_t *__RESTRICT pchSourceBase,
                                         int32_t  iOffset,
                                         int16_t iSourceStride,
                                         __API_INT_TYPE *__RESTRICT pTargetBase,
                                         int16_t iTargetStride,
                                         arm_2d_size_t *__RESTRICT ptCopySize,
                                         __API_INT_TYPE ForeColour)
{
    //! get in byte offset
    iOffset &= 0x07;
    iSourceStride = (iSourceStride + 7) & ~0x07;
    
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
        
        uint8_t *__RESTRICT pchSrc = pchSourceBase;
        __API_INT_TYPE * __RESTRICT pTarget = pTargetBase;
        uint8_t chBitMask = _BV(iOffset);
        
        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++) {
            
            if ((*pchSrc) & chBitMask) {
                *pTarget = ForeColour;
            }
            
            pTarget++;
            chBitMask <<= 1;
            if (!chBitMask) {
                chBitMask = 1;
                pchSrc++;
            }
        }
        //chBitMask = 1;
        
        pchSourceBase += (iSourceStride >> 3);
        pTargetBase += iTargetStride;
    }
}

__WEAK
void __ARM_2D_FUNC(draw_pattern_no_bg_comp)(  
                                            uint8_t *__RESTRICT pchSourceBase,
                                            int32_t  iOffset,
                                            int16_t iSourceStride,
                                            __API_INT_TYPE *__RESTRICT pTargetBase,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *__RESTRICT ptCopySize)
{
    //! get in byte offset
    iOffset &= 0x07;
    iSourceStride = (iSourceStride + 7) & ~0x07;
    
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
        
        uint8_t *__RESTRICT pchSrc = pchSourceBase;
        __API_INT_TYPE * __RESTRICT pTarget = pTargetBase;
        uint8_t chBitMask = _BV(iOffset);
        
        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++) {
            
            if ((*pchSrc) & chBitMask) {
                *pTarget = ~(*pTarget);
            }
            
            pTarget++;
            chBitMask <<= 1;
            if (!chBitMask) {
                chBitMask = 1;
                pchSrc++;
            }
        }
        chBitMask = 1;
        
        pchSourceBase += (iSourceStride >> 3);
        pTargetBase += iTargetStride;
    }
}

__WEAK
void __ARM_2D_FUNC(draw_pattern_bg_only)(   uint8_t *__RESTRICT pchSourceBase,
                                            int32_t  iOffset,
                                            int16_t iSourceStride,
                                            __API_INT_TYPE *__RESTRICT pTargetBase,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *__RESTRICT ptCopySize,
                                            __API_INT_TYPE BackColour)
{
    //! get in byte offset
    iOffset &= 0x07;
    iSourceStride = (iSourceStride + 7) & ~0x07;

    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
        
        uint8_t *__RESTRICT pchSrc = pchSourceBase;
        __API_INT_TYPE * __RESTRICT pTarget = pTargetBase;
        uint8_t chBitMask = _BV(iOffset);
        
        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++) {
            
            if (!((*pchSrc) & chBitMask)) {
                *pTarget = BackColour;
            }
            
            pTarget++;
            chBitMask <<= 1;
            if (!chBitMask) {
                chBitMask = 1;
                pchSrc++;
            }
        }
        chBitMask = 1;
        
        pchSourceBase += (iSourceStride >> 3);
        pTargetBase += iTargetStride;
    }
}


__WEAK
void __ARM_2D_FUNC(draw_pattern_bg_fg)( uint8_t *__RESTRICT pchSourceBase,
                                        int32_t  iOffset,
                                        int16_t iSourceStride,
                                        __API_INT_TYPE *__RESTRICT pTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        __API_INT_TYPE ForeColour,
                                        __API_INT_TYPE BackColour)
{
    //! get in byte offset
    iOffset &= 0x07;
    iSourceStride = (iSourceStride + 7) & ~0x07;
    
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
        
        uint8_t *__RESTRICT pchSrc = pchSourceBase;
        __API_INT_TYPE * __RESTRICT pTarget = pTargetBase;
        uint8_t chBitMask = _BV(iOffset);
        
        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++) {
            
            if ((*pchSrc) & chBitMask) {
                *pTarget++ = ForeColour;
            } else {
                *pTarget++ = BackColour;
            }
            
            chBitMask <<= 1;
            if (!chBitMask) {
                chBitMask = 1;
                pchSrc++;
            }
        }
        chBitMask = 1;
        
        pchSourceBase += (iSourceStride >> 3);
        pTargetBase += iTargetStride;
    }
}

__WEAK
void __ARM_2D_FUNC(draw_pattern_bg_comp)(   uint8_t *__RESTRICT pchSourceBase,
                                            int32_t  iOffset,
                                            int16_t iSourceStride,
                                            __API_INT_TYPE *__RESTRICT pTargetBase,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *__RESTRICT ptCopySize,
                                            __API_INT_TYPE BackColour)
{
    //! get in byte offset
    iOffset &= 0x07;
    iSourceStride = (iSourceStride + 7) & ~0x07;
    
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
        
        uint8_t *__RESTRICT pchSrc = pchSourceBase;
        __API_INT_TYPE * __RESTRICT pTarget = pTargetBase;
        uint8_t chBitMask = _BV(iOffset);
        
        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++) {
            
            if ((*pchSrc) & chBitMask) {
                *pTarget = ~(*pTarget);
            } else {
                *pTarget = BackColour;
            }
            
            pTarget++;
            chBitMask <<= 1;
            if (!chBitMask) {
                chBitMask = 1;
                pchSrc++;
            }
        }
        chBitMask = 1;
        
        pchSourceBase += (iSourceStride >> 3);
        pTargetBase += iTargetStride;
    }
}


__WEAK
void __ARM_2D_FUNC(draw_pattern)(   uint8_t *__RESTRICT pchSourceBase,
                                    int32_t  iOffset,
                                    int16_t iSourceStride,
                                    __API_INT_TYPE *__RESTRICT pTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint8_t chMode,
                                    __API_INT_TYPE ForeColour,
                                    __API_INT_TYPE BackColour)
{

    switch (chMode  &  (ARM_2D_DRW_PATN_MODE_NO_FG_COLOR    | 
                        ARM_2D_DRW_PATH_MODE_COMP_FG_COLOUR |
                        ARM_2D_DRW_PATN_MODE_WITH_BG_COLOR)) {
    
        case 0:
            //! foreground only
            __ARM_2D_FUNC(draw_pattern_fg_only)(pchSourceBase,
                                                iOffset,
                                                iSourceStride,
                                                pTargetBase,
                                                iTargetStride,
                                                ptCopySize,
                                                ForeColour);
            break;
            
        case ARM_2D_DRW_PATN_MODE_WITH_BG_COLOR:
            //! background + foreground
            __ARM_2D_FUNC(draw_pattern_bg_fg)(  pchSourceBase,
                                                iOffset,
                                                iSourceStride,
                                                pTargetBase,
                                                iTargetStride,
                                                ptCopySize,
                                                ForeColour,
                                                BackColour);
            
            break;
            
        case ARM_2D_DRW_PATN_MODE_NO_FG_COLOR:
            //! no foreground, no background, nothing to do
            break;
            
        case ARM_2D_DRW_PATN_MODE_NO_FG_COLOR   |
             ARM_2D_DRW_PATN_MODE_WITH_BG_COLOR :
            //! background only
            __ARM_2D_FUNC(draw_pattern_bg_only)(pchSourceBase,
                                                iOffset,
                                                iSourceStride,
                                                pTargetBase,
                                                iTargetStride,
                                                ptCopySize,
                                                BackColour);
            break;
    
        case ARM_2D_DRW_PATH_MODE_COMP_FG_COLOUR:
        case ARM_2D_DRW_PATH_MODE_COMP_FG_COLOUR | 
             ARM_2D_DRW_PATN_MODE_NO_FG_COLOR    :
            //! no background, use complement colour as foreground
            __ARM_2D_FUNC(draw_pattern_no_bg_comp)( pchSourceBase,
                                                    iOffset,
                                                    iSourceStride,
                                                    pTargetBase,
                                                    iTargetStride,
                                                    ptCopySize);
            break;
            
        case ARM_2D_DRW_PATN_MODE_NO_FG_COLOR    | 
             ARM_2D_DRW_PATH_MODE_COMP_FG_COLOUR |
             ARM_2D_DRW_PATN_MODE_WITH_BG_COLOR  :
             //! background, use complement colour as foreground
            __ARM_2D_FUNC(draw_pattern_bg_comp)(pchSourceBase,
                                                iOffset,
                                                iSourceStride,
                                                pTargetBase,
                                                iTargetStride,
                                                ptCopySize,
                                                BackColour);
            
            break;
    
    }
}

#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#undef __API_COLOUR
#undef __API_INT_TYPE
