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
#include <stdio.h>
#include "platform.h"
#include "example_gui.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "./controls/controls.h"

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
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define arm_2d_layer(__TILE_ADDR, __TRANS, __X, __Y, ...)                       \
    {                                                                           \
        .ptTile = (__TILE_ADDR),                                                \
        .tRegion.tLocation.iX = (__X),                                          \
        .tRegion.tLocation.iY = (__Y),                                          \
        .chTransparency = (__TRANS),                                            \
        __VA_ARGS__                                                             \
    }

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

declare_tile(c_tLayerB)
implement_tile(c_tLayerB, 200, 50, arm_2d_color_rgb565_t);

/*! picture helium */
extern
const arm_2d_tile_t c_tPictureHeliun ;


/*! picture cmsis_logo */
extern const uint8_t c_bmpCMSISLogo[163 * 65 * sizeof(uint16_t)];
const static arm_2d_tile_t c_tLogoCMSIS = {
    .tRegion = {
        .tSize = {
            .iWidth = 163,
            .iHeight = 65
        },
    },
    .tInfo = {
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {
            .chScheme = ARM_2D_COLOUR_RGB565,
        },
    },
    .pwBuffer = (uint32_t *)c_bmpCMSISLogo,
};

extern const uint8_t c_bmpSun[56*57*sizeof(uint16_t)];
const static arm_2d_tile_t c_tPictureSun = {
    .tRegion = {
        .tSize = {
            .iWidth = 56,
            .iHeight = 57
        },
    },
    .tInfo.bIsRoot = true,
    .phwBuffer = (uint16_t *)c_bmpSun,
};

extern const uint8_t c_bmpWhiteDot[19*20*sizeof(uint16_t)];
extern const arm_2d_tile_t c_tPictureWhiteDot;


static arm_2d_layer_t s_ptRefreshLayers[] = {
    arm_2d_layer(&c_tPictureHeliun, 0, -50, -100),

    arm_2d_layer(NULL, 128, 10, 80, 
                .tRegion.tSize.iWidth = 100, 
                .tRegion.tSize.iHeight = 100
                ),
    arm_2d_layer(&c_tLayerB, 144, 50, 150),
    arm_2d_layer(&c_tPictureSun, 0, 0, 0, 
                    .bIsIrregular = true, 
                    .hwMaskingColour = GLCD_COLOR_WHITE),
};

static floating_range_t s_ptFloatingBoxes[] = {
    {
        .tRegion = {{0-100, 0-100}, {APP_SCREEN_WIDTH + 200, 256 + 200}},
        .ptLayer = &s_ptRefreshLayers[0],
        .tOffset = {-1, -1},
    },
    {
        .tRegion = {{0, 0}, {APP_SCREEN_WIDTH, APP_SCREEN_HEIGHT}},
        .ptLayer = &s_ptRefreshLayers[1],
        .tOffset = {5, -2},
    },
    {
        .tRegion = {{0, 0}, {APP_SCREEN_WIDTH, APP_SCREEN_HEIGHT}},
        .ptLayer = &s_ptRefreshLayers[2],
        .tOffset = {-2, 4},
    },
    {
        .tRegion = {{-100, -100}, {APP_SCREEN_WIDTH+200, APP_SCREEN_HEIGHT+200}},
        .ptLayer = &s_ptRefreshLayers[3],
        .tOffset = {5, 5},
    },
};

/*============================ IMPLEMENTATION ================================*/

static volatile uint32_t s_wSystemTimeInMs = 0;
static volatile bool s_bTimeout = false;
extern void platform_1ms_event_handler(void);

void platform_1ms_event_handler(void)
{
    s_wSystemTimeInMs++; 
    if (!(s_wSystemTimeInMs & (_BV(10) - 1))) {
        s_bTimeout = true;
    }
}

void example_gui_init(void)
{
    controls_init();
    
    s_ptRefreshLayers[0].wMode = ARM_2D_CP_MODE_FILL;

    //arm_2d_rgb16_fill_colour(s_ptRefreshLayers[1].ptTile, NULL, GLCD_COLOR_RED);
    arm_2d_rgb16_fill_colour(s_ptRefreshLayers[2].ptTile, NULL, GLCD_COLOR_GREEN);

    arm_foreach(arm_2d_layer_t, s_ptRefreshLayers) {
        arm_2d_region_t tRegion = _->tRegion;
        if (!tRegion.tSize.iHeight) {
            tRegion.tSize.iHeight = _->ptTile->tRegion.tSize.iHeight;
        }
        if (!tRegion.tSize.iWidth) {
            tRegion.tSize.iWidth = _->ptTile->tRegion.tSize.iWidth;
        }
        
        _->tRegion = tRegion;
    }
}

__WEAK 
void example_gui_on_refresh_evt_handler(const arm_2d_tile_t *ptFrameBuffer)
{
     ARM_2D_UNUSED(ptFrameBuffer);
}


static void example_update_boxes(floating_range_t *ptBoxes, uint_fast16_t hwCount)
{
    ASSERT(NULL != ptBoxes);
    ASSERT(hwCount > 0);

    do {
        arm_2d_region_t tOldRegion = ptBoxes->ptLayer->tRegion;
        if (   (tOldRegion.tLocation.iX + tOldRegion.tSize.iWidth + ptBoxes->tOffset.iX)
            >= ptBoxes->tRegion.tLocation.iX + ptBoxes->tRegion.tSize.iWidth) {
            ptBoxes->tOffset.iX = -ptBoxes->tOffset.iX;
        }

        if (    (tOldRegion.tLocation.iX + ptBoxes->tOffset.iX)
            <   (ptBoxes->tRegion.tLocation.iX)) {
            ptBoxes->tOffset.iX = -ptBoxes->tOffset.iX;
        }

        if (   (tOldRegion.tLocation.iY + tOldRegion.tSize.iHeight + ptBoxes->tOffset.iY)
            >= ptBoxes->tRegion.tLocation.iY + ptBoxes->tRegion.tSize.iHeight) {
            ptBoxes->tOffset.iY = -ptBoxes->tOffset.iY;
        }

        if (    (tOldRegion.tLocation.iY + ptBoxes->tOffset.iY)
            <   (ptBoxes->tRegion.tLocation.iY)) {
            ptBoxes->tOffset.iY = -ptBoxes->tOffset.iY;
        }

        ptBoxes->ptLayer->tRegion.tLocation.iX += ptBoxes->tOffset.iX;
        ptBoxes->ptLayer->tRegion.tLocation.iY += ptBoxes->tOffset.iY;

        ptBoxes++;

    }while(--hwCount);
}

void example_gui_do_events(void)
{
    static uint32_t s_wCounter = 0;

    example_update_boxes(s_ptFloatingBoxes, dimof(s_ptFloatingBoxes));
    
    if (s_bTimeout) {                       //!< every 1024 ms
        s_bTimeout = false;
        
        if((s_wCounter & 0x0F) < 0x04) {
            s_ptRefreshLayers[0].wMode =    ARM_2D_CP_MODE_FILL;
        } else if((s_wCounter & 0x0F) < 0x08) {
            s_ptRefreshLayers[0].wMode =    ARM_2D_CP_MODE_FILL       | 
                                            ARM_2D_CP_MODE_X_MIRROR;
        } else if((s_wCounter & 0x0F) < 0x0C) {
            s_ptRefreshLayers[0].wMode =    ARM_2D_CP_MODE_FILL       | 
                                            ARM_2D_CP_MODE_Y_MIRROR;
        } else {
            s_ptRefreshLayers[0].wMode =    ARM_2D_CP_MODE_FILL       | 
                                            ARM_2D_CP_MODE_X_MIRROR   | 
                                            ARM_2D_CP_MODE_Y_MIRROR;
        }
        
        s_wCounter++;
    }
}



static void __draw_layers(  const arm_2d_tile_t *ptFrameBuffer,
                            arm_2d_layer_t *ptLayers, 
                            uint_fast16_t hwCount,
                            bool bIsNewFrame)
{
    ASSERT(NULL != ptLayers);
    ASSERT(hwCount > 0);

    static const arm_2d_region_t tFillRegion = {-200, 
                                                -100, 
                                                APP_SCREEN_WIDTH + 200, 
                                                APP_SCREEN_HEIGHT + 100 };
    

    do {

        arm_2d_rgb16_tile_copy( &c_tLogoCMSIS,
                                ptFrameBuffer,
                                &tFillRegion,
                                ptLayers[0].wMode);
       
        
        arm_2d_rgb16_tile_copy_with_colour_masking( &c_tPictureSun,
                                                    s_ptRefreshLayers[2].ptTile,
                                                    NULL,
                                                    GLCD_COLOR_WHITE,
                                                    ARM_2D_CP_MODE_FILL);
        
        arm_foreach(arm_2d_layer_t, ptLayers, hwCount, ptLayer) {
            arm_2d_region_t tRegion = ptLayer->tRegion;

            if (NULL == ptLayer->ptTile) { 
                continue;
            }
            
            if (ptLayer->bIsIrregular) {
                arm_2d_rgb16_tile_copy_with_colour_masking( ptLayer->ptTile,
                                            ptFrameBuffer,
                                            &tRegion,
                                            ptLayer->hwMaskingColour,
                                            ARM_2D_CP_MODE_COPY);
            } else {
                if (ptLayer->chTransparency) {
                    arm_2d_rgb565_alpha_blending(   ptLayer->ptTile,
                                                    ptFrameBuffer,
                                                    &tRegion,
                                                    255 - ptLayer->chTransparency);
                } else {
                    arm_2d_rgb16_tile_copy( ptLayer->ptTile,
                                            ptFrameBuffer,
                                            &tRegion,
                                            ARM_2D_CP_MODE_COPY);
                }
            }
        }
        
        arm_2d_rgb565_fill_colour_with_alpha(   ptFrameBuffer, 
                                                &s_ptRefreshLayers[1].tRegion,
                                                (arm_2d_color_rgb565_t){.tValue =  GLCD_COLOR_RED},
                                                255 - s_ptRefreshLayers[1].chTransparency);
        
        //! show progress wheel
        busy_wheel_show(ptFrameBuffer, bIsNewFrame);

        example_gui_on_refresh_evt_handler(ptFrameBuffer);
        
    } while (0);
}

void example_gui_refresh(const arm_2d_tile_t *ptFrameBuffer, bool bIsNewFrame)
{
    __draw_layers(  ptFrameBuffer, 
                    s_ptRefreshLayers, 
                    dimof(s_ptRefreshLayers),
                    bIsNewFrame);
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


