/*!
 * @file        apm32f0xx_dbg.c
 *
 * @brief       This file provides all the DBG firmware functions
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#include "apm32f0xx_dbg.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup DBG_Driver DBG Driver
  @{
*/

/** @addtogroup DBG_Fuctions Fuctions
  @{
*/

/*!
 * @brief     Read Device Identifier
 *
 * @param     None
 *
 * @retval    The value of the Device Identifier
 */
uint32_t DBG_ReadDevId(void)
{
    return ((uint32_t)DBG->IDCODE_B.EQR);
}

/*!
 * @brief     Read Revision Identifier
 *
 * @param     None
 *
 * @retval    The value of the Revision Identifier
 */
uint32_t DBG_ReadRevId(void)
{
    return ((uint32_t)DBG->IDCODE_B.WVR);
}

/*!
 * @brief     Enable Debug Mode
 *
 * @param     mode: specifies the low power mode.
 *                  The parameter can be combination of following values:
 *                  @arg DBG_MODE_STOP:    Keep debugger connection during STOP mode
 *                  @arg DBG_MODE_STANDBY: Keep debugger connection during STANDBY mode
 * @retval    None
 */
void DBG_EnableDebugMode(uint32_t mode)
{
    DBG->CFG |= (uint32_t)mode;
}

/*!
 * @brief     Disable Debug Mode
 *
 * @param     mode: specifies the low power mode.
 *                  The parameter can be combination of following values:
 *                  @arg DBG_MODE_STOP:    Keep debugger connection during STOP mode
 *                  @arg DBG_MODE_STANDBY: Keep debugger connection during STANDBY mode
 * @retval    None
 */
void DBG_DisableDebugMode(uint32_t mode)
{
    DBG->CFG &= (uint32_t)~mode;
}

/*!
 * @brief     Enable APB1 peripheral in Debug mode.
 *
 * @param     peripheral: Specifies the APB1 peripheral.
 *                        The parameter can be combination of following values:
 *                        @arg DBG_APB1_PER_TMR2_STOP:    TMR2  counter stopped when Core is halted, Not for APM32F030 devices
 *                        @arg DBG_APB1_PER_TMR3_STOP:    TMR3  counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_TMR6_STOP:    TMR6  counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_TMR7_STOP:    TMR7  counter stopped when Core is halted£¬Only for APM32F072 and APM32F091 devices
 *                        @arg DBG_APB1_PER_TMR14_STOP:   TMR14 counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_RTC_STOP:     RTC   counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_WWDT_STOP:    Debug WWDT stopped when Core is halted
 *                        @arg DBG_APB1_PER_IWDT_STOP:    Debug IWDT stopped when Core is halted
 *                        @arg DBG_APB1_PER_CAN_STOP:     Debug CAN stopped when Core is halted£¬Only for APM32F072 and APM32F091 devices
 *                        @arg DBG_APB1_PER_I2C1_SMBUS_TIMEOUT: I2C1 SMBUS timeout mode stopped when Core is halted
 * @retval      None
 */
void DBG_EnableAPB1Periph(uint32_t peripheral)
{
    DBG->APB1F |= (uint32_t)peripheral;
}

/*!
 * @brief     Disable APB1 peripheral in Debug mode.
 *
 * @param     peripheral: Specifies the APB1 peripheral.
 *                        The parameter can be combination of following values:
 *                        @arg DBG_APB1_PER_TMR2_STOP:    TMR2  counter stopped when Core is halted, Not for APM32F030 devices
 *                        @arg DBG_APB1_PER_TMR3_STOP:    TMR3  counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_TMR6_STOP:    TMR6  counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_TMR7_STOP:    TMR7  counter stopped when Core is halted£¬Only for APM32F072 and APM32F091 devices
 *                        @arg DBG_APB1_PER_TMR14_STOP:   TMR14 counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_RTC_STOP:     RTC   counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_WWDT_STOP:    Debug WWDT stopped when Core is halted
 *                        @arg DBG_APB1_PER_IWDT_STOP:    Debug IWDT stopped when Core is halted
 *                        @arg DBG_APB1_PER_CAN_STOP:     Debug CAN stopped when Core is halted£¬Only for APM32F072 and APM32F091 devices
 *                        @arg DBG_APB1_PER_I2C1_SMBUS_TIMEOUT: I2C1 SMBUS timeout mode stopped when Core is halted
 * @retval      None
 */
void DBG_DisableAPB1Periph(uint32_t peripheral)
{
    DBG->APB1F &= (uint32_t)~peripheral;
}

/*!
 * @brief     Enable APB2 peripheral in Debug mode.
 *
 * @param     peripheral: Specifies the APB2 peripheral.
 *                        The parameter can be combination of following values:
 *                        @arg DBG_APB2_PER_TMR1_STOP:    TMR1  counter stopped when Core is halted
 *                        @arg DBG_APB2_PER_TMR15_STOP:   TMR15 counter stopped when Core is halted
 *                        @arg DBG_APB2_PER_TMR16_STOP:   TMR16 counter stopped when Core is halted
 *                        @arg DBG_APB2_PER_TMR17_STOP:   TMR17 counter stopped when Core is halted
 * @retval      None
 */
void DBG_EnableAPB2Periph(uint32_t peripheral)
{
    DBG->APB2F |= (uint32_t)peripheral;
}

/*!
 * @brief     Disable APB2 peripheral in Debug mode.
 *
 * @param     peripheral: Specifies the APB2 peripheral.
 *                        The parameter can be combination of following values:
 *                        @arg DBG_APB2_PER_TMR1_STOP:    TMR1  counter stopped when Core is halted
 *                        @arg DBG_APB2_PER_TMR15_STOP:   TMR15 counter stopped when Core is halted
 *                        @arg DBG_APB2_PER_TMR16_STOP:   TMR16 counter stopped when Core is halted
 *                        @arg DBG_APB2_PER_TMR17_STOP:   TMR17 counter stopped when Core is halted
 *
 * @retval    None
 */
void DBG_DisableAPB2Periph(uint32_t peripheral)
{
    DBG->APB2F &= (uint32_t)~peripheral;
}

/**@} end of group DBG_Fuctions*/
/**@} end of group DBG_Driver */
/**@} end of group Peripherals_Library*/

