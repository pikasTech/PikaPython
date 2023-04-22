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

#ifndef __ARM_2D_DISP_ADAPTER0_H__
#define __ARM_2D_DISP_ADAPTER0_H__

#include "arm_2d.h"

#ifdef RTE_Acceleration_Arm_2D_Helper_Disp_Adapter0

#include "arm_2d_helper_scene.h"
#include "__common.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <o> Select the screen colour depth
//     <8=>     8 Bits
//     <16=>    16Bits
//     <32=>    32Bits
// <i> The colour depth of your screen
#ifndef __DISP0_CFG_COLOUR_DEPTH__
#   define __DISP0_CFG_COLOUR_DEPTH__                              16
#endif

// <o>Width of the screen <8-32767>
// <i> The width of your screen
// <i> Default: 320
#ifndef __DISP0_CFG_SCEEN_WIDTH__
#   define __DISP0_CFG_SCEEN_WIDTH__                               240
#endif

// <o>Height of the screen <8-32767>
// <i> The height of your screen
// <i> Default: 240
#ifndef __DISP0_CFG_SCEEN_HEIGHT__
#   define __DISP0_CFG_SCEEN_HEIGHT__                              240
#endif

// <o>Width of the PFB block
// <i> The width of your PFB block size used in disp0
#ifndef __DISP0_CFG_PFB_BLOCK_WIDTH__
#   define __DISP0_CFG_PFB_BLOCK_WIDTH__                           240
#endif

// <o>Height of the PFB block
// <i> The height of your PFB block size used in disp0
#ifndef __DISP0_CFG_PFB_BLOCK_HEIGHT__
#   define __DISP0_CFG_PFB_BLOCK_HEIGHT__                          60
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
#ifndef __DISP0_CFG_PFB_PIXEL_ALIGN_WIDTH__
#   define __DISP0_CFG_PFB_PIXEL_ALIGN_WIDTH__                     0
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
#ifndef __DISP0_CFG_PFB_PIXEL_ALIGN_HEIGHT__
#   define __DISP0_CFG_PFB_PIXEL_ALIGN_HEIGHT__                    0
#endif

// <o>PFB Block Count <1-65535>
// <i> The number of blocks in the PFB pool.
#ifndef __DISP0_CFG_PFB_HEAP_SIZE__
#   define __DISP0_CFG_PFB_HEAP_SIZE__                             1
#endif

// <o>Number of iterations <0-2000>
// <i> run number of iterations before calculate the FPS.
#ifndef __DISP0_CFG_ITERATION_CNT__
#   define __DISP0_CFG_ITERATION_CNT__                             30
#endif

// <o>FPS Calculation Mode
//     <0=>     Render-Only FPS
//     <1=>     Real FPS
// <i> Decide the meaning of the real time FPS display
#ifndef __DISP0_CFG_FPS_CACULATION_MODE__
#   define __DISP0_CFG_FPS_CACULATION_MODE__                       1
#endif

// <q> Enable Dirty Region Debug Mode
// <i> Draw dirty regions on the screen for debug.
#ifndef __DISP0_CFG_DEBUG_DIRTY_REGIONS__
#   define __DISP0_CFG_DEBUG_DIRTY_REGIONS__                       0
#endif

// <q> Swap the high and low bytes
// <i> Swap the high and low bytes of the 16bit-pixels
#ifndef __DISP0_CFG_SWAP_RGB16_HIGH_AND_LOW_BYTES__
#   define __DISP0_CFG_SWAP_RGB16_HIGH_AND_LOW_BYTES__             0
#endif

// <q>Enable the helper service for Asynchronous Flushing
// <i> Please select this option when using asynchronous flushing, e.g. DMA + ISR 
#ifndef __DISP0_CFG_ENABLE_ASYNC_FLUSHING__
#   define __DISP0_CFG_ENABLE_ASYNC_FLUSHING__                     0
#endif

// <q>Disable the default scene
// <i> Remove the default scene for this display adapter. We highly recommend you to disable the default scene when creating real applications.
#ifndef __DISP0_CFG_DISABLE_DEFAULT_SCENE__
#   define __DISP0_CFG_DISABLE_DEFAULT_SCENE__                     0
#endif

// <q>Disable the navigation layer
// <i> Remove the navigation layer for this display adapter. NOTE: Disable the navigation layer will also remove the real-time FPS display.
#ifndef __DISP0_CFG_DISABLE_NAVIGATION_LAYER__
#   define __DISP0_CFG_DISABLE_NAVIGATION_LAYER__                  0
#endif

// <q>Enable the virtual resource helper service
// <i> Introduce a helper service for loading virtual resources.
// <i> This feature is disabled by default.
#ifndef __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__
#   define __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__                   0
#endif

// <q>Use heap to allocate buffer in the virtual resource helper service
// <i> Use malloc and free in the virtual resource helper service. When disabled, a static buffer in the size of current display adapter PFB will be used. 
// <i> This feature is disabled by default.
#ifndef __DISP0_CFG_USE_HEAP_FOR_VIRTUAL_RESOURCE_HELPER__
#   define __DISP0_CFG_USE_HEAP_FOR_VIRTUAL_RESOURCE_HELPER__      0
#endif
// <<< end of configuration section >>>


/*============================ MACROFIED FUNCTIONS ===========================*/

#if __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__
#define disp_adapter0_impl_vres(__COLOUR_FORMAT, __WIDTH, __HEIGHT,...)\
{                                                                               \
    .tTile = {                                                                  \
        .tRegion = {                                                            \
            .tSize = {                                                          \
                .iWidth = (__WIDTH),                                            \
                .iHeight =(__HEIGHT),                                           \
            },                                                                  \
        },                                                                      \
        .tInfo = {                                                              \
            .bIsRoot = true,                                                    \
            .bHasEnforcedColour = true,                                         \
            .bVirtualResource = true,                                           \
            .tColourInfo = {                                                    \
                .chScheme = (__COLOUR_FORMAT),                                  \
            },                                                                  \
        },                                                                      \
    },                                                                          \
    .Load       = &__disp_adapter0_vres_asset_loader,                  \
    .Depose     = &__disp_adapter0_vres_buffer_deposer,                \
    __VA_ARGS__                                                                 \
}
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
ARM_NOINIT
extern
arm_2d_scene_player_t DISP0_ADAPTER;

/*============================ PROTOTYPES ====================================*/

extern
void disp_adapter0_init(void);

extern
arm_fsm_rt_t disp_adapter0_task(void);


#if __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__
/*!
 *  \brief a method to load a specific part of an image
 *  \note It is NOT an API for users to call, plese leave it alone
 *
 *  \param[in] pTarget a reference of an user object 
 *  \param[in] ptVRES a reference of this virtual resource
 *  \param[in] ptRegion the target region of the image
 *  \return intptr_t the address of a resource buffer which holds the content
 */
intptr_t __disp_adapter0_vres_asset_loader   (
                                                uintptr_t pTarget, 
                                                arm_2d_vres_t *ptVRES, 
                                                arm_2d_region_t *ptRegion);
    
/*!
 *  \brief a method to despose the buffer
 *  \note It is NOT an API for users to call, plese leave it alone
 *
 *  \param[in] pTarget a reference of an user object 
 *  \param[in] ptVRES a reference of this virtual resource
 *  \param[in] pBuffer the target buffer
 */
void __disp_adapter0_vres_buffer_deposer (  uintptr_t pTarget, 
                                                arm_2d_vres_t *ptVRES, 
                                                intptr_t pBuffer );

/*!
 * \brief A user implemented function to return the address for specific asset
 *        stored in external memory, e.g. SPI Flash
 * \note You MUST provide an implementation when 
 *       __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__ is enabled(1)
 *
 * \param[in] pObj an pointer of user defined object, it is used for OOC
 * \note You can ignore pObj if you don't care/don't use OOC 
 *
 * \param[in] ptVRES the target virtual resource object
 * \return uintptr_t the address of the target asset in external memory
 */
extern
uintptr_t __disp_adapter0_vres_get_asset_address(
                                                        uintptr_t pObj,
                                                        arm_2d_vres_t *ptVRES);

/*!
 * \brief A user implemented function to copy content from external memory
 *        (e.g. SPI Flash) to a local buffer with specified address and size.
 *
 * \note You MUST provide an implementation when 
 *       __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__ is enabled(1)
 *
 * \param[in] pObj an pointer of user defined object, it is used for OOC
 * \note You can ignore pObj if you don't care/don't use OOC 
 *
 * \param[in] pBuffer the address of the local buffer
 * \param[in] pAddress the address in the external memory
 * \param[in] nSizeInByte number of bytes to read
 */
extern
void __disp_adapter0_vres_read_memory( intptr_t pObj, 
                                                void *pBuffer,
                                                uintptr_t pAddress,
                                                size_t nSizeInByte);

#endif

/*!
 * \brief It is an user implemented function that request an LCD flushing in 
 *        asynchronous manner. 
 * \note User MUST implement this function when 
 *       __DISP0_CFG_ENABLE_ASYNC_FLUSHING__ is set to '1'
 *
 * \param[in] pTarget an user specified object address
 * \param[in] bIsNewFrame whether this flushing request is the first iteration 
 *            of a new frame.
 * \param[in] iX the x coordinate of a flushing window in the target screen
 * \param[in] iY the y coordinate of a flushing window in the target screen
 * \param[in] iWidth the width of a flushing window
 * \param[in] iHeight the height of a flushing window
 * \param[in] pBuffer the frame buffer address
 */
extern void __disp_adapter0_request_async_flushing( 
                                                    void *pTarget,
                                                    bool bIsNewFrame,
                                                    int16_t iX, 
                                                    int16_t iY,
                                                    int16_t iWidth,
                                                    int16_t iHeight,
                                                    const COLOUR_INT *pBuffer);


/*!
 * \brief the handler for the asynchronous flushing complete event.
 * \note When __DISP0_CFG_ENABLE_ASYNC_FLUSHING__ is set to '1', user 
 *       MUST call this function to notify the PFB helper that the previous
 *       asynchronous flushing is complete. 
 * \note When people using DMA+ISR to offload CPU, this fucntion is called in 
 *       the DMA transfer complete ISR.
 */
extern
void disp_adapter0_insert_async_flushing_complete_event_handler(void);





#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif

#endif
