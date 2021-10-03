/*
 * Copyright (C) 2020 Arm Limited or its affiliates. All rights reserved.
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
 * $Date:        01. December 2020
 * $Revision:    V.0.5.0
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
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern
const __arm_2d_op_info_t ARM_2D_OP_BARRIER;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_RGB32;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_MASKING_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_MASKING_RGB32;
    
extern 
const __arm_2d_op_info_t ARM_2D_OP_COLOUR_FILL_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_COLOUR_FILL_RGB32;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_RGB888;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_WITH_COLOUR_MASKING_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_WITH_COLOUR_MASKING_RGB888;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_COLOUR_FILL_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_COLOUR_FILL_RGB888;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_DRAW_POINT_RGB16;

extern
const __arm_2d_op_info_t ARM_2D_OP_DRAW_POINT_RGB32;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_DRAW_PATTERN_RGB16;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_DRAW_PATTERN_RGB32;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_CONVERT_TO_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_CONVERT_TO_RGB888;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ROTATE_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ROTATE_RGB888;

extern
const __arm_2d_op_info_t ARM_2D_OP_ROTATE_WITH_ALPHA_RGB565;
    
extern
const __arm_2d_op_info_t ARM_2D_OP_ROTATE_WITH_ALPHA_RGB888;

/*============================ PROTOTYPES ====================================*/


#ifdef   __cplusplus
}
#endif


#endif
