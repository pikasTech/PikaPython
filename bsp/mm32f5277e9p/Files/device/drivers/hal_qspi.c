/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hal_qspi.h"

void QSPI_Init(QSPI_Type * QSPIx, QSPI_Init_Type * init)
{
    if (NULL == init) /* if the init pointer is null, quit init. */
    {
        return;
    }

    QSPIx->CR = (QSPIx->CR & ~(QSPI_CR_SCKDIV_MASK | QSPI_CR_CSRHT_MASK | QSPI_CR_SCKMODE_MASK | QSPI_CR_OPMODE_MASK) )
              | QSPI_CR_SCKDIV(init->SckPrescaler) /* set the baudrate. */
              | QSPI_CR_CSRHT(init->CsHighLevelCycle) /* set the CS high level cycle. */
              | QSPI_CR_SCKMODE(init->SpiMode) /* set the SPI mode. */
              | QSPI_CR_OPMODE(QSPI_OpMode_Idle); /* set the operation mode, default is idle. */
}

uint32_t QSPI_GetStatus(QSPI_Type * QSPIx)
{
    return QSPIx->SR;
}

void QSPI_ClearStatus(QSPI_Type * QSPIx, uint32_t status)
{
    QSPIx->SR &= status; /* write 1 to clear status. */
}

uint32_t QSPI_GetOpMode(QSPI_Type * QSPIx)
{
    return (QSPIx->SR & QSPI_SR_OPCRCF_MASK) >> QSPI_SR_OPCRCF_SHIFT;
}

void QSPI_EnableDirectXferConf(QSPI_Type * QSPIx, QSPI_DirectXferConf_Type * conf)
{
    if (NULL == conf)
    {
        QSPIx->CR = (QSPIx->CR & ~QSPI_CR_OPMODE_MASK) | QSPI_CR_OPMODE(QSPI_OpMode_Idle);
        return;
    }

    uint32_t rxdly = ( (QSPIx->CR & QSPI_CR_SCKDIV_MASK >> QSPI_CR_SCKDIV_SHIFT) + 1u ) / 2u % 3u;

    QSPIx->DMCR = (QSPIx->DMCR & ~(QSPI_DMCR_NUMDC_MASK  | QSPI_DMCR_DSIZE_MASK  | QSPI_DMCR_DMODE_MASK
                                | QSPI_DMCR_ABMODE_MASK | QSPI_DMCR_ABSIZE_MASK | QSPI_DMCR_ADSIZE_MASK
                                | QSPI_DMCR_ADMODE_MASK | QSPI_DMCR_IMODE_MASK  | QSPI_DMCR_INST_MASK
                                | QSPI_DMCR_RXDLY_MASK) )
                | QSPI_DMCR_RXDLY(rxdly)
                | QSPI_DMCR_NUMDC(conf->DummyCycles)
                | QSPI_DMCR_DSIZE(QSPI_WordWidth_32b)
                | QSPI_DMCR_DMODE(conf->DataLineMode)
                | QSPI_DMCR_ABSIZE(conf->AltWidth)
                | QSPI_DMCR_ABMODE(conf->AltLineMode)
                | QSPI_DMCR_ADSIZE(conf->AddrWidth)
                | QSPI_DMCR_ADMODE(conf->AddrLineMode)
                | QSPI_DMCR_IMODE(conf->CmdLineMode)
                | QSPI_DMCR_INST(conf->CmdValue);

    QSPIx->DABR = QSPI_DABR_DALT(conf->AltValue);

    QSPIx->CR = ( QSPIx->CR & ~( QSPI_CR_OPMODE_MASK | QSPI_CR_XIPMODE_MASK) )
              | QSPI_CR_OPMODE(QSPI_OpMode_Direct);
}

void QSPI_EnableXiP(QSPI_Type * QSPIx, bool enable)
{
    if (true == enable)
    {
        QSPIx->CR = QSPIx->CR | QSPI_CR_XIPMODE_MASK;
    }
    else
    {
        QSPIx->CR = QSPIx->CR & ~QSPI_CR_XIPMODE_MASK;
    }
}

void QSPI_SetIndirectXferConf(QSPI_Type * QSPIx, QSPI_IndirectXferConf_Type * conf)
{
    if(NULL == conf)
    {
        return;
    }

    uint32_t rxdly = ( (QSPIx->CR & QSPI_CR_SCKDIV_MASK >> QSPI_CR_SCKDIV_SHIFT) + 1u ) / 2u % 3u;

    QSPIx->IMCR = ( QSPIx->IMCR & ~(QSPI_IMCR_IDMODE_MASK | QSPI_IMCR_NUMDC_MASK  | QSPI_IMCR_DSIZE_MASK
                                | QSPI_IMCR_DMODE_MASK  | QSPI_IMCR_ABSIZE_MASK | QSPI_IMCR_ABMODE_MASK
                                | QSPI_IMCR_ADSIZE_MASK | QSPI_IMCR_ADMODE_MASK | QSPI_IMCR_IMODE_MASK
                                | QSPI_IMCR_INST_MASK   | QSPI_IMCR_RXDLY_MASK  | QSPI_IMCR_XIPIM_MASK) )
                | QSPI_IMCR_XIPIM(conf->AutoEnableXIP)
                | QSPI_IMCR_RXDLY(rxdly)
                | QSPI_IMCR_IDMODE(conf->XferDirection)
                | QSPI_IMCR_NUMDC(conf->DummyCycles)
                | QSPI_IMCR_DSIZE(conf->DataWidth)
                | QSPI_IMCR_DMODE(conf->DataLineMode)
                | QSPI_IMCR_ABSIZE(conf->AltWidth)
                | QSPI_IMCR_ABMODE(conf->AltLineMode)
                | QSPI_IMCR_ADSIZE(conf->AddrWidth)
                | QSPI_IMCR_ADMODE(conf->AddrLineMode)
                | QSPI_IMCR_IMODE(conf->CmdLineMode)
                | QSPI_IMCR_INST(conf->CmdValue);
    QSPIx->IABR = QSPI_IABR_IALT(conf->AltValue);
    QSPIx->IADR = QSPI_IADR_IADDR(conf->AddrValue);
    QSPIx->IDLR = QSPI_IDLR_IDLEN(conf->DataLength - 1u); /* data len = reg value + 1. */
    QSPIx->IWCR = 0u; /* wait 7 AHB_CLK to quit inrect mode. */
    QSPIx->CR = (QSPIx->CR & ~QSPI_CR_OPMODE_MASK) | QSPI_CR_OPMODE(QSPI_OpMode_Indirect);
}

void QSPI_PutDataIndirect(QSPI_Type * QSPIx, uint32_t value)
{
    QSPIx->IDFR = value;
}

uint32_t QSPI_GetDataIndirect(QSPI_Type * QSPIx)
{
    return QSPIx->IDFR;
}

uint8_t QSPI_GetDataDirect8(QSPI_Type * QSPIx, uint32_t offset)
{
    (void)QSPIx;
    return (*( (uint8_t *)(QSPI_M_BASE + offset) ) );
}

uint16_t QSPI_GetDataDirect16(QSPI_Type * QSPIx, uint32_t offset)
{
    (void)QSPIx;
    return (*( (uint16_t *)(QSPI_M_BASE + offset) ) );
}

uint32_t QSPI_GetDataDirect32(QSPI_Type * QSPIx, uint32_t offset)
{
    (void)QSPIx;
    return (*( (uint32_t *)(QSPI_M_BASE + offset) ) );
}

void QSPI_EnableInterrupts(QSPI_Type * QSPIx, uint32_t interrupts, bool enable)
{
    if(true == enable)
    {
        QSPIx->IDER |=  interrupts;
    }
    else
    {
        QSPIx->IDER &= ~interrupts;
    }
}

void QSPI_EnableDMARequest(QSPI_Type * QSPIx, uint32_t request, bool enable)
{
    if(true == enable)
    {
        QSPIx->IDER |=  request;
    }
    else
    {
        QSPIx->IDER &= ~request;
    }
}

/* EOF. */
