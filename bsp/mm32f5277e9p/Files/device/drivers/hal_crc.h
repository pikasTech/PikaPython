/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_CRC_H__
#define __HAL_CRC_H__

#include "hal_common.h"

/*!
 * @addtogroup CRC
 * @{
 */

/*!
 * @brief CRC polynomail width type.
 */
typedef enum
{
    CRC_PolynomialWidth_8b = 2,   /*!< Polynomial Width 8 bits. */
    CRC_PolynomialWidth_16b = 1,  /*!< Polynomial Width 16 bits. */
    CRC_PolynomialWidth_32b = 0,  /*!< Polynomial Width 32 bits. */
} CRC_PolynomialWidth_Type;

/*!
 * @brief CRC input data processing sequence type.
 */
typedef enum
{
    CRC_InRev_None = 0, /*1< The input data are processed in normal order. */
    CRC_InRev_Rev = 1,  /*!< The input data are processed in bytes in reverse order. */
} CRC_InRev_Type;

/*!
 * @brief CRC output data processing sequence type.
 */
typedef enum
{
    CRC_OutRev_None = 0,  /*!< The output data are processed in normal order. */
    CRC_OutRev_Rev = 1,   /*!< The whole output data is processed in reverse order. */
} CRC_OutRev_Type;

/*!
 * @brief CRC output little/big endian type.
 */
typedef enum
{
    CRC_OutEndian_Little = 0,  /*!< Little-endian output. */
    CRC_OutEndian_Big = 1,     /*!< Big-endian output. */
} CRC_OutEndian_Type;

/*!
 * @brief CRC input little/big endian type.
 */
typedef enum
{
    CRC_InEndian_Little = 0,  /*!< Little-endian input. */
    CRC_InEndian_Big = 1,     /*!< Big-endian input. */
} CRC_InEndian_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref CRC_Init() to initialize the CRC module.
 */
typedef struct
{
    uint32_t                  Polynomial;       /*!< Configure arbitrary polynomial. */
    CRC_PolynomialWidth_Type  PolynomialWidth;  /*!< Specify the CRC polynomial width. */
    CRC_InRev_Type            InRev;            /*!< Specify whether the input data is in reverse order. */
    CRC_OutRev_Type           OutRev;           /*!< Specify whether the output data is in reverse order. */
    CRC_InEndian_Type         InEndian;         /*!< Specify input endian is little endian or big endian. */
    CRC_OutEndian_Type        OutEndian;        /*!< Specify output endian is little endian or big endian. */
} CRC_Init_Type;

/*!
 * @brief Initialize the CRC module.
 *
 * @param CRCx CRC instance.
 * @param init Pointer to the initialization structure. See to @ref CRC_Init_Type.
 * @return None.
 */
void CRC_Init(CRC_Type * CRCx, CRC_Init_Type * init);

/*!
 * @brief Set the value in CRC calculation unit as the initial value.
 *
 * @param CRCx CRC instance.
 * @param val The initial data of CRC.
 * @return None.
 */
void CRC_SetSeed(CRC_Type * CRCx, uint32_t val);

/*!
 * @brief Set data for CRC calculation.
 *
 * @param CRCx CRC instance.
 * @param val CRC calculate value.
 * @return None.
 */
void CRC_SetData(CRC_Type * CRCx, uint32_t val);

/*!
 * @brief Get CRC calculation result.
 *
 * @param CRCx CRC instance.
 * @return Return the last calculation result of CRC.
 */
uint32_t CRC_GetResult(CRC_Type * CRCx);

/*!
 *@}
 */
 
#endif /*__HAL_CRC_H__. */

