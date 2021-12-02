/*!
 * @file      apm32f0xx_crc.c
 *
 * @brief     This file provides all the CRC firmware functions
 *
 * @version   V1.0.1
 *
 * @date      2021-07-01
 *
 */

#include "apm32f0xx_crc.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup CRC_Driver CRC Driver
  @{
*/

/** @addtogroup CRC_Fuctions Fuctions
  @{
*/

/*!
 * @brief     Resets the CRC peripheral registers to their default reset values.
 *
 * @param     None
 *
 * @retval    None
 */
void CRC_Reset(void)
{
    CRC->DATA = 0xFFFFFFFF;
    CRC->INDATA = 0x00;
    CRC->INITVAL = 0xFFFFFFFF;
    CRC->CTRL = 0x00000000;
    CRC->POL = 0x04C11DB7;
}

/*!
 * @brief     Reset CRC data register (DATA)
 *
 * @param     None
 *
 * @retval    None
 */
void CRC_ResetDATA(void)
{
    CRC->CTRL_B.RST = BIT_SET;
}

/*!
 * @brief     Set the CRC polynomial size.
 *
 * @param     polynomialSize: CRC polynomial size
 *                         The parameter can be one of following values:
 *                         @arg CRC_POLYNOMIAL_SIZE_7:   7-bit polynomial for CRC calculation
 *                         @arg CRC_POLYNOMIAL_SIZE_8:   8-bit polynomial for CRC calculation
 *                         @arg CRC_POLYNOMIAL_SIZE_16:  16-bit polynomial for CRC calculation
 *                         @arg CRC_POLYNOMIAL_SIZE_32:  32-bit polynomial for CRC calculation
 *
 * @retval    None
 *
 * @note      It's only for APM32F072 and APM32F091 devices
 */
void CRC_SetPolynomialSize(CRC_POLYNOMIAL_SIZE_T polynomialSize)
{
    CRC->CTRL_B.POLSIZE = polynomialSize;
}

/*!
 * @brief     Set the CRC polynomail coefficients.
 *
 * @param     polynomialValue: Polynomial to be used for CRC calculation
 *
 * @retval    None
 *
 * @note      It's only for APM32F072 and APM32F091 devices
 */
void CRC_SetPolynomialValue(uint32_t polynomialValue)
{
    CRC->POL = polynomialValue;
}

/*!
 * @brief     Selects the reverse operation to be performed on input data
 *
 * @param     revInData:   Reverse input data
 *                         The parameter can be one of following values:
 *                         @arg CRC_REVERSE_INPUT_DATA_NO:   Bit order not affected
 *                         @arg CRC_REVERSE_INPUT_DATA_8B:   Bit reversal done by byte
 *                         @arg CRC_REVERSE_INPUT_DATA_16B:  Bit reversal done by half-word
 *                         @arg CRC_REVERSE_INPUT_DATA_32B:  Bit reversal done by word
 *
 * @retval    None
 */
void CRC_SelectReverseInputData(CRC_REVERSE_INPUT_DATA_T revInData)
{
    CRC->CTRL_B.REVI = revInData;
}

/*!
 * @brief     Enable the reverse operation on output data
 *
 * @param     None
 *
 * @retval    None
 */
void CRC_EnableReverseOutputData(void)
{
    CRC->CTRL_B.REVO = BIT_SET;
}

/*!
 * @brief     Disable the reverse operation on output data
 *
 * @param     None
 *
 * @retval    None
 */
void CRC_DisableReverseOutputData(void)
{
    CRC->CTRL_B.REVO = BIT_RESET;
}

/*!
 * @brief     Initializes the INITVAL register.
 *
 * @param     initValue: Programmable initial CRC value
 *
 * @retval    None
 */
void CRC_WriteInitRegister(uint32_t initValue)
{
    CRC->INITVAL = initValue;
}

/*!
 * @brief     Calculate a 32-bit CRC for a given data word (32 bits)
 *
 * @param     data: data word(32-bit) to compute its CRC
 *
 * @retval    32-bit CRC
 */
uint32_t CRC_CalculateCRC(uint32_t data)
{
    CRC->DATA = data;

    return (CRC->DATA);
}

/*!
 * @brief     Calculate a 32-bit CRC for a given data word (16 bits)
 *
 * @param     data: data word(16-bit) to compute its CRC
 *
 * @retval    16-bit CRC
 *
 * @note      It's only for APM32F072 and APM32F091 devices
 */
uint32_t CRC_CalculateCRC16bits(uint16_t data)
{
  *(uint16_t*)(CRC_BASE) = (uint16_t) data;
  
  return (CRC->DATA);
}

/*!
 * @brief     Calculate a 32-bit CRC for a given data word (8 bits)
 *
 * @param     data: data word(8-bit) to compute its CRC
 *
 * @retval    8-bit CRC
 *
 * @note      It's only for APM32F072 and APM32F091 devices
 */
uint32_t CRC_CalculateCRC8bits(uint8_t data)
{
  *(uint8_t*)(CRC_BASE) = (uint8_t) data;
  
  return (CRC->DATA);
}

/*!
 * @brief     Computes the 32-bit CRC of a given buffer of data word(32-bit)
 *
 * @param     pBuffer: Pointer to the buffer containing the data to be computed
 *
 * @param     bufferLength: buffer length
 *
 * @retval    32-bit CRC
 */
uint32_t CRC_CalculateBlockCRC(uint32_t pBuffer[], uint32_t bufferLength)
{
    uint32_t index = 0;

    for (index = 0; index < bufferLength; index++)
    {
        CRC->DATA = pBuffer[index];
    }

    return (CRC->DATA);
}

/*!
 * @brief     Returns the current CRC value
 *
 * @param     None
 *
 * @retval    32-bit CRC
 */
uint32_t CRC_ReadCRC(void)
{
    return (CRC->DATA);
}

/*!
 * @brief     Stores a 8-bit data in the Independent Data(INDATA) register
 *
 * @param     IDValue: 8-bit value to be stored in the INDATA register
 *
 * @retval    None
 */
void CRC_WriteIDRegister(uint8_t IDValue)
{
    CRC->INDATA = IDValue;
}

/*!
 * @brief      Returns a 8-bit data stored in the Independent Data(INDATA) register
 *
 * @param      None
 *
 * @retval     8-bit value of the INDATA register
 */
uint8_t CRC_ReadIDRegister(void)
{
    return (CRC->INDATA);
}

/**@} end of group CRC_Fuctions*/
/**@} end of group CRC_Driver */
/**@} end of group Peripherals_Library*/
