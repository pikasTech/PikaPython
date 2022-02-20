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
#include "pikaScript.h"
#include "PikaParser.h"
#include "PikaVM.h"
#include "dataStrs.h"
#include <stdio.h>

#include "RTE_Components.h"
#if defined(RTE_Compiler_EventRecorder) && defined(USE_EVR_FOR_STDOUR)
#   include <EventRecorder.h>
#endif

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
    extern void SystemCoreClockUpdate();

    SystemCoreClockUpdate();
    /*! \note if you do want to use SysTick in your application, please use 
     *!       init_cycle_counter(true); 
     *!       instead of 
     *!       init_cycle_counter(false); 
     */
    init_cycle_counter(false);

#if defined(RTE_Compiler_EventRecorder) && defined(USE_EVR_FOR_STDOUR)
    EventRecorderInitialize(0, 1);
#endif

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    
}

static void prime_number_100_c(void) {
    volatile int num = 0;
    /* run */
    for (volatile int i = 2; i < 100; i++) {
        volatile int is_prime = 1;
        for (volatile int j = 2; j < i; j++) {
            if (i % j == 0) {
                is_prime = 0;
                break;
            }
        }
        if (is_prime) {
            num = num + i;
        }
    }
    if ( num != 1060){
        printf("[error] in benchmark prime_number_100_c \r\n");
    }
}

int main(void) 
{
    system_init();

    printf("Hello Pico-Template\r\n");
    uint32_t n = 0;
    /* run unit test */
    obj_deinit(pikaScriptInit());

    /* benchmark */
    uint64_t nCycleUsed_c,nCycleUsed_pika = 0;
    printf("[----------benchmark----------]\r\n");
    __cycleof__("", {
            nCycleUsed_c = _;
            printf("[prime_number_100_c] Cycle Used %lld\r\n", _);

        }) {
        prime_number_100_c();
    }
        
    /* create pikaMain root obj */
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    /* parse python to pikaAsm */
    Args* buffs = New_strBuff();
    
    char *pikaAsm = Parser_multiLineToAsm(buffs, (char *)
            "num = 0\n"
            "i = 2\n"
            "for i in range(2,100):\n"
            "    j=2\n"
            "    is_prime = 1\n"
            "    for j in range(2,i):\n"
            "        if i%j==0 :\n"
            "            is_prime = 0\n"
            "            break\n"
            "    if is_prime:\n"
            "        num = num + i\n"
            "\n");
     __cycleof__("", {
            nCycleUsed_pika = _;
            printf("[prime_number_100_pika] Cycle Used %lld\r\n", _);
        }) {
        /* run pika Asm */
        pikaVM_runAsm(pikaMain, pikaAsm);
    }
    int num = obj_getInt(pikaMain, "num");
    if ( num != 1060){
        printf("[error] in benchmark prime_number_100_pika \r\n");
    }
    /* free the pikaMain obj */
    obj_deinit(pikaMain);
    
    uint64_t benchmark_result = ((double) nCycleUsed_c / (double)nCycleUsed_pika) * 100 * 100000;
    
    printf("\r\n[------benchmark finished ---------]\r\n");
    printf("benchmakr result :%lld\r\n", benchmark_result);

    while (true) {
        breath_led();
        //gpio_put(PICO_DEFAULT_LED_PIN, 1);
        //sleep_ms(500);
        //gpio_put(PICO_DEFAULT_LED_PIN, 0);
        //sleep_ms(500);
    }
    //return 0;
}
