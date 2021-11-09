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
 * Title:        cmsis_nn_typs.h
 * Description:  Public header file to contain the Arm-2D structs
 *
 * $Date:        01. December 2020
 * $Revision:    V.1.0.0
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


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


/*----------------------------------------------------------------------------*
 * Infrastructure                                                             *
 *----------------------------------------------------------------------------*/

//! \name finite-state-machine status return (Compatible with arm_status), int8_t
//! @{
typedef enum {
    arm_fsm_rt_err          = -1,    //!< fsm error
    arm_fsm_rt_cpl          = 0,     //!< fsm complete
    arm_fsm_rt_on_going     = 1,     //!< fsm on-going
    arm_fsm_rt_wait_for_obj = 2,     //!< fsm wait for IPC object
    arm_fsm_rt_async        = 3,     //!< fsm work asynchronosely, please check it later.
    arm_fsm_rt_wait_for_res = 4,     //!< wait for resource
} arm_fsm_rt_t;
//! @}

//! \name error code for arm-2d, int8_t
//! \note arm_2d_err_t is compatible with arm_fsm_rt_t
//! @{
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
//! @}

//! \name compare result
//! @{
typedef enum {
    ARM_2D_CMP_SMALLER = -1,    //!< the target is smaller than the reference
    ARM_2D_CMP_EQUALS  = 0,     //!< the target is equal to the reference
    ARM_2D_CMP_LARGER  = 1,     //!< the target is larger than the reference
} arm_2d_cmp_t;
//! @}

/*----------------------------------------------------------------------------*
 * Colour definitions                                                         *
 *----------------------------------------------------------------------------*/

typedef union arm_2d_color_rgb565_t {
    uint16_t tValue;
    struct {
        uint16_t u5R : 5;
        uint16_t u6G : 6;
        uint16_t u5B : 5;
    };
} arm_2d_color_rgb565_t;

/*! \brief In most cases four equal-sized pieces of adjacent memory are used,
 *!        one for each channel, and a 0 in a channel indicates black color or
 *!        transparent alpha, while all-1 bits indicates white or fully opaque
 *!        alpha. By far the most common format is to store 8 bits (one byte)
 *!        for each channel, which is 32 bits for each pixel.
 *!
 *!        (source: https://en.wikipedia.org/wiki/RGBA_color_model#ARGB32)
 */
typedef union arm_2d_color_rgba8888_t {
    uint32_t tValue;
    struct {
        uint32_t u8R : 8;
        uint32_t u8G : 8;
        uint32_t u8B : 8;
        uint32_t u8A : 8;
    };
} arm_2d_color_rgba8888_t;


typedef union arm_2d_color_rgb888_t {
    uint32_t tValue;
    struct {
        uint32_t u8R : 8;
        uint32_t u8G : 8;
        uint32_t u8B : 8;
        uint32_t     : 8;
    };
} arm_2d_color_rgb888_t;


typedef union arm_2d_color_ccca8888_t {
    uint32_t tValue;
    struct {
        uint8_t u8C[3];
        uint8_t u8A;
    };
} arm_2d_color_ccca8888_t;

typedef union arm_2d_color_accc8888_t {
    uint32_t tValue;
    struct {
        uint8_t u8A;
        uint8_t u8C[3];
    };
} arm_2d_color_accc8888_t;

typedef union arm_2d_color_cccn888_t {
    uint32_t tValue;
    struct {
        uint8_t u8C[3];
        uint8_t         : 8;
    };
} arm_2d_color_cccn888_t;

typedef union arm_2d_color_nccc888_t {
    uint32_t tValue;
    struct {
        uint8_t         : 8;
        uint8_t u8C[3];
    };
} arm_2d_color_nccc888_t;

//! \name colour size
//! @{
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
    
    ARM_2D_COLOUR_VARIANT = 5,
    ARM_2D_COLOUR_VARIANT_msk         = 0x07 << ARM_2D_COLOUR_VARIANT,
};
//! @}

//! \name colour scheme
//! @{
enum {
    ARM_2D_COLOUR_BIN         =   ARM_2D_COLOUR_SZ_1BIT_msk,
    ARM_2D_COLOUR_1BIT        =   ARM_2D_COLOUR_SZ_1BIT_msk,
    
    ARM_2D_COLOUR_8BIT        =   ARM_2D_COLOUR_SZ_8BIT_msk,
    ARM_2D_COLOUR_GRAY8       =   ARM_2D_COLOUR_SZ_8BIT_msk,
    
    ARM_2D_COLOUR_16BIT       =   ARM_2D_COLOUR_SZ_16BIT_msk,
    ARM_2D_COLOUR_RGB16       =   ARM_2D_COLOUR_SZ_16BIT_msk,
    ARM_2D_COLOUR_RGB565      =   ARM_2D_COLOUR_RGB16,
    ARM_2D_COLOUR_RGB565_BE   =   ARM_2D_COLOUR_SZ_16BIT_msk        |
                                  ARM_2D_COLOUR_BIG_ENDIAN_msk      ,
    ARM_2D_COLOUR_32BIT       =   ARM_2D_COLOUR_SZ_32BIT_msk        ,
    ARM_2D_COLOUR_RGB32       =   ARM_2D_COLOUR_SZ_32BIT_msk        ,
                                        
    ARM_2D_COLOUR_RGB888      =   ARM_2D_COLOUR_RGB32               ,
    ARM_2D_COLOUR_RGBA8888    =   ARM_2D_COLOUR_SZ_32BIT_msk        |
                                  ARM_2D_COLOUR_HAS_ALPHA           ,

    ARM_2D_COLOUR_CCCN888     =   ARM_2D_COLOUR_RGB32               ,
    ARM_2D_COLOUR_CCCA8888    =   ARM_2D_COLOUR_SZ_32BIT_msk        |
                                  ARM_2D_COLOUR_HAS_ALPHA           ,

    ARM_2D_COLOUR_NCCC888     =   ARM_2D_COLOUR_RGB32               |
                                  ARM_2D_COLOUR_BIG_ENDIAN_msk      ,
    ARM_2D_COLOUR_ACCC8888    =   ARM_2D_COLOUR_SZ_32BIT_msk        |
                                  ARM_2D_COLOUR_HAS_ALPHA           |
                                  ARM_2D_COLOUR_BIG_ENDIAN_msk      ,
                                  
    ARM_2D_CHANNEL_8in32      =   ARM_2D_COLOUR_SZ_32BIT_msk        |
                                  ARM_2D_COLOUR_HAS_ALPHA           |
                                  (0x07 << ARM_2D_COLOUR_VARIANT)   ,
};
//! @}


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

typedef struct arm_2d_location_t {
    int16_t iX;
    int16_t iY;
} arm_2d_location_t;

typedef struct arm_2d_point_float_t {
    float fX;
    float fY;
} arm_2d_point_float_t;

typedef struct arm_2d_point_fx_t {
    int32_t X;
    int32_t Y;
} arm_2d_point_fx_t;

typedef struct arm_2d_size_t {
    int16_t iWidth;
    int16_t iHeight;
} arm_2d_size_t;

typedef struct arm_2d_region_t {
    implement_ex(arm_2d_location_t, tLocation);
    implement_ex(arm_2d_size_t, tSize);
} arm_2d_region_t;

typedef struct arm_2d_tile_t arm_2d_tile_t;
struct arm_2d_tile_t {
    implement_ex(struct {
        uint8_t    bIsRoot              : 1;                                    //!< is this tile a root tile
        uint8_t    bHasEnforcedColour   : 1;                                    //!< does this tile contains enforced colour info
        uint8_t    bDerivedResource     : 1;                                    //!< indicate whether this is a derived resources (when bIsRoot == 0)
        uint8_t                         : 5;
        uint8_t                         : 8;
        uint8_t                         : 8;
        arm_2d_color_info_t    tColourInfo;                                     //!< enforced colour
    }, tInfo);

    implement_ex(arm_2d_region_t, tRegion);

    union {
        /*! when bIsRoot is true, phwBuffer is available,
         *! otherwise ptParent is available
         */
        arm_2d_tile_t       *ptParent;
        uint16_t            *phwBuffer;
        uint32_t            *pwBuffer;
        uint8_t             *pchBuffer;
        intptr_t            nAddress;
    };
};

/*----------------------------------------------------------------------------*
 * Task                                                                       *
 *----------------------------------------------------------------------------*/
typedef struct arm_2d_task_t {
ARM_PRIVATE(
    arm_fsm_rt_t tResult;
    uint8_t      chState;

    void         *ptTask;
)
} arm_2d_task_t;

/*----------------------------------------------------------------------------*
 * Operation and Events Handling                                              *
 *----------------------------------------------------------------------------*/

typedef struct arm_2d_op_core_t arm_2d_op_core_t;

typedef bool arm_2d_op_evt_handler_t(  arm_2d_op_core_t *ptThisOP,
                                    arm_fsm_rt_t tResult,
                                    void *pTarget);

typedef struct arm_2d_op_evt_t {
    arm_2d_op_evt_handler_t    *fnHandler;                                      //!< event handler
    void                       *pTarget;                                        //!< user attached target
} arm_2d_op_evt_t;

typedef bool arm_2d_evt_handler_t(void *pTarget);

typedef struct arm_2d_evt_t {
    arm_2d_evt_handler_t    *fnHandler;                                         //!< event handler
    void                    *pTarget;                                           //!< user attached target
} arm_2d_evt_t;


#define ARM_2D_OP_INFO_PARAM_HAS_SOURCE             _BV(0)
#define ARM_2D_OP_INFO_PARAM_HAS_TARGET             _BV(1)
#define ARM_2D_OP_INFO_PARAM_HAS_SOURCE_MASK        _BV(2)
#define ARM_2D_OP_INFO_PARAM_HAS_TARGET_MASK        _BV(3)
#define ARM_2D_OP_INFO_PARAM_HAS_ORIGIN             _BV(4)

//! an imcomplete defintion which is only used for defining pointers
typedef struct __arm_2d_low_level_io_t __arm_2d_low_level_io_t;

typedef union __arm_2d_op_info_t {
    struct {
        arm_2d_color_info_t Colour;     //!< the colour used in thie operation
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
            uint8_t chValue;
        }Param;

        uint8_t                 : 8;    //!< reserved
        uint8_t chOpIndex;              //!< __ARM_2D_OP_IDX_XXXXXX

        union {
            struct {
                uint8_t CopyLike;
                uint8_t FillLike;
            };
            struct {
                uint8_t CopyOrigLike;
                uint8_t FillOrigLike;
            };
            struct {
                uint8_t TileProcessLike;
            };
        }LowLevelInterfaceIndex;

        union {
            const __arm_2d_low_level_io_t *IO[2];

            struct {
                const __arm_2d_low_level_io_t *ptCopyLike;
                const __arm_2d_low_level_io_t *ptFillLike;
            };
            struct {
                const __arm_2d_low_level_io_t *ptCopyOrigLike;
                const __arm_2d_low_level_io_t *ptFillOrigLike;
            };
            struct {
                const __arm_2d_low_level_io_t *ptTileProcessLike;
            };
        }LowLevelIO;

    }Info;
    uint32_t    wID;                    //!< ID for a specific operation
} __arm_2d_op_info_t;

//! \name how would you want to accelerate the 2d-operation
//! @{
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
//! @}


#define __ARM_2D_OP_STATUS_BUSY_msk         (1 << 4)
#define __ARM_2D_OP_STATUS_IO_ERROR_msk     (1 << 5)
#define __ARM_2D_OP_STATUS_CPL_msk          (1 << 6)


typedef union arm_2d_op_status_t {
    struct {
        uint16_t            u4SubTaskCount  : 4;    //!< sub task count
        uint16_t            bIsBusy         : 1;    //!< busy flag
        uint16_t            bIOError        : 1;    //!< HW IO Error
        uint16_t            bOpCpl          : 1;    //!< the whole operation complete
        uint16_t                            : 9;    //!< reserved
    };
    uint16_t tValue;
} arm_2d_op_status_t;

struct arm_2d_op_core_t {
ARM_PRIVATE(
    arm_2d_op_core_t            *ptNext;              //!< pointer for a single list

    const __arm_2d_op_info_t    *ptOp;

    struct {
        uint8_t                 u2ACCMethods    : 2;    //!< acceleration Methods
        uint8_t                                 : 6;    //!< reserved
    }Preference;

    int8_t                      tResult;                //!< operation result
    volatile arm_2d_op_status_t Status;

    arm_2d_op_evt_t             evt2DOpCpl;             //!< operation complete event
    
)
    uintptr_t                   pUserParam;
};

typedef struct arm_2d_op_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
} arm_2d_op_t;

/*! \brief arm_2d_op_msk_t is inherit from arm_2d_op_t
 */
typedef struct arm_2d_op_msk_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    
    
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
    } Mask;
} arm_2d_op_msk_t;

/*! \brief arm_2d_op_src_t is inherit from arm_2d_op_t
 */
typedef struct arm_2d_op_src_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    
    
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile
    }Source;
    uint32_t wMode;
} arm_2d_op_src_t;

/*! \brief arm_2d_op_src_msk_t is inherit from arm_2d_op_src_t
 */
typedef struct arm_2d_op_src_msk_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile
    }Source;
    uint32_t wMode;
    
    
    struct {
        const arm_2d_tile_t     *ptSourceSide;  //!< source side mask
        const arm_2d_tile_t     *ptTargetSide;  //!< target side mask
    } Mask;
} arm_2d_op_src_msk_t;


/*! \brief arm_2d_op_src_orig_t is inherit from arm_2d_op_src_t
 */
typedef struct arm_2d_op_src_orig_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile
    }Source;
    uint32_t wMode;

    struct {
        const arm_2d_tile_t     *ptTile;        //!< the origin tile
        arm_2d_tile_t           tDummySource;   //!< the buffer for the source
    }Origin;

} arm_2d_op_src_orig_t;


/*! \brief arm_2d_op_src_orig_msk_t is inherit from arm_2d_op_src_orig_t
 */
typedef struct arm_2d_op_src_orig_msk_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile
    }Source;
    uint32_t wMode;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< the origin tile
        arm_2d_tile_t           tDummySource;   //!< the buffer for the source
    }Origin;
    
    
    struct {
        const arm_2d_tile_t     *ptSourceSide;  //!< source side mask
        const arm_2d_tile_t     *ptTargetSide;  //!< target side mask
    } Mask;
} arm_2d_op_src_orig_msk_t;


/*----------------------------------------------------------------------------*
 * Fast Rotation linear regression structure
 *----------------------------------------------------------------------------*/

#if     (__ARM_2D_HAS_HELIUM_FLOAT__ || __ARM_2D_HAS_FPU__)                     \
    && !__ARM_2D_CFG_FORCED_FIXED_POINT_ROTATION__
typedef struct arm_2d_rot_linear_regr_t {
    float   slopeY;
    float   interceptY;
    float   slopeX;
    float   interceptX;
} arm_2d_rot_linear_regr_t;

#else
/* fixed point */
typedef struct arm_2d_rot_linear_regr_t {
    int32_t   slopeY;
    int32_t   interceptY;
    int32_t   slopeX;
    int32_t   interceptX;
} arm_2d_rot_linear_regr_t;

#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


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


