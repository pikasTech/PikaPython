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

#ifndef __LCD_PRINTF_H__
#define __LCD_PRINTF_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define __PRINT_BANNER(__STR)                                                   \
        do {                                                                    \
            lcd_text_location(  (GLCD_HEIGHT / 8) / 2 - 1,                      \
                                ((GLCD_WIDTH / 6) - sizeof(__STR)) / 2);        \
            lcd_puts(__STR);                                                    \
        } while(0)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


extern
int lcd_printf(const char *format, ...);

extern
void lcd_puts(const char *str);

extern 
void lcd_text_location(uint8_t chY, uint8_t chX);

#ifdef __cplusplus
}
#endif

#endif
