/* File: startup_ARMCM7.S
 * Purpose: startup file for Cortex-M7 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V2.0
 * Date: 01 August 2014
 *
 * Copyright (c) 2011 - 2014 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

	.syntax	unified
	.arch	armv7e-m

	.section .stack
	.align	3

	.equ	Stack_Size, 0x0800

	.globl	__StackTop
	.globl	__StackLimit
__StackLimit:
	.space	Stack_Size
	.size	__StackLimit, . - __StackLimit
__StackTop:
	.size	__StackTop, . - __StackTop

	.section .heap
	.align	3

	.equ	Heap_Size, 0x0400

	.globl	__HeapBase
	.globl	__HeapLimit
__HeapBase:
	.if	Heap_Size
	.space	Heap_Size
	.endif
	.size	__HeapBase, . - __HeapBase
__HeapLimit:
	.size	__HeapLimit, . - __HeapLimit

	.section .isr_vector
	.align	2
	.globl	__isr_vector
__isr_vector:
	.long	__StackTop            /* Top of Stack */
	.long	Reset_Handler         /* Reset Handler */
	.long	NMI_Handler           /* NMI Handler */
	.long	HardFault_Handler     /* Hard Fault Handler */
	.long	MemManage_Handler     /* MPU Fault Handler */
	.long	BusFault_Handler      /* Bus Fault Handler */
	.long	UsageFault_Handler    /* Usage Fault Handler */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	SVC_Handler           /* SVCall Handler */
	.long	DebugMon_Handler      /* Debug Monitor Handler */
	.long	0                     /* Reserved */
	.long	PendSV_Handler        /* PendSV Handler */
	.long	SysTick_Handler       /* SysTick Handler */

	/* External interrupts */
  /* External Interrupts */
  .word     WWDG_IRQHandler                   /* Window WatchDog              */                                        
  .word     PVD_IRQHandler                    /* PVD through EXTI Line detection */                        
  .word     TAMP_STAMP_IRQHandler             /* Tamper and TimeStamps through the EXTI line */            
  .word     RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line */                      
  .word     FLASH_IRQHandler                  /* FLASH                        */                                          
  .word     RCC_IRQHandler                    /* RCC                          */                                            
  .word     EXTI0_IRQHandler                  /* EXTI Line0                   */                        
  .word     EXTI1_IRQHandler                  /* EXTI Line1                   */                          
  .word     EXTI2_IRQHandler                  /* EXTI Line2                   */                          
  .word     EXTI3_IRQHandler                  /* EXTI Line3                   */                          
  .word     EXTI4_IRQHandler                  /* EXTI Line4                   */                          
  .word     DMA1_Stream0_IRQHandler           /* DMA1 Stream 0                */                  
  .word     DMA1_Stream1_IRQHandler           /* DMA1 Stream 1                */                   
  .word     DMA1_Stream2_IRQHandler           /* DMA1 Stream 2                */                   
  .word     DMA1_Stream3_IRQHandler           /* DMA1 Stream 3                */                   
  .word     DMA1_Stream4_IRQHandler           /* DMA1 Stream 4                */                   
  .word     DMA1_Stream5_IRQHandler           /* DMA1 Stream 5                */                   
  .word     DMA1_Stream6_IRQHandler           /* DMA1 Stream 6                */                   
  .word     ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s         */                   
  .word     CAN1_TX_IRQHandler                /* CAN1 TX                      */                         
  .word     CAN1_RX0_IRQHandler               /* CAN1 RX0                     */                          
  .word     CAN1_RX1_IRQHandler               /* CAN1 RX1                     */                          
  .word     CAN1_SCE_IRQHandler               /* CAN1 SCE                     */                          
  .word     EXTI9_5_IRQHandler                /* External Line[9:5]s          */                          
  .word     TIM1_BRK_TIM9_IRQHandler          /* TIM1 Break and TIM9          */         
  .word     TIM1_UP_TIM10_IRQHandler          /* TIM1 Update and TIM10        */         
  .word     TIM1_TRG_COM_TIM11_IRQHandler     /* TIM1 Trigger and Commutation and TIM11 */
  .word     TIM1_CC_IRQHandler                /* TIM1 Capture Compare         */                          
  .word     TIM2_IRQHandler                   /* TIM2                         */                   
  .word     TIM3_IRQHandler                   /* TIM3                         */                   
  .word     TIM4_IRQHandler                   /* TIM4                         */                   
  .word     I2C1_EV_IRQHandler                /* I2C1 Event                   */                          
  .word     I2C1_ER_IRQHandler                /* I2C1 Error                   */                          
  .word     I2C2_EV_IRQHandler                /* I2C2 Event                   */                          
  .word     I2C2_ER_IRQHandler                /* I2C2 Error                   */                            
  .word     SPI1_IRQHandler                   /* SPI1                         */                   
  .word     SPI2_IRQHandler                   /* SPI2                         */                   
  .word     USART1_IRQHandler                 /* USART1                       */                   
  .word     USART2_IRQHandler                 /* USART2                       */                   
  .word     USART3_IRQHandler                 /* USART3                       */                   
  .word     EXTI15_10_IRQHandler              /* External Line[15:10]s        */                          
  .word     RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line */                 
  .word     OTG_FS_WKUP_IRQHandler            /* USB OTG FS Wakeup through EXTI line */                       
  .word     TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12         */         
  .word     TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13        */         
  .word     TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14 */
  .word     TIM8_CC_IRQHandler                /* TIM8 Capture Compare         */                          
  .word     DMA1_Stream7_IRQHandler           /* DMA1 Stream7                 */                          
  .word     FMC_IRQHandler                    /* FMC                          */                   
  .word     SDMMC1_IRQHandler                 /* SDMMC1                       */                   
  .word     TIM5_IRQHandler                   /* TIM5                         */                   
  .word     SPI3_IRQHandler                   /* SPI3                         */                   
  .word     UART4_IRQHandler                  /* UART4                        */                   
  .word     UART5_IRQHandler                  /* UART5                        */                   
  .word     TIM6_DAC_IRQHandler               /* TIM6 and DAC1&2 underrun errors */                   
  .word     TIM7_IRQHandler                   /* TIM7                         */
  .word     DMA2_Stream0_IRQHandler           /* DMA2 Stream 0                */                   
  .word     DMA2_Stream1_IRQHandler           /* DMA2 Stream 1                */                   
  .word     DMA2_Stream2_IRQHandler           /* DMA2 Stream 2                */                   
  .word     DMA2_Stream3_IRQHandler           /* DMA2 Stream 3                */                   
  .word     DMA2_Stream4_IRQHandler           /* DMA2 Stream 4                */                   
  .word     ETH_IRQHandler                    /* Ethernet                     */                   
  .word     ETH_WKUP_IRQHandler               /* Ethernet Wakeup through EXTI line */                     
  .word     CAN2_TX_IRQHandler                /* CAN2 TX                      */                          
  .word     CAN2_RX0_IRQHandler               /* CAN2 RX0                     */                          
  .word     CAN2_RX1_IRQHandler               /* CAN2 RX1                     */                          
  .word     CAN2_SCE_IRQHandler               /* CAN2 SCE                     */                          
  .word     OTG_FS_IRQHandler                 /* USB OTG FS                   */                   
  .word     DMA2_Stream5_IRQHandler           /* DMA2 Stream 5                */                   
  .word     DMA2_Stream6_IRQHandler           /* DMA2 Stream 6                */                   
  .word     DMA2_Stream7_IRQHandler           /* DMA2 Stream 7                */                   
  .word     USART6_IRQHandler                 /* USART6                       */                    
  .word     I2C3_EV_IRQHandler                /* I2C3 event                   */                          
  .word     I2C3_ER_IRQHandler                /* I2C3 error                   */                          
  .word     OTG_HS_EP1_OUT_IRQHandler         /* USB OTG HS End Point 1 Out   */                   
  .word     OTG_HS_EP1_IN_IRQHandler          /* USB OTG HS End Point 1 In    */                   
  .word     OTG_HS_WKUP_IRQHandler            /* USB OTG HS Wakeup through EXTI */                         
  .word     OTG_HS_IRQHandler                 /* USB OTG HS                   */                   
  .word     DCMI_IRQHandler                   /* DCMI                         */                   
  .word     0                                 /* Reserved                     */                   
  .word     RNG_IRQHandler                    /* Rng                          */
  .word     FPU_IRQHandler                    /* FPU                          */
  .word     UART7_IRQHandler                  /* UART7                        */      
  .word     UART8_IRQHandler                  /* UART8                        */
  .word     SPI4_IRQHandler                   /* SPI4                         */
  .word     SPI5_IRQHandler                   /* SPI5                           */
  .word     SPI6_IRQHandler                   /* SPI6                         */
  .word     SAI1_IRQHandler                   /* SAI1                          */
  .word     LTDC_IRQHandler                   /* LTDC                          */
  .word     LTDC_ER_IRQHandler                /* LTDC error                      */
  .word     DMA2D_IRQHandler                  /* DMA2D                          */
  .word     SAI2_IRQHandler                   /* SAI2                         */
  .word     QUADSPI_IRQHandler                /* QUADSPI                      */
  .word     LPTIM1_IRQHandler                 /* LPTIM1                       */
  .word     CEC_IRQHandler                    /* HDMI_CEC                     */
  .word     I2C4_EV_IRQHandler                /* I2C4 Event                   */
  .word     I2C4_ER_IRQHandler                /* I2C4 Error                   */
  .word     SPDIF_RX_IRQHandler               /* SPDIF_RX                     */      

	.long	Default_Handler

	.size	__isr_vector, . - __isr_vector

	.text
	.thumb
	.thumb_func
	.align	2
	.globl	Reset_Handler
	.type	Reset_Handler, %function
Reset_Handler:
/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.
 *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */


/*  Single section scheme.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__etext
	ldr	r2, =__data_start__
	ldr	r3, =__data_end__

.L_loop1:
	cmp	r2, r3
	ittt	lt
	ldrlt	r0, [r1], #4
	strlt	r0, [r2], #4
	blt	.L_loop1



/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__

	movs	r0, 0
.L_loop3:
	cmp	r1, r2
	itt	lt
	strlt	r0, [r1], #4
	blt	.L_loop3


	bl	SystemInit
    
	bl	main

	.pool
	.size	Reset_Handler, . - Reset_Handler

	.align	1
	.thumb_func
	.weak	Default_Handler
	.type	Default_Handler, %function
Default_Handler:
	b	.
	.size	Default_Handler, . - Default_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
	.macro	def_irq_handler	handler_name
	.weak	\handler_name
	.set	\handler_name, Default_Handler
	.endm

	def_irq_handler	NMI_Handler
	def_irq_handler	HardFault_Handler
	def_irq_handler	MemManage_Handler
	def_irq_handler	BusFault_Handler
	def_irq_handler	UsageFault_Handler
	def_irq_handler	SVC_Handler
	def_irq_handler	DebugMon_Handler
	def_irq_handler	PendSV_Handler
	def_irq_handler	SysTick_Handler
	def_irq_handler	DEF_IRQHandler
    
  def_irq_handler     WWDG_IRQHandler                   /* Window WatchDog              */                                        
  def_irq_handler     PVD_IRQHandler                    /* PVD through EXTI Line detection */                        
  def_irq_handler     TAMP_STAMP_IRQHandler             /* Tamper and TimeStamps through the EXTI line */            
  def_irq_handler     RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line */                      
  def_irq_handler     FLASH_IRQHandler                  /* FLASH                        */                                          
  def_irq_handler     RCC_IRQHandler                    /* RCC                          */                                            
  def_irq_handler     EXTI0_IRQHandler                  /* EXTI Line0                   */                        
  def_irq_handler     EXTI1_IRQHandler                  /* EXTI Line1                   */                          
  def_irq_handler     EXTI2_IRQHandler                  /* EXTI Line2                   */                          
  def_irq_handler     EXTI3_IRQHandler                  /* EXTI Line3                   */                          
  def_irq_handler     EXTI4_IRQHandler                  /* EXTI Line4                   */                          
  def_irq_handler     DMA1_Stream0_IRQHandler           /* DMA1 Stream 0                */                  
  def_irq_handler     DMA1_Stream1_IRQHandler           /* DMA1 Stream 1                */                   
  def_irq_handler     DMA1_Stream2_IRQHandler           /* DMA1 Stream 2                */                   
  def_irq_handler     DMA1_Stream3_IRQHandler           /* DMA1 Stream 3                */                   
  def_irq_handler     DMA1_Stream4_IRQHandler           /* DMA1 Stream 4                */                   
  def_irq_handler     DMA1_Stream5_IRQHandler           /* DMA1 Stream 5                */                   
  def_irq_handler     DMA1_Stream6_IRQHandler           /* DMA1 Stream 6                */                   
  def_irq_handler     ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s         */                   
  def_irq_handler     CAN1_TX_IRQHandler                /* CAN1 TX                      */                         
  def_irq_handler     CAN1_RX0_IRQHandler               /* CAN1 RX0                     */                          
  def_irq_handler     CAN1_RX1_IRQHandler               /* CAN1 RX1                     */                          
  def_irq_handler     CAN1_SCE_IRQHandler               /* CAN1 SCE                     */                          
  def_irq_handler     EXTI9_5_IRQHandler                /* External Line[9:5]s          */                          
  def_irq_handler     TIM1_BRK_TIM9_IRQHandler          /* TIM1 Break and TIM9          */         
  def_irq_handler     TIM1_UP_TIM10_IRQHandler          /* TIM1 Update and TIM10        */         
  def_irq_handler     TIM1_TRG_COM_TIM11_IRQHandler     /* TIM1 Trigger and Commutation and TIM11 */
  def_irq_handler     TIM1_CC_IRQHandler                /* TIM1 Capture Compare         */                          
  def_irq_handler     TIM2_IRQHandler                   /* TIM2                         */                   
  def_irq_handler     TIM3_IRQHandler                   /* TIM3                         */                   
  def_irq_handler     TIM4_IRQHandler                   /* TIM4                         */                   
  def_irq_handler     I2C1_EV_IRQHandler                /* I2C1 Event                   */                          
  def_irq_handler     I2C1_ER_IRQHandler                /* I2C1 Error                   */                          
  def_irq_handler     I2C2_EV_IRQHandler                /* I2C2 Event                   */                          
  def_irq_handler     I2C2_ER_IRQHandler                /* I2C2 Error                   */                            
  def_irq_handler     SPI1_IRQHandler                   /* SPI1                         */                   
  def_irq_handler     SPI2_IRQHandler                   /* SPI2                         */                   
  def_irq_handler     USART1_IRQHandler                 /* USART1                       */                   
  def_irq_handler     USART2_IRQHandler                 /* USART2                       */                   
  def_irq_handler     USART3_IRQHandler                 /* USART3                       */                   
  def_irq_handler     EXTI15_10_IRQHandler              /* External Line[15:10]s        */                          
  def_irq_handler     RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line */                 
  def_irq_handler     OTG_FS_WKUP_IRQHandler            /* USB OTG FS Wakeup through EXTI line */                       
  def_irq_handler     TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12         */         
  def_irq_handler     TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13        */         
  def_irq_handler     TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14 */
  def_irq_handler     TIM8_CC_IRQHandler                /* TIM8 Capture Compare         */                          
  def_irq_handler     DMA1_Stream7_IRQHandler           /* DMA1 Stream7                 */                          
  def_irq_handler     FMC_IRQHandler                    /* FMC                          */                   
  def_irq_handler     SDMMC1_IRQHandler                 /* SDMMC1                       */                   
  def_irq_handler     TIM5_IRQHandler                   /* TIM5                         */                   
  def_irq_handler     SPI3_IRQHandler                   /* SPI3                         */                   
  def_irq_handler     UART4_IRQHandler                  /* UART4                        */                   
  def_irq_handler     UART5_IRQHandler                  /* UART5                        */                   
  def_irq_handler     TIM6_DAC_IRQHandler               /* TIM6 and DAC1&2 underrun errors */                   
  def_irq_handler     TIM7_IRQHandler                   /* TIM7                         */
  def_irq_handler     DMA2_Stream0_IRQHandler           /* DMA2 Stream 0                */                   
  def_irq_handler     DMA2_Stream1_IRQHandler           /* DMA2 Stream 1                */                   
  def_irq_handler     DMA2_Stream2_IRQHandler           /* DMA2 Stream 2                */                   
  def_irq_handler     DMA2_Stream3_IRQHandler           /* DMA2 Stream 3                */                   
  def_irq_handler     DMA2_Stream4_IRQHandler           /* DMA2 Stream 4                */                   
  def_irq_handler     ETH_IRQHandler                    /* Ethernet                     */                   
  def_irq_handler     ETH_WKUP_IRQHandler               /* Ethernet Wakeup through EXTI line */                     
  def_irq_handler     CAN2_TX_IRQHandler                /* CAN2 TX                      */                          
  def_irq_handler     CAN2_RX0_IRQHandler               /* CAN2 RX0                     */                          
  def_irq_handler     CAN2_RX1_IRQHandler               /* CAN2 RX1                     */                          
  def_irq_handler     CAN2_SCE_IRQHandler               /* CAN2 SCE                     */                          
  def_irq_handler     OTG_FS_IRQHandler                 /* USB OTG FS                   */                   
  def_irq_handler     DMA2_Stream5_IRQHandler           /* DMA2 Stream 5                */                   
  def_irq_handler     DMA2_Stream6_IRQHandler           /* DMA2 Stream 6                */                   
  def_irq_handler     DMA2_Stream7_IRQHandler           /* DMA2 Stream 7                */                   
  def_irq_handler     USART6_IRQHandler                 /* USART6                       */                    
  def_irq_handler     I2C3_EV_IRQHandler                /* I2C3 event                   */                          
  def_irq_handler     I2C3_ER_IRQHandler                /* I2C3 error                   */                          
  def_irq_handler     OTG_HS_EP1_OUT_IRQHandler         /* USB OTG HS End Point 1 Out   */                   
  def_irq_handler     OTG_HS_EP1_IN_IRQHandler          /* USB OTG HS End Point 1 In    */                   
  def_irq_handler     OTG_HS_WKUP_IRQHandler            /* USB OTG HS Wakeup through EXTI */                         
  def_irq_handler     OTG_HS_IRQHandler                 /* USB OTG HS                   */                   
  def_irq_handler     DCMI_IRQHandler                   /* DCMI                         */                   
                  
  def_irq_handler     RNG_IRQHandler                    /* Rng                          */
  def_irq_handler     FPU_IRQHandler                    /* FPU                          */
  def_irq_handler     UART7_IRQHandler                  /* UART7                        */      
  def_irq_handler     UART8_IRQHandler                  /* UART8                        */
  def_irq_handler     SPI4_IRQHandler                   /* SPI4                         */
  def_irq_handler     SPI5_IRQHandler                   /* SPI5                           */
  def_irq_handler     SPI6_IRQHandler                   /* SPI6                         */
  def_irq_handler     SAI1_IRQHandler                   /* SAI1                          */
  def_irq_handler     LTDC_IRQHandler                   /* LTDC                          */
  def_irq_handler     LTDC_ER_IRQHandler                /* LTDC error                      */
  def_irq_handler     DMA2D_IRQHandler                  /* DMA2D                          */
  def_irq_handler     SAI2_IRQHandler                   /* SAI2                         */
  def_irq_handler     QUADSPI_IRQHandler                /* QUADSPI                      */
  def_irq_handler     LPTIM1_IRQHandler                 /* LPTIM1                       */
  def_irq_handler     CEC_IRQHandler                    /* HDMI_CEC                     */
  def_irq_handler     I2C4_EV_IRQHandler                /* I2C4 Event                   */
  def_irq_handler     I2C4_ER_IRQHandler                /* I2C4 Error                   */
  def_irq_handler     SPDIF_RX_IRQHandler               /* SPDIF_RX                     */      

	.end
