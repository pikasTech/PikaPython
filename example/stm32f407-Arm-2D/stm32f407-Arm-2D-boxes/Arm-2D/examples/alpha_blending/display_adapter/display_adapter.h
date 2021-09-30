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

#ifndef __DISPLAY_ADAPTER_H__
#define __DISPLAY_ADAPTER_H__

/*============================ INCLUDES ======================================*/
#include "./app_cfg.h"
#include "arm_2d.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __declare_tile(__NAME)                                                  \
            extern const arm_2d_tile_t __NAME;
#define declare_tile(__NAME)            __declare_tile(__NAME)

#define __implement_tile(__NAME, __WIDTH, __HEIGHT, __TYPE)                     \
            ARM_NOINIT static __TYPE                                            \
                __NAME##Buffer[(__WIDTH) * (__HEIGHT)];                         \
            const arm_2d_tile_t __NAME = {                                      \
                .tRegion = {                                                    \
                    .tSize = {(__WIDTH), (__HEIGHT)},                           \
                },                                                              \
                .tInfo.bIsRoot = true,                                          \
                .pchBuffer = (uint8_t *)__NAME##Buffer,                         \
            }
            
#define implement_tile(__NAME, __WIDTH, __HEIGHT, __TYPE)                       \
            __implement_tile(__NAME, __WIDTH, __HEIGHT, __TYPE)
                        
#define get_tile_buffer_pixel_count(__NAME)                                     \
            (uint32_t)((__NAME.tRegion.tSize.iWidth) * (__NAME.tRegion.tSize.iHeight))
            
#define get_root_tile_buffer_size(__NAME, __TYPE)                               \
            (get_2d_layer_buffer_pixel_count(__NAME) * sizeof(TYPE))


/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern 
void platform_disp_init(void);

extern 
arm_2d_tile_t *platform_disp_buffer_get(void);

extern 
arm_fsm_rt_t platform_disp_buffer_refresh(void);

extern
arm_fsm_rt_t platform_disp_draw_tile(const arm_2d_tile_t *ptTile,
                                     const arm_2d_region_t *ptRegion);
                                     


#endif
