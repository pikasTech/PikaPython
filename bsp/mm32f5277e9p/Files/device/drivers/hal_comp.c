/*
 * Copyright 2021 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hal_comp.h"

volatile uint32_t * COMP_GetCSR(COMP_Type * COMPx, uint32_t channel)
{
    volatile uint32_t * inten_index[] =
    {
        &COMPx->CSR[0],
        &COMPx->CSR[0],
        &COMPx->CSR3,
    };
    return inten_index[channel];
}

void COMP_Init(COMP_Type * COMPx, uint32_t channel, COMP_Init_Type * init)
{
    if ( channel < COMP_CHANNEL_NUM )
    {
        * COMP_GetCSR(COMPx, channel) = COMP_CSR_OUTANASEL(1u)
                                      | COMP_CSR_WE(0u)
                                      | COMP_CSR_OFLT(0u)
                                      | COMP_CSR_HYST(init->Hysteresis)
                                      | COMP_CSR_POL(0u)
                                      | COMP_CSR_OUTSEL(init->OutMux)
                                      | COMP_CSR_INPSEL(init->PosInMux)
                                      | COMP_CSR_INMSEL(init->InvInMux)
                                      | COMP_CSR_MODE(init->Speed)
                                      | COMP_CSR_WE(0u)
                                      ;
    }
}

void COMP_Enable(COMP_Type * COMPx, uint32_t channel, bool enable)
{
    if ( channel < COMP_CHANNEL_NUM )
    {
        if ( true == enable )
        {
            * COMP_GetCSR(COMPx, channel) |=  COMP_CSR_EN_MASK;
        }
        else
        {
            * COMP_GetCSR(COMPx, channel) &= ~COMP_CSR_EN_MASK;
        }
    }
}

void COMP_Lock(COMP_Type * COMPx, uint32_t channel)
{
    if ( channel < COMP_CHANNEL_NUM )
    {
        * COMP_GetCSR(COMPx, channel) |= COMP_CSR_LOCK_MASK;
    }
}

bool COMP_GetOutputStatus(COMP_Type * COMPx, uint32_t channel)
{
    if ( channel < COMP_CHANNEL_NUM )
    {
        if ( 0u != ( COMP_CSR_OUT_MASK & (* COMP_GetCSR(COMPx, channel) ) ) )
        {
            return true;
        }
        else
        {
            return false; /* normal input voltage lower than inverting input. */
        }
    }
    else
    {
        return false;
    }
}

void COMP_EnableExtVrefConf(COMP_Type * COMPx, COMP_ExtVrefConf_Type * conf)
{
    if ( NULL == conf ) /* disable the ext vref. */
    {
        COMPx->CRV &= ~COMP_CRV_CRVEN_MASK;
    }
    else /* init & enable ext vref. */
    {
        COMPx->CRV = COMP_CRV_CRVSRC(conf->VrefSource)
                   | COMP_CRV_CRVSEL(conf->Volt)
                   | COMP_CRV_CRVEN_MASK
                   ;
    }
}

void COMP_EnableRoundRobinConf(COMP_Type * COMPx, uint32_t channel, COMP_RoundRobinConf_Type * conf)
{
    if ( channel < COMP_CHANNEL_NUM )
    {
        if ( NULL == conf )
        {
            COMPx->POLL[channel] &= ~COMP_POLL_POLLEN_MASK;
        }
        else
        {
            COMPx->POLL[channel] = COMP_POLL_PERIOD(conf->Period)
                                 | COMP_POLL_FIXN(conf->InvInFix)
                                 | COMP_POLL_POLLCH(conf->ChnGroup)
                                 | COMP_POLL_POLLEN_MASK;
                                 ;
        }
    }
}

bool COMP_GetRoundRobinOutStatus(COMP_Type * COMPx, uint32_t channel, COMP_InMux_Type pos_in)
{
    if ( channel < COMP_CHANNEL_NUM )
    {
        uint32_t flag = COMP_POLL_POUT(1 << ( (uint32_t)pos_in - 1 ) );
        if ( 0 != (COMPx->POLL[channel] & flag ) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

void COMP_EnableWindow(COMP_Type * COMPx, uint32_t channel, bool enable)
{
    if (true == enable)
    {
        * COMP_GetCSR(COMPx, channel) |=  COMP_CSR_WE_MASK;
    }
    else
    {
        * COMP_GetCSR(COMPx, channel) &= ~COMP_CSR_WE_MASK;
    }
}

void COMP_SetOutFilter(COMP_Type * COMPx, uint32_t channel, COMP_OutFilter_Type filter)
{
    * COMP_GetCSR(COMPx, channel) = (COMPx->CSR[channel] & ~COMP_CSR_OFLT_MASK) | COMP_CSR_OFLT(filter);
}

void COMP_EnableOutInvert(COMP_Type * COMPx, uint32_t channel, bool invert)
{
    if(true == invert)
    {
        * COMP_GetCSR(COMPx, channel) = (COMPx->CSR[channel] & ~COMP_CSR_POL_MASK) | COMP_CSR_POL(1u);
    }
    else
    {
        * COMP_GetCSR(COMPx, channel) = (COMPx->CSR[channel] & ~COMP_CSR_POL_MASK) | COMP_CSR_POL(0u);
    }
}

/* EOF. */
