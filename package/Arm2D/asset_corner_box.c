/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
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

/*============================ INCLUDES ======================================*/
#include "asset_corner_box.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "Arm2D_common.h"
#include <math.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Winitializer-overrides"
#endif

/*============================ MACROS ========================================*/

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern
const arm_2d_tile_t c_tileWhiteDotAlphaQuarter;

/*============================ PROTOTYPES ====================================*/
__attribute__((nothrow)) 
extern int64_t clock(void);

/*============================ LOCAL VARIABLES ===============================*/

declare_tile(s_tCorner)
implement_tile(s_tCorner, 7, 7, uint8_t,
    .tInfo = {
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {
            .chScheme = ARM_2D_COLOUR_8BIT,
        },
    },
)

/*============================ IMPLEMENTATION ================================*/

void draw_round_corner_box( const arm_2d_tile_t *ptTarget, 
                            const arm_2d_region_t *ptRegion,
                            uint16_t hwColour,
                            uint8_t chAlpha,
                            bool bIsNewFrame)
{
    ARM_2D_UNUSED(bIsNewFrame);

    if (NULL == ptRegion) {
        ptRegion = (const arm_2d_region_t *)&(ptTarget->tRegion);
    }
    
    arm_2d_region_t tRegion = *ptRegion;
    
    //! copy the top left corner
    arm_2d_c8bit_tile_copy( &c_tileWhiteDotAlphaQuarter, 
                            &s_tCorner, 
                            NULL, 
                            ARM_2D_CP_MODE_COPY     |
                            ARM_2D_CP_MODE_X_MIRROR |
                            ARM_2D_CP_MODE_Y_MIRROR);
                            
    arm_2d_rgb565_fill_colour_with_mask_and_opacity(   
                                            ptTarget, 
                                            &tRegion, 
                                            &s_tCorner, 
                                            (arm_2d_color_rgb565_t){hwColour},
                                            chAlpha);
                                                
    arm_2d_op_wait_async(NULL);

    //! copy the top right corner
    tRegion.tLocation.iX += ptRegion->tSize.iWidth - s_tCorner.tRegion.tSize.iWidth;
    arm_2d_c8bit_tile_copy( &c_tileWhiteDotAlphaQuarter, 
                            &s_tCorner, 
                            NULL, 
                            ARM_2D_CP_MODE_COPY |
                            ARM_2D_CP_MODE_Y_MIRROR);
                            
    arm_2d_rgb565_fill_colour_with_mask_and_opacity(   
                                            ptTarget, 
                                            &tRegion, 
                                            &s_tCorner, 
                                            (arm_2d_color_rgb565_t){hwColour},
                                            chAlpha);

    arm_2d_op_wait_async(NULL);

    arm_2dp_rgb565_fill_colour_with_opacity(   
        NULL,
        ptTarget, 
        &(arm_2d_region_t) {
            .tSize = {
                .iHeight = s_tCorner.tRegion.tSize.iHeight,
                .iWidth = tRegion.tSize.iWidth - s_tCorner.tRegion.tSize.iWidth * 2,
            },
            .tLocation = {
                .iX = ptRegion->tLocation.iX + s_tCorner.tRegion.tSize.iWidth,
                .iY = ptRegion->tLocation.iY,
            },
        }, 
        (arm_2d_color_rgb565_t){hwColour},
        chAlpha);
    
    arm_2d_op_wait_async(NULL);

    arm_2dp_rgb565_fill_colour_with_opacity(   
        NULL,
        ptTarget, 
        &(arm_2d_region_t) {
            .tSize = {
                .iHeight = tRegion.tSize.iHeight - s_tCorner.tRegion.tSize.iHeight * 2,
                .iWidth = tRegion.tSize.iWidth,
            },
            .tLocation = {
                .iX = ptRegion->tLocation.iX,
                .iY = ptRegion->tLocation.iY + s_tCorner.tRegion.tSize.iHeight,
            },
        }, 
        (arm_2d_color_rgb565_t){hwColour},
        chAlpha);

    arm_2d_op_wait_async(NULL);
                            
    //! copy the bottom right corner 
    tRegion.tLocation.iY += ptRegion->tSize.iHeight - s_tCorner.tRegion.tSize.iHeight;
    arm_2d_c8bit_tile_copy( &c_tileWhiteDotAlphaQuarter, 
                            &s_tCorner, 
                            NULL, 
                            ARM_2D_CP_MODE_COPY);

    arm_2d_rgb565_fill_colour_with_mask_and_opacity(   
                                            ptTarget, 
                                            &tRegion, 
                                            &s_tCorner, 
                                            (arm_2d_color_rgb565_t){hwColour},
                                            chAlpha);

    arm_2d_op_wait_async(NULL);

    //! copy the bottom left corner 
    tRegion.tLocation.iX = ptRegion->tLocation.iX;
    arm_2d_c8bit_tile_copy( &c_tileWhiteDotAlphaQuarter, 
                            &s_tCorner, 
                            NULL, 
                            ARM_2D_CP_MODE_COPY     |
                            ARM_2D_CP_MODE_X_MIRROR );
                            
    arm_2d_rgb565_fill_colour_with_mask_and_opacity(  
                                            ptTarget, 
                                            &tRegion, 
                                            &s_tCorner, 
                                            (arm_2d_color_rgb565_t){hwColour},
                                            chAlpha);

    arm_2d_op_wait_async(NULL);

    arm_2dp_rgb565_fill_colour_with_opacity(   
        NULL,
        ptTarget, 
        &(arm_2d_region_t) {
            .tSize = {
                .iHeight = s_tCorner.tRegion.tSize.iHeight,
                .iWidth = tRegion.tSize.iWidth - s_tCorner.tRegion.tSize.iWidth * 2,
            },
            .tLocation = {
                .iX = tRegion.tLocation.iX + s_tCorner.tRegion.tSize.iWidth,
                .iY = tRegion.tLocation.iY,
            },
        }, 
        (arm_2d_color_rgb565_t){hwColour},
        chAlpha);

    arm_2d_op_wait_async(NULL);
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
