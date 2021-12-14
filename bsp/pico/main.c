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
/*============================ INCLUDES ======================================*/
#include "pico/stdlib.h"
#include "perf_counter.h"
#include "pico/stdio_usb.h"
#include <stdio.h>
#include "pikaScript.h"
/*============================ MACROS ========================================*/
#define TOP         (0x1FFF)

/*============================ MACROFIED FUNCTIONS ===========================*/
#define ABS(__N)    ((__N) < 0 ? -(__N) : (__N))
#define _BV(__N)    ((uint32_t)1<<(__N))



/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void SysTick_Handler(void)
{

}

/*! \brief set the 16-level led gradation
 *! \param hwLevel gradation
 *! \return none
 */
static void set_led_gradation(uint16_t hwLevel)
{
    static uint16_t s_hwCounter = 0;
    
    if (hwLevel >= s_hwCounter) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
    } else {
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
    }
    
    s_hwCounter++;
    s_hwCounter &= TOP;
}

static void breath_led(void)
{
    static uint16_t s_hwCounter = 0;
    static int16_t s_nGray = (TOP >> 1);
    
    s_hwCounter++;
    if (!(s_hwCounter & (_BV(10)-1))) {
        s_nGray++; 
        if (s_nGray == TOP) {
            s_nGray = 0;
        }
    }
    
    set_led_gradation(ABS(s_nGray - (TOP >> 1)));
}

static void system_init(void)
{
    /*! \note if you do want to use SysTick in your application, please use 
     *!       init_cycle_counter(true); 
     *!       instead of 
     *!       init_cycle_counter(false); 
     */
    // init_cycle_counter(false);
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

int main(void) 
{
    system_init();
    printf("pico system init ok.\r\n");
    
    printf("------------------------------------------------------------------\r\n");
    printf("|                                                                |\r\n");
    printf("|     ____   _   __            _____              _          __  |\r\n");
    printf("|    / __ \\ (_) / /__ ____ _  / ___/ _____ _____ (_) ____   / /_ |\r\n");
    printf("|   / /_/ // / / //_// __ `/  \\__ \\ / ___// ___// / / __ \\ / __/ |\r\n");
    printf("|  / ____// / / ,<  / /_/ /  ___/ // /__ / /   / / / /_/ // /_   |\r\n");
    printf("| /_/    /_/ /_/|_| \\__,_/  /____/ \\___//_/   /_/ / .___/ \\__/   |\r\n");
    printf("|                                                /_/             |\r\n");
    printf("|          PikaScript - An Ultra Lightweight Python Engine       |\r\n");
    printf("|                                                                |\r\n");
    printf("|           [ https://github.com/pikastech/pikascript ]          |\r\n");
    printf("|           [  https://gitee.com/lyon1998/pikascript  ]          |\r\n");
    printf("|                                                                |\r\n");
    printf("------------------------------------------------------------------\r\n");

    PikaObj * pikaMain = pikaScriptInit();
    while (true) {
        sleep_ms(500);
    }
    //return 0;
}
