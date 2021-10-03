/*
 * Copyright (c) 2009-2020 Arm Limited. All rights reserved.
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
#include "./app_cfg.h"
#include <stdbool.h>
#include "./display_adapter.h"
#include "../platform/platform.h"
#include <assert.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-qual"
#endif

/*============================ MACROS ========================================*/


#if defined(__STDC_VERSION__) && __STDC_VERSION__>=201112L

#if !(__IS_COMPILER_ARM_COMPILER_6__ && __ARMCC_VERSION < 6160001)

//! C11
static_assert(  APP_SCREEN_WIDTH <= GLCD_WIDTH, 
                "APP_SCREEN_WIDTH should be no larger than GLCD_WIDTH");

static_assert(  APP_SCREEN_HEIGHT <= GLCD_HEIGHT, 
                "APP_SCREEN_HEIGHT should be no larger than GLCD_HEIGHT");
                
#endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

declare_tile(c_tLCDDisplayBuffer)    

implement_tile(c_tLCDDisplayBuffer, APP_SCREEN_WIDTH, APP_SCREEN_HEIGHT, arm_2d_color_rgb565_t);



/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void platform_disp_init(void)
{
    arm_2d_rgb16_fill_colour(&c_tLCDDisplayBuffer, NULL, GLCD_COLOR_BLACK);
}

arm_2d_tile_t *platform_disp_buffer_get(void)
{
    return (arm_2d_tile_t *)&c_tLCDDisplayBuffer;
}

arm_fsm_rt_t platform_disp_buffer_refresh(void)
{
    GLCD_DrawBitmap((GLCD_WIDTH - APP_SCREEN_WIDTH) >> 1,
                    (GLCD_HEIGHT - APP_SCREEN_HEIGHT) >> 1, 
                    APP_SCREEN_WIDTH, 
                    APP_SCREEN_HEIGHT, 
                    c_tLCDDisplayBuffer.pchBuffer);
    
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t platform_disp_draw_tile(const arm_2d_tile_t *ptTile,
                                     const arm_2d_region_t *ptRegion)
{

    DISP_ADAPT_ASSERT(NULL != ptTile);
    DISP_ADAPT_ASSERT(NULL != ptRegion);
    DISP_ADAPT_ASSERT(NULL != ptTile->pchBuffer);
    
    arm_2d_rgb16_tile_copy(ptTile, &c_tLCDDisplayBuffer, ptRegion, false);
    
    GLCD_DrawBitmap(0,0, APP_SCREEN_WIDTH, APP_SCREEN_HEIGHT, c_tLCDDisplayBuffer.pchBuffer);
    
    return arm_fsm_rt_cpl;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
