/*!
 * @file        apm32f0xx_ob.c
 *
 * @brief       This file contains all the functions for the OB peripheral
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#include "apm32f0xx_ob.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup OB_Driver OB Driver
  @{
*/

/** @addtogroup OB_Fuctions Fuctions
  @{
*/

/*!
 * @brief     Read Flash Protection Level
 *
 * @param     readProtection:   Specifies the read protection level.
 *                              The parameter can be one of following values:
 *                              @arg OB_READ_PRO_LEVEL0:  No protection
 *                              @arg OB_READ_PRO_LEVEL1:  Read protection of the memory
 * @retval    None
 */
void OB_ReadProtectionOptionByte(OB_READ_PRO_T readProtection)
{
    OB->READPORT_B.READPROT = readProtection;
}

/*!
 * @brief     Option Bytes Watchdog
 *
 * @param     wdt: Select Watchdog SW/HW
 *
 * @retval    None
 */
void OB_OptionBytesWatchdog(OB_WDT_T wdt)
{
    OB->USER_B.WDTSEL = wdt;
}

/*!
 * @brief     Option Bytes nRST STOP
 *
 * @param     stop: Select nRST STOP RST/SET
 *
 * @retval    None
 */
void OB_OptionBytesStop(OB_STOP_T stop)
{
    OB->USER_B.RSTSTOP = stop;
}

/*!
 * @brief     Option Bytes nRST STDBY
 *
 * @param     standby: Select nRST STDBY RST/SET
 *
 * @retval    None
 */
void OB_OptionBytesStandby(OB_STANDBY_T standby)
{
    OB->USER_B.RSTSTDBY = standby;
}

/*!
 * @brief     Option Bytes nBOOT1
 *
 * @param     boot: Select nRST BOOT1 RST/SET
 *
 * @retval    None
 */
void OB_OptionBytesBoot1(OB_BOOT1_T boot)
{
    OB->USER_B.BOT1 = boot;
}

/*!
 * @brief     Option Bytes VDDA_Analog_Monitoring
 *
 * @param     vdda: Select VDDA ANALOG OFF/ON
 *
 * @retval    None
 */
void OB_OptionBytesVddaAnalog(OB_VDDA_T vdda)
{
    OB->USER_B.VDDAMON = vdda;
}

/*!
 * @brief     Option Bytes RAM PARITY CHECK
 *
 * @param     rpc: Select RAM PARITY OFF/ON
 *
 * @retval    None
 */
void OB_OptionBytesRamParity(OB_RAM_PARITY_CHECK_T ramParityCheck)
{
    OB->USER_B.RPC = ramParityCheck;
}

/**@} end of group OB_Fuctions */
/**@} end of group OB_Driver */
/**@} end of group Peripherals_Library */
