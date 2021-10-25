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
 * @file cm32m101a_tsc.c
 * @author CMIOT
 * @version v1.0.0
 *
 * @COPYRIGHT(c) 2020, China Mobile IOT. All rights reserved.
 */
#include "cm32m101a.h"
#include "cm32m101a_tsc.h"

/**
* @brief  Init TSC config
* @param  InitParam: TSC initialize structure
* @return : TSC_ErrorTypeDef
*/
TSC_ErrorTypeDef TSC_Init(TSC_InitType* InitParam)
{
    uint32_t tempreg,timeout;

    assert_param(IS_TSC_DET_MODE(InitParam->Mode));
    assert_param(IS_TSC_PAD_OPTION(InitParam->PadOpt));
    assert_param(IS_TSC_PAD_SPEED(InitParam->Speed));

    /* waiting tsc hw for idle status.*/
    timeout = 0;
    do
    {
        __TSC_HW_DISABLE();
        
        if(++timeout > TSC_TIMEOUT)
            return TSC_ERROR_HW_MODE;
    }while (__TSC_GET_HW_MODE());

    /*TSC_CTRL config*/
    tempreg = 0;
    if(InitParam->Mode == TSC_HW_DETECT_MODE)
    {
        assert_param(IS_TSC_DET_PERIOD(InitParam->Period));
        assert_param(IS_TSC_FILTER(InitParam->Filter));
        assert_param(IS_TSC_DET_TYPE(InitParam->Type));
        assert_param(IS_TSC_INT(InitParam->Int));
    
        tempreg |= InitParam->Period;
        tempreg |= InitParam->Filter;
        tempreg |= InitParam->Type;
        tempreg |= InitParam->Int;
    }
    else
    {
        assert_param(IS_TSC_OUT(InitParam->Out));
        tempreg |= InitParam->Out;
    }

    TSC->CTRL = tempreg;

    /*TSC_ANA_SEL config*/
    TSC->ANA_SEL = InitParam->PadOpt | InitParam->Speed;

    return TSC_ERROR_OK;
}

/**
 * @brief  Config the clock source of TSC 
 * @param  TSC_ClkSource specifies the clock source of TSC
 *   This parameter can be one of the following values:
 *     @arg TSC_CLK_SRC_LSI:              TSC clock source is LSI(default)
 *     @arg TSC_CLK_SRC_LSE:              TSC clock source is LSE,and LSE is oscillator
 *     @arg TSC_CLK_SRC_LSE_BYPASS:       TSC clock source is LSE,and LSE is extennal clock
 * @retval TSC error code
 */
TSC_ErrorTypeDef TSC_ClockConfig(uint32_t TSC_ClkSource)
{
    uint32_t timeout;
    
    /*Enable PWR  peripheral Clock*/
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR,ENABLE);

    if(TSC_CLK_SRC_LSI == TSC_ClkSource)
    {
        /*enable LSI clock*/
        RCC_EnableLsi(ENABLE);

        /*Wait LSI stable*/
        timeout = 0;
        while(RCC_GetFlagStatus(RCC_CTRLSTS_FLAG_LSIRD) == RESET)
        {
            if(++timeout >TSC_TIMEOUT)
                return TSC_ERROR_CLOCK;
        }
    }
    else if((TSC_CLK_SRC_LSE_BYPASS==TSC_ClkSource)||(TSC_CLK_SRC_LSE==TSC_ClkSource))
    {
        if(RCC_GetFlagStatus(RCC_LDCTRL_FLAG_LSERD)==RESET)
        {
            RCC_ConfigLse(TSC_ClkSource & (~RCC_LDCTRL_LSXSEL));
            timeout = 0;
            while(RCC_GetFlagStatus(RCC_LDCTRL_FLAG_LSERD) == RESET)
            {
                if(++timeout >TSC_TIMEOUT)
                    return TSC_ERROR_CLOCK;
            }
        }
    }
    else
        return TSC_ERROR_PARAMETER;
    
    // Set bit 8 of PWR_CTRL1.Open PWR DBP.
    PWR_BackupAccessEnable(ENABLE); //PWR->CTRL1 |=  0x100;

    /*set LSI as TSC clock source*/
    RCC_ConfigLSXClk(TSC_ClkSource & RCC_LDCTRL_LSXSEL);

    /*Enable TSC clk*/
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TSC,ENABLE);

    return TSC_ERROR_OK;
}

/**
* @brief  Configure internal charge resistor for some channels 
* @param  res:  internal resistor selecte
*   This parameter can be one of the following values:
*      @arg TSC_RESR_CHN_RESIST_0:    1M OHM
*      @arg TSC_RESR_CHN_RESIST_1:    882K OHM 
*      @arg TSC_RESR_CHN_RESIST_2:    756K OHM 
*      @arg TSC_RESR_CHN_RESIST_3:    630K OHM 
*      @arg TSC_RESR_CHN_RESIST_4:    504K OHM 
*      @arg TSC_RESR_CHN_RESIST_5:    378K OHM
*      @arg TSC_RESR_CHN_RESIST_6:    252K OHM 
*      @arg TSC_RESR_CHN_RESIST_7:    126K OHM 
* @param  Channels: channels to be configed, as TSC_CHNEN defined
*   This parameter:bit[0:23] used,bit[24:31] must be 0
*                  bitx: TSC channel x
* @return: none
*/
TSC_ErrorTypeDef TSC_ConfigInternalResistor(uint32_t Channels, uint32_t res )
{
    uint32_t i,chn,timeout,nReg,nPos;

    assert_param(IS_TSC_CHN(Channels));
    assert_param(IS_TSC_RESISTOR_VALUE(res));

    /*Check charge resistor value */
    if(res > TSC_RESRx_CHN_RESIST_7)
        return TSC_ERROR_PARAMETER;
    
    /* waiting tsc hw for idle status.*/
    timeout = 0;
    do
    {
        __TSC_HW_DISABLE();
        
        if(++timeout > TSC_TIMEOUT)
            return TSC_ERROR_HW_MODE;
    }while (__TSC_GET_HW_MODE());

    /* Mask invalie bits*/
    chn  = Channels & TSC_CHNEN_CHN_SELx_Msk;
    
    /* Set resistance for each channel one by one*/
    for (i = 0; i<MAX_TSC_HW_CHN; i++)
    {
        if (chn & 0x00000001)
        {
            nReg = i >> 3;
            nPos = (i & 0x7UL)*4;
            MODIFY_REG(TSC->RESR[nReg],TSC_RESRx_CHN_RESIST_Msk<<nPos,res<<nPos);
        }

        chn >>= 1;
    }

    return TSC_ERROR_OK;
}

/**
* @brief  Configure threshold value for some channels 
* @param  Channels: channels to be configed, as TSC_CHNEN defined
*   This parameter:bit[0:23] used,bit[24:31] must be 0
*                  bitx: TSC channel x
* @param  base:   base value of threshold, 0-MAX_TSC_THRESHOLD_BASE
* @param  delta:  delta value of threshold,0-MAX_TSC_THRESHOLD_DELRA
* @return: None
*/
TSC_ErrorTypeDef TSC_ConfigThreshold(  uint32_t Channels, uint32_t base, uint32_t delta)
{
    uint32_t i, chn,timeout;
    assert_param(IS_TSC_CHN(Channels));
    assert_param(IS_TSC_THRESHOLD_BASE(base));
    assert_param(IS_TSC_THRESHOLD_DELTA(delta));

    /*Check the base and delta value*/
    if( (base>MAX_TSC_THRESHOLD_BASE)||(delta>MAX_TSC_THRESHOLD_DELTA))
        return TSC_ERROR_PARAMETER;
    
    /* waiting tsc hw for idle status.*/
    timeout = 0;
    do
    {
        __TSC_HW_DISABLE();
        
        if(++timeout > TSC_TIMEOUT)
            return TSC_ERROR_HW_MODE;
    }while (__TSC_GET_HW_MODE());

    /*Mask invalie bits*/
    chn = Channels & TSC_CHNEN_CHN_SELx_Msk;
    
    /* Set the base and delta for each channnel one by one*/
    for (i = 0; i<MAX_TSC_HW_CHN; i++)
    {
        if (chn & 0x00000001)
        {
            TSC->THRHD[i] = (base<<TSC_THRHDx_BASE_Pos)|(delta<<TSC_THRHDx_DELTA_Pos);
        }

        chn >>= 1;
    }

    return TSC_ERROR_OK;
}


/**
* @brief  Get parameters of one channel.
* @param  ChnCfg: Pointer of TSC_ChnCfg structure.
* @param  ChannelNum: The channel number of which we want to get parameters,must be less then MAX_TSC_HW_CHN
* @return: None
*/
TSC_ErrorTypeDef TSC_GetChannelCfg( TSC_ChnCfg* ChnCfg, uint32_t ChannelNum)
{
    uint32_t nReg,nPos;

    assert_param(IS_TSC_CHN_NUMBER(ChannelNum));

    /*Check channel number*/
    if(!(IS_TSC_CHN_NUMBER(ChannelNum)))
        return TSC_ERROR_PARAMETER;

    /* Get the base and delta value for a channel*/
    ChnCfg->TSC_Base     = (TSC->THRHD[ChannelNum] & TSC_THRHDx_BASE_Msk) >> TSC_THRHDx_BASE_Pos;
    ChnCfg->TSC_Delta    = (TSC->THRHD[ChannelNum] & TSC_THRHDx_DELTA_Msk)>> TSC_THRHDx_DELTA_Pos;

    /* Get the charge resistor type for a channel*/
    nReg = ChannelNum>>3;
    nPos = (ChannelNum & 0x7UL)*4;
    ChnCfg->TSC_Resistor = (TSC->RESR[nReg] >> nPos) & TSC_RESRx_CHN_RESIST_Msk;

    return TSC_ERROR_OK;
}


