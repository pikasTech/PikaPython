/*!
 * @file        apm32f0xx_pmu.h
 *
 * @brief       This file contains all functions prototype and macros for the PMU peripheral
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#ifndef __PMU_H
#define __PMU_H

#include "apm32f0xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup PMU_Driver PMU Driver
  @{
*/

/** @addtogroup PMU_Enumerations Enumerations
  @{
*/

/**
 * @brief   PVD detection level (Not for APM32F030 devices)
 */
typedef enum
{
    PMU_PVDLEVEL_0 = ((uint8_t)0x00),
    PMU_PVDLEVEL_1 = ((uint8_t)0x01),
    PMU_PVDLEVEL_2 = ((uint8_t)0x02),
    PMU_PVDLEVEL_3 = ((uint8_t)0x03),
    PMU_PVDLEVEL_4 = ((uint8_t)0x04),
    PMU_PVDLEVEL_5 = ((uint8_t)0x05),
    PMU_PVDLEVEL_6 = ((uint8_t)0x06),
    PMU_PVDLEVEL_7 = ((uint8_t)0x07),
} PMU_PVDLEVEL_T;

/**
 * @brief   Wake Up PIN 
 */
typedef enum
{
    PMU_WAKEUPPIN_1 = ((uint16_t)0x0100),
    PMU_WAKEUPPIN_2 = ((uint16_t)0x0200),
    PMU_WAKEUPPIN_3 = ((uint16_t)0x0400),  //!< Only for APM32F072/091 devices
    PMU_WAKEUPPIN_4 = ((uint16_t)0x0800),  //!< Only for APM32F072/091 devices
    PMU_WAKEUPPIN_5 = ((uint16_t)0x1000),  //!< Only for APM32F072/091 devices
    PMU_WAKEUPPIN_6 = ((uint16_t)0x2000),  //!< Only for APM32F072/091 devices
    PMU_WAKEUPPIN_7 = ((uint16_t)0x4000),  //!< Only for APM32F072/091 devices
    PMU_WAKEUPPIN_8 = ((uint16_t)0x8000),  //!< Only for APM32F072/091 devices
} PMU_WAKEUPPIN_T;

/**
 * @brief   Sleep mode entry
 */
typedef enum
{
    PMU_SLEEPENTRY_WFI = 0x00,
    PMU_SLEEPENTRY_WFE = 0x01,
} PMU_SLEEPENTRY_T;

/**
 * @brief   Regulator state is Sleep/Stop mode
 */
typedef enum
{
    PMU_REGULATOR_ON = 0x00,
    PMU_REGULATOR_LowPower = 0x01,
} PMU_REGULATOR_T;

/**
 * @brief   Stop mode entry
 */
typedef enum
{
    PMU_STOPENTRY_WFI = 0x00,
    PMU_STOPENTRY_WFE = 0x01,
    PMU_STOPENTRY_SLEEPONEXIT = 0x02,
} PMU_STOPENTRY_T;

/**
 * @brief   Flag
 */
typedef enum
{
    PMU_FLAG_WUPF      = 0x01,
    PMU_FLAG_STDBYF    = 0x02,
    PMU_FLAG_PVDOF     = 0x04, //!< Not for APM32F030 devices
    PMU_FLAG_VREFINTF  = 0x08,
} PMU_FLAG_T;

/**@} end of group PMU_Enumerations*/


/** @addtogroup PMU_Fuctions Fuctions
  @{
*/

/** Function used to set the PMU configuration to the default reset state */
void PMU_Reset(void);

/** Backup Domain Access function */
void PMU_EnableBackupAccess(void);
void PMU_DisableBackupAccess(void);

/** PVD configuration functions */
void PMU_ConfigPVDLevel(PMU_PVDLEVEL_T level);  //!< Not for APM32F030 devices
void PMU_EnablePVD(void);                       //!< Not for APM32F030 devices
void PMU_DisablePVD(void);                      //!< Not for APM32F030 devices

/** WakeUp pins configuration functions */
void PMU_EnableWakeUpPin(PMU_WAKEUPPIN_T pin);
void PMU_DisableWakeUpPin(PMU_WAKEUPPIN_T pin);

/** Low Power modes configuration functions */
void PMU_EnterSleepMode(PMU_SLEEPENTRY_T entry);
void PMU_EnterSTOPMode(PMU_REGULATOR_T regulator, PMU_STOPENTRY_T entry);
void PMU_EnterSTANDBYMode(void);

/** Flags management functions */
uint8_t PMU_ReadStatusFlag(PMU_FLAG_T flag);
void PMU_ClearStatusFlag(uint8_t flag);

/**@} end of group PMU_Fuctions*/
/**@} end of group PMU_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __PMU_H */

