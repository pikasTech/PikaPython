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

/*============================ INCLUDES ======================================*/
#include <stdio.h>
#include "platform.h"
#include "arm_2d_helper.h"
#include "example_gui.h"

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wunused-variable"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Wbad-function-cast"
#   pragma clang diagnostic ignored "-Wunreachable-code-break"
#   pragma clang diagnostic ignored "-Wshorten-64-to-32"
#   pragma clang diagnostic ignored "-Wdouble-promotion"
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#endif

/*============================ MACROS ========================================*/
#ifndef __STR
#   define __STR(__A)      #__A
#endif

#ifndef STR
#   define STR(__A)         __STR(__A)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

__OVERRIDE_WEAK
arm_2d_runtime_feature_t ARM_2D_RUNTIME_FEATURE = {
    .TREAT_OUT_OF_RANGE_AS_COMPLETE         = 1,
    .HAS_DEDICATED_THREAD_FOR_2D_TASK       = 1,
};

/*============================ PROTOTYPES ====================================*/
__attribute__((nothrow)) 
extern int64_t clock(void);

/*============================ LOCAL VARIABLES ===============================*/

static char s_chPerformanceInfo[MAX(((GLCD_WIDTH/6)+1), 54)] = {0};

osEventFlagsId_t s_evt2DTaskAvailable = NULL;
osEventFlagsId_t s_evt2DResourceAvailable = NULL;

static ARM_NOINIT arm_2d_helper_pfb_t s_tExamplePFB;

/*============================ IMPLEMENTATION ================================*/


void display_task(void) 
{  
    /*! define dirty regions */
    IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static const)
        
        /* a region for the busy wheel */
        ADD_REGION_TO_LIST(s_tDirtyRegions,
            .tLocation = {(APP_SCREEN_WIDTH - 80) / 2,
                          (APP_SCREEN_HEIGHT - 80) / 2},
            .tSize = {
                .iWidth = 80,
                .iHeight = 80,  
            },
        ),
        
        /* a region for the status bar on the bottom of the screen */
        ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
            .tLocation = {0,APP_SCREEN_HEIGHT - 8*2},
            .tSize = {
                .iWidth = APP_SCREEN_WIDTH,
                .iHeight = 8*2,  
            },
        ),

    END_IMPL_ARM_2D_REGION_LIST()
            

/*! define the partial-flushing area */

    example_gui_do_events();

    //! call partial framebuffer helper service
    while(arm_fsm_rt_cpl != arm_2d_helper_pfb_task( 
                                &s_tExamplePFB, 
                                (arm_2d_region_list_item_t *)s_tDirtyRegions));
    
    //! update performance info
    do {
        
        int32_t nTotalCyclCount = s_tExamplePFB.Statistics.nTotalCycle;
        int32_t nTotalLCDCycCount = s_tExamplePFB.Statistics.nRenderingCycle;
        int64_t lTemp = clock();
        static int64_t s_tLast = 0;
        int32_t nElapsed = lTemp - s_tLast;
        s_tLast = lTemp;

        snprintf(s_chPerformanceInfo, 
                 sizeof(s_chPerformanceInfo),
                "UPS %d\tCPU Usage %2.1f%% (Including LCD Latency %2dms)", 
                (int32_t)SystemCoreClock / nElapsed, 
                (float)nTotalCyclCount / (float)nElapsed * 100.0f,
                nTotalLCDCycCount / ((int32_t)SystemCoreClock / 1000));

    } while(0);

}        

__OVERRIDE_WEAK 
void example_gui_on_refresh_evt_handler(const arm_2d_tile_t *ptFrameBuffer)
{
    ARM_2D_UNUSED(ptFrameBuffer);
    
    //! print performance info
    lcd_text_location( GLCD_HEIGHT / 8 - 2, 0);
    
    lcd_printf( "Screeen: " STR(APP_SCREEN_WIDTH) "*"
                STR(APP_SCREEN_HEIGHT) 
                " PFB: " STR(PFB_BLOCK_WIDTH) "*"
                STR(PFB_BLOCK_HEIGHT)
                " System Freq: %dMHz\r\n",
                (int32_t)SystemCoreClock / 1000000);
                
    //lcd_text_location( 0, 0);
    lcd_puts(s_chPerformanceInfo);
}


__OVERRIDE_WEAK 
void arm_2d_helper_perf_counter_start(void)
{
    start_cycle_counter();
}

__OVERRIDE_WEAK 
int32_t arm_2d_helper_perf_counter_stop(void)
{
    return stop_cycle_counter();
}


static 
IMPL_PFB_ON_DRAW(__pfb_draw_handler)
{
    ARM_2D_UNUSED(pTarget);
    
    example_gui_refresh(ptTile, bIsNewFrame);
        
    arm_2d_op_wait_async(NULL);
    return arm_fsm_rt_cpl;
}

static 
IMPL_PFB_ON_DRAW(__pfb_draw_background_handler)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    arm_2d_rgb16_fill_colour(ptTile, NULL, GLCD_COLOR_BLACK);
    arm_2d_op_wait_async(NULL);
    return arm_fsm_rt_cpl;
}

static 
IMPL_PFB_ON_LOW_LV_RENDERING(__pfb_render_handler)
{
    const arm_2d_tile_t *ptTile = &(ptPFB->tTile);

    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    GLCD_DrawBitmap(ptTile->tRegion.tLocation.iX,
                    ptTile->tRegion.tLocation.iY,
                    ptTile->tRegion.tSize.iWidth,
                    ptTile->tRegion.tSize.iHeight,
                    ptTile->pchBuffer);
                    
    arm_2d_helper_pfb_report_rendering_complete(&s_tExamplePFB, 
                                                (arm_2d_pfb_t *)ptPFB);
}


/*----------------------------------------------------------------------------*
 * RTOS Port                                                                  *
 *----------------------------------------------------------------------------*/

static volatile bool s_bWaitForOPCPL = false;

__OVERRIDE_WEAK
void arm_2d_notif_aync_op_cpl(uintptr_t pUserParam)
{
    osEventFlagsId_t evtFlag = (osEventFlagsId_t)pUserParam;
    assert (NULL != evtFlag) ;
    osEventFlagsSet(evtFlag, 0x01); 
}

__OVERRIDE_WEAK
bool arm_2d_port_wait_for_async(uintptr_t pUserParam)
{
    osEventFlagsId_t evtFlag = (osEventFlagsId_t)pUserParam;
    assert (NULL != evtFlag) ;

    osEventFlagsWait(evtFlag, 0x01, osFlagsWaitAny, osWaitForever );
    return true;
}


__OVERRIDE_WEAK
void arm_2d_notif_new_op_arrive(uintptr_t pUserParam)
{
    ARM_2D_UNUSED(pUserParam);
    
    assert (NULL != s_evt2DTaskAvailable) ;
    
    osEventFlagsSet(s_evt2DTaskAvailable, 0x01); 
}


__OVERRIDE_WEAK 
void arm_2d_notif_aync_sub_task_cpl(uintptr_t pUserParam)
{
    ARM_2D_UNUSED(pUserParam);
    assert (NULL != s_evt2DResourceAvailable) ;
    
    osEventFlagsSet(s_evt2DResourceAvailable, 0x01); 
}

/*----------------------------------------------------------------------------*
 * Application main thread                                                    *
 *----------------------------------------------------------------------------*/
 
 __NO_RETURN
void app_main (void *argument) 
{
    ARM_2D_UNUSED(argument);
    //! draw background first
    while(arm_fsm_rt_cpl != arm_2d_helper_pfb_task(&s_tExamplePFB,NULL));
    
    //! update draw function
    ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(   &s_tExamplePFB, 
                                                &__pfb_draw_handler);

    while(1) {
        //! retrieve the number of system ticks
        uint32_t wTick = osKernelGetTickCount();        
        display_task();
        
        //! lock frame rate
        osDelayUntil(wTick + (1000 / APP_TARGET_FPS));
    }

    //osThreadExit();
}

__NO_RETURN
void arm_2d_thread(void *argument)
{
    ARM_2D_UNUSED(argument);

    arm_fsm_rt_t tTaskResult;
    arm_2d_task_t tTaskCB = {0};
    
    do {
        tTaskResult = arm_2d_task(&tTaskCB);

        if (tTaskResult < 0) {
            //! a serious error is detected
            assert(false);
            break;
        }/* else if (arm_fsm_rt_wait_for_obj == tTaskResult) {
            //! user low level drivers want to sync-up with this thread
        } */
        else if (arm_fsm_rt_wait_for_res == tTaskResult) {
            /* wait for on-going OP releasing resources */
            assert (NULL != s_evt2DResourceAvailable) ;
            /* block current thread */
            osEventFlagsWait(   s_evt2DResourceAvailable, 
                                0x01, 
                                osFlagsWaitAny, 
                                osWaitForever );
        } else if (arm_fsm_rt_cpl == tTaskResult) {
            assert (NULL != s_evt2DTaskAvailable) ;
            
            /* block current thread */
            osEventFlagsWait(   s_evt2DTaskAvailable, 
                                0x01, 
                                osFlagsWaitAny, 
                                osWaitForever );
        }
        
    } while(true);
    
    osThreadExit();
}


/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) 
{
    arm_irq_safe {
        arm_2d_init();
        /* put your code here */
        example_gui_init();
    }     
    
    printf("\r\nArm-2D RTOS2 Template\r\n");

    //! initialise FPB helper
    if (ARM_2D_HELPER_PFB_INIT( 
            &s_tExamplePFB,                 //!< FPB Helper object
            APP_SCREEN_WIDTH,               //!< screen width
            APP_SCREEN_HEIGHT,              //!< screen height
            uint16_t,                       //!< colour date type
            PFB_BLOCK_WIDTH,                //!< PFB block width
            PFB_BLOCK_HEIGHT,               //!< PFB block height
            1,                              //!< number of PFB in the PFB pool
            {
                .evtOnLowLevelRendering = {
                    //! callback for low level rendering 
                    .fnHandler = &__pfb_render_handler,                         
                },
                .evtOnDrawing = {
                    //! callback for drawing GUI 
                    .fnHandler = &__pfb_draw_background_handler, 
                },
            }
        ) < 0) {
        //! error detected
        assert(false);
    }
    
    /* Initialize CMSIS-RTOS2 */
    osKernelInitialize ();

    /* Create application main thread */
    osThreadNew(app_main, NULL, NULL);
    
    osThreadNew(arm_2d_thread, NULL, NULL);
    
    s_evt2DTaskAvailable = osEventFlagsNew(NULL);
    s_evt2DResourceAvailable = osEventFlagsNew(NULL);
    
    assert(NULL != s_evt2DTaskAvailable );

    /*! \note create a event flag and attach it to the default OP */

    arm_2d_set_user_param(NULL, (uintptr_t)(osEventFlagsNew(NULL)));

    /* Start thread execution */
    osKernelStart();

    while (1) {
    }
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


