/*!
 * @file        apm32f0xx_eint.c
 *
 * @brief       This file contains all the functions for the EINT peripheral
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#include "apm32f0xx_eint.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup EINT_Driver EINT Driver
  @{
*/

/** @addtogroup EINT_Fuctions Fuctions
  @{
*/

/*!
 * @brief     Set the EINT peripheral registers to their default reset values
 *
 * @param     None

 * @retval    None
 */
void EINT_Reset(void)
{
    EINT->IMASK = EINT_INTMASK_RESET_VALUE;
    EINT->EMASK = EINT_EVTMASK_RESET_VALUE;
    EINT->RTEN = EINT_RTSEL_RESET_VALUE;
    EINT->FTEN = EINT_FTSEL_RESET_VALUE;
    EINT->IPEND  = EINT_PEND_RESET_VALUE;
}

/*!
 * @brief       Configure the EINT
 *
 * @param       eintConfig: pointer to EINT_Config_T structure
 *
 * @retval      None
 */

void EINT_Config( EINT_Config_T* eintConfig)
{
    if (eintConfig->lineCmd  == DISABLE)
    {
        if (eintConfig->mode == EINT_MODE_INTERRUPT)
        {
            EINT->IMASK &= ~eintConfig->line;
        }
        else if (eintConfig->mode == EINT_MODE_EVENT)
        {
            EINT->EMASK &= ~eintConfig->line;
        }
    }
    else
    {
        if (eintConfig->mode == EINT_MODE_INTERRUPT)
        {
            EINT->IMASK |= eintConfig->line;
        }
        else if (eintConfig->mode == EINT_MODE_EVENT)
        {
            EINT->EMASK |= eintConfig->line;
        }

        if (eintConfig->trigger == EINT_TRIGGER_RISING)
        {
            EINT->RTEN |= eintConfig->line;
        }
        else if (eintConfig->trigger == EINT_TRIGGER_FALLING)
        {
            EINT->FTEN |= eintConfig->line;
        }
        else
        {
            EINT->RTEN |= eintConfig->line;
            EINT->FTEN |= eintConfig->line;
        }

    }
}

/*!
 * @brief       Fills each EINT_Config_T member with its default value
 *
 * @param       eintConfig: Pointer to a EINT_Config_T structure which will be initialized
 *
 * @retval      None
 */
void EINT_ConfigStructInit(EINT_Config_T* eintConfig)
{
    eintConfig->line = EINT_LINENONE;
    eintConfig->mode = EINT_MODE_INTERRUPT;
    eintConfig->trigger = EINT_TRIGGER_FALLING;
    eintConfig->lineCmd = DISABLE;
}

/*!
 * @brief     Select Software interrupt on EINT line
 *
 * @param     line: specifies the EINT line on which the software interrupt
 *
 * @retval    None
 */
void EINT_SelectSWInterrupt(uint32_t line)
{
    EINT->SWINTE |= (uint32_t)line;
}

/*!
 * @brief    Read the specified EINT_Line flag
 *
 * @param    line: Select the EINT Line
 *
 * @retval   status: The new state of flag (SET or RESET)
 */
uint8_t EINT_ReadStatusFlag(uint32_t line)
{
    uint8_t status = RESET;

    if ((EINT->IPEND & line) != (uint32_t)RESET)
    {
        status = SET;
    }
    else
    {
        status = RESET;
    }

    return status;
}

/*!
 * @brief    Clears the EINT_Line pending bits
 *
 * @param    line: Select the EINT Line
 *
 * @retval   None
 */
void EINT_ClearStatusFlag(uint32_t line)
{
    EINT->IPEND = line;
}

/*!
 * @brief    Read the specified EINT_Line Interrupt Flag
 *
 * @param    line: Select the EINT Line
 *
 * @retval   None
 */
uint8_t EINT_ReadIntFlag(uint32_t line)
{
    uint8_t status = RESET;
    uint32_t enablestatus = 0;

    enablestatus = EINT->IMASK & line;

    if ((EINT->IPEND & line) != ((uint32_t)RESET) && (enablestatus != (uint32_t)RESET))
    {
        status = SET;
    }
    else
    {
        status = RESET;
    }

    return status;
}

/*!
 * @brief    Clears the EINT_LINE pending bits
 *
 * @param    line: Select the EINT Line
 *
 * @retval   None
 */
void EINT_ClearIntFlag(uint32_t line)
{
    EINT->IPEND = line;
}


/**@} end of group EINT_Fuctions*/
/**@} end of group EINT_Driver */
/**@} end of group Peripherals_Library*/

