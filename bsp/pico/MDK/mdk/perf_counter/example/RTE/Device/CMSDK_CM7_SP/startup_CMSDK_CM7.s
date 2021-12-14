;/**************************************************************************//**
; * @file     startup_CMSDK_CM7.s
; * @brief    CMSIS Core Device Startup File for
; *           CMSDK_CM7 Device
; * @version  V3.05
; * @date     09. November 2016
; ******************************************************************************/
;/* Copyright (c) 2011 - 2016 ARM LIMITED
;
;   All rights reserved.
;   Redistribution and use in source and binary forms, with or without
;   modification, are permitted provided that the following conditions are met:
;   - Redistributions of source code must retain the above copyright
;     notice, this list of conditions and the following disclaimer.
;   - Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the
;     documentation and/or other materials provided with the distribution.
;   - Neither the name of ARM nor the names of its contributors may be used
;     to endorse or promote products derived from this software without
;     specific prior written permission.
;   *
;   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
;   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
;   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
;   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
;   POSSIBILITY OF SUCH DAMAGE.
;   ---------------------------------------------------------------------------*/
;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000C00

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp               ;     Top of Stack
                DCD     Reset_Handler              ;     Reset Handler
                DCD     NMI_Handler                ;     NMI Handler
                DCD     HardFault_Handler          ;     Hard Fault Handler
                DCD     MemManage_Handler          ;     MPU Fault Handler
                DCD     BusFault_Handler           ;     Bus Fault Handler
                DCD     UsageFault_Handler         ;     Usage Fault Handler
                DCD     0                          ;     Reserved
                DCD     0                          ;     Reserved
                DCD     0                          ;     Reserved
                DCD     0                          ;     Reserved
                DCD     SVC_Handler                ;     SVCall Handler
                DCD     DebugMon_Handler           ;     Debug Monitor Handler
                DCD     0                          ;     Reserved
                DCD     PendSV_Handler             ;     PendSV Handler
                DCD     SysTick_Handler            ;     SysTick Handler

                ; External Interrupts
                DCD     UART0RX_Handler            ;   0 UART 0 receive interrupt
                DCD     UART0TX_Handler            ;   1 UART 0 transmit interrupt
                DCD     UART1RX_Handler            ;   2 UART 1 receive interrupt
                DCD     UART1TX_Handler            ;   3 UART 1 transmit interrupt
                DCD     UART2RX_Handler            ;   4 UART 2 receive interrupt
                DCD     UART2TX_Handler            ;   5 UART 2 transmit interrupt
                DCD     GPIO0ALL_Handler           ;   6 GPIO 0 combined interrupt
                DCD     GPIO1ALL_Handler           ;   7 GPIO 1 combined interrupt
                DCD     TIMER0_Handler             ;   8 Timer 0 interrupt
                DCD     TIMER1_Handler             ;   9 Timer 1 interrupt
                DCD     DUALTIMER_Handler          ;  10 Dual Timer interrupt
                DCD     SPI_0_1_Handler            ;  11 SPI #0, #1 interrupt
                DCD     UART_0_1_2_OVF_Handler     ;  12 UART overflow (0, 1 & 2) interrupt
                DCD     ETHERNET_Handler           ;  13 Ethernet interrupt
                DCD     I2S_Handler                ;  14 Audio I2S interrupt
                DCD     TOUCHSCREEN_Handler        ;  15 Touch Screen interrupt
                DCD     GPIO2_Handler              ;  16 GPIO 2 combined interrupt
                DCD     GPIO3_Handler              ;  17 GPIO 3 combined interrupt
                DCD     UART3RX_Handler            ;  18 UART 3 receive interrupt
                DCD     UART3TX_Handler            ;  19 UART 3 transmit interrupt
                DCD     UART4RX_Handler            ;  20 UART 4 receive interrupt
                DCD     UART4TX_Handler            ;  21 UART 4 transmit interrupt
                DCD     SPI_2_Handler              ;  22 SPI #2 interrupt
                DCD     SPI_3_4_Handler            ;  23 SPI #3, SPI #4 interrupt
                DCD     GPIO0_0_Handler            ;  24 GPIO 0 individual interrupt ( 0)
                DCD     GPIO0_1_Handler            ;  25 GPIO 0 individual interrupt ( 1)
                DCD     GPIO0_2_Handler            ;  26 GPIO 0 individual interrupt ( 2)
                DCD     GPIO0_3_Handler            ;  27 GPIO 0 individual interrupt ( 3)
                DCD     GPIO0_4_Handler            ;  28 GPIO 0 individual interrupt ( 4)
                DCD     GPIO0_5_Handler            ;  29 GPIO 0 individual interrupt ( 5)
                DCD     GPIO0_6_Handler            ;  30 GPIO 0 individual interrupt ( 6)
                DCD     GPIO0_7_Handler            ;  31 GPIO 0 individual interrupt ( 7)
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC
                EXPORT UART0RX_Handler            [WEAK]
                EXPORT UART0TX_Handler            [WEAK]
                EXPORT UART1RX_Handler            [WEAK]
                EXPORT UART1TX_Handler            [WEAK]
                EXPORT UART2RX_Handler            [WEAK]
                EXPORT UART2TX_Handler            [WEAK]
                EXPORT GPIO0ALL_Handler           [WEAK]
                EXPORT GPIO1ALL_Handler           [WEAK]
                EXPORT TIMER0_Handler             [WEAK]
                EXPORT TIMER1_Handler             [WEAK]
                EXPORT DUALTIMER_Handler          [WEAK]
                EXPORT SPI_0_1_Handler            [WEAK]
                EXPORT UART_0_1_2_OVF_Handler     [WEAK]
                EXPORT ETHERNET_Handler           [WEAK]
                EXPORT I2S_Handler                [WEAK]
                EXPORT TOUCHSCREEN_Handler        [WEAK]
                EXPORT GPIO2_Handler              [WEAK]
                EXPORT GPIO3_Handler              [WEAK]
                EXPORT UART3RX_Handler            [WEAK]
                EXPORT UART3TX_Handler            [WEAK]
                EXPORT UART4RX_Handler            [WEAK]
                EXPORT UART4TX_Handler            [WEAK]
                EXPORT SPI_2_Handler              [WEAK]
                EXPORT SPI_3_4_Handler            [WEAK]
                EXPORT GPIO0_0_Handler            [WEAK]
                EXPORT GPIO0_1_Handler            [WEAK]
                EXPORT GPIO0_2_Handler            [WEAK]
                EXPORT GPIO0_3_Handler            [WEAK]
                EXPORT GPIO0_4_Handler            [WEAK]
                EXPORT GPIO0_5_Handler            [WEAK]
                EXPORT GPIO0_6_Handler            [WEAK]
                EXPORT GPIO0_7_Handler            [WEAK]

UART0RX_Handler
UART0TX_Handler
UART1RX_Handler
UART1TX_Handler
UART2RX_Handler
UART2TX_Handler
GPIO0ALL_Handler
GPIO1ALL_Handler
TIMER0_Handler
TIMER1_Handler
DUALTIMER_Handler
SPI_0_1_Handler
UART_0_1_2_OVF_Handler
ETHERNET_Handler
I2S_Handler
TOUCHSCREEN_Handler
GPIO2_Handler
GPIO3_Handler
UART3RX_Handler
UART3TX_Handler
UART4RX_Handler
UART4TX_Handler
SPI_2_Handler
SPI_3_4_Handler
GPIO0_0_Handler
GPIO0_1_Handler
GPIO0_2_Handler
GPIO0_3_Handler
GPIO0_4_Handler
GPIO0_5_Handler
GPIO0_6_Handler
GPIO0_7_Handler
                B       .

                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END
