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
 * Title:        __arm_2d_alpha_blending_wrapper.inc
 * Description:  c code template for copy and fill like operations
 *
 * $Date:        06. Oct 2021
 * $Revision:    V.1.0.0
 *
 * -------------------------------------------------------------------- */
 
#ifndef __API_ABW_COLOUR
#   error You have to define __API_ABW_COLOUR before using this c template
#endif
#ifndef __API_ABW_INT_TYPE
#   error You have to define the __API_ABW_INT_TYPE before using this c template
#endif
#ifndef __API_ABW_INT_TYPE_BIT_NUM
#   error You have to define the __API_ABW_INT_TYPE_BIT_NUM before using this c template
#endif
#ifndef __API_ABW_COLOUR_SZ
#   error You have to define the __API_ABW_COLOUR_SZ before using this c template
#endif

#undef ____ABW_FUNC
#undef ___ABW_FUNC
#undef __ABW_FUNC



#ifndef __API_ABW_OP_NAME
#   define ____ABW_FUNC(__NAME, __COLOUR)                                      \
        __arm_2d_##__COLOUR##_sw_##__NAME
#   define ___ABW_FUNC(__NAME, __COLOUR)   ____ABW_FUNC(__NAME, __COLOUR) 
#else
#   define _____ABW_FUNC(__OP_NAME, __NAME, __COLOUR)                          \
        __arm_2d_##__COLOUR##_sw_##__OP_NAME##_##__NAME
#   define ____ABW_FUNC(__OP_NAME, __NAME, __COLOUR)                           \
        _____ABW_FUNC(__OP_NAME, __NAME, __COLOUR)
#   define ___ABW_FUNC(__NAME, __COLOUR)                                       \
        ____ABW_FUNC(__API_ABW_OP_NAME, __NAME, __COLOUR)
#endif

#define __ABW_FUNC(__NAME)   ___ABW_FUNC(__NAME, __API_ABW_COLOUR)




#undef ____ABW_LL_FUNC
#undef ___ABW_LL_FUNC
#undef __ABW_LL_FUNC



#ifndef __API_ABW_LL_OP_NAME
#   define ____ABW_LL_FUNC(__NAME, __COLOUR)                                    \
        __arm_2d_impl_##__COLOUR##_##__NAME
#   define ___ABW_LL_FUNC(__NAME, __COLOUR)   ____ABW_LL_FUNC(__NAME, __COLOUR) 
#else
#   define _____ABW_LL_FUNC(__OP_NAME, __NAME, __COLOUR)                        \
        __arm_2d_impl_##__COLOUR##_##__OP_NAME##_##__NAME
#   define ____ABW_LL_FUNC(__OP_NAME, __NAME, __COLOUR)                         \
        _____ABW_LL_FUNC(__OP_NAME, __NAME, __COLOUR)
#   define ___ABW_LL_FUNC(__NAME, __COLOUR)                                     \
        ____ABW_LL_FUNC(__API_ABW_LL_OP_NAME, __NAME, __COLOUR)
#endif

#define __ABW_LL_FUNC(__NAME)   ___ABW_LL_FUNC(__NAME, __API_ABW_COLOUR)


#undef ____ABW_TYPE
#undef ___ABW_TYPE
#undef __ABW_TYPE

#ifndef __API_ABW_OP_NAME
#   define ____ABW_TYPE(__NAME, __COLOUR)  arm_2d_##__COLOUR##_##__NAME
#   define ___ABW_TYPE(__NAME, __COLOUR)   ____ABW_TYPE(__NAME, __COLOUR) 
#else
#   define _____ABW_TYPE(__OP_NAME, __NAME, __COLOUR)                           \
        arm_2d_##__COLOUR##_##__OP_NAME##_##__NAME
#   define ____ABW_TYPE(__OP_NAME, __NAME, __COLOUR)                            \
        _____ABW_TYPE(__OP_NAME, __NAME, __COLOUR)
#   define ___ABW_TYPE(__NAME, __COLOUR)                                        \
        ____ABW_TYPE(__API_ABW_OP_NAME, __NAME, __COLOUR) 
#endif


#define __ABW_TYPE(__NAME)   ___ABW_TYPE(__NAME, __API_ABW_COLOUR)


 
arm_fsm_rt_t __ABW_FUNC(tile_copy_with_src_mask)( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptTask->ptOP);
    assert(__API_ABW_COLOUR_SZ == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    //! this should not happen
    assert(NULL != this.Mask.ptSourceSide);
            
    if (ptTask->Param.tCopyMask.tSrcMask.bInvalid) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    uint32_t wMode = this.wMode;


    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme) {
    
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
            __ABW_LL_FUNC(src_chn_msk_copy_mirror)(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,

                &ptTask->Param.tCopy.tCopySize,
                wMode);
        } else {
            __ABW_LL_FUNC(src_chn_msk_copy)(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,
                    
                &ptTask->Param.tCopy.tCopySize);
        }
    #else
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #endif
    } else {
        assert(     ARM_2D_COLOUR_8BIT 
              ==    ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme);
        
        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
            __ABW_LL_FUNC(src_msk_copy_mirror)(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,

                &ptTask->Param.tCopy.tCopySize,
                wMode);
        } else {
            __ABW_LL_FUNC(src_msk_copy)(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,
                    
                &ptTask->Param.tCopy.tCopySize);
        }
    }

    return arm_fsm_rt_cpl;
}
 
arm_fsm_rt_t __ABW_FUNC(tile_copy_with_des_mask)( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptTask->ptOP);
    assert(__API_ABW_COLOUR_SZ == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    //! this should not happen
    assert(NULL != this.Mask.ptTargetSide);
            
    if (ptTask->Param.tCopyMask.tDesMask.bInvalid) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    uint32_t wMode = this.wMode;


    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopyMask.tDesMask.tColour.chScheme) {
    
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        /*!  \note 1 horizontal line target mask in ARM_2D_CHANNEL_8in32 is not 
         *!       supported when ptTask->Param.tCopy.tCopySize.iHeight is larger
         *!       than 1 line.
         */
        
        assert (    ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize.iHeight
               >=   ptTask->Param.tCopy.tCopySize.iHeight);
    
        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {


            __ABW_LL_FUNC(des_chn_msk_copy_mirror)(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,


                ptTask->Param.tCopyMask.tDesMask.pBuffer,
                ptTask->Param.tCopyMask.tDesMask.iStride,
                &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                &ptTask->Param.tCopy.tCopySize,
                wMode);

        } else {

            __ABW_LL_FUNC(des_chn_msk_copy)(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,


                ptTask->Param.tCopyMask.tDesMask.pBuffer,
                ptTask->Param.tCopyMask.tDesMask.iStride,
                &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                &ptTask->Param.tCopy.tCopySize);
        }
    #else
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #endif
    
    } else 
    
    
    {
        assert(     ARM_2D_COLOUR_8BIT 
              ==    ptTask->Param.tCopyMask.tDesMask.tColour.chScheme);
              
        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {

            if (ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize.iHeight > 1) {
                __ABW_LL_FUNC(des_msk_copy_mirror)(
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.iStride,

                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.iStride,


                    ptTask->Param.tCopyMask.tDesMask.pBuffer,
                    ptTask->Param.tCopyMask.tDesMask.iStride,
                    &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                    &ptTask->Param.tCopy.tCopySize,
                    wMode);
            } else {
                __ABW_LL_FUNC(1h_des_msk_copy_mirror)(
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.iStride,

                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.iStride,


                    ptTask->Param.tCopyMask.tDesMask.pBuffer,
                    ptTask->Param.tCopyMask.tDesMask.iStride,
                    &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                    &ptTask->Param.tCopy.tCopySize,
                    wMode);
            }

        } else {
            if (ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize.iHeight > 1) {
                __ABW_LL_FUNC(des_msk_copy)(
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.iStride,

                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.iStride,


                    ptTask->Param.tCopyMask.tDesMask.pBuffer,
                    ptTask->Param.tCopyMask.tDesMask.iStride,
                    &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                    &ptTask->Param.tCopy.tCopySize);
            } else {
                __ABW_LL_FUNC(1h_des_msk_copy)(
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.iStride,

                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.iStride,


                    ptTask->Param.tCopyMask.tDesMask.pBuffer,
                    ptTask->Param.tCopyMask.tDesMask.iStride,
                    &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                    &ptTask->Param.tCopy.tCopySize);
            }
        }
    }
    
    return arm_fsm_rt_cpl;
}

 
arm_fsm_rt_t __ABW_FUNC(tile_copy_with_masks)( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptTask->ptOP);
    assert(__API_ABW_COLOUR_SZ == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    //! this should not happen
    assert(!(   (NULL == this.Mask.ptSourceSide) 
            &&  (NULL == this.Mask.ptTargetSide)));
            
    if (    (ptTask->Param.tCopyMask.tSrcMask.bInvalid)
       ||   (ptTask->Param.tCopyMask.tDesMask.bInvalid)) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }


    uint32_t wMode = this.wMode;
    
    if (    (   ARM_2D_CHANNEL_8in32 
            ==  ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme)
       &&   (   ARM_2D_CHANNEL_8in32 
            ==  ptTask->Param.tCopyMask.tDesMask.tColour.chScheme)) {
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        /*! \note both masks use ARM_2D_CHANNEL_8in32 format
         *! 
         *! \note 1 horizontal line target mask in ARM_2D_CHANNEL_8in32 is not 
         *!       supported when ptTask->Param.tCopy.tCopySize.iHeight is larger
         *!       than 1 line.
         */
        
        assert (    ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize.iHeight
               >=   ptTask->Param.tCopy.tCopySize.iHeight);
               
        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {

            __ABW_LL_FUNC(src_chn_msk_des_chn_msk_copy_mirror)(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,


                ptTask->Param.tCopyMask.tDesMask.pBuffer,
                ptTask->Param.tCopyMask.tDesMask.iStride,
                &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                &ptTask->Param.tCopy.tCopySize,
                wMode);
            
        } else {

            __ABW_LL_FUNC(src_chn_msk_des_chn_msk_copy)(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,


                ptTask->Param.tCopyMask.tDesMask.pBuffer,
                ptTask->Param.tCopyMask.tDesMask.iStride,
                &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                &ptTask->Param.tCopy.tCopySize);
        }
    #else
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #endif
        
    } else if (    (   ARM_2D_CHANNEL_8in32 
                   ==  ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme)
           &&   (   ARM_2D_CHANNEL_8in32 
                   !=  ptTask->Param.tCopyMask.tDesMask.tColour.chScheme)) {
                   
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        /*! only the source mask uses ARM_2D_CHANNEL_8in32 format */
        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
            if (ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize.iHeight > 1) {
                __ABW_LL_FUNC(src_chn_msk_des_msk_copy_mirror)(
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.iStride,

                    ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                    ptTask->Param.tCopyMask.tSrcMask.iStride,
                    &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.iStride,


                    ptTask->Param.tCopyMask.tDesMask.pBuffer,
                    ptTask->Param.tCopyMask.tDesMask.iStride,
                    &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                    &ptTask->Param.tCopy.tCopySize,
                    wMode);
            } else {
                __ABW_LL_FUNC(src_chn_msk_1h_des_msk_copy_mirror)(
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.iStride,

                    ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                    ptTask->Param.tCopyMask.tSrcMask.iStride,
                    &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.iStride,


                    ptTask->Param.tCopyMask.tDesMask.pBuffer,
                    ptTask->Param.tCopyMask.tDesMask.iStride,
                    &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                    &ptTask->Param.tCopy.tCopySize,
                    wMode);
            }
        
        } else {
            if (ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize.iHeight > 1) {
                __ABW_LL_FUNC(src_chn_msk_des_msk_copy)(
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.iStride,

                    ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                    ptTask->Param.tCopyMask.tSrcMask.iStride,
                    &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.iStride,


                    ptTask->Param.tCopyMask.tDesMask.pBuffer,
                    ptTask->Param.tCopyMask.tDesMask.iStride,
                    &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                    &ptTask->Param.tCopy.tCopySize);
            } else {
                __ABW_LL_FUNC(src_chn_msk_1h_des_msk_copy)(
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.iStride,

                    ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                    ptTask->Param.tCopyMask.tSrcMask.iStride,
                    &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.iStride,


                    ptTask->Param.tCopyMask.tDesMask.pBuffer,
                    ptTask->Param.tCopyMask.tDesMask.iStride,
                    &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                    &ptTask->Param.tCopy.tCopySize);
            }
        }
    #else
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #endif
    
    } else if (    (   ARM_2D_CHANNEL_8in32 
                   !=  ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme)
           &&   (   ARM_2D_CHANNEL_8in32 
                   ==  ptTask->Param.tCopyMask.tDesMask.tColour.chScheme)) {
   
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        /*! \note only the target mask uses ARM_2D_CHANNEL_8in32 format 
         *! 
         *! \note 1 horizontal line target mask in ARM_2D_CHANNEL_8in32 is not 
         *!       supported when ptTask->Param.tCopy.tCopySize.iHeight is larger
         *!       than 1 line.
         */
        
        assert (    ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize.iHeight
               >=   ptTask->Param.tCopy.tCopySize.iHeight);
        
        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {

            __ABW_LL_FUNC(masks_copy_mirror)(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,


                ptTask->Param.tCopyMask.tDesMask.pBuffer,
                ptTask->Param.tCopyMask.tDesMask.iStride,
                &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                &ptTask->Param.tCopy.tCopySize,
                wMode);
            
        } else {

            __ABW_LL_FUNC(masks_copy)(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,


                ptTask->Param.tCopyMask.tDesMask.pBuffer,
                ptTask->Param.tCopyMask.tDesMask.iStride,
                &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                &ptTask->Param.tCopy.tCopySize);
            
        }
    #else
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #endif
    
    } else {
        assert(     ARM_2D_COLOUR_8BIT 
              ==    ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme);
        assert(     ARM_2D_COLOUR_8BIT 
              ==    ptTask->Param.tCopyMask.tDesMask.tColour.chScheme);
              
        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
            if (ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize.iHeight > 1) {
                __ABW_LL_FUNC(masks_copy_mirror)(
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.iStride,

                    ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                    ptTask->Param.tCopyMask.tSrcMask.iStride,
                    &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.iStride,


                    ptTask->Param.tCopyMask.tDesMask.pBuffer,
                    ptTask->Param.tCopyMask.tDesMask.iStride,
                    &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                    &ptTask->Param.tCopy.tCopySize,
                    wMode);
            } else {
                __ABW_LL_FUNC(src_msk_1h_des_msk_copy_mirror)(
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.iStride,

                    ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                    ptTask->Param.tCopyMask.tSrcMask.iStride,
                    &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.iStride,


                    ptTask->Param.tCopyMask.tDesMask.pBuffer,
                    ptTask->Param.tCopyMask.tDesMask.iStride,
                    &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                    &ptTask->Param.tCopy.tCopySize,
                    wMode);
            }
        } else {
            if (ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize.iHeight > 1) {
                __ABW_LL_FUNC(masks_copy)(
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.iStride,

                    ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                    ptTask->Param.tCopyMask.tSrcMask.iStride,
                    &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.iStride,


                    ptTask->Param.tCopyMask.tDesMask.pBuffer,
                    ptTask->Param.tCopyMask.tDesMask.iStride,
                    &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                    &ptTask->Param.tCopy.tCopySize);
            } else {
                __ABW_LL_FUNC(src_msk_1h_des_msk_copy)(
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tSource.iStride,

                    ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                    ptTask->Param.tCopyMask.tSrcMask.iStride,
                    &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                    ptTask->Param.tCopyMask
                        .use_as____arm_2d_param_copy_t.tTarget.iStride,


                    ptTask->Param.tCopyMask.tDesMask.pBuffer,
                    ptTask->Param.tCopyMask.tDesMask.iStride,
                    &ptTask->Param.tCopyMask.tDesMask.tValidRegion.tSize,
                    &ptTask->Param.tCopy.tCopySize);
            }
        }
    }
    
    return arm_fsm_rt_cpl;
}

#if 0   /*! smart leads to larger code size */
arm_fsm_rt_t __ABW_FUNC(tile_copy_with_mask)( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptTask->ptOP);
    assert(__API_ABW_COLOUR_SZ == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    
    //! this should not happen
    assert(!(   (NULL == this.Mask.ptSourceSide) 
            &&  (NULL == this.Mask.ptTargetSide)));
    
    if (    (!ptTask->Param.tCopyMask.tSrcMask.bInvalid)
       &&   (ptTask->Param.tCopyMask.tDesMask.bInvalid)) {
        //! it only contains a mask on the source side
        return __ABW_FUNC(tile_copy_with_src_mask)(ptTask);
    } else if (    (ptTask->Param.tCopyMask.tSrcMask.bInvalid)
              &&   (!ptTask->Param.tCopyMask.tDesMask.bInvalid)) {
        //! it only contains a mask on the target side
        return __ABW_FUNC(tile_copy_with_des_mask)(ptTask);
    } else if (    (!ptTask->Param.tCopyMask.tSrcMask.bInvalid)
              &&   (!ptTask->Param.tCopyMask.tDesMask.bInvalid)) {
        //! it contains masks on both sides
        return __ABW_FUNC(tile_copy_with_masks)(ptTask);
    } else {
        //! it contains no mask
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }
    
    //return arm_fsm_rt_cpl;
}
#endif

 
arm_fsm_rt_t __ABW_FUNC(tile_fill_with_src_mask)( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptTask->ptOP);
    assert(__API_ABW_COLOUR_SZ == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    //! this should not happen
    assert(NULL != this.Mask.ptSourceSide);
    
    if (ptTask->Param.tFillMask.tSrcMask.bInvalid) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }
    
    uint32_t wMode = this.wMode;
    

    
    if (    ARM_2D_CHANNEL_8in32 
       ==   ptTask->Param.tFillMask.tSrcMask.tColour.chScheme) {
       
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {

            __ABW_LL_FUNC(src_chn_msk_fill_mirror)(
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                ptTask->Param.tFillMask.tSrcMask.pBuffer,
                ptTask->Param.tFillMask.tSrcMask.iStride,
                &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,
                wMode);

        
        } else {
            __ABW_LL_FUNC(src_chn_msk_fill)(   
                                    ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                ptTask->Param.tFillMask.tSrcMask.pBuffer,
                ptTask->Param.tFillMask.tSrcMask.iStride,
                &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize);
        }
    #else
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #endif
    
    } else {
        assert(     ARM_2D_COLOUR_8BIT
               ==   ptTask->Param.tFillMask.tSrcMask.tColour.chScheme);
               
        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {

            __ABW_LL_FUNC(src_msk_fill_mirror)(
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                ptTask->Param.tFillMask.tSrcMask.pBuffer,
                ptTask->Param.tFillMask.tSrcMask.iStride,
                &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,
                wMode);

        
        } else {
            __ABW_LL_FUNC(src_msk_fill)(   
                                    ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                ptTask->Param.tFillMask.tSrcMask.pBuffer,
                ptTask->Param.tFillMask.tSrcMask.iStride,
                &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize);
        }
    }

    
    return arm_fsm_rt_cpl;
}
 
arm_fsm_rt_t __ABW_FUNC(tile_fill_with_des_mask)( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptTask->ptOP);
    assert(__API_ABW_COLOUR_SZ == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    //! this should not happen
    assert(NULL != this.Mask.ptTargetSide);
    
    if (ptTask->Param.tFillMask.tDesMask.bInvalid) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }
    
    uint32_t wMode = this.wMode;
    
    if (    ARM_2D_CHANNEL_8in32 
       ==   ptTask->Param.tFillMask.tDesMask.tColour.chScheme) {

    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        /*!  \note 1 horizontal line target mask in ARM_2D_CHANNEL_8in32 is not 
         *!       supported when ptTask->Param.tCopy.tCopySize.iHeight is larger
         *!       than 1 line.
         */
        
        assert (    ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize.iHeight
               >=   ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t
                        .tTarget.tValidRegion.tSize.iHeight);
       
        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {

            __ABW_LL_FUNC(des_chn_msk_fill_mirror)(
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                ptTask->Param.tFillMask.tDesMask.pBuffer,
                ptTask->Param.tFillMask.tDesMask.iStride,
                &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize,
                wMode);
            
        
        } else {

            __ABW_LL_FUNC(des_chn_msk_fill)(   
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                ptTask->Param.tFillMask.tDesMask.pBuffer,
                ptTask->Param.tFillMask.tDesMask.iStride,
                &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize);
            
        }
    #else
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #endif
    
    } else {
        assert(     ARM_2D_COLOUR_8BIT
               ==   ptTask->Param.tFillMask.tDesMask.tColour.chScheme);
               
        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {

            if (ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize.iHeight > 1) {
                __ABW_LL_FUNC(des_msk_fill_mirror)(
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                    ptTask->Param.tFillMask.tDesMask.pBuffer,
                    ptTask->Param.tFillMask.tDesMask.iStride,
                    &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize,
                    wMode);
            } else {
                __ABW_LL_FUNC(1h_des_msk_fill_mirror)(
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                    ptTask->Param.tFillMask.tDesMask.pBuffer,
                    ptTask->Param.tFillMask.tDesMask.iStride,
                    &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize,
                    wMode);
            }
        
        } else {
            if (ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize.iHeight > 1) {
                __ABW_LL_FUNC(des_msk_fill)(   
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                    ptTask->Param.tFillMask.tDesMask.pBuffer,
                    ptTask->Param.tFillMask.tDesMask.iStride,
                    &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize);
            } else {
                __ABW_LL_FUNC(1h_des_msk_fill)(   
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                    ptTask->Param.tFillMask.tDesMask.pBuffer,
                    ptTask->Param.tFillMask.tDesMask.iStride,
                    &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize);
            }
        }
    }
        
    return arm_fsm_rt_cpl;
}
 
arm_fsm_rt_t __ABW_FUNC(tile_fill_with_masks)( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptTask->ptOP);
    assert(__API_ABW_COLOUR_SZ == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    //! this should not happen
    assert(!(   (NULL == this.Mask.ptSourceSide) 
            &&  (NULL == this.Mask.ptTargetSide)));
            
    if (    (ptTask->Param.tFillMask.tSrcMask.bInvalid)
       ||   (ptTask->Param.tFillMask.tDesMask.bInvalid)) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    uint32_t wMode = this.wMode;
    

    if (    (   ARM_2D_CHANNEL_8in32 
            ==  ptTask->Param.tFillMask.tSrcMask.tColour.chScheme)
       &&   (   ARM_2D_CHANNEL_8in32 
            ==  ptTask->Param.tFillMask.tDesMask.tColour.chScheme)) {
    
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        /*! \note both masks use ARM_2D_CHANNEL_8in32 format
         *! 
         *! \note 1 horizontal line target mask in ARM_2D_CHANNEL_8in32 is not 
         *!       supported when 
         *!       ptTask->Param.tFillMask
         *!             .use_as____arm_2d_param_fill_t
         *!                 .tTarget.tValidRegion.tSize.iHeight 
         *!       is larger
         *!       than 1 line.
         */
        assert (    ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize.iHeight
               >=   ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t
                            .tTarget.tValidRegion.tSize.iHeight);

        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {

            __ABW_LL_FUNC(src_chn_msk_des_chn_msk_fill_mirror)(
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                ptTask->Param.tFillMask.tSrcMask.pBuffer,
                ptTask->Param.tFillMask.tSrcMask.iStride,
                &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                ptTask->Param.tFillMask.tDesMask.pBuffer,
                ptTask->Param.tFillMask.tDesMask.iStride,
                &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize,
                wMode);
            
        } else {

            __ABW_LL_FUNC(src_chn_msk_des_chn_msk_fill)(   
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                ptTask->Param.tFillMask.tSrcMask.pBuffer,
                ptTask->Param.tFillMask.tSrcMask.iStride,
                &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                ptTask->Param.tFillMask.tDesMask.pBuffer,
                ptTask->Param.tFillMask.tDesMask.iStride,
                &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize);
            
        }
    #else
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #endif
            
    } else if (  (  ARM_2D_CHANNEL_8in32 
                 ==  ptTask->Param.tFillMask.tSrcMask.tColour.chScheme)
              && (  ARM_2D_CHANNEL_8in32 
                 !=  ptTask->Param.tFillMask.tDesMask.tColour.chScheme)) {
                 
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        /*! \note only the source mask uses ARM_2D_CHANNEL_8in32 format
         */
         
        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
            if (ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize.iHeight > 1) {
                __ABW_LL_FUNC(src_chn_msk_des_msk_fill_mirror)(
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                    ptTask->Param.tFillMask.tSrcMask.pBuffer,
                    ptTask->Param.tFillMask.tSrcMask.iStride,
                    &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                    ptTask->Param.tFillMask.tDesMask.pBuffer,
                    ptTask->Param.tFillMask.tDesMask.iStride,
                    &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize,
                    wMode);
            } else {
                __ABW_LL_FUNC(src_chn_msk_1h_des_msk_fill_mirror)(
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                    ptTask->Param.tFillMask.tSrcMask.pBuffer,
                    ptTask->Param.tFillMask.tSrcMask.iStride,
                    &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                    ptTask->Param.tFillMask.tDesMask.pBuffer,
                    ptTask->Param.tFillMask.tDesMask.iStride,
                    &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize,
                    wMode);
            }
        } else {
            if (ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize.iHeight > 1) {
                __ABW_LL_FUNC(src_chn_msk_des_msk_fill)(   
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                    ptTask->Param.tFillMask.tSrcMask.pBuffer,
                    ptTask->Param.tFillMask.tSrcMask.iStride,
                    &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                    ptTask->Param.tFillMask.tDesMask.pBuffer,
                    ptTask->Param.tFillMask.tDesMask.iStride,
                    &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize);
            } else {
                __ABW_LL_FUNC(src_chn_msk_1h_des_msk_fill)(   
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                    ptTask->Param.tFillMask.tSrcMask.pBuffer,
                    ptTask->Param.tFillMask.tSrcMask.iStride,
                    &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                    ptTask->Param.tFillMask.tDesMask.pBuffer,
                    ptTask->Param.tFillMask.tDesMask.iStride,
                    &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize);
            }
        }
    #else
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #endif
    } else if (  (  ARM_2D_CHANNEL_8in32 
                 !=  ptTask->Param.tFillMask.tSrcMask.tColour.chScheme)
              && (  ARM_2D_CHANNEL_8in32 
                 ==  ptTask->Param.tFillMask.tDesMask.tColour.chScheme)) {
                 
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        /*! \note only the target mask uses ARM_2D_CHANNEL_8in32 format
         *! 
         *! \note 1 horizontal line target mask in ARM_2D_CHANNEL_8in32 is not 
         *!       supported when 
         *!       ptTask->Param.tFillMask
         *!             .use_as____arm_2d_param_fill_t
         *!                 .tTarget.tValidRegion.tSize.iHeight 
         *!       is larger
         *!       than 1 line.
         */
        assert (    ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize.iHeight
               >=   ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t
                            .tTarget.tValidRegion.tSize.iHeight);

        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {

            __ABW_LL_FUNC(src_msk_des_chn_msk_fill_mirror)(
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                ptTask->Param.tFillMask.tSrcMask.pBuffer,
                ptTask->Param.tFillMask.tSrcMask.iStride,
                &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                ptTask->Param.tFillMask.tDesMask.pBuffer,
                ptTask->Param.tFillMask.tDesMask.iStride,
                &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize,
                wMode);
            
        } else {

            __ABW_LL_FUNC(src_msk_des_chn_msk_fill)(   
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                ptTask->Param.tFillMask.tSrcMask.pBuffer,
                ptTask->Param.tFillMask.tSrcMask.iStride,
                &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.iStride,
                &ptTask->Param.tFillMask
                    .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                ptTask->Param.tFillMask.tDesMask.pBuffer,
                ptTask->Param.tFillMask.tDesMask.iStride,
                &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize);
            
        }
    #else
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #endif
    
    } else {    
        if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
            if (ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize.iHeight > 1) {
                __ABW_LL_FUNC(masks_fill_mirror)(
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                    ptTask->Param.tFillMask.tSrcMask.pBuffer,
                    ptTask->Param.tFillMask.tSrcMask.iStride,
                    &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                    ptTask->Param.tFillMask.tDesMask.pBuffer,
                    ptTask->Param.tFillMask.tDesMask.iStride,
                    &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize,
                    wMode);
            } else {
                __ABW_LL_FUNC(src_msk_1h_des_msk_fill_mirror)(
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                    ptTask->Param.tFillMask.tSrcMask.pBuffer,
                    ptTask->Param.tFillMask.tSrcMask.iStride,
                    &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                    ptTask->Param.tFillMask.tDesMask.pBuffer,
                    ptTask->Param.tFillMask.tDesMask.iStride,
                    &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize,
                    wMode);
            }
        } else {
            if (ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize.iHeight > 1) {
                __ABW_LL_FUNC(masks_fill)(   
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                    ptTask->Param.tFillMask.tSrcMask.pBuffer,
                    ptTask->Param.tFillMask.tSrcMask.iStride,
                    &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                    ptTask->Param.tFillMask.tDesMask.pBuffer,
                    ptTask->Param.tFillMask.tDesMask.iStride,
                    &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize);
            } else {
                __ABW_LL_FUNC(src_msk_1h_des_msk_fill)(   
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tSource.tValidRegion.tSize,

                    ptTask->Param.tFillMask.tSrcMask.pBuffer,
                    ptTask->Param.tFillMask.tSrcMask.iStride,
                    &ptTask->Param.tFillMask.tSrcMask.tValidRegion.tSize,

                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.pBuffer,
                    ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.iStride,
                    &ptTask->Param.tFillMask
                        .use_as____arm_2d_param_fill_t.tTarget.tValidRegion.tSize,


                    ptTask->Param.tFillMask.tDesMask.pBuffer,
                    ptTask->Param.tFillMask.tDesMask.iStride,
                    &ptTask->Param.tFillMask.tDesMask.tValidRegion.tSize);
            }
        }

    }
    return arm_fsm_rt_cpl;
}

#if 0  //! smart detection comes with 50% more code size...
arm_fsm_rt_t __ABW_FUNC(tile_fill_with_mask)( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptTask->ptOP);
    assert(__API_ABW_COLOUR_SZ == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    //! this should not happen
    assert(!(   (NULL == this.Mask.ptSourceSide) 
            &&  (NULL == this.Mask.ptTargetSide)));


    if (    (!ptTask->Param.tFillMask.tSrcMask.bInvalid)
       &&   (ptTask->Param.tFillMask.tDesMask.bInvalid)) {
        //! it only contains a mask on the source side
        return __ABW_FUNC(tile_fill_with_src_mask)(ptTask);
    } else if (    (ptTask->Param.tFillMask.tSrcMask.bInvalid)
              &&   (!ptTask->Param.tFillMask.tDesMask.bInvalid)) {
        //! it only contains a mask on the target side
        return __ABW_FUNC(tile_fill_with_des_mask)(ptTask);
    } else if (    (!ptTask->Param.tFillMask.tSrcMask.bInvalid)
              &&   (!ptTask->Param.tFillMask.tDesMask.bInvalid)) {
        //! it contains masks on both sides
        return __ABW_FUNC(tile_fill_with_masks)(ptTask);
    } else {
        //! it contains no mask
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

        
    //return arm_fsm_rt_cpl;
    
}
#endif



#undef __API_ABW_COPY_LIKE_OP_NAME
#undef __API_ABW_COLOUR_SZ
#undef __API_ABW_OP_NAME
#undef ____ABW_FUNC
#undef ___ABW_FUNC
#undef __ABW_FUNC
#undef __API_ABW_LL_OP_NAME
#undef ____ABW_LL_FUNC
#undef ___ABW_LL_FUNC
#undef __ABW_LL_FUNC
#undef __API_ABW_COLOUR
#undef __API_ABW_INT_TYPE
#undef __API_ABW_INT_TYPE_BIT_NUM
#undef ____ABW_TYPE
#undef ___ABW_TYPE
#undef __ABW_TYPE
