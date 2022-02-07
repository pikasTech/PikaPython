/******************************************************************************
 * @file     startup_ARMCM0plus.c
 * @brief    CMSIS-Core(M) Device Startup File for a Cortex-M0+ Device
 * @version  V2.0.3
 * @date     31. March 2020
 ******************************************************************************/
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

#include "RP2040.h"

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;

extern __NO_RETURN void __PROGRAM_START(void);

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler  (void);
            void Default_Handler(void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* Exceptions */
void NMI_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler      (void) __attribute__ ((weak));
void SVC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));


void TIMER_IRQ_0_Handler    (void) __attribute__ ((weak, alias("Default_Handler"))); 
void TIMER_IRQ_1_Handler    (void) __attribute__ ((weak, alias("Default_Handler"))); 
void TIMER_IRQ_2_Handler    (void) __attribute__ ((weak, alias("Default_Handler"))); 
void TIMER_IRQ_3_Handler    (void) __attribute__ ((weak, alias("Default_Handler"))); 
void PWM_IRQ_WRAP_Handler   (void) __attribute__ ((weak, alias("Default_Handler"))); 
void USBCTRL_IRQ_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void XIP_IRQ_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void PIO0_IRQ_0_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void PIO0_IRQ_1_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void PIO1_IRQ_0_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void PIO1_IRQ_1_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA_IRQ_0_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA_IRQ_1_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void IO_IRQ_BANK0_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void IO_IRQ_QSPI_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void SIO_IRQ_PROC0_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void SIO_IRQ_PROC1_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void CLOCKS_IRQ_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI0_IRQ_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQ_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART0_IRQ_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void UART1_IRQ_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC_IRQ_FIFO_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C0_IRQ_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_IRQ_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_IRQ_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

extern const VECTOR_TABLE_Type __VECTOR_TABLE[48];
__attribute__((aligned(256)))
       const VECTOR_TABLE_Type __VECTOR_TABLE[48] __VECTOR_TABLE_ATTRIBUTE = {
  (VECTOR_TABLE_Type)(&__INITIAL_SP),       /*     Initial Stack Pointer */
  Reset_Handler,                            /*     Reset Handler */
  NMI_Handler,                              /* -14 NMI Handler */
  HardFault_Handler,                        /* -13 Hard Fault Handler */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  SVC_Handler,                              /*  -5 SVCall Handler */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  PendSV_Handler,                           /*  -2 PendSV Handler */
  SysTick_Handler,                          /*  -1 SysTick Handler */

  /* Interrupts */
    TIMER_IRQ_0_Handler, 
    TIMER_IRQ_1_Handler, 
    TIMER_IRQ_2_Handler, 
    TIMER_IRQ_3_Handler, 
    PWM_IRQ_WRAP_Handler, 
    USBCTRL_IRQ_Handler,
    XIP_IRQ_Handler,
    PIO0_IRQ_0_Handler,
    PIO0_IRQ_1_Handler,
    PIO1_IRQ_0_Handler,
    PIO1_IRQ_1_Handler,
    DMA_IRQ_0_Handler,
    DMA_IRQ_1_Handler,
    IO_IRQ_BANK0_Handler,
    IO_IRQ_QSPI_Handler,
    SIO_IRQ_PROC0_Handler,
    SIO_IRQ_PROC1_Handler,
    CLOCKS_IRQ_Handler,
    SPI0_IRQ_Handler,
    SPI1_IRQ_Handler,
    UART0_IRQ_Handler,
    UART1_IRQ_Handler,
    ADC_IRQ_FIFO_Handler,
    I2C0_IRQ_Handler,
    I2C1_IRQ_Handler,
    RTC_IRQ_Handler,
    
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
                                            /* Interrupts 10..31 are left out */
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/

__attribute__((constructor(255), used))
void __pico_entry(void)
{
    extern void runtime_init(void);
    runtime_init();
} 

__NO_RETURN void Reset_Handler(void)
{
    SCB->VTOR = (uintptr_t)__VECTOR_TABLE;
    __set_MSP((uintptr_t)(&__INITIAL_SP));
  //SystemInit();                             /* CMSIS System Initialization */
  __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

/*----------------------------------------------------------------------------
  Hard Fault Handler
 *----------------------------------------------------------------------------*/
void HardFault_Handler(void)
{
  while(1);
}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
  while(1);
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#endif

