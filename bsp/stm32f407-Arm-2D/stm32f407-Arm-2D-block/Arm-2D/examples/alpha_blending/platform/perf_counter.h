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


#ifndef __PERFORMANCE_COUNTER_H__
#define __PERFORMANCE_COUNTER_H__

/*============================ INCLUDES ======================================*/
#include <stdbool.h>
#include <stdint.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/


#ifndef __TPASTE2
#   define __TPASTE2(a, b)                            a##b
#endif

#define __cycleof__(__STR)                                                      \
            for (int32_t nCycles = 0,                                           \
                    __TPASTE2(__cycle_count_s_, __LINE__) = 1;                  \
                 __TPASTE2(__cycle_count_s_, __LINE__)-- ?                      \
                    (start_cycle_counter(),1) :                                 \
                    (                                                           \
                    printf( "\r\n-[Cycle Report]"                               \
                        "--------------------------------------------\r\n"      \
                        __STR                                                   \
                        " total cycle count: %d [%08x]\r\n", nCycles, nCycles)  \
                    ,0);                                                        \
                    nCycles = stop_cycle_counter()                              \
                 )

                    
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void cycle_counter_overflow(void);

extern void platform_delay_ms(uint32_t wMS);

extern void init_cycle_counter(bool bTimerOccupied);

extern bool start_cycle_counter(void);

extern int32_t stop_cycle_counter(void);

extern void delay_us(int32_t iUs);

#endif
