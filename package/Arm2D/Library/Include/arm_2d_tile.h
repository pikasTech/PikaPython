/*
 * Copyright (C) 2010-2022 Arm Limited or its affiliates. All rights reserved.
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
 * $Date:        09. Aug 2022
 * $Revision:    V.1.0.3
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

/*!
 * \addtogroup Deprecated
 * @{
 */
#define arm_2d_c8bit_tile_copy_with_colour_masking                              \
             arm_2d_c8bit_tile_copy_with_colour_keying

#define arm_2d_rgb16_tile_copy_with_colour_masking                              \
            arm_2d_rgb16_tile_copy_with_colour_keying

#define arm_2d_rgb32_tile_copy_with_colour_masking                              \
            arm_2d_rgb32_tile_copy_with_colour_keying
/*! @} */

/*!
 * \addtogroup Tile 2 Tile Operations
 * @{
 */
/*============================ MACROFIED FUNCTIONS ===========================*/

#define arm_2d_c8bit_tile_copy( __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR,  /*   target region address*/\
                                __MODE)             /*   mode */                \
            arm_2dp_c8bit_tile_copy(NULL,                                       \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR),                        \
                                    (__MODE))

#define arm_2d_rgb16_tile_copy( __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR,  /*   target region address*/\
                                __MODE)             /*   mode */                \
            arm_2dp_rgb16_tile_copy(NULL,                                       \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR),                        \
                                    (__MODE))

#define arm_2d_rgb32_tile_copy( __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR,  /*   target region address*/\
                                __MODE)             /*   mode */                \
            arm_2dp_rgb32_tile_copy(NULL,                                       \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR),                        \
                                    (__MODE))

#define arm_2d_c8bit_tile_copy_only(                                            \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_c8bit_tile_copy_only(NULL,                                  \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb16_tile_copy_only(                                            \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb16_tile_copy_only(NULL,                                  \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb32_tile_copy_only(                                            \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb32_tile_copy_only(NULL,                                  \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_c8bit_tile_copy_with_x_mirror(                                   \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_c8bit_tile_copy_with_x_mirror(                              \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb16_tile_copy_with_x_mirror(                                   \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb16_tile_copy_with_x_mirror(                              \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb32_tile_copy_with_x_mirror(                                   \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb32_tile_copy_with_x_mirror(                              \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_c8bit_tile_copy_with_y_mirror(                                   \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_c8bit_tile_copy_with_y_mirror(                              \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb16_tile_copy_with_y_mirror(                                   \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb16_tile_copy_with_y_mirror(                              \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb32_tile_copy_with_y_mirror(                                   \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb32_tile_copy_with_y_mirror(                              \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_c8bit_tile_copy_with_xy_mirror(                                  \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_c8bit_tile_copy_with_xy_mirror(                             \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb16_tile_copy_with_xy_mirror(                                  \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb16_tile_copy_with_xy_mirror(                             \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb32_tile_copy_with_xy_mirror(                                  \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb32_tile_copy_with_xy_mirror(                             \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_c8bit_tile_fill_only(                                            \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_c8bit_tile_fill_only(NULL,                                  \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb16_tile_fill_only(                                            \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb16_tile_fill_only(NULL,                                  \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb32_tile_fill_only(                                            \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb32_tile_fill_only(NULL,                                  \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_c8bit_tile_fill_with_x_mirror(                                   \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_c8bit_tile_fill_with_x_mirror(                              \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb16_tile_fill_with_x_mirror(                                   \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb16_tile_fill_with_x_mirror(                              \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb32_tile_fill_with_x_mirror(                                   \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb32_tile_fill_with_x_mirror(                              \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_c8bit_tile_fill_with_y_mirror(                                   \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_c8bit_tile_fill_with_y_mirror(                              \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb16_tile_fill_with_y_mirror(                                   \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb16_tile_fill_with_y_mirror(                              \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb32_tile_fill_with_y_mirror(                                   \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb32_tile_fill_with_y_mirror(                              \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_c8bit_tile_fill_with_xy_mirror(                                  \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_c8bit_tile_fill_with_xy_mirror(                             \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb16_tile_fill_with_xy_mirror(                                  \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb16_tile_fill_with_xy_mirror(                             \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_rgb32_tile_fill_with_xy_mirror(                                  \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR)  /*   target region address*/\
            arm_2dp_rgb32_tile_fill_with_xy_mirror(                             \
                                     NULL,                                      \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR))

#define arm_2d_c8bit_tile_copy_with_colour_keying(                              \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR,  /*   target region address*/\
                                __MSK_COLOUR,       /*   mask(key) colour */    \
                                __MODE)             /*   mode */                \
            arm_2dp_c8bit_tile_copy_with_colour_keying(                         \
                                    NULL,                                       \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR),                        \
                                    (__MSK_COLOUR),                             \
                                    (__MODE))

               
#define arm_2d_rgb16_tile_copy_with_colour_keying(                              \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR,  /*   target region address*/\
                                __MSK_COLOUR,       /*   mask(key) colour */    \
                                __MODE)             /*   mode */                \
            arm_2dp_rgb16_tile_copy_with_colour_keying(                         \
                                    NULL,                                       \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR),                        \
                                    (__MSK_COLOUR),                             \
                                    (__MODE))

#define arm_2d_rgb32_tile_copy_with_colour_keying(                              \
                                __SRC_ADDR,         /*   source tile address */ \
                                __DES_ADDR,         /*   target tile address */ \
                                __DES_REGION_ADDR,  /*   target region address*/\
                                __MSK_COLOUR,       /*   mask(key) colour */    \
                                __MODE)             /*   mode */                \
            arm_2dp_rgb32_tile_copy_with_colour_keying(                         \
                                    NULL,                                       \
                                    (__SRC_ADDR),                               \
                                    (__DES_ADDR),                               \
                                    (__DES_REGION_ADDR),                        \
                                    (__MSK_COLOUR),                             \
                                    (__MODE))


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

/*! \note arm_2d_op_cp_cl_key_t inherits from arm_2d_op_src_t explicitly 
 */
typedef struct arm_2d_op_cp_cl_key_t {
    inherit(arm_2d_op_core_t);                  //!< base
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile 
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile 
    }Source;
    uint32_t wMode;                             //!< copy mode
    union {
        uint8_t  chColour;                      //!< 8bit colour
        uint16_t hwColour;                      //!< 16bit colour
        uint32_t wColour;                       //!< 32bit colour
    };
} arm_2d_op_cp_cl_key_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


/*----------------------------------------------------------------------------*
 * Tile Operations                                                            *
 *----------------------------------------------------------------------------*/

/*!
 * \brief check whether a given tile is root or not
 * \param[in] ptTile the target tile
 * \retval true the target tile is a root tile
 * \retval false the target tile is a child tile
 */
ARM_NONNULL(1)
__STATIC_INLINE bool arm_2d_is_root_tile(const arm_2d_tile_t *ptTile)
{
    return ptTile->tInfo.bIsRoot;
}


/*!
 * \brief intersect two regions and find the overlapped region
 * \param[in] ptRegionIn0 the input region 0
 * \param[in] ptRegionIn1 the input region 1
 * \param[out] ptRegionOut the overlapped region
 * \retval false the two regions do not overlap
 * \retval true the two regions overlap
 * 
 * \code

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
 *  \endcode
 */
extern
ARM_NONNULL(1,2)
bool arm_2d_region_intersect(   const arm_2d_region_t *ptRegionIn0,
                                const arm_2d_region_t *ptRegionIn1,
                                arm_2d_region_t *ptRegionOut);

/*!
 * \brief check whether a point is inside a given region
 * \param[in] ptRegion the target region
 * \param[in] ptPoint the target point
 * \retval true the point is inside the target region
 * \retval false the point is outside the target region
 */
extern
ARM_NONNULL(1,2)
bool arm_2d_is_point_inside_region( const arm_2d_region_t *ptRegion,
                                    const arm_2d_location_t *ptPoint);

/*!
 * \brief get the root tile and the valid region for a given tile
 * \param[in] ptTile the target tile
 * \param[out] ptValidRegion the valid region inside the root tile
 * \param[out] ptOffset the offsite of the root tile as if the root tile is inside the target tile
 * \return const arm_2d_tile_t* the root tile
 * 
 * \code
  HOW IT WORKS:
  
   Root Tile (Output Tile)
  +------------------------------------------------------------------------+
  |     ... ...                                                            |
  |                                                                        |
  |                    Child Tile of Parent Tile                           |
  |                 +------------------------------------+                 |
  |                 |        Child Tile of Parent Tile   |                 |
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
  \endcode
 */
extern
ARM_NONNULL(1,2)
const arm_2d_tile_t *arm_2d_tile_get_root(  const arm_2d_tile_t *ptTile,
                                            arm_2d_region_t *ptValidRegion,
                                            arm_2d_location_t *ptOffset);

/*!
 * \brief generate a child tile from the target tile with a given region
 * \param[in] ptTargetTile the target tile
 * \param[in] ptRegion the target region
 * \param[out] ptOutput the child tile
 * \param[in] bClipRegion whether clip the region for the child tile
 * \note We highly recommend that please DO NOT clip the child tile if 
 *       you don't know what you are doing.
 * \retval non-NULL the child tile
 * \retval NULL the given region is outside the target tile
 * 
 * \note If you want to use the child tile generated by 
 *       arm_2d_tile_generate_child() as a source tile / mask, please set the 
 *       bDerivedResource to true manually, otherwise all mirror related
 *       operation will NOT work correctly.
 *
 * \code
  HOW IT WORKS:
  
   Parent Tile (NOT necessarily a ROOT tile )
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
   \endcode
 */
extern 
ARM_NONNULL(1,2,3)
arm_2d_tile_t *arm_2d_tile_generate_child(
                                        const arm_2d_tile_t *ptTargetTile,
                                        const arm_2d_region_t *ptRegion,
                                        arm_2d_tile_t *ptOutput,
                                        bool bClipRegion);

/*!
 * \brief compare the widths of two tiles
 * \param[in] ptTarget the target tile
 * \param[in] ptReference the reference tile
 * \return arm_2d_cmp_t the comparision result
 */
extern
ARM_NONNULL(1,2)
arm_2d_cmp_t arm_2d_tile_width_compare( const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptReference);
/*!
 * \brief compare the heights of two tiles
 * \param[in] ptTarget the target tile
 * \param[in] ptReference the reference tile
 * \return arm_2d_cmp_t the comparision result
 */
extern
ARM_NONNULL(1,2)
arm_2d_cmp_t arm_2d_tile_height_compare(const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptReference);

/*!
 * \brief compare the shape (both widths and heights) of two tiles
 * \param[in] ptTarget the target tile
 * \param[in] ptReference the reference tile
 * \return arm_2d_cmp_t the comparision result
 */
extern
ARM_NONNULL(1,2)
arm_2d_cmp_t arm_2d_tile_shape_compare( const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptReference);

/*!
 * \brief get the absolute location of a given tile
 * \param[in] ptTile the target tile
 * \param[out] ptLocation the absolute location in the root tile
 * \return const arm_2d_tile_t * the root tile
 */
extern
ARM_NONNULL(1,2)
const arm_2d_tile_t * arm_2d_get_absolute_location(
                                        const arm_2d_tile_t *ptTile, 
                                        arm_2d_location_t *ptLocation);

/*!
 * \brief calculate the region differences between two tiles
 * \param[in] ptTarget the target tile
 * \param[in] ptReference the reference tile
 * \param[out] ptBuffer the difference stored in a region
 * \return arm_2d_region_t * the ptBuffer passed to this function
 */
extern
ARM_NONNULL(1,2,3)
arm_2d_region_t *arm_2d_tile_region_diff(   const arm_2d_tile_t *ptTarget,
                                            const arm_2d_tile_t *ptReference,
                                            arm_2d_region_t *ptBuffer);
/*----------------------------------------------------------------------------*
 * Copy/Fill tile to destination with Mirroring                               *
 *----------------------------------------------------------------------------*/

/*!
 * \brief tile copy modes
 */
enum __arm_2d_copy_mode_t {
    ARM_2D_CP_MODE_COPY =         0,
    ARM_2D_CP_MODE_FILL =         _BV(0),
    ARM_2D_CP_MODE_Y_MIRROR =     _BV(2),
    ARM_2D_CP_MODE_X_MIRROR =     _BV(3),
    ARM_2D_CP_MODE_XY_MIRROR =    ARM_2D_CP_MODE_X_MIRROR |
                                  ARM_2D_CP_MODE_Y_MIRROR,
};

/*!
 * \brief tile copy with specified mode
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \param[in] wMode the copy mode
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_c8bit_tile_copy(arm_2d_op_cp_t *ptOP,
                                     const arm_2d_tile_t *ptSource,
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     uint32_t wMode);

/*!
 * \brief tile copy with specified mode
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \param[in] wMode the copy mode
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb16_tile_copy(arm_2d_op_cp_t *ptOP,
                                     const arm_2d_tile_t *ptSource,
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     uint32_t wMode);

/*!
 * \brief tile copy with specified mode
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \param[in] wMode the copy mode
 * \return arm_fsm_rt_t the operation result
 */                             
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb32_tile_copy(arm_2d_op_cp_t *ptOP,
                                     const arm_2d_tile_t *ptSource,
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     uint32_t wMode);

/*----------------------------------------------------------------------------*
 * Copy Only                                                                  *
 *----------------------------------------------------------------------------*/
/*!
 * \brief tile copy only
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_c8bit_tile_copy_only(  arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief tile copy only
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb16_tile_copy_only(  arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief tile copy only
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */                
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb32_tile_copy_only(  arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*----------------------------------------------------------------------------*
 * Copy with X mirroring                                                      *
 *----------------------------------------------------------------------------*/
/*!
 * \brief tile copy with x-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_c8bit_tile_copy_with_x_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief tile copy with x-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb16_tile_copy_with_x_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief tile copy with x-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */                  
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb32_tile_copy_with_x_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*----------------------------------------------------------------------------*
 * Copy with Y mirroring                                                      *
 *----------------------------------------------------------------------------*/

/*!
 * \brief tile copy with y-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_c8bit_tile_copy_with_y_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief tile copy with y-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb16_tile_copy_with_y_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief tile copy with y-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb32_tile_copy_with_y_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*----------------------------------------------------------------------------*
 * Copy with XY mirroring                                                     *
 *----------------------------------------------------------------------------*/

/*!
 * \brief tile copy with xy-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_c8bit_tile_copy_with_xy_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief tile copy with xy-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb16_tile_copy_with_xy_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief tile copy with xy-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */        
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb32_tile_copy_with_xy_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*----------------------------------------------------------------------------*
 * Fill Only                                                                  *
 *----------------------------------------------------------------------------*/

/*!
 * \brief Tiling only
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_c8bit_tile_fill_only(  arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief Tiling only
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb16_tile_fill_only(  arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief Tiling only
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb32_tile_fill_only(  arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*----------------------------------------------------------------------------*
 * Fill with X mirroring                                                      *
 *----------------------------------------------------------------------------*/

/*!
 * \brief Tiling with x-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_c8bit_tile_fill_with_x_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief Tiling with x-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb16_tile_fill_with_x_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief Tiling with x-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb32_tile_fill_with_x_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*----------------------------------------------------------------------------*
 * Fill with Y mirroring                                                      *
 *----------------------------------------------------------------------------*/

/*!
 * \brief Tiling with y-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_c8bit_tile_fill_with_y_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief Tiling with y-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb16_tile_fill_with_y_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief Tiling with y-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb32_tile_fill_with_y_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*----------------------------------------------------------------------------*
 * Fill with XY mirroring                                                     *
 *----------------------------------------------------------------------------*/

/*!
 * \brief Tiling with xy-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_c8bit_tile_fill_with_xy_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief Tiling with xy-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb16_tile_fill_with_xy_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief Tiling with xy-mirroring
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb32_tile_fill_with_xy_mirror(
                                            arm_2d_op_cp_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*----------------------------------------------------------------------------*
 * Copy/Fill tile to destination with colour-keying and mirroring             *
 *----------------------------------------------------------------------------*/

/*! 
 * \brief tile copy with colour-keying and specified mode
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \param[in] chMaskColour the key colour in any 8bit colour format
 * \param[in] wMode the copy mode
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_c8bit_tile_copy_with_colour_keying(
                                            arm_2d_op_cp_cl_key_t *ptOP,
                                            const arm_2d_tile_t *ptSource, 
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint8_t chMaskColour,
                                            uint32_t wMode);

/*! 
 * \brief tile copy with colour-keying and specified mode
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \param[in] hwMaskColour the key colour in any 16bit colour format
 * \param[in] wMode the copy mode
 * \return arm_fsm_rt_t the operation result
 * 
 * \note  alpha channel is not handled, i.e. rgba5551
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb16_tile_copy_with_colour_keying(
                                            arm_2d_op_cp_cl_key_t *ptOP,
                                            const arm_2d_tile_t *ptSource, 
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint16_t hwMaskColour,
                                            uint32_t wMode);


/*! 
 * \brief tile copy with colour-keying and specified mode
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \param[in] wMaskColour the key colour in any 32bit colour format
 * \param[in] wMode the copy mode
 * \return arm_fsm_rt_t the operation result
 * 
 * \note  alpha channel is not handled
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb32_tile_copy_with_colour_keying(
                                            arm_2d_op_cp_cl_key_t *ptOP,
                                            const arm_2d_tile_t *ptSource, 
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint32_t wMaskColour,
                                            uint32_t wMode);

/*! @} */

#ifdef   __cplusplus
}
#endif

#endif
