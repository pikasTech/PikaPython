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
 * Title:        arm_2d_tile.h
 * Description:  Public header file to contain the basic tile operations
 *
 * $Date:        01. December 2020
 * $Revision:    V.0.9.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */
 
#ifndef __ARM_2D_TILE_H__
#define __ARM_2D_TILE_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d_types.h"

#ifdef   __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

/*
  calculate the start address 
  HOW IT WORKS:
  
  Base Address
  +------------------------------------------------------------------------+
  |                                                                        |
  |<------------------------------- iWidth ------------------------------->|
  |                                                                        |
  |                                                                        |
  |                    Start Address = Base Address + iX + iY * iWidth     |
  |                    (iX,iY)                                             |
  |<-------- iX --------->+------------------------------+                 |
  |                       |                              |                 |
  |                       |       Valid Region           |                 |
  |                       |                              |                 |
  |                       +------------------------------+                 |
 ...                                                                      ...
  |                                                                        |
  +------------------------------------------------------------------------+
 */

#define __arm_2d_get_address_and_region_from_tile(  __TILE_PTR,                 \
                                                    __VALID_REGION_NAME,        \
                                                    __LOCATION_OFFSET_PTR,      \
                                                    __TYPE,                     \
                                                    __BUF_PTR_NAME)             \
    arm_2d_region_t __VALID_REGION_NAME;                                        \
    assert(NULL != (__TILE_PTR));                                               \
    __TYPE *(__BUF_PTR_NAME) = NULL;                                            \
                                                                                \
    (__TILE_PTR) = arm_2d_tile_get_root((__TILE_PTR),                           \
                                        &__VALID_REGION_NAME,                   \
                                         __LOCATION_OFFSET_PTR);                \
                                                                                \
    if (NULL != (__TILE_PTR)) {                                                 \
        (__BUF_PTR_NAME) = ((__TYPE *)((__TILE_PTR)->pchBuffer))                \
                        + (__TILE_PTR)->tRegion.tSize.iWidth *                  \
                                __VALID_REGION_NAME.tLocation.iY                \
                        + __VALID_REGION_NAME.tLocation.iX;                     \
    }


#define arm_2d_get_address_and_region_from_tile(__TILE_PTR,                     \
                                                __VALID_REGION_NAME,            \
                                                __LOCATION_OFFSET_PTR,          \
                                                __TYPE,                         \
                                                __BUF_PTR_NAME)                 \
        __arm_2d_get_address_and_region_from_tile(  __TILE_PTR,                 \
                                                    __VALID_REGION_NAME,        \
                                                    __LOCATION_OFFSET_PTR,      \
                                                    __TYPE,                     \
                                                    __BUF_PTR_NAME)
                                                    

#define __arm_2d_get_address_and_region_from_tile_with_mirroring(               \
                                                    __TILE_PTR,                 \
                                                    __VALID_REGION_NAME,        \
                                                    __LOCATION_OFFSET_PTR,      \
                                                    __TYPE,                     \
                                                    __BUF_PTR_NAME,             \
                                                    __MODE)                     \
    arm_2d_region_t __VALID_REGION_NAME;                                        \
    assert(NULL != (__TILE_PTR));                                               \
    __TYPE *(__BUF_PTR_NAME) = NULL;                                            \
                                                                                \
    (__TILE_PTR) = arm_2d_tile_get_root((__TILE_PTR),                           \
                                        &__VALID_REGION_NAME,                   \
                                         __LOCATION_OFFSET_PTR);                \
                                                                                \
    if (NULL != (__TILE_PTR)) {                                                 \
        arm_2d_location_t tOffset = __VALID_REGION_NAME.tLocation;              \
        if ((__MODE) & ARM_2D_CP_MODE_X_MIRROR) {                               \
            tOffset.iX = 0;                                                     \
        }                                                                       \
        if ((__MODE) & ARM_2D_CP_MODE_Y_MIRROR) {                               \
            tOffset.iY = 0;                                                     \
        }                                                                       \
        (__BUF_PTR_NAME) = ((__TYPE *)((__TILE_PTR)->pchBuffer))                \
                            + (__TILE_PTR)->tRegion.tSize.iWidth * tOffset.iY   \
                            + tOffset.iX;                                       \
    }


#define arm_2d_get_address_and_region_from_tile_with_mirroring(                 \
                                                __TILE_PTR,                     \
                                                __VALID_REGION_NAME,            \
                                                __LOCATION_OFFSET_PTR,          \
                                                __TYPE,                         \
                                                __BUF_PTR_NAME,                 \
                                                __MODE)                         \
        __arm_2d_get_address_and_region_from_tile_with_mirroring(               \
                                                    __TILE_PTR,                 \
                                                    __VALID_REGION_NAME,        \
                                                    __LOCATION_OFFSET_PTR,      \
                                                    __TYPE,                     \
                                                    __BUF_PTR_NAME,             \
                                                    (__MODE))     


/*============================ TYPES =========================================*/

typedef arm_2d_op_src_t arm_2d_op_cp_t;

/*! \note arm_2d_op_cp_cl_msk_t inherits from arm_2d_op_src_t explicitly 
 */
typedef struct arm_2d_op_cp_cl_msk_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile 
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile 
    }Source;
    uint32_t wMode;
    union {
        uint16_t hwColour;
        uint32_t wColour;
    };
} arm_2d_op_cp_cl_msk_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


/*----------------------------------------------------------------------------*
 * Tile Operations                                                            *
 *----------------------------------------------------------------------------*/


ARM_NONNULL(1)
__STATIC_INLINE bool arm_2d_is_root_tile(const arm_2d_tile_t *ptTile)
{
    return ptTile->tInfo.bIsRoot;
}


/*
  HOW IT WORKS:
  
    Input Region 0
  +------------------------------------------------------+
  |                                                      |
  |                                                      |
  |                                                      |
  |                       +------------------------------+---------+
  |                       |                              |/////////|
  |                       |        Output Region         |/////////|
  |                       |                              |/////////|
  +-----------------------+------------------------------+/////////|
                          |////////////////////////////////////////|
                          |////////////////////////////////////////|
                          +----------------------------------------+
                                                   Input Region 1
 */
extern
ARM_NONNULL(1,2)
bool arm_2d_region_intersect(   const arm_2d_region_t *ptRegionIn0,
                                const arm_2d_region_t *ptRegionIn1,
                                arm_2d_region_t *ptRegionOut);
extern
ARM_NONNULL(1,2)
bool arm_2d_is_point_inside_region( const arm_2d_region_t *ptRegion,
                                    const arm_2d_location_t *ptPoint);

/*
  HOW IT WORKS:
  
   Root Tile (Output Tile)
  +------------------------------------------------------------------------+
  |     ... ...                                                            |
  |                                                                        |
  |                   Parent Tile' (Child Tile of Parent Tile'')           |
  |                 +------------------------------------+                 |
  |                 |        Child Tile of Parent Tile'  |                 |
  |                 |     +------------------------------+---------+       |
  |                 |     |                              |/////////|       |
  |                 |     |       Valid Region           |/////////|       |
  |                 |     |                              |/////////|       |
  |                 +-----+------------------------------+/////////|       |
  |                       |////////////////////////////////////////|       |
  |                       |////////////////////////////////////////|       |
  |                       +----------------------------------------+       |
  |                                                                        |
  +------------------------------------------------------------------------+
 */
extern
ARM_NONNULL(1,2)
const arm_2d_tile_t *arm_2d_tile_get_root(  const arm_2d_tile_t *ptTile,
                                            arm_2d_region_t *ptValidRegion,
                                            arm_2d_location_t *ptOffset);

/*
  HOW IT WORKS:
  
   Parent Tile (Are NOT necessarily a ROOT tile )
  +------------------------------------------------------+
  |                                                      |
  |                                                      |
  |                         Target Region                |
  |                       +------------------------------+---------+
  |                       |                              |/////////|
  |                       |    New Child Tile (Output)   |/////////|
  |                       |                              |/////////|
  +-----------------------+------------------------------+/////////|
                          |////////////////////////////////////////|
                          |////////////////////////////////////////|
                          +----------------------------------------+

 */
extern 
ARM_NONNULL(1,2,3)
arm_2d_tile_t *arm_2d_tile_generate_child(
                                        const arm_2d_tile_t *ptTargetTile,
                                        const arm_2d_region_t *ptRegion,
                                        arm_2d_tile_t *ptOutput,
                                        bool bClipRegion);

/*----------------------------------------------------------------------------*
 * Copy tile to destination directly                                          *
 *----------------------------------------------------------------------------*/

enum {
    ARM_2D_CP_MODE_COPY =         0,
    ARM_2D_CP_MODE_FILL =         _BV(0),
    ARM_2D_CP_MODE_Y_MIRROR =     _BV(2),
    ARM_2D_CP_MODE_X_MIRROR =     _BV(3),
};

extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_rgb16_tile_copy( const arm_2d_tile_t *ptSource,
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     uint32_t wMode);
                                     
extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_rgb32_tile_copy( const arm_2d_tile_t *ptSource,
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     uint32_t wMode);


/*----------------------------------------------------------------------------*
 * Copy tile to destination with specified transparency color mask            *
 *----------------------------------------------------------------------------*/


/*! \brief copy source tile to destination tile and use destination tile as 
 *!        background. When encountering specified mask colour, the background
 *!        pixel should be used, otherwise the foreground pixel from source tile
 *!        is used. 
 *!         
 *! \note  All color formats which using 16bits per pixel are treated equally.
 *! 
 *! \note  alpha channel is not handled, i.e. rgba5551
 */
extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_rgb16_tile_copy_with_colour_masking(
                                            const arm_2d_tile_t *ptSource, 
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint16_t hwMaskColour,
                                            uint32_t wMode);


/*! \brief copy source tile to destination tile and use destination tile as 
 *!        background. When encountering specified mask colour, the background
 *!        pixel should be used, otherwise the foreground pixel from source tile
 *!        is used. 
 *! 
 *! \note  All color formats which using 32bits per pixel are treated equally.
 *! 
 *! \note  alpha channel is not handled.
 */
extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_rgb32_tile_copy_with_colour_masking(
                                            const arm_2d_tile_t *ptSource, 
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint32_t wMaskColour,
                                            uint32_t wMode);

#ifdef   __cplusplus
}
#endif

#endif
