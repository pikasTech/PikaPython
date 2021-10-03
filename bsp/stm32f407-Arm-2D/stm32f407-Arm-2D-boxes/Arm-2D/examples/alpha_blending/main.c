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

/*============================ INCLUDES ======================================*/
#include <stdio.h>
#include "platform.h"
#include "./display_adapter/display_adapter.h"
#include "example_gui.h"

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#   pragma clang diagnostic ignored "-Wdouble-promotion"
#endif

/*============================ MACROS ========================================*/
#ifndef ITERATION_CNT
#   define ITERATION_CNT       50
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef STR
#   undef __STR
#   define __STR(__STRING)      #__STRING
#   define STR(__STRING)        __STR(__STRING)
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static struct {
    uint32_t wMin;
    uint32_t wMax;
    uint64_t dwTotal;
    uint32_t wAverage;
    uint32_t wIterations;
} BENCHMARK = {
    .wMin = UINT32_MAX,
    .wMax = 0,
    .dwTotal = 0,
    .wAverage = 0,
    .wIterations = ITERATION_CNT,
};

/*============================ IMPLEMENTATION ================================*/


static void display_task(void)
{
    example_gui_do_events();
    
    do {
        arm_2d_tile_t *ptDispBufferTile = platform_disp_buffer_get();
        assert(NULL != ptDispBufferTile);
        
        start_cycle_counter();
        example_gui_refresh(ptDispBufferTile);
        int32_t nCycles = stop_cycle_counter();
        
        if (BENCHMARK.wIterations) {
            BENCHMARK.wMin = MIN((uint32_t)nCycles, BENCHMARK.wMin);
            BENCHMARK.wMax = MAX(nCycles, (int32_t)BENCHMARK.wMax);
            BENCHMARK.dwTotal += nCycles;
            BENCHMARK.wIterations--;
            
            if (0 == BENCHMARK.wIterations) {
                BENCHMARK.wAverage = 
                    (uint32_t)(BENCHMARK.dwTotal / (uint64_t)ITERATION_CNT);
            }
            
        } else {
            static int32_t s_nLCDLatency = 0;
            
            lcd_text_location( GLCD_HEIGHT / 8 - 2, 0);
            lcd_puts(
                "Cycles\tAvrage\tFPS30Freq\tFPS\tLCD Latency");
            lcd_puts("\r\n\t");
            lcd_printf("%d\t", BENCHMARK.wAverage);
            lcd_printf("%4.2f MHz\t", ((float)BENCHMARK.wAverage * 30.0f) / 1000000.0f);
            lcd_printf("%3d:%dms",
                                SystemCoreClock / BENCHMARK.wAverage, 
                                BENCHMARK.wAverage / (SystemCoreClock / 1000ul));
            
            lcd_printf("   %2dms", s_nLCDLatency / (SystemCoreClock / 1000ul) );   
            start_cycle_counter();
            //! send content to LCD
            platform_disp_buffer_refresh();
            s_nLCDLatency = stop_cycle_counter();
            
        }
    } while(0);
}

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void)
{
    arm_irq_safe {
        arm_2d_init();
        /* put your code here */
        platform_disp_init();
        example_gui_init();
    }
    
    __PRINT_BANNER("Arm-2D Benchmark");
    
    lcd_text_location( GLCD_HEIGHT / 8 - 2, 0);
    lcd_puts(  "Alpha-blending Test, running " 
                STR(ITERATION_CNT) 
                " iterations\r\n");

    lcd_puts( "Testing...");
    platform_disp_buffer_refresh();

    while (1) {
        display_task();
    }
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


