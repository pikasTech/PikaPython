/*!
 * @file        apm32f0xx_rcm.h
 *
 * @brief       This file contains all the functions prototypes for the RCM firmware library
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#ifndef __RCM_H
#define __RCM_H

#include "apm32f0xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup RCM_Driver RCM Driver
  @{
*/

/** @addtogroup RCM_Enumerations Enumerations
  @{
*/

/**
 * @brief   HSE enum
 */
typedef enum
{
    RCM_HSE_CLOSE =0x00, 
    RCM_HSE_OPEN  =0x01, 
    RCM_HSE_BYPASS=0x05,
} RCM_HSE_T;

/**
 * @brief   LSE enum
 */
typedef enum
{
    RCM_LSE_CLOSE =0x00,
    RCM_LSE_OPEN  =0x01,
    RCM_LSE_BYPASS=0x05,
} RCM_LSE_T;

/**
 * @brief   LSE_Drive enum
 */
typedef enum
{
    RCM_LSE_DRIVE_Low =0x00,
    RCM_LSE_DRIVE_MediumLow,
    RCM_LSE_DRIVE_MediumHigh,
    RCM_LSE_DRIVE_High,
} RCM_LSE_DRIVE_T;

/**
 * @brief   System clock select
 */
typedef enum
{
    RCM_PLL_SEL_HSI_DIV2,
    RCM_PLL_SEL_HSI,     //!< Only for APM32F072 and APM32F091 devices
    RCM_PLL_SEL_HSE,
    RCM_PLL_SEL_HSI48    //!< Only for APM32F072 and APM32F091 devices
} RCM_PLL_SEL_T;

/**
 * @brief   PLL multiplication factor
 */
typedef enum
{
    RCM_PLLMF_2,
    RCM_PLLMF_3,
    RCM_PLLMF_4,
    RCM_PLLMF_5,
    RCM_PLLMF_6,
    RCM_PLLMF_7,
    RCM_PLLMF_8,
    RCM_PLLMF_9,
    RCM_PLLMF_10,
    RCM_PLLMF_11,
    RCM_PLLMF_12,
    RCM_PLLMF_13,
    RCM_PLLMF_14,
    RCM_PLLMF_15,
    RCM_PLLMF_16,
} RCM_PLLMF_T;

/**
 * @brief   RCM clock division
 */
typedef enum
{
    RCM_CLK_DIV_1,
    RCM_CLK_DIV_2,
    RCM_CLK_DIV_3,
    RCM_CLK_DIV_4,
    RCM_CLK_DIV_5,
    RCM_CLK_DIV_6,
    RCM_CLK_DIV_7,
    RCM_CLK_DIV_8,
    RCM_CLK_DIV_9,
    RCM_CLK_DIV_10,
    RCM_CLK_DIV_11,
    RCM_CLK_DIV_12,
    RCM_CLK_DIV_13,
    RCM_CLK_DIV_14,
    RCM_CLK_DIV_15,
    RCM_CLK_DIV_16,
} RCM_CLK_DIV_T;

/**
 * @brief   Clock output control
 */
typedef enum
{
    RCM_COC_NO_CLOCK,
    RCM_COC_HSI14,
    RCM_COC_LSI,
    RCM_COC_LSE,
    RCM_COC_SYSCLK,
    RCM_COC_HSI,
    RCM_COC_HSE,
    RCM_COC_PLLCLK_DIV_2,
    RCM_COC_HSI48,  //!< Only for APM32F072 and APM32F091 devices
    RCM_COC_PLLCLK
} RCM_COCCLK_T;

/**
 * @brief   Clock output division
 */
typedef enum
{
    RCM_COC_DIV_1,
    RCM_COC_DIV_2,
    RCM_COC_DIV_4,
    RCM_COC_DIV_8,
    RCM_COC_DIV_16,
    RCM_COC_DIV_32,
    RCM_COC_DIV_64,
    RCM_COC_DIV_128,
} RCM_COCPRE_T;

/**
 * @brief   System clock select
 */
typedef enum
{
    RCM_SYSCLK_SEL_HSI,
    RCM_SYSCLK_SEL_HSE,
    RCM_SYSCLK_SEL_PLL,
    RCM_SYSCLK_SEL_HSI48,    //!< Only for APM32F072 and APM32F091 devices
} RCM_SYSCLK_SEL_T;

/**
 * @brief   AHB divider Number
 */
typedef enum
{
    RCM_SYSCLK_DIV_1 = 7,
    RCM_SYSCLK_DIV_2,
    RCM_SYSCLK_DIV_4,
    RCM_SYSCLK_DIV_8,
    RCM_SYSCLK_DIV_16,
    RCM_SYSCLK_DIV_64,
    RCM_SYSCLK_DIV_128,
    RCM_SYSCLK_DIV_256,
    RCM_SYSCLK_DIV_512,
} RCM_AHBDIV_T;

/**
 * @brief   APB divider Number
 */
typedef enum
{
    RCM_HCLK_DIV_1 = 3,
    RCM_HCLK_DIV_2,
    RCM_HCLK_DIV_4,
    RCM_HCLK_DIV_8,
    RCM_HCLK_DIV_16
} RCM_APBDIV_T;

/**
 * @brief   CEC divider Number
 */
typedef enum
{
    RCM_CECCLK_HSI_DIV_224,
    RCM_CECCLK_LSI_DIV,
} RCM_CECCLK_T;

/**
 * @brief   I2C clock source selectio
 */
typedef enum
{
    RCM_I2C1CLK_HSI,
    RCM_I2C1CLK_SYSCLK,
} RCM_I2CCLK_T;

/**
 * @brief   USART clock source selectio
 */
typedef enum
{
    RCM_USART1CLK_PCLK    =   ((uint32_t)0x00010000),
    RCM_USART1CLK_SYSCLK  =   ((uint32_t)0x00010001),
    RCM_USART1CLK_LSE     =   ((uint32_t)0x00010002),
    RCM_USART1CLK_HSI     =   ((uint32_t)0x00010003),
    RCM_USART2CLK_PCLK    =   ((uint32_t)0x00020000),  //!< Only for APM32F072 and APM32F091 devices
    RCM_USART2CLK_SYSCLK  =   ((uint32_t)0x00020001),  //!< Only for APM32F072 and APM32F091 devices
    RCM_USART2CLK_LSE     =   ((uint32_t)0x00020002),  //!< Only for APM32F072 and APM32F091 devices
    RCM_USART2CLK_HSI     =   ((uint32_t)0x00020003),  //!< Only for APM32F072 and APM32F091 devices
    RCM_USART3CLK_PCLK    =   ((uint32_t)0x00040000),  //!< Only for APM32F091 devices
    RCM_USART3CLK_SYSCLK  =   ((uint32_t)0x00040001),  //!< Only for APM32F091 devices
    RCM_USART3CLK_LSE     =   ((uint32_t)0x00040002),  //!< Only for APM32F091 devices
    RCM_USART3CLK_HSI     =   ((uint32_t)0x00040003),  //!< Only for APM32F091 devices
} RCM_USARTCLK_T;

/**
 * @brief    USB clock source selection
 */
typedef enum
{
    RCM_USBCLK_HSI48,  //!< Only for APM32F072 devices
    RCM_USBCLK_PLLCLK,  //!< Only for APM32F072 devices
} RCM_USBCLK_T;

/**
 * @brief   RTC clock select
 */
typedef enum
{
    RCM_RTCCLK_LSE = 0X01,
    RCM_RTCCLK_LSI,
    RCM_RTCCLK_HSE_DIV_32
} RCM_RTCCLK_T;

/**
 * @brief   AHB peripheral
 */
typedef enum
{
    RCM_AHB_PERIPH_DMA1     = BIT0,
    RCM_AHB_PERIPH_DMA2     = BIT1,
    RCM_AHB_PERIPH_SRAM     = BIT2,
    RCM_AHB_PERIPH_FPU      = BIT4,
    RCM_AHB_PERIPH_CRC      = BIT6,
    RCM_AHB_PERIPH_GPIOA    = BIT17,
    RCM_AHB_PERIPH_GPIOB    = BIT18,
    RCM_AHB_PERIPH_GPIOC    = BIT19,
    RCM_AHB_PERIPH_GPIOD    = BIT20,
    RCM_AHB_PERIPH_GPIOE    = BIT21,  //!< Only for APM32F072 and APM32F091 devices
    RCM_AHB_PERIPH_GPIOF    = BIT22,
    RCM_AHB_PERIPH_TSC      = BIT24,
} RCM_AHB_PERIPH_T;

/**
 * @brief   AHB2 peripheral
 */
typedef enum
{
    RCM_APB2_PERIPH_SYSCFG  = BIT0,
    RCM_APB2_PERIPH_USART6  = BIT5,
    RCM_APB2_PERIPH_USART7  = BIT6,
    RCM_APB2_PERIPH_USART8  = BIT7,
    RCM_APB2_PERIPH_ADC1    = BIT9,
    RCM_APB2_PERIPH_TMR1    = BIT11,
    RCM_APB2_PERIPH_SPI1    = BIT12,
    RCM_APB2_PERIPH_USART1  = BIT14,
    RCM_APB2_PERIPH_TMR15   = BIT16,
    RCM_APB2_PERIPH_TMR16   = BIT17,
    RCM_APB2_PERIPH_TMR17   = BIT18,
    RCM_APB2_PERIPH_DBGMCU  = BIT22,
} RCM_APB2_PERIPH_T;

/**
 * @brief   AHB1 peripheral
 */
typedef enum
{
    RCM_APB1_PERIPH_TMR2    = BIT0,  //!< Only for APM32F072 and APM32F091 devices
    RCM_APB1_PERIPH_TMR3    = BIT1,
    RCM_APB1_PERIPH_TMR6    = BIT4,
    RCM_APB1_PERIPH_TMR7    = BIT5,  //!< Only for APM32F072 and APM32F091 devices
    RCM_APB1_PERIPH_TMR14   = BIT8,
    RCM_APB1_PERIPH_WWDT    = BIT11,
    RCM_APB1_PERIPH_SPI2    = BIT14,
    RCM_APB1_PERIPH_USART2  = BIT17,  
    RCM_APB1_PERIPH_USART3  = BIT18, //!< Only for APM32F072 and APM32F091 devices
    RCM_APB1_PERIPH_USART4  = BIT19, //!< Only for APM32F072 and APM32F091 devices
    RCM_APB1_PERIPH_USART5  = BIT20, //!< Only for APM32F091 devices
    RCM_APB1_PERIPH_I2C1    = BIT21,
    RCM_APB1_PERIPH_I2C2    = BIT22,
    RCM_APB1_PERIPH_USB     = BIT23, //!< Only for APM32F072 devices
    RCM_APB1_PERIPH_CAN     = BIT25, //!< Only for APM32F072 and APM32F091 devices
    RCM_APB1_PERIPH_CRS     = BIT27, //!< Only for APM32F072 and APM32F091 devices
    RCM_APB1_PERIPH_PMU     = BIT28,
    RCM_APB1_PERIPH_DAC     = BIT29, //!< Only for APM32F072 and APM32F091 devices
    RCM_APB1_PERIPH_CEC     = BIT30, //!< Only for APM32F072 and APM32F091 devices
} RCM_APB1_PERIPH_T;

/**
 * @brief   RCM Interrupt Source
 */
typedef enum
{
    RCM_INT_LSIRDY      = BIT0,      //!< LSI ready interrupt
    RCM_INT_LSERDY      = BIT1,      //!< LSE ready interrupt
    RCM_INT_HSIRDY      = BIT2,      //!< HSI ready interrupt
    RCM_INT_HSERDY      = BIT3,      //!< HSE ready interrupt
    RCM_INT_PLLRDY      = BIT4,      //!< PLL ready interrupt
    RCM_INT_HSI14RDY    = BIT5,      //!< HSI14 ready interrupt
    RCM_INT_HSI48RDY    = BIT6,      //!< HSI48 ready interrupt (Only for APM32F072 and APM32F091 devices)
    RCM_INT_CSS         = BIT7       //!< Clock security system interrupt
} RCM_INT_T;

/**
 * @brief   RCM FLAG define
 */
typedef enum
{
    RCM_FLAG_HSIRDY     = 0x001,      //!< HSI Ready Flag
    RCM_FLAG_HSERDY     = 0x011,      //!< HSE Ready Flag
    RCM_FLAG_PLLRDY     = 0x019,      //!< PLL Ready Flag
    RCM_FLAG_LSERDY     = 0x101,      //!< LSE Ready Flag
    RCM_FLAG_LSIRDY     = 0x201,      //!< LSI Ready Flag
    RCM_FLAG_V18PRRST   = 0x217,      //!< Software reset flag
    RCM_FLAG_OBRST      = 0x219,      //!< Option byte loader reset flag
    RCM_FLAG_PINRST     = 0x21A,      //!< PIN reset flag
    RCM_FLAG_PWRRST     = 0x21B,      //!< POR/PDR reset flag
    RCM_FLAG_SWRST      = 0x21C,      //!< Software reset flag
    RCM_FLAG_IWDTRST    = 0x21D,      //!< Independent watchdog reset flag
    RCM_FLAG_WWDTRST    = 0x21E,      //!< Window watchdog reset flag
    RCM_FLAG_LPRRST     = 0x21F,      //!< Low-power reset flag
    RCM_FLAG_HSI14RDY   = 0x301,      //!< HSI14 Ready Flag
    RCM_FLAG_HSI48RDY   = 0x311,      //!< HSI48 Ready Flag (Only for APM32F072 devices and APM32F091 devices)
} RCM_FLAG_T;

/**@} end of group RCM_Enumerations*/


/** @addtogroup RCM_Fuctions Fuctions
  @{
*/

/** Function description */

void RCM_Reset(void);

void RCM_ConfigHSE(RCM_HSE_T state);
uint8_t RCM_WaitHSEReady(void);
void RCM_SetHSITrim(uint8_t HSITrim);
void RCM_EnableHSI(void);
void RCM_DisableHSI(void);

void RCM_SetHSI14Trim(uint8_t HSI14Trim);
void RCM_EnableHSI14(void);
void RCM_DisableHSI14(void);
void RCM_EnableHSI14ADC(void);
void RCM_DisableHSI14ADC(void);

void RCM_ConfigLSE(RCM_LSE_T state);
void RCM_ConfigDriveLSE(RCM_LSE_DRIVE_T state);

void RCM_EnableLSI(void);
void RCM_DisableLSI(void);

void RCM_ConfigPLL(RCM_PLL_SEL_T pllSelect, RCM_PLLMF_T pllMf);
void RCM_EnablePLL(void);
void RCM_DisablePLL(void);

void RCM_EnableHSI48(void);   //!< Only for APM32F072 and APM32F091 devices
void RCM_DisableHSI48(void);  //!< Only for APM32F072 and APM32F091 devices
uint32_t RCM_ReadHSI48CalibrationValue(void);  //!< Only for APM32F072 and APM32F091 devices

void RCM_ConfigCLKDIV(RCM_CLK_DIV_T state);

void RCM_EnableCCS(void);
void RCM_DisableCCS(void);

#if defined (APM32F030) || defined (APM32F051)
void RCM_ConfigCOC(RCM_COCCLK_T cocClock);
#else
void RCM_ConfigCOC(RCM_COCCLK_T cocClock, RCM_COCPRE_T divided);
#endif

void RCM_ConfigSYSCLK(RCM_SYSCLK_SEL_T sysClkSelect);
RCM_SYSCLK_SEL_T RCM_ReadSYSCLKSource(void);

void RCM_ConfigAHB(RCM_AHBDIV_T AHBDiv);
void RCM_ConfigAPB(RCM_APBDIV_T APBDiv);
void RCM_ConfigCECCLK(RCM_CECCLK_T CECClk);
void RCM_ConfigI2CCLK(RCM_I2CCLK_T I2CCLk);
void RCM_ConfigUSARTCLK(RCM_USARTCLK_T USARTClk);
void RCM_ConfigUSBCLK(RCM_USBCLK_T USBClk);  //!< Only for APM32F072 devices

uint32_t RCM_ReadSYSCLKFreq(void);
uint32_t RCM_ReadHCLKFreq(void);
uint32_t RCM_ReadPCLKFreq(void);
uint32_t RCM_ReadADCCLKFreq(void);
uint32_t RCM_ReadCECCLKFreq(void);
uint32_t RCM_ReadI2C1CLKFreq(void);
uint32_t RCM_ReadUSART1CLKFreq(void);
uint32_t RCM_ReadUSART2CLKFreq(void);
uint32_t RCM_ReadUSBCLKFreq(void);

void RCM_ConfigRTCCLK(RCM_RTCCLK_T RTCClk);
void RCM_EnableRTCCLK(void);
void RCM_DisableRTCCLK(void);

void RCM_EnableBackupReset(void);
void RCM_DisableBackupReset(void);

void RCM_EnableAHBPeriphClock(uint32_t AHBPeriph);
void RCM_DisableAHBPeriphClock(uint32_t AHBPeriph);
void RCM_EnableAPB2PeriphClock(uint32_t APB2Periph);
void RCM_DisableAPB2PeriphClock(uint32_t APB2Periph);
void RCM_EnableAPB1PeriphClock(uint32_t APB1Periph);
void RCM_DisableAPB1PeriphClock(uint32_t APB1Periph);

void RCM_EnableAHBPeriphReset(uint32_t AHBPeriph);
void RCM_DisableAHBPeriphReset(uint32_t AHBPeriph);
void RCM_EnableAPB1PeriphReset(uint32_t APB1Periph);
void RCM_DisableAPB1PeriphReset(uint32_t APB1Periph);
void RCM_EnableAPB2PeriphReset(uint32_t APB2Periph);
void RCM_DisableAPB2PeriphReset(uint32_t APB2Periph);

void RCM_EnableInterrupt(uint8_t interrupt);
void RCM_DisableInterrupt(uint8_t interrupt);
uint16_t RCM_ReadStatusFlag(RCM_FLAG_T flag);
void RCM_ClearStatusFlag(void);
uint8_t RCM_ReadIntFlag(RCM_INT_T flag);
void RCM_ClearIntFlag(uint8_t flag);

/**@} end of group RCM_Fuctions*/
/**@} end of group RCM_Driver*/
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __RCM_H */
