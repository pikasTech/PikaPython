/*!
 * @file        apm32f0xx_tsc.h
 *
 * @brief       This file contains all the functions prototypes for the TSC firmware library
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#ifndef __TSC_H
#define __TSC_H

#include "apm32f0xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup TSC_Driver TSC Driver
  @{
*/

/** @addtogroup TSC_Enumerations Enumerations
  @{
*/

/**
  * @brief TSC state structure definition
  */
typedef enum
{
    TSC_ACQ_FINISH = ((uint8_t)0x00),
    TSC_ACQ_BUSY   = ((uint8_t)0x01), 
    TSC_ACQ_ERROR  = ((uint8_t)0x02), 
} TSC_ACQ_T;

/**
 * @brief    TSC Charge-transfer high pulse length
 */
typedef enum
{
    TSC_CTPH_1CYCLE = ((uint8_t)0x00),
    TSC_CTPH_2CYCLE = ((uint8_t)0x01),
    TSC_CTPH_3CYCLE = ((uint8_t)0x02),
    TSC_CTPH_4CYCLE = ((uint8_t)0x03),
    TSC_CTPH_5CYCLE = ((uint8_t)0x04),
    TSC_CTPH_6CYCLE = ((uint8_t)0x05),
    TSC_CTPH_7CYCLE = ((uint8_t)0x06),
    TSC_CTPH_8CYCLE = ((uint8_t)0x07),
    TSC_CTPH_9CYCLE = ((uint8_t)0x08),
    TSC_CTPH_10CYCLE = ((uint8_t)0x09),
    TSC_CTPH_11CYCLE = ((uint8_t)0x0A),
    TSC_CTPH_12CYCLE = ((uint8_t)0x0B),
    TSC_CTPH_13CYCLE = ((uint8_t)0x0C),
    TSC_CTPH_14CYCLE = ((uint8_t)0x0D),
    TSC_CTPH_15CYCLE = ((uint8_t)0x0E),
    TSC_CTPH_16CYCLE = ((uint8_t)0x0F),
} TSC_CTPH_T;

/**
 * @brief    TSC Charge-transfer Low pulse length
 */
typedef enum
{
    TSC_CTPL_1CYCLE = ((uint8_t)0x00),
    TSC_CTPL_2CYCLE = ((uint8_t)0x01),
    TSC_CTPL_3CYCLE = ((uint8_t)0x02),
    TSC_CTPL_4CYCLE = ((uint8_t)0x03),
    TSC_CTPL_5CYCLE = ((uint8_t)0x04),
    TSC_CTPL_6CYCLE = ((uint8_t)0x05),
    TSC_CTPL_7CYCLE = ((uint8_t)0x06),
    TSC_CTPL_8CYCLE = ((uint8_t)0x07),
    TSC_CTPL_9CYCLE = ((uint8_t)0x08),
    TSC_CTPL_10CYCLE = ((uint8_t)0x09),
    TSC_CTPL_11CYCLE = ((uint8_t)0x0A),
    TSC_CTPL_12CYCLE = ((uint8_t)0x0B),
    TSC_CTPL_13CYCLE = ((uint8_t)0x0C),
    TSC_CTPL_14CYCLE = ((uint8_t)0x0D),
    TSC_CTPL_15CYCLE = ((uint8_t)0x0E),
    TSC_CTPL_16CYCLE = ((uint8_t)0x0F),
} TSC_CTPL_T;

/**
 * @brief    TSC Spread spectrum prescaler
 */
typedef enum
{
    TSC_SS_PRE1 = ((uint8_t)0x00),
    TSC_SS_PRE2 = ((uint8_t)0x01),
} TSC_SSP_T;

/**
 * @brief    TSC Pulse Generator Prescaler
 */
typedef enum
{
    TSC_PG_PRESC1 = ((uint8_t)0x00),
    TSC_PG_PRESC2 = ((uint8_t)0x01),
    TSC_PG_PRESC4 = ((uint8_t)0x02),
    TSC_PG_PRESC8 = ((uint8_t)0x03),
    TSC_PG_PRESC16 = ((uint8_t)0x04),
    TSC_PG_PRESC32 = ((uint8_t)0x05),
    TSC_PG_PRESC64 = ((uint8_t)0x06),
    TSC_PG_PRESC128 = ((uint8_t)0x07),
} TSC_PGP_T;

/**
 * @brief    TSC Max Count Value
 */
typedef enum
{
    TSC_MCE_255  = ((uint8_t)0x00),
    TSC_MCE_511  = ((uint8_t)0x01),
    TSC_MCE_1023 = ((uint8_t)0x02),
    TSC_MCE_2047 = ((uint8_t)0x03),
    TSC_MCE_4095 = ((uint8_t)0x04),
    TSC_MCE_8191 = ((uint8_t)0x05),
    TSC_MCE_16383 = ((uint8_t)0x06),
} TSC_MCE_T;

/**
 * @brief    TSC IO Default Mode
 */
typedef enum
{
    TSC_IODM_OUT_PP_LOW = ((uint8_t)0x00),
    TSC_IODM_IN_FLOAT = ((uint8_t)0x01),
} TSC_IODM_T;

/**
 * @brief    TSC Synchro Pin Polarity
 */
typedef enum
{
    TSC_SYNPPOL_FALLING = ((uint8_t)0x00),
    TSC_SYNPPOL_RISING = ((uint8_t)0x01),
} TSC_SYNPPOL_T;

/**
 * @brief    TSC Acquisition Mode
 */
typedef enum
{
    TSC_ACQMOD_NORMAL = ((uint8_t)0x00),
    TSC_ACQMOD_SYNCHRO = ((uint8_t)0x01),
} TSC_ACQMOD_T;

/**
 * @brief    TSC IO Group
 */
typedef enum
{
    TSC_GROUP_1 = ((uint8_t)0x01),
    TSC_GROUP_2 = ((uint8_t)0x02),
    TSC_GROUP_3 = ((uint8_t)0x03),
    TSC_GROUP_4 = ((uint8_t)0x04),
    TSC_GROUP_5 = ((uint8_t)0x05),
    TSC_GROUP_6 = ((uint8_t)0x06),
} TSC_GROUP_T;

/**
 * @brief    TSC IO channel
 */
typedef enum
{
    TSC_IO_G1P1 = BIT0,
    TSC_IO_G1P2 = BIT1,
    TSC_IO_G1P3 = BIT2,
    TSC_IO_G1P4 = BIT3,
    TSC_IO_G2P1 = BIT4,
    TSC_IO_G2P2 = BIT5,
    TSC_IO_G2P3 = BIT6,
    TSC_IO_G2P4 = BIT7,
    TSC_IO_G3P1 = BIT8,
    TSC_IO_G3P2 = BIT9,
    TSC_IO_G3P3 = BIT10,
    TSC_IO_G3P4 = BIT11,
    TSC_IO_G4P1 = BIT12,
    TSC_IO_G4P2 = BIT13,
    TSC_IO_G4P3 = BIT14,
    TSC_IO_G4P4 = BIT15,
    TSC_IO_G5P1 = BIT16,
    TSC_IO_G5P2 = BIT17,
    TSC_IO_G5P3 = BIT18,
    TSC_IO_G5P4 = BIT19,
    TSC_IO_G6P1 = BIT20,
    TSC_IO_G6P2 = BIT21,
    TSC_IO_G6P3 = BIT22,
    TSC_IO_G6P4 = BIT23,
} TSC_IO_T;

/**
 * @brief  TSC interrupts definition
 */
typedef enum
{
    TSC_INT_NONE = ((uint8_t)0x00),
    TSC_INT_AC   = ((uint8_t)0x01),
    TSC_INT_MCE  = ((uint8_t)0x02),
} TSC_INT_T;

/**
 * @brief   TSC interrupts definition
 */
typedef enum
{
    TSC_FLAG_AC  = ((uint8_t)0x01),
    TSC_FLAG_MCE = ((uint8_t)0x02),
} TSC_FLAG_T;

/**
 * @brief   TSC Interrupt flag
 */
typedef enum
{
    TSC_INT_FLAG_AC  = ((uint8_t)0x01),
    TSC_INT_FLAG_MCE = ((uint8_t)0x02),
} TSC_INT_FLAG_T;

/**@} end of group TSC_Enumerations*/


/** @addtogroup TSC_Structure Data Structure
  @{
*/

/**
 * @brief   TSC Config struct definition
 */
typedef struct
{
    TSC_CTPH_T      CTPHSEL;
    TSC_CTPL_T      CTPLSEL;
    uint8_t         SpreadSpectrumDev;/*!< Spread spectrum deviation
                                This parameter must be a number between Min_Data = 0 and Max_Data = 127 */
    uint8_t         SpreadSpectrum;
    TSC_SSP_T       SpreadSpectrumPre;
    TSC_PGP_T       PulseGeneratorPre;
    TSC_MCE_T       MCountValue;
    TSC_IODM_T      IOMode;
    TSC_SYNPPOL_T   SynchroPinPolarity;
    TSC_ACQMOD_T    AcqMode;
    TSC_INT_T       Interrupts;
} TSC_Config_T;

/**
 * @brief   TSC IOs configuration structure definition
 */
typedef struct
{
    /** These parameters can be a value of @ref TSC_IO_T */
    uint32_t IOChannel;  /** Channel IOs mask */
    uint32_t IOShield;   /** Shield IOs mask */
    uint32_t IOSampling; /** Sampling IOs mask */
} TSC_ConfigIO_T;

/**@} end of group TSC_Structure*/


/** @addtogroup TSC_Fuctions Fuctions
  @{
*/

/** TSC reset and configuration */
void TSC_Reset(void);
void TSC_Config(TSC_Config_T* tscConfig);
void TSC_ConfigStructInit(TSC_Config_T* tscConfig);

void TSC_ConfigIO(TSC_ConfigIO_T* tscConfigIO);

void TSC_Start(void);
void TSC_Stop(void);

void TSC_Enable(void);
void TSC_Disable(void);

uint8_t TSC_PollForAcquisition(void);

/** Read Value */
uint16_t TSC_ReadGroupValue(TSC_GROUP_T group);

void TSC_EnableSpreadSpectrum(void);
void TSC_DisableSpreadSpectrum(void);

/** Interrupt and flag */
void TSC_EnableInterrupt(uint8_t interrupt);
void TSC_DisableInterrupt(uint8_t interrupt);

uint8_t TSC_ReadStatusFlag(TSC_FLAG_T flag);
void TSC_ClearStatusFlag(uint32_t flag);

uint8_t TSC_ReadIntFlag(TSC_INT_FLAG_T flag);
void TSC_ClearIntFlag(uint32_t flag);

/**@} end of group TSC_Fuctions*/
/**@} end of group TSC_Driver*/
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __TSC_H */
