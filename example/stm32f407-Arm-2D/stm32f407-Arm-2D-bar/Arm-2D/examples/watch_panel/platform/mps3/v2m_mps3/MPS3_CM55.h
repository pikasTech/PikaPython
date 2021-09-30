/*
 * Copyright (c) 2019 Arm Limited. All rights reserved.
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

/*-----------------------------------------------------------------------------
 * File     MPS3_CM55.h
 * Brief    CMSIS Core Peripheral Access Layer Header File for
 *          MPS3_CM55 Device (AN540)
 * Version  1.0.0
 *----------------------------------------------------------------------------*/

#ifndef MPS3_CM55_H
#define MPS3_CM55_H

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
/* -------------------  Processor Exceptions Numbers  ----------------------------- */
  NonMaskableInt_IRQn           =  -14,     /*  2 Non Maskable Interrupt */
  HardFault_IRQn                =  -13,     /*  3 HardFault Interrupt */
  MemoryManagement_IRQn         =  -12,     /*  4 Memory Management Interrupt */
  BusFault_IRQn                 =  -11,     /*  5 Bus Fault Interrupt */
  UsageFault_IRQn               =  -10,     /*  6 Usage Fault Interrupt */
  SecureFault_IRQn              =   -9,     /*  7 Secure Fault Interrupt */
  SVC_IRQn                      =   -5,     /* 11 SV Call Interrupt */
  DebugMonitor_IRQn             =   -4,     /* 12 Debug Monitor Interrupt */
  PendSV_IRQn                   =   -2,     /* 14 Pend SV Interrupt */
  SysTick_IRQn                  =   -1,     /* 15 System Tick Interrupt */

/* ----------------------  Processor Interrupt Numbers  --------------------------- */
  NONSEC_WATCHDOG_RESET_IRQn    =    0,     /*    Non-Secure Watchdog Reset Interrupt */
  NONSEC_WATCHDOG_IRQn          =    1,     /*    Non-Secure Watchdog Interrupt */
  TIMER0_IRQn                   =    3,     /*    TIMER 0 Interrupt */
  TIMER1_IRQn                   =    4,     /*    TIMER 1 Interrupt */
  DUALTIMER_IRQn                =    5,     /*    Dual Timer Interrupt */
  MPC_IRQn                      =    9,     /*    MPC Combined (Secure) Interrupt */
  PPC_IRQn                      =   10,     /*    PPC Combined (Secure) Interrupt */
  MSC_IRQn                      =   11,     /*    MSC Combined (Secure) Interrput */
  BRIDGE_ERROR_IRQn             =   12,     /*    Bridge Error Combined (Secure) Interrupt */

  FPIDC_IRQn                    =   27,     /*    Masked floating-point divide-by-zero exception */
  FPDZC_IRQn                    =   28,     /*    Masked floating-point underflow exception */
  FPOFC_IRQn                    =   29,     /*    Masked floating-point overflow exception */
  FPUFC_IRQn                    =   30,     /*    Masked floating-point input denormal exception */
  FPIXC_IRQn                    =   31,     /*    Masked floating-point inexact exception */

  UART0RX_IRQn                  =   32,     /*    UART 0 Receive Interrupt */
  UART0TX_IRQn                  =   33,     /*    UART 0 Transmit Interrupt */
  UART1RX_IRQn                  =   34,     /*    UART 1 Receive Interrupt */
  UART1TX_IRQn                  =   35,     /*    UART 1 Transmit Interrupt */
  UART2RX_IRQn                  =   36,     /*    UART 2 Receive Interrupt */
  UART2TX_IRQn                  =   37,     /*    UART 2 Transmit Interrupt */
  UART3RX_IRQn                  =   38,     /*    UART 3 Receive Interrupt */
  UART3TX_IRQn                  =   39,     /*    UART 3 Transmit Interrupt */
  UART4RX_IRQn                  =   40,     /*    UART 4 Receive Interrupt */
  UART4TX_IRQn                  =   41,     /*    UART 4 Transmit Interrupt */
  UART0_IRQn                    =   42,     /*    UART 0 Combined Interrupt */
  UART1_IRQn                    =   43,     /*    UART 1 Combined Interrupt */
  UART2_IRQn                    =   44,     /*    UART 2 Combined Interrupt */
  UART3_IRQn                    =   45,     /*    UART 3 Combined Interrupt */
  UART4_IRQn                    =   46,     /*    UART 4 Combined Interrupt */
  UARTOVF_IRQn                  =   47,     /*    UART Overflow (0, 1, 2, 3, 4 & 5) Interrupt */
  ETHERNET_IRQn                 =   48,     /*    Ethernet Interrupt */
  I2S0_IRQn                     =   49,     /*    FPGA Audio I2S Interrupt */
  TSC_IRQn                      =   50,     /*    Touch Screen Interrupt */
  USB_IRQn                      =   51,     /*    USB Interrupt */
  SPI0_IRQn                     =   52,     /*    SPI ADC Interrupt */
  SPI1_IRQn                     =   53,     /*    SPI (Shield 0) Interrupt */
  SPI2_IRQn                     =   54,     /*    SPI (Shield 1) Interrupt */
  GPIO0_IRQn                    =   68,     /*    GPIO 0 Combined Interrupt */
  GPIO1_IRQn                    =   69,     /*    GPIO 1 Combined Interrupt */
  GPIO2_IRQn                    =   70,     /*    GPIO 2 Combined Interrupt */
  GPIO3_IRQn                    =   71,     /*    GPIO 3 Combined Interrupt */
  GPIO0_0_IRQn                  =   72,     /*    GPIO 0 individual interrupt 0 */
  GPIO0_1_IRQn                  =   73,     /*    GPIO 0 individual interrupt 1 */
  GPIO0_2_IRQn                  =   74,     /*    GPIO 0 individual interrupt 2 */
  GPIO0_3_IRQn                  =   75,     /*    GPIO 0 individual interrupt 3 */
  GPIO0_4_IRQn                  =   76,     /*    GPIO 0 individual interrupt 4 */
  GPIO0_5_IRQn                  =   77,     /*    GPIO 0 individual interrupt 5 */
  GPIO0_6_IRQn                  =   78,     /*    GPIO 0 individual interrupt 6 */
  GPIO0_7_IRQn                  =   79,     /*    GPIO 0 individual interrupt 7 */
  GPIO0_8_IRQn                  =   80,     /*    GPIO 0 individual interrupt 8 */
  GPIO0_9_IRQn                  =   81,     /*    GPIO 0 individual interrupt 9 */
  GPIO0_10_IRQn                 =   82,     /*    GPIO 0 individual interrupt 10 */
  GPIO0_11_IRQn                 =   83,     /*    GPIO 0 individual interrupt 11 */
  GPIO0_12_IRQn                 =   84,     /*    GPIO 0 individual interrupt 12 */
  GPIO0_13_IRQn                 =   85,     /*    GPIO 0 individual interrupt 13 */
  GPIO0_14_IRQn                 =   86,     /*    GPIO 0 individual interrupt 14 */
  GPIO0_15_IRQn                 =   87,     /*    GPIO 0 individual interrupt 15 */
  GPIO1_0_IRQn                  =   88,     /*    GPIO 1 individual interrupt 0 */
  GPIO1_1_IRQn                  =   89,     /*    GPIO 1 individual interrupt 1 */
  GPIO1_2_IRQn                  =   90,     /*    GPIO 1 individual interrupt 2 */
  GPIO1_3_IRQn                  =   91,     /*    GPIO 1 individual interrupt 3 */
  GPIO1_4_IRQn                  =   92,     /*    GPIO 1 individual interrupt 4 */
  GPIO1_5_IRQn                  =   93,     /*    GPIO 1 individual interrupt 5 */
  GPIO1_6_IRQn                  =   94,     /*    GPIO 1 individual interrupt 6 */
  GPIO1_7_IRQn                  =   95,     /*    GPIO 1 individual interrupt 7 */
  GPIO1_8_IRQn                  =   96,     /*    GPIO 1 individual interrupt 8 */
  GPIO1_9_IRQn                  =   97,     /*    GPIO 1 individual interrupt 9 */
  GPIO1_10_IRQn                 =   98,     /*    GPIO 1 individual interrupt 10 */
  GPIO1_11_IRQn                 =   99,     /*    GPIO 1 individual interrupt 11 */
  GPIO1_12_IRQn                 =  100,     /*    GPIO 1 individual interrupt 12 */
  GPIO1_13_IRQn                 =  101,     /*    GPIO 1 individual interrupt 13 */
  GPIO1_14_IRQn                 =  102,     /*    GPIO 1 individual interrupt 14 */
  GPIO1_15_IRQn                 =  103,     /*    GPIO 1 individual interrupt 15 */
  GPIO2_0_IRQn                  =  104,     /*    GPIO 2 individual interrupt 0 */
  GPIO2_1_IRQn                  =  105,     /*    GPIO 2 individual interrupt 1 */
  GPIO2_2_IRQn                  =  106,     /*    GPIO 2 individual interrupt 2 */
  GPIO2_3_IRQn                  =  107,     /*    GPIO 2 individual interrupt 3 */
  GPIO2_4_IRQn                  =  108,     /*    GPIO 2 individual interrupt 4 */
  GPIO2_5_IRQn                  =  109,     /*    GPIO 2 individual interrupt 5 */
  GPIO2_6_IRQn                  =  110,     /*    GPIO 2 individual interrupt 6 */
  GPIO2_7_IRQn                  =  111,     /*    GPIO 2 individual interrupt 7 */
  GPIO2_8_IRQn                  =  112,     /*    GPIO 2 individual interrupt 8 */
  GPIO2_9_IRQn                  =  113,     /*    GPIO 2 individual interrupt 9 */
  GPIO2_10_IRQn                 =  114,     /*    GPIO 2 individual interrupt 10 */
  GPIO2_11_IRQn                 =  115,     /*    GPIO 2 individual interrupt 11 */
  GPIO2_12_IRQn                 =  116,     /*    GPIO 2 individual interrupt 12 */
  GPIO2_13_IRQn                 =  117,     /*    GPIO 2 individual interrupt 13 */
  GPIO2_14_IRQn                 =  118,     /*    GPIO 2 individual interrupt 14 */
  GPIO2_15_IRQn                 =  119,     /*    GPIO 2 individual interrupt 15 */
  GPIO3_0_IRQn                  =  120,     /*    GPIO 3 individual interrupt 0 */
  GPIO3_1_IRQn                  =  121,     /*    GPIO 3 individual interrupt 1 */
  GPIO3_2_IRQn                  =  122,     /*    GPIO 3 individual interrupt 2 */
  GPIO3_3_IRQn                  =  123,     /*    GPIO 3 individual interrupt 3 */
  UART5RX_IRQn                  =  124,     /*    UART 5 Receive Interrupt */
  UART5TX_IRQn                  =  125,     /*    UART 5 Transmit Interrupt */
  UART5_IRQn                    =  126,     /*    UART 5 Combined Interrupt */
  HDCLCD_IRQn                   =  127      /*    HDCLCD Interrupt */
} IRQn_Type;


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* -------  Start of section using anonymous unions and disabling warnings  ------- */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif


/* --------  Configuration of the Cortex-M Processor and Core Peripherals  ------- */
#define __ARMv81MML_REV           0x0001U   /* Core revision r0p1 */
#define __SAUREGION_PRESENT       1U        /* SAU regions present */
#define __MPU_PRESENT             1U        /* MPU present */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __NVIC_PRIO_BITS          3U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             1U        /* FPU present */
#define __FPU_DP                  1U        /* double precision FPU */
#define __DSP_PRESENT             1U        /* DSP extension present */
#define __MVE_PRESENT             1U        /* MVE extensions present */
#define __MVE_FP                  1U        /* MVE floating point present */

#include "core_armv81mml.h"                 /* Processor and core peripherals */
#include "system_MPS3_CM55.h"                   /* System Header */


/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */

/*------------- Universal Asynchronous Receiver Transmitter (UART) -----------*/
typedef struct /* see "Cortex-M System Design Kit Technical Reference Manual r1p1" */
{
  __IOM  uint32_t  DATA;                     /* Offset: 0x000 (R/W) Data Register    */
  __IOM  uint32_t  STATE;                    /* Offset: 0x004 (R/W) Status Register  */
  __IOM  uint32_t  CTRL;                     /* Offset: 0x008 (R/W) Control Register */
  union {
    __IM   uint32_t  INTSTATUS;              /* Offset: 0x00C (R/ ) Interrupt Status Register */
    __OM   uint32_t  INTCLEAR;               /* Offset: 0x00C ( /W) Interrupt Clear Register  */
    };
  __IOM  uint32_t  BAUDDIV;                  /* Offset: 0x010 (R/W) Baudrate Divider Register */
} UART_TypeDef;

/* UART DATA Register Definitions */
#define UART_DATA_DATA_Pos                0                                              /* UART DATA: DATA position */
#define UART_DATA_DATA_Msk               (0xFFUL /*<< UART_DATA_DATA_Pos*/)              /* UART DATA: DATA mask */

/* UART STATE Register Definitions */
#define UART_STATE_RXBO_Pos               3                                              /* UART STATE: RX buffer overrun position */
#define UART_STATE_RXBO_Msk              (1UL << UART_STATE_RXOR_Pos)                    /* UART STATE: RX buffer overrun mask */

#define UART_STATE_TXBO_Pos               2                                              /* UART STATE: TX buffer overrun position */
#define UART_STATE_TXBO_Msk              (1UL << UART_STATE_TXOR_Pos)                    /* UART STATE: TX buffer overrun mask */

#define UART_STATE_RXBF_Pos               1                                              /* UART STATE: RX buffer full position */
#define UART_STATE_RXBF_Msk              (1UL << UART_STATE_RXBF_Pos)                    /* UART STATE: RX buffer full mask */

#define UART_STATE_TXBF_Pos               0                                              /* UART STATE: TX buffer full position */
#define UART_STATE_TXBF_Msk              (1UL /*<< UART_STATE_TXBF_Pos*/)                /* UART STATE: TX buffer full mask */

/* UART CTRL Register Definitions */
#define UART_CTRL_HSTM_Pos                6                                              /* UART CTRL: HSTM position */
#define UART_CTRL_HSTM_Msk               (1UL << UART_CTRL_HSTM_Pos)                     /* UART CTRL: HSTM mask */

#define UART_CTRL_RXORINTEN_Pos           5                                              /* UART CTRL: RX overrun interrupt enable position */
#define UART_CTRL_RXORINTEN_Msk          (1UL << UART_CTRL_RXORINTEN_Pos)                /* UART CTRL: RX overrun interrupt enable mask */

#define UART_CTRL_TXORINTEN_Pos           4                                              /* UART CTRL: TX overrun interrupt enable position */
#define UART_CTRL_TXORINTEN_Msk          (1UL << UART_CTRL_TXORINTEN_Pos)                /* UART CTRL: TX overrun interrupt enable mask */

#define UART_CTRL_RXINTEN_Pos             3                                              /* UART CTRL: RX interrupt enable position */
#define UART_CTRL_RXINTEN_Msk            (1UL << UART_CTRL_RXINTEN_Pos)                  /* UART CTRL: RX interrupt enable mask */

#define UART_CTRL_TXINTEN_Pos             2                                              /* UART CTRL: TX interrupt enable position */
#define UART_CTRL_TXINTEN_Msk            (1UL << UART_CTRL_TXINTEN_Pos)                  /* UART CTRL: TX interrupt enable mask */

#define UART_CTRL_RXEN_Pos                1                                              /* UART CTRL: RXEN position */
#define UART_CTRL_RXEN_Msk               (1UL << UART_CTRL_RXEN_Pos)                     /* UART CTRL: RXEN mask */

#define UART_CTRL_TXEN_Pos                0                                              /* UART CTRL: TXEN position */
#define UART_CTRL_TXEN_Msk               (1UL /*<< UART_CTRL_TXEN_Pos*/)                 /* UART CTRL: TXEN mask */

/* UART INTSTATUS Register Definitions */
#define UART_INTSTATUS_RXORINT_Pos       3                                               /* UART INTSTATUS: RX overrun interrupt position */
#define UART_INTSTATUS_RXORINT_Msk       (1UL << UART_INTSTATUS_RXORINT_Pos)             /* UART INTSTATUS: RX overrun interrupt mask */

#define UART_INTSTATUS_TXORINT_Pos        2                                              /* UART INTSTATUS: TX overrun interrupt position */
#define UART_INTSTATUS_TXORINT_Msk       (1UL << UART_INTSTATUS_TXORINT_Pos)             /* UART INTSTATUS: TX overrun interrupt mask */

#define UART_INTSTATUS_RXINT_Pos          1                                              /* UART INTSTATUS: RX interrupt position */
#define UART_INTSTATUS_RXINT_Msk         (1UL << UART_INTSTATUS_RXINT_Pos)               /* UART INTSTATUS: RX interrupt mask */

#define INTSTATUS_TXINT_Pos               0                                              /* UART INTSTATUS: TX interrupt position */
#define INTSTATUS_TXINT_Msk              (1UL /*<< UART_INTSTATUS_TXINT_Pos*/)           /* UART INTSTATUS: TX interrupt mask */

/* UART INTCLEAR Register Definitions */
#define UART_INTCLEAR_RXORINT_Pos         3                                              /* UART INTCLEAR: RX overrun interrupt position */
#define UART_INTCLEAR_RXORINT_Msk        (1UL << UART_INTCLEAR_RXORINT_Pos)              /* UART INTCLEAR: RX overrun interrupt mask */

#define UART_INTCLEAR_TXORINT_Pos         2                                              /* UART INTCLEAR: TX overrun interrupt position */
#define UART_INTCLEAR_TXORINT_Msk        (1UL << UART_INTCLEAR_TXORINT_Pos)              /* UART INTCLEAR: TX overrun interrupt mask */

#define UART_INTCLEAR_RXINT_Pos           1                                              /* UART INTCLEAR: RX interrupt position */
#define UART_INTCLEAR_RXINT_Msk          (1UL << UART_INTCLEAR_RXINT_Pos)                /* UART INTCLEAR: RX interrupt mask */

#define UART_INTCLEAR_TXINT_Pos           0                                              /* UART INTCLEAR: TX interrupt position */
#define UART_INTCLEAR_TXINT_Msk          (1UL /*<< UART_INTCLEAR_TXINT_Pos*/)            /* UART INTCLEAR: TX interrupt mask */

/* UART BAUDDIV Register Definitions */
#define UART_BAUDDIV_Pos                  0                                              /* UART BAUDDIV: BAUDDIV position */
#define UART_BAUDDIV_Msk                 (0xFFFFFUL /*<< UART_BAUDDIV_Pos*/)             /* UART BAUDDIV: BAUDDIV mask */


/*--------------------- Timer (TIMER) ---------------------------------------*/
typedef struct /* see "Cortex-M System Design Kit Technical Reference Manual r1p1" */
{
  __IOM  uint32_t  CTRL;                    /* Offset: 0x000 (R/W) Control Register */
  __IOM  uint32_t  VALUE;                   /* Offset: 0x004 (R/W) Current Value Register */
  __IOM  uint32_t  RELOAD;                  /* Offset: 0x008 (R/W) Reload Value Register */
  union {
  __IM   uint32_t  INTSTATUS;               /* Offset: 0x00C (R/ ) Interrupt Status Register */
  __OM   uint32_t  INTCLEAR;                /* Offset: 0x00C ( /W) Interrupt Clear Register */
    };
} TIMER_TypeDef;

/* TIMER CTRL Register Definitions */
#define TIMER_CTRL_INTEN_Pos              3                                              /* TIMER CTRL.INTEN: Interrupt enable position */
#define TIMER_CTRL_INTEN_Msk             (1UL << TIMER_CTRL_INTEN_Pos)                   /* TIMER CTRL.INTEN: Interrupt enable mask */

#define TIMER_CTRL_SELEXTCLK_Pos          2                                              /* TIMER CTRL.SELEXTCLK: Select external input as clock position */
#define TIMER_CTRL_SELEXTCLK_Msk         (1UL << TIMER_CTRL_SELEXTCLK_Pos)               /* TIMER CTRL.SELEXTCLK: Select external input as clock mask */

#define TIMER_CTRL_SELEXTEN_Pos           1                                              /* TIMER CTRL.SELEXTEN: Select external input as enable position */
#define TIMER_CTRL_SELEXTEN_Msk          (1UL << TIMER_CTRL_SELEXTEN_Pos)                /* TIMER CTRL.SELEXTEN: Select external input as enable mask */

#define TIMER_CTRL_EN_Pos                 0                                              /* TIMER CTRL.EN: Enable position */
#define TIMER_CTRL_EN_Msk                (1UL /*<< TIMER_CTRL_EN_Pos*/)                  /* TIMER CTRL.EN: Enable mask */

/* TIMER VALUE Register Definitions */
#define TIMER_VALUE_Pos                   0                                              /* TIMER VALUE: Current value position */
#define TIMER_VALUE_Msk                  (0xFFFFFFFFUL /*<< TIMER_VALUE_Pos*/)           /* TIMER VALUE: Current value mask */

/* TIMER RELOAD Register Definitions */
#define TIMER_RELOAD_Pos                  0                                              /* TIMER RELOAD: Reload value position */
#define TIMER_RELOAD_Msk                 (0xFFFFFFFFUL /*<< TIMER_RELOAD_Pos*/)          /* TIMER RELOAD: Reload value mask */

/* TIMER INTSTATUS Register Definitions */
#define TIMER_INTSTATUS_INT_Pos           0                                              /* TIMER INTSTATUS.INT: Interrupt status position */
#define TIMER_INTSTATUS_INT_Msk          (1UL /*<< TIMER_INTSTATUS_INT_Pos*/)            /* TIMER INTSTATUS.INT: Interrupt status mask */

/* TIMER INTCLEAR Register Definitions */
#define TIMER_INTCLEAR_INT_Pos            0                                             /* TIMER INTCLEAR.INT: Interrupt clear position */
#define TIMER_INTCLEAR_INT_Msk           (1UL /*<< TIMER_INTCLEAR_INT_Pos*/)            /* TIMER INTCLEAR.INT: Interrupt clear mask */


/*-------------------- Dual Timer (DUALTIMER) -------------------------------------------*/
typedef struct /* see "Cortex-M System Design Kit Technical Reference Manual r1p1" */
{
  __IOM uint32_t TIMER1LOAD;                 /* Offset: 0x000 (R/W) Timer 1 Load */
  __IM  uint32_t TIMER1VALUE;                /* Offset: 0x004 (R/ ) Timer 1 Counter Current Value */
  __IOM uint32_t TIMER1CONTROL;              /* Offset: 0x008 (R/W) Timer 1 Control */
  __OM  uint32_t TIMER1INTCLR;               /* Offset: 0x00C ( /W) Timer 1 Interrupt Clear */
  __IM  uint32_t TIMER1RIS;                  /* Offset: 0x010 (R/ ) Timer 1 Raw Interrupt Status */
  __IM  uint32_t TIMER1MIS;                  /* Offset: 0x014 (R/ ) Timer 1 masked Interrupt Status */
  __IOM uint32_t TIMER1BGLOAD;               /* Offset: 0x018 (R/W) Background Load Register */
        uint32_t RESERVED0;
  __IOM uint32_t TIMER2LOAD;                 /* Offset: 0x020 (R/W) Timer 2 Load */
  __IM  uint32_t TIMER2VALUE;                /* Offset: 0x024 (R/ ) Timer 2 Counter Current Value */
  __IOM uint32_t TIMER2CTRL;                 /* Offset: 0x028 (R/W) Timer 2 Control */
  __OM  uint32_t TIMER2INTCLR;               /* Offset: 0x02C ( /W) Timer 2 Interrupt Clear */
  __IM  uint32_t TIMER2RIS;                  /* Offset: 0x030 (R/ ) Timer 2 Raw Interrupt Status */
  __IM  uint32_t TIMER2MIS;                  /* Offset: 0x034 (R/ ) Timer 2 masked Interrupt Status */
  __IOM uint32_t TIMER2BGLOAD;               /* Offset: 0x038 (R/W) Background Load Register */
        uint32_t RESERVED1[945];
  __IOM uint32_t TIMERITCR;                  /* Offset: 0xF00 (R/W) Integration Test Control Register */
  __OM  uint32_t TIMERITOP;                  /* Offset: 0xF04 ( /W) Integration Test Output Set Register */
} DUALTIMER_TypeDef;


typedef struct /* see "Cortex-M System Design Kit Technical Reference Manual r1p1" */
{
  __IOM uint32_t LOAD;                       /* Offset: 0x000 (R/W) Timer Load */
  __IM  uint32_t VALUE;                      /* Offset: 0x000 (R/W) Timer Counter Current Value */
  __IOM uint32_t CTRL;                       /* Offset: 0x000 (R/W) Timer Control */
  __OM  uint32_t INTCLR;                     /* Offset: 0x000 (R/W) Timer Interrupt Clear */
  __IM  uint32_t RIS;                        /* Offset: 0x000 (R/W) Timer Raw Interrupt Status */
  __IM  uint32_t MIS;                        /* Offset: 0x000 (R/W) Timer masked Interrupt Status */
  __IOM uint32_t BGLOAD;                     /* Offset: 0x000 (R/W) Background Load Register */
} DUALTIMER_SINGLE_TypeDef;

/* DUALTIMER LOAD Register Definitions */
#define DUALTIMER_LOAD_Pos                0                                              /* DUALTIMER LOAD: LOAD position */
#define DUALTIMER_LOAD_Msk               (0xFFFFFFFFUL /*<< DUALTIMER_LOAD_Pos*/)        /* DUALTIMER LOAD: LOAD mask */

/* DUALTIMER VALUE Register Definitions */
#define DUALTIMER_VALUE_Pos               0                                              /* DUALTIMER VALUE: VALUE position */
#define DUALTIMER_VALUE_Msk              (0xFFFFFFFFUL /*<< DUALTIMER_VALUE_Pos*/)       /* DUALTIMER VALUE: VALUE mask */

/* DUALTIMER CONTROL Register Definitions */
#define DUALTIMER_CONTROL_EN_Pos          7                                              /* DUALTIMER CONTROL_EN: CONTROL Enable position */
#define DUALTIMER_CONTROL_EN_Msk         (1UL << DUALTIMER_CTRL_EN_Pos)                  /* DUALTIMER CONTROL_EN: CONTROL Enable mask */

#define DUALTIMER_CONTROL_MODE_Pos        6                                              /* DUALTIMER CONTROL_MODE: CONTROL MODE position */
#define DUALTIMER_CONTROL_MODE_Msk       (1UL << DUALTIMER_CTRL_MODE_Pos)                /* DUALTIMER CONTROL_MODE: CONTROL MODE mask */

#define DUALTIMER_CONTROL_INTEN_Pos       5                                              /* DUALTIMER CONTROL_INTEN: CONTROL Int Enable position */
#define DUALTIMER_CONTROL_INTEN_Msk      (1UL << DUALTIMER_CTRL_INTEN_Pos)               /* DUALTIMER CONTROL_INTEN: CONTROL Int Enable mask */

#define DUALTIMER_CONTROL_PRE_Pos         2                                              /* DUALTIMER CONTROL.PRE: CONTROL TimerPre position */
#define DUALTIMER_CONTROL_PRE_Msk        (3UL << DUALTIMER_CTRL_PRE_Pos)                 /* DUALTIMER CONTROL.PRE: CONTROL TimerPre mask */

#define DUALTIMER_CONTROL_SIZE_Pos        1                                              /* DUALTIMER CONTROL.SIZE: CONTROL SIZE position */
#define DUALTIMER_CONTROL_SIZE_Msk       (1UL << DUALTIMER_CTRL_SIZE_Pos)                /* DUALTIMER CONTROL.SIZE: CONTROL SIZE mask */

#define DUALTIMER_CONTROL_ONESHOT_Pos     0                                              /* DUALTIMER CONTROL.ONESHOT: CONTROL ONESHOOT position */
#define DUALTIMER_CONTROL_ONESHOT_Msk    (1UL /*<< DUALTIMER_CONTROL_ONESHOT_Pos*/)      /* DUALTIMER CONTROL.ONESHOT: CONTROL ONESHOOT mask */

/* DUALTIMER INTCLR Register Definitions */
#define DUALTIMER_INTCLR_Pos              0                                              /* DUALTIMER INTCLR: INT Clear position */
#define DUALTIMER_INTCLR_Msk             (1UL /*<< DUALTIMER_INTCLR_Pos*/)               /* DUALTIMER INTCLR: INT Clear  mask */

/* DUALTIMER RIS Register Definitions */
#define DUALTIMER_RIS_RINT_Pos            0                                              /* DUALTIMER RIS.RINT: Raw Int Status position */
#define DUALTIMER_RIS_RINT_Msk           (1UL /*<< DUALTIMER_RIS_RINT_Pos*/)             /* DUALTIMER RIS.RINT: Raw Int Status mask */

/* DUALTIMER MIS Register Definitions */
#define DUALTIMER_MIS_INT_Pos             0                                              /* DUALTIMER MIS.INT: mask Int Status position */
#define DUALTIMER_MIS_INT_Msk            (1UL /*<< DUALTIMER_MIS_INT_Pos*/)              /* DUALTIMER MIS.INT: mask Int Status mask */

/* DUALTIMER BGLOAD Register Definitions */
#define DUALTIMER_BGLOAD_Pos              0                                              /* DUALTIMER BGLOAD: Background Load position */
#define DUALTIMER_BGLOAD_Msk             (0xFFFFFFFFUL /*<< DUALTIMER_BGLOAD_Pos*/)      /* DUALTIMER BGLOAD: Background Load mask */

/* DUALTIMER TIMERITCR Register Definitions */
#define DUALTIMER_TIMERITCR_ITMEN_Pos     0                                              /* DUALTIMER TIMERITCR.ITMEN: Integration Test Mode Enable position */
#define DUALTIMER_TIMERITCR_ITMEN_Msk    (1UL /*<< DUALTIMER_TIMERITCR_ITMEN_Pos*/)      /* DUALTIMER TIMERITCR.ITMEN: Integration Test Mode Enable mask */

/* DUALTIMER TIMERITOP Register Definitions */
#define DUALTIMER_TIMERITOP_TIMINT2_Pos   1                                              /* DUALTIMER TIMERITOP.TIMINT2: Integration Test TIMINT2 value position */
#define DUALTIMER_TIMERITOP_TIMINT2_Msk  (1UL << DUALTIMER_TIMERITOP_TIMINT2_Pos)        /* DUALTIMER TIMERITOP.TIMINT2: Integration Test TIMINT2 value mask */

#define DUALTIMER_TIMERITOP_TIMINT1_Pos   0                                              /* DUALTIMER TIMERITOP.TIMINT1: Integration Test TIMINT1 value position */
#define DUALTIMER_TIMERITOP_TIMINT1_Msk  (1UL /*<< DUALTIMER_TIMERITOP_TIMINT1_Pos*/)    /* DUALTIMER TIMERITOP.TIMINT1: Integration Test TIMINT1 value mask */


/*------------------- Watchdog ----------------------------------------------*/
typedef struct /* see "Cortex-M System Design Kit Technical Reference Manual r1p1" */
{
  __IOM  uint32_t  LOAD;                    /* Offset: 0x000 (R/W) Load Register */
  __IOM  uint32_t  VALUE;                   /* Offset: 0x004 (R/ ) Value Register */
  __IOM  uint32_t  CONTROL;                 /* Offset: 0x008 (R/W) Control Register */
  __IOM  uint32_t  INTCLR;                  /* Offset: 0x00C ( /W) Clear Interrupt Register */
  __IOM  uint32_t  RIS;                     /* Offset: 0x010 (R/ ) Raw Interrupt Status Register */
  __IOM  uint32_t  MIS;                     /* Offset: 0x014 (R/ ) Interrupt Status Register */
         uint32_t  RESERVED0[762];
  __IOM  uint32_t  LOCK;                    /* Offset: 0xC00 (R/W) Lock Register */
         uint32_t  RESERVED1[191];
  __IOM  uint32_t  ITCR;                    /* Offset: 0xF00 (R/W) Integration Test Control Register */
  __IOM  uint32_t  ITOP;                    /* Offset: 0xF04 ( /W) Integration Test Output Set Register */
} WATCHDOG_TypeDef;

/* WATCHDOG LOAD Register Definitions */
#define WATCHDOG_LOAD_Pos                 0                                              /* WATCHDOG LOAD: LOAD position */
#define WATCHDOG_LOAD_Msk                (0xFFFFFFFFUL /*<< WATCHDOG_LOAD_Pos*/)         /* WATCHDOG LOAD: LOAD mask */

/* WATCHDOG VALUE Register Definitions */
#define WATCHDOG_VALUE_Pos                0                                              /* WATCHDOG VALUE: VALUE position */
#define WATCHDOG_VALUE_Msk               (0xFFFFFFFFUL /*<< WATCHDOG_VALUE_Pos*/)        /* WATCHDOG VALUE: VALUE mask */

/* WATCHDOG CONTROL Register Definitions */
#define WATCHDOG_CONTROL_RESEN_Pos        1                                              /* WATCHDOG CONTROL.RESEN: Reset Output Enable position */
#define WATCHDOG_CONTROL_RESEN_Msk       (1UL << WATCHDOG_CONTROL_RESEN_Pos)             /* WATCHDOG CONTROL.RESEN: Reset Output Enable mask */

#define WATCHDOG_CONTROL_INTEN_Pos        0                                              /* WATCHDOG CONTROL.INTEN: Int Enable position */
#define WATCHDOG_CONTROL_INTEN_Msk       (1UL /*<< WATCHDOG_CONTROL_INTEN_Pos*/)         /* WATCHDOG CONTROL.INTEN: Int Enable mask */

/* WATCHDOG INTCLR Register Definitions */
#define WATCHDOG_INTCLR_Pos               0                                              /* WATCHDOG INTCLR: Interrutp clear position */
#define WATCHDOG_INTCLR_Msk              (1UL /*<< WATCHDOG_INTCLR_Pos*/)                /* WATCHDOG INTCLR: Interrutp clear mask */

/* WATCHDOG RIS Register Definitions */
#define WATCHDOG_RIS_RINT_Pos             0                                              /* WATCHDOG RIS.RINT: Raw Watchdog Interrupt position */
#define WATCHDOG_RIS_RINT_Msk            (1UL /*<< WATCHDOG_RIS_RINT_Pos*/)              /* WATCHDOG RIS.RINT: Raw Watchdog Interrupt mask */

/* WATCHDOG MIS Register Definitions */
#define WATCHDOG_MIS_INT_Pos              0                                              /* WATCHDOG MIS.INT: Watchdog Interrupt position */
#define WATCHDOG_MIS_INT_Msk             (1UL /*<< WATCHDOG_MIS_INT_Pos*/)               /* WATCHDOG MIS.INT: Watchdog Interrupt mask */

/* WATCHDOG LOCK Register Definitions */
#define WATCHDOG_LOCK_WEN_Pos             0                                              /* WATCHDOG LOCK.WEN: Register write enable status position */
#define WATCHDOG_LOCK_WEN_Msk            (1UL /*<< WATCHDOG_LOCK_WEN_Pos*/)              /* WATCHDOG LOCK.WEN: Register write enable status mask */

/* WATCHDOG INTEGTESTEN Register Definitions */
#define WATCHDOG_ITCR_ITMEN_Pos           0                                              /* WATCHDOG ITCR.ITMEN: Integration Test Mode Enable position */
#define WATCHDOG_ITCR_ITMEN_Msk          (1UL /*<< WATCHDOG_ITCR_ITMEN_Pos*/)            /* WATCHDOG ITCR.ITMEN: Integration Test Mode Enable mask */

/* WATCHDOG INTEGTESTOUTSET Register Definitions */
#define WATCHDOG_ITOP_WDOGINT_Pos         1                                              /* WATCHDOG ITOP.WDOGINT: Integration Test WDOGINT value position */
#define WATCHDOG_ITOP_WDOGINT_Msk        (1UL << WATCHDOG_ITOP_WDOGINT_Pos)              /* WATCHDOG ITOP.WDOGINT: Integration Test WDOGINT value mask */

#define WATCHDOG_ITOP_WDOGRES_Pos         0                                              /* WATCHDOG ITOP.WDOGRES: Integration Test WDOGRES value position */
#define WATCHDOG_ITOP_WDOGRES_Msk        (1UL /*<< WATCHDOG_ITOP_WDOGRES_Pos*/)          /* WATCHDOG ITOP.WDOGRES: Integration Test WDOGRES value mask */


/*-------------------- General Purpose Input Output (GPIO) -------------------*/
typedef struct /* see "Cortex-M System Design Kit Technical Reference Manual r1p1" */
{
  __IOM  uint32_t  DATA;                    /* Offset: 0x000 (R/W) DATA Register */
  __IOM  uint32_t  DATAOUT;                 /* Offset: 0x004 (R/W) Data Output Latch Register */
         uint32_t  RESERVED0[2];
  __IOM  uint32_t  OUTENSET;                /* Offset: 0x010 (R/W) Output Enable Set Register */
  __IOM  uint32_t  OUTENCLR;                /* Offset: 0x014 (R/W) Output Enable Clear Register */
  __IOM  uint32_t  ALTFUNCSET;              /* Offset: 0x018 (R/W) Alternate Function Set Register */
  __IOM  uint32_t  ALTFUNCCLR;              /* Offset: 0x01C (R/W) Alternate Function Clear Register */
  __IOM  uint32_t  INTENSET;                /* Offset: 0x020 (R/W) Interrupt Enable Set Register */
  __IOM  uint32_t  INTENCLR;                /* Offset: 0x024 (R/W) Interrupt Enable Clear Register */
  __IOM  uint32_t  INTTYPESET;              /* Offset: 0x028 (R/W) Interrupt Type Set Register */
  __IOM  uint32_t  INTTYPECLR;              /* Offset: 0x02C (R/W) Interrupt Type Clear Register */
  __IOM  uint32_t  INTPOLSET;               /* Offset: 0x030 (R/W) Interrupt Polarity Set Register */
  __IOM  uint32_t  INTPOLCLR;               /* Offset: 0x034 (R/W) Interrupt Polarity Clear Register */
  union {
  __IM   uint32_t  INTSTATUS;               /* Offset: 0x038 (R/ ) Interrupt Status Register */
  __OM   uint32_t  INTCLEAR;                /* Offset: 0x038 ( /W) Interrupt Clear Register */
  };
         uint32_t RESERVED1[241];
  __IOM  uint32_t MASKLOWBYTE[256];         /* Offset: 0x400 (R/W) Lower byte masked Access Register */
  __IOM  uint32_t MASKHIGHBYTE[256];        /* Offset: 0x800 (R/W) Upper byte masked Access Register */
} GPIO_TypeDef;

/* GPIO DATA Register Definitions */
#define GPIO_DATA_Pos                     0                                              /* GPIO DATA: DATA position */
#define GPIO_DATA_Msk                    (0xFFFFUL /*<< GPIO_DATA_Pos*/)                 /* GPIO DATA: DATA mask */

/* GPIO DATAOUT Register Definitions */
#define GPIO_DATAOUT_Pos                  0                                              /* GPIO DATAOUT: DATAOUT position */
#define GPIO_DATAOUT_Msk                 (0xFFFFUL /*<< GPIO_DATAOUT_Pos*/)              /* GPIO DATAOUT: DATAOUT mask */

/* GPIO OUTENSET Register Definitions */
#define GPIO_OUTENSET_Pos                 0                                              /* GPIO OUTEN: OUTEN position */
#define GPIO_OUTENSET_Msk                (0xFFFFUL /*<< GPIO_OUTEN_Pos*/)                /* GPIO OUTEN: OUTEN mask */

/* GPIO OUTENCLR Register Definitions */
#define GPIO_OUTENCLR_Pos                 0                                              /* GPIO OUTEN: OUTEN position */
#define GPIO_OUTENCLR_Msk                (0xFFFFUL /*<< GPIO_OUTEN_Pos*/)                /* GPIO OUTEN: OUTEN mask */

/* GPIO ALTFUNCSET Register Definitions */
#define GPIO_ALTFUNCSET_Pos               0                                              /* GPIO ALTFUNC: ALTFUNC position */
#define GPIO_ALTFUNCSET_Msk              (0xFFFFUL /*<< GPIO_ALTFUNC_Pos*/)              /* GPIO ALTFUNC: ALTFUNC mask */

/* GPIO ALTFUNCCLR Register Definitions */
#define GPIO_ALTFUNCCLR_Pos               0                                              /* GPIO ALTFUNC: ALTFUNC position */
#define GPIO_ALTFUNCCLR_Msk              (0xFFFFUL /*<< GPIO_ALTFUNC_Pos*/)              /* GPIO ALTFUNC: ALTFUNC mask */

/* GPIO INTENSET Register Definitions */
#define GPIO_INTENSET_Pos                 0                                              /* GPIO INTEN: INTEN position */
#define GPIO_INTENSET_Msk                (0xFFFFUL /*<< GPIO_INTEN_Pos*/)                /* GPIO INTEN: INTEN mask */

/* GPIO INTENCLR Register Definitions */
#define GPIO_INTENCLR_Pos                 0                                              /* GPIO INTEN: INTEN position */
#define GPIO_INTENCLR_Msk                (0xFFFFUL /*<< GPIO_INTEN_Pos*/)                /* GPIO INTEN: INTEN mask */

/* GPIO INTTYPESET Register Definitions */
#define GPIO_INTTYPESET_Pos               0                                              /* GPIO INTTYPE: INTTYPE position */
#define GPIO_INTTYPESET_Msk              (0xFFFFUL /*<< GPIO_INTTYPE_Pos*/)              /* GPIO INTTYPE: INTTYPE mask */

/* GPIO INTTYPECLR Register Definitions */
#define GPIO_INTTYPECLR_Pos               0                                              /* GPIO INTTYPE: INTTYPE position */
#define GPIO_INTTYPECLR_Msk              (0xFFFFUL /*<< GPIO_INTTYPE_Pos*/)              /* GPIO INTTYPE: INTTYPE mask */

/* GPIO INTPOLSET Register Definitions */
#define GPIO_INTPOLSET_Pos                0                                              /* GPIO INTPOL: INTPOL position */
#define GPIO_INTPOLSET_Msk               (0xFFFFUL /*<< GPIO_INTPOL_Pos*/)               /* GPIO INTPOL: INTPOL mask */

/* GPIO INTPOLCLR Register Definitions */
#define GPIO_INTPOLCLR_Pos                0                                              /* GPIO INTPOL: INTPOL position */
#define GPIO_INTPOLCLR_Msk               (0xFFFFUL /*<< GPIO_INTPOL_Pos*/)               /* GPIO INTPOL: INTPOL mask */

/* GPIO INTCLEAR Register Definitions */
#define GPIO_INTSTATUS_Pos                0                                              /* GPIO INTSTATUS: INTSTATUS position */
#define GPIO_INTCLEAR_Msk                (0xFFUL /*<< GPIO_INTSTATUS_Pos*/)              /* GPIO INTSTATUS: INTSTATUS mask */

/* GPIO INTCLEAR Register Definitions */
#define GPIO_INTCLEAR_Pos                 0                                              /* GPIO INTCLEAR: INTCLEAR position */
#define GPIO_INTCLEAR_Msk                (0xFFUL /*<< GPIO_INTCLEAR_Pos*/)               /* GPIO INTCLEAR: INTCLEAR mask */

/* GPIO maskLOWBYTE Register Definitions */
#define GPIO_MASKLOWBYTE_Pos              0                                              /* GPIO maskLOWBYTE: maskLOWBYTE position */
#define GPIO_MASKLOWBYTE_Msk             (0x00FFUL /*<< GPIO_MASKLOWBYTE_Pos*/)          /* GPIO maskLOWBYTE: maskLOWBYTE mask */

/* GPIO maskHIGHBYTE Register Definitions */
#define GPIO_MASKHIGHBYTE_Pos             0                                              /* GPIO maskHIGHBYTE: maskHIGHBYTE position */
#define GPIO_MASKHIGHBYTE_Msk            (0xFF00UL /*<< GPIO_MASKHIGHBYTE_Pos*/)         /* GPIO maskHIGHBYTE: maskHIGHBYTE mask */


/*------------------- FPGA I/O ----------------------------------------------*/
typedef struct /* see "Application Note AN540 MPS3_CM55 FPGA for MPS3" */
{
  __IOM  uint32_t LED;                       /* Offset: 0x000 (R/W)  LED connections */
  __IOM  uint32_t MPS3_CM55CTRL;                 /* Offset: 0x004 (R/W)  MPS3_CM55 Control signals */
  __IOM  uint32_t BUTTON;                    /* Offset: 0x008 (R/W)  Buttons */
  __IOM  uint32_t GPIOALT2;                  /* Offset: 0x00C (R/W)  GPIO Alt Function 2 select */
  __IOM  uint32_t CLK1HZ;                    /* Offset: 0x010 (R/W)  1Hz up counter */
  __IOM  uint32_t CLK100HZ;                  /* Offset: 0x014 (R/W)  100Hz up counter */
  __IOM  uint32_t COUNTER;                   /* Offset: 0x018 (R/W)  Cycle Up Counter */
  __IOM  uint32_t PRESCALE;                  /* Offset: 0x01C (R/W)  Reload value for prescale counter */
  __IOM  uint32_t PSCNTR;                    /* Offset: 0x020 (R/W)  32-bit Prescale counter */
         uint32_t RESERVED3[1];
  __IOM  uint32_t SWITCHES;                  /* Offset: 0x028 (R/W)  Switch */
         uint32_t RESERVED4[8];
  __IOM  uint32_t MISC;                      /* Offset: 0x04C (R/W)  Miscellaneous control */
} FPGAIO_TypeDef;

/* FPGAIO_LED Register Definitions */
#define FPGAIO_LED_LED_Pos                0                                              /* FPGAIO LED.LED: LED position */
#define FPGAIO_LED_LED_Msk               (0x1FFUL /*<< FPGAIO_LED_LED_Pos*/)             /* FPGAIO LED.LED: LED mask */

/* FPGAIO_MPS3_CM55CTRL Register Definitions */
#define FPGAIO_MPS3_CM55CTRL_SPNIDEN_Pos      3                                              /* FPGAIO MPS3_CM55CTRL.SPNIDEN:  position */
#define FPGAIO_MPS3_CM55CTRL_SPNIDEN_Msk     (1UL << FPGAIO_MPS3_CM55CTRL_SPNIDEN_Pos)           /* FPGAIO MPS3_CM55CTRL.SPNIDEN:  mask */

#define FPGAIO_MPS3_CM55CTRL_SPIDEN_Pos       2                                              /* FPGAIO MPS3_CM55CTRL.SPIDEN:  position */
#define FPGAIO_MPS3_CM55CTRL_SPIDEN_Msk      (1UL << FPGAIO_MPS3_CM55CTRL_SPIDEN_Pos)            /* FPGAIO MPS3_CM55CTRL.SPIDEN:  mask */

#define FPGAIO_MPS3_CM55CTRL_NIDEN_Pos        1                                              /* FPGAIO MPS3_CM55CTRL.NIDEN:  position */
#define FPGAIO_MPS3_CM55CTRL_NIDEN_Msk       (1UL << FPGAIO_MPS3_CM55CTRL_NIDEN_Pos)             /* FPGAIO MPS3_CM55CTRL.NIDEN:  mask */

#define FPGAIO_MPS3_CM55CTRL_DBGEN_Pos        0                                              /* FPGAIO MPS3_CM55CTRL.DBGEN:  position */
#define FPGAIO_MPS3_CM55CTRL_DBGEN_Msk       (1UL /*<< FPGAIO_MPS3_CM55CTRL_DBGEN_Pos*/)         /* FPGAIO MPS3_CM55CTRL.DBGEN:  mask */

/* FPGAIO_BUTTON Register Definitions */
#define FPGAIO_BUTTON_Buttons_Pos         0                                              /* FPGAIO MPS3_CM55CTRL.Buttons:  position */
#define FPGAIO_BUTTON_Buttons_Msk        (3UL /*<< FPGACTRL_BUTTON_Buttons_Pos*/)        /* FPGAIO MPS3_CM55CTRL.Buttons:  mask */

/* FPGAIO_GPIOALT2 Register Definitions */
#define FPGAIO_GPIOALT2_PDMCLK_Pos        8                                              /* FPGAIO GPIOALT2.PDM_CLK:  position */
#define FPGAIO_GPIOALT2_PDMCLK_Msk       (1UL << FPGAIO_GPIOALT2_PDM_CLK_Pos)            /* FPGAIO GPIOALT2.PDM_CLK:  mask */

#define FPGAIO_GPIOALT2_PDMIN_Pos         0                                              /* FPGAIO GPIOALT2.PDM_IN:  position */
#define FPGAIO_GPIOALT2_PDMIN_Msk        (0xFFUL /*<< FPGAIO_GPIOALT2_PDM_IN_Pos*/)      /* FPGAIO GPIOALT2.PDM_IN:  mask */

/* FPGAIO_SWITCHES Register Definitions */
#define FPGAIO_SWITCHES_Switches_Pos      0                                              /* FPGAIO SWITCHES.Switches: SWITCHES position */
#define FPGAIO_SWITCHES_Switches_Msk     (7UL /*<< FPGAIO_SWITCHES_Switches_Pos*/)       /* FPGAIO SWITCHES.Switches: SWITCHES mask */

/* FPGAIO_MISC Register Definitions */
#define FPGAIO_MISC_SHIELD1SPInCS_Pos     9                                              /* FPGAIO MISC.SHIELD1SPInCS: SHIELD_1_SPI_nCS position */
#define FPGAIO_MISC_SHIELD1SPInCS_Msk    (1UL << FPGAIO_MISC_SHIELD1SPInCS_Pos)          /* FPGAIO MISC.SHIELD1SPInCS: SHIELD_1_SPI_nCS mask */

#define FPGAIO_MISC_SHIELD0SPInCS_Pos     8                                              /* FPGAIO MISC.SHIELD0SPInCS: SHIELD_0_SPI_nCS position */
#define FPGAIO_MISC_SHIELD0SPInCS_Msk    (1UL << FPGAIO_MISC_SHIELD0SPInCS_Pos)          /* FPGAIO MISC.SHIELD0SPInCS: SHIELD_0_SPI_nCS mask */

#define FPGAIO_MISC_ADCSPInCS_Pos         7                                              /* FPGAIO MISC.ADCSPInCS: ADC_SPI_nCS position */
#define FPGAIO_MISC_ADCSPInCS_Msk        (1UL << FPGAIO_MISC_ADCSPInCS_Pos)              /* FPGAIO MISC.ADCSPInCS: ADC_SPI_nCS mask */


/*------------------- Serial Communication Controller (SCC) ------------*/
typedef struct /* see "Application Note AN540 MPS3_CM55 FPGA for MPS3" */
{
  __IOM uint32_t CFG_REG0;                   /* Offset: 0x000 (R/W)  CPU_WAIT ctrl */
  __IOM uint32_t CFG_REG1;                   /* Offset: 0x004 (R/W)  DATA [r/w] */
  __IOM uint32_t CFG_REG2;                   /* Offset: 0x004 (R/W)  QSPI Select signal */
  __IM  uint32_t CFG_REG3;                   /* Offset: 0x008 (R/ )  Reserved */
  __IM  uint32_t CFG_REG4;                   /* Offset: 0x00C (R/ )  Board Revision */
  __I   uint32_t CFG_REG5;                   /* Offset: 0x014 (R/ )  System Clock */
        uint32_t RESERVED0[34];
  __IOM uint32_t SYS_CFGDATA_RTN;            /* Offset: 0x0A0 (R/W)  User data register */
  __IOM uint32_t SYS_CFGDATA_OUT;            /* Offset: 0x0A4 (R/W)  User data register */
  __IOM uint32_t SYS_CFGCTRL;                /* Offset: 0x0A8 (R/W)  Control register */
  __IOM uint32_t SYS_CFGSTAT;                /* Offset: 0x0AC (R/W)  Status register */
  __IOM uint32_t RESERVED1[20];
        uint32_t RESERVED2[958];
  __IM  uint32_t SCC_AID;                    /* Offset: 0xFF8 (R/ )  SCC AID Register */
  __IM  uint32_t SCC_ID;                     /* Offset: 0xFFC (R/ )  Contains information about the FPGA image */
} SCC_TypeDef;

/*
    CFG_REG0             [31:2] : Reserved
                            [1] : CPU_WAIT ctrl
                            [0] : Reserved
    CFG_REG1             [31:0] : DATA [r/w]
    CFG_REG2             [31:1] : Reserved
                            [0] : QSPI Select signal
    CFG_REG3             [31:0] : Reserved
    CFG_REG4             [31:4] : Reserved
                          [3:0] : Board Revision
    CFG_REG5             [31:0] : ACLK Frequency in Hz
    SYS_CFGDATA_RTN      [31:0] : DATA [r/w]
    SYS_CFGDATA_OUT      [31:0] : DATA [r/w]
    SYS_CFGCTRL            [31] : Start (generates interrupt on write to this bit)
                           [30] : R/W access
                        [29:26] : Reserved
                        [25:20] : Function value
                        [19:12] : Reserved
                         [11:0] : Device (value of 0/1/2 for supported clocks)
    SYS_CFGSTAT             [1] : Error
                            [0] : Complete
    SCC_AID             [31:24] : FPGA build number
                        [23:20] : V2M-MPS3 target board revision (A = 0, B = 1, C = 2)
                         [19:8] : Reserved
                          [7:0] : number of SCC configuration register
    SCC_ID              [31:24] : Implementer ID: 0x41 = ARM
                        [23:20] : Reserved
                        [19:16] : IP Architecture: 0x4 = AHB
                         [11:4] : Primary part number: 540 = AN540
                          [3:0] : Reserved
 */


/*------------------- Audio I2S control ----------------------------------------------*/
typedef struct /* see "?" */
{
  __IOM  uint32_t CONTROL;                   /* Offset: 0x000 (R/W) CONTROL Register */
  __IM   uint32_t STATUS;                    /* Offset: 0x004 (R/ ) STATUS Register */
  union {
    __IM   uint32_t ERROR;                   /* Offset: 0x008 (R/ ) Error Status Register */
    __OM   uint32_t ERRORCLR;                /* Offset: 0x008 ( /W) Error Clear Register */
  };
  __IOM  uint32_t DIVIDE;                    /* Offset: 0x00C (R/W) Divide ratio Register */
  __OM   uint32_t TXBUF;                     /* Offset: 0x010 ( /W) Transmit Buffer */
  __IM   uint32_t RXBUF;                     /* Offset: 0x014 (R/ ) Receive Buffer */
         uint32_t RESERVED1[186];
  __IOM  uint32_t ITCR;                      /* Offset: 0x300 (R/W) Integration Test Control Register */
  __OM   uint32_t ITIP1;                     /* Offset: 0x304 ( /W) Integration Test Input Register 1 */
  __OM   uint32_t ITOP1;                     /* Offset: 0x308 ( /W) Integration Test Output Register 1 */
} I2S_TypeDef;

/* I2S_CONTROL Register Definitions */
#define I2S_CONTROL_CODECRST_Pos         17                                              /* I2S CONTROL.CODECRST: Codec reset position */
#define I2S_CONTROL_CODECRST_Msk         (1UL << I2S_CONTROL_CODECRST_Pos)               /* I2S CONTROL.CODECRST: Codec reset mask */

#define I2S_CONTROL_FIFORST_Pos          16                                              /* I2S CONTROL.FIFORST: FIFO reset position */
#define I2S_CONTROL_FIFORST_Msk          (1UL << I2S_CONTROL_FIFORST_Pos)                /* I2S CONTROL.FIFORST: FIFO reset mask */

#define I2S_CONTROL_RXWLVL_Pos           12                                              /* I2S CONTROL.RXWLVL: RX Buffer IRQ Water Level position */
#define I2S_CONTROL_RXWLVL_Msk           (7UL << I2S_CONTROL_RXWLVL_Pos)                 /* I2S CONTROL.RXWLVL: RX Buffer IRQ Water Level mask */

#define I2S_CONTROL_TXWLVL_Pos            8                                              /* I2S CONTROL.TXWLVL: TX Buffer IRQ Water Level position */
#define I2S_CONTROL_TXWLVL_Msk           (7UL << I2S_CONTROL_TXWLVL_Pos)                 /* I2S CONTROL.TXWLVL: TX Buffer IRQ Water Level mask */

#define I2S_CONTROL_RXINTEN_Pos           3                                              /* I2S CONTROL.RXINTEN: RX interrupt enable position */
#define I2S_CONTROL_RXINTEN_Msk          (1UL << I2S_CONTROL_RXINTEN_Pos)                /* I2S CONTROL.RXINTEN: RX interrupt enable mask */

#define I2S_CONTROL_RXEN_Pos              2                                              /* I2S CONTROL.RXEN: RX enable position */
#define I2S_CONTROL_RXEN_Msk             (1UL << I2S_CONTROL_RXEN_Pos)                   /* I2S CONTROL.RXEN: RX enable mask */

#define I2S_CONTROL_TXINTEN_Pos           1                                              /* I2S CONTROL.TXINTEN: TX interrupt enable position */
#define I2S_CONTROL_TXINTEN_Msk          (1UL << I2S_CONTROL_TXINTEN_Pos)                /* I2S CONTROL.TXINTEN: TX interrupt enable mask */

#define I2S_CONTROL_TXEN_Pos              0                                              /* I2S CONTROL.TXEN: TX enable position */
#define I2S_CONTROL_TXEN_Msk             (1UL /*<< I2S_CONTROL_TXEN_Pos*/)               /* I2S CONTROL.TXEN: TX enable mask */

/* I2S_STATUS Register Definitions */
#define I2S_STATUS_RXFULL_Pos             5                                              /* I2S STATUS.RXFULL: RX Buffer Full position */
#define I2S_STATUS_RXFULL_Msk            (1UL << I2S_STATUS_RXFULL_Pos)                  /* I2S STATUS.RXFULL: RX Buffer Full mask */

#define I2S_STATUS_RXEMPTY_Pos            4                                              /* I2S STATUS.RXEMPTY: RX Buffer Empty position */
#define I2S_STATUS_RXEMPTY_Msk           (1UL << I2S_STATUS_RXEMPTY_Pos)                 /* I2S STATUS.RXEMPTY: RX Buffer Empty mask */

#define I2S_STATUS_TXFULL_Pos             3                                              /* I2S STATUS.TXFULL: TX Buffer Full position */
#define I2S_STATUS_TXFULL_Msk            (1UL << I2S_STATUS_TXFULL_Pos)                  /* I2S STATUS.TXFULL: TX Buffer Full mask */

#define I2S_STATUS_TXEMPTY_Pos            2                                              /* I2S STATUS.TXEMPTY: TX Buffer Empty position */
#define I2S_STATUS_TXEMPTY_Msk           (1UL << I2S_STATUS_TXEMPTY_Pos)                 /* I2S STATUS.TXEMPTY: TX Buffer Empty mask */

#define I2S_STATUS_RXINT_Pos              1                                              /* I2S STATUS.RXINT: RX Buffer Alert position */
#define I2S_STATUS_RXINT_Msk             (1UL << I2S_STATUS_RXINT_Pos)                   /* I2S STATUS.RXINT: RX Buffer Alert mask */

#define I2S_STATUS_TXINT_Pos              0                                              /* I2S STATUS.TXINT: TX Buffer Alert position */
#define I2S_STATUS_TXINT_Msk             (1UL /*<< I2S_STATUS_TXINT_Pos*/)               /* I2S STATUS.TXINT: TX Buffer Alert mask */

/* I2S_ERROR Register Definitions */
#define I2S_ERROR_RXERR_Pos               1                                              /* I2S ERROR.RXERR: RX overrun position */
#define I2S_ERROR_RXERR_Msk              (1UL << I2S_ERROR_RXERR_Pos)                    /* I2S ERROR.RXERR: RX overrun mask */

#define I2S_ERROR_TXERR_Pos               0                                              /* I2S ERROR.TXERR: TX overrun or underrun position */
#define I2S_ERROR_TXERR_Msk              (1UL /*<< I2S_ERROR_TXERR_Pos*/)                /* I2S ERROR.TXERR: TX overrun or underrun mask */


/*------------------- I2C ----------------------------------------------*/
typedef struct /* see "?" */
{
  union {
    __OM  uint32_t CONTROLS;                 /* Offset: 0x000 ( /W)  CONTROL Set Register */
    __IM  uint32_t CONTROL;                  /* Offset: 0x000 (R/ )  CONTROL Status Register */
  };
  __OM  uint32_t CONTROLC;                   /* Offset: 0x004 ( /W)  CONTROL Clear Register */
} I2C_TypeDef;

/* I2C_CONTROLS Register Definitions */
#define I2C_CONTROLS_SDA_Pos              1                                              /* I2C CONTROLS.SDA: SDA position */
#define I2C_CONTROLS_SDA_Msk             (1UL << I2C_CONTROLS_SDA_Pos)                   /* I2C CONTROLS.SDA: SDA mask */

#define I2C_CONTROLS_SCL_Pos              0                                              /* I2C CONTROLS.SCL: SCL position */
#define I2C_CONTROLS_SCL_Msk             (1UL /*<< I2C_CONTROLS_SCL_Pos*/)               /* I2C CONTROLS.SCL: SCL mask */

/* I2C_CONTROL Register Definitions */
#define I2C_CONTROL_SDA_Pos               1                                              /* I2C CONTROL.SDA: SDA position */
#define I2C_CONTROL_SDA_Msk              (1UL << I2C_CONTROL_SDA_Pos)                    /* I2C CONTROL.SDA: SDA mask */

#define I2C_CONTROL_SCL_Pos               0                                              /* I2C CONTROL.SCL: SCL position */
#define I2C_CONTROL_SCL_Msk              (1UL /*<< I2C_CONTROL_SCL_Pos*/)                /* I2C CONTROL.SCL: SCL mask */

/* I2C_CONTROLC Register Definitions */
#define I2C_CONTROLC_SDA_Pos              1                                              /* I2C CONTROLC.SDA: SDA position */
#define I2C_CONTROLC_SDA_Msk             (1UL << I2C_CONTROLC_SDA_Pos)                   /* I2C CONTROLC.SDA: SDA mask */

#define I2C_CONTROLC_SCL_Pos              0                                              /* I2C CONTROLC.SCL: SCL position */
#define I2C_CONTROLC_SCL_Msk             (1UL /*<< I2C_CONTROLC_SCL_Pos*/)               /* I2C CONTROLC.SCL: SCL mask */


/*------------------- PrimeCell Synchronous Serial Port (PL022) ----------------*/
typedef struct /* see "ARM PrimeCell Synchronous Serial Port (PL022) Technical Reference Manual r1p4" */
{
  __IOM uint32_t CR0;                        /* Offset: 0x000 (R/W)  Control register 0 */
  __IOM uint32_t CR1;                        /* Offset: 0x004 (R/W)  Control register 1 */
  __IOM uint32_t DR;                         /* Offset: 0x008 (R/W)  Data register */
  __IM  uint32_t SR;                         /* Offset: 0x00C (R/ )  Status register */
  __IOM uint32_t CPSR;                       /* Offset: 0x010 (R/W)  Clock prescale register */
  __IOM uint32_t IMSC;                       /* Offset: 0x014 (R/W)  Interrupt mask set or clear register */
  __IM  uint32_t RIS;                        /* Offset: 0x018 (R/ )  Raw interrupt status register */
  __IM  uint32_t MIS;                        /* Offset: 0x01C (R/ )  masked interrupt status register */
  __OM  uint32_t ICR;                        /* Offset: 0x020 ( /W)  Interrupt clear register */
  __IOM uint32_t DMACR;                      /* Offset: 0x024 (R/W)  DMA control register */
} SSP_TypeDef;


/*------------------- PrimeCell Real Time Clock (PL031) ------------------------*/
typedef struct /* see "ARM PrimeCell Real Time Clock (PL031) Technical Reference Manual" */
{
  __IM  uint32_t DR;                         /* Offset: 0x000 (R/ )  Data register */
  __IOM uint32_t MR;                         /* Offset: 0x004 (R/W)  Match register */
  __IOM uint32_t LR;                         /* Offset: 0x008 (R/W)  Load register */
  __IOM uint32_t CR;                         /* Offset: 0x00C (R/W)  Control register */
  __IOM uint32_t IMSC;                       /* Offset: 0x010 (R/W)  Interrupt mask set and clear register */
  __OM  uint32_t RIS;                        /* Offset: 0x014 (R/ )  Raw interrupt status register */
  __OM  uint32_t MIS;                        /* Offset: 0x018 (R/ )  Masked interrupt status register */
  __IOM uint32_t ICR;                        /* Offset: 0x01C ( /W)  Interrupt clear register */
} RTC_TypeDef;


/*------------------- SMSC9220 (Ethernet) ------------------------------------*/
typedef struct /* see "?" */
{
  __IM   uint32_t RX_DATA_PORT;              /* Offset: 0x000 (R/ ) Receive FIFO Ports */
         uint32_t RESERVED0[7];
  __OM   uint32_t TX_DATA_PORT;              /* Offset: 0x020 ( /W) Transmit FIFO Ports */
         uint32_t RESERVED1[7];
  __IM   uint32_t RX_STAT_PORT;              /* Offset: 0x040 (R/ ) Receive FIFO status port */
  __IM   uint32_t RX_STAT_PEEK;              /* Offset: 0x044 (R/ ) Receive FIFO status peek */
  __IM   uint32_t TX_STAT_PORT;              /* Offset: 0x048 (R/ ) Transmit FIFO status port */
  __IM   uint32_t TX_STAT_PEEK;              /* Offset: 0x04C (R/ ) Transmit FIFO status peek */
  __IM   uint32_t ID_REV;                    /* Offset: 0x050 (R/ ) Chip ID and Revision */
  __IOM  uint32_t IRQ_CFG;                   /* Offset: 0x054 (R/W) Main Interrupt Configuration */
  __IOM  uint32_t INT_STS;                   /* Offset: 0x058 (R/W) Interrupt Status */
  __IOM  uint32_t INT_EN;                    /* Offset: 0x05C (R/W) Interrupt Enable Register */
         uint32_t RESERVED3;                 /* Offset: 0x060 (R/W) Reserved for future use */
  __IM   uint32_t BYTE_TEST;                 /* Offset: 0x064 (R/ ) Read-only byte order testing register 87654321h */
  __IOM  uint32_t FIFO_INT;                  /* Offset: 0x068 (R/W) FIFO Level Interrupts */
  __IOM  uint32_t RX_CFG;                    /* Offset: 0x06C (R/W) Receive Configuration */
  __IOM  uint32_t TX_CFG;                    /* Offset: 0x070 (R/W) Transmit Configuration */
  __IOM  uint32_t HW_CFG;                    /* Offset: 0x074 (R/W) Hardware Configuration */
  __IOM  uint32_t RX_DP_CTL;                 /* Offset: 0x078 (R/W) RX Datapath Control */
  __IM   uint32_t RX_FIFO_INF;               /* Offset: 0x07C (R/ ) Receive FIFO Information */
  __IM   uint32_t TX_FIFO_INF;               /* Offset: 0x080 (R/W) Transmit FIFO Information */
  __IOM  uint32_t PMT_CTRL;                  /* Offset: 0x084 (R/W) Power Management Control */
  __IOM  uint32_t GPIO_CFG;                  /* Offset: 0x088 (R/W) General Purpose IO Configuration */
  __IOM  uint32_t GPT_CFG;                   /* Offset: 0x08C (R/W) General Purpose Timer Configuration */
  __IM   uint32_t GPT_CNT;                   /* Offset: 0x090 (R/W) General Purpose Timer Count */
         uint32_t RESERVED4;                 /* Offset: 0x094 (R/W) Reserved for future use */
  __IOM  uint32_t ENDIAN;                    /* Offset: 0x098 (R/W) WORD SWAP Register */
  __IM   uint32_t FREE_RUN;                  /* Offset: 0x09C (R/W) Free Run Counter */
  __IM   uint32_t RX_DROP;                   /* Offset: 0x0A0 (R/W) RX Dropped Frames Counter */
  __IOM  uint32_t MAC_CSR_CMD;               /* Offset: 0x0A4 (R/W) MAC CSR Synchronizer Command */
  __IOM  uint32_t MAC_CSR_DATA;              /* Offset: 0x0A8 (R/W) MAC CSR Synchronizer Data */
  __IOM  uint32_t AFC_CFG;                   /* Offset: 0x0AC (R/W) Automatic Flow Control Configuration */
  __IOM  uint32_t E2P_CMD;                   /* Offset: 0x0B0 (R/W) EEPROM Command */
  __IOM  uint32_t E2P_DATA;                  /* Offset: 0x0B4 (R/W) EEPROM Data */
} ETH_TypeDef;


/*------------------- Color LCD (CLCD) ------------------------*/
typedef struct /* see "?" */
{
  __IOM uint32_t COM;                        /* Offset: 0x000 (R/W)   */
  __IOM uint32_t DAT;                        /* Offset: 0x004 (R/W)   */
  __IOM uint32_t RD;                         /* Offset: 0x008 (R/W)   */
  __IOM uint32_t RAW;                        /* Offset: 0x00C (R/W)   */
  __IOM uint32_t MASK;                       /* Offset: 0x010 (R/W)   */
  __IOM uint32_t STAT;                       /* Offset: 0x014 (R/W)   */
        uint32_t RESERVED0[13];              /* Offset: 0x018 (R/W)   */
  __IOM uint32_t MISC;                       /* Offset: 0x04C (R/W)   */
} CLCD_TypeDef;

/* CLDC_MISC Register Definitions */
#define CLCD_MISC_CLCDBLCTRL_Pos          6                                              /* CLCD MISC.CLCDBLCTRL: CLCD_BL_CTRL position */
#define CLCD_MISC_CLCDBLCTRL_Msk         (1UL << CLCD_MISC_CLCDBLCTRL_Pos)               /* CLCD MISC.CLCDBLCTRL: CLCD_BL_CTRL mask */

#define CLCD_MISC_CLCDRD_Pos              5                                              /* CLCD MISC.CLCDRD: CLCD_RD position */
#define CLCD_MISC_CLCDRD_Msk             (1UL << CLCD_MISC_CLCDRD_Pos)                   /* CLCD MISC.CLCDRD: CLCD_RD mask */

#define CLCD_MISC_CLCDRS_Pos              4                                              /* CLCD MISC.CLCDRS: CLCD_RS position */
#define CLCD_MISC_CLCDRS_Msk             (1UL << CLCD_MISC_CLCDRS_Pos)                   /* CLCD MISC.CLCDRS: CLCD_RS mask */

#define CLCD_MISC_CLCDRESET_Pos           3                                              /* CLCD MISC.CLCDRESET: CLCD_RESET position */
#define CLCD_MISC_CLCDRESET_Msk          (1UL << CLCD_MISC_CLCDRESET_Pos)                /* CLCD MISC.CLCDRESET: CLCD_RESET mask */

#define CLCD_MISC_CLCDCS_Pos              0                                              /* CLCD MISC.CLCDCS: CLCD_CS position */
#define CLCD_MISC_CLCDCS_Msk             (1UL /*<< CLCD_MISC_CLCDCS_Pos*/)               /* CLCD MISC.CLCDCS: CLCD_CS mask */


/*------------------- Secure Privilege Control Block -----------------------------*/
typedef struct /* see "ARM CoreLink SSE-200 Subsystem Technical Reference Manual r1p1" */
{
  __IOM  uint32_t SPCSECCTRL;               /* Offset: 0x000 (R/W) Secure Privilege Controller Secure Configuration Control Register */
  __IOM  uint32_t BUSWAIT;                  /* Offset: 0x004 (R/W) Parameterized Bus Access wait control after reset */
         uint32_t RESERVED0[2U];
  __IOM  uint32_t SECRESPCFG;               /* Offset: 0x010 (R/W) Security Violation Response Configuration Register */
  __IOM  uint32_t NSCCFG;                   /* Offset: 0x014 (R/W) Non Secure Callable Configuration for IDAU */
         uint32_t RESERVED1[1U];
  __IM   uint32_t SECMPCINTSTATUS;          /* Offset: 0x01C (R/ ) Secure MPC Interrupt Status */
  __IM   uint32_t SECPPCINTSTAT;            /* Offset: 0x020 (R/ ) Secure PPC Interrupt Status */
  __OM   uint32_t SECPPCINTCLR;             /* Offset: 0x024 ( /W) Secure PPC Interrupt Clear */
  __IOM  uint32_t SECPPCINTEN;              /* Offset: 0x028 (R/W) Secure PPC Interrupt Enable */
         uint32_t RESERVED2[1U];
  __IM   uint32_t SECMSCINTSTAT;            /* Offset: 0x030 (R/ ) Secure MSC Interrupt Status */
  __OM   uint32_t SECMSCINTCLR;             /* Offset: 0x034 ( /W) Secure MSC Interrupt Clear */
  __IOM  uint32_t SECMSCINTEN;              /* Offset: 0x038 (R/W) Secure MSC Interrupt Enable */
         uint32_t RESERVED3[1U];
  __IM   uint32_t BRGINTSTAT;               /* Offset: 0x040 (R/ ) Bridge Buffer Error Interrupt Status */
  __OM   uint32_t BRGINTCLR;                /* Offset: 0x044 ( /W) Bridge Buffer Error Interrupt Clear */
  __IOM  uint32_t BRGINTEN;                 /* Offset: 0x048 (R/W) Bridge Buffer Error Interrupt Enable */
         uint32_t RESERVED4[1U];
  __IOM  uint32_t AHBNSPPC[4U];             /* Offset: 0x050 (R/W) Non-Secure Access AHB slave Peripheral Protection Control */
  __IOM  uint32_t AHBNSPPCEXP[4U];          /* Offset: 0x060 (R/W) Expansion Non_Secure Access AHB slave Peripheral Protection Control */
  __IOM  uint32_t APBNSPPC[4U];             /* Offset: 0x070 (R/W) Non-Secure Access APB slave Peripheral Protection Control */
  __IOM  uint32_t APBNSPPCEXP[4U];          /* Offset: 0x080 (R/W) Expansion Non_Secure Access APB slave Peripheral Protection Control */
  __IM   uint32_t AHBSPPPC[4U];             /* Offset: 0x090 (R/ ) Secure Unprivileged Access AHB slave Peripheral Protection Control */
  __IOM  uint32_t AHBSPPPCEXP[4U];          /* Offset: 0x0A0 (R/W) Expansion Secure Unprivileged Access AHB slave Peripheral Protection Control */
  __IOM  uint32_t APBSPPPC[4U];             /* Offset: 0x0B0 (R/W) Secure Unprivileged Access APB slave Peripheral Protection Control */
  __IOM  uint32_t APBSPPPCEXP[4U];          /* Offset: 0x0C0 (R/W) Expansion Secure Unprivileged Access APB slave Peripheral Protection Control */
  __IM   uint32_t NSMSCEXP;                 /* Offset: 0x0D0 (R/ ) Expansion MSC Non-Secure Configuration */
} SPCTRL_TypeDef;


/*------------------- Non-Secure Privilege Control Block -------------------------*/
typedef struct /* see "ARM CoreLink SSE-200 Subsystem Technical Reference Manual r1p1" */
{
         uint32_t RESERVED0[36U];
  __IOM  uint32_t AHBNSPPPC[4U];            /* Offset: 0x090 (R/W) Non-Secure Unprivileged Access AHB slave Peripheral Protection Control */
  __IOM  uint32_t AHBNSPPPCEXP[4U];         /* Offset: 0x0A0 (R/W) Expansion 0 Non-Secure Unprivileged Access AHB slave Peripheral Protection Control */
  __IOM  uint32_t APBNSPPPC[4U];            /* Offset: 0x0B0 (R/W) Non-Secure Unprivileged Access APB slave Peripheral Protection Control */
  __IOM  uint32_t APBNSPPPCEXP[4U];         /* Offset: 0x0C0 (R/W) Expansion 0 Non-Secure Unprivileged Access APB slave Peripheral Protection Control */
} NSPCTRL_TypeDef;


/*------------------- Memory Protection Controller -------------------------------*/
typedef struct /* see "ARM CoreLink SIE-200 System IP for Embedded r3p1" */
{
  __IOM  uint32_t CTRL;                     /* Offset: 0x000 (R/W) Control Register */
         uint32_t RESERVED0[3];
  __IM   uint32_t BLK_MAX;                  /* Offset: 0x010 (R/ ) Block Maximum Register */
  __IM   uint32_t BLK_CFG;                  /* Offset: 0x014 (R/ ) Block Configuration Register */
  __IOM  uint32_t BLK_IDX;                  /* Offset: 0x018 (R/W) Block Index Register */
  __IOM  uint32_t BLK_LUT;                  /* Offset: 0x01C (R/W) Block Lookup Table Register */
  __IM   uint32_t INT_STAT;                 /* Offset: 0x020 (R/ ) Interrupt Status Register */
  __OM   uint32_t INT_CLEAR;                /* Offset: 0x024 ( /W) Interrupt Clear Register */
  __IOM  uint32_t INT_EN;                   /* Offset: 0x028 (R/W) Interrupt Enable Register */
  __IM   uint32_t INT_INFO1;                /* Offset: 0x02C (R/ ) Interrupt Info1 Register */
  __IM   uint32_t INT_INFO2;                /* Offset: 0x030 (R/ ) Interrupt Info2 Register */
  __OM   uint32_t INT_SET;                  /* Offset: 0x034 ( /W) Interrupt Set Register */
} MPC_TypeDef;


/*------------------- TCM Gating Unit --------------------------------------------*/
typedef struct /* see "MPS3_CM55 Engineering Specification" */
{
  __IOM  uint32_t CTRL;                     /* Offset: 0x000 (R/W) Gate Control Register */
  __IM   uint32_t CFG;                      /* Offset: 0x004 (R/ ) Gate Configuration Register */
         uint32_t RESERVED0[2];
  __IOM  uint32_t LUT[16];                  /* Offset: 0x010 (R/W) Gate Look-up Table Register */
} TGU_TypeDef;

/* TGU_CTRL Register Definitions */
#define TGU_CTRL_DEREN_Pos                1                                              /* TGU CTRL.DEREN: Enable S-AHB error response for TGU fault */
#define TGU_CTRL_DEREN_Msk               (1UL << TGU_CTRL_DEREN_Pos)

#define TGU_CTRL_DBFEN_Pos                0                                              /* TGU CTRL.DBFEN: Enable Data access BusFault for TGU fault */
#define TGU_CTRL_DBFEN_Msk               (1UL /*<< TGU_CTRL_DBFEN_Pos*/)

/* TGU_CFG Register Definitions */
#define TGU_CFG_PRESENT_Pos              31                                              /* TGU CFG.PRESENT: TGU present */
#define TGU_CFG_PRESENT_Msk              (1UL << TGU_CFG_PRESENT_Pos)

#define TGU_CFG_NUMBLKS_Pos               8                                              /* TGU CFG.NUMBLKS: Number of blocks = 2^NUMBLKS */
#define TGU_CFG_NUMBLKS_Msk              (0xFUL << TGU_CFG_NUMBLKS_Pos)

#define TGU_CFG_BLKSZ_Pos                 0                                              /* TGU CFG.BLKSZ: Block Size in bytes = 2^(BLKSZ+5) */
#define TGU_CFG_BLKSZ_Msk                (0xFUL /*<< TGU_CFG_BLKSZ_Pos*/)


/* --------  End of section using anonymous unions and disabling warnings  -------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif




/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */
#define        TIMER0_BASE        (0x40000000UL)       /* CMSDK Timer 0 - NS-PPC */
#define        TIMER1_BASE        (0x40001000UL)       /* CMSDK Timer 1 - NS-PPC */
#define        DUALTIMER_BASE     (0x40002000UL)       /* CMSDK Dual Timer - NS-PPC */
#define        DUALTIMER_1_BASE   (0x40002000UL)       /* CMSDK Dual Timer timer 1 */
#define        DUALTIMER_2_BASE   (0x40002020UL)       /* CMSDK Dual Timer timer 2 */
#define        NSPCTRL_BASE       (0x40080000UL)       /* Non-secure Privilege Control Block - NSP */
#define        WATCHDOG_BASE      (0x40081000UL)       /* Non-secure CMSDK Watchdog - NSP */
#define        GPIO0_BASE         (0x41100000UL)       /* GPIO 0 */
#define        GPIO1_BASE         (0x41101000UL)       /* GPIO 1 */
#define        GPIO2_BASE         (0x41102000UL)       /* GPIO 2 */
#define        GPIO3_BASE         (0x41103000UL)       /* GPIO 3 */
#define        I2C0_BASE          (0x41200000UL)       /* FPGA - SBCon I2C (Touch) */
#define        I2C1_BASE          (0x41201000UL)       /* FPGA - SBCon I2C (Audio Conf) */
#define        SPI0_BASE          (0x41202000UL)       /* FPGA - PL022 (SPI ADC) */
#define        SPI1_BASE          (0x41203000UL)       /* FPGA - PL022 (SPI Shield0) */
#define        SPI2_BASE          (0x41204000UL)       /* FPGA - PL022 (SPI Shield1) */
#define        I2C2_BASE          (0x41205000UL)       /* SBCon (I2C - Shield0) */
#define        I2C3_BASE          (0x41206000UL)       /* SBCon (I2C – Shield1) */
#define        I2C4_BASE          (0x41208000UL)       /* FPGA - SBCon I2C (DDR4 EEPROM) */
#define        SCC_BASE           (0x41300000UL)       /* FPGA - SCC registers */
#define        I2S0_BASE          (0x41301000UL)       /* FPGA - I2S (Audio) */
#define        FPGAIO_BASE        (0x41302000UL)       /* FPGA - IO (System Ctrl + I/O) */
#define        UART0_BASE         (0x41303000UL)       /* UART0 - UART_F[0] */
#define        UART1_BASE         (0x41304000UL)       /* UART1 - UART_F[1] */
#define        UART2_BASE         (0x41305000UL)       /* UART2 - UART_F[2] */
#define        UART3_BASE         (0x41306000UL)       /* UART3 - UART Shield 0 */
#define        UART4_BASE         (0x41307000UL)       /* UART4 - UART Shield 1 */
#define        UART5_BASE         (0x41308000UL)       /* UART5 - UART_F[3] */
#define        PDM_BASE           (0x41309000UL)       /* PDM */
#define        CLCD_BASE          (0x4130A000UL)       /* Color LCD */
#define        RTC_BASE           (0x4130B000UL)       /* RTC */
#define        ETH_BASE           (0x41400000UL)       /* Ethernet */
#define        USB_BASE           (0x41500000UL)       /* USB */


/* ================================================================================ */
/* ================          Secure Peripheral memory map          ================ */
/* ================================================================================ */
#define SECURE_TIMER0_BASE        (0x50000000UL)       /* CMSDK Timer- S-PPC */
#define SECURE_TIMER1_BASE        (0x50001000UL)       /* CMSDK Timer - S-PPC */
#define SECURE_DUALTIMER_BASE     (0x50002000UL)       /* CMSDK Dual Timer - S-PPC */
#define SECURE_DUALTIMER_1_BASE   (0x50002000UL)       /* CMSDK Dual Timer timer 1 */
#define SECURE_DUALTIMER_2_BASE   (0x50002020UL)       /* CMSDK Dual Timer timer 2 */
#define        SPCTRL_BASE        (0x50080000UL)       /* Secure Privilege Control Block - SP */
#define SECURE_WATCHDOG_BASE      (0x50081000UL)       /* Secure CMSDK Watchdog - SP */
#define SECURE_GPIO0_BASE         (0x51100000UL)       /* GPIO 0 */
#define SECURE_GPIO1_BASE         (0x51101000UL)       /* GPIO 1 */
#define SECURE_GPIO2_BASE         (0x51102000UL)       /* GPIO 2 */
#define SECURE_GPIO3_BASE         (0x51103000UL)       /* GPIO 3 */
#define SECURE_I2C0_BASE          (0x51200000UL)       /* FPGA - SBCon I2C (Touch) */
#define SECURE_I2C1_BASE          (0x51201000UL)       /* FPGA - SBCon I2C (Audio Conf) */
#define SECURE_SPI0_BASE          (0x51202000UL)       /* FPGA - PL022 (SPI ADC) */
#define SECURE_SPI1_BASE          (0x51203000UL)       /* FPGA - PL022 (SPI Shield0) */
#define SECURE_SPI2_BASE          (0x51204000UL)       /* FPGA - PL022 (SPI Shield1) */
#define SECURE_I2C2_BASE          (0x51205000UL)       /* SBCon (I2C - Shiled0) */
#define SECURE_I2C3_BASE          (0x51206000UL)       /* SBCon (I2C - Shiled1) */
#define SECURE_I2C4_BASE          (0x51208000UL)       /* FPGA - SBCon I2C (DDR4 EEPROM) */
#define SECURE_SCC_BASE           (0x51300000UL)       /* FPGA - SCC registers */
#define SECURE_I2S0_BASE          (0x51301000UL)       /* FPGA - I2S (Audio) */
#define SECURE_FPGAIO_BASE        (0x51302000UL)       /* FPGA - IO (System Ctrl + I/O) */
#define SECURE_UART0_BASE         (0x51303000UL)       /* UART0 - UART_F[0] */
#define SECURE_UART1_BASE         (0x51304000UL)       /* UART1 - UART_F[1] */
#define SECURE_UART2_BASE         (0x51305000UL)       /* UART2 - UART_F[2] */
#define SECURE_UART3_BASE         (0x51306000UL)       /* UART3 - UART Shield 0 */
#define SECURE_UART4_BASE         (0x51307000UL)       /* UART4 - UART Shield 1 */
#define SECURE_UART5_BASE         (0x51308000UL)       /* UART5 - UART_F[3] */
#define SECURE_PDM_BASE           (0x51309000UL)       /* PDM */
#define SECURE_CLCD_BASE          (0x5130A000UL)       /* Color LCD */
#define SECURE_RTC_BASE           (0x5130B000UL)       /* RTC */
#define SECURE_ETH_BASE           (0x51400000UL)       /* Ethernet */
#define SECURE_USB_BASE           (0x51500000UL)       /* USB */
#define        MPC_SSRAM_BASE     (0x58007000UL)       /* SSRAM Memory Protection Controller (MPC) */
#define        MPC_QSPI_BASE      (0x58008000UL)       /* QSPI Memory Protection Controller (MPC) */
#define        MPC_DDR4_BASE      (0x58009000UL)       /* DDR4 Memory Protection Controller (MPC) */
#define        ITGU_BASE          (0xE001E500UL)       /* ITCM Gating Unit */
#define        DTGU_BASE          (0xE001E600UL)       /* DTCM Gating Unit */


/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */
#define        TIMER0        ((TIMER_TypeDef            *)        TIMER0_BASE)
#define        TIMER1        ((TIMER_TypeDef            *)        TIMER1_BASE)
#define        DUALTIMER     ((DUALTIMER_TypeDef        *)        DUALTIMER_BASE)
#define        DUALTIMER1    ((DUALTIMER_SINGLE_TypeDef *)        DUALTIMER_1_BASE)
#define        DUALTIMER2    ((DUALTIMER_SINGLE_TypeDef *)        DUALTIMER_2_BASE)
#define        WATCHDOG      ((WATCHDOG_TypeDef         *)        WATCHDOG_BASE)
#define        NSPCTRL       ((NSPCTRL_TypeDef          *)        NSPCTRL_BASE)
#define        WATCHDOG      ((WATCHDOG_TypeDef         *)        WATCHDOG_BASE)
#define        GPIO0         ((GPIO_TypeDef             *)        GPIO0_BASE)
#define        GPIO1         ((GPIO_TypeDef             *)        GPIO1_BASE)
#define        GPIO2         ((GPIO_TypeDef             *)        GPIO2_BASE)
#define        GPIO3         ((GPIO_TypeDef             *)        GPIO3_BASE)
#define        I2C0          ((I2C_TypeDef              *)        I2C0_BASE)
#define        I2C1          ((I2C_TypeDef              *)        I2C1_BASE)
#define        SPI0          ((SPI_TypeDef              *)        SPI0_BASE)
#define        SPI1          ((SPI_TypeDef              *)        SPI1_BASE)
#define        SPI2          ((SPI_TypeDef              *)        SPI2_BASE)
#define        I2C2          ((I2C_TypeDef              *)        I2C2_BASE)
#define        I2C3          ((I2C_TypeDef              *)        I2C3_BASE)
#define        I2C4          ((I2C_TypeDef              *)        I2C4_BASE)
#define        SCC           ((SCC_TypeDef              *)        SCC_BASE)
#define        I2S0          ((I2S_TypeDef              *)        I2S0_BASE)
#define        FPGAIO        ((FPGAIO_TypeDef           *)        FPGAIO_BASE)
#define        UART0         ((UART_TypeDef             *)        UART0_BASE)
#define        UART1         ((UART_TypeDef             *)        UART1_BASE)
#define        UART2         ((UART_TypeDef             *)        UART2_BASE)
#define        UART3         ((UART_TypeDef             *)        UART3_BASE)
#define        UART4         ((UART_TypeDef             *)        UART4_BASE)
#define        UART5         ((UART_TypeDef             *)        UART5_BASE)
//#define        PDM           ((PDM_TypeDef              *)        PDM_BASE)
#define        CLCD          ((CLCD_TypeDef             *)        CLCD_BASE)
#define        RTC           ((RTC_TypeDef              *)        RTC_BASE)
#define        ETH           ((ETH_TypeDef              *)        ETH_BASE)
#define        USB           ((USB_TypeDef              *)        USB_BASE)


#define SECURE_TIMER0        ((TIMER_TypeDef            *) SECURE_TIMER0_BASE)
#define SECURE_TIMER1        ((TIMER_TypeDef            *) SECURE_TIMER1_BASE)
#define SECURE_DUALTIMER     ((DUALTIMER_TypeDef        *) SECURE_DUALTIMER_BASE)
#define SECURE_DUALTIMER1    ((DUALTIMER_SINGLE_TypeDef *) SECURE_DUALTIMER_1_BASE)
#define SECURE_DUALTIMER2    ((DUALTIMER_SINGLE_TypeDef *) SECURE_DUALTIMER_2_BASE)
#define        SPCTRL        ((SPCTRL_TypeDef           *)        SPCTRL_BASE)
#define SECURE_WATCHDOG      ((WATCHDOG_TypeDef         *) SECURE_WATCHDOG_BASE)
#define SECURE_GPIO0         ((GPIO_TypeDef             *) SECURE_GPIO0_BASE)
#define SECURE_GPIO1         ((GPIO_TypeDef             *) SECURE_GPIO1_BASE)
#define SECURE_GPIO2         ((GPIO_TypeDef             *) SECURE_GPIO2_BASE)
#define SECURE_GPIO3         ((GPIO_TypeDef             *) SECURE_GPIO3_BASE)
#define SECURE_I2C0          ((I2C_TypeDef              *) SECURE_I2C0_BASE)
#define SECURE_I2C1          ((I2C_TypeDef              *) SECURE_I2C1_BASE)
#define SECURE_SPI0          ((SPI_TypeDef              *) SECURE_SPI0_BASE)
#define SECURE_SPI1          ((SPI_TypeDef              *) SECURE_SPI1_BASE)
#define SECURE_SPI2          ((SPI_TypeDef              *) SECURE_SPI2_BASE)
#define SECURE_I2C2          ((I2C_TypeDef              *) SECURE_I2C2_BASE)
#define SECURE_I2C3          ((I2C_TypeDef              *) SECURE_I2C3_BASE)
#define SECURE_I2C4          ((I2C_TypeDef              *) SECURE_I2C4_BASE)
#define SECURE_SCC           ((SCC_TypeDef              *) SECURE_SCC_BASE)
#define SECURE_I2S0          ((I2S_TypeDef              *) SECURE_I2S0_BASE)
#define SECURE_FPGAIO        ((FPGAIO_TypeDef           *) SECURE_FPGAIO_BASE)
#define SECURE_UART0         ((UART_TypeDef             *) SECURE_UART0_BASE)
#define SECURE_UART1         ((UART_TypeDef             *) SECURE_UART1_BASE)
#define SECURE_UART2         ((UART_TypeDef             *) SECURE_UART2_BASE)
#define SECURE_UART3         ((UART_TypeDef             *) SECURE_UART3_BASE)
#define SECURE_UART4         ((UART_TypeDef             *) SECURE_UART4_BASE)
#define SECURE_UART5         ((UART_TypeDef             *) SECURE_UART5_BASE)
//#define SECURE_PDM           ((PDM_TypeDef              *) SECURE_PDM_BASE)
#define SECURE_CLCD          ((CLCD_TypeDef             *) SECURE_CLCD_BASE)
#define SECURE_RTC           ((RTC_TypeDef              *) SECURE_RTC_BASE)
#define SECURE_ETH           ((ETH_TypeDef              *) SECURE_ETH_BASE)
#define SECURE_USB           ((USB_TypeDef              *) SECURE_USB_BASE)
#define       MPC_SSRAM      ((MPC_TypeDef              *)        MPC_SSRAM_BASE)
#define       MPC_QSPI       ((MPC_TypeDef              *)        MPC_QSPI_BASE)
#define       MPC_DDR4       ((MPC_TypeDef              *)        MPC_DDR4_BASE)
#define       ITGU           ((TGU_TypeDef              *)        ITGU_BASE)
#define       DTGU           ((TGU_TypeDef              *)        DTGU_BASE)


#ifdef __cplusplus
}
#endif

#endif  /* MPS3_CM55_H */
