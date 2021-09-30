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
 * Title:        arm-2d_op_table.c
 * Description:  Tables for pixel pipeline OPs
 *
 * $Date:        29 April 2021
 * $Revision:    V.0.8.0
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */


/*============================ INCLUDES ======================================*/
#define __ARM_2D_IMPL__

#include "arm_2d.h"
#include "__arm_2d_impl.h"

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
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144
#endif


/*============================ MACROS ========================================*/
#define LOW_LEVEL_IO__ARM_2D_IO_NONE    (*(void *)NULL)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

/*----------------------------------------------------------------------------*
 * Low Level IO Interfaces                                                    *
 *----------------------------------------------------------------------------*/
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_RGB16, __arm_2d_rgb16_sw_tile_copy);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_RGB32, __arm_2d_rgb32_sw_tile_copy);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_RGB16, __arm_2d_rgb16_sw_tile_fill);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_RGB32, __arm_2d_rgb32_sw_tile_fill);

__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_RGB16, 
                __arm_2d_rgb16_sw_tile_copy_with_colour_masking);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_RGB32, 
                __arm_2d_rgb32_sw_tile_copy_with_colour_masking);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_RGB16, 
                __arm_2d_rgb16_sw_tile_fill_with_colour_masking);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_RGB32, 
                __arm_2d_rgb32_sw_tile_fill_with_colour_masking);
                
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_RGB16, __arm_2d_rgb16_sw_colour_filling);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_RGB32, __arm_2d_rgb32_sw_colour_filling);

__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_RGB565, 
                __arm_2d_rgb565_sw_alpha_blending);
__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_RGB888, 
                __arm_2d_rgb888_sw_alpha_blending);

__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_WITH_COLOUR_MASKING_RGB565, 
                __arm_2d_rgb565_sw_alpha_blending_with_colour_masking);
__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_WITH_COLOUR_MASKING_RGB888, 
                __arm_2d_rgb888_sw_alpha_blending_with_colour_masking);
                
__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_FILL_COLOUR_RGB565, 
                __arm_2d_rgb565_sw_colour_filling_with_alpha);
__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_FILL_COLOUR_RGB888, 
                __arm_2d_rgb888_sw_colour_filling_with_alpha);
                
__WEAK
def_low_lv_io(__ARM_2D_IO_DRAW_POINT, __arm_2d_sw_draw_point);

__WEAK
def_low_lv_io(__ARM_2D_IO_DRAW_PATTERN_RGB16, __arm_2d_rgb16_sw_draw_pattern);
__WEAK
def_low_lv_io(__ARM_2D_IO_DRAW_PATTERN_RGB32, __arm_2d_rgb32_sw_draw_pattern);

__WEAK
def_low_lv_io(__ARM_2D_IO_COLOUR_CONVERT_TO_RGB565, 
                __arm_2d_sw_convert_colour_to_rgb565);
__WEAK
def_low_lv_io(__ARM_2D_IO_COLOUR_CONVERT_TO_RGB888, 
                __arm_2d_sw_convert_colour_to_rgb888);
                
__WEAK
def_low_lv_io(__ARM_2D_IO_ROTATE_RGB565, 
                __arm_2d_rgb565_sw_rotate);

__WEAK
def_low_lv_io(__ARM_2D_IO_ROTATE_RGB888, 
                __arm_2d_rgb888_sw_rotate);


__WEAK
def_low_lv_io(__ARM_2D_IO_ROTATE_WITH_ALPHA_RGB565, 
                __arm_2d_rgb565_sw_rotate_with_alpha);

__WEAK
def_low_lv_io(__ARM_2D_IO_ROTATE_WITH_ALPHA_RGB888, 
                __arm_2d_rgb888_sw_rotate_with_alpha);


/*----------------------------------------------------------------------------*
 * OP-Table                                                                   *
 *----------------------------------------------------------------------------*/

const __arm_2d_op_info_t ARM_2D_OP_BARRIER = {
    .Info = {
        //! control operation
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_BARRIER,
        
        .LowLevelIO = {
            .ptCopyLike = NULL, 
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_RGB16),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_RGB32),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_RGB32),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_MASKING_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB16,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_MASKING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_RGB16),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_MASKING_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB32,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_MASKING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_RGB32),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_RGB32),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_COLOUR_FILL_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB16,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_COLOUR_FILL_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB32,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_RGB32),
        },
    },
};
    
    
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ALPHA_BLENDING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_RGB565),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_RGB888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ALPHA_BLENDING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_RGB888),
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_WITH_COLOUR_MASKING_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ALPHA_BLENDING_WITH_COLOUR_MASKING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_WITH_COLOUR_MASKING_RGB565),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_BLENDING_WITH_COLOUR_MASKING_RGB888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ALPHA_BLENDING_WITH_COLOUR_MASKING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_WITH_COLOUR_MASKING_RGB888),
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_COLOUR_FILL_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ALPHA_FILL_COLOUR,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_ALPHA_FILL_COLOUR_RGB565),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_ALPHA_COLOUR_FILL_RGB888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB888,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ALPHA_FILL_COLOUR,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_ALPHA_FILL_COLOUR_RGB888),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_DRAW_POINT_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB16,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_DRAW_POINT,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_DRAW_POINT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_DRAW_POINT_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB32,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_DRAW_POINT,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_DRAW_POINT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_DRAW_PATTERN_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB16,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_DRAW_PATTERN,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_DRAW_PATTERN_RGB16),
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_DRAW_PATTERN_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB32,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_DRAW_PATTERN,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_DRAW_PATTERN_RGB32),
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_CONVERT_TO_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COLOUR_FORMAT_CONVERSION,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COLOUR_CONVERT_TO_RGB565),
            .ptFillLike = NULL,
        },
    },
};
    
    
const __arm_2d_op_info_t ARM_2D_OP_CONVERT_TO_RGB888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COLOUR_FORMAT_CONVERSION,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COLOUR_CONVERT_TO_RGB888),
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_ROTATE_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ROTATE,

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_ROTATE_RGB565),
            .ptFillOrigLike = NULL,
        },
    },
};
    
    
const __arm_2d_op_info_t ARM_2D_OP_ROTATE_RGB888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ROTATE,
        
        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_ROTATE_RGB888),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_ROTATE_WITH_ALPHA_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ROTATE_WITH_ALPHA,

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_ROTATE_WITH_ALPHA_RGB565),
            .ptFillOrigLike = NULL,
        },
    },
};
    
    
const __arm_2d_op_info_t ARM_2D_OP_ROTATE_WITH_ALPHA_RGB888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_ROTATE_WITH_ALPHA,
        
        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_ROTATE_WITH_ALPHA_RGB888),
            .ptFillOrigLike = NULL,
        },
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_warning 174,177,188,68,513,144
#endif

#ifdef   __cplusplus
}
#endif
