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
 * Title:        #include "arm_2d_helper_pfb.h"
 * Description:  Public header file for the PFB helper service 
 *
 * $Date:        29. Aug 2022
 * $Revision:    V.1.2.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_HELPER_PFB_H__
#define __ARM_2D_HELPER_PFB_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"

#ifdef   __cplusplus
extern "C" {
#endif


#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wpadded"
#endif


/*!
 * \addtogroup gHelper 7 Helper Services
 * @{
 */

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

/*!
 * \brief a macro wrapper in uppercase to help initialising PFB service
 * \param[in] __CB_ADDR the address of the arm_2d_helper_pfb_t object
 * \param[in] __SCREEN_WIDTH the width of the screen
 * \param[in] __SCREEN_HEIGHT the hight of the screen
 * \param[in] __PIXEL_TYPE the integer type of the pixel, i.e. uint8_t, uint16_t,
 *                         uint32_t
 * \param[in] __WIDTH the width of the PFB block
 * \param[in] __HEIGHT the height of the PFB block
 * \note For the same number of pixels in a PFB block, please priority the width
 *       over height, for example, 240 * 1 is better than 30 * 8 
 * \param[in] __PFB_NUM the number of PFB blocks in the built-in PFB pool.
 * \param[in] ... a code block to add additional initializer, see example below:
 * \return arm_2d_err_t the result of the initialisation process
 * 
 * \code {.c}
    
    static ARM_NOINIT arm_2d_helper_pfb_t s_tExamplePFB;
    ...
    // initialise FPB helper
    if (ARM_2D_HELPER_PFB_INIT(
        &s_tExamplePFB,                 // FPB Helper object
        __GLCD_CFG_SCEEN_WIDTH__,       // screen width
        __GLCD_CFG_SCEEN_HEIGHT__,      // screen height
        uint16_t,                       // colour date type
        240,                            // PFB block width
        1,                              // PFB block height
        1,                              // number of PFB in the PFB pool
        {
            .evtOnLowLevelRendering = {
                // callback for low level rendering
                .fnHandler = &__pfb_render_handler,
            },
            .evtOnDrawing = {
                // callback for drawing GUI
                .fnHandler = &__pfb_draw_background_handler,
            },
        },
        //.FrameBuffer.bSwapRGB16 = true,
    ) < 0) {
        //! error detected
        assert(false);
    }
 * \endcode
 * 
 */
#define ARM_2D_HELPER_PFB_INIT( __CB_ADDR,      /* PFB Helper object address */ \
                                __SCREEN_WIDTH, /* Screen width */              \
                                __SCREEN_HEIGHT,/* Screen height */             \
                                __PIXEL_TYPE,   /* The type of the pixels */    \
                                __WIDTH,        /* The width of the PFB block */\
                                __HEIGHT,       /* The height of the PFB block*/\
                                __PFB_NUM,      /* Block count in the PFB pool*/\
                                ...             /* Event Handler */             \
                                )                                               \
    ({                                                                          \
        __attribute__((section(".bss.noinit.arm_2d_pfb_pool")))                 \
         static struct {                                                        \
            arm_2d_pfb_t tFPB;                                                  \
            __ALIGNED(4)                                                        \
            __PIXEL_TYPE tBuffer[(__WIDTH) * (__HEIGHT)];                       \
        } s_tPFBs[__PFB_NUM];                                                   \
                                                                                \
        arm_2d_helper_pfb_cfg_t tCFG = {                                        \
            .tDisplayArea.tSize = {                                             \
                .iWidth = (__SCREEN_WIDTH),                                     \
                .iHeight = (__SCREEN_HEIGHT),                                   \
            },                                                                  \
                                                                                \
            .FrameBuffer.ptPFBs = (arm_2d_pfb_t *)s_tPFBs,                      \
            .FrameBuffer.tFrameSize = {                                         \
                .iWidth = (__WIDTH),                                            \
                .iHeight = (__HEIGHT),                                          \
            },                                                                  \
            .FrameBuffer.wBufferSize = sizeof(s_tPFBs[0].tBuffer),              \
            .FrameBuffer.hwPFBNum = dimof(s_tPFBs),                             \
            .Dependency =                                                       \
            __VA_ARGS__                                                         \
        };                                                                      \
                                                                                \
        arm_2d_helper_pfb_init((__CB_ADDR), &tCFG);                             \
    })

/*!
 * \brief a macro wrapper to update the evtOnDrawring event handler
 * \param[in] __CB_ADDR the address of the arm_2d_helper_pfb_t object
 * \param[in] __HANDLER the new handler
 * \param[in] ... [Optional] an address (of user defined structure) passed to the 
 *                      event handler.
 * \return arm_2d_err_t the process result
 */
#define ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(                               \
                                    __CB_ADDR, /* PFB Helper object address */  \
                                    __HANDLER, /* new on-draw-handler function*/\
                                    ...)       /* An optional target address */ \
    arm_2d_helper_pfb_update_dependency((__CB_ADDR),                            \
                                        ARM_2D_PFB_DEPEND_ON_DRAWING,           \
                                        (arm_2d_helper_pfb_dependency_t []) {{  \
                                            .evtOnDrawing = {                   \
                                                .fnHandler = (__HANDLER),       \
                                                .pTarget = (NULL,##__VA_ARGS__),\
                                            },                                  \
                                        }})


#define __IMPL_ARM_2D_REGION_LIST(__NAME, ...)                                  \
            enum {                                                              \
                __NAME##_offset = __COUNTER__,                                  \
            };                                                                  \
            __VA_ARGS__                                                         \
            arm_2d_region_list_item_t __NAME[] = {
            
            
#define IMPL_ARM_2D_REGION_LIST(__NAME, ...)                                    \
            __IMPL_ARM_2D_REGION_LIST(__NAME,##__VA_ARGS__)
            

#define END_IMPL_ARM_2D_REGION_LIST(...)                                        \
            };
            
#define __ADD_REGION_TO_LIST(__NAME, ...)                                       \
            {                                                                   \
                .ptNext = (arm_2d_region_list_item_t *)                         \
                            &(__NAME[__COUNTER__ - __NAME##_offset]),           \
                .tRegion = {                                                    \
                    __VA_ARGS__                                                 \
                },                                                              \
            }
            
#define ADD_REGION_TO_LIST(__NAME, ...)                                         \
            __ADD_REGION_TO_LIST(__NAME, ##__VA_ARGS__) 
            
           
#define __ADD_LAST_REGION_TO_LIST(__NAME, ...)                                  \
            {                                                                   \
                .ptNext = NULL,                                                 \
                .tRegion = {                                                    \
                    __VA_ARGS__                                                 \
                },                                                              \
            }
            
#define ADD_LAST_REGION_TO_LIST(__NAME, ...)                                    \
            __ADD_LAST_REGION_TO_LIST(__NAME, ##__VA_ARGS__) 
            
#define IMPL_PFB_ON_DRAW(__NAME)                                                \
            arm_fsm_rt_t __NAME(void *pTarget,                                  \
                                const arm_2d_tile_t *ptTile,                    \
                                bool bIsNewFrame)    

#define IMPL_PFB_ON_LOW_LV_RENDERING(__NAME)                                    \
            void __NAME(void *pTarget,                                          \
                        const arm_2d_pfb_t *ptPFB,                              \
                        bool bIsNewFrame)    


#define IMPL_PFB_ON_FRAME_SYNC_UP(__NAME)                                       \
            bool __NAME(void *pTarget)


/*!
 * \brief a macro wrapper in lowercase to help initialising PFB service
 * \param[in] __CB_ADDR the address of the arm_2d_helper_pfb_t object
 * \param[in] __SCREEN_WIDTH the width of the screen
 * \param[in] __SCREEN_HEIGHT the hight of the screen
 * \param[in] __PIXEL_TYPE the integer type of the pixel, i.e. uint8_t, uint16_t,
 *                         uint32_t
 * \param[in] __WIDTH the width of the PFB block
 * \param[in] __HEIGHT the height of the PFB block
 * \note For the same number of pixels in a PFB block, please priority the width
 *       over height, for example, 240 * 1 is better than 30 * 8 
 * \param[in] __PFB_NUM the number of PFB blocks in the built-in PFB pool.
 * \param[in] ... a code block to add additional initializer, see example below:
 * \return arm_2d_err_t the result of the initialisation process
 * 
 * \code {.c}
    
    static ARM_NOINIT arm_2d_helper_pfb_t s_tExamplePFB;
    ...
    // initialise FPB helper
    if (init_arm_2d_helper_pfb(
        &s_tExamplePFB,                 // FPB Helper object
        __GLCD_CFG_SCEEN_WIDTH__,       // screen width
        __GLCD_CFG_SCEEN_HEIGHT__,      // screen height
        uint16_t,                       // colour date type
        240,                            // PFB block width
        1,                              // PFB block height
        1,                              // number of PFB in the PFB pool
        {
            .evtOnLowLevelRendering = {
                // callback for low level rendering
                .fnHandler = &__pfb_render_handler,
            },
            .evtOnDrawing = {
                // callback for drawing GUI
                .fnHandler = &__pfb_draw_background_handler,
            },
        },
        //.FrameBuffer.bSwapRGB16 = true,
    ) < 0) {
        //! error detected
        assert(false);
    }
 * \endcode
 * 
 */
#define init_arm_2d_helper_pfb( __CB_ADDR,                                      \
                                __SCREEN_WIDTH,                                 \
                                __SCREEN_HEIGHT,                                \
                                __PIXEL_TYPE,                                   \
                                __WIDTH,                                        \
                                __HEIGHT,                                       \
                                __PFB_NUM,                                      \
                                ...                                             \
                                )                                               \
            ARM_2D_HELPER_PFB_INIT(                                             \
                                __CB_ADDR,                                      \
                                __SCREEN_WIDTH,                                 \
                                __SCREEN_HEIGHT,                                \
                                __PIXEL_TYPE,                                   \
                                __WIDTH,                                        \
                                __HEIGHT,                                       \
                                __PFB_NUM,                                      \
                                ##__VA_ARGS__                                   \
                                )

/*!
 * \brief a macro wrapper to update the evtOnDrawring event handler
 * \param[in] __CB_ADDR the address of the arm_2d_helper_pfb_t object
 * \param[in] __HANDLER the new handler
 * \param[in] ... [Optional] an address (of user defined structure) passed to the 
 *                      event handler.
 * \return arm_2d_err_t the process result
 */
#define update_arm_2d_helper_pfb_on_draw_handler(                               \
                                    __CB_ADDR, /* PFB Helper object address */  \
                                    __HANDLER, /* new on-draw-handler function*/\
                                    ...)       /* An optional target address */ \
            ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(                           \
                                    (__CB_ADDR),                                \
                                    (__HANDLER),##__VA_ARGRS__)

#define impl_arm_2d_region_list(__NAME, ...)                                    \
            IMPL_ARM_2D_REGION_LIST(__NAME,##__VA_ARGS__)
#define add_region_to_list(__NAME, ...)                                         \
            ADD_REGION_TO_LIST(__NAME, ##__VA_ARGS__) 
#define add_last_region_to_list(__NAME, ...)                                    \
            ADD_LAST_REGION_TO_LIST(__NAME, ##__VA_ARGS__)
#define end_impl_arm_2d_region_list(...)                                        \
            END_IMPL_ARM_2D_REGION_LIST(__VA_ARGS__)
            
#define impl_pfb_on_draw(__NAME)    IMPL_PFB_ON_DRAW(__NAME)
#define impl_pfb_on_low_lv_rendering(__NAME)                                    \
            IMPL_PFB_ON_LOW_LV_RENDERING(__NAME)

/*============================ TYPES =========================================*/

typedef struct arm_2d_helper_pfb_t arm_2d_helper_pfb_t;
/*!
 * \brief the header of a PFB block
 */
typedef struct arm_2d_pfb_t {
    struct arm_2d_pfb_t *ptNext;                                                //!< next pfb block
    arm_2d_helper_pfb_t *ptPFBHelper;                                           //!< the pfb helper service current PFB block comes from
    arm_2d_tile_t tTile;                                                        //!< descriptor
    uint32_t u24Size                : 24;
    uint32_t                        : 7;
    uint32_t bIsNewFrame            : 1;                                        //!< a flag to indicate the starting of a frame
}arm_2d_pfb_t;

/*!
 * \brief the node of a region list
 * 
 */
typedef struct arm_2d_region_list_item_t {
    struct arm_2d_region_list_item_t *ptNext;   //!< the next node
    arm_2d_region_t tRegion;                    //!< the region
}arm_2d_region_list_item_t;

/*!
 * \brief the On-Drawing event handler for application layer
 * 
 * \param[in] pTarget a user attached target address 
 * \param[in] ptTile a tile for the virtual screen
 * \param[in] bIsNewFrame a flag indicate the starting of a new frame
 * \return arm_fsm_rt_t the status of the FSM.  
 */
typedef arm_fsm_rt_t arm_2d_helper_draw_handler_t( 
                                          void *pTarget,
                                          const arm_2d_tile_t *ptTile,
                                          bool bIsNewFrame);

/*!
 * \brief the On Low Level Rendering event handler for the low level (LCD Driver)
 * 
 * \param[in] pTarget a user attached target address 
 * \param[in] ptPFB the PFB block
 * \param[in] bIsNewFrame a flag indicate the starting of a new frame
 */
typedef void arm_2d_helper_render_handler_t( 
                                          void *pTarget,
                                          const arm_2d_pfb_t *ptPFB,
                                          bool bIsNewFrame);

/*!
 * \brief on low level render event 
 */
typedef struct arm_2d_helper_render_evt_t {
    arm_2d_helper_render_handler_t *fnHandler;              //!< event handler function
    void *pTarget;                                          //!< user attached target
} arm_2d_helper_render_evt_t;

/*!
 * \brief on drawing event 
 */
typedef struct arm_2d_helper_draw_evt_t {
    arm_2d_helper_draw_handler_t *fnHandler;                //!< event handler function
    void *pTarget;                                          //!< user attached target
} arm_2d_helper_draw_evt_t;

/*!
 * \brief the enumeration for events
 * 
 */
enum {
    ARM_2D_PFB_DEPEND_ON_LOW_LEVEL_RENDERING    = _BV(0),   //!< On Low Level Rendering Event
    ARM_2D_PFB_DEPEND_ON_DRAWING                = _BV(1),   //!< On Drawing Event
    ARM_2D_PFB_DEPEND_ON_LOW_LEVEL_SYNC_UP      = _BV(2),   //!< On Low Level Sync-up Event
    ARM_2D_PFB_DEPEND_ON_FRAME_SYNC_UP          = _BV(3),   //!< On Frame Sync-up Event
};

/*!
 * \brief The PFB Helper Service Dependency 
 * 
 */
typedef struct arm_2d_helper_pfb_dependency_t {
    //! event handler for low level rendering
    arm_2d_helper_render_evt_t  evtOnLowLevelRendering;                           

    //! event handler for drawing GUI
    arm_2d_helper_draw_evt_t    evtOnDrawing;  

    //! low level rendering handler wants to sync-up (return arm_fsm_rt_wait_for_obj)
    arm_2d_evt_t                evtOnLowLevelSyncUp;  
    
} arm_2d_helper_pfb_dependency_t;

/*!
 * \brief PFB Helper configuration
 * 
 */
typedef struct arm_2d_helper_pfb_cfg_t {

    arm_2d_region_t tDisplayArea;                               //!< screen description
    
    struct {
        arm_2d_pfb_t  *ptPFBs;                                  //!< current PFB block
        arm_2d_size_t  tFrameSize;                              //!< the size of the frame
        uint32_t       wBufferSize;                             //!< the buffer size
        uint16_t       hwPFBNum;                                //!< the number of PFB
        uint16_t       bDoNOTUpdateDefaultFrameBuffer   : 1;    //!< A flag to disable automatically default-framebuffer-registration
        uint16_t       bDisableDynamicFPBSize           : 1;    //!< A flag to disable resize of the PFB block
        uint16_t       bSwapRGB16                       : 1;    //!< A flag to enable swapping high and low bytes of an RGB16 pixel
        uint16_t                                        : 9;
        uint16_t       u4PoolReserve                    : 4;    //!< reserve specific number of PFB for other helper services

    } FrameBuffer;                                              //!< frame buffer context
    
    arm_2d_helper_pfb_dependency_t Dependency;                  //!< user registered dependency

} arm_2d_helper_pfb_cfg_t;

/*!
 * \brief the PFB helper control block
 * 
 */
struct arm_2d_helper_pfb_t{

ARM_PRIVATE(
    arm_2d_helper_pfb_cfg_t tCFG;                               //!< user configuration 
    
    struct {                                                    
        arm_2d_region_t             tDrawRegion;
        arm_2d_region_t             tTargetRegion;
        arm_2d_region_list_item_t  *ptDirtyRegion;
        arm_2d_tile_t               tPFBTile;
        arm_2d_size_t               tFrameSize;
        bool                        bFirstIteration;
        bool                        bIsRegionChanged;
        uint8_t                     chPT;
        struct {
            uint8_t                 bIsNewFrame  : 1;
            uint8_t                 bIsFlushRequested :1;
        };
        uint16_t                    hwFreePFBCount;
        arm_2d_pfb_t               *ptCurrent;
        arm_2d_pfb_t               *ptFreeList;
        
        struct {
            arm_2d_pfb_t           *ptHead;
            arm_2d_pfb_t           *ptTail;
        }FlushFIFO;
        arm_2d_tile_t               *ptFrameBuffer;
    } Adapter;                                                  
)   

    struct {
        int64_t lTimestamp;                                     //!< PLEASE DO NOT USE
        int32_t nTotalCycle;                                    //!< cycles used by drawing 
        int32_t nRenderingCycle;                                //!< cycles used in LCD flushing
    } Statistics;                                               //!< performance statistics

};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*!
 * \brief initialize pfb helper service
 * \param[in] ptThis the pfb helper control block
 * \param[in] ptCFG the configuration 
 * \return arm_2d_err_t the process result
 */
extern
ARM_NONNULL(1,2)
arm_2d_err_t arm_2d_helper_pfb_init(arm_2d_helper_pfb_t *ptThis, 
                                    arm_2d_helper_pfb_cfg_t *ptCFG);

/*!
 * \brief get the display (screen) region
 * \param[in] ptThis the pfb helper control block
 * \return arm_2d_region_t the screen region
 */
extern
ARM_NONNULL(1)
arm_2d_region_t arm_2d_helper_pfb_get_display_area(arm_2d_helper_pfb_t *ptThis);

/*!
 * \brief the task function for pfb helper
 * \param[in] ptThis an initialised PFB control block
 * \param[in] ptDirtyRegions a region list pending for refresh, NULL means 
 *                           refreshing the whole screen
 * \retval arm_fsm_rt_cpl complete refreshing one frame
 * \retval arm_fsm_rt_on_going the refreshing work is on-going
 * \retval arm_fsm_rt_wait_for_obj user's OnDrawing event handler wants to wait 
 *                                 for some objects, e.g. semaphore etc.
 * \retval <0 An error is detected
 */
extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_helper_pfb_task(arm_2d_helper_pfb_t *ptThis, 
                                    arm_2d_region_list_item_t *ptDirtyRegions);

/*!
 * \brief flush the FPB FIFO 
 * \note This function is THREAD-SAFE
 * \note For normal usage, please DO NOT use this function unless you know what
 *       you are doing.
 * \param[in] ptThis an initialised PFB control block
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_pfb_flush(arm_2d_helper_pfb_t *ptThis);

/*!
 * \brief update PFB dependency (event handlers)
 * \param[in] ptThis the PFB control block
 * \param[in] chMask the bit mask for event handlers
 * \param[in] ptDependency the new dependency description
 * \return arm_2d_err_t the process result
 */
extern
ARM_NONNULL(1,3)
arm_2d_err_t arm_2d_helper_pfb_update_dependency(
                            arm_2d_helper_pfb_t *ptThis, 
                            uint_fast8_t chMask,
                            const arm_2d_helper_pfb_dependency_t *ptDependency);

/*!
 *  \brief tell PFB helper that a low level LCD flushing work is complete
 *  \note This function is THREAD-SAFE, You can call this function asynchronously, 
 *        e.g.
 *        - A ISR to indicate DMA-transfer complete event or 
 *        - A different Thread
 *  \param[in] ptThis the PFB control block
 *  \param[in] ptPFB the used PFB block
 */
extern
ARM_NONNULL(1,2)
void arm_2d_helper_pfb_report_rendering_complete(   arm_2d_helper_pfb_t *ptThis,
                                                    arm_2d_pfb_t *ptPFB);

/*!
 * \brief swap the high and low bytes for each rgb16 pixel
 *
 * \params[in] phwBuffer the pixel buffer
 * \note the phwBuffer MUST aligned to half-word addresses
 *
 * \params[in] wSize the number of pixels
 */
extern
void arm_2d_helper_swap_rgb16(uint16_t *phwBuffer, uint32_t wCount);

/*! 
 * \brief convert ticks of a reference timer to millisecond 
 *
 * \param[in] lTick the tick count
 * \return int64_t the millisecond
 */
extern
int64_t arm_2d_helper_convert_ticks_to_ms(int64_t lTick);

/*! 
 * \brief convert millisecond into ticks of the reference timer 
 *
 * \param[in] wMS the target time in millisecond
 * \return int64_t the ticks
 */
extern
int64_t arm_2d_helper_convert_ms_to_ticks(uint32_t wMS);

/*!
 * \brief get the reference clock frequency
 * \return uint32_t the frequency
 */
extern
uint32_t arm_2d_helper_get_reference_clock_frequency(void);

/*!
 * \brief get the current system stamp from the reference clock
 * 
 * \return int64_t the timestamp in ticks (no overflow issue)
 * \note you have to call arm_2d_helper_convert_ticks_to_ms() to convert the 
 *       the timestamp into milliseconds when required.
 */
extern
int64_t arm_2d_helper_get_system_timestamp(void);

/*!
 * \brief try to get a PFB block from the pool
 * \param[in] ptThis the PFB control block
 * \retval NULL the pool is empty
 * \retval !NULL a valid pfb block 
 */
extern
ARM_NONNULL(1)
arm_2d_pfb_t *__arm_2d_helper_pfb_new(arm_2d_helper_pfb_t *ptThis);

/*!
 * \brief free a PFB block to the pool
 * \param[in] ptThis the PFB control block
 * \param[in] ptPFB the target PFB block
 */
extern
ARM_NONNULL(1)
void __arm_2d_helper_pfb_free(arm_2d_helper_pfb_t *ptThis, arm_2d_pfb_t *ptPFB);

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif



#endif
