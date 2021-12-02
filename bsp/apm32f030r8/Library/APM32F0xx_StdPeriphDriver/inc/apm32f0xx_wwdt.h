/*!
 * @file        apm32f0xx_wwdt.h
 *
 * @brief       This file contains all the functions prototypes for the WWDG firmware library
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#ifndef __WWDT_H
#define __WWDT_H

#include "apm32f0xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup WWDT_Driver  WWDT Driver
  @{
*/

/** @addtogroup  WWDT_Enumerations Enumerations
  @{
*/

/**
 * @brief   WWDT Timebase(Prescaler) define
 */
typedef enum
{
    WWDT_DIV_1 = ((uint8_t)0x00),
    WWDT_DIV_2 = ((uint8_t)0x01),
    WWDT_DIV_4 = ((uint8_t)0x02),
    WWDT_DIV_8 = ((uint8_t)0x03)
} WWDT_DIV_T;

/**@} end of group WWDT_Enumerations*/

/** @addtogroup  WWDT_Fuctions Fuctions
  @{
*/

/** WWDT reset */
void WWDT_Reset(void);

/** Set WWDT Timebase */
void WWDT_SetTimebase(uint32_t div);

/** Set Window Data */
void WWDT_ConfigWindowValue(uint16_t windowValue);

/** Set Couter */
void WWDT_ConfigCounter(uint8_t couter);

/** Enable WWDT and Early Wakeup interrupt */
void WWDT_EnableEWI(void);
void WWDT_Enable(uint8_t count);

/** Read Flag and Clear Flag */
uint8_t WWDT_ReadStatusFlag(void);
void WWDT_ClearStatusFlag(void);

/**@} end of group WWDT_Fuctions*/
/**@} end of group WWDT_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __WWDT_H */

