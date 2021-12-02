/*!
 * @file        apm32f0xx_tsc.c
 *
 * @brief       This file contains all the functions for the TSC peripheral
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#include "apm32f0xx_tsc.h"
#include "apm32f0xx_rcm.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup TSC_Driver TSC Driver
  @{
*/

/** @addtogroup TSC_Fuctions Fuctions
  @{
*/
/*!
 * @brief     Set the TSC peripheral registers to their default reset values
 *
 * @param     None
 *
 * @retval    None
 */
void TSC_Reset(void)
{
    RCM_EnableAHBPeriphReset(RCM_AHB_PERIPH_TSC);
    RCM_DisableAHBPeriphReset(RCM_AHB_PERIPH_TSC);
}

/*!
 * @brief       Config the ADC peripheral according to the specified parameters in the adcConfig
 *
 * @param       tscConfig:  Pointer to a TSC_Config_T structure that
 *                          contains the configuration information for the TSC peripheral
 *
 * @retval      None
 */
void TSC_Config(TSC_Config_T* tscConfig)
{
    /** Set all functions */
    TSC->CTRL_B.CTPHSEL   = tscConfig->CTPHSEL;
    TSC->CTRL_B.CTPLSEL   = tscConfig->CTPLSEL;
    TSC->CTRL_B.SSERRVSEL = tscConfig->SpreadSpectrumDev;
    TSC->CTRL_B.SSEN      = tscConfig->SpreadSpectrum;
    TSC->CTRL_B.SSCDFSEL  = tscConfig->SpreadSpectrumPre;
    TSC->CTRL_B.PGCDFSEL  = tscConfig->PulseGeneratorPre;
    TSC->CTRL_B.MCNTVSEL  = tscConfig->MCountValue;
    TSC->CTRL_B.IODEFCFG  = tscConfig->IOMode;
    TSC->CTRL_B.SYNPPOL   = tscConfig->SynchroPinPolarity;
    TSC->CTRL_B.AMCFG     = tscConfig->AcqMode;
    
    /* Clear flags */
    TSC->INTFCLR = 0x00000003;
    
    /* Config interrupts */
    TSC->INTEN = tscConfig->Interrupts;
}

/*!
 * @brief       Fills each adcConfig member with its default value
 *
 * @param       tscConfig:  Pointer to a TSC_Config_T structure which will be initialized
 *
 * @retval      None
 */
void TSC_ConfigStructInit(TSC_Config_T* tscConfig)
{
    tscConfig->CTPHSEL            = TSC_CTPH_2CYCLE;
    tscConfig->CTPLSEL            = TSC_CTPL_2CYCLE;
    tscConfig->SpreadSpectrum     = ENABLE;
    tscConfig->SpreadSpectrumDev  = 1;
    tscConfig->SpreadSpectrumPre  = TSC_SS_PRE1;
    tscConfig->PulseGeneratorPre  = TSC_PG_PRESC1;
    tscConfig->MCountValue        = TSC_MCE_4095;
    tscConfig->IOMode             = TSC_IODM_OUT_PP_LOW;
    tscConfig->SynchroPinPolarity = TSC_SYNPPOL_FALLING;
    tscConfig->AcqMode            = TSC_ACQMOD_NORMAL;
    tscConfig->Interrupts         = TSC_INT_NONE;
}

/*!
 * @brief       Configure TSC IOs
 *
 * @param       tscConfigIO:  config Pointer to the configuration structure
 *
 * @retval      None
 */
void TSC_ConfigIO(TSC_ConfigIO_T* tscConfigIO)
{
    uint32_t IOChannel;
    uint8_t groups = 0x00;
    uint8_t i;
    
    /** Stop acquisition  */
    TSC->CTRL_B.STARTAFLG = BIT_RESET;

    /** Disable Schmitt trigger hysteresis on all used TSC IOs */
    TSC->IOHCTRL = (~(tscConfigIO->IOChannel | tscConfigIO->IOShield | tscConfigIO->IOSampling));

    /** Set channel and shield IOs */
    TSC->IOCHCTRL = (tscConfigIO->IOChannel & (~tscConfigIO->IOShield));

    /** Set sampling IOs */
    TSC->IOSMPCTRL = tscConfigIO->IOSampling;

    /** Set groups to be acquired */
    IOChannel = tscConfigIO->IOChannel;

    for (i = 0; i < 8; i++)
    {
        if ((IOChannel & 0x0F) != 0x00)
        {
            groups |= (0x01 << i);
        }

        IOChannel >>= 0x04;
    }

    TSC->IOGCSTS = groups;
}

/*!
 * @brief       Start the acquisition
 *
 * @param       None
 *
 * @retval      None
 */
void TSC_Start(void)
{
    TSC->CTRL_B.STARTAFLG = BIT_SET;
}

/*!
 * @brief       Stop the acquisition
 *
 * @param       None
 *
 * @retval      None
 */
void TSC_Stop(void)
{
    TSC->CTRL_B.STARTAFLG = BIT_RESET;
}

/*!
 * @brief       Enable the TSC peripheral
 *
 * @param       None
 *
 * @retval      None
 */
void TSC_Enable(void)
{
    TSC->CTRL_B.TSCEN = BIT_SET;
}

/*!
 * @brief       Disable the TSC peripheral
 *
 * @param       None
 *
 * @retval      None
 */
void TSC_Disable(void)
{
    TSC->CTRL_B.TSCEN = BIT_RESET;
}

/*!
 * @brief       Start acquisition and wait until completion.
 *
 * @param       None
 *
 * @retval      The acquisition state
 */
uint8_t TSC_PollForAcquisition(void)
{
    if (TSC->INTSTS_B.EOAFLG == BIT_SET)
    {
        if (TSC->INTSTS_B.MCEFLG == BIT_RESET)
        {
            return TSC_ACQ_FINISH;
        }
        else
        {
            return TSC_ACQ_ERROR;
        }
    }

    return TSC_ACQ_BUSY;
}

/*!
 * @brief       Read the acquisition value.
 *
 * @param       group:  Specifies the TSC IO Group from 1 to 6
 *                      The parameter can be combination of following values:
 *                          @arg TSC_GROUP_1 : IO Group 1
 *                          @arg TSC_GROUP_2 : IO Group 2
 *                          @arg TSC_GROUP_3 : IO Group 3
 *                          @arg TSC_GROUP_4 : IO Group 4
 *                          @arg TSC_GROUP_5 : IO Group 5
 *                          @arg TSC_GROUP_6 : IO Group 6
 *
 * @retval      The counter value
 */
uint16_t TSC_ReadGroupValue(TSC_GROUP_T group)
{
    uint16_t value;

    /** Return the group acquisition counter */
    switch (group)
    {
        case 1:
            value = TSC->IOG1CNT_B.CNTVAL;
            break;

        case 2:
            value = TSC->IOG2CNT_B.CNTVAL;
            break;

        case 3:
            value = TSC->IOG3CNT_B.CNTVAL;
            break;

        case 4:
            value = TSC->IOG4CNT_B.CNTVAL;
            break;

        case 5:
            value = TSC->IOG5CNT_B.CNTVAL;
            break;

        case 6:
            value = TSC->IOG6CNT_B.CNTVAL;
            break;
    }

    return value;
}

/*!
 * @brief       Enable the TSC SpreadSpectrum
 *
 * @param       None
 *
 * @retval      None
 */
void TSC_EnableSpreadSpectrum(void)
{
    TSC->CTRL_B.SSEN = BIT_SET;
}

/*!
 * @brief       Disable the TSC SpreadSpectrum
 *
 * @param       None
 *
 * @retval      None
 */
void TSC_DisableSpreadSpectrum(void)
{
    TSC->CTRL_B.SSEN = BIT_RESET;
}

/*!
 * @brief       Enables the specified interrupts
 *
 * @param       interrupt:  Specifies the TSC interrupts sources
 *                          The parameter can be combination of following values:
 *                          @arg TSC_INT_AC
 *                          @arg TSC_INT_MCE
 *
 * @retval      None
 */
void TSC_EnableInterrupt(uint8_t interrupt)
{
    TSC->INTEN |= (uint32_t)interrupt;
}

/*!
 * @brief       Disables the specified interrupts
 *
 * @param       interrupt:  Specifies the TSC interrupts sources
 *                          The parameter can be combination of following values:
 *                          @arg TSC_INT_AC
 *                          @arg TSC_INT_MCE
 *
 * @retval      None
 */
void TSC_DisableInterrupt(uint8_t interrupt)
{
    TSC->INTEN &= ~(uint32_t)interrupt;
}

/*!
 * @brief       Reads the Specifies TSC flag
 *
 * @param       flag:   Specifies the flag to check
 *                      This parameter can be one of the following values:
 *                          @arg TSC_FLAG_AC
 *                          @arg TSC_FLAG_MCE
 *
 * @retval      The new state of flag (SET or RESET)
 */
uint8_t TSC_ReadStatusFlag(TSC_FLAG_T flag)
{
    uint32_t status;
    
    status = TSC->INTSTS;
    
    if ((status & flag) != (uint32_t)RESET)
    {
        return SET;
    }

    return RESET;
}

/*!
 * @brief       Clear the specified TSC flag.
 *
 * @param       flag:   Specifies the flag to check
 *                      This parameter can be any combination of the following values:
 *                          @arg TSC_FLAG_AC
 *                          @arg TSC_FLAG_MCE
 *
 * @retval      None
 */
void TSC_ClearStatusFlag(uint32_t flag)
{
    TSC->INTFCLR = (uint32_t)flag;
}

/*!
 * @brief       Reads the Specifies TSC interrupt flag.
 *
 * @param       flag:   Specifies the flag to check
 *                      This parameter can be one of the following values:
 *                          @arg TSC_INT_FLAG_AC
 *                          @arg TSC_INT_FLAG_MCE
 *
 * @retval      The new state of flag (SET or RESET)
 */
uint8_t TSC_ReadIntFlag(TSC_INT_FLAG_T flag)
{
    uint8_t intEnable;
    uint8_t intStatus;

    intEnable = (uint8_t)(TSC->INTEN& (uint32_t)flag);

    intStatus = (uint8_t)(TSC->INTSTS & (uint32_t)(flag & 0xff));

    if (intEnable && intStatus)
    {
        return SET;
    }

    return RESET;
}

/*!
 * @brief       Clears the Specifies TSC interrupt flag.
 *
 * @param       flag:   Specifies the TSC interrupt pending bit to check
 *                      The parameter can be any combination of following values:
 *                          @arg TSC_INT_FLAG_AC
 *                          @arg TSC_INT_FLAG_MCE
 *
 * @retval      None
 */
void TSC_ClearIntFlag(uint32_t flag)
{
    TSC->INTFCLR = (uint32_t)flag;
}

/**@} end of group TSC_Fuctions*/
/**@} end of group TSC_Driver*/
/**@} end of group Peripherals_Library*/
