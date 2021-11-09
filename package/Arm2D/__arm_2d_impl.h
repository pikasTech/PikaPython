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
 * Title:        arm-2d_acc.h
 * Description:  Basic Tile operations
 *
 * $Date:        13. Jan 2021
 * $Revision:    V.0.5.0
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_ACCELERATION_H__
#   define __ARM_2D_ACCELERATION_H__      1

/*============================ INCLUDES ======================================*/

#include "arm_2d.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wpadded"
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_suppress 174,177,188,68,513,144,64
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-value"
#endif

/*============================ MACROS ========================================*/
#undef this
#define this            (*ptThis)

#undef OP_CORE
#define OP_CORE         this.use_as__arm_2d_op_core_t

#define ARM_2D_IMPL(__TYPE, ...)                                                \
            __TYPE *ptThis = (__TYPE *)(NULL,##__VA_ARGS__);                    \
            if (NULL == ptThis) {                                               \
                ptThis = (__TYPE *)&ARM_2D_CTRL.DefaultOP;                      \
            }

/*============================ MACROFIED FUNCTIONS ===========================*/

#define ARM_2D_TRY_ACCELERATION(__ID, __FUNC_PROTOTYPE, ...)                    \
        if (    (NULL != OP_CORE.ptOp->Info.LowLevelIO.IO[__ID]->HW)            \
            &&  (NULL != OP_CORE.ptOp->Info.LowLevelIO.IO[__ID])) {             \
            tResult =                                                           \
            (*(__FUNC_PROTOTYPE *)OP_CORE.ptOp->Info.LowLevelIO.IO[__ID]->HW)(  \
                                        ptTask,                                 \
                                        ##__VA_ARGS__);                         \
        }

#define ARM_2D_RUN_DEFAULT(__ID, __FUNC_PROTOTYPE, ...)                         \
        if (    (NULL != OP_CORE.ptOp->Info.LowLevelIO.IO[__ID]->SW)            \
            &&  (NULL != OP_CORE.ptOp->Info.LowLevelIO.IO[__ID])) {             \
            tResult =                                                           \
            (*(__FUNC_PROTOTYPE *)OP_CORE.ptOp->Info.LowLevelIO.IO[__ID]->SW)(  \
                                        ptTask,                                 \
                                        ##__VA_ARGS__);                         \
        } else {                                                                \
            tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;                     \
        }



#define __ARM_2D_PIXEL_AVERAGE_GRAY8(__PIXEL_IN, __ALPHA)                       \
    do {                                                                        \
        tPixel += (uint16_t)(__PIXEL_IN) * (uint16_t)(__ALPHA);                 \
    } while(0)
    
#define __ARM_2D_PIXEL_BLENDING_GRAY8(__SRC_ADDR, __DES_ADDR, __OPACITY)        \
            do {                                                                \
                uint16_t chTransparency = 256 - (__OPACITY);                    \
                const uint8_t *pchSrc = (uint8_t *)(__SRC_ADDR);                \
                uint8_t *pchDes = (uint8_t *)(__DES_ADDR);                      \
                                                                                \
                *pchDes = ((uint16_t)( ((uint16_t)(*pchSrc++) * chTransparency) \
                                     + ((uint16_t)(*pchDes) * (__OPACITY))      \
                                     ) >> 8);                                   \
            } while(0)

#define __ARM_2D_PIXEL_BLENDING_RGB565(__SRC_ADDR, __DES_ADDR, __OPACITY)       \
            do {                                                                \
                uint16_t chTransparency = 256 - (__OPACITY);                    \
                __arm_2d_color_fast_rgb_t tSrcPix, tTargetPix;                  \
                uint16_t *phwTargetPixel = (__DES_ADDR);                        \
                __arm_2d_rgb565_unpack(*(__SRC_ADDR), &tSrcPix);                \
                __arm_2d_rgb565_unpack(*phwTargetPixel, &tTargetPix);           \
                                                                                \
                for (int i = 0; i < 3; i++) {                                   \
                    uint16_t        hwTemp =                                    \
                        (uint16_t) (tSrcPix.RGB[i] * chTransparency) +          \
                        (tTargetPix.RGB[i] * (__OPACITY));                      \
                    tTargetPix.RGB[i] = (uint16_t) (hwTemp >> 8);               \
                }                                                               \
                                                                                \
                /* pack merged stream */                                        \
                *phwTargetPixel = __arm_2d_rgb565_pack(&tTargetPix);            \
            } while(0);

#define __ARM_2D_PIXEL_AVERAGE_RGB565(__PIXEL_IN, __ALPHA)                      \
    do {                                                                        \
        __arm_2d_color_fast_rgb_t tTempColour;                                  \
        __arm_2d_rgb565_unpack((__PIXEL_IN), &tTempColour);                     \
        tPixel.R += tTempColour.R * (__ALPHA);                                  \
        tPixel.G += tTempColour.G * (__ALPHA);                                  \
        tPixel.B += tTempColour.B * (__ALPHA);                                  \
    } while(0)


#define __ARM_2D_PIXEL_AVERAGE_CCCN888(__PIXEL_IN, __ALPHA)                     \
    do {                                                                        \
        arm_2d_color_rgb888_t tTempColour = {.tValue = (__PIXEL_IN)};           \
        tPixel.R += tTempColour.u8R * (__ALPHA);                                \
        tPixel.G += tTempColour.u8G * (__ALPHA);                                \
        tPixel.B += tTempColour.u8B * (__ALPHA);                                \
    } while(0)

#define __ARM_2D_PIXEL_BLENDING_CCCN888(__SRC_ADDR, __DES_ADDR, __OPACITY)      \
            do {                                                                \
                uint16_t chTransparency = 256 - (__OPACITY);                    \
                uint_fast8_t n = sizeof(uint32_t) - 1; /* do not change alpha */\
                const uint8_t *pchSrc = (uint8_t *)(__SRC_ADDR);                \
                uint8_t *pchDes = (uint8_t *)(__DES_ADDR);                      \
                                                                                \
                do {                                                            \
                    *pchDes = ( ((uint_fast16_t)(*pchSrc++) * chTransparency)   \
                              + ((uint_fast16_t)(*pchDes) * (__OPACITY))        \
                              ) >> 8;                                           \
                     pchDes++;                                                  \
                } while(--n);                                                   \
            } while(0)

/*============================ TYPES =========================================*/

typedef struct __arm_2d_point_adj_alpha_t{
    struct {
        arm_2d_location_t tOffset;
        uint8_t chAlpha;
    }tMatrix[4];
} __arm_2d_point_adj_alpha_t;




//! \name Operation Index: used for logging and debugging purpose
//! @{
enum {
    /*------------ arm-2d operation idx begin --------------*/
    __ARM_2D_OP_IDX_BARRIER,
    __ARM_2D_OP_IDX_SYNC = __ARM_2D_OP_IDX_BARRIER,
    
    __ARM_2D_OP_IDX_COPY,
    __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING,
    __ARM_2D_OP_IDX_COPY_WITH_MASK,
    __ARM_2D_OP_IDX_COPY_WITH_SRC_MASK,
    __ARM_2D_OP_IDX_COPY_WITH_DES_MASK,
    __ARM_2D_OP_IDX_FILL_COLOUR,
    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_COLOUR_KEYING,
    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK,
    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_AND_OPACITY,
    
    __ARM_2D_OP_IDX_ALPHA_BLENDING,
    __ARM_2D_OP_IDX_ALPHA_BLENDING_WITH_COLOUR_KEYING,
    __ARM_2D_OP_IDX_ALPHA_FILL_COLOUR,
    __ARM_2D_OP_IDX_ALPHA_FILL_COLOUR_WITH_COLOUR_KEYING,
    
    __ARM_2D_OP_IDX_DRAW_POINT,
    __ARM_2D_OP_IDX_DRAW_PATTERN,
    
    __ARM_2D_OP_IDX_COLOUR_FORMAT_CONVERSION,
    
    __ARM_2D_OP_IDX_ROTATE,
    __ARM_2D_OP_IDX_ROTATE_WITH_ALPHA
    /*------------ cmsisi-2d operation idx end --------------*/
};
//! @}


typedef struct __arm_2d_sub_task_t __arm_2d_sub_task_t;


typedef arm_fsm_rt_t __arm_2d_io_func_t(__arm_2d_sub_task_t *ptTask);

typedef struct __arm_2d_low_level_io_t {
    __arm_2d_io_func_t *SW;
    __arm_2d_io_func_t *HW;
} __arm_2d_low_level_io_t;


typedef struct __arm_2d_tile_param_t {
    void *              pBuffer;
    int32_t             nOffset;
    int16_t             iStride;
    arm_2d_color_info_t tColour;
    
    uint8_t             bInvalid            : 1;
    uint8_t             bDerivedResource    : 1;
    uint8_t                                 : 6;
    
    arm_2d_region_t     tValidRegion;
} __arm_2d_tile_param_t;

typedef struct __arm_2d_param_copy_t {
    __arm_2d_tile_param_t tSource;
    __arm_2d_tile_param_t tTarget;
    arm_2d_size_t       tCopySize;
} __arm_2d_param_copy_t;

typedef struct __arm_2d_param_copy_msk_t {
    implement(__arm_2d_param_copy_t);
    
    __arm_2d_tile_param_t tSrcMask;
    __arm_2d_tile_param_t tDesMask;
} __arm_2d_param_copy_msk_t;

typedef struct __arm_2d_param_copy_orig_t {
    implement(__arm_2d_param_copy_t);
    
    __arm_2d_tile_param_t tOrigin;
    
} __arm_2d_param_copy_orig_t;

typedef struct __arm_2d_param_fill_t {
    __arm_2d_tile_param_t tSource;
    __arm_2d_tile_param_t tTarget;
} __arm_2d_param_fill_t;

typedef struct __arm_2d_param_fill_msk_t {
    implement(__arm_2d_param_fill_t);
    
    __arm_2d_tile_param_t tSrcMask;
    __arm_2d_tile_param_t tDesMask;
} __arm_2d_param_fill_msk_t;

typedef struct __arm_2d_param_fill_orig_t {
    implement(__arm_2d_param_fill_t);
    
    __arm_2d_tile_param_t tOrigin;
    
} __arm_2d_param_fill_orig_t;

struct __arm_2d_sub_task_t{
ARM_PRIVATE(
    __arm_2d_sub_task_t         *ptNext;
    
    arm_2d_op_core_t            *ptOP;
    
    uint8_t         chLowLeveIOIndex;                                              //!< the type of IO interface
    uint8_t         bIsCPL              : 1;
    uint8_t                             : 7;
    uint16_t                            : 16;
    
    union {
        __arm_2d_tile_param_t           tTileProcess;
        __arm_2d_param_copy_t           tCopy;
        __arm_2d_param_fill_t           tFill;
        __arm_2d_param_copy_orig_t      tCopyOrig;
        __arm_2d_param_copy_msk_t       tCopyMask;
        __arm_2d_param_fill_orig_t      tFillOrig;
        __arm_2d_param_fill_msk_t       tFillMask;
    }Param;
)};


struct __arm_2d_op_control {
ARM_PRIVATE(
    __arm_2d_sub_task_t   *ptFreeList;
    struct {
        __arm_2d_sub_task_t   *ptHead;
        __arm_2d_sub_task_t   *ptTail;
    } TaskFIFO;
    
    struct {
        arm_2d_op_core_t   *ptHead;
        arm_2d_op_core_t   *ptTail;
    } OPFIFO;
    
    uint16_t                hwFreeCount;
    uint16_t                hwTaskCount;
    uint16_t                hwBookCount;
    uint16_t                            : 16;
    
    arm_2d_tile_t           *ptDefaultFrameBuffer;
    
    union {
        arm_2d_op_t                         tBasic;
        arm_2d_op_fill_cl_t                 tFillColour;
        arm_2d_op_fill_cl_msk_t             tFillColourMask;
        arm_2d_op_fill_cl_opc_t             tFillColourOpacity;
        arm_2d_op_src_t                     tWithSource;
        
        arm_2d_op_alpha_t                   tAlpha;
        arm_2d_op_alpha_cl_key_t            tAlphaColourKeying;
        arm_2d_op_alpha_fill_cl_msk_opc_t   tAlphaFillColourMaskOpacity;
        arm_2d_op_cp_msk_t                  tCopyMasks;
        
        arm_2d_op_drw_patn_t                tDrawPattern;
        arm_2d_op_rotate_t                  tRotate;
        arm_2d_op_rotate_opacity_t          tRotateOpacity;
        
        arm_2d_op_msk_t                     tBasicMask;
        arm_2d_op_src_msk_t                 tSourceMask;
        arm_2d_op_src_orig_msk_t            tSourceOrigMask;
    } DefaultOP;
)};



/*============================ GLOBAL VARIABLES ==============================*/
extern 
const struct __arm_2d_io_table __ARM_2D_IO_TABLE;

extern struct __arm_2d_op_control ARM_2D_CTRL;

/*============================ PROTOTYPES ====================================*/

/*----------------------------------------------------------------------------*
 * Pipeline                                                                   *
 *----------------------------------------------------------------------------*/
extern
arm_fsm_rt_t __arm_2d_op_invoke(arm_2d_op_core_t *ptOP);

extern 
/*! \brief sync up with operation 
 *! \retval true operation is busy
 *! \retval false operation isn't busy
 */
bool __arm_2d_op_acquire(arm_2d_op_core_t *ptOP);

/*! \note This API should be called by both arm_2d_task and hardware 
 *!       accelerator to indicate the completion of a sub task
 */
extern
void __arm_2d_notify_sub_task_cpl(__arm_2d_sub_task_t *ptTask, 
                                    arm_fsm_rt_t tResult,
                                    bool bFromHW);

extern 
arm_fsm_rt_t __arm_2d_op_frontend_op_decoder(arm_2d_op_core_t *ptThis);


extern
arm_fsm_rt_t __arm_2d_op_depose(arm_2d_op_core_t *ptThis, 
                                arm_fsm_rt_t tResult);
/*----------------------------------------------------------------------------*
 * Utilities                                                                  *
 *----------------------------------------------------------------------------*/
extern
__arm_2d_point_adj_alpha_t 
__arm_2d_point_get_adjacent_alpha_fp(arm_2d_point_float_t *ptPoint);

extern
__arm_2d_point_adj_alpha_t 
__arm_2d_point_get_adjacent_alpha_q16(arm_2d_point_fx_t *ptPoint);

/*----------------------------------------------------------------------------*
 * Default Software Implementations                                           *
 *----------------------------------------------------------------------------*/
        
extern 
arm_fsm_rt_t __arm_2d_sw_draw_point(__arm_2d_sub_task_t *ptTask);      

extern 
arm_fsm_rt_t __arm_2d_c8bit_sw_draw_pattern( __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb16_sw_draw_pattern( __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb32_sw_draw_pattern( __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_fill( __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_fill( __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_fill( __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_copy(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_copy(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_copy(  __arm_2d_sub_task_t *ptTask);


extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_masks(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_masks(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_masks(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_masks(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_masks(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_masks(
                                                __arm_2d_sub_task_t *ptTask);



extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_src_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_src_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_src_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_src_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_src_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_src_mask(
                                                __arm_2d_sub_task_t *ptTask);


extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_des_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_des_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_des_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_des_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_des_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_des_mask(
                                                __arm_2d_sub_task_t *ptTask);
                                                

extern 
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_copy_with_colour_keying(
                                                __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_copy_with_colour_keying(
                                        __arm_2d_sub_task_t *ptTask);
                           
extern 
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_copy_with_colour_keying(
                                        __arm_2d_sub_task_t *ptTask);
                           
extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_fill_with_colour_keying( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_fill_with_colour_keying( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_fill_with_colour_keying( 
                                        __arm_2d_sub_task_t *ptTask);
       
extern 
arm_fsm_rt_t __arm_2d_c8bit_sw_colour_filling(__arm_2d_sub_task_t *ptTask);
       
extern 
arm_fsm_rt_t __arm_2d_rgb16_sw_colour_filling(__arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb32_sw_colour_filling(__arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_mask(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_mask(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_mask(
                                        __arm_2d_sub_task_t *ptTask);


extern 
arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_mask_and_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_mask_and_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_mask_and_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_alpha_blending(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_alpha_blending(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_alpha_blending(__arm_2d_sub_task_t *ptTask);
 

extern
arm_fsm_rt_t __arm_2d_gray8_sw_alpha_blending_with_colour_keying(
                                        __arm_2d_sub_task_t *ptTask);
                                        
extern
arm_fsm_rt_t __arm_2d_rgb565_sw_alpha_blending_with_colour_keying(
                                        __arm_2d_sub_task_t *ptTask);
                                        
extern
arm_fsm_rt_t __arm_2d_cccn888_sw_alpha_blending_with_colour_keying(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_sw_convert_colour_to_rgb565(  
                                    __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_sw_convert_colour_to_rgb888(  
                                    __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_rotate(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_rotate(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_rotate(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_rotate_with_alpha(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_rotate_with_alpha(__arm_2d_sub_task_t *ptTask);

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_warning 174,177,188,68,513,144,64
#endif

#ifdef   __cplusplus
}
#endif

#endif


