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

#if defined(__PICO_USE_LCD_1IN3__) && __PICO_USE_LCD_1IN3__
#include "DEV_Config.h"
#include "LCD_1In3.h"
#include "GLCD_Config.h"
#endif

#include <stdio.h>

#include "RTE_Components.h"
#if defined(RTE_Compiler_EventRecorder) && defined(RTE_Compiler_IO_STDOUT_EVR)
#   include <EventRecorder.h>
#endif

#if defined(RTE_Script_PikaScript) || defined(USING_PIKAPYTHON)
#   include "pikaScript.h"
#endif

#if defined(__RTE_ACCELERATION_ARM_2D__) || defined(RTE_Acceleration_Arm_2D)
#   include "arm_2d.h"
#   include "arm_2d_helper.h"
#   include "arm_2d_disp_adapters.h"
#   include "arm_2d_scenes.h"
#endif

#if     defined(__RTE_ACCELERATION_ARM_2D_EXTRA_BENCHMARK_WATCH_PANEL__)            \
    ||  defined(__RTE_ACCELERATION_ARM_2D_EXTRA_BENCHMARK_GENERIC__)
#   include "arm_2d_benchmark.h"
#endif
/*============================ MACROS ========================================*/
#define TOP         (0x1FFF)

/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ABS
#   define ABS(__N)    ((__N) < 0 ? -(__N) : (__N))
#endif
#ifndef _BV
#   define _BV(__N)    ((uint32_t)1<<(__N))
#endif


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
    extern void SystemCoreClockUpdate();

    SystemCoreClockUpdate();
    /*! \note if you do want to use SysTick in your application, please use 
     *!       init_cycle_counter(true); 
     *!       instead of 
     *!       init_cycle_counter(false); 
     */
    init_cycle_counter(false);

#if defined(RTE_Compiler_EventRecorder) && defined(RTE_Compiler_IO_STDOUT_EVR)
    EventRecorderInitialize(0, 1);
#endif
    stdio_init_all();
    
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

#if defined(__PICO_USE_LCD_1IN3__) && __PICO_USE_LCD_1IN3__
    DEV_Delay_ms(100);

    if(DEV_Module_Init()!=0){
        //assert(0);
    }
   
    DEV_SET_PWM(50);
    /* LCD Init */
    
    LCD_1IN3_Init(HORIZONTAL);
    LCD_1IN3_Clear(GLCD_COLOR_BLUE);
    
    for (int n = 0; n < KEY_NUM; n++) {
        dev_key_init(n);
    }
#endif

    
}


int stdin_getchar(void);
char pika_platform_getchar(void) {
		return stdin_getchar();
}


int main(void) 
{
    system_init();

    __cycleof__("printf") {
        printf("Hello Pico-Template\r\n");
    }
    
#if defined(RTE_Script_PikaScript) || defined(USING_PIKAPYTHON)
    pikaScriptInit();
#endif

#if defined( __PERF_COUNTER_COREMARK__ ) && __PERF_COUNTER_COREMARK__
    printf("\r\nRun Coremark 1.0...\r\n");
    coremark_main();
#endif

#if     defined(__RTE_ACCELERATION_ARM_2D_EXTRA_BENCHMARK_WATCH_PANEL__)            \
    ||  defined(__RTE_ACCELERATION_ARM_2D_EXTRA_BENCHMARK_GENERIC__)
    arm_2d_run_benchmark();
#endif

#if defined(__RTE_ACCELERATION_ARM_2D__) || defined(RTE_Acceleration_Arm_2D)
    arm_2d_init();
    disp_adapter0_init();
    
    //arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
#endif

    while (true) {
        breath_led();

#if defined(__RTE_ACCELERATION_ARM_2D__) || defined(RTE_Acceleration_Arm_2D)
        disp_adapter0_task();
#endif
        //gpio_put(PICO_DEFAULT_LED_PIN, 1);
        //sleep_ms(500);
        //gpio_put(PICO_DEFAULT_LED_PIN, 0);
        //sleep_ms(500);
    }
    //return 0;
}
