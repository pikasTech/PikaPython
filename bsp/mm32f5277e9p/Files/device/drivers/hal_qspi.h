/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_QSPI_H__
#define __HAL_QSPI_H__

#include "hal_common.h"

/*!
 * @addtogroup QSPI
 * @{
 */

/*!
 * @brief QSPI driver version number.
 */
#define QSPI_DRIVER_VERSION 0u /*!< QSPI_0. */

/*!
 * @addtogroup QSPI_STATUS
 * @{
 */
#define QSPI_STATUS_XIP_ENABLE          QSPI_SR_XIPST_MASK          /*!< Status flag when QSPI XIP enabled. */
#define QSPI_STATUS_FIFO_FULL           QSPI_SR_FULL_MASK           /*!< Status flag when QSPI FIFO full. */
#define QSPI_STATUS_FIFO_EMPTY          QSPI_SR_EMPTY_MASK          /*!< Status flag when QSPI FIFO empty. */
#define QSPI_STATUS_BUSY                QSPI_SR_BUSY_MASK           /*!< Status flag when QSPI xfer data. */
#define QSPI_STATUS_XFER_DONE           QSPI_SR_TCF_MASK            /*!< Status flag when QSPI xfer done. */
#define QSPI_STATUS_OPMODE_CHANGE_DONE  QSPI_SR_OPCRCF_MASK         /*!< Status flag when QSPI change the operation mode done. */
/*!
 * @}
 */

/*!
 * @addtogroup QSPI_INT
 * @{
 */
#define QSPI_INT_FIFO_FULL              QSPI_IDER_FULLINTEN_MASK    /*!< Interrupt enable when QSPI FIFO is full. */
#define QSPI_INT_FIFO_EMPTY             QSPI_IDER_EMPTYINTEN_MASK   /*!< Interrupt enable when QSPI FIFO is empty. */
#define QSPI_INT_XFER_DONE              QSPI_IDER_TCFINTEN_MASK     /*!< Interrupt enable when QSPI xfer done. */
/*!
 * @}
 */

/*!
 * @addtogroup QSPI_DMA
 * @{
 */
#define QSPI_DMA_FIFO_FULL              QSPI_IDER_FULLDMAEN_MASK    /*!< DMA request enable when QSPI FIFO is full. */
#define QSPI_DMA_FIFO_EMPTY             QSPI_IDER_EMPTYDMAEN_MASK   /*!< DMA request enable when QSPI FIFO is empty. */
#define QSPI_DMA_XFER_DONE              QSPI_IDER_TCFDMAEN_MASK     /*!< DMA request enable when QSPI xfer done. */
/*!
 * @}
 */

/*!
 * @brief The QSPI SCK Prescaler.
 */
typedef enum
{
    QSPI_SckPrescaler_Div2   = 0x00u, /*!< SCK output clock = AHB_CLK / 2. */
    QSPI_SckPrescaler_Div4   = 0x01u, /*!< SCK output clock = AHB_CLK / 4. */
    QSPI_SckPrescaler_Div6   = 0x02u, /*!< SCK output clock = AHB_CLK / 6. */
    QSPI_SckPrescaler_Div8   = 0x03u, /*!< SCK output clock = AHB_CLK / 8. */
    QSPI_SckPrescaler_Div10  = 0x04u, /*!< SCK output clock = AHB_CLK / 0. */
    QSPI_SckPrescaler_Div12  = 0x05u, /*!< SCK output clock = AHB_CLK / 12. */
    QSPI_SckPrescaler_Div14  = 0x06u, /*!< SCK output clock = AHB_CLK / 14. */
    QSPI_SckPrescaler_Div16  = 0x07u, /*!< SCK output clock = AHB_CLK / 16. */
    QSPI_SckPrescaler_Div18  = 0x08u, /*!< SCK output clock = AHB_CLK / 18. */
    QSPI_SckPrescaler_Div20  = 0x09u, /*!< SCK output clock = AHB_CLK / 20. */
    QSPI_SckPrescaler_Div22  = 0x0Au, /*!< SCK output clock = AHB_CLK / 22. */
    QSPI_SckPrescaler_Div24  = 0x0Bu, /*!< SCK output clock = AHB_CLK / 24. */
    QSPI_SckPrescaler_Div26  = 0x0Cu, /*!< SCK output clock = AHB_CLK / 26. */
    QSPI_SckPrescaler_Div28  = 0x0Du, /*!< SCK output clock = AHB_CLK / 28. */
    QSPI_SckPrescaler_Div30  = 0x0Eu, /*!< SCK output clock = AHB_CLK / 30. */
    QSPI_SckPrescaler_Div32  = 0x0Fu, /*!< SCK output clock = AHB_CLK / 32. */
    QSPI_SckPrescaler_Div34  = 0x10u, /*!< SCK output clock = AHB_CLK / 34. */
    QSPI_SckPrescaler_Div36  = 0x11u, /*!< SCK output clock = AHB_CLK / 36. */
    QSPI_SckPrescaler_Div38  = 0x12u, /*!< SCK output clock = AHB_CLK / 38. */
    QSPI_SckPrescaler_Div40  = 0x13u, /*!< SCK output clock = AHB_CLK / 40. */
    QSPI_SckPrescaler_Div42  = 0x14u, /*!< SCK output clock = AHB_CLK / 42. */
    QSPI_SckPrescaler_Div44  = 0x15u, /*!< SCK output clock = AHB_CLK / 44. */
    QSPI_SckPrescaler_Div46  = 0x16u, /*!< SCK output clock = AHB_CLK / 46. */
    QSPI_SckPrescaler_Div48  = 0x17u, /*!< SCK output clock = AHB_CLK / 48. */
    QSPI_SckPrescaler_Div50  = 0x18u, /*!< SCK output clock = AHB_CLK / 50. */
    QSPI_SckPrescaler_Div52  = 0x19u, /*!< SCK output clock = AHB_CLK / 52. */
    QSPI_SckPrescaler_Div54  = 0x1Au, /*!< SCK output clock = AHB_CLK / 54. */
    QSPI_SckPrescaler_Div56  = 0x1Bu, /*!< SCK output clock = AHB_CLK / 56. */
    QSPI_SckPrescaler_Div58  = 0x1Cu, /*!< SCK output clock = AHB_CLK / 58. */
    QSPI_SckPrescaler_Div60  = 0x1Du, /*!< SCK output clock = AHB_CLK / 60. */
    QSPI_SckPrescaler_Div62  = 0x1Eu, /*!< SCK output clock = AHB_CLK / 62. */
    QSPI_SckPrescaler_Div64  = 0x1Fu, /*!< SCK output clock = AHB_CLK / 64. */
    QSPI_SckPrescaler_Div66  = 0x20u, /*!< SCK output clock = AHB_CLK / 66. */
    QSPI_SckPrescaler_Div68  = 0x21u, /*!< SCK output clock = AHB_CLK / 68. */
    QSPI_SckPrescaler_Div70  = 0x22u, /*!< SCK output clock = AHB_CLK / 70. */
    QSPI_SckPrescaler_Div72  = 0x23u, /*!< SCK output clock = AHB_CLK / 72. */
    QSPI_SckPrescaler_Div74  = 0x24u, /*!< SCK output clock = AHB_CLK / 74. */
    QSPI_SckPrescaler_Div76  = 0x25u, /*!< SCK output clock = AHB_CLK / 76. */
    QSPI_SckPrescaler_Div78  = 0x26u, /*!< SCK output clock = AHB_CLK / 78. */
    QSPI_SckPrescaler_Div80  = 0x27u, /*!< SCK output clock = AHB_CLK / 80. */
    QSPI_SckPrescaler_Div82  = 0x28u, /*!< SCK output clock = AHB_CLK / 82. */
    QSPI_SckPrescaler_Div84  = 0x29u, /*!< SCK output clock = AHB_CLK / 84. */
    QSPI_SckPrescaler_Div86  = 0x2Au, /*!< SCK output clock = AHB_CLK / 86. */
    QSPI_SckPrescaler_Div88  = 0x2Bu, /*!< SCK output clock = AHB_CLK / 88. */
    QSPI_SckPrescaler_Div90  = 0x2Cu, /*!< SCK output clock = AHB_CLK / 90. */
    QSPI_SckPrescaler_Div92  = 0x2Du, /*!< SCK output clock = AHB_CLK / 92. */
    QSPI_SckPrescaler_Div94  = 0x2Eu, /*!< SCK output clock = AHB_CLK / 94. */
    QSPI_SckPrescaler_Div96  = 0x2Fu, /*!< SCK output clock = AHB_CLK / 96. */
    QSPI_SckPrescaler_Div98  = 0x30u, /*!< SCK output clock = AHB_CLK / 98. */
    QSPI_SckPrescaler_Div100 = 0x31u, /*!< SCK output clock = AHB_CLK / 100. */
    QSPI_SckPrescaler_Div102 = 0x32u, /*!< SCK output clock = AHB_CLK / 102. */
    QSPI_SckPrescaler_Div104 = 0x33u, /*!< SCK output clock = AHB_CLK / 104. */
    QSPI_SckPrescaler_Div106 = 0x34u, /*!< SCK output clock = AHB_CLK / 106. */
    QSPI_SckPrescaler_Div108 = 0x35u, /*!< SCK output clock = AHB_CLK / 108. */
    QSPI_SckPrescaler_Div110 = 0x36u, /*!< SCK output clock = AHB_CLK / 110. */
    QSPI_SckPrescaler_Div112 = 0x37u, /*!< SCK output clock = AHB_CLK / 112. */
    QSPI_SckPrescaler_Div114 = 0x38u, /*!< SCK output clock = AHB_CLK / 114. */
    QSPI_SckPrescaler_Div116 = 0x39u, /*!< SCK output clock = AHB_CLK / 116. */
    QSPI_SckPrescaler_Div118 = 0x3Au, /*!< SCK output clock = AHB_CLK / 118. */
    QSPI_SckPrescaler_Div120 = 0x3Bu, /*!< SCK output clock = AHB_CLK / 120. */
    QSPI_SckPrescaler_Div122 = 0x3Cu, /*!< SCK output clock = AHB_CLK / 122. */
    QSPI_SckPrescaler_Div124 = 0x3Du, /*!< SCK output clock = AHB_CLK / 124. */
    QSPI_SckPrescaler_Div126 = 0x3Eu, /*!< SCK output clock = AHB_CLK / 126. */
    QSPI_SckPrescaler_Div128 = 0x3Fu, /*!< SCK output clock = AHB_CLK / 128. */
} QSPI_SckPrescaler_Type;

/*!
 * @brief The cycle for CS pin to keep high level between two effective levels.
 */
typedef enum
{
    QSPI_CsHighLevelCycle_2 = 0x00u, /*!< Keep 2 AHB_CLK cycles. */
    QSPI_CsHighLevelCycle_3 = 0x01u, /*!< Keep 3 AHB_CLK cycles. */
    QSPI_CsHighLevelCycle_4 = 0x02u, /*!< Keep 4 AHB_CLK cycles. */
    QSPI_CsHighLevelCycle_5 = 0x03u, /*!< Keep 5 AHB_CLK cycles. */
    QSPI_CsHighLevelCycle_6 = 0x04u, /*!< Keep 6 AHB_CLK cycles. */
    QSPI_CsHighLevelCycle_7 = 0x05u, /*!< Keep 7 AHB_CLK cycles. */
    QSPI_CsHighLevelCycle_8 = 0x06u, /*!< Keep 8 AHB_CLK cycles. */
    QSPI_CsHighLevelCycle_9 = 0x07u, /*!< Keep 9 AHB_CLK cycles. */
} QSPI_CsHighLevelCycle_Type;

/*!
 * @brief SPI mode, set the clock polarity(CPOL) & clock phase(CPHA).
 */
typedef enum
{
    QSPI_SpiMode_0 = 0x00u, /*!< SPI mode 0, CPOL = 0, CPHA = 0. */
    QSPI_SpiMode_3 = 0x01u, /*!< SPI mode 0, CPOL = 1, CPHA = 1. */
} QSPI_SpiMode_Type;

/*!
 * @brief Operation mode.
 */
typedef enum
{
    QSPI_OpMode_Direct   = 0x00u, /*!< Direct mode, read data from memory addr. */
    QSPI_OpMode_Indirect = 0x02u, /*!< Indirect mode, read & write data by QSPI peripheral. */
    QSPI_OpMode_Idle     = 0x03u, /*!< Idle mode, cannot read data by direct mode. */
} QSPI_OpMode_Type;

/*!
 * @brief Xfer direction.
 */
typedef enum
{
    QSPI_XferDirection_Rx = 0x00u, /*!< Xfer data from mcu to external flash. */
    QSPI_XferDirection_Tx = 0x01u, /*!< Xfer data from external flash to mcu. */
} QSPI_XferDirection_Type;

/*!
 * @brief Word width, defines how many bits the word width.
 */
typedef enum
{
    QSPI_WordWidth_8b  = 0x00u, /*!< The word wide is 8 bit. */
    QSPI_WordWidth_16b = 0x01u, /*!< The word wide is 16 bit. */
    QSPI_WordWidth_24b = 0x02u, /*!< The word wide is 24 bit. */
    QSPI_WordWidth_32b = 0x03u, /*!< The word wide is 32 bit. */
} QSPI_WordWidth_Type;

/*!
 * @brief Line mode, defines how many lines to xfer data.
 */
typedef enum
{
    QSPI_LineMode_None   = 0x00u, /*!< Skip this phase. */
    QSPI_LineMode_Single = 0x01u, /*!< Xfer data by signal line. (IO0) */
    QSPI_LineMode_Dual   = 0x02u, /*!< Xfer data by dual lines.  (IO0, IO1) */
    QSPI_LineMode_Quad   = 0x03u, /*!< Xfer data by quad lines.  (IO0, IO1, IO2, IO3)*/
} QSPI_LineMode_Type;

/*!
 * @brief Dummy Cycle.
 */
typedef enum
{
    QSPI_DummyCycle_0  = 0x00u, /*!< Keep  0 SCK output clock. */
    QSPI_DummyCycle_1  = 0x01u, /*!< Keep  1 SCK output clock. */
    QSPI_DummyCycle_2  = 0x02u, /*!< Keep  2 SCK output clock. */
    QSPI_DummyCycle_3  = 0x03u, /*!< Keep  3 SCK output clock. */
    QSPI_DummyCycle_4  = 0x04u, /*!< Keep  4 SCK output clock. */
    QSPI_DummyCycle_5  = 0x05u, /*!< Keep  5 SCK output clock. */
    QSPI_DummyCycle_6  = 0x06u, /*!< Keep  6 SCK output clock. */
    QSPI_DummyCycle_7  = 0x07u, /*!< Keep  7 SCK output clock. */
    QSPI_DummyCycle_8  = 0x08u, /*!< Keep  8 SCK output clock. */
    QSPI_DummyCycle_9  = 0x09u, /*!< Keep  9 SCK output clock. */
    QSPI_DummyCycle_10 = 0x0Au, /*!< Keep 10 SCK output clock. */
    QSPI_DummyCycle_11 = 0x0Bu, /*!< Keep 11 SCK output clock. */
    QSPI_DummyCycle_12 = 0x0Cu, /*!< Keep 12 SCK output clock. */
    QSPI_DummyCycle_13 = 0x0Du, /*!< Keep 13 SCK output clock. */
    QSPI_DummyCycle_14 = 0x0Eu, /*!< Keep 14 SCK output clock. */
    QSPI_DummyCycle_15 = 0x0Fu, /*!< Keep 15 SCK output clock. */
    QSPI_DummyCycle_16 = 0x10u, /*!< Keep 16 SCK output clock. */
    QSPI_DummyCycle_17 = 0x11u, /*!< Keep 17 SCK output clock. */
    QSPI_DummyCycle_18 = 0x12u, /*!< Keep 18 SCK output clock. */
    QSPI_DummyCycle_19 = 0x13u, /*!< Keep 19 SCK output clock. */
    QSPI_DummyCycle_20 = 0x14u, /*!< Keep 20 SCK output clock. */
    QSPI_DummyCycle_21 = 0x15u, /*!< Keep 21 SCK output clock. */
    QSPI_DummyCycle_22 = 0x16u, /*!< Keep 22 SCK output clock. */
    QSPI_DummyCycle_23 = 0x17u, /*!< Keep 23 SCK output clock. */
    QSPI_DummyCycle_24 = 0x18u, /*!< Keep 24 SCK output clock. */
    QSPI_DummyCycle_25 = 0x19u, /*!< Keep 25 SCK output clock. */
    QSPI_DummyCycle_26 = 0x1Au, /*!< Keep 26 SCK output clock. */
    QSPI_DummyCycle_27 = 0x1Bu, /*!< Keep 27 SCK output clock. */
    QSPI_DummyCycle_28 = 0x1Cu, /*!< Keep 28 SCK output clock. */
    QSPI_DummyCycle_29 = 0x1Du, /*!< Keep 29 SCK output clock. */
    QSPI_DummyCycle_30 = 0x1Eu, /*!< Keep 30 SCK output clock. */
    QSPI_DummyCycle_31 = 0x1Fu, /*!< Keep 31 SCK output clock. */
} QSPI_DummyCycle_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref QSPI_Init() to initialize the QSPI module.
 */
typedef struct
{
    QSPI_SckPrescaler_Type      SckPrescaler;     /*!< Specify the QSPI SCK Prescaler. */
    QSPI_CsHighLevelCycle_Type  CsHighLevelCycle; /*!< Specify the QSPI CS High Level Cycle. */
    QSPI_SpiMode_Type           SpiMode;          /*!< Specify the SPI mode. */
} QSPI_Init_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref QSPI_EnableDirectXferConf() to enable direct mode.
 */
typedef struct
{
    QSPI_LineMode_Type      CmdLineMode;    /*!< Specify the QSPI instruction line mode. */
    uint32_t                CmdValue;       /*!< Specify the QSPI instruction value. */
    QSPI_WordWidth_Type     AddrWidth;      /*!< Specify the QSPI addr bit width. */
    QSPI_LineMode_Type      AddrLineMode;   /*!< Specify the QSPI addr line mode. */
    QSPI_WordWidth_Type     AltWidth;       /*!< Specify the QSPI alt bit width. */
    QSPI_LineMode_Type      AltLineMode;    /*!< Specify the QSPI alt line mode. */
    uint32_t                AltValue;       /*!< Specify the QSPI alt value. */
    QSPI_DummyCycle_Type    DummyCycles;    /*!< Specify the QSPI dummy cycles. */
    QSPI_LineMode_Type      DataLineMode;   /*!< Specify the QSPI data line mode. */
} QSPI_DirectXferConf_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref QSPI_EnableIndirectXferConf() to enable xfer.
 */
typedef struct
{
    bool                    AutoEnableXIP;  /*!< Specify the QSPI enbale XIP mode after quit Indirect mode. */
    QSPI_XferDirection_Type XferDirection;  /*!< Specify the QSPI xfer direction. */
    QSPI_LineMode_Type      CmdLineMode;    /*!< Specify the QSPI instruction line mode. */
    uint32_t                CmdValue;       /*!< Specify the QSPI instruction value. */
    QSPI_WordWidth_Type     AddrWidth;      /*!< Specify the QSPI addr bit width. */
    QSPI_LineMode_Type      AddrLineMode;   /*!< Specify the QSPI addr line mode. */
    uint32_t                AddrValue;      /*!< Specify the QSPI addr value. */
    QSPI_WordWidth_Type     AltWidth;       /*!< Specify the QSPI alt bit width. */
    QSPI_LineMode_Type      AltLineMode;    /*!< Specify the QSPI alt line mode. */
    uint32_t                AltValue;       /*!< Specify the QSPI alt value. */
    QSPI_DummyCycle_Type    DummyCycles;    /*!< Specify the QSPI dummy cycles. */
    QSPI_WordWidth_Type     DataWidth;      /*!< Specify the QSPI data bit width. */
    QSPI_LineMode_Type      DataLineMode;   /*!< Specify the QSPI data line mode. */
    uint32_t                DataLength;     /*!< Specify the QSPI data data length. */
} QSPI_IndirectXferConf_Type;

/*!
 * @brief Initialize the QSPI module.
 *
 * @param QSPIx QSPI instance.
 * @param init  Pointer to the initialization structure. See to @ref QSPI_Init_Type.
 * @return None.
 */
void QSPI_Init(QSPI_Type * QSPIx, QSPI_Init_Type * init);

/*!
 * @brief Get the current status flags of the QSPI module.
 *
 * @param QSPIx QSPI instance.
 * @return Status flags. See to @ref QSPI_STATUS.
 */
uint32_t QSPI_GetStatus(QSPI_Type * QSPIx);

/*!
 * @brief Clear the interrupts status flags of the QSPI module.
 *
 * @param QSPIx QSPI instance.
 * @param status The mask codes of the indicated status flags to be cleared.
 * @return Status flags. See to @ref QSPI_INT.
 */
void QSPI_ClearStatus(QSPI_Type * QSPIx, uint32_t status);

/*!
 * @brief Gets the current operating mode.
 *
 * @param QSPIx QSPI instance.
 * @return Operation, See to @ref QSPI_OpMode_Type.
 */
uint32_t QSPI_GetOpMode(QSPI_Type * QSPIx);

/*!
 * @brief Enable the QSPI direct mode configure.
 *
 * @param QSPIx QSPI instance.
 * @param conf Pointer to the initialization structure. See to @ref QSPI_DirectXferConf_Type.
 * @return None.
 */
void QSPI_EnableDirectXferConf(QSPI_Type * QSPIx, QSPI_DirectXferConf_Type * conf);

/*!
 * @brief Enable the QSPI XIP mode.
 *
 * @param QSPIx QSPI instance.
 * @param enable 'true' to enable XIP, 'false' to disable XIP.
 * @return None.
 */
void QSPI_EnableXIP(QSPI_Type * QSPIx, bool enable);

/*!
 * @brief Set the QSPI indirect xfer.
 *
 * @param QSPIx QSPI instance.
 * @param conf Pointer to the config structure. See to @ref QSPI_IndirectXferConf_Type.
 * @return None.
 */
void QSPI_SetIndirectXferConf(QSPI_Type * QSPIx, QSPI_IndirectXferConf_Type * conf);

/*!
 * @brief Put the data into FIFO of the QSPI module.
 *
 * @param QSPIx QSPI instance.
 * @param value Data value to be send through the FIFO.
 * @return None.
 */
void QSPI_PutDataIndirect(QSPI_Type * QSPIx, uint32_t value);

/*!
 * @brief Get the data from FIFO of the QSPI module.
 *
 * @param QSPIx QSPI instance.
 * @return The data value received from the FIFO.
 */
uint32_t QSPI_GetDataIndirect(QSPI_Type * QSPIx);

/*!
 * @brief Get the data from memory addr as 8 bit.
 *
 * @param QSPIx QSPI instance.
 * @return The data value received from the FIFO.
 */
uint8_t QSPI_GetDataDirect8(QSPI_Type * QSPIx, uint32_t addr);

/*!
 * @brief Get the data from memory addr as 16 bit.
 *
 * @param QSPIx QSPI instance.
 * @return The data value received from the FIFO.
 */
uint16_t QSPI_GetDataDirect16(QSPI_Type * QSPIx, uint32_t addr);

/*!
 * @brief Get the data from memory addr as 32 bit.
 *
 * @param QSPIx QSPI instance.
 * @return The data value received from the FIFO.
 */
uint32_t QSPI_GetDataDirect32(QSPI_Type * QSPIx, uint32_t addr);

/*!
 * @brief Enable interrupts of the QSPI module.
 *
 * @param QSPIx QSPI instance.
 * @param interrupts Interrupt code masks. See to @ref QSPI_INT.
 * @param enable 'true' to enable the indicated interrupts, 'false' to disable the indicated interrupts.
 * @return None.
 */
void QSPI_EnableInterrupts(QSPI_Type * QSPIx, uint32_t interrupts, bool enable);

/*!
 * @brief Enable DMA request of the QSPI module.
 *
 * @param QSPIx QSPI instance.
 * @param dmas dma request code masks. See to @ref QSPI_DMA.
 * @param enable 'true' to enable the indicated request, 'false' to disable the indicated request.
 * @return None.
 */
void QSPI_EnableDMA(QSPI_Type * QSPIx, uint32_t dmas, bool enable);

/*!
 *@}
 */

#endif /* __HAL_QSPI_H__ */
