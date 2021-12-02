/*!
 * @file        apm32f0xx_iwdt.h
 *
 * @brief       This file contains all the functions prototypes for the IWDT firmware library
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#ifndef __IWDT_H
#define __IWDT_H

#include "apm32f0xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup IWDT_Driver IWDT Driver
  @{
*/

/** @addtogroup IWDT_Enumerations Enumerations
  @{
*/

/**
 * @brief   IWDT key definition
 */
typedef enum
{
    IWDT_KEY_REFRESH = ((uint16_t)0xAAAA),
    IWDT_KEY_ENABLE  = ((uint16_t)0xCCCC),
    IWDT_KEY_ACCESS  = ((uint16_t)0x5555),
} IWDT_KEY_T;

/**
 * @brief   IWDT divider
 */
typedef enum
{
    IWDT_DIV_4   = ((uint8_t)0x00),
    IWDT_DIV_8   = ((uint8_t)0x01),
    IWDT_DIV_16  = ((uint8_t)0x02),
    IWDT_DIV_32  = ((uint8_t)0x03),
    IWDT_DIV_64  = ((uint8_t)0x04),
    IWDT_DIV_128 = ((uint8_t)0x05),
    IWDT_DIV_256 = ((uint8_t)0x06),
} IWDT_DIV_T;

/**
 * @brief   IWDT flag definition
 */
typedef enum
{
    IWDT_FLAG_DIVU = ((uint8_t)0X01),
    IWDT_FLAG_CNTU = ((uint8_t)0X02),
    IWDT_FLAG_WINU = ((uint8_t)0X04),
} IWDT_FLAG_T;

/**@} end of group IWDT_Enumerations*/


/** @addtogroup IWDT_Fuctions Fuctions
  @{
*/

/** Enable IWDT */
void IWDT_Enable(void);

/** Refresh IWDT */
void IWDT_Refresh(void);

/** Window Value */
void IWDT_ConfigWindowValue(uint16_t windowValue);

/** Set Counter reload */
void IWDT_ConfigReload(uint16_t reload);

/** Write Access */
void IWDT_EnableWriteAccess(void);
void IWDT_DisableWriteAccess(void);

/** divider */
void IWDT_ConfigDivider(IWDT_DIV_T div);

/** flag */
uint8_t IWDT_ReadStatusFlag(uint8_t flag);

/**@} end of group IWDT_Fuctions*/
/**@} end of group IWDT_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __IWDT_H */
