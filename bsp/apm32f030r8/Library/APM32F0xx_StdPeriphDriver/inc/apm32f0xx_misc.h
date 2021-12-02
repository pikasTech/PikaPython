/*!
 * @file        apm32f0xx_misc.h
 *
 * @brief       This file contains all the functions prototypes for the miscellaneous
 *              firmware library functions (add-on to CMSIS functions).
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#ifndef __MISC_H
#define __MISC_H

#include "apm32f0xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup MISC_Driver MISC Driver
  @{
*/

/** @addtogroup MISC_Fuctions Fuctions
  @{
*/

/**
 * @brief    System low power mode
 */
typedef enum
{
    NVIC_LOWPOER_SEVONPEND   = 0x10, //!< Low Power SEV on Pend
    NVIC_LOWPOER_SLEEPDEEP   = 0x04, //!< Low Power DEEPSLEEP request
    NVIC_LOWPOER_SLEEPONEXIT = 0x02  //!< Low Power Sleep on Exit
} NVIC_LOWPOER_T;

/**@} end of group MISC_Enumerations*/


/** @addtogroup MISC_Macros Macros
  @{
*/

/** Macros description */

/** MISC SysTick clock source */
#define SysTick_CLKSource_HCLK_Div8 ((uint32_t)0xFFFFFFFB)
#define SysTick_CLKSource_HCLK      ((uint32_t)0x00000004)

/**@} end of group MISC_Macros*/


/** @addtogroup MISC_Fuctions Fuctions
  @{
*/

/** NVIC */
void NVIC_EnableIRQRequest(IRQn_Type irq, uint8_t priority);
void NVIC_DisableIRQRequest(IRQn_Type irq);

/** Low Power */
void NVIC_EnableSystemLowPower(uint8_t lowPowerMode);
void NVIC_DisableSystemLowPower(uint8_t lowPowerMode);

/** SysTick */
void SysTick_ConfigCLKSource(uint32_t sysTickCLKSource);

/** PMU */
void PMU_EnterWaitMode(void);
void PMU_EnterHaltModeWFI(void);
void PMU_EnterHaltModeWFE(void);

/**@} end of group MISC_Fuctions*/
/**@} end of group MISC_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __MISC_H */
