/**
 * @file    wm_regs.h
 *
 * @brief   register & operation file
 *
 * @author  winnermicro
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_REGS_H
#define WM_REGS_H

//#include "core_cm3.h"
#include <stdint.h>
#include "core_804.h"


typedef enum IRQn
{
/****** specific Interrupt Numbers *********************************************************/
  SDIO_IRQn                   = 0,      /*!< SDIO Receiver/Transmit/Command Receiver/SDIO Command Transmit Interrupt */
  MAC_IRQn                 	  = 1,      /*!< MAC Interrupt                                        */
  RF_CFG_IRQn                 = 2,      /*!< RF_CFG Interrupt                                        */
  SEC_IRQn                    = 3,      /*!< SEC RX/TX_MNGT/TX_DATA Interrupt                     */
  DMA_Channel0_IRQn           = 4,      /*!< DMA  Channel 0 global Interrupt                      */
  DMA_Channel1_IRQn           = 5,      /*!< DMA  Channel 1 global Interrupt                      */
  DMA_Channel2_IRQn           = 6,      /*!< DMA  Channel 2 global Interrupt                      */
  DMA_Channel3_IRQn           = 7,      /*!< DMA  Channel 3 global Interrupt                      */
  DMA_Channel4_7_IRQn         = 8,      /*!< DMA  Channel 4~7 global Interrupt                    */
  DMA_BRUST_IRQn              = 9,      /*!< DMA Burst global Interrupt                           */
  I2C_IRQn         			  = 10,     /*!< I2C Interrupts                                       */
  ADC_IRQn        			  = 11,     /*!< ADC Convert Interrupts                               */
  SPI_LS_IRQn                 = 12,     /*!< Low Speed SPI Interrupt                              */
  SPI_HS_IRQn                 = 13,     /*!< High Speed SPI Interrupt                             */
  GPIOA_IRQn                  = 14,     /*!< GPIO A Interrupt                                     */
  GPIOB_IRQn                  = 15,     /*!< GPIO B Interrupt                                     */
  UART0_IRQn                  = 16,     /*!< UART0 Interrupts                                     */
  UART1_IRQn               	  = 17,     /*!< UART1 Interrupt                                      */
  TOUCH_IRQn				  = 18,		/*!< TOUCH SENSOR                                      */
  UART2_5_IRQn                = 19,     /*!< UART2/3/4/5 Interrupt                                */
  BLE_IRQn                    = 20,     /*!< BLE Interrupt                                        */
  BT_IRQn                     = 21,     /*!< BT Interrupt                                         */
  PWM_IRQn                 	  = 22,     /*!< PWM Interrupt                                        */
  I2S_IRQn              	  = 23,     /*!< I2S  Interrupts                                      */
  SIDO_HOST_IRQn              = 24,     /*!< SDIO Host Interrupts                                 */
  SYS_TICK_IRQn               = 25,     /*!< SYS Tick Interrupts                                  */
  RSA_IRQn                    = 26,     /*!< RSA Interrupts                                       */
  CRYPTION_IRQn               = 27,     /*!< GPSEC Interrupts                                     */
  FLASH_IRQn                  = 28,     /*!< Flash Interrupt                                      */
  PMU_IRQn                    = 29,     /*!< PMU Interrupt                                        */
  TIM_IRQn                    = 30,     /*!< Timer0~5 Trigger                                     */
  WDG_IRQn                	  = 31,     /*!< Watch Dog Interrupt                                  */
} IRQn_Type;

#define APB_CLK      (40000000) /* 40MHz */

#define DEVICE_BASE_ADDR    0x40000000
/* APB BaseAddr*/
#define HR_APB_BASE_ADDR 	0x40010000UL


/***************************************************************
 * DMA Reg
 ***************************************************************/
 
#define DMA_BASE	(DEVICE_BASE_ADDR + 0x0800)
#define DMA_Channel0_BASE	(DMA_BASE + 0x0010)
#define DMA_Channel1_BASE	(DMA_BASE + 0x0040)
#define DMA_Channel2_BASE	(DMA_BASE + 0x0070)
#define DMA_Channel3_BASE	(DMA_BASE + 0x00A0)
#define DMA_Channel4_BASE	(DMA_BASE + 0x00D0)
#define DMA_Channel5_BASE	(DMA_BASE + 0x0100)
#define DMA_Channel6_BASE	(DMA_BASE + 0x0130)
#define DMA_Channel7_BASE	(DMA_BASE + 0x0460)

typedef struct
{
	__IOM uint32_t IM;
	__IOM uint32_t IF;
	__IOM uint32_t REQCH;
	__IOM uint32_t RSV;
} DMA_TypeDef;

typedef struct
{
	__IOM uint32_t SA;
	__IOM uint32_t DA;
	__IOM uint32_t SWA;
	__IOM uint32_t DWA;
	__IOM uint32_t WLEN;
	__IOM uint32_t CR1;
	__IOM uint32_t MODE;
	__IOM uint32_t CR2;
	__IM uint32_t SR;
	__IOM uint32_t LA;
	__IM uint32_t CDA;
} DMA_Channel_TypeDef;

#define DMA_IM_TRANSFER_DONE_Pos			(1)
#define DMA_IM_TRANSFER_DONE_Msk			(0x1UL << DMA_IM_TRANSFER_DONE_Pos)
#define DMA_IM_TRANSFER_DONW				DMA_IM_TRANSFER_DONE_Msk
#define DMA_IM_BURST_DONE_Pos				(0)
#define DMA_IM_BURST_DONE_Msk				(0x1UL << DMA_IM_BURST_DONE_Pos)
#define DMA_IM_BURST_DONE					DMA_IM_BURST_DONE_Msk

#define DMA_IF_TRANSFER_DONE_Pos			(1)
#define DMA_IF_TRANSFER_DONE_Msk			(0x1UL << DMA_IF_TRANSFER_DONE_Pos)
#define DMA_IF_TRANSFER_DONE				(DMA_IF_TRANSFER_DONE_Msk)
#define DMA_IF_BURST_DONE_Pos				(0)
#define DMA_IF_BURST_DONE_Msk				(0x1UL << DMA_IF_BURST_DONE_Pos)
#define DMA_IF_BURST_DONE					DMA_IF_BURST_DONE_Msk

#define DMA_REQCH_UART_Pos					(0)
#define DMA_REQCH_UART_Msk					(0x7UL << DMA_REQCH_UART_Pos)
#define DMA_REQCH_UART						DMA_REQCH_UART_Msk
#define DMA_REQCH_UART0						(0x0UL << DMA_REQCH_UART_Pos)
#define DMA_REQCH_UART1						(0x1UL << DMA_REQCH_UART_Pos)
#define DMA_REQCH_UART2						(0x2UL << DMA_REQCH_UART_Pos)
#define DMA_REQCH_UART3						(0x3UL << DMA_REQCH_UART_Pos)
#define DMA_REQCH_UART4						(0x4UL << DMA_REQCH_UART_Pos)
#define DMA_REQCH_UART5						(0x5UL << DMA_REQCH_UART_Pos)

#define DMA_WLEN_D_Pos						(16)
#define DMA_WLEN_D_Msk						(0xFFFFUL << DMA_WLEN_D_Pos)
#define DMA_WLEN_D							DMA_WLEN_D_Msk
#define DMA_WLEN_S_Pos						(0)
#define DMA_WLEN_S_Msk						(0xFFFFUL << DMA_WLEN_S_Pos)
#define DMA_WLEN_S							DMA_WLEN_S_Msk

#define DMA_CR1_STOP_Pos					(1)
#define DMA_CR1_STOP_Msk					(0x1UL << DMA_CR1_STOP_Pos)
#define DMA_CR1_STOP						DMA_CR1_STOP_Msk
#define DMA_CR1_START_Pos					(0)
#define DMA_CR1_START_Msk					(0x1UL << DMA_CR1_START_Pos)
#define DMA_CR1_START						DMA_CR1_START_Msk

#define DMA_MODE_LINK_Pos					(6)
#define DMA_MODE_LINK_Msk					(0x1UL << DMA_MODE_LINK_Pos)
#define DMA_MODE_LINK						DMA_MODE_LINK_Msk
#define DMA_MODE_CH_Pos						(2)
#define DMA_MODE_CH_Msk						(0xFUL << DMA_MODE_CH_Pos)
#define DMA_MODE_CH							DMA_MODE_CH_Msk
#define DMA_MODE_CH_UART_RX					(0x0UL << DMA_MODE_CH_Pos)
#define DMA_MODE_CH_UART_TX					(0x1UL << DMA_MODE_CH_Pos)
#define DMA_MODE_CH_PWM_CAP0				(0x2UL << DMA_MODE_CH_Pos)
#define DMA_MODE_CH_PWM_CAP1				(0x3UL << DMA_MODE_CH_Pos)
#define DMA_MODE_CH_LSPI_RX					(0x4UL << DMA_MODE_CH_Pos)
#define DMA_MODE_CH_LSPI_TX					(0x5UL << DMA_MODE_CH_Pos)
#define DMA_MODE_CH_ADC0					(0x6UL << DMA_MODE_CH_Pos)
#define DMA_MODE_CH_ADC1					(0x7UL << DMA_MODE_CH_Pos)
#define DMA_MODE_CH_ADC2					(0x8UL << DMA_MODE_CH_Pos)
#define DMA_MODE_CH_ADC3					(0x9UL << DMA_MODE_CH_Pos)
#define DMA_MODE_CH_I2SRX					(0xAUL << DMA_MODE_CH_Pos)
#define DMA_MODE_CH_I2STX					(0xBUL << DMA_MODE_CH_Pos)
#define DMA_MODE_CH_SDIO					(0xCUL << DMA_MODE_CH_Pos)
#define DMA_MODE_LNM_Pos					(1)
#define DMA_MODE_LNM_Msk					(0x1UL << DMA_MODE_LNM_Pos)
#define DMA_MODE_LNM						DMA_MODE_LNM_Msk
#define DMA_MODE_SHM_Pos					(0)
#define DMA_MODE_SHM_Msk					(0x1UL << DMA_MODE_SHM_Pos)
#define DMA_MODE_SHM						DMA_MODE_SHM_Msk

#define DMA_CR2_LEN_Pos						(8)
#define DMA_CR2_LEN_Msk						(0xFFFFUL << DMA_CR2_LEN_Pos)
#define DMA_CR2_LEN							DMA_CR2_LEN_Msk
#define DMA_CR2_BURST_SIZE_Pos				(7)
#define DMA_CR2_BURST_SIZE_Msk				(0x1UL << DMA_CR2_BURST_SIZE_Pos)
#define DMA_CR2_BURST_SIZE					DMA_CR2_BURST_SIZE_Msk
#define DMA_CR2_TRANS_SIZE_Pos				(5)
#define DMA_CR2_TRANS_SIZE_Msk				(0x3UL << DMA_CR2_TRANS_SIZE_Pos)
#define DMA_CR2_TRANS_SIZE					DMA_CR2_TRANS_SIZE_Msk
#define DMA_CR2_TRANS_SIZE_BYTE				(0x0UL << DMA_CR2_TRANS_SIZE_Pos)
#define DMA_CR2_TRANS_SIZE_HALFWORD			(0x1UL << DMA_CR2_TRANS_SIZE_Pos)
#define DMA_CR2_TRANS_SIZE_WORD				(0x2UL << DMA_CR2_TRANS_SIZE_Pos)
#define DMA_CR2_DINC_Pos					(3)
#define DMA_CR2_DINC_Msk					(0x3UL << DMA_CR2_DINC_Pos)
#define DMA_CR2_DINC						DMA_CR2_DINC_Msk
#define DMA_CR2_DINC_DISABLE				(0x0UL << DMA_CR2_DINC_Pos)
#define DMA_CR2_DINC_ENABLE					(0x1UL << DMA_CR2_DINC_Pos)
#define DMA_CR2_DINC_CYCLE					(0x3UL << DMA_CR2_DINC_Pos)
#define DMA_CR2_SINC_Pos					(1)
#define DMA_CR2_SINC_Msk					(0x3UL << DMA_CR2_SINC_Pos)
#define DMA_CR2_SINC						DMA_CR2_SINC_Msk
#define DMA_CR2_SINC_DISABLE				(0x0UL << DMA_CR2_SINC_Pos)
#define DMA_CR2_SINC_ENABLE					(0x1UL << DMA_CR2_SINC_Pos)
#define DMA_CR2_SINC_CYCLE					(0x3UL << DMA_CR2_SINC_Pos)
#define DMA_CR2_AUTORELOAD_Pos				(0)
#define DMA_CR2_AUTORELOAD_Msk				(0x1UL << DMA_CR2_AUTORELOAD_Pos)
#define DMA_CR2_AUTORELOAD					DMA_CR2_AUTORELOAD_Msk


/***************************************************************
 * PMU Reg
 ***************************************************************/

#define PMU_BASE	(DEVICE_BASE_ADDR + 0x0D00)

typedef struct
{
	__IOM uint32_t CR;
	__IOM uint32_t TIMER0;
	__IOM uint32_t RSV;
	__IOM uint32_t RTCCR0;
	__IOM uint32_t RTCCR1;
	__IOM uint32_t IF;
} PMU_TypeDef;

#define PMU_CR_WAKEUP_LEVEL_Pos				(10)
#define PMU_CR_WAKEUP_LEVEL_Msk				(0x01 << PMU_CR_WAKEUP_LEVEL_Pos)
#define PMU_CR_WAKEUP_LEVEL					PMU_CR_WAKEUP_LEVEL_Msk
#define PMU_CR_WAKEUP_COUNT_Pos				(6)
#define PMU_CR_WAKEUP_COUNT_Msk				(0x0F << PMU_CR_WAKEUP_COUNT_Pos)
#define PMU_CR_WAKEUP_COUNT					PMU_CR_WAKEUP_COUNT_Msk
#define PMU_CR_32KRCBYPASS_Pos				(4)
#define	PMU_CR_32KRCBYPASS_Msk				(0x01 << PMU_CR_32KRCBYPASS_Pos)
#define PMU_CR_32KRCBYPASS					PMU_CR_32KRCBYPASS_Msk
#define PMU_CR_32KRC_CAL_EN_Pos				(3)
#define PMU_CR_32KRC_CAL_EN_Msk				(0x01 << PMU_CR_32KRC_CAL_EN_Pos)
#define PMU_CR_32KRC_CAL_EN					PMU_CR_32KRC_CAL_EN_Msk
#define PMU_CR_SLEEP_EN_Pos					(1)
#define PMU_CR_SLEEP_EN_Msk					(0x01 << PMU_CR_SLEEP_EN_Pos)
#define PMU_CR_SLEEP_EN						PMU_CR_SLEEP_EN_Msk
#define PMU_CR_STANDBY_EN_Pos				(0)
#define PMU_CR_STANDBY_EN_Msk				(0x01 << PMU_CR_STANDBY_EN_Pos)
#define PMU_CR_STANDBY_EN					PMU_CR_STANDBY_EN_Msk

#define PMU_TIMER0_EN_Pos					(16)
#define PMU_TIMER0_EN_Msk					(0x01 << PMU_TIMER0_EN_Pos)
#define PMU_TIMER0_EN						PMU_TIMER0_EN_Msk
#define PMU_TIMER0_VALUE_Pos				(0)
#define PMU_TIMER0_VALUE_Msk				(0x0FFFF << PMU_TIMER0_VALUE_Pos)
#define PMU_TIMER0_VALUE					PMU_TIMER0_VALUE_Msk

#define PMU_RTCCR0_TIMING_EN_Pos			(31)
#define PMU_RTCCR0_TIMING_EN_Msk			(0x01 << PMU_RTCCR0_TIMING_EN_Pos)
#define PMU_RTCCR0_TIMING_EN				PMU_RTCCR0_TIMING_EN_Msk
#define PMU_RTCCR0_DATE_Pos					(24)
#define PMU_RTCCR0_DATE_Msk					(0x01F << PMU_RTCCR0_DATE_Pos)
#define PMU_RTCCR0_DATE						PMU_RTCCR0_DATE_Msk
#define PMU_RTCCR0_HOUR_Pos					(16)
#define PMU_RTCCR0_HOUR_Msk					(0x01F << PMU_RTCCR0_HOUR_Pos)
#define PMU_RTCCR0_HOUR						PMU_RTCCR0_HOUR_Msk
#define PMU_RTCCR0_MINUTE_Pos				(8)
#define PMU_RTCCR0_MINUTE_Msk				(0x03F << PMU_RTCCR0_MINUTE_Pos)
#define PMU_RTCCR0_MINUTE					PMU_RTCCR0_MINUTE_Msk
#define PMU_RTCCR0_SECOND_Pos				(0)
#define PMU_RTCCR0_SECOND_Msk				(0x03F << PMU_RTCCR0_SECOND_Pos)
#define PMU_RTCCR0_SECOND					PMU_RTCCR0_SECOND_Msk

#define PMU_RTCCR1_EN_Pos					(16)
#define PMU_RTCCR1_EN_Msk					(0x01 << PMU_RTCCR1_EN_Pos)
#define PMU_RTCCR1_EN						PMU_RTCCR1_EN_Msk
#define PMU_RTCCR1_YEAR_Pos					(8)
#define PMU_RTCCR1_YEAR_Msk					(0x07F << PMU_RTCCR1_YEAR_Pos)
#define PMU_RTCCR1_YEAR						PMU_RTCCR1_YEAR_Msk
#define PMU_RTCCR1_MONTH_Pos				(0)
#define PMU_RTCCR1_MONTH_Msk				(0x0F << PMU_RTCCR1_MONTH_Pos)
#define PMU_RTCCR1_MONTH					PMU_RTCCR1_MONTH_Msk

#define PMU_IF_SLEEP_Pos					(8)
#define PMU_IF_SLEEP_Msk					(0x01 << PMU_IF_SLEEP_Pos)
#define PMU_IF_SLEEP						PMU_IF_SLEEP_Msk
#define PMU_IF_STANDBY_Pos					(7)
#define PMU_IF_STANDBY_Msk					(0x01 << PMU_IF_STANDBY_Pos)
#define PMU_IF_STANDBY						PMU_IF_STANDBY_Msk
#define PMU_IF_RTC_Pos						(4)
#define PMU_IF_RTC_Msk						(0x01 << PMU_IF_RTC_Pos)
#define PMU_IF_RTC							PMU_IF_RTC_Msk
#define PMU_IF_IO_WAKE_Pos					(2)
#define PMU_IF_IO_WAKE_Msk					(0x01 << PMU_IF_IO_WAKE_Pos)
#define PMU_IF_IO_WAKE						PMU_IF_IO_WAKE_Msk
#define PMU_IF_TIM0_Pos						(0)
#define PMU_IF_TIM0_Msk						(0x01 << PMU_IF_TIM0_Pos)
#define PMU_IF_TIM0							PMU_IF_TIM0_Msk


/***************************************************************
 * system clock and bus clock Reg
 ***************************************************************/

#define RCC_BASE	(DEVICE_BASE_ADDR + 0xE00)

typedef struct
{
	__IOM uint32_t CLK_EN;
	__IOM uint32_t CLK_MASK;
	__IOM uint32_t BBP_CLK;
	__IOM uint32_t RST;
	__IOM uint32_t CLK_DIV;
	__IOM uint32_t CLK_SEL;
	__IOM uint32_t I2S_CLK;
	__IOM uint32_t RST_STATUS;
} RCC_TypeDef;

#define RCC_CLK_EN_TOUCH_Pos				(21)
#define RCC_CLK_EN_TOUCH_Msk                (0x1 << RCC_CLK_EN_TOUCH_Pos)
#define RCC_CLK_EN_TOUCH                    RCC_CLK_EN_TOUCH_Msk
#define RCC_CLK_EN_BT_Pos					(20)
#define RCC_CLK_EN_BT_Msk                	(0x1 << RCC_CLK_EN_BT_Pos)
#define RCC_CLK_EN_BT                    	RCC_CLK_EN_BT_Msk
#define RCC_CLK_EN_QSRAM_Pos				(19)
#define RCC_CLK_EN_QSRAM_Msk                (0x1 << RCC_CLK_EN_QSRAM_Pos)
#define RCC_CLK_EN_QSRAM                    RCC_CLK_EN_QSRAM_Msk
#define RCC_CLK_EN_SDIO_Pos					(18)
#define RCC_CLK_EN_SDIO_Msk                	(0x1 << RCC_CLK_EN_SDIO_Pos)
#define RCC_CLK_EN_SDIO                    	RCC_CLK_EN_SDIO_Msk
#define RCC_CLK_EN_GPSEC_Pos				(17)
#define RCC_CLK_EN_GPSEC_Msk                (0x1 << RCC_CLK_EN_GPSEC_Pos)
#define RCC_CLK_EN_GPSEC                    RCC_CLK_EN_GPSEC_Msk
#define RCC_CLK_EN_RSA_Pos					(16)
#define RCC_CLK_EN_RSA_Msk                	(0x1 << RCC_CLK_EN_RSA_Pos)
#define RCC_CLK_EN_RSA                    	RCC_CLK_EN_RSA_Msk
#define RCC_CLK_EN_I2S_Pos					(15)
#define RCC_CLK_EN_I2S_Msk                	(0x1 << RCC_CLK_EN_I2S_Pos)
#define RCC_CLK_EN_I2S                    	RCC_CLK_EN_I2S_Msk
#define RCC_CLK_EN_LCD_Pos					(14)
#define RCC_CLK_EN_LCD_Msk                	(0x1 << RCC_CLK_EN_LCD_Pos)
#define RCC_CLK_EN_LCD                    	RCC_CLK_EN_LCD_Msk
#define RCC_CLK_EN_PWM_Pos					(13)
#define RCC_CLK_EN_PWM_Msk                	(0x1 << RCC_CLK_EN_PWM_Pos)
#define RCC_CLK_EN_PWM                    	RCC_CLK_EN_PWM_Msk
#define RCC_CLK_EN_ADC_Pos					(12)
#define RCC_CLK_EN_ADC_Msk                	(0x1 << RCC_CLK_EN_ADC_Pos)
#define RCC_CLK_EN_ADC                    	RCC_CLK_EN_ADC_Msk
#define RCC_CLK_EN_GPIO_Pos					(11)
#define RCC_CLK_EN_GPIO_Msk                	(0x1 << RCC_CLK_EN_GPIO_Pos)
#define RCC_CLK_EN_GPIO                    	RCC_CLK_EN_GPIO_Msk
#define RCC_CLK_EN_TIMER_Pos				(10)
#define RCC_CLK_EN_TIMER_Msk                (0x1 << RCC_CLK_EN_TIMER_Pos)
#define RCC_CLK_EN_TIMER                    RCC_CLK_EN_TIMER_Msk
#define RCC_CLK_EN_RF_Pos					(9)
#define RCC_CLK_EN_RF_Msk                	(0x1 << RCC_CLK_EN_RF_Pos)
#define RCC_CLK_EN_RF                    	RCC_CLK_EN_RF_Msk
#define RCC_CLK_EN_DMA_Pos					(8)
#define RCC_CLK_EN_DMA_Msk                	(0x1 << RCC_CLK_EN_DMA_Pos)
#define RCC_CLK_EN_DMA                    	RCC_CLK_EN_DMA_Msk
#define RCC_CLK_EN_LSPI_Pos					(7)
#define RCC_CLK_EN_LSPI_Msk                	(0x1 << RCC_CLK_EN_LSPI_Pos)
#define RCC_CLK_EN_LSPI                    	RCC_CLK_EN_LSPI_Msk
#define RCC_CLK_EN_UART5_Pos				(6)
#define RCC_CLK_EN_UART5_Msk                (0x1 << RCC_CLK_EN_UART5_Pos)
#define RCC_CLK_EN_UART5                    RCC_CLK_EN_UART5_Msk
#define RCC_CLK_EN_UART4_Pos				(5)
#define RCC_CLK_EN_UART4_Msk                (0x1 << RCC_CLK_EN_UART4_Pos)
#define RCC_CLK_EN_UART4                    RCC_CLK_EN_UART4_Msk
#define RCC_CLK_EN_UART3_Pos				(4)
#define RCC_CLK_EN_UART3_Msk                (0x1 << RCC_CLK_EN_UART3_Pos)
#define RCC_CLK_EN_UART3                    RCC_CLK_EN_UART3_Msk
#define RCC_CLK_EN_UART2_Pos				(3)
#define RCC_CLK_EN_UART2_Msk                (0x1 << RCC_CLK_EN_UART2_Pos)
#define RCC_CLK_EN_UART2                    RCC_CLK_EN_UART2_Msk
#define RCC_CLK_EN_UART1_Pos				(2)
#define RCC_CLK_EN_UART1_Msk                (0x1 << RCC_CLK_EN_UART1_Pos)
#define RCC_CLK_EN_UART1                    RCC_CLK_EN_UART1_Msk
#define RCC_CLK_EN_UART0_Pos				(1)
#define RCC_CLK_EN_UART0_Msk                (0x1 << RCC_CLK_EN_UART0_Pos)
#define RCC_CLK_EN_UART0                    RCC_CLK_EN_UART0_Msk
#define RCC_CLK_EN_I2C_Pos					(0)
#define RCC_CLK_EN_I2C_Msk                	(0x1 << RCC_CLK_EN_I2C_Pos)
#define RCC_CLK_EN_I2C                    	RCC_CLK_EN_I2C_Msk
#define RCC_CLK_EN_ALL_Pos					(0)
#define RCC_CLK_EN_ALL_Msk                	(0x3FFFFF << RCC_CLK_EN_ALL_Pos)
#define RCC_CLK_EN_ALL                    	RCC_CLK_EN_ALL_Msk

#define RCC_CLK_MASK_CPU_Pos				(6)
#define RCC_CLK_MASK_CPU_Msk                (0x1 << RCC_CLK_MASK_CPU_Pos)
#define RCC_CLK_MASK_CPU                    RCC_CLK_MASK_CPU_Msk
#define RCC_CLK_MASK_TPIK_WEP_Pos			(5)
#define RCC_CLK_MASK_TPIK_WEP_Msk           (0x1 << RCC_CLK_MASK_TPIK_WEP_Pos)
#define RCC_CLK_MASK_TPIK_WEP               RCC_CLK_MASK_TPIK_WEP_Msk
#define RCC_CLK_MASK_CCMP_Pos				(4)
#define RCC_CLK_MASK_CCMP_Msk               (0x1 << RCC_CLK_MASK_CCMP_Pos)
#define RCC_CLK_MASK_CCMP                   RCC_CLK_MASK_CCMP_Msk
#define RCC_CLK_MASK_WPI_Pos				(3)
#define RCC_CLK_MASK_WPI_Msk                (0x1 << RCC_CLK_MASK_WPI_Pos)
#define RCC_CLK_MASK_WPI                    RCC_CLK_MASK_WPI_Msk
#define RCC_CLK_MASK_SDIOAHB_Pos			(1)
#define RCC_CLK_MASK_SDIOAHB_Msk            (0x1 << RCC_CLK_MASK_SDIOAHB_Pos)
#define RCC_CLK_MASK_SDIOAHB                RCC_CLK_MASK_SDIOAHB_Msk
#define RCC_CLK_MASK_PMU_Pos				(0)
#define RCC_CLK_MASK_PMU_Msk            	(0x1 << RCC_CLK_MASK_PMU_Pos)
#define RCC_CLK_MASK_PMU                	RCC_CLK_MASK_PMU_Msk

#define RCC_BBP_CLK_WIFI_Pos				(4)
#define RCC_BBP_CLK_WIFI_Msk            	(0x1 << RCC_BBP_CLK_WIFI_Pos)
#define RCC_BBP_CLK_WIFI                	RCC_BBP_CLK_WIFI_Msk
#define RCC_BBP_CLK_RX_Pos					(2)
#define RCC_BBP_CLK_RX_Msk            		(0x3 << RCC_BBP_CLK_RX_Pos)
#define RCC_BBP_CLK_RX                		RCC_BBP_CLK_RX_Msk
#define RCC_BBP_CLK_RX_00					(0x0 << RCC_BBP_CLK_RX_Pos)
#define RCC_BBP_CLK_RX_01					(0x1 << RCC_BBP_CLK_RX_Pos)
#define RCC_BBP_CLK_RX_10					(0x2 << RCC_BBP_CLK_RX_Pos)
#define RCC_BBP_CLK_RX_11					(0x3 << RCC_BBP_CLK_RX_Pos)
#define RCC_BBP_CLK_TX_Pos					(0)
#define RCC_BBP_CLK_TX_Msk            		(0x3 << RCC_BBP_CLK_TX_Pos)
#define RCC_BBP_CLK_TX                		RCC_BBP_CLK_TX_Msk
#define RCC_BBP_CLK_TX_00					(0x0 << RCC_BBP_CLK_TX_Pos)
#define RCC_BBP_CLK_TX_01					(0x1 << RCC_BBP_CLK_TX_Pos)
#define RCC_BBP_CLK_TX_11					(0x3 << RCC_BBP_CLK_TX_Pos)

#define RCC_RST_TOUCH_Pos					(31)
#define RCC_RST_TOUCH_Msk            		(0x1 << RCC_RST_TOUCH_Pos)
#define RCC_RST_TOUCH                		RCC_RST_TOUCH_Msk
#define RCC_RST_FLASH_Pos					(30)
#define RCC_RST_FLASH_Msk            		(0x1 << RCC_RST_FLASH_Pos)
#define RCC_RST_FLASH                		RCC_RST_FLASH_Msk
#define RCC_RST_BT_Pos						(29)
#define RCC_RST_BT_Msk            			(0x1 << RCC_RST_BT_Pos)
#define RCC_RST_BT                			RCC_RST_BT_Msk
#define RCC_RST_QSPI_Pos					(28)
#define RCC_RST_QSPI_Msk            		(0x1 << RCC_RST_QSPI_Pos)
#define RCC_RST_QSPI                		RCC_RST_QSPI_Msk
#define RCC_RST_GPSEC_Pos					(26)
#define RCC_RST_GPSEC_Msk            		(0x1 << RCC_RST_GPSEC_Pos)
#define RCC_RST_GPSEC                		RCC_RST_GPSEC_Msk
#define RCC_RST_RSA_Pos						(25)
#define RCC_RST_RSA_Msk            			(0x1 << RCC_RST_RSA_Pos)
#define RCC_RST_RSA                			RCC_RST_RSA_Msk
#define RCC_RST_I2S_Pos						(24)
#define RCC_RST_I2S_Msk            			(0x1 << RCC_RST_I2S_Pos)
#define RCC_RST_I2S                			RCC_RST_I2S_Msk
#define RCC_RST_LCD_Pos						(23)
#define RCC_RST_LCD_Msk            			(0x1 << RCC_RST_LCD_Pos)
#define RCC_RST_LCD                			RCC_RST_LCD_Msk
#define RCC_RST_PWM_Pos						(22)
#define RCC_RST_PWM_Msk            			(0x1 << RCC_RST_PWM_Pos)
#define RCC_RST_PWM                			RCC_RST_PWM_Msk
#define RCC_RST_ADC_Pos						(21)
#define RCC_RST_ADC_Msk            			(0x1 << RCC_RST_ADC_Pos)
#define RCC_RST_ADC                			RCC_RST_ADC_Msk
#define RCC_RST_TIMER_Pos					(20)
#define RCC_RST_TIMER_Msk            		(0x1 << RCC_RST_TIMER_Pos)
#define RCC_RST_TIMER                		RCC_RST_TIMER_Msk
#define RCC_RST_GPIO_Pos					(19)
#define RCC_RST_GPIO_Msk            		(0x1 << RCC_RST_GPIO_Pos)
#define RCC_RST_GPIO                		RCC_RST_GPIO_Msk
#define RCC_RST_RF_Pos						(18)
#define RCC_RST_RF_Msk            			(0x1 << RCC_RST_RF_Pos)
#define RCC_RST_RF                			RCC_RST_RF_Msk
#define RCC_RST_SSPI_Pos					(17)
#define RCC_RST_SSPI_Msk            		(0x1 << RCC_RST_SSPI_Pos)
#define RCC_RST_SSPI                		RCC_RST_SSPI_Msk
#define RCC_RST_MSPI_Pos					(16)
#define RCC_RST_MSPI_Msk            		(0x1 << RCC_RST_MSPI_Pos)
#define RCC_RST_MSPI                		RCC_RST_MSPI_Msk
#define RCC_RST_UART5_Pos					(15)
#define RCC_RST_UART5_Msk            		(0x1 << RCC_RST_UART5_Pos)
#define RCC_RST_UART5                		RCC_RST_UART5_Msk
#define RCC_RST_UART4_Pos					(14)
#define RCC_RST_UART4_Msk            		(0x1 << RCC_RST_UART4_Pos)
#define RCC_RST_UART4                		RCC_RST_UART4_Msk
#define RCC_RST_UART3_Pos					(13)
#define RCC_RST_UART3_Msk            		(0x1 << RCC_RST_UART3_Pos)
#define RCC_RST_UART3                		RCC_RST_UART3_Msk
#define RCC_RST_UART2_Pos					(12)
#define RCC_RST_UART2_Msk            		(0x1 << RCC_RST_UART2_Pos)
#define RCC_RST_UART2                		RCC_RST_UART2_Msk
#define RCC_RST_UART1_Pos					(11)
#define RCC_RST_UART1_Msk            		(0x1 << RCC_RST_UART1_Pos)
#define RCC_RST_UART1                		RCC_RST_UART1_Msk
#define RCC_RST_UART0_Pos					(10)
#define RCC_RST_UART0_Msk            		(0x1 << RCC_RST_UART0_Pos)
#define RCC_RST_UART0                		RCC_RST_UART0_Msk
#define RCC_RST_I2C_Pos						(9)
#define RCC_RST_I2C_Msk            			(0x1 << RCC_RST_I2C_Pos)
#define RCC_RST_I2C                			RCC_RST_I2C_Msk
#define RCC_RST_BUS2_Pos					(8)
#define RCC_RST_BUS2_Msk            		(0x1 << RCC_RST_BUS2_Pos)
#define RCC_RST_BUS2                		RCC_RST_BUS2_Msk
#define RCC_RST_BUS1_Pos					(7)
#define RCC_RST_BUS1_Msk            		(0x1 << RCC_RST_BUS1_Pos)
#define RCC_RST_BUS1                		RCC_RST_BUS1_Msk
#define RCC_RST_APB_Pos						(6)
#define RCC_RST_APB_Msk            			(0x1 << RCC_RST_APB_Pos)
#define RCC_RST_APB                			RCC_RST_APB_Msk
#define RCC_RST_MEM_Pos						(5)
#define RCC_RST_MEM_Msk            			(0x1 << RCC_RST_MEM_Pos)
#define RCC_RST_MEM                			RCC_RST_MEM_Msk
#define RCC_RST_DMA_Pos						(4)
#define RCC_RST_DMA_Msk            			(0x1 << RCC_RST_DMA_Pos)
#define RCC_RST_DMA                			RCC_RST_DMA_Msk
#define RCC_RST_SDIO_Pos					(3)
#define RCC_RST_SDIO_Msk            		(0x1 << RCC_RST_SDIO_Pos)
#define RCC_RST_SDIO                		RCC_RST_SDIO_Msk
#define RCC_RST_SEC_Pos						(2)
#define RCC_RST_SEC_Msk            			(0x1 << RCC_RST_SEC_Pos)
#define RCC_RST_SEC                			RCC_RST_SEC_Msk
#define RCC_RST_MAC_Pos						(1)
#define RCC_RST_MAC_Msk            			(0x1 << RCC_RST_MAC_Pos)
#define RCC_RST_MAC                			RCC_RST_MAC_Msk
#define RCC_RST_BBP_Pos						(0)
#define RCC_RST_BBP_Msk            			(0x1 << RCC_RST_BBP_Pos)
#define RCC_RST_BBP                			RCC_RST_BBP_Msk

#define RCC_CLK_DIV_FREQ_EN_Pos				(31)
#define RCC_CLK_DIV_FREQ_EN_Msk            	(0x1 << RCC_CLK_DIV_FREQ_EN_Pos)
#define RCC_CLK_DIV_FREQ_EN					RCC_CLK_DIV_FREQ_EN_Msk
#define RCC_CLK_DIV_PERIPHERAL_Pos			(24)
#define RCC_CLK_DIV_PERIPHERAL_Msk          (0xF << RCC_CLK_DIV_PERIPHERAL_Pos)
#define RCC_CLK_DIV_PERIPHERAL				RCC_CLK_DIV_PERIPHERAL_Msk
#define RCC_CLK_DIV_BUS2_SYNCDN_FACTOR_Pos	(16)
#define RCC_CLK_DIV_BUS2_SYNCDN_FACTOR_Msk  (0xFF << RCC_CLK_DIV_BUS2_SYNCDN_FACTOR_Pos)
#define RCC_CLK_DIV_BUS2_SYNCDN_FACTOR		RCC_CLK_DIV_BUS2_SYNCDN_FACTOR_Msk
#define RCC_CLK_DIV_WLAN_Pos				(8)
#define RCC_CLK_DIV_WLAN_Msk          		(0xFF << RCC_CLK_DIV_WLAN_Pos)
#define RCC_CLK_DIV_WLAN					RCC_CLK_DIV_WLAN_Msk
#define RCC_CLK_DIV_CPU_Pos					(0)
#define RCC_CLK_DIV_CPU_Msk          		(0xFF << RCC_CLK_DIV_CPU_Pos)
#define RCC_CLK_DIV_CPU						RCC_CLK_DIV_CPU_Msk

#define RCC_CLK_SEL_ADC_Pos					(16)
#define RCC_CLK_SEL_ADC_Msk          		(0x1 << RCC_CLK_SEL_ADC_Pos)
#define RCC_CLK_SEL_ADC						RCC_CLK_SEL_ADC_Msk
#define RCC_CLK_SEL_ADC_DIV_Pos				(8)
#define RCC_CLK_SEL_ADC_DIV_Msk          	(0xFF << RCC_CLK_SEL_ADC_DIV_Pos)
#define RCC_CLK_SEL_ADC_DIV					RCC_CLK_SEL_ADC_DIV_Msk
#define RCC_CLK_SEL_QSPI_RAM_Pos			(7)
#define RCC_CLK_SEL_QSPI_RAM_Msk          	(0x1 << RCC_CLK_SEL_QSPI_RAM_Pos)
#define RCC_CLK_SEL_QSPI_RAM				RCC_CLK_SEL_QSPI_RAM_Msk
#define RCC_CLK_SEL_QFLASH_Pos				(6)
#define RCC_CLK_SEL_QFLASH_Msk          	(0x1 << RCC_CLK_SEL_QFLASH_Pos)
#define RCC_CLK_SEL_QFLASH					RCC_CLK_SEL_QFLASH_Msk
#define RCC_CLK_SEL_GPSEC_Pos				(5)
#define RCC_CLK_SEL_GPSEC_Msk          		(0x1 << RCC_CLK_SEL_GPSEC_Pos)
#define RCC_CLK_SEL_GPSEC					RCC_CLK_SEL_GPSEC_Msk
#define RCC_CLK_SEL_RSA_Pos					(4)
#define RCC_CLK_SEL_RSA_Msk          		(0x1 << RCC_CLK_SEL_RSA_Pos)
#define RCC_CLK_SEL_RSA						RCC_CLK_SEL_RSA_Msk
#define RCC_CLK_SEL_DAC_REV_Pos				(3)
#define RCC_CLK_SEL_DAC_REV_Msk          	(0x1 << RCC_CLK_SEL_DAC_REV_Pos)
#define RCC_CLK_SEL_DAC_REV					RCC_CLK_SEL_DAC_REV_Msk
#define RCC_CLK_SEL_ADC_REV_Pos				(2)
#define RCC_CLK_SEL_ADC_REV_Msk          	(0x1 << RCC_CLK_SEL_ADC_REV_Pos)
#define RCC_CLK_SEL_ADC_REV					RCC_CLK_SEL_ADC_REV_Msk
#define RCC_CLK_SEL_TEST_Pos				(1)
#define RCC_CLK_SEL_TEST_Msk          		(0x1 << RCC_CLK_SEL_TEST_Pos)
#define RCC_CLK_SEL_TEST					RCC_CLK_SEL_TEST_Msk
#define RCC_CLK_SEL_ADC_DAC_LOOP_Pos		(0)
#define RCC_CLK_SEL_ADC_DAC_LOOP_Msk        (0x1 << RCC_CLK_SEL_ADC_DAC_LOOP_Pos)
#define RCC_CLK_SEL_ADC_DAC_LOOP			RCC_CLK_SEL_ADC_DAC_LOOP_Msk

#define RCC_I2S_CLK_BCLKDIV_Pos				(8)
#define RCC_I2S_CLK_BCLKDIV_Msk          	(0x3FF << RCC_I2S_CLK_BCLKDIV_Pos)
#define RCC_I2S_CLK_BCLKDIV					RCC_I2S_CLK_BCLKDIV_Msk
#define RCC_I2S_CLK_MCLKDIV_Pos				(2)
#define RCC_I2S_CLK_MCLKDIV_Msk          	(0x3F << RCC_I2S_CLK_MCLKDIV_Pos)
#define RCC_I2S_CLK_MCLKDIV					RCC_I2S_CLK_MCLKDIV_Msk
#define RCC_I2S_CLK_MCLK_EN_Pos				(1)
#define RCC_I2S_CLK_MCLK_EN_Msk          	(0x1 << RCC_I2S_CLK_MCLK_EN_Pos)
#define RCC_I2S_CLK_MCLK_EN					RCC_I2S_CLK_MCLK_EN_Msk
#define RCC_I2S_CLK_EXTAL_EN_Pos			(0)
#define RCC_I2S_CLK_EXTAL_EN_Msk          	(0x1 << RCC_I2S_CLK_EXTAL_EN_Pos)
#define RCC_I2S_CLK_EXTAL_EN				RCC_I2S_CLK_EXTAL_EN_Msk

#define RCC_RST_STATUS_CPU_CLEAR_Pos		(3)
#define RCC_RST_STATUS_CPU_CLEAR_Msk        (0x1 << RCC_RST_STATUS_CPU_CLEAR_Pos)
#define RCC_RST_STATUS_CPU_CLEAR			RCC_RST_STATUS_CPU_CLEAR_Msk
#define RCC_RST_STATUS_WDG_CLEAR_Pos		(2)
#define RCC_RST_STATUS_WDG_CLEAR_Msk        (0x1 << RCC_RST_STATUS_WDG_CLEAR_Pos)
#define RCC_RST_STATUS_WDG_CLEAR			RCC_RST_STATUS_WDG_CLEAR_Msk
#define RCC_RST_STATUS_CPU_Pos				(1)
#define RCC_RST_STATUS_CPU_Msk        		(0x1 << RCC_RST_STATUS_CPU_Pos)
#define RCC_RST_STATUS_CPU					RCC_RST_STATUS_CPU_Msk
#define RCC_RST_STATUS_WDG_Pos				(0)
#define RCC_RST_STATUS_WDG_Msk        		(0x1 << RCC_RST_STATUS_WDG_Pos)
#define RCC_RST_STATUS_WDG					RCC_RST_STATUS_WDG_Msk


/***************************************************************
 * I2C Reg
 ***************************************************************/
 
#define I2C_BASE	(HR_APB_BASE_ADDR + 0x0000)

typedef struct
{
	__IOM uint32_t PRESCALE_L;
	__IOM uint32_t PRESCALE_H;
	__IOM uint32_t EN;
	__IOM uint32_t DATA;
	__IOM uint32_t CR_SR;
} I2C_TypeDef;

#define I2C_PRESCALE_L_Pos					(0)
#define I2C_PRESCALE_L_Msk					(0xFF << I2C_PRESCALE_L_Pos)
#define I2C_PRESCALE_L						I2C_PRESCALE_L_Msk

#define I2C_PRESCALE_H_Pos					(0)
#define I2C_PRESCALE_H_Msk					(0xFF << I2C_PRESCALE_H_Pos)
#define I2C_PRESCALE_H						I2C_PRESCALE_H_Msk

#define I2C_EN_ENABLE_Pos					(7)
#define I2C_EN_ENABLE_Msk					(0x01 << I2C_EN_ENABLE_Pos)
#define I2C_EN_ENABLE						I2C_EN_ENABLE_Msk
#define I2C_EN_IEMASK_Pos					(6)
#define I2C_EN_IEMASK_Msk					(0x01 << I2C_EN_IEMASK_Pos)
#define I2C_EN_IEMASK						I2C_EN_IEMASK_Msk

#define I2C_DATA_Pos						(0)
#define I2C_DATA_Msk						(0x0FF << I2C_DATA_Pos)
#define I2C_DATA							I2C_DATA_MskS

#define I2C_CR_START_Pos					(7)
#define I2C_CR_START_Msk					(0x01 << I2C_CR_START_Pos)
#define I2C_CR_START						I2C_CR_START_Msk
#define I2C_CR_STOP_Pos						(6)
#define I2C_CR_STOP_Msk						(0x01 << I2C_CR_STOP_Pos)
#define I2C_CR_STOP							I2C_CR_STOP_Msk
#define I2C_CR_RD_Pos						(5)
#define I2C_CR_RD_Msk						(0x01 << I2C_CR_RD_Pos)
#define I2C_CR_RD							I2C_CR_RD_Msk
#define I2C_CR_WR_Pos						(4)
#define I2C_CR_WR_Msk						(0x01 << I2C_CR_WR_Pos)
#define I2C_CR_WR							I2C_CR_WR_Msk
#define I2C_CR_ACK_Pos						(3)
#define I2C_CR_ACK_Msk						(0x01 << I2C_CR_ACK_Pos)
#define I2C_CR_ACK							I2C_CR_ACK_Msk
#define I2C_CR_IF_Pos						(0)
#define I2C_CR_IF_Msk						(0x01 << I2C_CR_IF_Pos)
#define I2C_CR_IF							I2C_CR_IF_Msk
#define I2C_SR_RXACK_Pos					(7)
#define I2C_SR_RXACK_Msk					(0x01 << I2C_SR_RXACK_Pos)
#define I2C_SR_RXACK						I2C_SR_RXACK_Msk
#define I2C_SR_BUSY_Pos						(6)
#define I2C_SR_BUSY_Msk						(0x01 << I2C_SR_BUSY_Pos)
#define I2C_SR_BUSY							I2C_SR_BUSY_Msk
#define I2C_SR_TIP_Pos						(1)
#define I2C_SR_TIP_Msk						(0x01 << I2C_SR_TIP_Pos)
#define I2C_SR_TIP							I2C_SR_TIP_Msk
#define I2C_SR_IF_Pos						(0)
#define I2C_SR_IF_Msk						(0x01 << I2C_SR_IF_Pos)
#define I2C_SR_IF							I2C_SR_IF_Msk


/***************************************************************
 * SPI Reg
 ***************************************************************/

#define SPI_BASE	(HR_APB_BASE_ADDR + 0x400)

typedef struct
{
	__IOM uint32_t CH_CFG;
	__IOM uint32_t SPI_CFG;
	__IOM uint32_t CLK_CFG;
	__IOM uint32_t MODE_CFG;
	__IOM uint32_t INT_MASK;
	__IOM uint32_t INT_SRC;
	__IM uint32_t STATUS;
	__IOM uint32_t TIMEOUT;
	__IOM uint32_t TXDATA;
	__IOM uint32_t TRANS_MODE;
	__IM uint32_t SLV_LEN;
	__IM uint32_t RSV;
	__IM uint32_t RXDATA;
} SPI_TypeDef;

#define SPI_CH_CFG_RXINVALIDBIT_Pos			(23)
#define SPI_CH_CFG_RXINVALIDBIT_Msk         (0xFF << SPI_CH_CFG_RXINVALIDBIT_Pos)
#define SPI_CH_CFG_RXINVALIDBIT             SPI_CH_CFG_RXINVALIDBIT_Msk
#define SPI_CH_CFG_CLEARFIFOS_Pos			(22)
#define SPI_CH_CFG_CLEARFIFOS_Msk         	(0x1 << SPI_CH_CFG_CLEARFIFOS_Pos)
#define SPI_CH_CFG_CLEARFIFOS             	SPI_CH_CFG_CLEARFIFOS_Msk
#define SPI_CH_CFG_CONTINUEMODE_Pos			(21)
#define SPI_CH_CFG_CONTINUEMODE_Msk         (0x1 << SPI_CH_CFG_CONTINUEMODE_Pos)
#define SPI_CH_CFG_CONTINUEMODE             SPI_CH_CFG_CONTINUEMODE_Msk
#define SPI_CH_CFG_RXON_Pos					(20)
#define SPI_CH_CFG_RXON_Msk         		(0x1 << SPI_CH_CFG_RXON_Pos)
#define SPI_CH_CFG_RXON             		SPI_CH_CFG_RXON_Msk
#define SPI_CH_CFG_TXON_Pos					(19)
#define SPI_CH_CFG_TXON_Msk         		(0x1 << SPI_CH_CFG_TXON_Pos)
#define SPI_CH_CFG_TXON             		SPI_CH_CFG_TXON_Msk
#define SPI_CH_CFG_LEN_Pos					(3)
#define SPI_CH_CFG_LEN_Msk         			(0xFFFF << SPI_CH_CFG_LEN_Pos)
#define SPI_CH_CFG_LEN             			SPI_CH_CFG_LEN_Msk
#define SPI_CH_CFG_CSLEVEL_Pos				(2)
#define SPI_CH_CFG_CSLEVEL_Msk         		(0x1 << SPI_CH_CFG_CSLEVEL_Pos)
#define SPI_CH_CFG_CSLEVEL             		SPI_CH_CFG_CSLEVEL_Msk
#define SPI_CH_CFG_CSSEL_Pos				(1)
#define SPI_CH_CFG_CSSEL_Msk         		(0x1 << SPI_CH_CFG_CSSEL_Pos)
#define SPI_CH_CFG_CSSEL             		SPI_CH_CFG_CSSEL_Msk
#define SPI_CH_CFG_START_Pos				(0)
#define SPI_CH_CFG_START_Msk         		(0x1 << SPI_CH_CFG_START_Pos)
#define SPI_CH_CFG_START             		SPI_CH_CFG_START_Msk

#define SPI_SPI_CFG_FORMAT_Pos				(17)
#define SPI_SPI_CFG_FORMAT_Msk         		(0x3 << SPI_SPI_CFG_FORMAT_Pos)
#define SPI_SPI_CFG_FORMAT             		SPI_SPI_CFG_FORMAT_Msk
#define SPI_SPI_CFG_FORMAT_MOTO				(0x0 << SPI_SPI_CFG_FORMAT_Pos)
#define SPI_SPI_CFG_FORMAT_TI				(0x1 << SPI_SPI_CFG_FORMAT_Pos)
#define SPI_SPI_CFG_FORMAT_MW				(0x2 << SPI_SPI_CFG_FORMAT_Pos)
#define SPI_SPI_CFG_PINOUT_Pos				(16)
#define SPI_SPI_CFG_PINOUT_Msk         		(0x1 << SPI_SPI_CFG_PINOUT_Pos)
#define SPI_SPI_CFG_PINOUT             		SPI_SPI_CFG_PINOUT_Msk
#define SPI_SPI_CFG_CSHOLD_Pos				(12)
#define SPI_SPI_CFG_CSHOLD_Msk         		(0x7 << SPI_SPI_CFG_CSHOLD_Pos)
#define SPI_SPI_CFG_CSHOLD             		SPI_SPI_CFG_CSHOLD_Msk
#define SPI_SPI_CFG_CSHOLD_1         		(0x0 << SPI_SPI_CFG_CSHOLD_Pos)
#define SPI_SPI_CFG_CSHOLD_2         		(0x1 << SPI_SPI_CFG_CSHOLD_Pos)
#define SPI_SPI_CFG_CSHOLD_4         		(0x2 << SPI_SPI_CFG_CSHOLD_Pos)
#define SPI_SPI_CFG_CSHOLD_8         		(0x3 << SPI_SPI_CFG_CSHOLD_Pos)
#define SPI_SPI_CFG_CSHOLD_16         		(0x4 << SPI_SPI_CFG_CSHOLD_Pos)
#define SPI_SPI_CFG_CSHOLD_32         		(0x5 << SPI_SPI_CFG_CSHOLD_Pos)
#define SPI_SPI_CFG_CSHOLD_64         		(0x6 << SPI_SPI_CFG_CSHOLD_Pos)
#define SPI_SPI_CFG_CSHOLD_127         		(0x7 << SPI_SPI_CFG_CSHOLD_Pos)
#define SPI_SPI_CFG_CSSETUP_Pos				(9)
#define SPI_SPI_CFG_CSSETUP_Msk         	(0x7 << SPI_SPI_CFG_CSSETUP_Pos)
#define SPI_SPI_CFG_CSSETUP             	SPI_SPI_CFG_CSSETUP_Msk
#define SPI_SPI_CFG_CSSETUP_1         		(0x0 << SPI_SPI_CFG_CSSETUP_Pos)
#define SPI_SPI_CFG_CSSETUP_2         		(0x1 << SPI_SPI_CFG_CSSETUP_Pos)
#define SPI_SPI_CFG_CSSETUP_4         		(0x2 << SPI_SPI_CFG_CSSETUP_Pos)
#define SPI_SPI_CFG_CSSETUP_8         		(0x3 << SPI_SPI_CFG_CSSETUP_Pos)
#define SPI_SPI_CFG_CSSETUP_16         		(0x4 << SPI_SPI_CFG_CSSETUP_Pos)
#define SPI_SPI_CFG_CSSETUP_32         		(0x5 << SPI_SPI_CFG_CSSETUP_Pos)
#define SPI_SPI_CFG_CSSETUP_64         		(0x6 << SPI_SPI_CFG_CSSETUP_Pos)
#define SPI_SPI_CFG_CSSETUP_127         	(0x7 << SPI_SPI_CFG_CSSETUP_Pos)
#define SPI_SPI_CFG_OUTDELAY_Pos			(7)
#define SPI_SPI_CFG_OUTDELAY_Msk         	(0x3 << SPI_SPI_CFG_OUTDELAY_Pos)
#define SPI_SPI_CFG_OUTDELAY             	SPI_SPI_CFG_OUTDELAY_Msk
#define SPI_SPI_CFG_OUTDELAY_0         		(0x0 << SPI_SPI_CFG_OUTDELAY_Pos)
#define SPI_SPI_CFG_OUTDELAY_1         		(0x1 << SPI_SPI_CFG_OUTDELAY_Pos)
#define SPI_SPI_CFG_OUTDELAY_2         		(0x2 << SPI_SPI_CFG_OUTDELAY_Pos)
#define SPI_SPI_CFG_OUTDELAY_3         		(0x3 << SPI_SPI_CFG_OUTDELAY_Pos)
#define SPI_SPI_CFG_FRAMEDELAY_Pos			(4)
#define SPI_SPI_CFG_FRAMEDELAY_Msk         	(0x7 << SPI_SPI_CFG_FRAMEDELAY_Pos)
#define SPI_SPI_CFG_FRAMEDELAY             	SPI_SPI_CFG_FRAMEDELAY_Msk
#define SPI_SPI_CFG_FRAMEDELAY_0         	(0x0 << SPI_SPI_CFG_FRAMEDELAY_Pos)
#define SPI_SPI_CFG_FRAMEDELAY_2         	(0x1 << SPI_SPI_CFG_FRAMEDELAY_Pos)
#define SPI_SPI_CFG_FRAMEDELAY_4         	(0x2 << SPI_SPI_CFG_FRAMEDELAY_Pos)
#define SPI_SPI_CFG_FRAMEDELAY_8         	(0x3 << SPI_SPI_CFG_FRAMEDELAY_Pos)
#define SPI_SPI_CFG_FRAMEDELAY_16         	(0x4 << SPI_SPI_CFG_FRAMEDELAY_Pos)
#define SPI_SPI_CFG_FRAMEDELAY_32         	(0x5 << SPI_SPI_CFG_FRAMEDELAY_Pos)
#define SPI_SPI_CFG_FRAMEDELAY_64         	(0x6 << SPI_SPI_CFG_FRAMEDELAY_Pos)
#define SPI_SPI_CFG_FRAMEDELAY_127         	(0x7 << SPI_SPI_CFG_FRAMEDELAY_Pos)
#define SPI_SPI_CFG_BIGENDIAN_Pos			(3)
#define SPI_SPI_CFG_BIGENDIAN_Msk         	(0x1 << SPI_SPI_CFG_BIGENDIAN_Pos)
#define SPI_SPI_CFG_BIGENDIAN             	SPI_SPI_CFG_BIGENDIAN_Msk
#define SPI_SPI_CFG_MASTER_Pos				(2)
#define SPI_SPI_CFG_MASTER_Msk         		(0x1 << SPI_SPI_CFG_MASTER_Pos)
#define SPI_SPI_CFG_MASTER            		SPI_SPI_CFG_MASTER_Msk
#define SPI_SPI_CFG_CPHA_Pos				(1)
#define SPI_SPI_CFG_CPHA_Msk         		(0x1 << SPI_SPI_CFG_CPHA_Pos)
#define SPI_SPI_CFG_CPHA            		SPI_SPI_CFG_CPHA_Msk
#define SPI_SPI_CFG_CPOL_Pos				(0)
#define SPI_SPI_CFG_CPOL_Msk         		(0x1 << SPI_SPI_CFG_CPOL_Pos)
#define SPI_SPI_CFG_CPOL            		SPI_SPI_CFG_CPOL_Msk

#define SPI_CLK_CFG_DIV_Pos					(0)
#define SPI_CLK_CFG_DIV_Msk         		(0xFFFF << SPI_CLK_CFG_DIV_Pos)
#define SPI_CLK_CFG_DIV            			SPI_CLK_CFG_DIV_Msk

#define SPI_MODE_CFG_RXLEVEL_Pos			(6)
#define SPI_MODE_CFG_RXLEVEL_Msk         	(0x7 << SPI_MODE_CFG_RXLEVEL_Pos)
#define SPI_MODE_CFG_RXLEVEL            	SPI_MODE_CFG_RXLEVEL_Msk
#define SPI_MODE_CFG_TXLEVEL_Pos			(2)
#define SPI_MODE_CFG_TXLEVEL_Msk         	(0x7 << SPI_MODE_CFG_TXLEVEL_Pos)
#define SPI_MODE_CFG_TXLEVEL            	SPI_MODE_CFG_TXLEVEL_Msk
#define SPI_MODE_CFG_RXDMA_Pos				(1)
#define SPI_MODE_CFG_RXDMA_Msk         		(0x1 << SPI_MODE_CFG_RXDMA_Pos)
#define SPI_MODE_CFG_RXDMA            		SPI_MODE_CFG_RXDMA_Msk
#define SPI_MODE_CFG_TXDMA_Pos				(0)
#define SPI_MODE_CFG_TXDMA_Msk         		(0x1 << SPI_MODE_CFG_TXDMA_Pos)
#define SPI_MODE_CFG_TXDMA            		SPI_MODE_CFG_TXDMA_Msk

#define SPI_INT_MASK_TIMEOUT_Pos			(7)
#define SPI_INT_MASK_TIMEOUT_Msk         	(0x1 << SPI_INT_MASK_TIMEOUT_Pos)
#define SPI_INT_MASK_TIMEOUT            	SPI_INT_MASK_TIMEOUT_Msk
#define SPI_INT_MASK_DONE_Pos				(6)
#define SPI_INT_MASK_DONE_Msk         		(0x1 << SPI_INT_MASK_DONE_Pos)
#define SPI_INT_MASK_DONE            		SPI_INT_MASK_DONE_Msk
#define SPI_INT_MASK_RXOV_Pos				(5)
#define SPI_INT_MASK_RXOV_Msk         		(0x1 << SPI_INT_MASK_RXOV_Pos)
#define SPI_INT_MASK_RXOV            		SPI_INT_MASK_RXOV_Msk
#define SPI_INT_MASK_RXUN_Pos				(4)
#define SPI_INT_MASK_RXUN_Msk         		(0x1 << SPI_INT_MASK_RXUN_Pos)
#define SPI_INT_MASK_RXUN            		SPI_INT_MASK_RXUN_Msk
#define SPI_INT_MASK_TXOV_Pos				(3)
#define SPI_INT_MASK_TXOV_Msk         		(0x1 << SPI_INT_MASK_TXOV_Pos)
#define SPI_INT_MASK_TXOV            		SPI_INT_MASK_TXOV_Msk
#define SPI_INT_MASK_TXUN_Pos				(2)
#define SPI_INT_MASK_TXUN_Msk         		(0x1 << SPI_INT_MASK_TXUN_Pos)
#define SPI_INT_MASK_TXUN            		SPI_INT_MASK_TXUN_Msk
#define SPI_INT_MASK_RXRDY_Pos				(1)
#define SPI_INT_MASK_RXRDY_Msk         		(0x1 << SPI_INT_MASK_RXRDY_Pos)
#define SPI_INT_MASK_RXRDY            		SPI_INT_MASK_RXRDY_Msk
#define SPI_INT_MASK_TXRDY_Pos				(0)
#define SPI_INT_MASK_TXRDY_Msk         		(0x1 << SPI_INT_MASK_TXRDY_Pos)
#define SPI_INT_MASK_TXRDY            		SPI_INT_MASK_TXRDY_Msk

#define SPI_INT_SRC_TIMEOUT_Pos				(7)
#define SPI_INT_SRC_TIMEOUT_Msk         	(0x1 << SPI_INT_SRC_TIMEOUT_Pos)
#define SPI_INT_SRC_TIMEOUT            		SPI_INT_SRC_TIMEOUT_Msk
#define SPI_INT_SRC_DONE_Pos				(6)
#define SPI_INT_SRC_DONE_Msk         		(0x1 << SPI_INT_SRC_DONE_Pos)
#define SPI_INT_SRC_DONE            		SPI_INT_SRC_DONE_Msk
#define SPI_INT_SRC_RXOV_Pos				(5)
#define SPI_INT_SRC_RXOV_Msk         		(0x1 << SPI_INT_SRC_RXOV_Pos)
#define SPI_INT_SRC_RXOV            		SPI_INT_SRC_RXOV_Msk
#define SPI_INT_SRC_RXUN_Pos				(4)
#define SPI_INT_SRC_RXUN_Msk         		(0x1 << SPI_INT_SRC_RXUN_Pos)
#define SPI_INT_SRC_RXUN            		SPI_INT_SRC_RXUN_Msk
#define SPI_INT_SRC_TXOV_Pos				(3)
#define SPI_INT_SRC_TXOV_Msk         		(0x1 << SPI_INT_SRC_TXOV_Pos)
#define SPI_INT_SRC_TXOV            		SPI_INT_SRC_TXOV_Msk
#define SPI_INT_SRC_TXUN_Pos				(2)
#define SPI_INT_SRC_TXUN_Msk         		(0x1 << SPI_INT_SRC_TXUN_Pos)
#define SPI_INT_SRC_TXUN            		SPI_INT_SRC_TXUN_Msk
#define SPI_INT_SRC_RXRDY_Pos				(1)
#define SPI_INT_SRC_RXRDY_Msk         		(0x1 << SPI_INT_SRC_RXRDY_Pos)
#define SPI_INT_SRC_RXRDY            		SPI_INT_SRC_RXRDY_Msk
#define SPI_INT_SRC_TXRDY_Pos				(0)
#define SPI_INT_SRC_TXRDY_Msk         		(0x1 << SPI_INT_SRC_TXRDY_Pos)
#define SPI_INT_SRC_TXRDY            		SPI_INT_SRC_TXRDY_Msk

#define SPI_STATUS_BUSY_Pos					(12)
#define SPI_STATUS_BUSY_Msk         		(0x1 << SPI_STATUS_BUSY_Pos)
#define SPI_STATUS_BUSY            			SPI_STATUS_BUSY_Msk
#define SPI_STATUS_RXFIFO_Pos				(6)
#define SPI_STATUS_RXFIFO_Msk         		(0x3F << SPI_STATUS_RXFIFO_Pos)
#define SPI_STATUS_RXFIFO            		SPI_STATUS_RXFIFO_Msk
#define SPI_STATUS_TXFIFO_Pos				(0)
#define SPI_STATUS_TXFIFO_Msk         		(0x3F << SPI_STATUS_TXFIFO_Pos)
#define SPI_STATUS_TXFIFO            		SPI_STATUS_TXFIFO_Msk

#define SPI_TIMEOUT_TIMEREN_Pos				(31)
#define SPI_TIMEOUT_TIMEREN_Msk         	(0x1 << SPI_TIMEOUT_TIMEREN_Pos)
#define SPI_TIMEOUT_TIMEREN            		SPI_TIMEOUT_TIMEREN_Msk
#define SPI_TIMEOUT_TIMERVAL_Pos			(0)
#define SPI_TIMEOUT_TIMERVAL_Msk         	(0x7FFFFFFF << SPI_TIMEOUT_TIMERVAL_Pos)
#define SPI_TIMEOUT_TIMERVAL            	SPI_TIMEOUT_TIMERVAL_Msk

#define SPI_TRANS_MODE_BLKLEN_Pos			(24)
#define SPI_TRANS_MODE_BLKLEN_Msk         	(0x3F << SPI_TRANS_MODE_BLKLEN_Pos)
#define SPI_TRANS_MODE_BLKLEN            	SPI_TRANS_MODE_BLKLEN_Msk
#define SPI_TRANS_MODE_MICROBURST_Pos		(16)
#define SPI_TRANS_MODE_MICROBURST_Msk       (0x1 << SPI_TRANS_MODE_MICROBURST_Pos)
#define SPI_TRANS_MODE_MICROBURST           SPI_TRANS_MODE_MICROBURST_Msk
#define SPI_TRANS_MODE_MICRODATLEN_Pos		(8)
#define SPI_TRANS_MODE_MICRODATLEN_Msk      (0x3F << SPI_TRANS_MODE_MICRODATLEN_Pos)
#define SPI_TRANS_MODE_MICRODATLEN          SPI_TRANS_MODE_MICRODATLEN_Msk
#define SPI_TRANS_MODE_MICROCONTROLLEN_Pos	(0)
#define SPI_TRANS_MODE_MICROCONTROLLEN_Msk  (0x3F << SPI_TRANS_MODE_MICROCONTROLLEN_Pos)
#define SPI_TRANS_MODE_MICROCONTROLLEN      SPI_TRANS_MODE_MICROCONTROLLEN_Msk

#define SPI_SLV_LEN_TXLEN_Pos				(16)
#define SPI_SLV_LEN_TXLEN_Msk  				(0xFFFF << SPI_SLV_LEN_TXLEN_Pos)
#define SPI_SLV_LEN_TXLEN      				SPI_SLV_LEN_TXLEN_Msk
#define SPI_SLV_LEN_RXLEN_Pos				(0)
#define SPI_SLV_LEN_RXLEN_Msk  				(0xFFFF << SPI_SLV_LEN_RXLEN_Pos)
#define SPI_SLV_LEN_RXLEN      				SPI_SLV_LEN_RXLEN_Msk


/***************************************************************
 * UART Reg
 ***************************************************************/

#define UART0_BASE	(HR_APB_BASE_ADDR + 0x600)
#define UART1_BASE	(HR_APB_BASE_ADDR + 0x800)
#define UART2_BASE	(HR_APB_BASE_ADDR + 0xA00)
#define UART3_BASE	(HR_APB_BASE_ADDR + 0xC00)
#define UART4_BASE	(HR_APB_BASE_ADDR + 0xE00)
#define UART5_BASE	(HR_APB_BASE_ADDR + 0x1000)

typedef struct
{
	__IOM uint32_t LC;
	__IOM uint32_t FC;
	__IOM uint32_t DMAC;
	__IOM uint32_t FIFOC;
	__IOM uint32_t BAUDR;
	__IOM uint32_t INTM;
	__IOM uint32_t INTS;
	__IOM uint32_t FIFOS;
	__OM uint32_t TDW;
	__IOM uint32_t RSV1;
	__IOM uint32_t RSV2;
	__IOM uint32_t RSV3;
	__IM uint32_t RDW;
} USART_TypeDef;

#define UART_LC_RE_Pos						(7)
#define UART_LC_RE_Msk                     	(0x1 << UART_LC_RE_Pos)
#define UART_LC_RE                         	UART_LC_RE_Msk
#define UART_LC_TE_Pos                     	(6)
#define UART_LC_TE_Msk                     	(0x1 << UART_LC_TE_Pos)
#define UART_LC_TE                         	UART_LC_TE_Msk
#define UART_LC_SBE_Pos                    	(5)
#define UART_LC_SBE_Msk                    	(0x1 << UART_LC_SBE_Pos)
#define UART_LC_SBE                        	UART_LC_SBE_Msk
#define UART_LC_PS_Pos                     	(4)
#define UART_LC_PS_Msk                     	(0x1 << UART_LC_PS_Pos)
#define UART_LC_PS                         	UART_LC_PS_Msk
#define UART_LC_PCE_Pos                    	(3)
#define UART_LC_PCE_Msk                    	(0x1 << UART_LC_PCE_Pos)
#define UART_LC_PCE                        	UART_LC_PCE_Msk
#define UART_LC_STOP_Pos                   	(2)
#define UART_LC_STOP_Msk                   	(0x1 << UART_LC_STOP_Pos)
#define UART_LC_STOP                       	UART_LC_STOP_Msk
#define UART_LC_DATAL_Pos                  	(0)
#define UART_LC_DATAL_Msk                  	(0x3 << UART_LC_DATAL_Pos)
#define UART_LC_DATAL                      	UART_LC_DATAL_Msk
#define UART_LC_DATAL_5BIT					(0x0 << UART_LC_DATAL_Pos)
#define UART_LC_DATAL_6BIT					(0x1 << UART_LC_DATAL_Pos)
#define UART_LC_DATAL_7BIT					(0x2 << UART_LC_DATAL_Pos)
#define UART_LC_DATAL_8BIT					(0x3 << UART_LC_DATAL_Pos)

#define UART_FC_RTSL_Pos					(2)
#define UART_FC_RTSL_Msk                    (0x7 << UART_FC_RTSL_Pos)
#define UART_FC_RTSL                        UART_FC_RTSL_Msk
#define UART_FC_RTSL_4                     	(0x0 << UART_FC_RTSL_Pos)
#define UART_FC_RTSL_8                     	(0x1 << UART_FC_RTSL_Pos)
#define UART_FC_RTSL_12                     (0x2 << UART_FC_RTSL_Pos)
#define UART_FC_RTSL_16                     (0x3 << UART_FC_RTSL_Pos)
#define UART_FC_RTSL_20                     (0x4 << UART_FC_RTSL_Pos)
#define UART_FC_RTSL_24                     (0x5 << UART_FC_RTSL_Pos)
#define UART_FC_RTSL_28                     (0x6 << UART_FC_RTSL_Pos)
#define UART_FC_RTSL_31                     (0x7 << UART_FC_RTSL_Pos)
#define UART_FC_RTSS_Pos					(1)
#define UART_FC_RTSS_Msk                    (0x1 << UART_FC_RTSS_Pos)
#define UART_FC_RTSS                        UART_FC_RTSS_Msk
#define UART_FC_AFCE_Pos					(0)
#define UART_FC_AFCE_Msk                    (0x1 << UART_FC_AFCE_Pos)
#define UART_FC_AFCE                        UART_FC_AFCE_Msk

#define UART_DMAC_RTO_Pos					(3)
#define UART_DMAC_RTO_Msk                   (0x1F << UART_DMAC_RTO_Pos)
#define UART_DMAC_RTO                       UART_DMAC_RTO_Msk
#define UART_DMAC_RTOE_Pos					(2)
#define UART_DMAC_RTOE_Msk                  (0x1 << UART_DMAC_RTOE_Pos)
#define UART_DMAC_RTOE                      UART_DMAC_RTOE_Msk
#define UART_DMAC_RDE_Pos					(1)
#define UART_DMAC_RDE_Msk                   (0x1 << UART_DMAC_RDE_Pos)
#define UART_DMAC_RDE                       UART_DMAC_RDE_Msk
#define UART_DMAC_TDE_Pos					(0)
#define UART_DMAC_TDE_Msk                   (0x1 << UART_DMAC_TDE_Pos)
#define UART_DMAC_TDE                       UART_DMAC_TDE_Msk

#define UART_FIFOC_RFL_Pos					(4)
#define UART_FIFOC_RFL_Msk                  (0x3 << UART_FIFOC_RFL_Pos)
#define UART_FIFOC_RFL                      UART_FIFOC_RFL_Msk
#define UART_FIFOC_RFL_1                  	(0x0 << UART_FIFOC_RFL_Pos)
#define UART_FIFOC_RFL_4                  	(0x1 << UART_FIFOC_RFL_Pos)
#define UART_FIFOC_RFL_8                  	(0x2 << UART_FIFOC_RFL_Pos)
#define UART_FIFOC_RFL_16                  	(0x3 << UART_FIFOC_RFL_Pos)
#define UART_FIFOC_TFL_Pos					(2)
#define UART_FIFOC_TFL_Msk                  (0x3 << UART_FIFOC_TFL_Pos)
#define UART_FIFOC_TFL                      UART_FIFOC_TFL_Msk
#define UART_FIFOC_TFL_0                  	(0x0 << UART_FIFOC_TFL_Pos)
#define UART_FIFOC_TFL_4                  	(0x1 << UART_FIFOC_TFL_Pos)
#define UART_FIFOC_TFL_8                  	(0x2 << UART_FIFOC_TFL_Pos)
#define UART_FIFOC_TFL_16                  	(0x3 << UART_FIFOC_TFL_Pos)
#define UART_FIFOC_RFRST_Pos				(1)
#define UART_FIFOC_RFRST_Msk                (0x1 << UART_FIFOC_RFRST_Pos)
#define UART_FIFOC_RFRST                    UART_FIFOC_RFRST_Msk
#define UART_FIFOC_TFRST_Pos				(0)
#define UART_FIFOC_TFRST_Msk                (0x1 << UART_FIFOC_TFRST_Pos)
#define UART_FIFOC_TFRST                    UART_FIFOC_TFRST_Msk

#define UART_BAUDR_DIVF_Pos					(16)
#define UART_BAUDR_DIVF_Msk                	(0xF << UART_BAUDR_DIVF_Pos)
#define UART_BAUDR_DIVF                    	UART_BAUDR_DIVF_Msk
#define UART_BAUDR_DIV_Pos					(0)
#define UART_BAUDR_DIV_Msk                	(0xFFFF << UART_BAUDR_DIV_Pos)
#define UART_BAUDR_DIV                    	UART_BAUDR_DIV_Msk

#define UART_INTM_OE_Pos					(8)
#define UART_INTM_OE_Msk                	(0x1 << UART_INTM_OE_Pos)
#define UART_INTM_OE                    	UART_INTM_OE_Msk
#define UART_INTM_PE_Pos					(7)
#define UART_INTM_PE_Msk                	(0x1 << UART_INTM_PE_Pos)
#define UART_INTM_PE                    	UART_INTM_PE_Msk
#define UART_INTM_FE_Pos					(6)
#define UART_INTM_FE_Msk                	(0x1 << UART_INTM_FE_Pos)
#define UART_INTM_FE                    	UART_INTM_FE_Msk
#define UART_INTM_BD_Pos					(5)
#define UART_INTM_BD_Msk                	(0x1 << UART_INTM_BD_Pos)
#define UART_INTM_BD                    	UART_INTM_BD_Msk
#define UART_INTM_CTS_Pos					(4)
#define UART_INTM_CTS_Msk                	(0x1 << UART_INTM_CTS_Pos)
#define UART_INTM_CTS                    	UART_INTM_CTS_Msk
#define UART_INTM_RTO_Pos					(3)
#define UART_INTM_RTO_Msk                	(0x1 << UART_INTM_RTO_Pos)
#define UART_INTM_RTO                    	UART_INTM_RTO_Msk
#define UART_INTM_RL_Pos					(2)
#define UART_INTM_RL_Msk                	(0x1 << UART_INTM_RL_Pos)
#define UART_INTM_RL                    	UART_INTM_RL_Msk
#define UART_INTM_TL_Pos					(1)
#define UART_INTM_TL_Msk                	(0x1 << UART_INTM_TL_Pos)
#define UART_INTM_TL                    	UART_INTM_TL_Msk
#define UART_INTM_TEMPT_Pos					(0)
#define UART_INTM_TEMPT_Msk                	(0x1 << UART_INTM_TEMPT_Pos)
#define UART_INTM_TEMPT                    	UART_INTM_TEMPT_Msk

#define UART_INTS_OE_Pos					(8)
#define UART_INTS_OE_Msk                	(0x1 << UART_INTS_OE_Pos)
#define UART_INTS_OE                    	UART_INTS_OE_Msk
#define UART_INTS_PE_Pos					(7)
#define UART_INTS_PE_Msk                	(0x1 << UART_INTS_PE_Pos)
#define UART_INTS_PE                    	UART_INTS_PE_Msk
#define UART_INTS_FE_Pos					(6)
#define UART_INTS_FE_Msk                	(0x1 << UART_INTS_FE_Pos)
#define UART_INTS_FE                    	UART_INTS_FE_Msk
#define UART_INTS_BD_Pos					(5)
#define UART_INTS_BD_Msk                	(0x1 << UART_INTS_BD_Pos)
#define UART_INTS_BD                    	UART_INTS_BD_Msk
#define UART_INTS_CTS_Pos					(4)
#define UART_INTS_CTS_Msk                	(0x1 << UART_INTS_CTS_Pos)
#define UART_INTS_CTS                    	UART_INTS_CTS_Msk
#define UART_INTS_RTO_Pos					(3)
#define UART_INTS_RTO_Msk                	(0x1 << UART_INTS_RTO_Pos)
#define UART_INTS_RTO                    	UART_INTS_RTO_Msk
#define UART_INTS_RL_Pos					(2)
#define UART_INTS_RL_Msk                	(0x1 << UART_INTS_RL_Pos)
#define UART_INTS_RL                    	UART_INTS_RL_Msk
#define UART_INTS_TL_Pos					(1)
#define UART_INTS_TL_Msk                	(0x1 << UART_INTS_TL_Pos)
#define UART_INTS_TL                    	UART_INTS_TL_Msk
#define UART_INTS_TEMPT_Pos					(0)
#define UART_INTS_TEMPT_Msk                	(0x1 << UART_INTS_TEMPT_Pos)
#define UART_INTS_TEMPT                    	UART_INTS_TEMPT_Msk

#define UART_FIFOS_CTSS_Pos					(12)
#define UART_FIFOS_CTSS_Msk                	(0x1 << UART_FIFOS_CTSS_Pos)
#define UART_FIFOS_CTSS                    	UART_FIFOS_CTSS_Msk
#define UART_FIFOS_RFC_Pos					(6)
#define UART_FIFOS_RFC_Msk                	(0x3F << UART_FIFOS_RFC_Pos)
#define UART_FIFOS_RFC                    	UART_FIFOS_RFC_Msk
#define UART_FIFOS_TFC_Pos					(0)
#define UART_FIFOS_TFC_Msk                	(0x3F << UART_FIFOS_TFC_Pos)
#define UART_FIFOS_TFC                    	UART_FIFOS_TFC_Msk


/***************************************************************
 * TIM Reg
 ***************************************************************/
 
#define TIM_BASE		(HR_APB_BASE_ADDR + 0x00001800)

typedef struct
{
	__IOM uint32_t TMR_CONFIG;
	__IOM uint32_t CR;
	__IOM uint32_t TIM0_PRD;
	__IOM uint32_t TIM1_PRD;
	__IOM uint32_t TIM2_PRD;
	__IOM uint32_t TIM3_PRD;
	__IOM uint32_t TIM4_PRD;
	__IOM uint32_t TIM5_PRD;
	__IM uint32_t TIM0_CNT;
	__IM uint32_t TIM1_CNT;
	__IM uint32_t TIM2_CNT;
	__IM uint32_t TIM3_CNT;
	__IM uint32_t TIM4_CNT;
	__IM uint32_t TIM5_CNT;
	
} TIM_TypeDef;

#define TIM_TMR_CONFIG_DIV_Pos					(0)
#define TIM_TMR_CONFIG_DIV_Msk					(0x7F << TIM_TMR_CONFIG_DIV_Pos)
#define TIM_TMR_CONFIG_DIV						TIM_TMR_CONFIG_DIV_Msk

#define TIM_CR_TIM5_TIF_Pos						(29)
#define TIM_CR_TIM5_TIF_Msk						(0x1 << TIM_CR_TIM5_TIF_Pos)
#define TIM_CR_TIM5_TIF							TIM_CR_TIM5_TIF_Msk
#define TIM_CR_TIM5_TIE_Pos						(28)
#define TIM_CR_TIM5_TIE_Msk						(0x1 << TIM_CR_TIM5_TIE_Pos)
#define TIM_CR_TIM5_TIE							TIM_CR_TIM5_TIE_Msk
#define TIM_CR_TIM5_EN_Pos						(27)
#define TIM_CR_TIM5_EN_Msk						(0x1 << TIM_CR_TIM5_EN_Pos)
#define TIM_CR_TIM5_EN							TIM_CR_TIM5_EN_Msk
#define TIM_CR_TIM5_MODE_Pos					(26)
#define TIM_CR_TIM5_MODE_Msk					(0x1 << TIM_CR_TIM5_MODE_Pos)
#define TIM_CR_TIM5_MODE						TIM_CR_TIM5_MODE_Msk
#define TIM_CR_TIM5_UNIT_Pos					(25)
#define TIM_CR_TIM5_UNIT_Msk					(0x1 << TIM_CR_TIM5_UNIT_Pos)
#define TIM_CR_TIM5_UNIT						TIM_CR_TIM5_UNIT_Msk
#define TIM_CR_TIM4_TIF_Pos						(24)
#define TIM_CR_TIM4_TIF_Msk						(0x1 << TIM_CR_TIM4_TIF_Pos)
#define TIM_CR_TIM4_TIF							TIM_CR_TIM4_TIF_Msk
#define TIM_CR_TIM4_TIE_Pos						(23)
#define TIM_CR_TIM4_TIE_Msk						(0x1 << TIM_CR_TIM4_TIE_Pos)
#define TIM_CR_TIM4_TIE							TIM_CR_TIM4_TIE_Msk
#define TIM_CR_TIM4_EN_Pos						(22)
#define TIM_CR_TIM4_EN_Msk						(0x1 << TIM_CR_TIM4_EN_Pos)
#define TIM_CR_TIM4_EN							TIM_CR_TIM4_EN_Msk
#define TIM_CR_TIM4_MODE_Pos					(21)
#define TIM_CR_TIM4_MODE_Msk					(0x1 << TIM_CR_TIM4_MODE_Pos)
#define TIM_CR_TIM4_MODE						TIM_CR_TIM4_MODE_Msk
#define TIM_CR_TIM4_UNIT_Pos					(20)
#define TIM_CR_TIM4_UNIT_Msk					(0x1 << TIM_CR_TIM4_UNIT_Pos)
#define TIM_CR_TIM4_UNIT						TIM_CR_TIM4_UNIT_Msk
#define TIM_CR_TIM3_TIF_Pos						(19)
#define TIM_CR_TIM3_TIF_Msk						(0x1 << TIM_CR_TIM3_TIF_Pos)
#define TIM_CR_TIM3_TIF							TIM_CR_TIM3_TIF_Msk
#define TIM_CR_TIM3_TIE_Pos						(18)
#define TIM_CR_TIM3_TIE_Msk						(0x1 << TIM_CR_TIM3_TIE_Pos)
#define TIM_CR_TIM3_TIE							TIM_CR_TIM3_TIE_Msk
#define TIM_CR_TIM3_EN_Pos						(17)
#define TIM_CR_TIM3_EN_Msk						(0x1 << TIM_CR_TIM3_EN_Pos)
#define TIM_CR_TIM3_EN							TIM_CR_TIM3_EN_Msk
#define TIM_CR_TIM3_MODE_Pos					(16)
#define TIM_CR_TIM3_MODE_Msk					(0x1 << TIM_CR_TIM3_MODE_Pos)
#define TIM_CR_TIM3_MODE						TIM_CR_TIM3_MODE_Msk
#define TIM_CR_TIM3_UNIT_Pos					(15)
#define TIM_CR_TIM3_UNIT_Msk					(0x1 << TIM_CR_TIM3_UNIT_Pos)
#define TIM_CR_TIM3_UNIT						TIM_CR_TIM3_UNIT_Msk
#define TIM_CR_TIM2_TIF_Pos						(14)
#define TIM_CR_TIM2_TIF_Msk						(0x1 << TIM_CR_TIM2_TIF_Pos)
#define TIM_CR_TIM2_TIF							TIM_CR_TIM2_TIF_Msk
#define TIM_CR_TIM2_TIE_Pos						(13)
#define TIM_CR_TIM2_TIE_Msk						(0x1 << TIM_CR_TIM2_TIE_Pos)
#define TIM_CR_TIM2_TIE							TIM_CR_TIM2_TIE_Msk
#define TIM_CR_TIM2_EN_Pos						(12)
#define TIM_CR_TIM2_EN_Msk						(0x1 << TIM_CR_TIM2_EN_Pos)
#define TIM_CR_TIM2_EN							TIM_CR_TIM2_EN_Msk
#define TIM_CR_TIM2_MODE_Pos					(11)
#define TIM_CR_TIM2_MODE_Msk					(0x1 << TIM_CR_TIM2_MODE_Pos)
#define TIM_CR_TIM2_MODE						TIM_CR_TIM2_MODE_Msk
#define TIM_CR_TIM2_UNIT_Pos					(10)
#define TIM_CR_TIM2_UNIT_Msk					(0x1 << TIM_CR_TIM2_UNIT_Pos)
#define TIM_CR_TIM2_UNIT						TIM_CR_TIM2_UNIT_Msk
#define TIM_CR_TIM1_TIF_Pos						(9)
#define TIM_CR_TIM1_TIF_Msk						(0x1 << TIM_CR_TIM1_TIF_Pos)
#define TIM_CR_TIM1_TIF							TIM_CR_TIM1_TIF_Msk
#define TIM_CR_TIM1_TIE_Pos						(8)
#define TIM_CR_TIM1_TIE_Msk						(0x1 << TIM_CR_TIM1_TIE_Pos)
#define TIM_CR_TIM1_TIE							TIM_CR_TIM1_TIE_Msk
#define TIM_CR_TIM1_EN_Pos						(7)
#define TIM_CR_TIM1_EN_Msk						(0x1 << TIM_CR_TIM1_EN_Pos)
#define TIM_CR_TIM1_EN							TIM_CR_TIM1_EN_Msk
#define TIM_CR_TIM1_MODE_Pos					(6)
#define TIM_CR_TIM1_MODE_Msk					(0x1 << TIM_CR_TIM1_MODE_Pos)
#define TIM_CR_TIM1_MODE						TIM_CR_TIM1_MODE_Msk
#define TIM_CR_TIM1_UNIT_Pos					(5)
#define TIM_CR_TIM1_UNIT_Msk					(0x1 << TIM_CR_TIM1_UNIT_Pos)
#define TIM_CR_TIM1_UNIT						TIM_CR_TIM1_UNIT_Msk
#define TIM_CR_TIM0_TIF_Pos						(4)
#define TIM_CR_TIM0_TIF_Msk						(0x1 << TIM_CR_TIM0_TIF_Pos)
#define TIM_CR_TIM0_TIF							TIM_CR_TIM0_TIF_Msk
#define TIM_CR_TIM0_TIE_Pos						(3)
#define TIM_CR_TIM0_TIE_Msk						(0x1 << TIM_CR_TIM0_TIE_Pos)
#define TIM_CR_TIM0_TIE							TIM_CR_TIM0_TIE_Msk
#define TIM_CR_TIM0_EN_Pos						(2)
#define TIM_CR_TIM0_EN_Msk						(0x1 << TIM_CR_TIM0_EN_Pos)
#define TIM_CR_TIM0_EN							TIM_CR_TIM0_EN_Msk
#define TIM_CR_TIM0_MODE_Pos					(1)
#define TIM_CR_TIM0_MODE_Msk					(0x1 << TIM_CR_TIM0_MODE_Pos)
#define TIM_CR_TIM0_MODE						TIM_CR_TIM0_MODE_Msk
#define TIM_CR_TIM0_UNIT_Pos					(0)
#define TIM_CR_TIM0_UNIT_Msk					(0x1 << TIM_CR_TIM0_UNIT_Pos)
#define TIM_CR_TIM0_UNIT						TIM_CR_TIM0_UNIT_Msk

#define TIM_CR_TIM_TIF_Pos(n)					(4  + 5 * n)
#define TIM_CR_TIM_TIF_Msk(n)					(0x1 << TIM_CR_TIM_TIF_Pos(n))
#define TIM_CR_TIM_TIF(n)						TIM_CR_TIM_TIF_Msk(n)
#define TIM_CR_TIM_TIE_Pos(n)					(3  + 5 * n)
#define TIM_CR_TIM_TIE_Msk(n)					(0x1 << TIM_CR_TIM_TIE_Pos(n))
#define TIM_CR_TIM_TIE(n)						TIM_CR_TIM_TIE_Msk(n)
#define TIM_CR_TIM_EN_Pos(n)					(2  + 5 * n)
#define TIM_CR_TIM_EN_Msk(n)					(0x1 << TIM_CR_TIM_EN_Pos(n))
#define TIM_CR_TIM_EN(n)						TIM_CR_TIM_EN_Msk(n)
#define TIM_CR_TIM_MODE_Pos(n)					(1  + 5 * n)
#define TIM_CR_TIM_MODE_Msk(n)					(0x1 << TIM_CR_TIM_MODE_Pos(n))
#define TIM_CR_TIM_MODE(n)						TIM_CR_TIM_MODE_Msk(n)
#define TIM_CR_TIM_UNIT_Pos(n)					(0  + 5 * n)
#define TIM_CR_TIM_UNIT_Msk(n)					(0x1 << TIM_CR_TIM_UNIT_Pos(n))
#define TIM_CR_TIM_UNIT(n)						TIM_CR_TIM_UNIT_Msk(n)

#define TIM_TIM0_PRD_Pos						(0)
#define TIM_TIM0_PRD_Msk						(0xFFFFFFFF << TIM_TIM0_PRD_Pos)
#define TIM_TIM0_PRD							TIM_TIM0_PRD_Msk

#define TIM_TIM1_PRD_Pos						(0)
#define TIM_TIM1_PRD_Msk						(0xFFFFFFFF << TIM_TIM1_PRD_Pos)
#define TIM_TIM1_PRD							TIM_TIM1_PRD_Msk

#define TIM_TIM2_PRD_Pos						(0)
#define TIM_TIM2_PRD_Msk						(0xFFFFFFFF << TIM_TIM2_PRD_Pos)
#define TIM_TIM2_PRD							TIM_TIM2_PRD_Msk

#define TIM_TIM3_PRD_Pos						(0)
#define TIM_TIM3_PRD_Msk						(0xFFFFFFFF << TIM_TIM3_PRD_Pos)
#define TIM_TIM3_PRD							TIM_TIM3_PRD_Msk

#define TIM_TIM4_PRD_Pos						(0)
#define TIM_TIM4_PRD_Msk						(0xFFFFFFFF << TIM_TIM4_PRD_Pos)
#define TIM_TIM4_PRD							TIM_TIM4_PRD_Msk

#define TIM_TIM5_PRD_Pos						(0)
#define TIM_TIM5_PRD_Msk						(0xFFFFFFFF << TIM_TIM5_PRD_Pos)
#define TIM_TIM5_PRD							TIM_TIM5_PRD_Msk

#define TIM_TIM0_CNT_Pos						(0)
#define TIM_TIM0_CNT_Msk						(0xFFFFFFFF << TIM_TIM0_CNT_Pos)
#define TIM_TIM0_CNT							TIM_TIM0_CNT_Msk

#define TIM_TIM1_CNT_Pos						(0)
#define TIM_TIM1_CNT_Msk						(0xFFFFFFFF << TIM_TIM1_CNT_Pos)
#define TIM_TIM1_CNT							TIM_TIM1_CNT_Msk

#define TIM_TIM2_CNT_Pos						(0)
#define TIM_TIM2_CNT_Msk						(0xFFFFFFFF << TIM_TIM2_CNT_Pos)
#define TIM_TIM2_CNT							TIM_TIM2_CNT_Msk

#define TIM_TIM3_CNT_Pos						(0)
#define TIM_TIM3_CNT_Msk						(0xFFFFFFFF << TIM_TIM3_CNT_Pos)
#define TIM_TIM3_CNT							TIM_TIM3_CNT_Msk

#define TIM_TIM4_CNT_Pos						(0)
#define TIM_TIM4_CNT_Msk						(0xFFFFFFFF << TIM_TIM4_CNT_Pos)
#define TIM_TIM4_CNT							TIM_TIM4_CNT_Msk

#define TIM_TIM5_CNT_Pos						(0)
#define TIM_TIM5_CNT_Msk						(0xFFFFFFFF << TIM_TIM5_CNT_Pos)
#define TIM_TIM5_CNT							TIM_TIM5_CNT_Msk


/***************************************************************
 * WDG Reg
 ***************************************************************/

#define WDG_BASE				(HR_APB_BASE_ADDR + 0x1600)	

typedef struct
{
	__IOM uint32_t LD;
	__IM uint32_t VAL;
	__IOM uint32_t CR;
	__OM uint32_t CLR;
	__IM uint32_t SRC;
	__IM uint32_t STATE;
} WDG_TypeDef;

#define WDG_LD_Pos						(0)
#define WDG_LD_Msk						(0xFFFFFFFF << WDG_LD_Pos)
#define WDG_LD							WDG_LD_Msk

#define WDG_VAL_Pos						(0)
#define WDG_VAL_Msk						(0xFFFFFFFF << WDG_VAL_Pos)
#define WDG_VAL							WDG_VAL_Msk

#define WDG_CR_RST_EN_Pos				(1)
#define WDG_CR_RST_EN_Msk				(0x1 << WDG_CR_RST_EN_Pos)
#define WDG_CR_RST_EN					WDG_CR_RST_EN_Msk
#define WDG_CR_EN_Pos					(0)
#define WDG_CR_EN_Msk					(0x1 << WDG_CR_EN_Pos)
#define WDG_CR_EN						WDG_CR_EN_Msk

#define WDG_CLR_Pos						(0)
#define WDG_CLR_Msk						(0x1 << WDG_CLR_Pos)
#define WDG_CLR							WDG_CLR_Msk

#define WDG_SRC_Pos						(0)
#define WDG_SRC_Msk						(0x1 << WDG_SRC_Pos)
#define WDG_SRC							WDG_SRC_Msk

#define WDG_STATE_Pos					(0)
#define WDG_STATE_Msk					(0x1 << WDG_STATE_Pos)
#define WDG_STATE						WDG_STATE_Msk


/***************************************************************
 * LCD Reg
 ***************************************************************/
#define LCD_REG_BASE     		 (HR_APB_BASE_ADDR + 0x1C00) //(0x4001 1C00)

typedef struct
{
	__IO uint32_t CTRL;     /** Control Register */
	__IO uint32_t FRAMECNT; /** Refresh Rate Prescaler Register */
	__IO uint32_t RAM0;
	__IO uint32_t RAM1;
	__IO uint32_t RAM2;
	__IO uint32_t RAM3;
	__IO uint32_t RAM4;
	__IO uint32_t RAM5;
	__IO uint32_t RAM6;
	__IO uint32_t RAM7;
	__IO uint32_t COM_EN;   /** LCD COM Control Register */
	__IO uint32_t SEG_EN;   /** LCD SEG Control Register */
} LCD_T;

/** LCD base pointer */
#define LCD 		((LCD_T *)LCD_REG_BASE)

/***************************************************************
 * PWM Reg
 ***************************************************************/
 
#define PWM_BASE				(HR_APB_BASE_ADDR + 0x1E00)

typedef struct
{
	__IOM uint32_t CLKDIV01;
	__IOM uint32_t CLKDIV23;
	__IOM uint32_t CR;
	__IOM uint32_t PERIOD;
	__IOM uint32_t PNUM;
	__IOM uint32_t CMPDAT;
	__IOM uint32_t DTCR;
	__IOM uint32_t IE;
	__IOM uint32_t IF;
	__IM uint32_t CH0CAPDAT;
	__IOM uint32_t BKCR;
	__IOM uint32_t CH4CR1;
	__IOM uint32_t CH4CR2;
	__IM uint32_t CH4CAPDAT;
	__IOM uint32_t CH4CR3;
	
} PWM_TypeDef;

#define PWM_CLKDIV01_CH1_Pos			(16)
#define PWM_CLKDIV01_CH1_Msk			(0xFFFF << PWM_CLKDIV01_CH1_Pos)
#define PWM_CLKDIV01_CH1				PWM_CLKDIV01_CH1_Msk
#define PWM_CLKDIV01_CH0_Pos			(0)
#define PWM_CLKDIV01_CH0_Msk			(0xFFFF << PWM_CLKDIV01_CH0_Pos)
#define PWM_CLKDIV01_CH0				PWM_CLKDIV01_CH0_Msk

#define PWM_CLKDIV23_CH3_Pos			(16)
#define PWM_CLKDIV23_CH3_Msk			(0xFFFF << PWM_CLKDIV23_CH3_Pos)
#define PWM_CLKDIV23_CH3				PWM_CLKDIV23_CH3_Msk
#define PWM_CLKDIV23_CH2_Pos			(0)
#define PWM_CLKDIV23_CH2_Msk			(0xFFFF << PWM_CLKDIV23_CH2_Pos)
#define PWM_CLKDIV23_CH2				PWM_CLKDIV23_CH2_Msk

#define PWM_CR_CNTEN_Pos				(27)
#define PWM_CR_CNTEN_Msk				(0x1F << PWM_CR_CNTEN_Pos)
#define PWM_CR_CNTEN					PWM_CR_CNTEN_Msk
#define PWM_CR_CAPINV_Pos				(25)
#define PWM_CR_CAPINV_Msk				(0x1 << PWM_CR_CAPINV_Pos)
#define PWM_CR_CAPINV					PWM_CR_CAPINV_Msk
#define PWM_CR_CPEN_Pos					(24)
#define PWM_CR_CPEN_Msk					(0x1 << PWM_CR_CPEN_Pos)
#define PWM_CR_CPEN						PWM_CR_CPEN_Msk
#define PWM_CR_CNTTYPE3_Pos				(22)
#define PWM_CR_CNTTYPE3_Msk				(0x3 << PWM_CR_CNTTYPE3_Pos)
#define PWM_CR_CNTTYPE3					PWM_CR_CNTTYPE3_Msk
#define PWM_CR_CNTTYPE2_Pos				(20)
#define PWM_CR_CNTTYPE2_Msk				(0x3 << PWM_CR_CNTTYPE2_Pos)
#define PWM_CR_CNTTYPE2					PWM_CR_CNTTYPE2_Msk
#define PWM_CR_CNTTYPE1_Pos				(18)
#define PWM_CR_CNTTYPE1_Msk				(0x3 << PWM_CR_CNTTYPE1_Pos)
#define PWM_CR_CNTTYPE1					PWM_CR_CNTTYPE1_Msk
#define PWM_CR_CNTTYPE0_Pos				(16)
#define PWM_CR_CNTTYPE0_Msk				(0x3 << PWM_CR_CNTTYPE0_Pos)
#define PWM_CR_CNTTYPE0					PWM_CR_CNTTYPE0_Msk
#define PWM_CR_TWOSYNCEN_Pos			(14)
#define PWM_CR_TWOSYNCEN_Msk			(0x3 << PWM_CR_TWOSYNCEN_Pos)
#define PWM_CR_TWOSYNCEN				PWM_CR_TWOSYNCEN_Msk
#define PWM_CR_POEN_Pos					(12)
#define PWM_CR_POEN_Msk					(0x1 << PWM_CR_POEN_Pos)
#define PWM_CR_POEN						PWM_CR_POEN_Msk
#define PWM_CR_CNTMODE_Pos				(8)
#define PWM_CR_CNTMODE_Msk				(0xF << PWM_CR_CNTMODE_Pos)
#define PWM_CR_CNTMODE					PWM_CR_CNTMODE_Msk
#define PWM_CR_ALLSYNCEN_Pos			(6)
#define PWM_CR_ALLSYNCEN_Msk			(0x1 << PWM_CR_ALLSYNCEN_Pos)
#define PWM_CR_ALLSYNCEN				PWM_CR_ALLSYNCEN_Msk
#define PWM_CR_PINV_Pos					(2)
#define PWM_CR_PINV_Msk					(0xF << PWM_CR_PINV_Pos)
#define PWM_CR_PINV						PWM_CR_PINV_Msk
#define PWM_CR_2COMPLEMENTARY_Pos		(0)
#define PWM_CR_2COMPLEMENTARY_Msk		(0x3 << PWM_CR_2COMPLEMENTARY_Pos)
#define PWM_CR_2COMPLEMENTARY			PWM_CR_2COMPLEMENTARY_Msk

#define PWM_PERIOD_CH3_Pos				(24)
#define PWM_PERIOD_CH3_Msk				(0xFF << PWM_PERIOD_CH3_Pos)
#define PWM_PERIOD_CH3					PWM_PERIOD_CH3_Msk
#define PWM_PERIOD_CH2_Pos				(16)
#define PWM_PERIOD_CH2_Msk				(0xFF << PWM_PERIOD_CH2_Pos)
#define PWM_PERIOD_CH2					PWM_PERIOD_CH2_Msk
#define PWM_PERIOD_CH1_Pos				(8)
#define PWM_PERIOD_CH1_Msk				(0xFF << PWM_PERIOD_CH1_Pos)
#define PWM_PERIOD_CH1					PWM_PERIOD_CH1_Msk
#define PWM_PERIOD_CH0_Pos				(0)
#define PWM_PERIOD_CH0_Msk				(0xFF << PWM_PERIOD_CH0_Pos)
#define PWM_PERIOD_CH0					PWM_PERIOD_CH0_Msk

#define PWM_PNUM_CH3_Pos				(24)
#define PWM_PNUM_CH3_Msk				(0xFF << PWM_PNUM_CH3_Pos)
#define PWM_PNUM_CH3					PWM_PNUM_CH3_Msk
#define PWM_PNUM_CH2_Pos				(16)
#define PWM_PNUM_CH2_Msk				(0xFF << PWM_PNUM_CH2_Pos)
#define PWM_PNUM_CH2					PWM_PNUM_CH2_Msk
#define PWM_PNUM_CH1_Pos				(8)
#define PWM_PNUM_CH1_Msk				(0xFF << PWM_PNUM_CH1_Pos)
#define PWM_PNUM_CH1					PWM_PNUM_CH1_Msk
#define PWM_PNUM_CH0_Pos				(0)
#define PWM_PNUM_CH0_Msk				(0xFF << PWM_PNUM_CH0_Pos)
#define PWM_PNUM_CH0					PWM_PNUM_CH0_Msk

#define PWM_CMPDAT_CH3_Pos				(24)
#define PWM_CMPDAT_CH3_Msk				(0xFF << PWM_CMPDAT_CH3_Pos)
#define PWM_CMPDAT_CH3					PWM_CMPDAT_CH3_Msk
#define PWM_CMPDAT_CH2_Pos				(16)
#define PWM_CMPDAT_CH2_Msk				(0xFF << PWM_CMPDAT_CH2_Pos)
#define PWM_CMPDAT_CH2					PWM_CMPDAT_CH2_Msk
#define PWM_CMPDAT_CH1_Pos				(8)
#define PWM_CMPDAT_CH1_Msk				(0xFF << PWM_CMPDAT_CH1_Pos)
#define PWM_CMPDAT_CH1					PWM_CMPDAT_CH1_Msk
#define PWM_CMPDAT_CH0_Pos				(0)
#define PWM_CMPDAT_CH0_Msk				(0xFF << PWM_CMPDAT_CH0_Pos)
#define PWM_CMPDAT_CH0					PWM_CMPDAT_CH0_Msk

#define PWM_DTCR_DTEN23_Pos				(21)
#define PWM_DTCR_DTEN23_Msk				(0x1 << PWM_DTCR_DTEN23_Pos)
#define PWM_DTCR_DTEN23					PWM_DTCR_DTEN23_Msk
#define PWM_DTCR_DTEN01_Pos				(20)
#define PWM_DTCR_DTEN01_Msk				(0x1 << PWM_DTCR_DTEN01_Pos)
#define PWM_DTCR_DTEN01					PWM_DTCR_DTEN01_Msk
#define PWM_DTCR_DTDIV_Pos				(16)
#define PWM_DTCR_DTDIV_Msk				(0x3 << PWM_DTCR_DTDIV_Pos)
#define PWM_DTCR_DTDIV					PWM_DTCR_DTDIV_Msk
#define PWM_DTCR_DTDIV_1				(0x00 << PWM_DTCR_DTDIV_Pos)
#define PWM_DTCR_DTDIV_2				(0x01 << PWM_DTCR_DTDIV_Pos)
#define PWM_DTCR_DTDIV_4				(0x02 << PWM_DTCR_DTDIV_Pos)
#define PWM_DTCR_DTDIV_8				(0x03 << PWM_DTCR_DTDIV_Pos)
#define PWM_DTCR_DTCNT23_Pos			(8)
#define PWM_DTCR_DTCNT23_Msk			(0xFF << PWM_DTCR_DTCNT23_Pos)
#define PWM_DTCR_DTCNT23				PWM_DTCR_DTCNT23_Msk
#define PWM_DTCR_DTCNT01_Pos			(0)
#define PWM_DTCR_DTCNT01_Msk			(0xFF << PWM_DTCR_DTCNT01_Pos)
#define PWM_DTCR_DTCNT01				PWM_DTCR_DTCNT01_Msk

#define PWM_IE_DMAEN_Pos				(7)
#define PWM_IE_DMAEN_Msk				(0x1 << PWM_IE_DMAEN_Pos)
#define PWM_IE_DMAEN					PWM_IE_DMAEN_Msk
#define PWM_IE_FLIEN_Pos				(6)
#define PWM_IE_FLIEN_Msk				(0x1 << PWM_IE_FLIEN_Pos)
#define PWM_IE_FLIEN					PWM_IE_FLIEN_Msk
#define PWM_IE_RLIEN_Pos				(5)
#define PWM_IE_RLIEN_Msk				(0x1 << PWM_IE_RLIEN_Pos)
#define PWM_IE_RLIEN					PWM_IE_RLIEN_Msk
#define PWM_IE_PIEN_Pos					(0)
#define PWM_IE_PIEN_Msk					(0x1F << PWM_IE_PIEN_Pos)
#define PWM_IE_PIEN						PWM_IE_PIEN_Msk

#define PWM_IF_OVERFL_Pos				(9)
#define PWM_IF_OVERFL_Msk				(0x1 << PWM_IF_OVERFL_Pos)
#define PWM_IF_OVERFL					PWM_IF_OVERFL_Msk
#define PWM_IF_FLIFOV_Pos				(8)
#define PWM_IF_FLIFOV_Msk				(0x1 << PWM_IF_FLIFOV_Pos)
#define PWM_IF_FLIFOV					PWM_IF_FLIFOV_Msk
#define PWM_IF_RLIFOV_Pos				(7)
#define PWM_IF_RLIFOV_Msk				(0x1 << PWM_IF_RLIFOV_Pos)
#define PWM_IF_RLIFOV					PWM_IF_RLIFOV_Msk
#define PWM_IF_CFLIF_Pos				(6)
#define PWM_IF_CFLIF_Msk				(0x1 << PWM_IF_CFLIF_Pos)
#define PWM_IF_CFLIF					PWM_IF_CFLIF_Msk
#define PWM_IF_CRLIF_Pos				(5)
#define PWM_IF_CRLIF_Msk				(0x1 << PWM_IF_CRLIF_Pos)
#define PWM_IF_CRLIF					PWM_IF_CRLIF_Msk
#define PWM_IF_PIF_Pos					(0)
#define PWM_IF_PIF_Msk					(0x1F << PWM_IF_PIF_Pos)
#define PWM_IF_PIF						PWM_IF_PIF_Msk

#define PWM_CH0CAPDAT_F_Pos				(16)
#define PWM_CH0CAPDAT_F_Msk				(0xFFFF << PWM_CH0CAPDAT_F_Pos)
#define PWM_CH0CAPDAT_F					PWM_CH0CAPDAT_F_Msk
#define PWM_CH0CAPDAT_R_Pos				(0)
#define PWM_CH0CAPDAT_R_Msk				(0xFFFF << PWM_CH0CAPDAT_R_Pos)
#define PWM_CH0CAPDAT_R					PWM_CH0CAPDAT_R_Msk

#define PWM_BKCR_EN_Pos					(11)
#define PWM_BKCR_EN_Msk					(0x1F << PWM_BKCR_EN_Pos)
#define PWM_BKCR_EN						PWM_BKCR_EN_Msk
#define PWM_BKCR_EN_CH0					(0x01 << PWM_BKCR_EN_Pos)
#define PWM_BKCR_EN_CH1					(0x02 << PWM_BKCR_EN_Pos)
#define PWM_BKCR_EN_CH2					(0x04 << PWM_BKCR_EN_Pos)
#define PWM_BKCR_EN_CH3					(0x08 << PWM_BKCR_EN_Pos)
#define PWM_BKCR_EN_CH4					(0x10 << PWM_BKCR_EN_Pos)
#define PWM_BKCR_OD_Pos					(3)
#define PWM_BKCR_OD_Msk					(0x1F << PWM_BKCR_OD_Pos)
#define PWM_BKCR_OD						PWM_BKCR_OD_Msk
#define PWM_BKCR_OD_CH0					(0x01 << PWM_BKCR_OD_Pos)
#define PWM_BKCR_OD_CH1					(0x02 << PWM_BKCR_OD_Pos)
#define PWM_BKCR_OD_CH2					(0x04 << PWM_BKCR_OD_Pos)
#define PWM_BKCR_OD_CH3					(0x08 << PWM_BKCR_OD_Pos)
#define PWM_BKCR_OD_CH4					(0x10 << PWM_BKCR_OD_Pos)

#define PWM_CH4CR1_DIV_Pos				(16)
#define PWM_CH4CR1_DIV_Msk				(0xFFFF << PWM_CH4CR1_DIV_Pos)
#define PWM_CH4CR1_DIV					PWM_CH4CR1_DIV_Msk
#define PWM_CH4CR1_PRD_Pos				(8)
#define PWM_CH4CR1_PRD_Msk				(0xFF << PWM_CH4CR1_PRD_Pos)
#define PWM_CH4CR1_PRD					PWM_CH4CR1_PRD_Msk
#define PWM_CH4CR1_PNUM_Pos				(0)
#define PWM_CH4CR1_PNUM_Msk				(0xFF << PWM_CH4CR1_PNUM_Pos)
#define PWM_CH4CR1_PNUM					PWM_CH4CR1_PNUM_Msk

#define PWM_CH4CR2_CMP_Pos				(8)
#define PWM_CH4CR2_CMP_Msk				(0xFF << PWM_CH4CR2_CMP_Pos)
#define PWM_CH4CR2_CMP					PWM_CH4CR2_CMP_Msk
#define PWM_CH4CR2_CNTTYPE_Pos			(3)
#define PWM_CH4CR2_CNTTYPE_Msk			(0x3 << PWM_CH4CR2_CNTTYPE_Pos)
#define PWM_CH4CR2_CNTTYPE				PWM_CH4CR2_CNTTYPE_Msk
#define PWM_CH4CR2_CNTMODE_Pos			(1)
#define PWM_CH4CR2_CNTMODE_Msk			(0x1 << PWM_CH4CR2_CNTMODE_Pos)
#define PWM_CH4CR2_CNTMODE				PWM_CH4CR2_CNTMODE_Msk
#define PWM_CH4CR2_PINV_Pos				(0)
#define PWM_CH4CR2_PINV_Msk				(0x1 << PWM_CH4CR2_PINV_Pos)
#define PWM_CH4CR2_PINV					PWM_CH4CR2_PINV_Msk

#define PWM_CH4CAPDAT_F_Pos				(16)
#define PWM_CH4CAPDAT_F_Msk				(0xFFFF << PWM_CH4CAPDAT_F_Pos)
#define PWM_CH4CAPDAT_F					PWM_CH4CAPDAT_F_Msk
#define PWM_CH4CAPDAT_R_Pos				(0)
#define PWM_CH4CAPDAT_R_Msk				(0xFFFF << PWM_CH4CAPDAT_R_Pos)
#define PWM_CH4CAPDAT_R					PWM_CH4CAPDAT_R_Msk

#define PWM_CH4CR3_DMAEN_Pos			(10)
#define PWM_CH4CR3_DMAEN_Msk			(0x1 << PWM_CH4CR3_DMAEN_Pos)
#define PWM_CH4CR3_DMAEN				PWM_CH4CR3_DMAEN_Msk
#define PWM_CH4CR3_FLIEN_Pos			(9)
#define PWM_CH4CR3_FLIEN_Msk			(0x1 << PWM_CH4CR3_FLIEN_Pos)
#define PWM_CH4CR3_FLIEN				PWM_CH4CR3_FLIEN_Msk
#define PWM_CH4CR3_RLIEN_Pos			(8)
#define PWM_CH4CR3_RLIEN_Msk			(0x1 << PWM_CH4CR3_RLIEN_Pos)
#define PWM_CH4CR3_RLIEN				PWM_CH4CR3_RLIEN_Msk
#define PWM_CH4CR3_OVERFL_Pos			(7)
#define PWM_CH4CR3_OVERFL_Msk			(0x1 << PWM_CH4CR3_OVERFL_Pos)
#define PWM_CH4CR3_OVERFL				PWM_CH4CR3_OVERFL_Msk
#define PWM_CH4CR3_FLIFOV_Pos			(6)
#define PWM_CH4CR3_FLIFOV_Msk			(0x1 << PWM_CH4CR3_FLIFOV_Pos)
#define PWM_CH4CR3_FLIFOV				PWM_CH4CR3_FLIFOV_Msk
#define PWM_CH4CR3_RLIFOV_Pos			(5)
#define PWM_CH4CR3_RLIFOV_Msk			(0x1 << PWM_CH4CR3_RLIFOV_Pos)
#define PWM_CH4CR3_RLIFOV				PWM_CH4CR3_RLIFOV_Msk
#define PWM_CH4CR3_CFLIF_Pos			(4)
#define PWM_CH4CR3_CFLIF_Msk			(0x1 << PWM_CH4CR3_CFLIF_Pos)
#define PWM_CH4CR3_CFLIF				PWM_CH4CR3_CFLIF_Msk
#define PWM_CH4CR3_CRLIF_Pos			(3)
#define PWM_CH4CR3_CRLIF_Msk			(0x1 << PWM_CH4CR3_CRLIF_Pos)
#define PWM_CH4CR3_CRLIF				PWM_CH4CR3_CRLIF_Msk
#define PWM_CH4CR3_POEN_Pos				(2)
#define PWM_CH4CR3_POEN_Msk				(0x1 << PWM_CH4CR3_POEN_Pos)
#define PWM_CH4CR3_POEN					PWM_CH4CR3_POEN_Msk
#define PWM_CH4CR3_CPEN_Pos				(1)
#define PWM_CH4CR3_CPEN_Msk				(0x1 << PWM_CH4CR3_CPEN_Pos)
#define PWM_CH4CR3_CPEN					PWM_CH4CR3_CPEN_Msk
#define PWM_CH4CR3_CAPINV_Pos			(0)
#define PWM_CH4CR3_CAPINV_Msk			(0x1 << PWM_CH4CR3_CAPINV_Pos)
#define PWM_CH4CR3_CAPINV				PWM_CH4CR3_CAPINV_Msk


/***************************************************************
 * Internal Flash Reg
 ***************************************************************/
 
#define FLASH_BASE				(DEVICE_BASE_ADDR + 0x2000)

typedef struct
{
	__IOM uint32_t CMD_INFO;
	__IOM uint32_t CMD_START;
	__IOM uint32_t FLASH_CR;
	__IOM uint32_t REMAP;
	__IOM uint32_t ADDR;
	__IOM uint32_t DECRYPT_CR;
	__IM uint32_t DECRYPT_STA;
} FLASH_TypeDef;

#define FLASH_CMD_INFO_A_Pos			(31)
#define FLASH_CMD_INFO_A_Msk			(0x1 << FLASH_CMD_INFO_A_Pos)
#define FLASH_CMD_INFO_A				FLASH_CMD_INFO_A_Msk
#define FLASH_CMD_INFO_CRM_Pos			(30)
#define FLASH_CMD_INFO_CRM_Msk			(0x1 << FLASH_CMD_INFO_CRM_Pos)
#define FLASH_CMD_INFO_CRM				FLASH_CMD_INFO_CRM_Msk
#define FLASH_CMD_INFO_DM_Pos			(29)
#define FLASH_CMD_INFO_DM_Msk			(0x1 << FLASH_CMD_INFO_DM_Pos)
#define FLASH_CMD_INFO_DM				FLASH_CMD_INFO_DM_Msk
#define FLASH_CMD_INFO_DM_LEN_Pos		(26)
#define FLASH_CMD_INFO_DM_LEN_Msk		(0x7 << FLASH_CMD_INFO_DM_LEN_Pos)
#define FLASH_CMD_INFO_DM_LEN			FLASH_CMD_INFO_DM_LEN_Msk
#define FLASH_CMD_INFO_DATA_LEN_Pos		(16)
#define FLASH_CMD_INFO_DATA_LEN_Msk		(0x2FF << FLASH_CMD_INFO_DATA_LEN_Pos)
#define FLASH_CMD_INFO_DATA_LEN			FLASH_CMD_INFO_DATA_LEN_Msk
#define FLASH_CMD_INFO_DATA_Pos			(15)
#define FLASH_CMD_INFO_DATA_Msk			(0x1 << FLASH_CMD_INFO_DATA_Pos)
#define FLASH_CMD_INFO_DATA				FLASH_CMD_INFO_DATA_Msk
#define FLASH_CMD_INFO_RD_Pos			(14)
#define FLASH_CMD_INFO_RD_Msk			(0x1 << FLASH_CMD_INFO_RD_Pos)
#define FLASH_CMD_INFO_RD				FLASH_CMD_INFO_RD_Msk
#define FLASH_CMD_INFO_WRSR_Pos			(13)
#define FLASH_CMD_INFO_WRSR_Msk			(0x1 << FLASH_CMD_INFO_WRSR_Pos)
#define FLASH_CMD_INFO_WRSR				FLASH_CMD_INFO_WRSR_Msk
#define FLASH_CMD_INFO_PP_Pos			(12)
#define FLASH_CMD_INFO_PP_Msk			(0x1 << FLASH_CMD_INFO_PP_Pos)
#define FLASH_CMD_INFO_PP				FLASH_CMD_INFO_PP_Msk
#define FLASH_CMD_INFO_SE_Pos			(11)
#define FLASH_CMD_INFO_SE_Msk			(0x1 << FLASH_CMD_INFO_SE_Pos)
#define FLASH_CMD_INFO_SE				FLASH_CMD_INFO_SE_Msk
#define FLASH_CMD_INFO_BE_Pos			(10)
#define FLASH_CMD_INFO_BE_Msk			(0x1 << FLASH_CMD_INFO_BE_Pos)
#define FLASH_CMD_INFO_BE				FLASH_CMD_INFO_BE_Msk
#define FLASH_CMD_INFO_HBE_Pos			(9)
#define FLASH_CMD_INFO_HBE_Msk			(0x1 << FLASH_CMD_INFO_HBE_Pos)
#define FLASH_CMD_INFO_HBE				FLASH_CMD_INFO_HBE_Msk
#define FLASH_CMD_INFO_CE_Pos			(8)
#define FLASH_CMD_INFO_CE_Msk			(0x1 << FLASH_CMD_INFO_CE_Pos)
#define FLASH_CMD_INFO_CE				FLASH_CMD_INFO_CE_Msk
#define FLASH_CMD_INFO_CMD_Pos			(0)
#define FLASH_CMD_INFO_CMD_Msk			(0xFF << FLASH_CMD_INFO_CMD_Pos)
#define FLASH_CMD_INFO_CMD				FLASH_CMD_INFO_CMD_Msk

#define FLASH_CMD_START_CMD_Pos			(8)
#define FLASH_CMD_START_CMD_Msk			(0x1 << FLASH_CMD_START_CMD_Pos)
#define FLASH_CMD_START_CMD				FLASH_CMD_START_CMD_Msk
#define FLASH_CMD_START_CRM_Pos			(0)
#define FLASH_CMD_START_CRM_Msk			(0xFF << FLASH_CMD_START_CRM_Pos)
#define FLASH_CMD_START_CRM				FLASH_CMD_START_CRM_Msk

#define FLASH_FLASH_CR_ADMOD_Pos		(4)
#define FLASH_FLASH_CR_ADMOD_Msk		(0x3 << FLASH_FLASH_CR_ADMOD_Pos)
#define FLASH_FLASH_CR_ADMOD			FLASH_FLASH_CR_ADMOD_Msk
#define FLASH_FLASH_CR_TSHSL_Pos		(2)
#define FLASH_FLASH_CR_TSHSL_Msk		(0x3 << FLASH_FLASH_CR_TSHSL_Pos)
#define FLASH_FLASH_CR_TSHSL			FLASH_FLASH_CR_TSHSL_Msk
#define FLASH_FLASH_CR_QPIM_Pos			(1)
#define FLASH_FLASH_CR_QPIM_Msk			(0x1 << FLASH_FLASH_CR_QPIM_Pos)
#define FLASH_FLASH_CR_QPIM				FLASH_FLASH_CR_QPIM_Msk
#define FLASH_FLASH_CR_QIOM_Pos			(0)
#define FLASH_FLASH_CR_QIOM_Msk			(0x1 << FLASH_FLASH_CR_QIOM_Pos)
#define FLASH_FLASH_CR_QIOM				FLASH_FLASH_CR_QIOM_Msk

#define FLASH_REMAP_Pos					(0)
#define FLASH_REMAP_Msk					(0x1 << FLASH_REMAP_Pos)
#define FLASH_REMAP						FLASH_REMAP_Msk

#define FLASH_ADDR_Pos					(0)
#define FLASH_ADDR_Msk					(0xFFFFFFFF << FLASH_ADDR_Pos)
#define FLASH_ADDR						FLASH_ADDR_Msk

#define FLASH_DECRYPT_CR_START_Pos		(7)
#define FLASH_DECRYPT_CR_START_Msk		(0x1 << FLASH_DECRYPT_CR_START_Pos)
#define FLASH_DECRYPT_CR_START			FLASH_DECRYPT_CR_START_Msk
#define FLASH_DECRYPT_CR_EXPTSEL_Pos	(4)
#define FLASH_DECRYPT_CR_EXPTSEL_Msk	(0x7 << FLASH_DECRYPT_CR_EXPTSEL_Pos)
#define FLASH_DECRYPT_CR_EXPTSEL		FLASH_DECRYPT_CR_EXPTSEL_Msk
#define FLASH_DECRYPT_CR_DATADECRYPT_Pos		(3)
#define FLASH_DECRYPT_CR_DATADECRYPT_Msk		(0x1 << FLASH_DECRYPT_CR_DATADECRYPT_Pos)
#define FLASH_DECRYPT_CR_DATADECRYPT			FLASH_DECRYPT_CR_DATADECRYPT_Msk
#define FLASH_DECRYPT_CR_DBUSDECRYPT_Pos		(2)
#define FLASH_DECRYPT_CR_DBUSDECRYPT_Msk		(0x1 << FLASH_DECRYPT_CR_DBUSDECRYPT_Pos)
#define FLASH_DECRYPT_CR_DBUSDECRYPT			FLASH_DECRYPT_CR_DBUSDECRYPT_Msk
#define FLASH_DECRYPT_CR_CODEDECRYPT_Pos		(1)
#define FLASH_DECRYPT_CR_CODEDECRYPT_Msk		(0x1 << FLASH_DECRYPT_CR_CODEDECRYPT_Pos)
#define FLASH_DECRYPT_CR_CODEDECRYPT			FLASH_DECRYPT_CR_CODEDECRYPT_Msk

#define FLASH_DECRYPT_STA_KEYERR_Pos		(2)
#define FLASH_DECRYPT_STA_KEYERR_Msk		(0x1 << FLASH_DECRYPT_STA_KEYERR_Pos)
#define FLASH_DECRYPT_STA_KEYERR			FLASH_DECRYPT_STA_KEYERR_Msk
#define FLASH_DECRYPT_STA_KEYRDY_Pos		(1)
#define FLASH_DECRYPT_STA_KEYRDY_Msk		(0x1 << FLASH_DECRYPT_STA_KEYRDY_Pos)
#define FLASH_DECRYPT_STA_KEYRDY			FLASH_DECRYPT_STA_KEYRDY_Msk
#define FLASH_DECRYPT_STA_RSA_Pos			(0)
#define FLASH_DECRYPT_STA_RSA_Msk			(0x1 << FLASH_DECRYPT_STA_RSA_Pos)
#define FLASH_DECRYPT_STA_RSA				FLASH_DECRYPT_STA_RSA_Msk


/***************************************************************
 * RSA Reg
 ***************************************************************/
 
#define RSA_BASE       0x40000000

typedef struct
{
	__IOM uint32_t XBUF[256];
	__IOM uint32_t YBUF[256];
	__IOM uint32_t MBUF[256];
	__IOM uint32_t DBUF[256];
	__IOM uint32_t RSACON;
	__IOM uint32_t RSAMC;
	__IOM uint32_t RSAN;
} RSA_TypeDef;
 

/***************************************************************
 * ADC Reg
 ***************************************************************/
 
#define ADC_BASE	(HR_APB_BASE_ADDR + 0x0200)

typedef struct
{
	__IM uint32_t DR;
	__IOM uint32_t ANA_CR;
	__IOM uint32_t PGA_CR;
	__IOM uint32_t TEMP_CR;
	__IOM uint32_t ADC_CR;
	__IOM uint32_t IF;
	__IOM uint32_t CMP_VAL;
} ADC_TypeDef;

#define ADC_ANA_CR_CH_Pos				(8)
#define ADC_ANA_CR_CH_Msk				(0xFUL << ADC_ANA_CR_CH_Pos)
#define ADC_ANA_CR_CH					ADC_ANA_CR_CH_Msk
#define ADC_ANA_CR_CH_0					(0x0UL << ADC_ANA_CR_CH_Pos)
#define ADC_ANA_CR_CH_1					(0x1UL << ADC_ANA_CR_CH_Pos)
#define ADC_ANA_CR_CH_2					(0x2UL << ADC_ANA_CR_CH_Pos)
#define ADC_ANA_CR_CH_3					(0x3UL << ADC_ANA_CR_CH_Pos)
#define ADC_ANA_CR_CH_8					(0x8UL << ADC_ANA_CR_CH_Pos)
#define ADC_ANA_CR_CH_9					(0x9UL << ADC_ANA_CR_CH_Pos)
#define ADC_ANA_CR_CH_TEMP				(0xCUL << ADC_ANA_CR_CH_Pos)
#define ADC_ANA_CR_CH_VOL				(0xDUL << ADC_ANA_CR_CH_Pos)
#define ADC_ANA_CR_CH_OFFSET			(0xEUL << ADC_ANA_CR_CH_Pos)
#define ADC_ANA_CR_CHOPENR_Pos			(5)
#define ADC_ANA_CR_CHOPENR_Msk			(0x3UL << ADC_ANA_CR_CHOPENR_Pos)
#define ADC_ANA_CR_CHOPENR				ADC_ANA_CR_CHOPENR_Msk
#define ADC_ANA_CR_CHOPENS_Pos			(4)
#define ADC_ANA_CR_CHOPENS_Msk			(0x1UL << ADC_ANA_CR_CHOPENS_Pos)
#define ADC_ANA_CR_CHOPENS				ADC_ANA_CR_CHOPENS_Msk
#define ADC_ANA_CR_PD_Pos				(2)
#define ADC_ANA_CR_PD_Msk				(0x1UL << ADC_ANA_CR_PD_Pos)
#define ADC_ANA_CR_PD					ADC_ANA_CR_PD_Msk
#define ADC_ANA_CR_RST_Pos				(1)
#define ADC_ANA_CR_RST_Msk				(0x1UL << ADC_ANA_CR_RST_Pos)
#define ADC_ANA_CR_RST					ADC_ANA_CR_RST_Msk
#define ADC_ANA_CR_LDOEN_Pos			(0)
#define ADC_ANA_CR_LDOEN_Msk			(0x1UL << ADC_ANA_CR_LDOEN_Pos)
#define ADC_ANA_CR_LDOEN				ADC_ANA_CR_LDOEN_Msk

#define ADC_PGA_CR_GAIN_Pos				(4)
#define ADC_PGA_CR_GAIN_Msk				(0x1FUL << ADC_PGA_CR_GAIN_Pos)
#define ADC_PGA_CR_GAIN					ADC_PGA_CR_GAIN_Msk
#define ADC_PGA_CR_BP_Pos				(3)
#define ADC_PGA_CR_BP_Msk				(0x1UL << ADC_PGA_CR_BP_Pos)
#define ADC_PGA_CR_BP					ADC_PGA_CR_BP_Msk
#define ADC_PGA_CR_BPREF_Pos			(2)
#define ADC_PGA_CR_BPREF_Msk			(0x1UL << ADC_PGA_CR_BPREF_Pos)
#define ADC_PGA_CR_BPREF				ADC_PGA_CR_BPREF_Msk
#define ADC_PGA_CR_CHOPEN_Pos			(1)
#define ADC_PGA_CR_CHOPEN_Msk			(0x1UL << ADC_PGA_CR_CHOPEN_Pos)
#define ADC_PGA_CR_CHOPEN				ADC_PGA_CR_CHOPEN_Msk
#define ADC_PGA_CR_PGAEN_Pos			(0)
#define ADC_PGA_CR_PGAEN_Msk			(0x1UL << ADC_PGA_CR_PGAEN_Pos)
#define ADC_PGA_CR_PGAEN				ADC_PGA_CR_PGAEN_Msk

#define ADC_TEMP_CR_GAIN_Pos			(4)
#define ADC_TEMP_CR_GAIN_Msk			(0x3UL << ADC_TEMP_CR_GAIN_Pos)
#define ADC_TEMP_CR_GAIN				ADC_TEMP_CR_GAIN_Msk
#define ADC_TEMP_CR_OFFSET_Pos			(1)
#define ADC_TEMP_CR_OFFSET_Msk			(0x1UL << ADC_TEMP_CR_OFFSET_Pos)
#define ADC_TEMP_CR_OFFSET				ADC_TEMP_CR_OFFSET_Msk
#define ADC_TEMP_CR_EN_Pos				(0)
#define ADC_TEMP_CR_EN_Msk				(0x1UL << ADC_TEMP_CR_EN_Pos)
#define ADC_TEMP_CR_EN					ADC_TEMP_CR_EN_Msk

#define ADC_ADC_CR_SWITCHTIME_Pos		(20)
#define ADC_ADC_CR_SWITCHTIME_Msk		(0x3FFUL << ADC_ADC_CR_SWITCHTIME_Pos)
#define ADC_ADC_CR_SWITCHTIME			ADC_ADC_CR_SWITCHTIME_Msk
#define ADC_ADC_CR_INITTIME_Pos			(8)
#define ADC_ADC_CR_INITTIME_Msk			(0x3FFUL << ADC_ADC_CR_INITTIME_Pos)
#define ADC_ADC_CR_INITTIME				ADC_ADC_CR_INITTIME_Msk
#define ADC_ADC_CR_CMPPOL_Pos			(6)
#define ADC_ADC_CR_CMPPOL_Msk			(0x1UL << ADC_ADC_CR_CMPPOL_Pos)
#define ADC_ADC_CR_CMPPOL				ADC_ADC_CR_CMPPOL_Msk
#define ADC_ADC_CR_CMPIE_Pos			(5)
#define ADC_ADC_CR_CMPIE_Msk			(0x1UL << ADC_ADC_CR_CMPIE_Pos)
#define ADC_ADC_CR_CMPIE				ADC_ADC_CR_CMPIE_Msk
#define ADC_ADC_CR_CMPEN_Pos			(4)
#define ADC_ADC_CR_CMPEN_Msk			(0x1UL << ADC_ADC_CR_CMPEN_Pos)
#define ADC_ADC_CR_CMPEN				ADC_ADC_CR_CMPEN_Msk
#define ADC_ADC_CR_ADCIE_Pos			(1)
#define ADC_ADC_CR_ADCIE_Msk			(0x1UL << ADC_ADC_CR_ADCIE_Pos)
#define ADC_ADC_CR_ADCIE				ADC_ADC_CR_ADCIE_Msk
#define ADC_ADC_CR_DMAEN_Pos			(0)
#define ADC_ADC_CR_DMAEN_Msk			(0x1UL << ADC_ADC_CR_DMAEN_Pos)
#define ADC_ADC_CR_DMAEN				ADC_ADC_CR_DMAEN_Msk

#define ADC_IF_CMP_Pos					(1)
#define ADC_IF_CMP_Msk					(0x1UL << ADC_IF_CMP_Pos)
#define ADC_IF_CMP						ADC_IF_CMP_Msk
#define ADC_IF_ADC_Pos					(0)
#define ADC_IF_ADC_Msk					(0x1UL << ADC_IF_ADC_Pos)
#define ADC_IF_ADC						ADC_IF_ADC_Msk


/***************************************************************
 * GPIO Reg
 ***************************************************************/
#define GPIOA_BASE            (HR_APB_BASE_ADDR + 0x1200)
#define GPIOB_BASE            (HR_APB_BASE_ADDR + 0x1400)

typedef struct
{
	__IOM uint32_t DATA;
	__IOM uint32_t DATA_B_EN;
	__IOM uint32_t DIR;
	__IOM uint32_t PULLUP_EN;
	__IOM uint32_t AF_SEL;
	__IOM uint32_t AF_S1;
	__IOM uint32_t AF_S0;
	__IOM uint32_t PULLDOWN_EN;
	__IOM uint32_t IS;
	__IOM uint32_t IBE;
	__IOM uint32_t IEV;
	__IOM uint32_t IE;
	__IM uint32_t RIS;
	__IM uint32_t MIS;
	__OM uint32_t IC;
} GPIO_TypeDef;


/***************************************************************
 * I2S Reg
 ***************************************************************/

#define I2S_BASE            (HR_APB_BASE_ADDR + 0x2000)

typedef struct
{
	__IOM uint32_t CR;
	__IOM uint32_t IM;
	__IOM uint32_t IF;
	__IM uint32_t SR;
	__OM uint32_t TXDR;
	__IM uint32_t RXDR;
}I2S_TypeDef;

#define I2S_CR_MODE_Pos				(28)
#define I2S_CR_MODE_Msk				(0x1UL << I2S_CR_MODE_Pos)
#define I2S_CR_MODE					I2S_CR_MODE_Msk
#define I2S_CR_DUPLEX_Pos			(27)
#define I2S_CR_DUPLEX_Msk			(0x1UL << I2S_CR_DUPLEX_Pos)
#define I2S_CR_DUPLEX				I2S_CR_DUPLEX_Msk
#define I2S_CR_RXTIMEOUT_Pos		(26)
#define I2S_CR_RXTIMEOUT_Msk		(0x1UL << I2S_CR_RXTIMEOUT_Pos)
#define I2S_CR_RXTIMEOUT			I2S_CR_RXTIMEOUT_Msk
#define I2S_CR_FORMAT_Pos			(24)
#define I2S_CR_FORMAT_Msk			(0x3UL << I2S_CR_FORMAT_Pos)
#define I2S_CR_FORMAT				I2S_CR_FORMAT_Msk
#define I2S_CR_FORMAT_I2S			(0x0UL << I2S_CR_FORMAT_Pos)
#define I2S_CR_FORMAT_MSBJUSTIFIED	(0x1UL << I2S_CR_FORMAT_Pos)
#define I2S_CR_FORMAT_PCMA			(0x2UL << I2S_CR_FORMAT_Pos)
#define I2S_CR_FORMAT_PCMB			(0x3UL << I2S_CR_FORMAT_Pos)
#define I2S_CR_RXLRCH_Pos			(23)
#define I2S_CR_RXLRCH_Msk			(0x1UL << I2S_CR_RXLRCH_Pos)
#define I2S_CR_RXLRCH				I2S_CR_RXLRCH_Msk
#define I2S_CR_MONO_STEREO_Pos		(22)
#define I2S_CR_MONO_STEREO_Msk		(0x1UL << I2S_CR_MONO_STEREO_Pos)
#define I2S_CR_MONO_STEREO			I2S_CR_MONO_STEREO_Msk
#define I2S_CR_RXDMA_EN_Pos			(21)
#define I2S_CR_RXDMA_EN_Msk			(0x1UL << I2S_CR_RXDMA_EN_Pos)
#define I2S_CR_RXDMA_EN				I2S_CR_RXDMA_EN_Msk
#define I2S_CR_TXDMA_EN_Pos			(20)
#define I2S_CR_TXDMA_EN_Msk			(0x1UL << I2S_CR_TXDMA_EN_Pos)
#define I2S_CR_TXDMA_EN				I2S_CR_TXDMA_EN_Msk
#define I2S_CR_RXFIFO_CLR_Pos		(19)
#define I2S_CR_RXFIFO_CLR_Msk		(0x1UL << I2S_CR_RXFIFO_CLR_Pos)
#define I2S_CR_RXFIFO_CLR			I2S_CR_RXFIFO_CLR_Msk
#define I2S_CR_TXFIFO_CLR_Pos		(18)
#define I2S_CR_TXFIFO_CLR_Msk		(0x1UL << I2S_CR_TXFIFO_CLR_Pos)
#define I2S_CR_TXFIFO_CLR			I2S_CR_TXFIFO_CLR_Msk
#define I2S_CR_LZCEN_Pos			(17)
#define I2S_CR_LZCEN_Msk			(0x1UL << I2S_CR_LZCEN_Pos)
#define I2S_CR_LZCEN				I2S_CR_LZCEN_Msk
#define I2S_CR_RZCEN_Pos			(16)
#define I2S_CR_RZCEN_Msk			(0x1UL << I2S_CR_RZCEN_Pos)
#define I2S_CR_RZCEN				I2S_CR_RZCEN_Msk
#define I2S_CR_RXCLK_INVERSE_Pos	(15)
#define I2S_CR_RXCLK_INVERSE_Msk	(0x1UL << I2S_CR_RXCLK_INVERSE_Pos)
#define I2S_CR_RXCLK_INVERSE		(I2S_CR_RXCLK_INVERSE_Msk)
#define I2S_CR_RXFIFO_TH_Pos		(12)
#define I2S_CR_RXFIFO_TH_Msk		(0x7UL << I2S_CR_RXFIFO_TH_Pos)
#define I2S_CR_RXFIFO_TH			I2S_CR_RXFIFO_TH_Msk
#define I2S_CR_RXFIFO_TH_0			(0x0UL << I2S_CR_RXFIFO_TH_Pos)
#define I2S_CR_RXFIFO_TH_1			(0x1UL << I2S_CR_RXFIFO_TH_Pos)
#define I2S_CR_RXFIFO_TH_2			(0x2UL << I2S_CR_RXFIFO_TH_Pos)
#define I2S_CR_RXFIFO_TH_3			(0x3UL << I2S_CR_RXFIFO_TH_Pos)
#define I2S_CR_RXFIFO_TH_4			(0x4UL << I2S_CR_RXFIFO_TH_Pos)
#define I2S_CR_RXFIFO_TH_5			(0x5UL << I2S_CR_RXFIFO_TH_Pos)
#define I2S_CR_RXFIFO_TH_6			(0x6UL << I2S_CR_RXFIFO_TH_Pos)
#define I2S_CR_RXFIFO_TH_7			(0x7UL << I2S_CR_RXFIFO_TH_Pos)
#define I2S_CR_TXFIFO_TH_Pos		(9)
#define I2S_CR_TXFIFO_TH_Msk		(0x7UL << I2S_CR_TXFIFO_TH_Pos)
#define I2S_CR_TXFIFO_TH			I2S_CR_TXFIFO_TH_Msk
#define I2S_CR_TXFIFO_TH_0			(0x0UL << I2S_CR_TXFIFO_TH_Pos)
#define I2S_CR_TXFIFO_TH_1			(0x1UL << I2S_CR_TXFIFO_TH_Pos)
#define I2S_CR_TXFIFO_TH_2			(0x2UL << I2S_CR_TXFIFO_TH_Pos)
#define I2S_CR_TXFIFO_TH_3			(0x3UL << I2S_CR_TXFIFO_TH_Pos)
#define I2S_CR_TXFIFO_TH_4			(0x4UL << I2S_CR_TXFIFO_TH_Pos)
#define I2S_CR_TXFIFO_TH_5			(0x5UL << I2S_CR_TXFIFO_TH_Pos)
#define I2S_CR_TXFIFO_TH_6			(0x6UL << I2S_CR_TXFIFO_TH_Pos)
#define I2S_CR_TXFIFO_TH_7			(0x7UL << I2S_CR_TXFIFO_TH_Pos)
#define I2S_CR_TXCLK_INVERSE_Pos	(8)
#define I2S_CR_TXCLK_INVERSE_Msk	(0x1UL << I2S_CR_TXCLK_INVERSE_Pos)
#define I2S_CR_TXCLK_INVERSE		I2S_CR_TXCLK_INVERSE_Msk
#define I2S_CR_DATALEN_Pos			(4)
#define I2S_CR_DATALEN_Msk			(0x3UL << I2S_CR_DATALEN_Pos)
#define I2S_CR_DATALEN				I2S_CR_DATALEN_Msk
#define I2S_CR_DATALEN_8BIT			(0x0UL << I2S_CR_DATALEN_Pos)
#define I2S_CR_DATALEN_16BIT		(0x1UL << I2S_CR_DATALEN_Pos)
#define I2S_CR_DATALEN_24BIT		(0x2UL << I2S_CR_DATALEN_Pos)
#define I2S_CR_DATALEN_32BIT		(0x3UL << I2S_CR_DATALEN_Pos)
#define I2S_CR_MUTE_Pos				(3)
#define I2S_CR_MUTE_Msk				(0x1UL << I2S_CR_MUTE_Pos)
#define I2S_CR_MUTE					I2S_CR_MUTE_Msk
#define I2S_CR_RXEN_Pos				(2)
#define I2S_CR_RXEN_Msk				(0x1UL << I2S_CR_RXEN_Pos)
#define I2S_CR_RXEN					(I2S_CR_RXEN_Msk)
#define I2S_CR_TXEN_Pos				(1)
#define I2S_CR_TXEN_Msk				(0x1UL << I2S_CR_TXEN_Pos)
#define I2S_CR_TXEN					I2S_CR_TXEN_Msk
#define I2S_CR_EN_Pos				(0)
#define I2S_CR_EN_Msk				(0x1UL << I2S_CR_EN_Pos)
#define I2S_CR_EN					I2S_CR_EN_Msk

#define I2S_IM_LZCI_Pos				(9)
#define I2S_IM_LZCI_Msk				(0x1UL << I2S_IM_LZCI_Pos)
#define I2S_IM_LZCI					I2S_IM_LZCI_Msk
#define I2S_IM_RZCI_Pos				(8)
#define I2S_IM_RZCI_Msk				(0x1UL << I2S_IM_RZCI_Pos)
#define I2S_IM_RZCI					I2S_IM_RZCI_Msk
#define I2S_IM_TXDONE_Pos			(7)
#define I2S_IM_TXDONE_Msk			(0x1UL << I2S_IM_TXDONE_Pos)
#define I2S_IM_TXDONE				I2S_IM_TXDONE_Msk
#define I2S_IM_TXTH_Pos				(6)
#define I2S_IM_TXTH_Msk				(0x1UL << I2S_IM_TXTH_Pos)
#define I2S_IM_TXTH					I2S_IM_TXTH_Msk
#define I2S_IM_TX_OVERFLOW_Pos		(5)
#define I2S_IM_TX_OVERFLOW_Msk		(0x1UL << I2S_IM_TX_OVERFLOW_Pos)
#define I2S_IM_TX_OVERFLOW			I2S_IM_TX_OVERFLOW_Msk
#define I2S_IM_TX_UNDERFLOW_Pos		(4)
#define I2S_IM_TX_UNDERFLOW_Msk		(0x1UL << I2S_IM_TX_UNDERFLOW_Pos)
#define I2S_IM_TX_UNDERFLOW			I2S_IM_TX_UNDERFLOW_Msk
#define I2S_IM_RXDONE_Pos			(3)
#define I2S_IM_RXDONE_Msk			(0x1UL << I2S_IM_RXDONE_Pos)
#define I2S_IM_RXDONE				I2S_IM_RXDONE_Msk
#define I2S_IM_RXTH_Pos				(2)
#define I2S_IM_RXTH_Msk				(0x1UL << I2S_IM_RXTH_Pos)
#define I2S_IM_RXTH					I2S_IM_RXTH_Msk
#define I2S_IM_RX_OVERFLOW_Pos		(1)
#define I2S_IM_RX_OVERFLOW_Msk		(0x1UL << I2S_IM_RX_OVERFLOW_Pos)
#define I2S_IM_RX_OVERFLOW			I2S_IM_RX_OVERFLOW_Msk
#define I2S_IM_RX_UNDERFLOW_Pos		(0)
#define I2S_IM_RX_UNDERFLOW_Msk		(0x1UL << I2S_IM_RX_UNDERFLOW_Pos)
#define I2S_IM_RX_UNDERFLOW			I2S_IM_RX_UNDERFLOW_Msk

#define I2S_IF_TX_Pos				(12)
#define I2S_IF_TX_Msk				(0x1UL << I2S_IF_TX_Pos)
#define I2S_IF_TX					I2S_IF_TX_Msk
#define I2S_IF_RX_Pos				(11)
#define I2S_IF_RX_Msk				(0x1UL << I2S_IF_RX_Pos)
#define I2S_IF_RX					I2S_IF_RX_Msk
#define I2S_IF_I2S_Pos				(10)
#define I2S_IF_I2S_Msk				(0x1UL << I2S_IF_I2S_Pos)
#define I2S_IF_I2S					I2S_IF_I2S_Msk
#define I2S_IF_LZCI_Pos				(9)
#define I2S_IF_LZCI_Msk				(0x1UL << I2S_IF_LZCI_Pos)
#define I2S_IF_LZCI					I2S_IF_LZCI_Msk
#define I2S_IF_RZCI_Pos				(8)
#define I2S_IF_RZCI_Msk				(0x1UL << I2S_IF_RZCI_Pos)
#define I2S_IF_RZCI					I2S_IF_RZCI_Msk
#define I2S_IF_TXDONE_Pos			(7)
#define I2S_IF_TXDONE_Msk			(0x1UL << I2S_IF_TXDONE_Pos)
#define I2S_IF_TXDONE				I2S_IF_TXDONE_Msk
#define I2S_IF_TXTH_Pos				(6)
#define I2S_IF_TXTH_Msk				(0x1UL << I2S_IF_TXTH_Pos)
#define I2S_IF_TXTH					I2S_IF_TXTH_Msk
#define I2S_IF_TX_OVERFLOW_Pos		(5)
#define I2S_IF_TX_OVERFLOW_Msk		(0x1UL << I2S_IF_TX_OVERFLOW_Pos)
#define I2S_IF_TX_OVERFLOW			I2S_IF_TX_OVERFLOW_Msk
#define I2S_IF_TX_UNDERFLOW_Pos		(4)
#define I2S_IF_TX_UNDERFLOW_Msk		(0x1UL << I2S_IF_TX_UNDERFLOW_Pos)
#define I2S_IF_TX_UNDERFLOW			I2S_IF_TX_UNDERFLOW_Msk
#define I2S_IF_RXDONE_Pos			(3)
#define I2S_IF_RXDONE_Msk			(0x1UL << I2S_IF_RXDONE_Pos)
#define I2S_IF_RXDONE				I2S_IF_RXDONE_Msk
#define I2S_IF_RXTH_Pos				(2)
#define I2S_IF_RXTH_Msk				(0x1UL << I2S_IF_RXTH_Pos)
#define I2S_IF_RXTH					I2S_IF_RXTH_Msk
#define I2S_IF_RX_OVERFLOW_Pos		(1)
#define I2S_IF_RX_OVERFLOW_Msk		(0x1UL << I2S_IF_RX_OVERFLOW_Pos)
#define I2S_IF_RX_OVERFLOW			I2S_IF_RX_OVERFLOW_Msk
#define I2S_IF_RX_UNDERFLOW_Pos		(0)
#define I2S_IF_RX_UNDERFLOW_Msk		(0x1UL << I2S_IF_RX_UNDERFLOW_Pos)
#define I2S_IF_RX_UNDERFLOW			I2S_IF_RX_UNDERFLOW_Msk

#define I2S_SR_VALIDBYTE_Pos		(8)
#define I2S_SR_VALIDBYTE_Msk		(0x3UL << I2S_SR_VALIDBYTE_Pos)
#define I2S_SR_VALIDBYTE			I2S_SR_VALIDBYTE_Msk
#define I2S_SR_TXCNT_Pos			(4)
#define I2S_SR_TXCNT_Msk			(0xFUL << I2S_SR_TXCNT_Pos)
#define I2S_SR_TXCNT				I2S_SR_TXCNT_Msk
#define I2S_SR_RXCNT_Pos			(0)
#define I2S_SR_RXCNT_Msk			(0xFUL << I2S_SR_RXCNT_Pos)
#define I2S_SR_RXCNT				I2S_SR_RXCNT_Msk


/***************************************************************
 * TOUCH Reg
 ***************************************************************/
#define TOUCH_BASE            (HR_APB_BASE_ADDR + 0x2400)

typedef struct
{
	__IOM uint32_t CR;
	__IOM uint32_t CH0CR;
	__IOM uint32_t CH1CR;
	__IOM uint32_t CH2CR;
	__IOM uint32_t CH3CR;
	__IOM uint32_t CH4CR;
	__IOM uint32_t CH5CR;
	__IOM uint32_t CH6CR;
	__IOM uint32_t CH7CR;
	__IOM uint32_t CH8CR;
	__IOM uint32_t CH9CR;
	__IOM uint32_t CH10CR;
	__IOM uint32_t CH11CR;
	__IOM uint32_t CH12CR;
	__IOM uint32_t CH13CR;
	__IOM uint32_t CH14CR;
	__IOM uint32_t CH15CR;
	__IOM uint32_t IE_IF;
} TOUCH_TypeDef;

#define TOUCH_CR_SCAN_PERIOD_Pos	(26)
#define TOUCH_CR_SCAN_PERIOD_Msk	(0x03F << TOUCH_CR_SCAN_PERIOD_Pos)
#define TOUCH_CR_SCAN_PERIOD		TOUCH_CR_SCAN_PERIOD_Msk
#define TOUCH_CR_CAPDET_CNT_Pos		(20)
#define TOUCH_CR_CAPDET_CNT_Msk		(0x03F << TOUCH_CR_CAPDET_CNT_Pos)
#define TOUCH_CR_CAPDET_CNT			TOUCH_CR_CAPDET_CNT_Msk
#define TOUCH_CR_CH_SEL_Pos			(4)
#define TOUCH_CR_CH_SEL_Msk			(0x0FFFF << TOUCH_CR_CH_SEL_Pos)
#define TOUCH_CR_CH_SEL				TOUCH_CR_CH_SEL_Msk
#define TOUCH_CR_CH_SEL_NONE		(0 << TOUCH_CR_CH_SEL_Pos)
#define TOUCH_CR_CH_SEL_ALL			(0x0FFFF << TOUCH_CR_CH_SEL_Pos)
#define TOUCH_CR_EN_Pos				(0)
#define TOUCH_CR_EN_Msk				(0x01 << TOUCH_CR_EN_Pos)
#define TOUCH_CR_EN					TOUCH_CR_EN_Msk

#define TOUCH_CH0CR_COUNT_Pos		(8)
#define TOUCH_CH0CR_COUNT_Msk		(0x07FFF << TOUCH_CH0CR_COUNT_Pos)
#define TOUCH_CH0CR_COUNT			TOUCH_CH0CR_COUNT_Msk
#define TOUCH_CH0CR_THRESHOLD_Pos	(0)
#define TOUCH_CH0CR_THRESHOLD_Msk	(0x07F << TOUCH_CH0CR_THRESHOLD_Pos)
#define TOUCH_CH0CR_THRESHOLD		TOUCH_CH0CR_THRESHOLD_Msk

#define TOUCH_IE_IF_EN_Pos			(16)
#define TOUCH_IE_IF_EN_Msk			(0x0FFFF << TOUCH_IE_IF_EN_Pos)
#define TOUCH_IE_IF_EN				TOUCH_IE_IF_EN_Msk
#define TOUCH_IE_IF_FLAG_Pos		(0)
#define TOUCH_IE_IF_FLAG_Msk		(0x0FFFF << TOUCH_IE_IF_FLAG_Pos)
#define TOUCH_IE_IF_FLAG			TOUCH_IE_IF_FLAG_Msk


#define SET_BIT(REG, BIT)		((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)		((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)		((REG) & (BIT))
#define CLEAR_REG(REG)			((REG) = (0x0))
#define WRITE_REG(REG, VAL)   	((REG) = (VAL))
#define READ_REG(REG)         	((REG)) 
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))


#endif /* WM_REGS_H */

