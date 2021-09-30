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

#ifndef __TOP_APP_CFG_H__
#define __TOP_APP_CFG_H__

/*============================ INCLUDES ======================================*/
#include <assert.h>

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#   error This demo requires C11. 
#endif
#if !defined(__GNUC__)
#   error This demo requires GNU extensions
#endif

/*============================ MACROS ========================================*/
#ifndef ITERATION_CNT
#   define ITERATION_CNT       10
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/



#define ASSERT(...)         assert(__VA_ARGS__)


//#define APP_SCREEN_WIDTH    320
//#define APP_SCREEN_HEIGHT   240

#ifndef APP_SCREEN_WIDTH 
#   define APP_SCREEN_WIDTH         GLCD_WIDTH
#endif

#ifndef APP_SCREEN_HEIGHT 
#   define APP_SCREEN_HEIGHT        GLCD_HEIGHT
#endif


/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


#endif
