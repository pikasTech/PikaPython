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
#define __ARM_2D_IMPL__

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "arm_2d_helper.h"


#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#   pragma clang diagnostic ignored "-Wundef"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wconditional-uninitialized"
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

/*============================ MACROS ========================================*/

#undef  this
#define this            (*ptThis)

/*----------------------------------------------------------------------------*
 * PT Operations                                                              *
 *----------------------------------------------------------------------------*/
/*
Protothreads open source BSD-style license
The protothreads library is released under an open source license that allows 
both commercial and non-commercial use without restrictions. The only 
requirement is that credits is given in the source code and in the documentation 
for your product.

The full license text follows.

Copyright (c) 2004-2005, Swedish Institute of Computer Science.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. Neither the name of the Institute nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS `AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

Author: Adam Dunkels
*/

#define ARM_PT_BEGIN(__STATE)                                                   \
            enum {                                                              \
                count_offset = __COUNTER__ + 1,                                 \
            };                                                                  \
            switch (__STATE) {                                                  \
                case __COUNTER__ - count_offset: 

#define ARM_PT_ENTRY(__STATE, ...)                                              \
            (__STATE) = (__COUNTER__ - count_offset + 1) >> 1;                  \
            __VA_ARGS__                                                         \
            case (__COUNTER__ - count_offset) >> 1: (__STATE) = (__STATE);
            
#define ARM_PT_YIELD(__STATE)                                                   \
            ARM_PT_ENTRY(__STATE, return arm_fsm_rt_on_going;)
            
#define ARM_PT_END(__STATE)                                                     \
            __STATE = 0;                                                        \
            break;}

#define ARM_PT_GOTO_PREV_ENTRY()    return arm_fsm_rt_on_going;

            
#define ARM_PT_REPORT_STATUS(__STATE, __VAL)                                    \
            ARM_PT_ENTRY(__STATE,                                               \
                return (arm_fsm_rt_t)(__VAL);                                   \
            )
            
#define ARM_PT_RETURN(__STATE, __VAL)                                           \
            __STATE = 0;                                                        \
            return (arm_fsm_rt_t)(__VAL);

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
arm_2d_err_t arm_2d_helper_pfb_init(arm_2d_helper_pfb_t *ptThis, 
                            arm_2d_helper_pfb_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    
    memset(ptThis, 0, sizeof(this));
    this.tCFG = *ptCFG;
    
    if (    (NULL == this.tCFG.Dependency.evtOnDrawing.fnHandler)
       ||   (NULL == this.tCFG.Dependency.evtOnLowLevelRendering.fnHandler)) {
        return ARM_2D_ERR_MISSING_PARAM;
    }
    
    //! perform validation
    do {
        int_fast16_t n = this.tCFG.FrameBuffer.hwPFBNum;
        arm_2d_pfb_t *ptItem = this.tCFG.FrameBuffer.ptPFBs;
        uint32_t wBufferSize = this.tCFG.FrameBuffer.wBufferSize;
        
        //! handle alignments
        wBufferSize += __alignof__(arm_2d_pfb_t) - 1;
        wBufferSize &= ~(__alignof__(arm_2d_pfb_t) - 1);
        
        if (0 == n || NULL == ptItem) {
            return ARM_2D_ERR_MISSING_PARAM;
        } else if ( (0 == this.tCFG.FrameBuffer.tFrameSize.iHeight)
                 || (0 == this.tCFG.FrameBuffer.tFrameSize.iWidth)
                 || (0 == this.tCFG.FrameBuffer.wBufferSize)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        
        //! add PFBs to pool
        do {
            ptItem->tTile = (arm_2d_tile_t) {
                .tRegion = {
                    .tSize = this.tCFG.FrameBuffer.tFrameSize,
                },
                .tInfo.bIsRoot = true,
                .pchBuffer = (uint8_t *)((uintptr_t)ptItem + sizeof(arm_2d_pfb_t)),
            };
            ARM_LIST_STACK_PUSH(this.Adapter.ptFreeList, ptItem);
            
            //! update pointer
            ptItem = (arm_2d_pfb_t *)(  (uintptr_t)ptItem 
                                     +  wBufferSize
                                     +  sizeof(arm_2d_pfb_t));
        } while(--n);
    
    } while(0);

    this.Adapter.bFirstIteration = true;
    this.Adapter.bIsFlushRequested = true;
    
    return ARM_2D_ERR_NONE;
}


static 
void __arm_2d_helper_swap_rgb16(uint16_t *phwBuffer, uint32_t wSize)
{
    if (0 == wSize) {
        return ;
    }
    
    //! aligned (4)
    assert((((uintptr_t) phwBuffer) & 0x03) == 0);
    
    uint32_t wWords = wSize >> 1;
    uint32_t *pwBuffer = (uint32_t *)phwBuffer;
    wSize &= 0x01;
    
    if (wWords > 0) {
        do {
            uint32_t wTemp = *pwBuffer;
            *pwBuffer++ = __REV16(wTemp);
        } while(--wWords);
    }
    
    if (wSize) {
        uint32_t wTemp = *pwBuffer;
        (*(uint16_t *)pwBuffer) = (uint16_t)__REV16(wTemp);
    }
    
    
}

static 
void __arm_2d_helper_flush_pfb(arm_2d_helper_pfb_t *ptThis)
{
    arm_2d_pfb_t *ptPFB = NULL;
    
    arm_irq_safe {
        ARM_LIST_QUEUE_DEQUEUE( this.Adapter.FlushFIFO.ptHead, 
                                this.Adapter.FlushFIFO.ptTail, 
                                ptPFB);
        this.Adapter.bIsFlushRequested = (NULL == ptPFB);
    }

    if (NULL != ptPFB) {
        //! call handler
        (*this.tCFG.Dependency.evtOnLowLevelRendering.fnHandler)(
                        this.tCFG.Dependency.evtOnLowLevelRendering.pTarget,
                        ptPFB,
                        ptPFB->bIsNewFrame);
    }
}

static 
void __arm_2d_helper_enqueue_pfb(arm_2d_helper_pfb_t *ptThis)
{
    this.Adapter.ptCurrent->bIsNewFrame = this.Adapter.bFirstIteration;
    bool bIsFlushRequested;
    
    arm_irq_safe {
        bIsFlushRequested = this.Adapter.bIsFlushRequested;
        ARM_LIST_QUEUE_ENQUEUE( this.Adapter.FlushFIFO.ptHead, 
                                this.Adapter.FlushFIFO.ptTail, 
                                this.Adapter.ptCurrent);
    }
    
    if (bIsFlushRequested) {
        __arm_2d_helper_flush_pfb(ptThis);
    }
    
}


static
void __arm_2d_helper_low_level_rendering(arm_2d_helper_pfb_t *ptThis)
{

    assert(NULL != this.tCFG.Dependency.evtOnLowLevelRendering.fnHandler);
    assert(NULL != this.Adapter.ptCurrent);
    
    //! update location info
    this.Adapter.ptCurrent->tTile.tRegion.tLocation = (arm_2d_location_t) {
        .iX = this.Adapter.tDrawRegion.tLocation.iX
            + this.Adapter.tTargetRegion.tLocation.iX,
        .iY = this.Adapter.tDrawRegion.tLocation.iY
            + this.Adapter.tTargetRegion.tLocation.iY,
    };

    if (this.tCFG.FrameBuffer.bSwapRGB16) {
        __arm_2d_helper_swap_rgb16( this.Adapter.ptCurrent->tTile.phwBuffer, 
                                    get_tile_buffer_pixel_count(
                                        this.Adapter.ptCurrent->tTile));
    }

    __arm_2d_helper_enqueue_pfb(ptThis);

    this.Adapter.bFirstIteration = false;

}


static bool __arm_2d_helper_pfb_get_next_dirty_region(arm_2d_helper_pfb_t *ptThis)
{
    if (NULL == this.Adapter.ptDirtyRegion) {
        //! no dirty region list
        this.Adapter.bFirstIteration = true;
        
        return false;
    } 
    
    this.Adapter.ptDirtyRegion = this.Adapter.ptDirtyRegion->ptNext;
    
    if (NULL == this.Adapter.ptDirtyRegion) {
        //! reach last item in a chain
        this.Adapter.bFirstIteration = true;
        
        return false;
    } else {
        this.Adapter.bIsRegionChanged = true;
    }
    
    return true;
}

/*! \brief begin a iteration of drawing and request a frame buffer from 
 *!        low level display driver.
 *! \param ptTargetRegion the address of the target region in the LCD
 *!        passing NULL means we want to draw the whole LCD.
 *! \retval NULL the display driver is not ready
 *! \retval (intptr_t)-1 the display driver want to ignore this drawing 
 *!         (maybe because the target area is out of the LCD)
 *! \retval non-null a tile which contains the (partial) frame buffer
 */
static 
arm_2d_tile_t * __arm_2d_helper_pfb_drawing_iteration_begin(
                                    arm_2d_helper_pfb_t *ptThis,
                                    arm_2d_region_list_item_t *ptDirtyRegions)
{ 
    arm_irq_safe {
        ARM_LIST_STACK_POP(this.Adapter.ptFreeList, this.Adapter.ptCurrent);
    }
    
    if (NULL == this.Adapter.ptCurrent) {
        //! no resource left
        return NULL;
    }
    arm_2d_tile_t *ptPartialFrameBuffer = &(this.Adapter.ptCurrent->tTile);
    
    
    
    if (this.Adapter.bFirstIteration) {
        this.Adapter.ptDirtyRegion = ptDirtyRegions;
        //this.Adapter.bFirstIteration = false;
        this.Adapter.bIsRegionChanged = true;
    }
    
    
    do {
        if (this.Adapter.bIsRegionChanged) {
        
            this.Adapter.bIsRegionChanged = false;

            if (NULL != this.Adapter.ptDirtyRegion) { 
                //! calculate the valid region
                if (!arm_2d_region_intersect(   &this.tCFG.tDisplayArea, 
                                                &(this.Adapter.ptDirtyRegion->tRegion), 
                                                &this.Adapter.tTargetRegion)) {
                                                
                    if (__arm_2d_helper_pfb_get_next_dirty_region(ptThis)) {
                        //! try next region
                        continue;
                    }
                    //! out of lcd 
                    return (arm_2d_tile_t *)-1;
                }
            } else {
                this.Adapter.tTargetRegion = this.tCFG.tDisplayArea;
            }

        #if __ARM_ARCH == 6 || __TARGET_ARCH_THUMB == 3
            //! reset adapter frame size
            this.Adapter.tFrameSize = this.tCFG.FrameBuffer.tFrameSize;
        #else
            if (this.tCFG.FrameBuffer.bDisableDynamicFPBSize) {
                //! reset adapter frame size
                this.Adapter.tFrameSize = this.tCFG.FrameBuffer.tFrameSize;

            } else {                                                            //!< update PFB size
                uint32_t wTargetPixelCount 
                    = this.Adapter.tTargetRegion.tSize.iWidth
                    * this.Adapter.tTargetRegion.tSize.iHeight;
                
                uint32_t wPFBPixelCount
                    = this.tCFG.FrameBuffer.tFrameSize.iWidth
                    * this.tCFG.FrameBuffer.tFrameSize.iHeight;
                        
                if (    (wTargetPixelCount <= wPFBPixelCount)
                   ||   (   this.Adapter.tTargetRegion.tSize.iWidth 
                        <   this.tCFG.FrameBuffer.tFrameSize.iWidth)) {
                    //! redefine the shape of PFB
                    
                    this.Adapter.tFrameSize.iWidth 
                        = this.Adapter.tTargetRegion.tSize.iWidth;
                        
                    this.Adapter.tFrameSize.iHeight = (int16_t)(
                            wPFBPixelCount 
                        /   (uint32_t)this.Adapter.tTargetRegion.tSize.iWidth);
                
                } else {
                    //! reset adapter frame size
                    this.Adapter.tFrameSize = this.tCFG.FrameBuffer.tFrameSize;
                }
            } 
        #endif
        }
        break;
    } while(true);
    
    
    arm_2d_region_t tTempRegion = {
        .tSize = this.tCFG.tDisplayArea.tSize,
        .tLocation = {
            .iX = - (   this.Adapter.tTargetRegion.tLocation.iX 
                    +   this.Adapter.tDrawRegion.tLocation.iX),
            .iY = - (   this.Adapter.tTargetRegion.tLocation.iY 
                    +   this.Adapter.tDrawRegion.tLocation.iY),
        },
    };
    
    
    ptPartialFrameBuffer->tRegion.tSize.iWidth 
        = MIN(  this.Adapter.tFrameSize.iWidth, 
                this.Adapter.tTargetRegion.tSize.iWidth 
            -   this.Adapter.tDrawRegion.tLocation.iX);
    ptPartialFrameBuffer->tRegion.tSize.iHeight 
        = MIN(  this.Adapter.tFrameSize.iHeight, 
                this.Adapter.tTargetRegion.tSize.iHeight 
            -   this.Adapter.tDrawRegion.tLocation.iY);
            
    arm_2d_tile_generate_child( ptPartialFrameBuffer, 
                                &tTempRegion, 
                                &this.Adapter.tPFBTile, 
                                false);


    if (!this.tCFG.FrameBuffer.bDoNOTUpdateDefaultFrameBuffer) {
        //! update default frame buffer
        arm_2d_set_default_frame_buffer(&this.Adapter.tPFBTile);
    }
    
    //! uncomment this when necessary for debug purpose
    //arm_2d_rgb16_fill_colour(&this.Adapter.tPFBTile, NULL, 0);

    return (arm_2d_tile_t *)&(this.Adapter.tPFBTile);
}



/*! \brief end a drawing iteration and decide wether a new iteration is required
 *!        or not based on the return value
 *! \param none
 *! \retval true a new iteration is required
 *! \retval false no more iteration is required
 */
static 
bool __arm_2d_helper_pfb_drawing_iteration_end(arm_2d_helper_pfb_t *ptThis)
{
    __arm_2d_helper_low_level_rendering(ptThis);
    
    arm_2d_tile_t *ptPartialFrameBuffer = &(this.Adapter.ptCurrent->tTile);
    
    if (!this.tCFG.FrameBuffer.bDoNOTUpdateDefaultFrameBuffer) {
        //! update default frame buffer
        arm_2d_set_default_frame_buffer(NULL);
    }

    this.Adapter.tDrawRegion.tLocation.iX 
        += ptPartialFrameBuffer->tRegion.tSize.iWidth;
    if (    this.Adapter.tDrawRegion.tLocation.iX 
        >=  this.Adapter.tTargetRegion.tSize.iWidth) {
        this.Adapter.tDrawRegion.tLocation.iY 
            += ptPartialFrameBuffer->tRegion.tSize.iHeight;
        this.Adapter.tDrawRegion.tLocation.iX = 0;
        
        if (    this.Adapter.tDrawRegion.tLocation.iY 
            >=  this.Adapter.tTargetRegion.tSize.iHeight) {
            //! finished
            this.Adapter.tDrawRegion.tLocation.iY = 0;

            return __arm_2d_helper_pfb_get_next_dirty_region(ptThis);
            
        }
    }

    return true;
}


__WEAK void arm_2d_helper_perf_counter_start(void)
{
}

__WEAK int32_t arm_2d_helper_perf_counter_stop(void)
{
    return 0;
}



ARM_NONNULL(1,2)
void arm_2d_helper_pfb_report_rendering_complete(arm_2d_helper_pfb_t *ptThis,
                                                 arm_2d_pfb_t *ptPFB)
{
    assert(NULL != ptThis);
    assert(NULL != ptPFB);
    
    ptPFB->tTile.tRegion.tLocation = (arm_2d_location_t) {0,0};
    
    arm_irq_safe {
        ARM_LIST_STACK_PUSH(this.Adapter.ptFreeList, ptPFB);
    }
    
    __arm_2d_helper_flush_pfb(ptThis);
}


ARM_NONNULL(1,3)
arm_2d_err_t arm_2d_helper_pfb_update_dependency(
                            arm_2d_helper_pfb_t *ptThis, 
                            uint_fast8_t chMask,
                            const arm_2d_helper_pfb_dependency_t *ptDependency)
{
    assert(NULL != ptThis);
    assert(NULL != ptDependency);
    
    arm_irq_safe {
        if (chMask & ARM_2D_PFB_DEPEND_ON_LOW_LEVEL_RENDERING) {
            this.tCFG.Dependency.evtOnLowLevelRendering 
                = ptDependency->evtOnLowLevelRendering;
        }
        
        if (chMask & ARM_2D_PFB_DEPEND_ON_DRAWING) {
            this.tCFG.Dependency.evtOnDrawing 
                = ptDependency->evtOnDrawing;
        }
        
        if (chMask & ARM_2D_PFB_DEPEND_ON_LOW_LEVEL_SYNC_UP) {
            this.tCFG.Dependency.evtOnLowLevelSyncUp 
                = ptDependency->evtOnLowLevelSyncUp;
        }
    }
    
    return ARM_2D_ERR_NONE;
}


arm_fsm_rt_t arm_2d_helper_pfb_task(arm_2d_helper_pfb_t *ptThis, 
                                    arm_2d_region_list_item_t *ptDirtyRegions) 
{   
    assert(NULL != ptThis);
    assert(NULL != this.tCFG.Dependency.evtOnDrawing.fnHandler);
    arm_fsm_rt_t tResult;

ARM_PT_BEGIN(this.Adapter.chPT)

    this.Statistics.nTotalCycle = 0;
    this.Statistics.nRenderingCycle = 0;
    this.Adapter.bIsNewFrame = true;
    arm_2d_helper_perf_counter_start(); 
    do {
        this.Statistics.nRenderingCycle += arm_2d_helper_perf_counter_stop(); 

        /*! begin of the drawing iteration, 
         *! try to request the tile of frame buffer
         */
        
        do {
        
            /*! \note In deep embedded applications, a LCD usually is connected 
             *!       via a serial interface to save pins, hence the bandwidth 
             *!       is limited and the FPS is low due to the bandwidth.
             *!       To overcome this issue, some partial-flushing schemes are 
             *!       used, such as:
             *!       - Dirty Region based partial-flushing
             *!       - Flush the known and fixed small area that is updated 
             *!         frequently based on the application scenarios. 
             *!       
             *!       It is worth emphasizing that as we are using partial 
             *!       flushing scheme, which means for a given frame, we only 
             *!       update those changed area(s) but not the complete frame,
             *!       using the term frame per sec (FPS) might confuse people,
             *!       hence, we decide to introduce a NEW term called update per
             *!       sec (UPS) to avoid this confusion. It reflects what people
             *!       feel when looking at the LCD but not necessarily means
             *!       the rate that a complete frame is flushed into LCD.  
             *!       
             *!       In Arm-2D:
             *!       - FPS is a sub-set of UPS. 
             *!       - UPS forcus on how people feel and FPS is sticks to the 
             *!         concept of (full) frame per sec.              
             */
        
            //! request to draw the whole LCD
            this.Adapter.ptFrameBuffer = 
                __arm_2d_helper_pfb_drawing_iteration_begin( 
                                                        ptThis, 
                                                        ptDirtyRegions);
            if (NULL == this.Adapter.ptFrameBuffer) {
                if (NULL != this.tCFG.Dependency.evtOnLowLevelSyncUp.fnHandler){
                     //!wait until lcd is ready
                    (*this.tCFG.Dependency.evtOnLowLevelSyncUp.fnHandler)(
                        this.tCFG.Dependency.evtOnLowLevelSyncUp.pTarget
                    );
                }
                continue;
            } else if (-1 == (intptr_t)this.Adapter.ptFrameBuffer) {
                /* display driver wants to end the drawing */
                return arm_fsm_rt_cpl;
            }
        } while(NULL == this.Adapter.ptFrameBuffer);

    ARM_PT_ENTRY(this.Adapter.chPT)
        
        arm_2d_helper_perf_counter_start(); 
        //! draw all the gui elements on target frame buffer
        tResult = this.tCFG.Dependency.evtOnDrawing.fnHandler(
                                        this.tCFG.Dependency.evtOnDrawing.pTarget,
                                        this.Adapter.ptFrameBuffer,
                                        this.Adapter.bIsNewFrame);
        //! just in case some one forgot to do this...
        arm_2d_op_wait_async(NULL);
        
        this.Adapter.bIsNewFrame = false;
        this.Statistics.nTotalCycle += arm_2d_helper_perf_counter_stop();                                 

        if (arm_fsm_rt_on_going == tResult) {
    ARM_PT_GOTO_PREV_ENTRY()
        } else if (tResult < 0) {
            //! error was reported
    ARM_PT_RETURN(this.Adapter.chPT, tResult)
        } else if (arm_fsm_rt_wait_for_obj == tResult) {
    ARM_PT_REPORT_STATUS(this.Adapter.chPT, tResult)
        } else { 
    ARM_PT_YIELD(this.Adapter.chPT)
        }
        
        arm_2d_helper_perf_counter_start(); 
    } while(__arm_2d_helper_pfb_drawing_iteration_end(ptThis));
    
    this.Statistics.nRenderingCycle += arm_2d_helper_perf_counter_stop();
ARM_PT_END(this.Adapter.chPT)
    
    return arm_fsm_rt_cpl;
}       

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif
