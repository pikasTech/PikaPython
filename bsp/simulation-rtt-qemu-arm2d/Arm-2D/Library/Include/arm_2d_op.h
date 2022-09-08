/*
 * Copyright (C) 2022 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        #include "arm_2d.h"
 * Description:  Public header file to contain the all avaialble Arm-2D 
 *               interface header files 
 *
 * $Date:        17. May 2022
 * $Revision:    V.1.2.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */
 
#ifndef __ARM_2D_OP_H__
#define __ARM_2D_OP_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d_types.h"

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#define LOW_LEVEL_IO__ARM_2D_IO_NONE    (*(void *)NULL)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

/*----------------------------------------------------------------------------*
 * Misc                                                                       *
 *----------------------------------------------------------------------------*/
extern
const __arm_2d_op_info_t ARM_2D_OP_BARRIER;

/*----------------------------------------------------------------------------*
 * Basic Tile Operation (Core)                                                *
 *----------------------------------------------------------------------------*/
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_C8BIT;    

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_RGB32;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_ONLY_C8BIT;    

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_ONLY_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_ONLY_RGB32;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_X_MIRROR_C8BIT;    

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_X_MIRROR_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_X_MIRROR_RGB32;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_Y_MIRROR_C8BIT;    

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_Y_MIRROR_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_Y_MIRROR_RGB32;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_XY_MIRROR_C8BIT;    

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_XY_MIRROR_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_XY_MIRROR_RGB32;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_ONLY_C8BIT;    

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_ONLY_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_ONLY_RGB32;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_X_MIRROR_C8BIT;    

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_X_MIRROR_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_X_MIRROR_RGB32;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_Y_MIRROR_C8BIT;    

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_Y_MIRROR_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_Y_MIRROR_RGB32;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_XY_MIRROR_C8BIT;    

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_XY_MIRROR_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_XY_MIRROR_RGB32;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_C8BIT;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_RGB32;


/*----------------------------------------------------------------------------*
 * Alpha-Blending and Masks                                                   *
 *----------------------------------------------------------------------------*/
extern 
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_GRAY8;

extern 
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_CCCN888;

extern 
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_AND_OPACITY_GRAY8;

extern 
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_AND_OPACITY_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_AND_OPACITY_CCCN888;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_GRAY8;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_RGB888;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_WITH_COLOUR_KEYING_GRAY8;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_WITH_COLOUR_KEYING_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_WITH_COLOUR_KEYING_RGB888;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_COLOUR_FILL_GRAY8;

extern
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_COLOUR_FILL_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_COLOUR_FILL_RGB888;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_GRAY8;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_RGB565;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_CCCN888;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_GRAY8;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_RGB565;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_CCCN888;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_GRAY8;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_RGB565;

extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_CCCN888;

/*----------------------------------------------------------------------------*
 * Basic Drawing                                                              *
 *----------------------------------------------------------------------------*/
extern 
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_C8BIT;

extern 
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_RGB32; 

extern
const __arm_2d_op_info_t ARM_2D_OP_DRAW_POINT_C8BIT;
  
extern
const __arm_2d_op_info_t ARM_2D_OP_DRAW_POINT_RGB16;

extern
const __arm_2d_op_info_t ARM_2D_OP_DRAW_POINT_RGB32;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_DRAW_PATTERN_C8BIT;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_DRAW_PATTERN_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_DRAW_PATTERN_RGB32;

/*----------------------------------------------------------------------------*
 * Colour Conversion                                                          *
 *----------------------------------------------------------------------------*/
extern
const __arm_2d_op_info_t ARM_2D_OP_CONVERT_TO_GRAY8;

extern
const __arm_2d_op_info_t ARM_2D_OP_CONVERT_TO_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_CONVERT_TO_RGB888;

/*----------------------------------------------------------------------------*
 * Transform                                                                  *
 *----------------------------------------------------------------------------*/
extern
const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_GRAY8;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_CCCN888;

extern
const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_OPACITY_GRAY8;

extern
const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_OPACITY_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_OPACITY_CCCN888;

extern
const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_GRAY8;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_CCCN888;

extern
const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_GRAY8;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_CCCN888;

/*============================ PROTOTYPES ====================================*/


#ifdef   __cplusplus
}
#endif


#endif
