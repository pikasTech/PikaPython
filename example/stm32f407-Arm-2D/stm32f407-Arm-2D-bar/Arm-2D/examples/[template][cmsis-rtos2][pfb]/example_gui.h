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

#ifndef __EXAMPLE_GUI_H__
#define __EXAMPLE_GUI_H__

/*============================ INCLUDES ======================================*/
#include "app_cfg.h"
#include <stdint.h>
#include "arm_2d.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct arm_2d_layer_t {
    const arm_2d_tile_t *ptTile;
    arm_2d_region_t tRegion;
    uint32_t    wMode;
    uint8_t     chTransparency;
    uint8_t     bIsIrregular    : 1;
    uint8_t                     : 7;
    uint16_t    hwMaskingColour;
} arm_2d_layer_t;

typedef struct floating_range_t {
    arm_2d_region_t tRegion;
    arm_2d_layer_t *ptLayer;
    arm_2d_location_t tOffset;
} floating_range_t;



/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
void example_gui_init(void);

extern
void example_gui_refresh(const arm_2d_tile_t *ptFrameBuffer, bool bIsNewFrame);
                            
extern
void example_gui_do_events(void);


#endif
