/*
 * Copyright (C) 2022 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        cmsis_nn_typs.h
 * Description:  Public header file to contain the Arm-2D structs
 *
 * $Date:        09. Aug 2022
 * $Revision:    V.1.0.4
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */


#ifndef __ARM_2D_TYPES_H__
#define __ARM_2D_TYPES_H__

/*============================ INCLUDES ======================================*/
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "arm_2d_features.h"
#include "arm_2d_utils.h"
#include "__arm_2d_math.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wc11-extensions"
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_suppress 64
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wmissing-declarations"
#   pragma GCC diagnostic ignored "-Wpadded"
#endif

/*!
 * \addtogroup gKernel 1 Kernel
 * @{
 */

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


/*----------------------------------------------------------------------------*
 * Infrastructure                                                             *
 *----------------------------------------------------------------------------*/

/*!
 * \brief finite-state-machine status return (Compatible with arm_status, minimal integer: int8_t)
 * 
 */
typedef enum {
    arm_fsm_rt_err          = -1,    //!< fsm error
    arm_fsm_rt_cpl          = 0,     //!< fsm complete
    arm_fsm_rt_on_going     = 1,     //!< fsm on-going
    arm_fsm_rt_wait_for_obj = 2,     //!< fsm wait for IPC object
    arm_fsm_rt_async        = 3,     //!< fsm work asynchronously, please check it later.
    arm_fsm_rt_wait_for_res = 4,     //!< wait for resource
} arm_fsm_rt_t;

/*!
 * \brief the error code for arm-2d (minimal integer: int8_t)
 * 
 */
typedef enum {
    ARM_2D_ERR_UNSUPPORTED_COLOUR       = -11,  //!< the specified colour is not supported
    ARM_2D_ERR_BUSY                     = -10,  //!< service is busy
    ARM_2D_ERR_INSUFFICIENT_RESOURCE    = -9,   //!< insufficient resource
    ARM_2D_ERR_IO_BUSY                  = -8,   //!< HW accelerator is busy
    ARM_2D_ERR_IO_ERROR                 = -7,   //!< Generic HW error
    ARM_2D_ERR_MISSING_PARAM            = -6,   //!< missing mandatory parameter
    ARM_2D_ERR_INVALID_OP               = -5,   //!< unsupported / invalid operation
    ARM_2D_ERR_NOT_SUPPORT              = -4,   //!< feature/service/operation is not supported
    ARM_2D_ERR_OUT_OF_REGION            = -3,   //!< the operation is out of target area
    ARM_2D_ERR_INVALID_PARAM            = -2,   //!< invalid parameter
    ARM_2D_ERR_UNKNOWN                  = -1,   //!< generic or unknown errors
    ARM_2D_ERR_NONE                     = 0,    //!< no error
} arm_2d_err_t;

/*!
 * \brief comparison result
 * 
 */
typedef enum {
    ARM_2D_CMP_SMALLER = -1,    //!< the target is smaller than the reference
    ARM_2D_CMP_EQUALS  = 0,     //!< the target is equal to the reference
    ARM_2D_CMP_LARGER  = 1,     //!< the target is larger than the reference
} arm_2d_cmp_t;

/*----------------------------------------------------------------------------*
 * Colour definitions                                                         *
 *----------------------------------------------------------------------------*/

/*!
 * \brief the colour type for gray8 (8bit gray scale)
 * 
 */
typedef union arm_2d_color_gray8_t {
    uint8_t tValue;
} arm_2d_color_gray8_t;

/*!
 * \brief the colour type for rgb565
 * 
 */
typedef union arm_2d_color_rgb565_t {
    uint16_t tValue;
    struct {
        uint16_t u5B : 5;
        uint16_t u6G : 6;
        uint16_t u5R : 5;
    };
} arm_2d_color_rgb565_t;

/*! 
 * \brief the colour type for brga8888
 *
 * \details In most cases four equal-sized pieces of adjacent memory are used,
 *          one for each channel, and a 0 in a channel indicates black color or
 *          transparent alpha, while all-1 bits indicates white or fully opaque
 *          alpha. By far the most common format is to store 8 bits (one byte)
 *          for each channel, which is 32 bits for each pixel.
 *
 *          (source: https://en.wikipedia.org/wiki/RGBA_color_model#ARGB32)
 */
typedef union arm_2d_color_bgra8888_t {
    uint32_t tValue;
    struct {
        uint32_t u8B : 8;
        uint32_t u8G : 8;
        uint32_t u8R : 8;
        uint32_t u8A : 8;
    };
} arm_2d_color_bgra8888_t;

/*!
 * \brief the colour type for rgb888 (compliant with ccca888 and bgra8888)
 * 
 * \details In most cases four equal-sized pieces of adjacent memory are used,
 *          one for each channel, and a 0 in a channel indicates black color or
 *          transparent alpha, while all-1 bits indicates white or fully opaque
 *          alpha. By far the most common format is to store 8 bits (one byte)
 *          for each channel, which is 32 bits for each pixel.
 *
 *          (source: https://en.wikipedia.org/wiki/RGBA_color_model#ARGB32)
 */
typedef union arm_2d_color_rgb888_t {
    uint32_t tValue;
    struct {
        uint32_t u8B : 8;
        uint32_t u8G : 8;
        uint32_t u8R : 8;
        uint32_t     : 8;
    };
} arm_2d_color_rgb888_t;

/*!
 * \brief the colour type for any 32bit colour formats which has an alpha channel on its 3rd byte.
 * 
 * \details In most cases four equal-sized pieces of adjacent memory are used,
 *          one for each channel, and a 0 in a channel indicates black color or
 *          transparent alpha, while all-1 bits indicates white or fully opaque
 *          alpha. By far the most common format is to store 8 bits (one byte)
 *          for each channel, which is 32 bits for each pixel.
 *
 *          (source: https://en.wikipedia.org/wiki/RGBA_color_model#ARGB32)
 */
typedef union arm_2d_color_ccca8888_t {
    uint32_t tValue;
    struct {
        uint8_t u8C[3];
        uint8_t u8A;
    };
} arm_2d_color_ccca8888_t;

/*!
 * \brief the colour type for any 32bit colour formats which has an alpha channel on its first byte.
 *
 * \details In most cases four equal-sized pieces of adjacent memory are used,
 *          one for each channel, and a 0 in a channel indicates black color or
 *          transparent alpha, while all-1 bits indicates white or fully opaque
 *          alpha. By far the most common format is to store 8 bits (one byte)
 *          for each channel, which is 32 bits for each pixel.
 *
 *          (source: https://en.wikipedia.org/wiki/RGBA_color_model#ARGB32)
 */
typedef union arm_2d_color_accc8888_t {
    uint32_t tValue;
    struct {
        uint8_t u8A;
        uint8_t u8C[3];
    };
} arm_2d_color_accc8888_t;

/*!
 * \brief the colour type for any 32bit colour formats which has an unused-alpha channel on its 3rd byte.
 *
 * \details In most cases four equal-sized pieces of adjacent memory are used,
 *          one for each channel, and a 0 in a channel indicates black color or
 *          transparent alpha, while all-1 bits indicates white or fully opaque
 *          alpha. By far the most common format is to store 8 bits (one byte)
 *          for each channel, which is 32 bits for each pixel.
 *
 *          (source: https://en.wikipedia.org/wiki/RGBA_color_model#ARGB32)
 */
typedef union arm_2d_color_cccn888_t {
    uint32_t tValue;
    struct {
        uint8_t u8C[3];
        uint8_t         : 8;
    };
} arm_2d_color_cccn888_t;

/*!
 * \brief the colour type for any 32bit colour formats which has an unused-alpha channel on its first byte.
 *
 * \details In most cases four equal-sized pieces of adjacent memory are used,
 *          one for each channel, and a 0 in a channel indicates black color or
 *          transparent alpha, while all-1 bits indicates white or fully opaque
 *          alpha. By far the most common format is to store 8 bits (one byte)
 *          for each channel, which is 32 bits for each pixel.
 *
 *          (source: https://en.wikipedia.org/wiki/RGBA_color_model#ARGB32)
 */
typedef union arm_2d_color_nccc888_t {
    uint32_t tValue;
    struct {
        uint8_t         : 8;
        uint8_t u8C[3];
    };
} arm_2d_color_nccc888_t;

/*! 
 * \brief enumerations for colour attributes
 */
enum {
    ARM_2D_COLOUR_SZ_1BIT = 0,            //!< 1 bit:black and white
    ARM_2D_COLOUR_SZ_2BIT = 1,            //!< 4 colours or 4 gray-levels
    ARM_2D_COLOUR_SZ_4BIT = 2,            //!< 16 colours or 16 gray-levels
    ARM_2D_COLOUR_SZ_8BIT = 3,            //!< 256 colours
    ARM_2D_COLOUR_SZ_16BIT = 4,           //!< 16bits
    ARM_2D_COLOUR_SZ_32BIT = 5,           //!< true colour

    ARM_2D_COLOUR_SZ_1BIT_msk =   ARM_2D_COLOUR_SZ_1BIT << 1,
    ARM_2D_COLOUR_SZ_2BIT_msk =   ARM_2D_COLOUR_SZ_2BIT << 1,
    ARM_2D_COLOUR_SZ_4BIT_msk =   ARM_2D_COLOUR_SZ_4BIT << 1,
    ARM_2D_COLOUR_SZ_8BIT_msk =   ARM_2D_COLOUR_SZ_8BIT << 1,
    ARM_2D_COLOUR_SZ_16BIT_msk =  ARM_2D_COLOUR_SZ_16BIT<< 1,
    ARM_2D_COLOUR_SZ_32BIT_msk =  ARM_2D_COLOUR_SZ_32BIT<< 1,
    ARM_2D_COLOUR_SZ_msk      =   (0x07 << 1),

    ARM_2D_COLOUR_LITTLE_ENDIAN       = 0,
    ARM_2D_COLOUR_BIG_ENDIAN          = 1,

    ARM_2D_COLOUR_LITTLE_ENDIAN_msk   = ARM_2D_COLOUR_LITTLE_ENDIAN << 4,
    ARM_2D_COLOUR_BIG_ENDIAN_msk      = ARM_2D_COLOUR_BIG_ENDIAN    << 4,

    ARM_2D_COLOUR_NO_ALPHA = 0,
    ARM_2D_COLOUR_HAS_ALPHA = 1,

    ARM_2D_COLOUR_NO_ALPHA_msk        = ARM_2D_COLOUR_NO_ALPHA      << 0,
    ARM_2D_COLOUR_HAS_ALPHA_msk       = ARM_2D_COLOUR_HAS_ALPHA     << 0,
    
    ARM_2D_COLOUR_VARIANT_pos = 5,
    ARM_2D_COLOUR_VARIANT_msk         = 0x07 << ARM_2D_COLOUR_VARIANT_pos,
};

/* macros for colour attributes */
#define ARM_2D_M_COLOUR_SZ_1BIT             0       //!< 1 bit:black and white
#define ARM_2D_M_COLOUR_SZ_2BIT             1       //!< 4 colours or 4 gray-levels
#define ARM_2D_M_COLOUR_SZ_4BIT             2       //!< 16 colours or 16 gray-levels
#define ARM_2D_M_COLOUR_SZ_8BIT             3       //!< 256 colours
#define ARM_2D_M_COLOUR_SZ_16BIT            4       //!< 16bits
#define ARM_2D_M_COLOUR_SZ_32BIT            5       //!< true colour

#define ARM_2D_M_COLOUR_SZ_1BIT_msk         (ARM_2D_M_COLOUR_SZ_1BIT << 1)
#define ARM_2D_M_COLOUR_SZ_2BIT_msk         (ARM_2D_M_COLOUR_SZ_2BIT << 1)
#define ARM_2D_M_COLOUR_SZ_4BIT_msk         (ARM_2D_M_COLOUR_SZ_4BIT << 1)
#define ARM_2D_M_COLOUR_SZ_8BIT_msk         (ARM_2D_M_COLOUR_SZ_8BIT << 1)
#define ARM_2D_M_COLOUR_SZ_16BIT_msk        (ARM_2D_M_COLOUR_SZ_16BIT<< 1)
#define ARM_2D_M_COLOUR_SZ_32BIT_msk        (ARM_2D_M_COLOUR_SZ_32BIT<< 1)
#define ARM_2D_M_COLOUR_SZ_msk              (0x07 << 1),

#define ARM_2D_M_COLOUR_LITTLE_ENDIAN       0
#define ARM_2D_M_COLOUR_BIG_ENDIAN          1

#define ARM_2D_M_COLOUR_LITTLE_ENDIAN_msk   (ARM_2D_M_COLOUR_LITTLE_ENDIAN << 4)
#define ARM_2D_M_COLOUR_BIG_ENDIAN_msk      (ARM_2D_M_COLOUR_BIG_ENDIAN    << 4)

#define ARM_2D_M_COLOUR_NO_ALPHA            0
#define ARM_2D_M_COLOUR_HAS_ALPHA           1

#define ARM_2D_M_COLOUR_NO_ALPHA_msk        (ARM_2D_M_COLOUR_NO_ALPHA  << 0)
#define ARM_2D_M_COLOUR_HAS_ALPHA_msk       (ARM_2D_M_COLOUR_HAS_ALPHA << 0)
    
#define ARM_2D_M_COLOUR_VARIANT_pos         5
#define ARM_2D_M_COLOUR_VARIANT_msk         (0x07 << ARM_2D_M_COLOUR_VARIANT_pos)

/*!
 * \brief enumerations for colour types
 * 
 */
enum {
    ARM_2D_COLOUR_BIN         =   ARM_2D_COLOUR_SZ_1BIT_msk,
    ARM_2D_COLOUR_1BIT        =   ARM_2D_COLOUR_SZ_1BIT_msk,
    
    ARM_2D_COLOUR_8BIT        =   ARM_2D_COLOUR_SZ_8BIT_msk,
    ARM_2D_COLOUR_GRAY8       =   ARM_2D_COLOUR_SZ_8BIT_msk,
    
    ARM_2D_COLOUR_16BIT       =   ARM_2D_COLOUR_SZ_16BIT_msk,
    ARM_2D_COLOUR_RGB16       =   ARM_2D_COLOUR_SZ_16BIT_msk,
    ARM_2D_COLOUR_RGB565      =   ARM_2D_COLOUR_RGB16,
    
/*  won't support
    ARM_2D_COLOUR_RGB565_BE   =   ARM_2D_COLOUR_SZ_16BIT_msk        |
                                  ARM_2D_COLOUR_BIG_ENDIAN_msk      ,
 */
 
    ARM_2D_COLOUR_32BIT       =   ARM_2D_COLOUR_SZ_32BIT_msk        ,
    ARM_2D_COLOUR_RGB32       =   ARM_2D_COLOUR_SZ_32BIT_msk        ,

    ARM_2D_COLOUR_CCCN888     =   ARM_2D_COLOUR_RGB32               ,
    ARM_2D_COLOUR_CCCA8888    =   ARM_2D_COLOUR_SZ_32BIT_msk        |
                                  ARM_2D_COLOUR_HAS_ALPHA_msk       ,

    ARM_2D_COLOUR_RGB888      =   ARM_2D_COLOUR_CCCN888             ,
    ARM_2D_COLOUR_BGRA8888    =   ARM_2D_COLOUR_CCCA8888            ,

/* not supported yet
    ARM_2D_COLOUR_NCCC888     =   ARM_2D_COLOUR_RGB32               |
                                  ARM_2D_COLOUR_BIG_ENDIAN_msk      ,
    ARM_2D_COLOUR_ACCC8888    =   ARM_2D_COLOUR_SZ_32BIT_msk        |
                                  ARM_2D_COLOUR_HAS_ALPHA_msk       |
                                  ARM_2D_COLOUR_BIG_ENDIAN_msk      ,
*/
    ARM_2D_CHANNEL_8in32      =   ARM_2D_COLOUR_SZ_32BIT_msk        |
                                  ARM_2D_COLOUR_HAS_ALPHA_msk       |
                                  ARM_2D_COLOUR_VARIANT_msk   ,
};

/* macros for colour formats */
#define ARM_2D_M_COLOUR_BIN         ARM_2D_M_COLOUR_SZ_1BIT_msk
#define ARM_2D_M_COLOUR_1BIT        ARM_2D_M_COLOUR_SZ_1BIT_msk
    
#define ARM_2D_M_COLOUR_8BIT        ARM_2D_M_COLOUR_SZ_8BIT_msk
#define ARM_2D_M_COLOUR_GRAY8       ARM_2D_M_COLOUR_SZ_8BIT_msk
    
#define ARM_2D_M_COLOUR_16BIT       ARM_2D_M_COLOUR_SZ_16BIT_msk
#define ARM_2D_M_COLOUR_RGB16       ARM_2D_M_COLOUR_SZ_16BIT_msk
#define ARM_2D_M_COLOUR_RGB565      ARM_2D_M_COLOUR_RGB16
    
/* won't support
#define ARM_2D_M_COLOUR_RGB565_BE   (   ARM_2D_M_COLOUR_SZ_16BIT_msk            \
                                    |   ARM_2D_M_COLOUR_BIG_ENDIAN_msk          )
 */
 
#define ARM_2D_M_COLOUR_32BIT       ARM_2D_M_COLOUR_SZ_32BIT_msk
#define ARM_2D_M_COLOUR_RGB32       ARM_2D_M_COLOUR_SZ_32BIT_msk

#define ARM_2D_M_COLOUR_CCCN888     ARM_2D_M_COLOUR_RGB32
#define ARM_2D_M_COLOUR_CCCA8888    (   ARM_2D_M_COLOUR_SZ_32BIT_msk            \
                                    |   ARM_2D_M_COLOUR_HAS_ALPHA_msk)

#define ARM_2D_M_COLOUR_RGB888      ARM_2D_M_COLOUR_CCCN888
#define ARM_2D_M_COLOUR_RGBA8888    ARM_2D_M_COLOUR_CCCA8888

/* not supported yet
#define ARM_2D_M_COLOUR_NCCC888     (   ARM_2D_M_COLOUR_RGB32                   \
                                    |   ARM_2D_M_COLOUR_BIG_ENDIAN_msk          )
#define ARM_2D_M_COLOUR_ACCC8888    (   ARM_2D_M_COLOUR_SZ_32BIT_msk            \
                                    |   ARM_2D_M_COLOUR_HAS_ALPHA_msk           \
                                    |   ARM_2D_M_COLOUR_BIG_ENDIAN_msk          )
*/
#define ARM_2D_M_CHANNEL_8in32      (   ARM_2D_M_COLOUR_SZ_32BIT_msk            \
                                    |   ARM_2D_M_COLOUR_HAS_ALPHA_msk)          \
                                    |   ARM_2D_M_COLOUR_VARIANT_msk             )

/*!
 * \brief a type used as colour descriptor
 * 
 */
typedef union {
    struct {
        uint8_t bHasAlpha  : 1;     //!< whether the target colour has alpha channel
        uint8_t u3ColourSZ : 3;     //!< the size of the colour
        uint8_t bBigEndian : 1;     //!< whether the colour is stored in big endian
        uint8_t u3Variant  : 3;
    };
    uint8_t chScheme;
} arm_2d_color_info_t;

/*----------------------------------------------------------------------------*
 * Tile and Regions                                                           *
 *----------------------------------------------------------------------------*/

/*!
 * \brief a type for coordinates (integer)
 * 
 */
typedef struct arm_2d_location_t {
    int16_t iX;                         //!< x in Cartesian coordinate system
    int16_t iY;                         //!< y in Cartesian coordinate system
} arm_2d_location_t;

/*!
 * \brief a type for coordinates in floating point
 * 
 */
typedef struct arm_2d_point_float_t {
    float fX;                           //!< x in Cartesian coordinate system
    float fY;                           //!< y in Cartesian coordinate system
} arm_2d_point_float_t;

/*!
 * \brief a type for coordinates in fixed point
 * 
 */
typedef struct arm_2d_point_fx_t {
    int32_t X;                          //!< x in Cartesian coordinate system
    int32_t Y;                          //!< y in Cartesian coordinate system
} arm_2d_point_fx_t;

/*!
 * \brief a type for the size of an rectangular area
 * 
 */
typedef struct arm_2d_size_t {
    int16_t iWidth;                     //!< width of an rectangular area
    int16_t iHeight;                    //!< height of an rectangular area
} arm_2d_size_t;

/*!
 * \brief a type for an rectangular area
 * 
 */
typedef struct arm_2d_region_t {
    implement_ex(arm_2d_location_t, tLocation); //!< the location (top-left corner)
    implement_ex(arm_2d_size_t, tSize);         //!< the size 
} arm_2d_region_t;

/*!
 * \brief a type for tile
 * 
 */
typedef struct arm_2d_tile_t arm_2d_tile_t;
struct arm_2d_tile_t {
    implement_ex(struct {
        uint8_t    bIsRoot              : 1;                                    //!< is this tile a root tile
        uint8_t    bHasEnforcedColour   : 1;                                    //!< does this tile contains enforced colour info
        uint8_t    bDerivedResource     : 1;                                    //!< indicate whether this is a derived resources (when bIsRoot == 0)
        uint8_t    bVirtualResource     : 1;                                    //!< indicate whether the resource should be loaded on-demand
        uint8_t                         : 4;
        uint8_t                         : 8;
        uint8_t                         : 8;
        arm_2d_color_info_t    tColourInfo;                                     //!< enforced colour
    }, tInfo);

    implement_ex(arm_2d_region_t, tRegion);                                     //!< the region of the tile

    union {
        /* when bIsRoot is true, phwBuffer is available,
         * otherwise ptParent is available
         */
        arm_2d_tile_t       *ptParent;                                          //!< a pointer points to the parent tile
        uint8_t             *pchBuffer;                                         //!< a pointer points to a buffer in a 8bit colour type
        uint16_t            *phwBuffer;                                         //!< a pointer points to a buffer in a 16bit colour type
        uint32_t            *pwBuffer;                                          //!< a pointer points to a buffer in a 32bit colour type
        
        intptr_t            nAddress;                                           //!< a pointer in integer
    };
};

/*!
 * \brief a type for virtual resource
 *
 * \note the flag tTile.tInfo.bVirtualResource must be true (1)
 */
typedef struct arm_2d_vres_t arm_2d_vres_t;
struct arm_2d_vres_t {

    /*! base class: tTile */
    implement_ex( arm_2d_tile_t, tTile);
    
    /*!  a reference of an user object  */
    uintptr_t pTarget;
    
    /*!
     *  \brief a method to load a specific part of an image
     *  \param[in] pTarget a reference of an user object 
     *  \param[in] ptVRES a reference of this virtual resource
     *  \param[in] ptRegion the target region of the image
     *  \return intptr_t the address of a resource buffer which holds the content
     */
    intptr_t  (*Load)   (   uintptr_t pTarget, 
                            arm_2d_vres_t *ptVRES, 
                            arm_2d_region_t *ptRegion);
    
    /*!
     *  \brief a method to despose the buffer
     *  \param[in] pTarget a reference of an user object 
     *  \param[in] ptVRES a reference of this virtual resource
     *  \param[in] pBuffer the target buffer
     */
    void      (*Depose) (   uintptr_t pTarget, 
                            arm_2d_vres_t *ptVRES, 
                            intptr_t pBuffer );
};

/*----------------------------------------------------------------------------*
 * Task                                                                       *
 *----------------------------------------------------------------------------*/

/*!
 * \brief arm-2d application level task control block
 * 
 */
typedef struct arm_2d_task_t {
ARM_PRIVATE(
    arm_fsm_rt_t tResult;                                                       //!< the temporary result of the task
    uint8_t      chState;                                                       //!< the state of the FSM

    void         *ptTask;                                                       //!< a pointer for an internal object
)
} arm_2d_task_t;

/*----------------------------------------------------------------------------*
 * Operation and Events Handling                                              *
 *----------------------------------------------------------------------------*/


typedef struct arm_2d_op_core_t arm_2d_op_core_t;

/*!
 * \brief a prototype of event handlers for 2D operations
 * 
 * \param[in] ptThisOP the target 2D operation descriptor 
 * \param[in] tResult  the operation result
 * \param[in] pTarget  A user attached object
 * \return bool a boolean value to indicate whether the event has been handled
 */
typedef bool arm_2d_op_evt_handler_t(  arm_2d_op_core_t *ptThisOP,
                                    arm_fsm_rt_t tResult,
                                    void *pTarget);

/*!
 * \brief a type for 2D operation event handling
 * 
 */
typedef struct arm_2d_op_evt_t {
    arm_2d_op_evt_handler_t    *fnHandler;                                      //!< event handler
    void                       *pTarget;                                        //!< user attached target
} arm_2d_op_evt_t;

/*!
 * \brief a prototype for generic event handlers
 * 
 * \param pTarget A user attached object
 * \return bool a boolean value to indicate whether the event has been handled  
 */
typedef bool arm_2d_evt_handler_t(void *pTarget);

/*!
 * \brief a type for generic event handling
 * 
 */
typedef struct arm_2d_evt_t {
    arm_2d_evt_handler_t    *fnHandler;                                         //!< event handler
    void                    *pTarget;                                           //!< user attached target
} arm_2d_evt_t;


#define ARM_2D_OP_INFO_PARAM_HAS_SOURCE             _BV(0)
#define ARM_2D_OP_INFO_PARAM_HAS_TARGET             _BV(1)
#define ARM_2D_OP_INFO_PARAM_HAS_SOURCE_MASK        _BV(2)
#define ARM_2D_OP_INFO_PARAM_HAS_TARGET_MASK        _BV(3)
#define ARM_2D_OP_INFO_PARAM_HAS_ORIGIN             _BV(4)

#define ARM_2D_OP_INFO_PARAM_TILES_MASK             (                           \
            ARM_2D_OP_INFO_PARAM_HAS_SOURCE         |                           \
            ARM_2D_OP_INFO_PARAM_HAS_TARGET         |                           \
            ARM_2D_OP_INFO_PARAM_HAS_SOURCE_MASK    |                           \
            ARM_2D_OP_INFO_PARAM_HAS_TARGET_MASK    |                           \
            ARM_2D_OP_INFO_PARAM_HAS_ORIGIN         )
            

//! \brief an incomplete defintion which is only used for defining pointers
typedef struct __arm_2d_low_level_io_t __arm_2d_low_level_io_t;

/*!
 * \brief A descriptive header for 2D operations
 */
typedef union __arm_2d_op_info_t {
    struct {
        arm_2d_color_info_t Colour;                                             //!< the colour used in thie operation
        union {
            struct {
                uint8_t bHasSource              : 1;                            //!< whether this operation contains source tile
                uint8_t bHasTarget              : 1;                            //!< whether this operation contains target tile
                uint8_t bHasSrcMask             : 1;                            //!< whether this operation has Mask layer for source tile
                uint8_t bHasDesMask             : 1;                            //!< whether this operation has Mask layer for target tile
                uint8_t bHasOrigin              : 1;                            //!< whether the Source has an origin tile
                uint8_t                         : 2;
                uint8_t bAllowEnforcedColour    : 1;                            //!< whether this operation allow enforced colours in tiles
            };
            uint8_t chValue;                                                    //!< feature value
        }Param;                                                                 //!< operation feature set

        uint8_t chInClassOffset;                                                //!< some operation uses this as the offset of the key member in the class
        uint8_t chOpIndex;                                                      //!< __ARM_2D_OP_IDX_XXXXXX

        union {
            struct {
                uint8_t CopyLike;                                               //!< A copy-like interface contains the target tile, the source tile and the copy size
                uint8_t FillLike;                                               //!< A copy-like interface contains the target tile and the source tile
            };
            struct {
                uint8_t CopyOrigLike;                                           //!< A copy-like interface contains the target tile, the dummy tile, the reference to the original source tile and the copy size 
                uint8_t FillOrigLike;                                           //!< A copy-like interface contains the target tile, the dummy tile and the reference to the original source tile 
            };
            struct {
                uint8_t TileProcessLike;                                        //!< A simple interface contains only the target tile
            };
        }LowLevelInterfaceIndex;                                                //!< Low level interface index

        union {
            const __arm_2d_low_level_io_t *IO[2];                               //!< array of IOs

            struct {
                const __arm_2d_low_level_io_t *ptCopyLike;                      //!< the function pointer for a copy-like implementation   
                const __arm_2d_low_level_io_t *ptFillLike;                      //!< the function pointer for a fill-like implementation
            };
            struct {
                const __arm_2d_low_level_io_t *ptCopyOrigLike;                  //!< the function pointer for a copy-orig-like implementation
                const __arm_2d_low_level_io_t *ptFillOrigLike;                  //!< the function pointer for a fill-orig-like implementation
            };
            struct {
                const __arm_2d_low_level_io_t *ptTileProcessLike;               //!< the function pointer for the tile-process-like implementation
            };
        }LowLevelIO;                                                            //!< low level IO definition

    }Info;                                                                      //!< operation description
    uint32_t    wID;                                                            //!< Operation ID
} __arm_2d_op_info_t;

/*!
 * \brief how would you want to accelerate the 2d-operation
 */
enum {
    //! Use hardware acceleration if possible, even if there is a long queue to wait
    ARM_2D_PREF_ACC_USE_HW_IF_POSSIBLE    = 0,

    //! Only use Hardware Acceleration, if it is not supported, IO error will be issued
    ARM_2D_PREF_ACC_HW_ONLY               = 1,

    //! Only use software algorithm
    ARM_2D_PREF_ACC_SW_ONLY               = 2,

    //!< don't care, let the arm-2d library decide
    ARM_2D_PREF_ACC_DONT_CARE             = 3,
};

#define __ARM_2D_OP_STATUS_BUSY_msk         (1 << 4)
#define __ARM_2D_OP_STATUS_IO_ERROR_msk     (1 << 5)
#define __ARM_2D_OP_STATUS_CPL_msk          (1 << 6)

/*!
 * \brief a type for 2D operation status
 * 
 */
typedef union arm_2d_op_status_t {
    struct {
        uint16_t            u4SubTaskCount  : 4;        //!< sub task count
        uint16_t            bIsBusy         : 1;        //!< busy flag
        uint16_t            bIOError        : 1;        //!< HW IO Error
        uint16_t            bOpCpl          : 1;        //!< the whole operation complete
        uint16_t                            : 9;        //!< reserved
    };
    uint16_t tValue;                                    //!< the host integer
} arm_2d_op_status_t;

/*!
 * \brief the abstract class of 2D operations
 * 
 */
struct arm_2d_op_core_t {
ARM_PRIVATE(
    arm_2d_op_core_t            *ptNext;                //!< a pointer for a single list

    const __arm_2d_op_info_t    *ptOp;                  //!< the pointer for the corresponding 2D operation description

    struct {
        uint8_t                 u2ACCMethods    : 2;    //!< acceleration Methods
        uint8_t                                 : 6;    //!< reserved
    }Preference;

    int8_t                      tResult;                //!< operation result
    volatile arm_2d_op_status_t Status;                 //!< operation status

    arm_2d_op_evt_t             evt2DOpCpl;             //!< operation-complete event
    
)
    uintptr_t                   pUserParam;             //!< user attached object
};

/*! 
 * \brief the base class for operations with only a target tile
 * \note arm_2d_op_msk_t inherits from arm_2d_op_core_t
 */
typedef struct arm_2d_op_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;                //!< target tile
        const arm_2d_region_t   *ptRegion;              //!< target region
    } Target;
} arm_2d_op_t;

/*! 
 * \brief the base class for operations with a target tile and a target mask 
 * \note arm_2d_op_msk_t inherits from arm_2d_op_t
 */
typedef struct arm_2d_op_msk_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;                //!< target tile
        const arm_2d_region_t   *ptRegion;              //!< target region
    } Target;
    
    /* derived part */
    struct {
        const arm_2d_tile_t     *ptTile;                //!< target tile
    } Mask;
} arm_2d_op_msk_t;

/*! 
 * \brief the base class for operations with a target tile and a source tile 
 * \note arm_2d_op_src_t inherits from arm_2d_op_t
 */
typedef struct arm_2d_op_src_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;                //!< target tile
        const arm_2d_region_t   *ptRegion;              //!< target region
    } Target;

    /* derived part */
    struct {
        const arm_2d_tile_t     *ptTile;                //!< source tile
    }Source;
    uint32_t wMode;
} arm_2d_op_src_t;

/*! 
 * \brief the base class for operations with a target tile, a source tile and masks
 * \note arm_2d_op_src_msk_t inherits from arm_2d_op_src_t
 */
typedef struct arm_2d_op_src_msk_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;                //!< target tile
        const arm_2d_region_t   *ptRegion;              //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;                //!< source tile
    }Source;
    uint32_t wMode;
    
    /* derived part */
    struct {
        const arm_2d_tile_t     *ptSourceSide;          //!< source side mask
        const arm_2d_tile_t     *ptTargetSide;          //!< target side mask
    } Mask;
} arm_2d_op_src_msk_t;

/*! 
 * \brief the base class for operations with a target tile, a dummy tile and a reference to the original source tile
 * \note arm_2d_op_src_orig_t inherits from arm_2d_op_src_t
 */
typedef struct arm_2d_op_src_orig_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;                //!< target tile
        const arm_2d_region_t   *ptRegion;              //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;                //!< the dummy source tile
    }Source;
    uint32_t wMode;

    /* derived part */
    struct {
        const arm_2d_tile_t     *ptTile;                //!< the origin tile
        arm_2d_tile_t           tDummySource;           //!< the buffer for the source
    }Origin;

} arm_2d_op_src_orig_t;

/*! 
 * \brief the base class for operations with a target tile, a dummy tile, a reference to the original source tile and masks
 * \note arm_2d_op_src_orig_msk_t inherits from arm_2d_op_src_orig_t
 */
typedef struct arm_2d_op_src_orig_msk_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;                //!< target tile
        const arm_2d_region_t   *ptRegion;              //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;                //!< the dummy source tile
    }Source;
    uint32_t wMode;
    struct {
        const arm_2d_tile_t     *ptTile;                //!< the origin tile
        arm_2d_tile_t           tDummySource;           //!< the buffer for the source
    }Origin;
    
    /* derived part */
    struct {
        const arm_2d_tile_t     *ptOriginSide;          //!< origin side mask
        const arm_2d_tile_t     *ptTargetSide;          //!< target side mask
    } Mask;
} arm_2d_op_src_orig_msk_t;


/*----------------------------------------------------------------------------*
 * Fast Rotation linear regression structure
 *----------------------------------------------------------------------------*/

#if     (__ARM_2D_HAS_HELIUM_FLOAT__ || __ARM_2D_HAS_FPU__)                     \
    && !__ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__
/*!
 * \brief a type for parameters of linear interpolation (in floating point)
 * 
 */
typedef struct arm_2d_rot_linear_regr_t {
    float   slopeY;
    float   interceptY;
    float   slopeX;
    float   interceptX;
} arm_2d_rot_linear_regr_t;

#else
/*!
 * \brief a type for parameters of linear interpolation (in fixed point)
 * 
 */
typedef struct arm_2d_rot_linear_regr_t {
    int32_t   slopeY;
    int32_t   interceptY;
    int32_t   slopeX;
    int32_t   interceptX;
} arm_2d_rot_linear_regr_t;

#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*! @} */

#if defined(__clang__)
#pragma clang diagnostic pop
#elif __IS_COMPILER_ARM_COMPILER_5__
#pragma diag_warning 64
#elif __IS_COMPILER_GCC__
#pragma GCC diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif // __ARM_2D_TYPES_H__


