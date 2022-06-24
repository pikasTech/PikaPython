;/* ------------------------------------------------------------------------- */
;/*  @file:    startup_MM32F5277.s                                            */
;/*  @purpose: CMSIS Cortex-M0 Core Device Startup File                       */
;/*                                                                           */
;/*  @version: 1.0                                                            */
;/*  @date:    2022-03-03                                                     */
;/*  @build:   b220303                                                        */
;/* ------------------------------------------------------------------------- */
;/*                                                                           */
;/* Copyright 2022 MindMotion                                                 */
;/* All rights reserved.                                                      */
;/*                                                                           */
;/* SPDX-License-Identifier: BSD-3-Clause                                     */
;/*****************************************************************************/
;/* Version:  MDK for ARM Embedded Processors                                  */
;/*****************************************************************************/
                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
                IMPORT  |Image$$ARM_LIB_STACK$$ZI$$Limit|

__Vectors       DCD     |Image$$ARM_LIB_STACK$$ZI$$Limit|                       ;         Top of Stack
                DCD     Reset_Handler                                           ;         Reset Handler
                DCD     NMI_Handler                                             ; -14     NMI Handler
                DCD     HardFault_Handler                                       ; -13     Hard Fault Handler
                DCD     MemManage_Handler                                       ; -12     MPU Fault Handler
                DCD     BusFault_Handler                                        ; -11     Bus Fault Handler
                DCD     UsageFault_Handler                                      ; -10     Usage Fault Handler
                DCD    0                                                        ; -9  Reserved
                DCD    0                                                        ; -8  Reserved
                DCD    0                                                        ; -7  Reserved
                DCD    0                                                        ; -6  Reserved
                DCD     SVC_Handler                                             ; -5      SVCall Handler
                DCD     DebugMon_Handler                                        ; -4      Debug Monitor Handler
                DCD    0                                                        ; -3  Reserved
                DCD     PendSV_Handler                                          ; -2      PendSV Handler
                DCD     SysTick_Handler                                         ; -1      SysTick Handler  ; External Interrupts
                DCD    WWDG_IWDG_IRQHandler                                     ; 0  Watchdog interrupt (IWDG is EXTI21)
                DCD    PVD_IRQHandler                                           ; 1  Supply Voltage Detect (PVD) Interrupt (EXTI16)
                DCD    BKP_TAMPER_IRQHandler                                    ; 2  BKP intrusion detection interrupted
                DCD    RTC_IRQHandler                                           ; 3  RTC global interrupt
                DCD    FLASH_IRQHandler                                         ; 4  Flash Global Interrupt
                DCD    RCC_CRS_IRQHandler                                       ; 5  RCC and CRS global interrupt
                DCD    EXTI0_IRQHandler                                         ; 6  EXTI line 0 interrupt
                DCD    EXTI1_IRQHandler                                         ; 7  EXTI line 1 interrupt
                DCD    EXTI2_IRQHandler                                         ; 8  EXTI line 2 interrupt
                DCD    EXTI3_IRQHandler                                         ; 9  EXTI line 3 interrupt
                DCD    EXTI4_IRQHandler                                         ; 10  EXTI line 4 interrupt
                DCD    DMA1_CH1_IRQHandler                                      ; 11  DMA1 channel 1 global interrupt
                DCD    DMA1_CH2_IRQHandler                                      ; 12  DMA1 channel 2 global interrupt
                DCD    DMA1_CH3_IRQHandler                                      ; 13  DMA1 channel 3 global interrupt
                DCD    DMA1_CH4_IRQHandler                                      ; 14  DMA1 channel 4 global interrupt
                DCD    DMA1_CH5_IRQHandler                                      ; 15  DMA1 channel 5 global interrupt
                DCD    DMA1_CH6_IRQHandler                                      ; 16  DMA1 channel 6 global interrupt
                DCD    DMA1_CH7_IRQHandler                                      ; 17  DMA1 channel 7 global interrupt
                DCD    ADC1_2_IRQHandler                                        ; 18  ADC1/2 global interrupt
                DCD    0                                                        ; 19  Reserved
                DCD    DMA1_CH8_IRQHandler                                      ; 20  DMA1 channel 8 global interrupt
                DCD    FlexCAN1_IRQHandler                                      ; 21  FlexCAN1 global interrupt
                DCD    0                                                        ; 22  Reserved
                DCD    EXTI9_5_IRQHandler                                       ; 23  EXTI line[9:5] interrupt
                DCD    TIM1_BRK_IRQHandler                                      ; 24  TIM1 brake interrupt
                DCD    TIM1_UP_IRQHandler                                       ; 25  TIM1 update interrupted
                DCD    TIM1_TRG_COM_IRQHandler                                  ; 26  TIM1 trigger/COM interrupt
                DCD    TIM1_CC_IRQHandler                                       ; 27  TIM1 capture compare interrupt
                DCD    TIM2_IRQHandler                                          ; 28  TIM2 global interrupt
                DCD    TIM3_IRQHandler                                          ; 29  TIM3 global interrupt
                DCD    TIM4_IRQHandler                                          ; 30  TIM4 global interrupt
                DCD    I2C1_IRQHandler                                          ; 31  I2C1 global interrupt
                DCD    0                                                        ; 32  Reserved
                DCD    I2C2_IRQHandler                                          ; 33  I2C2 global interrupt
                DCD    0                                                        ; 34  Reserved
                DCD    SPI1_IRQHandler                                          ; 35  SPI1 global interrupt
                DCD    SPI2_IRQHandler                                          ; 36  SPI2 global interrupt
                DCD    UART1_IRQHandler                                         ; 37  UART1 global interrupt
                DCD    UART2_IRQHandler                                         ; 38  UART2 global interrupt
                DCD    UART3_IRQHandler                                         ; 39  UART3 global interrupt
                DCD    EXTI15_10_IRQHandler                                     ; 40  Interrupt on EXTI line[15:10]
                DCD    RTC_ALR_IRQHandler                                       ; 41  RTC Alarm Interrupt (EXTI17)
                DCD    USB_WKUP_IRQHandler                                      ; 42  USB Wakeup Interrupt (EXTI18)
                DCD    TIM8_BRK_IRQHandler                                      ; 43  TIM8 brake interrupt
                DCD    TIM8_UP_IRQHandler                                       ; 44  TIM8 update interrupted
                DCD    TIM8_TRG_COM_IRQHandler                                  ; 45  TIM8 trigger/COM interrupt
                DCD    TIM8_CC_IRQHandler                                       ; 46  TIM8 capture compare interrupt
                DCD    0                                                        ; 47  Reserved
                DCD    0                                                        ; 48  Reserved
                DCD    0                                                        ; 49  Reserved
                DCD    TIM5_IRQHandler                                          ; 50  TIM5 global interrupt
                DCD    SPI3_IRQHandler                                          ; 51  SPI3 global interrupt
                DCD    UART4_IRQHandler                                         ; 52  UART4 global interrupt
                DCD    UART5_IRQHandler                                         ; 53  UART5 global interrupt
                DCD    TIM6_IRQHandler                                          ; 54  TIM6 global interrupt
                DCD    TIM7_IRQHandler                                          ; 55  TIM7 global interrupt
                DCD    DMA2_CH1_IRQHandler                                      ; 56  DMA2 channel 1 global interrupt
                DCD    DMA2_CH2_IRQHandler                                      ; 57  DMA2 channel 2 global interrupt
                DCD    DMA2_CH3_IRQHandler                                      ; 58  DMA2 channel 3 global interrupt
                DCD    DMA2_CH4_IRQHandler                                      ; 59  DMA2 channel 4 global interrupt
                DCD    DMA2_CH5_IRQHandler                                      ; 60  DMA2 channel 5 global interrupt
                DCD    ENET_IRQHandler                                          ; 61  ENET global interrupt
                DCD    ENET_WKUP_IRQHandler                                     ; 62  ENET wake-up interrupt (EXTI25)
                DCD    0                                                        ; 63  Reserved
                DCD    COMP_IRQHandler                                          ; 64  Comparator 1/2/3 Global Interrupts (EXTI19/20/24)
                DCD    FlexCAN2_IRQHandler                                      ; 65  FLexCAN2 global interrupt
                DCD    0                                                        ; 66  Reserved
                DCD    USB_FS_IRQHandler                                        ; 67  USB OTG global interrupt
                DCD    DMA2_CH6_IRQHandler                                      ; 68  DMA2 channel 6 global interrupt
                DCD    DMA2_CH7_IRQHandler                                      ; 69  DMA2 channel 7 global interrupt
                DCD    DMA2_CH8_IRQHandler                                      ; 70  DMA2 channel 8 global interrupt
                DCD    UART6_IRQHandler                                         ; 71  UART6 global interrupt
                DCD    0                                                        ; 72  Reserved
                DCD    0                                                        ; 73  Reserved
                DCD    0                                                        ; 74  Reserved
                DCD    0                                                        ; 75  Reserved
                DCD    0                                                        ; 76  Reserved
                DCD    0                                                        ; 77  Reserved
                DCD    0                                                        ; 78  Reserved
                DCD    0                                                        ; 79  Reserved
                DCD    0                                                        ; 80  Reserved
                DCD    0                                                        ; 81  Reserved
                DCD    UART7_IRQHandler                                         ; 82  UART7 global interrupt
                DCD    0                                                        ; 83  Reserved
                DCD    0                                                        ; 84  Reserved
                DCD    0                                                        ; 85  Reserved
                DCD    0                                                        ; 86  Reserved
                DCD    0                                                        ; 87  Reserved
                DCD    0                                                        ; 88  Reserved
                DCD    0                                                        ; 89  Reserved
                DCD    0                                                        ; 90  Reserved
                DCD    0                                                        ; 91  Reserved
                DCD    0                                                        ; 92  Reserved
                DCD    0                                                        ; 93  Reserved
                DCD    0                                                        ; 94  Reserved
                DCD    QSPI_IRQHandler                                          ; 95  QSPI global interrupt
                DCD    0                                                        ; 96  Reserved
                DCD    0                                                        ; 97  Reserved
                DCD    0                                                        ; 98  Reserved
                DCD    0                                                        ; 99  Reserved
                DCD    0                                                        ; 100  Reserved
                DCD    0                                                        ; 101  Reserved
                DCD    LPTIM_IRQHandler                                         ; 102  LPTIM global interrupt (EXTI22)
                DCD    0                                                        ; 103  Reserved
                DCD    LPUART_IRQHandler                                        ; 104  LPUART global interrupt (EXTI23)


__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset handler
Reset_Handler   PROC
                EXPORT  Reset_Handler              [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler                [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler          [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler          [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler           [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler           [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler             [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler            [WEAK]
                B       .
                ENDP

Default_Handler PROC
                EXPORT  WWDG_IWDG_IRQHandler       [WEAK]
                EXPORT  PVD_IRQHandler             [WEAK]
                EXPORT  BKP_TAMPER_IRQHandler      [WEAK]
                EXPORT  RTC_IRQHandler             [WEAK]
                EXPORT  FLASH_IRQHandler           [WEAK]
                EXPORT  RCC_CRS_IRQHandler         [WEAK]
                EXPORT  EXTI0_IRQHandler           [WEAK]
                EXPORT  EXTI1_IRQHandler           [WEAK]
                EXPORT  EXTI2_IRQHandler           [WEAK]
                EXPORT  EXTI3_IRQHandler           [WEAK]
                EXPORT  EXTI4_IRQHandler           [WEAK]
                EXPORT  DMA1_CH1_IRQHandler        [WEAK]
                EXPORT  DMA1_CH2_IRQHandler        [WEAK]
                EXPORT  DMA1_CH3_IRQHandler        [WEAK]
                EXPORT  DMA1_CH4_IRQHandler        [WEAK]
                EXPORT  DMA1_CH5_IRQHandler        [WEAK]
                EXPORT  DMA1_CH6_IRQHandler        [WEAK]
                EXPORT  DMA1_CH7_IRQHandler        [WEAK]
                EXPORT  ADC1_2_IRQHandler          [WEAK]
                EXPORT  DMA1_CH8_IRQHandler        [WEAK]
                EXPORT  FlexCAN1_IRQHandler        [WEAK]
                EXPORT  EXTI9_5_IRQHandler         [WEAK]
                EXPORT  TIM1_BRK_IRQHandler        [WEAK]
                EXPORT  TIM1_UP_IRQHandler         [WEAK]
                EXPORT  TIM1_TRG_COM_IRQHandler    [WEAK]
                EXPORT  TIM1_CC_IRQHandler         [WEAK]
                EXPORT  TIM2_IRQHandler            [WEAK]
                EXPORT  TIM3_IRQHandler            [WEAK]
                EXPORT  TIM4_IRQHandler            [WEAK]
                EXPORT  I2C1_IRQHandler            [WEAK]
                EXPORT  I2C2_IRQHandler            [WEAK]
                EXPORT  SPI1_IRQHandler            [WEAK]
                EXPORT  SPI2_IRQHandler            [WEAK]
                EXPORT  UART1_IRQHandler           [WEAK]
                EXPORT  UART2_IRQHandler           [WEAK]
                EXPORT  UART3_IRQHandler           [WEAK]
                EXPORT  EXTI15_10_IRQHandler       [WEAK]
                EXPORT  RTC_ALR_IRQHandler         [WEAK]
                EXPORT  USB_WKUP_IRQHandler        [WEAK]
                EXPORT  TIM8_BRK_IRQHandler        [WEAK]
                EXPORT  TIM8_UP_IRQHandler         [WEAK]
                EXPORT  TIM8_TRG_COM_IRQHandler    [WEAK]
                EXPORT  TIM8_CC_IRQHandler         [WEAK]
                EXPORT  TIM5_IRQHandler            [WEAK]
                EXPORT  SPI3_IRQHandler            [WEAK]
                EXPORT  UART4_IRQHandler           [WEAK]
                EXPORT  UART5_IRQHandler           [WEAK]
                EXPORT  TIM6_IRQHandler            [WEAK]
                EXPORT  TIM7_IRQHandler            [WEAK]
                EXPORT  DMA2_CH1_IRQHandler        [WEAK]
                EXPORT  DMA2_CH2_IRQHandler        [WEAK]
                EXPORT  DMA2_CH3_IRQHandler        [WEAK]
                EXPORT  DMA2_CH4_IRQHandler        [WEAK]
                EXPORT  DMA2_CH5_IRQHandler        [WEAK]
                EXPORT  ENET_IRQHandler            [WEAK]
                EXPORT  ENET_WKUP_IRQHandler       [WEAK]
                EXPORT  COMP_IRQHandler            [WEAK]
                EXPORT  FlexCAN2_IRQHandler        [WEAK]
                EXPORT  USB_FS_IRQHandler          [WEAK]
                EXPORT  DMA2_CH6_IRQHandler        [WEAK]
                EXPORT  DMA2_CH7_IRQHandler        [WEAK]
                EXPORT  DMA2_CH8_IRQHandler        [WEAK]
                EXPORT  UART6_IRQHandler           [WEAK]
                EXPORT  UART7_IRQHandler           [WEAK]
                EXPORT  QSPI_IRQHandler            [WEAK]
                EXPORT  LPTIM_IRQHandler           [WEAK]
                EXPORT  LPUART_IRQHandler		   [WEAK]


WWDG_IWDG_IRQHandler
PVD_IRQHandler
BKP_TAMPER_IRQHandler
RTC_IRQHandler
FLASH_IRQHandler
RCC_CRS_IRQHandler
EXTI0_IRQHandler
EXTI1_IRQHandler
EXTI2_IRQHandler
EXTI3_IRQHandler
EXTI4_IRQHandler
DMA1_CH1_IRQHandler
DMA1_CH2_IRQHandler
DMA1_CH3_IRQHandler
DMA1_CH4_IRQHandler
DMA1_CH5_IRQHandler
DMA1_CH6_IRQHandler
DMA1_CH7_IRQHandler
ADC1_2_IRQHandler
DMA1_CH8_IRQHandler
FlexCAN1_IRQHandler
EXTI9_5_IRQHandler
TIM1_BRK_IRQHandler
TIM1_UP_IRQHandler
TIM1_TRG_COM_IRQHandler
TIM1_CC_IRQHandler
TIM2_IRQHandler
TIM3_IRQHandler
TIM4_IRQHandler
I2C1_IRQHandler
I2C2_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
UART1_IRQHandler
UART2_IRQHandler
UART3_IRQHandler
EXTI15_10_IRQHandler
RTC_ALR_IRQHandler
USB_WKUP_IRQHandler
TIM8_BRK_IRQHandler
TIM8_UP_IRQHandler
TIM8_TRG_COM_IRQHandler
TIM8_CC_IRQHandler
TIM5_IRQHandler
SPI3_IRQHandler
UART4_IRQHandler
UART5_IRQHandler
TIM6_IRQHandler
TIM7_IRQHandler
DMA2_CH1_IRQHandler
DMA2_CH2_IRQHandler
DMA2_CH3_IRQHandler
DMA2_CH4_IRQHandler
DMA2_CH5_IRQHandler
ENET_IRQHandler
ENET_WKUP_IRQHandler
COMP_IRQHandler
FlexCAN2_IRQHandler
USB_FS_IRQHandler
DMA2_CH6_IRQHandler
DMA2_CH7_IRQHandler
DMA2_CH8_IRQHandler
UART6_IRQHandler
UART7_IRQHandler
QSPI_IRQHandler
LPTIM_IRQHandler
LPUART_IRQHandler
                B       .
                ENDP
                ALIGN
                END