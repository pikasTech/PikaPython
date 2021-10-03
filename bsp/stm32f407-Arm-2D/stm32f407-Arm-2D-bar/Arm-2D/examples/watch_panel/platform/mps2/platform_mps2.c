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


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "platform.h"
#include "app_cfg.h"

//#include "Driver_USART.h"

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wformat-nonliteral"
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

__OVERRIDE_WEAK 
bool device_specific_init(void)
{

    //LED_Initialize();                         /* Initializ LEDs                 */
    //Buttons_Initialize();                     /* Initializ Push Buttons         */

    //Touch_Initialize();                       /* Initialize touchscreen         */
    GLCD_Initialize();                          /* Initialize the GLCD            */

    /* display initial screen */
    GLCD_SetFont(&GLCD_Font_6x8);
    GLCD_SetBackgroundColor(GLCD_COLOR_BLACK);
    GLCD_ClearScreen();
    //GLCD_SetBackgroundColor(GLCD_COLOR_BLUE);
    GLCD_SetForegroundColor(GLCD_COLOR_GREEN);
    
#if defined(RTE_Compiler_IO_STDOUT_User)
    void uart_config(uint32_t wUARTFrequency);
    uart_config(25000000ul);
#endif
    return false;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

