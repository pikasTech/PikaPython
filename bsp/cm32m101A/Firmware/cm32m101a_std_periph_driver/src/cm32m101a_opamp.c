/*
  ******************************************************************************
  *
  * COPYRIGHT(c) 2020, China Mobile IOT
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of China Mobile IOT nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/**
 * @file cm32m101a_opamp.c
 * @author CMIOT
 * @version v1.0.0
 *
 * @COPYRIGHT(c) 2020, China Mobile IOT. All rights reserved.
 */
#include "cm32m101a_opamp.h"
#include "cm32m101a_rcc.h"

/** @addtogroup cm32m101a_StdPeriph_Driver
 * @{
 */

/** @addtogroup OPAMP
 * @brief OPAMP driver modules
 * @{
 */

/** @addtogroup OPAMP_Private_TypesDefinitions
 * @{
 */

/**
 * @}
 */

/** @addtogroup OPAMP_Private_Defines
 * @{
 */

/**
 * @}
 */

/** @addtogroup OPAMP_Private_Macros
 * @{
 */

/**
 * @}
 */

/** @addtogroup OPAMP_Private_Variables
 * @{
 */

/**
 * @}
 */

/** @addtogroup OPAMP_Private_FunctionPrototypes
 * @{
 */

/**
 * @}
 */

/** @addtogroup OPAMP_Private_Functions
 * @{
 */
#define SetBitMsk(reg, bit, msk) ((reg) = ((reg) & ~(msk) | (bit)))
#define ClrBit(reg, bit)         ((reg) &= ~(bit))
#define SetBit(reg, bit)         ((reg) |= (bit))
#define GetBit(reg, bit)         ((reg) & (bit))
/**
 * @brief  Deinitializes the OPAMP peripheral registers to their default reset values.
 */
void OPAMP_DeInit(void)
{
    RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_OPAMP, ENABLE);
    RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_OPAMP, DISABLE);
}
void OPAMP_StructInit(OPAMP_InitType* OPAMP_InitStruct)
{
    OPAMP_InitStruct->Opa2SrcSel        = OPAMP2_CS_TIMSRCSEL_TIM1CC6;
    OPAMP_InitStruct->Gain              = OPAMP_CS_PGA_GAIN_2;
    OPAMP_InitStruct->HighVolRangeEn    = ENABLE;
    OPAMP_InitStruct->TimeAutoMuxEn     = DISABLE;
    OPAMP_InitStruct->Mod               = OPAMP_CS_PGA_EN;
}
void OPAMP_Init(OPAMPX OPAMPx, OPAMP_InitType* OPAMP_InitStruct)
{
    __IO uint32_t* pCs = &OPAMP->CS1 + OPAMPx;
    __IO uint32_t tmp  = *pCs;
    if(OPAMPx == OPAMP2)
        SetBitMsk(tmp, OPAMP_InitStruct->Opa2SrcSel, OPAMP_CS_OPAMP2_TIMSRCSEL);
    SetBitMsk(tmp, OPAMP_InitStruct->Gain, OPAMP_CS_PGA_GAIN_MASK);
		if(OPAMP_InitStruct->HighVolRangeEn==ENABLE)
			SetBitMsk(tmp, OPAMP_CS_RANGE_MASK, OPAMP_CS_RANGE_MASK);
		else
			ClrBit(tmp,OPAMP_CS_RANGE_MASK);
		if(OPAMP_InitStruct->TimeAutoMuxEn==ENABLE)
			SetBitMsk(tmp,OPAMP_CS_TCMEN_MASK, OPAMP_CS_TCMEN_MASK);
		else
			ClrBit(tmp,OPAMP_CS_TCMEN_MASK);
    SetBitMsk(tmp, OPAMP_InitStruct->Mod, OPAMP_CS_MOD_MASK);
    *pCs = tmp;
}
void OPAMP_Enable(OPAMPX OPAMPx, FunctionalState en)
{
    __IO uint32_t* pCs = &OPAMP->CS1 + OPAMPx;
    if (en)
        SetBit(*pCs, OPAMP_CS_EN_MASK);
    else
        ClrBit(*pCs, OPAMP_CS_EN_MASK);
}

void OPAMP_SetPgaGain(OPAMPX OPAMPx, OPAMP_CS_PGA_GAIN Gain)
{
    __IO uint32_t* pCs = &OPAMP->CS1 + OPAMPx;
    __IO uint32_t tmp  = *pCs;
    SetBitMsk(tmp, Gain, OPAMP_CS_PGA_GAIN_MASK);
    *pCs = tmp;
}
void OPAMP_SetVpSecondSel(OPAMPX OPAMPx, OPAMP_CS_VPSSEL VpSSel)
{
    __IO uint32_t* pCs = &OPAMP->CS1 + OPAMPx;
    __IO uint32_t tmp  = *pCs;
    SetBitMsk(tmp, VpSSel, OPAMP_CS_VPSEL_SECOND_MASK);
    *pCs = tmp;
}
void OPAMP_SetVmSecondSel(OPAMPX OPAMPx, OPAMP_CS_VMSSEL VmSSel)
{
    __IO uint32_t* pCs = &OPAMP->CS1 + OPAMPx;
    __IO uint32_t tmp  = *pCs;
    SetBitMsk(tmp, VmSSel, OPAMP_CS_VMSEL_SECOND_MASK);
    *pCs = tmp;
}
void OPAMP_SetVpSel(OPAMPX OPAMPx, OPAMP_CS_VPSEL VpSel)
{
    __IO uint32_t* pCs = &OPAMP->CS1 + OPAMPx;
    __IO uint32_t tmp  = *pCs;
    SetBitMsk(tmp, VpSel, OPAMP_CS_VPSEL_MASK);
    *pCs = tmp;
}
void OPAMP_SetVmSel(OPAMPX OPAMPx, OPAMP_CS_VMSEL VmSel)
{
    __IO uint32_t* pCs = &OPAMP->CS1 + OPAMPx;
    __IO uint32_t tmp  = *pCs;
    SetBitMsk(tmp, VmSel, OPAMP_CS_VMSEL_MASK);
    *pCs = tmp;
}
bool OPAMP_IsCalOutHigh(OPAMPX OPAMPx)
{
    __IO uint32_t* pCs = &OPAMP->CS1 + OPAMPx;
    return (GetBit(*pCs, OPAMP_CS_CALOUT_MASK)) ? true : false;
}
void OPAMP_CalibrationEnable(OPAMPX OPAMPx, FunctionalState en)
{
    __IO uint32_t* pCs = &OPAMP->CS1 + OPAMPx;
    if (en)
        SetBit(*pCs, OPAMP_CS_CALON_MASK);
    else
        ClrBit(*pCs, OPAMP_CS_CALON_MASK);
}
// Lock see @OPAMP_LOCK
void OPAMP_SetLock(uint32_t Lock)
{
    OPAMP->LOCK = Lock;
}
/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */
