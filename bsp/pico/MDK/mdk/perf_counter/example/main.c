/****************************************************************************
*  Copyright 2021 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <cmsis_compiler.h>
#include "perf_counter.h"

#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wcompound-token-split-by-macro"
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic ignored "-Wpedantic"
#endif


#ifndef __PERF_CNT_USE_LONG_CLOCK__
#include <time.h>
#else
typedef int64_t clock_t ;
#endif

void systimer_1ms_handler(void)
{
    //printf("Running original Systick_Handler...\r\n");
}

typedef struct example_lv1_t {
    uint32_t wLV1A;
    uint16_t hwLV1B;
    uint8_t  chLV1C;
}example_lv1_t;


typedef struct example_lv0_t {
    
    uint32_t    wA;
    uint16_t    hwB;
    uint8_t     chC;
    uint8_t     chID;
    example_lv1_t tLV1;
} example_lv0_t;

example_lv0_t s_tItem[8] = {
    {.chID = 0},
    {.chID = 1},
    {.chID = 2},
    {.chID = 3},
    {.chID = 4},
    {.chID = 5},
    {.chID = 6},
    {.chID = 7},
};

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) 
{
    
    /*! demo of using() block */
    using(int a = 0,printf("========= On Enter =======\r\n"), 
                    printf("========= On Leave =======\r\n")) {
        printf("\t In Body a=%d \r\n", ++a);
    }    

    __cycleof__("Calibration") {}


    printf("\r\n\r\n\r\n\r\n");

    /*! demo of __cycleof__() operation */
    __cycleof__() {
        foreach(example_lv0_t, s_tItem, ptItem) {
            printf("Processing item with ID = %d\r\n", _->chID);
        }
    }
    
    int32_t iCycleResult = 0;
    
    /* measure cycles and store it in a dedicated variable without printf */
    __cycleof__("delay_us(1000ul)", 
        /* insert code to __cycleof__ body, "{}" can be omitted  */
        {
            iCycleResult = __cycle_count__;   /*< "__cycle_count__" stores the result */
        }) {
        delay_us(1000ul);
    }
    
    printf("\r\n delay_us(1000ul) takes %d cycles\r\n", (int)iCycleResult);

    /*! demo of with block */
    with(example_lv0_t, &s_tItem[0], pitem) {
        _->wA = 1;
        _->hwB = 2;
        _->chC = 3;
        
        with(example_lv1_t, &pitem->tLV1) {
            _->wLV1A = 4;
            _->hwLV1B = 5;
            _->chLV1C = 6;
        }
    }

    //! demo of using clock() in timer.h
    do {
        int64_t tStart = get_system_ticks();
        __IRQ_SAFE {
            printf("no interrupt \r\n");
        }
        printf("used clock cycle: %d", (int32_t)(get_system_ticks() - tStart));
    } while(0);
    
    while (1) {
        printf("\r\nhello world\r\n");
        delay_us(1000000);
    }
}
