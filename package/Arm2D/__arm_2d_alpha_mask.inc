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
 * Title:        __arm_2d_alpha_mask.inc
 * Description:  c code template for copy and fill like operations
 *
 * $Date:        30. Sept 2021
 * $Revision:    V.1.0.0
 *
 * -------------------------------------------------------------------- */
 
#ifndef __API_CAFWM_COLOUR
#   error You have to define __API_CAFWM_COLOUR before using this c template
#endif
#ifndef __API_CAFWM_INT_TYPE
#   error You have to define the __API_CAFWM_INT_TYPE before using this c template
#endif
#ifndef __API_CAFWM_INT_TYPE_BIT_NUM
#   error You have to define the __API_CAFWM_INT_TYPE_BIT_NUM before using this c template
#endif
#ifndef __API_CAFWM_PIXEL_BLENDING
#   error You have to define __API_PIXEL_BLENDING before using this c template
#endif

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
        __arm_2d_impl_##__COLOUR##_##__NAME
#   define ___CAFWM_FUNC(__NAME, __COLOUR)   ____CAFWM_FUNC(__NAME, __COLOUR) 
#else
#   define _____CAFWM_FUNC(__OP_NAME, __NAME, __COLOUR)                          \
        __arm_2d_impl_##__COLOUR##_##__OP_NAME##_##__NAME
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
#   define __API_MCWM_INT_TYPE_BIT_NUM         __API_CAFWM_INT_TYPE_BIT_NUM
#   define __API_MCWM_PIXEL_BLENDING           __API_CAFWM_PIXEL_BLENDING

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


#   include "__arm_2d_meta_copy_with_masks.inc"



/*! \note source mask only */
#   define __API_MCWM_COLOUR                   __API_CAFWM_COLOUR
#   define __API_MCWM_INT_TYPE                 __API_CAFWM_INT_TYPE
#   define __API_MCWM_INT_TYPE_BIT_NUM         __API_CAFWM_INT_TYPE_BIT_NUM
#   define __API_MCWM_PIXEL_BLENDING           __API_CAFWM_PIXEL_BLENDING

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


#   include "__arm_2d_meta_copy_with_masks.inc"


/*! \note des mask only */
#   define __API_MCWM_COLOUR                   __API_CAFWM_COLOUR
#   define __API_MCWM_INT_TYPE                 __API_CAFWM_INT_TYPE
#   define __API_MCWM_INT_TYPE_BIT_NUM         __API_CAFWM_INT_TYPE_BIT_NUM
#   define __API_MCWM_PIXEL_BLENDING           __API_CAFWM_PIXEL_BLENDING

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


#   include "__arm_2d_meta_copy_with_masks.inc"


/*! \note both masks */
#   define __API_MCWM_COLOUR                   __API_CAFWM_COLOUR
#   define __API_MCWM_INT_TYPE                 __API_CAFWM_INT_TYPE
#   define __API_MCWM_INT_TYPE_BIT_NUM         __API_CAFWM_INT_TYPE_BIT_NUM
#   define __API_MCWM_PIXEL_BLENDING           __API_CAFWM_PIXEL_BLENDING

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


#   include "__arm_2d_meta_copy_with_masks.inc"

#else

#   define __API_MCWM_COLOUR                   __API_CAFWM_COLOUR
#   define __API_MCWM_INT_TYPE                 __API_CAFWM_INT_TYPE
#   define __API_MCWM_INT_TYPE_BIT_NUM         __API_CAFWM_INT_TYPE_BIT_NUM
#   define __API_MCWM_PIXEL_BLENDING           __API_CAFWM_PIXEL_BLENDING

#   if defined(__API_CAFWM_OP_NAME)
#       define __API_MCWM_OP_NAME              __API_CAFWM_OP_NAME
#   endif

#   define __API_MCWM_CFG_SUPPORT_SRC_MSK_WRAPING                               \
                __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
#   define __API_MCWM_CFG_1_HORIZONTAL_LINE                                     \
                __API_CAFWM_CFG_1_HORIZONTAL_LINE
#   define __API_MCWM_CFG_CHANNEL_8in32_SUPPORT                     0

#   include "__arm_2d_meta_copy_with_masks.inc"

#endif


/*----------------------------------------------------------------------------*
 * Fill with Mirroring (target mask only)                                     *
 *----------------------------------------------------------------------------*/

__WEAK
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

                __API_CAFWM_INT_TYPE *__RESTRICT ptSrc = ptSource;

                for (int_fast16_t x = 0; x < wLength; x++) {
                #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    uint16_t hwOpacity = 256 - *(uint8_t *)(ptTargetMask++);
                #else
                    uint16_t hwOpacity = 256 - (*ptTargetMask++);
                #endif
                    __API_CAFWM_PIXEL_BLENDING(ptSrc++, ptTarget++, hwOpacity);
                }

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

__WEAK
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

                __API_CAFWM_INT_TYPE *__RESTRICT ptSrc = ptSource;
                
                ptSrc += ptSourceSize->iWidth - 1;
                
                for (int_fast16_t x = 0; x < wLength; x++) {
                #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    uint16_t hwOpacity = 256 - *(uint8_t *)(ptTargetMask++);
                #else
                    uint16_t hwOpacity = 256 - (*ptTargetMask++);
                #endif
                    __API_CAFWM_PIXEL_BLENDING(ptSrc--, ptTarget++, hwOpacity);
                }

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

__WEAK
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

                __API_CAFWM_INT_TYPE *__RESTRICT ptSrc = ptSource;
                
                for (int_fast16_t x = 0; x < wLength; x++) {
                #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    uint16_t hwOpacity = 256 - *(uint8_t *)(ptTargetMask++);
                #else
                    uint16_t hwOpacity = 256 - (*ptTargetMask++);
                #endif
                    __API_CAFWM_PIXEL_BLENDING(ptSrc++, ptTarget++, hwOpacity);
                }

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

__WEAK
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

                __API_CAFWM_INT_TYPE *__RESTRICT ptSrc = ptSource;                
                ptSrc += ptSourceSize->iWidth - 1;

                
                for (int_fast16_t x = 0; x < wLength; x++) {
                #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    uint16_t hwOpacity = 256 - *(uint8_t *)(ptTargetMask++);
                #else
                    uint16_t hwOpacity = 256 - (*ptTargetMask++);
                #endif
                    __API_CAFWM_PIXEL_BLENDING(ptSrc--, ptTarget++, hwOpacity);
                }

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


__WEAK
void __CAFWM_FUNC(des_msk_fill_mirror)(
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
                        arm_2d_size_t *__RESTRICT ptTargetMaskSize,
                        uint32_t wMode)
{
    switch (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
        case ARM_2D_CP_MODE_X_MIRROR:
            __CAFWM_FUNC(des_msk_fill_x_mirror)(
                    ptSourceBase, iSourceStride, ptSourceSize,
                    ptTargetBase, iTargetStride, ptTargetSize,
                    ptTargetMaskBase, iTargetMaskStride, ptTargetMaskSize);
            break;
        case ARM_2D_CP_MODE_Y_MIRROR:
            __CAFWM_FUNC(des_msk_fill_y_mirror)(
                    ptSourceBase, iSourceStride, ptSourceSize,
                    ptTargetBase, iTargetStride, ptTargetSize,
                    ptTargetMaskBase, iTargetMaskStride, ptTargetMaskSize);
            break;
        case ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR :
            __CAFWM_FUNC(des_msk_fill_xy_mirror)(
                    ptSourceBase, iSourceStride, ptSourceSize,
                    ptTargetBase, iTargetStride, ptTargetSize,
                    ptTargetMaskBase, iTargetMaskStride, ptTargetMaskSize);
            break;
        default:
            assert(false);  /*! this should not happen */
            //break;
    }
}



/*----------------------------------------------------------------------------*
 * Fill with Mirroring (src mask only)                                        *
 *----------------------------------------------------------------------------*/
#if !__API_CAFWM_CFG_1_HORIZONTAL_LINE
__WEAK
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

                __API_CAFWM_INT_TYPE *__RESTRICT ptSrc = ptSource;
            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t *__RESTRICT ptSrcMsk = ptSourceMask;
            #else
                uint8_t *__RESTRICT ptSrcMsk = ptSourceMask;
            #endif
                for (int_fast16_t x = 0; x < wLength; x++) {
                #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    uint16_t hwOpacity = 256 - *(uint8_t *)(ptSrcMsk++);
                #else
                    uint16_t hwOpacity = 256 - (*ptSrcMsk++);
                #endif
                    __API_CAFWM_PIXEL_BLENDING(ptSrc++, ptTarget++, hwOpacity);
                }

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

__WEAK
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

                __API_CAFWM_INT_TYPE *__RESTRICT ptSrc = ptSource;
            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t *__RESTRICT ptSrcMsk = ptSourceMask;
            #else
                uint8_t *__RESTRICT ptSrcMsk = ptSourceMask;
            #endif
            
                ptSrc += ptSourceSize->iWidth - 1;
                ptSrcMsk += ptSourceSize->iWidth - 1;
                
                for (int_fast16_t x = 0; x < wLength; x++) {
                #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    uint16_t hwOpacity = 256 - *(uint8_t *)(ptSrcMsk--);
                #else
                    uint16_t hwOpacity = 256 - (*ptSrcMsk--);
                #endif
                    __API_CAFWM_PIXEL_BLENDING(ptSrc--, ptTarget++, hwOpacity);
                }

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

__WEAK
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

                __API_CAFWM_INT_TYPE *__RESTRICT ptSrc = ptSource;
            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t *__RESTRICT ptSrcMsk = ptSourceMask;
            #else
                uint8_t *__RESTRICT ptSrcMsk = ptSourceMask;
            #endif
                for (int_fast16_t x = 0; x < wLength; x++) {
                #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    uint16_t hwOpacity = 256 - *(uint8_t *)(ptSrcMsk++);
                #else
                    uint16_t hwOpacity = 256 - (*ptSrcMsk++);
                #endif
                    __API_CAFWM_PIXEL_BLENDING(ptSrc++, ptTarget++, hwOpacity);
                }

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

__WEAK
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

                __API_CAFWM_INT_TYPE *__RESTRICT ptSrc = ptSource;
            #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                uint32_t *__RESTRICT ptSrcMsk = ptSourceMask;
            #else
                uint8_t *__RESTRICT ptSrcMsk = ptSourceMask;
            #endif
            
                ptSrc += ptSourceSize->iWidth - 1;
                ptSrcMsk += ptSourceSize->iWidth - 1;
                
                
                for (int_fast16_t x = 0; x < wLength; x++) {
                #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
                    uint16_t hwOpacity = 256 - *(uint8_t *)(ptSrcMsk--);
                #else
                    uint16_t hwOpacity = 256 - (*ptSrcMsk--);
                #endif
                    __API_CAFWM_PIXEL_BLENDING(ptSrc--, ptTarget++, hwOpacity);
                }

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


__WEAK
void __CAFWM_FUNC(src_msk_fill_mirror)(
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
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint32_t wMode)
{
    switch (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
        case ARM_2D_CP_MODE_X_MIRROR:
            __CAFWM_FUNC(src_msk_fill_x_mirror)(
                    ptSourceBase, iSourceStride, ptSourceSize,
                    ptSourceMaskBase, iSourceMaskStride, ptSourceMaskSize,
                    ptTargetBase, iTargetStride, ptTargetSize);
            break;
        case ARM_2D_CP_MODE_Y_MIRROR:
            __CAFWM_FUNC(src_msk_fill_y_mirror)(
                    ptSourceBase, iSourceStride, ptSourceSize,
                    ptSourceMaskBase, iSourceMaskStride, ptSourceMaskSize,
                    ptTargetBase, iTargetStride, ptTargetSize);
            break;
        case ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR :
            __CAFWM_FUNC(src_msk_fill_xy_mirror)(
                    ptSourceBase, iSourceStride, ptSourceSize,
                    ptSourceMaskBase, iSourceMaskStride, ptSourceMaskSize,
                    ptTargetBase, iTargetStride, ptTargetSize);
            break;
        default:
            assert(false);  /*! this should not happen */
            //break;
    }
}
#endif


/*----------------------------------------------------------------------------*
 * Copy with Mirroring  (target mask only)                                    *
 *----------------------------------------------------------------------------*/

__WEAK
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
    //uint16_t        hwRatioCompl = 256 - chRatio;
#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *ptTargetMask = ptTargetMaskBase;
#else
    uint8_t *ptTargetMask = ptTargetMaskBase;
#endif

    for (   int_fast16_t y = 0; 
            y < iHeight; 
            y++) {
    
        for (int_fast16_t x = 0; x < iWidth; x++) {
        #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            uint16_t hwOpacity = 256 - *(uint8_t *)(ptTargetMask++);
        #else
            uint16_t hwOpacity = 256 - (*ptTargetMask++);
        #endif
            __API_CAFWM_PIXEL_BLENDING( pSourceBase++, pTargetBase++, hwOpacity);
            
        }
        pSourceBase += (iSourceStride - iWidth);
        pTargetBase += (iTargetStride - iWidth);
    
    #if __API_CAFWM_CFG_1_HORIZONTAL_LINE
        ptTargetMask = ptTargetMaskBase;
    #else
        ptTargetMask += (iTargetMaskStride - iWidth);
    #endif
    }
}


__WEAK
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
    
#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *ptTargetMask = ptTargetMaskBase;
#else
    uint8_t *ptTargetMask = ptTargetMaskBase;
#endif

    for (   int_fast16_t y = 0; 
            y < iHeight; 
            y++) {
    
        //__API_CAFWM_INT_TYPE  *ptTargetCur = pTargetBase;
        __API_CAFWM_INT_TYPE  *ptSourceCur = pSourceBase;

        ptSourceCur += ptCopySize->iWidth - 1;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
        #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            uint16_t hwOpacity = 256 - *(uint8_t *)(ptTargetMask++);
        #else
            uint16_t hwOpacity = 256 - (*ptTargetMask++);
        #endif
            __API_CAFWM_PIXEL_BLENDING( ptSourceCur--, pTargetBase++, hwOpacity);
            
        }
        pSourceBase += iSourceStride;
        pTargetBase += (iTargetStride - iWidth);
        
    #if __API_CAFWM_CFG_1_HORIZONTAL_LINE
        ptTargetMask = ptTargetMaskBase;
    #else
        ptTargetMask += (iTargetMaskStride - iWidth);
    #endif
    }
}


__WEAK
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

    pSourceBase += iSourceStride * (ptCopySize->iHeight - 1);
#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *ptTargetMask = ptTargetMaskBase;
#else
    uint8_t *ptTargetMask = ptTargetMaskBase;
#endif

    for (   int_fast16_t y = 0; y < iHeight; y++) {
    
        __API_CAFWM_INT_TYPE  *ptSourceCur = pSourceBase;

        for (int_fast16_t x = 0; x < iWidth; x++) {
        #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            uint16_t hwOpacity = 256 - *(uint8_t *)(ptTargetMask++);
        #else
            uint16_t hwOpacity = 256 - (*ptTargetMask++);
        #endif
            __API_CAFWM_PIXEL_BLENDING( ptSourceCur++, pTargetBase++, hwOpacity);
            
        }
        pSourceBase -= iSourceStride;
        pTargetBase += (iTargetStride - iWidth);
    
    #if __API_CAFWM_CFG_1_HORIZONTAL_LINE
        ptTargetMask = ptTargetMaskBase;
    #else
        ptTargetMask += (iTargetMaskStride - iWidth);
    #endif
    }

}


__WEAK
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

    pSourceBase += iSourceStride * (ptCopySize->iHeight - 1);
#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *ptTargetMask = ptTargetMaskBase;
#else
    uint8_t *ptTargetMask = ptTargetMaskBase;
#endif

    for (   int_fast16_t y = 0; y < iHeight; y++) {
    
        __API_CAFWM_INT_TYPE  *ptSourceCur = pSourceBase;
        
        ptSourceCur += ptCopySize->iWidth - 1;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
        #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            uint16_t hwOpacity = 256 - *(uint8_t *)(ptTargetMask++);
        #else
            uint16_t hwOpacity = 256 - (*ptTargetMask++);
        #endif

            __API_CAFWM_PIXEL_BLENDING( ptSourceCur--, pTargetBase++, hwOpacity);
            
        }
        pSourceBase -= iSourceStride;
        pTargetBase += (iTargetStride - iWidth);
    
    #if __API_CAFWM_CFG_1_HORIZONTAL_LINE
        ptTargetMask = ptTargetMaskBase;
    #else
        ptTargetMask += (iTargetMaskStride - iWidth);
    #endif
    }

}

__WEAK
void __CAFWM_FUNC(des_msk_copy_mirror)(  
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

                                    arm_2d_size_t * __RESTRICT ptCopySize,
                                    uint32_t wMode)
{
    switch (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
      case ARM_2D_CP_MODE_X_MIRROR:
          __CAFWM_FUNC(des_msk_copy_x_mirror)(pSourceBase, iSourceStride,
                                            pTargetBase, iTargetStride, 
                                            ptTargetMaskBase, iTargetMaskStride,
                                            ptTargetMaskSize,
                                            ptCopySize);
          break;
      case ARM_2D_CP_MODE_Y_MIRROR:
          __CAFWM_FUNC(des_msk_copy_y_mirror)(pSourceBase, iSourceStride,
                                            pTargetBase, iTargetStride, 
                                            ptTargetMaskBase, iTargetMaskStride,
                                            ptTargetMaskSize,
                                            ptCopySize);
          break;
      case ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR:
          __CAFWM_FUNC(des_msk_copy_xy_mirror)(pSourceBase, iSourceStride,
                                            pTargetBase, iTargetStride, 
                                            ptTargetMaskBase, iTargetMaskStride,
                                            ptTargetMaskSize,
                                            ptCopySize);
          break;
      default:
          assert(false);        /*! this should not happen */
          //break;
    }
}



/*----------------------------------------------------------------------------*
 * Copy with Mirroring  (src mask only)                                       *
 *----------------------------------------------------------------------------*/
#if !__API_CAFWM_CFG_1_HORIZONTAL_LINE
__WEAK
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
#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *ptSourceMask = ptSourceMaskBase;
#else
    uint8_t *ptSourceMask = ptSourceMaskBase;
#endif

#if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
    int_fast16_t iSourceMaskY = 0;
#endif  

    for (int_fast16_t y = 0; y < iHeight; y++) {
    
        for (int_fast16_t x = 0; x < iWidth; x++) {
        #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            uint16_t hwOpacity = 256 - *(uint8_t *)(ptSourceMask++);
        #else
            uint16_t hwOpacity = 256 - (*ptSourceMask++);
        #endif
            __API_CAFWM_PIXEL_BLENDING( pSourceBase++, pTargetBase++, hwOpacity);
            
        }
        pSourceBase += (iSourceStride - iWidth);
        pTargetBase += (iTargetStride - iWidth);
        
    #if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
        //! handle source mask 
        iSourceMaskY++;
        if (    (iSourceMaskY >= ptSourceMaskSize->iHeight)
           ||   (iSourceMaskY >= iHeight)) {
            ptSourceMask = ptSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            ptSourceMask += (iSourceMaskStride - iWidth);
        }
    #else
        ptSourceMask += (iSourceMaskStride - iWidth);
    #endif
    }
}


__WEAK
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
#if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
    uint32_t *ptSourceMask = ptSourceMaskBase;
#else
    uint8_t *ptSourceMask = ptSourceMaskBase;
#endif

#if __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
    int_fast16_t iSourceMaskY = 0;
#endif  

    for (int_fast16_t y = 0; y < iHeight; y++) {
    
        __API_CAFWM_INT_TYPE  *ptTargetCur = pTargetBase;
        __API_CAFWM_INT_TYPE  *ptSourceCur = pSourceBase;
        
    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t *pchSourceMaskCur = ptSourceMask;
    #else
        uint8_t *pchSourceMaskCur = ptSourceMask;
    #endif
    
        ptSourceCur += ptCopySize->iWidth - 1;
        //! \note do not use ptSourceMaskSize->iWidth
        pchSourceMaskCur += ptCopySize->iWidth - 1; 
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
        #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            uint16_t hwOpacity = 256 - *(uint8_t *)(pchSourceMaskCur--);
        #else
            uint16_t hwOpacity = 256 - (*pchSourceMaskCur--);
        #endif
            __API_CAFWM_PIXEL_BLENDING( ptSourceCur--, ptTargetCur++, hwOpacity);
            
        }
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


__WEAK
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

    for (int_fast16_t y = 0; y < iHeight; y++) {
    
        __API_CAFWM_INT_TYPE  *ptSourceCur = pSourceBase;
    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t *pchSourceMaskCur = ptSourceMask;
    #else
        uint8_t *pchSourceMaskCur = ptSourceMask;
    #endif
    
        for (int_fast16_t x = 0; x < iWidth; x++) {
        #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            uint16_t hwOpacity = 256 - *(uint8_t *)(pchSourceMaskCur++);
        #else
            uint16_t hwOpacity = 256 - (*pchSourceMaskCur++);
        #endif
            __API_CAFWM_PIXEL_BLENDING( ptSourceCur++, pTargetBase++, hwOpacity);
        }
        
        pSourceBase -= iSourceStride;
        pTargetBase += (iTargetStride - iWidth);
        
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


__WEAK
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

    for (int_fast16_t y = 0; y < iHeight; y++) {
    
        __API_CAFWM_INT_TYPE  *ptSourceCur = pSourceBase;
    #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
        uint32_t *pchSourceMaskCur = ptSourceMask;
    #else
        uint8_t *pchSourceMaskCur = ptSourceMask;
    #endif
    
        ptSourceCur += ptCopySize->iWidth - 1;
        //! \note do not use ptSourceMaskSize->iWidth
        pchSourceMaskCur += ptCopySize->iWidth - 1; 
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
        #if __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT
            uint16_t hwOpacity = 256 - *(uint8_t *)(pchSourceMaskCur--);
        #else
            uint16_t hwOpacity = 256 - (*pchSourceMaskCur--);
        #endif

            __API_CAFWM_PIXEL_BLENDING( ptSourceCur--, pTargetBase++, hwOpacity);
            
        }
        pSourceBase -= iSourceStride;
        pTargetBase += (iTargetStride - iWidth);
        
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

__WEAK
void __CAFWM_FUNC(src_msk_copy_mirror)(  
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

                                    arm_2d_size_t * __RESTRICT ptCopySize,
                                    uint32_t wMode)
{
    switch (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
      case ARM_2D_CP_MODE_X_MIRROR:
          __CAFWM_FUNC(src_msk_copy_x_mirror)(pSourceBase, iSourceStride,
                                            ptSourceMaskBase, iSourceMaskStride,
                                            ptSourceMaskSize,
                                            pTargetBase, iTargetStride, 
                                            ptCopySize);
          break;
      case ARM_2D_CP_MODE_Y_MIRROR:
          __CAFWM_FUNC(src_msk_copy_y_mirror)(pSourceBase, iSourceStride,
                                            ptSourceMaskBase, iSourceMaskStride,
                                            ptSourceMaskSize,
                                            pTargetBase, iTargetStride, 
                                            ptCopySize);
          break;
      case ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR:
          __CAFWM_FUNC(src_msk_copy_xy_mirror)(pSourceBase, iSourceStride,
                                            ptSourceMaskBase, iSourceMaskStride,
                                            ptSourceMaskSize,
                                            pTargetBase, iTargetStride, 
                                            ptCopySize);
          break;
      default:
          assert(false);        /*! this should not happen */
          //break;
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
#undef __API_CAFWM_INT_TYPE_BIT_NUM
#undef ____CAFWM_TYPE
#undef ___CAFWM_TYPE
#undef __CAFWM_TYPE
#undef __API_CAFWM_CFG_SUPPORT_SRC_MSK_WRAPING
#undef __API_CAFWM_CFG_1_HORIZONTAL_LINE
#undef __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT

