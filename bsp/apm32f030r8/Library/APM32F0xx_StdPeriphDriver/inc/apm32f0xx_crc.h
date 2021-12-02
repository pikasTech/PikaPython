/*!
 * @file        apm32f0xx_crc.h
 *
 * @brief       This file contains all the functions prototypes for the CRC firmware library
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */
 
#ifndef __CRC_H
#define __CRC_H

#include "apm32f0xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup CRC_Driver CRC Driver
  @{
*/

/** @addtogroup CRC_Enumerations Enumerations
  @{
*/

/**
 * @brief   CRC Reverse Input Data
 */
typedef enum
{
    CRC_REVERSE_INPUT_DATA_NO   = ((uint8_t)0x00),
    CRC_REVERSE_INPUT_DATA_8B   = ((uint8_t)0x01),
    CRC_REVERSE_INPUT_DATA_16B  = ((uint8_t)0x02),
    CRC_REVERSE_INPUT_DATA_32B  = ((uint8_t)0x03),
} CRC_REVERSE_INPUT_DATA_T;

/**
 * @brief   CRC Polynomial Size
 */
typedef enum
{
    CRC_POLYNOMIAL_SIZE_7   = ((uint8_t)0x03),
    CRC_POLYNOMIAL_SIZE_8   = ((uint8_t)0x02),
    CRC_POLYNOMIAL_SIZE_16  = ((uint8_t)0x01),
    CRC_POLYNOMIAL_SIZE_32  = ((uint8_t)0x00),
} CRC_POLYNOMIAL_SIZE_T;

/**@} end of group CRC_Enumerations*/

/** Reset CRC */
void CRC_Reset(void);

/** Reset DATA */
void CRC_ResetDATA(void);

/** CRC Polynomial Size */
void CRC_SetPolynomialSize(CRC_POLYNOMIAL_SIZE_T polynomialSize); //!< Only for APM32F072 and APM32F091 devices
void CRC_SetPolynomialValue(uint32_t polynomialValue); //!< Only for APM32F072 and APM32F091 devices

/** Performed on input data */
void CRC_SelectReverseInputData(CRC_REVERSE_INPUT_DATA_T revInData);

/** Enable and Disable Reverse Output Data */
void CRC_EnableReverseOutputData(void);
void CRC_DisableReverseOutputData(void);

/** Write INITVAL register */
void CRC_WriteInitRegister(uint32_t initValue);

/** Calculate CRC */
uint32_t CRC_CalculateCRC(uint32_t data);
uint32_t CRC_CalculateCRC8bits(uint8_t data);   //!< Only for APM32F072 and APM32F091 devices
uint32_t CRC_CalculateCRC16bits(uint16_t data); //!< Only for APM32F072 and APM32F091 devices
uint32_t CRC_CalculateBlockCRC(uint32_t pBuffer[], uint32_t bufferLength);

/** Read CRC */
uint32_t CRC_ReadCRC(void);

/** Independent Data(ID) */
void CRC_WriteIDRegister(uint8_t IDValue);
uint8_t CRC_ReadIDRegister(void);

/**@} end of group CRC_Fuctions*/
/**@} end of group CRC_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __CRC_H */

