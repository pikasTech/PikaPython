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

#ifndef __ARM_2D_HELPER_H__
#define __ARM_2D_HELPER_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "./arm_2d_helper_pfb.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __declare_tile(__NAME)                                                  \
            extern const arm_2d_tile_t __NAME;
#define declare_tile(__NAME)            __declare_tile(__NAME)

#define __implement_tile(__NAME, __WIDTH, __HEIGHT, __TYPE, ...)                \
            ARM_NOINIT static __TYPE                                            \
                __NAME##Buffer[(__WIDTH) * (__HEIGHT)];                         \
            const arm_2d_tile_t __NAME = {                                      \
                .tRegion = {                                                    \
                    .tSize = {(__WIDTH), (__HEIGHT)},                           \
                },                                                              \
                .tInfo.bIsRoot = true,                                          \
                .pchBuffer = (uint8_t *)__NAME##Buffer,                         \
                __VA_ARGS__                                                     \
            };
            
#define implement_tile(__NAME, __WIDTH, __HEIGHT, __TYPE, ...)                  \
            __implement_tile(__NAME, __WIDTH, __HEIGHT, __TYPE, ##__VA_ARGS__)
                        
#define get_tile_buffer_pixel_count(__NAME)                                     \
            (uint32_t)(     (__NAME.tRegion.tSize.iWidth)                       \
                        *   (__NAME.tRegion.tSize.iHeight))
            
#define get_tile_buffer_size(__NAME, __TYPE)                                    \
            (get_2d_layer_buffer_pixel_count(__NAME) * sizeof(TYPE))



#define __arm_2d_align_centre2(__region, __size)                                \
    for (arm_2d_region_t __centre_region = {                                    \
            .tSize = (__size),                                                  \
            .tLocation = {                                                      \
                .iX = ((__region).tRegion.tSize.iWidth - (__size).iWidth)  >> 1,\
                .iY = ((__region).tRegion.tSize.iHeight - (__size).iHeight)>> 1,\
            },                                                                  \
        },                                                                      \
        *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;                      \
         ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;                   \
        )
                
#define __arm_2d_align_centre3(__region, __width, __height)                     \
    for (arm_2d_region_t __centre_region = {                                    \
            .tSize = {                                                          \
                .iWidth = (__width),                                            \
                .iHeight = (__height),                                          \
            },                                                                  \
            .tLocation = {                                                      \
                .iX = ((__region).tRegion.tSize.iWidth - (__width))  >> 1,      \
                .iY = ((__region).tRegion.tSize.iHeight - (__height))>> 1,      \
            },                                                                  \
        },                                                                      \
        *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;                      \
         ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;                   \
         arm_2d_op_wait_async(NULL)                                             \
        )    

#define arm_2d_align_centre(...)                                                \
            ARM_CONNECT2(   __arm_2d_align_centre,                              \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
