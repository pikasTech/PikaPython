/*
 * Copyright (c) 2009-2022 Arm Limited. All rights reserved.
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

#ifndef __ARM_2D_USER_CFG_H__
#define __ARM_2D_USER_CFG_H__

/*============================ INCLUDES ======================================*/

#include "RTE_Components.h"

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/


//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>Arm-2D General Configurations
// =======================
//
// <q>Enable Asynchronous Programmers' model support
// <i> Note that if you don't have any hardware accelerator, disable this feature can reduce code size and gain a small performance uplift.
// <i> This feature is enabled by default.
#ifndef __ARM_2D_HAS_ASYNC__
#   define __ARM_2D_HAS_ASYNC__                                     0
#endif

// <q>Enable anti-alias support for all tranform operations.
// <i> Note that enabling this feature suffers a non-negligible performance drop.
// <i> This feature is disabled by default.
#ifndef __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
#   define __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__                      1
#endif

// <q>Enable support for accessing individual colour channels
// <i> Note that enabling this feature will add the support for a special colour type: ARM_2D_CHANNEL_8in32
// <i> This feature is disabled by default to save code size
#ifndef __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
#   define __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__             1
#endif
// </h>

// <h>Patches for improving performance
// =======================
// 
// <c1> Do NOT treat alpha value 255 as completely opaque in mask related operations
// <i> When define this macro, alpha value 0xFF will not be treated as opaque in mask related operations and you can barely see the background. Defining this macro improves performance.
//#define __ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__  
// </c>

// <c1> Ignore calibrartion for small angles in transform operations
// <i> This option is used to speed up M-cores without DSP support. It skips saturation in the QADD/QDADD/QDSUB involved in the rotation. The chances of overflow remain low as elements involved are using non-accumulating Q15.16 format and integer parts are in the range of the screen size providing enough margin.
//#define __ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__ 
// </c>

// <c1> Ignore satuation protection in fixed-point operations
// <i> This option is used to remove calibration in angle computations to gain a better performance, small error might be noticible for angles like 90, 180, 270 etc.
//#define __ARM_2D_CFG_UNSAFE_NO_SATURATION_IN_FIXED_POINT__ 
// </c>


// <q> Optimize the scaler version of transform operations for pointer-like resources
// <i> This feature is enabled by default. There is no guarantee that the performance will increase or decrease. It is all depends your applications. In most of the case, enabling it helps.
// <i> This feature has no meaning when the anti-alias transform is disabled or the helium acceleration is available.
#ifndef __ARM_2D_CFG_OPTIMIZE_FOR_POINTER_LIKE_SHAPES_IN_TRANSFORM__
#   define __ARM_2D_CFG_OPTIMIZE_FOR_POINTER_LIKE_SHAPES_IN_TRANSFORM__     1
#endif
// </h>


// <h>Extra Components
// =======================
//
#ifndef __GLCD_CFG_COLOUR_DEPTH__
// <o __GLCD_CFG_COLOUR_DEPTH__> Select the screen colour depth
//     <8=>     8 Bits
//     <16=>    16Bits
//     <32=>    32Bits
// <i> The colour depth of your LCD
// <i> Default: 16
#   define __GLCD_CFG_COLOUR_DEPTH__                                    16
#endif

// <o>Width of the screen <8-32767>
// <i> The width of your screen
// <i> Default: 320
#ifndef __GLCD_CFG_SCEEN_WIDTH__
#   define __GLCD_CFG_SCEEN_WIDTH__                                     240
#endif

// <o>Height of the screen <8-32767>
// <i> The height of your screen
// <i> Default: 240
#ifndef __GLCD_CFG_SCEEN_HEIGHT__
#   define __GLCD_CFG_SCEEN_HEIGHT__                                    240
#endif

// <o> The size of the LCD printf text buffer <16-65535>
// <i> The text buffer size for the lcd printf service. It determins how many character you can use in one printf string.
#ifndef __LCD_PRINTF_CFG_TEXT_BUFFER_SIZE__
#   define __LCD_PRINTF_CFG_TEXT_BUFFER_SIZE__                          64
#endif

// <h>Benchmark
// <o>Width of the PFB block <8-32767>
// <i> The width of your PFB block size used in arm-2d benchmark
#ifndef BENCHMARK_PFB_BLOCK_WIDTH
#   define BENCHMARK_PFB_BLOCK_WIDTH                                    240
#endif

// <o>Height of the PFB block <8-32767>
// <i> The height of your PFB block size used in arm-2d benchmark
#ifndef BENCHMARK_PFB_BLOCK_HEIGHT
#   define BENCHMARK_PFB_BLOCK_HEIGHT                                   240
#endif

// <o>PFB Block Count <1-65535>
// <i> The number of blocks in the PFB pool.
#ifndef BENCHMARK_PFB_HEAP_SIZE
#   define BENCHMARK_PFB_HEAP_SIZE                                      1
#endif

// <o>Width Alignment of generated PFBs
//     <0=>     1 pixel
//     <1=>     2 pixel
//     <2=>     4 pixel
//     <3=>     8 pixel
//     <4=>    16 pixel
//     <5=>    32 pixel
//     <6=>    64 pixel
//     <7=>   128 pixel
// <i> Make sure the x and width of the PFB is always aligned to 2^n pixels
#ifndef __BENCHMARK_PFB_PIXEL_ALIGN_WIDTH__
#   define __BENCHMARK_PFB_PIXEL_ALIGN_WIDTH__                          0
#endif

// <o>Height Alignment of generated PFBs
//     <0=>     1 pixel
//     <1=>     2 pixel
//     <2=>     4 pixel
//     <3=>     8 pixel
//     <4=>    16 pixel
//     <5=>    32 pixel
//     <6=>    64 pixel
//     <7=>   128 pixel
// <i> Make sure the y and height of the PFB is always aligned to 2^n pixels
#ifndef __BENCHMARK_PFB_PIXEL_ALIGN_HEIGHT__
#   define __BENCHMARK_PFB_PIXEL_ALIGN_HEIGHT__                         0
#endif

// <q> Swap the high and low bytes
// <i> Swap the high and low bytes of the 16bit-pixels
#ifndef __ARM_2D_CFG_BENCHMARK_SWAP_RGB16_HIGH_AND_LOW_BYTES__
#   define __ARM_2D_CFG_BENCHMARK_SWAP_RGB16_HIGH_AND_LOW_BYTES__       0
#endif

// <q>Enable the helper service for Asynchronous Flushing
// <i> Please select this option when using asynchronous flushing, e.g. DMA + ISR 
#ifndef __ARM_2D_CFG_BENCHMARK_ENABLE_ASYNC_FLUSHING__
#   define __ARM_2D_CFG_BENCHMARK_ENABLE_ASYNC_FLUSHING__               0
#endif

// <o>Number of iterations <1-2000>
// <i> run number of iterations in arm-2d benchmark before calculating the result.
#ifndef ITERATION_CNT
#   define ITERATION_CNT                                                1000
#endif

// <q>Use Tiny mode to run benchmark
// <i> Enable this mode to reduce the benchmark memory footprint (removing background picture etc.)
// <i> This feature is disabled by default.
#ifndef __ARM_2D_CFG_BENCHMARK_TINY_MODE__
#   define __ARM_2D_CFG_BENCHMARK_TINY_MODE__               0
#endif

// <q> Enable Stopwatch mode in the Benchmark:Watch-panel
// <i> Only update the second-hand (i.e. red pointer) every second in the watch-panel demo
// <i> This feature is disabled by default.
#ifndef __ARM_2D_CFG_WATCH_PANEL_STOPWATCH_MODE__
#   define __ARM_2D_CFG_WATCH_PANEL_STOPWATCH_MODE__        0
#endif


// <q> Exit benchmark when finished
// <i> Exit the arm_2d_run_benchmark() after running specified iterations
// <i> This feature is disabled by default.
#ifndef __ARM_2D_CFG_BENCHMARK_EXIT_WHEN_FINISH__
#   define __ARM_2D_CFG_BENCHMARK_EXIT_WHEN_FINISH__        0
#endif

//</h>
// </h>




// <<< end of configuration section >>>


/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#ifdef   __cplusplus
}
#endif

#endif
