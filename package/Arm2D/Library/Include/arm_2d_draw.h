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
 * Title:        #include "arm_2d.h"
 * Description:  Public header file to contain the APIs for colour space
 *               conversions
 *
 * $Date:        17. June 2022
 * $Revision:    V.1.0.2
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */
 
#ifndef __ARM_2D_DRAW_H__
#define __ARM_2D_DRAW_H__

/*============================ INCLUDES ======================================*/

#include "arm_2d_types.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#endif

/*!
 * \addtogroup Drawing 3 Drawing Operations
 * @{
 */

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define arm_2d_c8bit_draw_point(__TARGET_ADDR,  /* target tile address */       \
                                __LOCATION,     /* point coordinate */          \
                                __COLOUR)       /* target colour */             \
            arm_2dp_c8bit_draw_point(NULL,                                      \
                                     (__TARGET_ADDR),                           \
                                     (__LOCATION),                              \
                                     (__COLOUR))

#define arm_2d_rgb16_draw_point(__TARGET_ADDR,  /* target tile address */       \
                                __LOCATION,     /* point coordinate */          \
                                __COLOUR)       /* target colour */             \
            arm_2dp_rgb16_draw_point(NULL,                                      \
                                     (__TARGET_ADDR),                           \
                                     (__LOCATION),                              \
                                     (__COLOUR))

#define arm_2d_rgb32_draw_point(__TARGET_ADDR,  /* target tile address */       \
                                __LOCATION,     /* point coordinate */          \
                                __COLOUR)       /* target colour */             \
            arm_2dp_rgb32_draw_point(NULL,                                      \
                                     (__TARGET_ADDR),                           \
                                     (__LOCATION),                              \
                                     (__COLOUR))


#define arm_2d_c8bit_draw_pattern(  __PATTERN_ADDR, /* pattern tile address */  \
                                    __TARGET_ADDR,  /* target tile address*/    \
                                    __REGION_ADDR,  /* target region address*/  \
                                    __MODE,         /* draw mode */             \
                                    __FG_COLOUR,    /* foreground colour */     \
                                    __BG_COLOUR)    /* background colour */     \
            arm_2dp_c8bit_draw_pattern(NULL,                                    \
                                     (__PATTERN_ADDR),                          \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__MODE),                                  \
                                     (__FG_COLOUR),                             \
                                     (__BG_COLOUR))

#define arm_2d_rgb16_draw_pattern(  __PATTERN_ADDR, /* pattern tile address */  \
                                    __TARGET_ADDR,  /* target tile address*/    \
                                    __REGION_ADDR,  /* target region address*/  \
                                    __MODE,         /* draw mode */             \
                                    __FG_COLOUR,    /* foreground colour */     \
                                    __BG_COLOUR)    /* background colour */     \
            arm_2dp_rgb16_draw_pattern(NULL,                                    \
                                     (__PATTERN_ADDR),                          \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__MODE),                                  \
                                     (__FG_COLOUR),                             \
                                     (__BG_COLOUR))

#define arm_2d_rgb32_draw_pattern(  __PATTERN_ADDR, /* pattern tile address */  \
                                    __TARGET_ADDR,  /* target tile address*/    \
                                    __REGION_ADDR,  /* target region address*/  \
                                    __MODE,         /* draw mode */             \
                                    __FG_COLOUR,    /* foreground colour */     \
                                    __BG_COLOUR)    /* background colour */     \
            arm_2dp_rgb32_draw_pattern(NULL,                                    \
                                     (__PATTERN_ADDR),                          \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__MODE),                                  \
                                     (__FG_COLOUR),                             \
                                     (__BG_COLOUR))

#define arm_2d_c8bit_fill_colour(   __TARGET_ADDR,  /* target tile address*/    \
                                    __REGION_ADDR,  /* target region address*/  \
                                    __COLOUR)       /* colour */                \
            arm_2dp_c8bit_fill_colour(NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__COLOUR))
 
#define arm_2d_rgb16_fill_colour(   __TARGET_ADDR,  /* target tile address*/    \
                                    __REGION_ADDR,  /* target region address*/  \
                                    __COLOUR)       /* colour */                \
            arm_2dp_rgb16_fill_colour(NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__COLOUR))

#define arm_2d_rgb32_fill_colour(   __TARGET_ADDR,  /* target tile address*/    \
                                    __REGION_ADDR,  /* target region address*/  \
                                    __COLOUR)       /* colour */                \
            arm_2dp_rgb32_fill_colour(NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__COLOUR))

/*============================ TYPES =========================================*/

/*! 
 *  \brief the control block for colour-filling-operations
 *  \note arm_2d_op_fill_cl_t inherits from arm_2d_op_t explicitly 
 */
typedef struct arm_2d_op_fill_cl_t {
    inherit(arm_2d_op_core_t);                  //!< base
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile 
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    union {
        uint8_t  chColour;                      //!< 8bit colour
        uint16_t hwColour;                      //!< 16bit colour
        uint32_t wColour;                       //!< 32bit colour
    };
} arm_2d_op_fill_cl_t;

/*! \note arm_2d_op_fill_cl_t inherits from arm_2d_op_t explicitly 
 */
typedef arm_2d_op_fill_cl_t arm_2d_op_drw_pt_t;

/*!
 *  \brief the control block for draw-bit-pattern operations
 *  \note arm_2d_op_drw_patn_t inherits from arm_2d_op_src_t explicitly 
 */
typedef struct arm_2d_op_drw_patn_t {
    inherit(arm_2d_op_core_t);                  //!< base
    
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile 
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile 
    }Source;
    uint32_t wMode;                             //!< mode of the operation
    union {
        uint8_t  chColour;                      //!< 8bit colour
        uint16_t hwColour;                      //!< 16bit colour
        uint32_t wColour;                       //!< 32bit colour
    }Foreground;                                //!< forground colour
    union {
        uint8_t  chColour;                      //!< 8bit colour
        uint16_t hwColour;                      //!< 16bit colour
        uint32_t wColour;                       //!< 32bit colour
    }Background;                                //!< background colour
    
} arm_2d_op_drw_patn_t;


/*!
 *  \brief modes for copying bit-patterns
 */
enum {
    ARM_2D_DRW_PATN_MODE_COPY               =  0,                               //!< copy bit pattern
    //ARM_2D_DRW_PATN_MODE_FILL             =  _BV(0),                          //!< not support yet
    //ARM_2D_DRW_PATN_MODE_Y_MIRROR         =  _BV(2),                          //!< not support yet
    //ARM_2D_DRW_PATN_MODE_X_MIRROR         =  _BV(3),                          //!< not support yet
    ARM_2D_DRW_PATN_MODE_WITH_BG_COLOR      =  _BV(4),                          //!< use user specified backgound colour
    ARM_2D_DRW_PATN_MODE_NO_FG_COLOR        =  _BV(5),                          //!< no forground
    
    /*! use complementary colour as foreground colour
     *  
     *  \note this option is only avaialble when ARM_2D_DRW_PATN_MODE_NO_FG_COLOR
     *        is used together.
     */
    ARM_2D_DRW_PATH_MODE_COMP_FG_COLOUR   =  _BV(6),
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*----------------------------------------------------------------------------*
 * Draw a point with specified colour                                         *
 *----------------------------------------------------------------------------*/

/*!
 *  \brief draw a point on a root tile with a given 8bit colour
 *  \param[in] ptTarget the target root tile
 *  \param[in] tLocation the target location
 *  \note      the point must be inside the region of the target tile
 *  \param[in] chColour an 8bit colour
 */
ARM_NONNULL(1)
__STATIC_INLINE void arm_2d_c8bit_draw_point_fast(  const arm_2d_tile_t *ptTarget,
                                    const arm_2d_location_t tLocation,
                                    uint_fast8_t chColour)
{
    assert(NULL != ptTarget);
    assert(ptTarget->bIsRoot);          // must be root tile
    assert(tLocation.iX < ptTarget->tRegion.tSize.iWidth);
    assert(tLocation.iY < ptTarget->tRegion.tSize.iHeight);
    
    uint8_t *pchPoint  = ptTarget->pchBuffer
                       + tLocation.iY * ptTarget->tRegion.tSize.iWidth
                       + tLocation.iX;
    *pchPoint = (uint8_t)chColour;
}

/*!
 *  \brief draw a point on a root tile with a given 16bit colour
 *  \param[in] ptTarget the target root tile
 *  \param[in] tLocation the target location
 *  \note      the point must be inside the region of the target tile
 *  \param[in] hwColour an 16bit colour
 */
ARM_NONNULL(1)
__STATIC_INLINE void arm_2d_rgb16_draw_point_fast(
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_location_t tLocation,
                                            uint_fast16_t hwColour)
{
    assert(NULL != ptTarget);
    assert(ptTarget->bIsRoot);          // must be root tile
    assert(tLocation.iX < ptTarget->tRegion.tSize.iWidth);
    assert(tLocation.iY < ptTarget->tRegion.tSize.iHeight);
    
    uint16_t *phwPoint = ptTarget->phwBuffer
                       + tLocation.iY * ptTarget->tRegion.tSize.iWidth
                       + tLocation.iX;
    *phwPoint = (uint16_t)hwColour;
}

/*!
 *  \brief draw a point on a root tile with a given 32bit colour
 *  \param[in] ptTarget the target root tile
 *  \param[in] tLocation the target location
 *  \note      the point must be inside the region of the target tile
 *  \param[in] wColour an 32bit colour
 */
ARM_NONNULL(1)
__STATIC_INLINE void arm_2d_rgb32_draw_point_fast(   
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_location_t tLocation,
                                            uint32_t wColour)
{
    assert(NULL != ptTarget);
    assert(ptTarget->bIsRoot);          // must be root tile
    assert(tLocation.iX < ptTarget->tRegion.tSize.iWidth);
    assert(tLocation.iY < ptTarget->tRegion.tSize.iHeight);
    
    uint32_t *pwPoint = ptTarget->pwBuffer
                       + tLocation.iY * ptTarget->tRegion.tSize.iWidth
                       + tLocation.iX;
    *pwPoint = wColour;
}

/*!
 *  \brief draw a point with a given 8bit colour
 *  \param[in] ptOP the control block, NULL means using the default control block
 *  \param[in] ptTarget the target root tile
 *  \param[in] tLocation the target location
 *  \param[in] chColour an 8bit colour
 *  \return arm_fsm_rt_t the operation result
 *
 *  \note As those draw point APIs involve the region calculation
 *        which is only useful when partial framebuffer is used, it is slow.
 *        For gettting better performance, if the target tile is root and the 
 *        target location is inside the target region, please use the
 *        functions with "_fast" posfix.
 *        
 */
extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_c8bit_draw_point(  arm_2d_op_drw_pt_t  *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_location_t tLocation,
                                        uint_fast8_t chColour);

/*!
 *  \brief draw a point with a given 16bit colour
 *  \param[in] ptOP the control block, NULL means using the default control block
 *  \param[in] ptTarget the target root tile
 *  \param[in] tLocation the target location
 *  \param[in] hwColour an 16bit colour
 *  \return arm_fsm_rt_t the operation result
 *
 *  \note As those draw point APIs involve the region calculation
 *        which is only useful when partial framebuffer is used, it is slow.
 *        For gettting better performance, if the target tile is root and the 
 *        target location is inside the target region, please use the
 *        functions with "_fast" posfix.
 *        
 */
extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb16_draw_point(  arm_2d_op_drw_pt_t  *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_location_t tLocation,
                                        uint_fast16_t hwColour);

/*!
 *  \brief draw a point with a given 32bit colour
 *  \param[in] ptOP the control block, NULL means using the default control block
 *  \param[in] ptTarget the target root tile
 *  \param[in] tLocation the target location
 *  \param[in] wColour an 32bit colour
 *  \return arm_fsm_rt_t the operation result
 *
 *  \note As those draw point APIs involve the region calculation
 *        which is only useful when partial framebuffer is used, it is slow.
 *        For gettting better performance, if the target tile is root and the 
 *        target location is inside the target region, please use the
 *        functions with "_fast" posfix.
 *        
 */
extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb32_draw_point(  arm_2d_op_drw_pt_t  *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_location_t tLocation,
                                        uint32_t wColour);


#if 0       // todo: draw point with alpha
extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_rgba8888_draw_point(const arm_2d_tile_t *ptTarget,
                                        const arm_2d_location_t tLocation,
                                        arm_2d_color_bgra8888_t tColour);
#endif 


/*----------------------------------------------------------------------------*
 * Draw a bit patterns                                                        *
 *----------------------------------------------------------------------------*/

/*!
 *  \brief copy a bit-pattern with given 8bit colours
 *  \param[in] ptOP the control block, NULL means using the default control block
 *  \param[in] ptPattern the source bit pattern
 *  \param[in] ptTarget the target tile
 *  \param[in] ptRegion the target region
 *  \param[in] wMode the copy mode
 *  \param[in] chForeColour the foreground colour
 *  \param[in] chBackColour the background colour
 *  \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_c8bit_draw_pattern( arm_2d_op_drw_patn_t   *ptOP, 
                                         const arm_2d_tile_t    *ptPattern,
                                         const arm_2d_tile_t    *ptTarget,
                                         const arm_2d_region_t  *ptRegion,
                                         uint32_t wMode,
                                         uint8_t chForeColour,
                                         uint8_t chBackColour);

/*!
 *  \brief copy a bit-pattern with given 16bit colours
 *  \param[in] ptOP the control block, NULL means using the default control block
 *  \param[in] ptPattern the source bit pattern
 *  \param[in] ptTarget the target tile
 *  \param[in] ptRegion the target region
 *  \param[in] wMode the copy mode
 *  \param[in] hwForeColour the foreground colour
 *  \param[in] hwBackColour the background colour
 *  \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb16_draw_pattern( arm_2d_op_drw_patn_t   *ptOP,
                                         const arm_2d_tile_t    *ptPattern,
                                         const arm_2d_tile_t    *ptTarget,
                                         const arm_2d_region_t  *ptRegion,
                                         uint32_t wMode,
                                         uint16_t hwForeColour,
                                         uint16_t hwBackColour);

/*!
 *  \brief copy a bit-pattern with given 32bit colours
 *  \param[in] ptOP the control block, NULL means using the default control block
 *  \param[in] ptPattern the source bit pattern
 *  \param[in] ptTarget the target tile
 *  \param[in] ptRegion the target region
 *  \param[in] wMode the copy mode
 *  \param[in] wForeColour the foreground colour
 *  \param[in] wBackColour the background colour
 *  \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb32_draw_pattern( arm_2d_op_drw_patn_t   *ptOP,
                                         const arm_2d_tile_t    *ptPattern,
                                         const arm_2d_tile_t    *ptTarget,
                                         const arm_2d_region_t  *ptRegion,
                                         uint32_t wMode,
                                         uint32_t wForeColour,
                                         uint32_t wBackColour);

/*----------------------------------------------------------------------------*
 * Fill tile with a specified colour                                          *
 *----------------------------------------------------------------------------*/

/*!
 *  \brief fill the target region with a given 8bit colour
 *  \param[in] ptOP the control block, NULL means using the default control block
 *  \param[in] ptTarget the target tile
 *  \param[in] ptRegion the target region
 *  \param[in] chColour a 8bit colour
 *  \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_c8bit_fill_colour( arm_2d_op_fill_cl_t     *ptOP,
                                        const arm_2d_tile_t     *ptTarget,
                                        const arm_2d_region_t   *ptRegion,
                                        uint_fast8_t chColour);

/*!
 *  \brief fill the target region with a given 16bit colour
 *  \param[in] ptOP the control block, NULL means using the default control block
 *  \param[in] ptTarget the target tile
 *  \param[in] ptRegion the target region
 *  \param[in] hwColour a 16bit colour
 *  \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb16_fill_colour( arm_2d_op_fill_cl_t     *ptOP,
                                        const arm_2d_tile_t     *ptTarget,
                                        const arm_2d_region_t   *ptRegion,
                                        uint_fast16_t hwColour);

/*!
 *  \brief fill the target region with a given 32bit colour
 *  \param[in] ptOP the control block, NULL means using the default control block
 *  \param[in] ptTarget the target tile
 *  \param[in] ptRegion the target region
 *  \param[in] wColour a 32bit colour
 *  \return arm_fsm_rt_t the operations result
 */
extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb32_fill_colour( arm_2d_op_fill_cl_t     *ptOP,
                                        const arm_2d_tile_t     *ptTarget,
                                        const arm_2d_region_t   *ptRegion,
                                        uint32_t wColour);


/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
