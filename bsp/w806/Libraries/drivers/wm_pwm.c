#include "wm_pwm.h"

/* 输出波形的频率： f = 40MHz / Prescaler / (Period + 1)；
 * 输出波形的占空比： 
 *     沿对齐模式（递减）：(Pulse + 1) / (Period + 1)
 *                         Pulse >= Period：PWM输出一直为高电平
 *                         Pulse < Period ：PWM输出高电平宽度为(Pulse + 1)，低电平宽度为(Period - Pulse)
 *                         Pulse = 0      ：PWM输出高电平宽度为(1)，低电平宽度为(Period)
 * 
 *    中间对齐模式       ：(2 * Pulse + 1) / (2 * (Period + 1))
 *                         Pulse > Period ：PWM输出一直为高电平
 *                         Pulse <= Period：PWM输出高电平宽度为(2 * Pulse + 1)，低电平宽度为(2 * (Period - Pulse) + 1)
 *                         Pulse = 0      ：PWM输出高电平宽度为(1)，低电平宽度为(2 * Period + 1)
 */

HAL_StatusTypeDef __PWM_OutMode_Config(PWM_HandleTypeDef *hpwm, uint32_t Channel, uint32_t OutMode)
{
	if (PWM_OUT_MODE_BREAK == OutMode)
	{
		SET_BIT(hpwm->Instance->BKCR, (1 << (PWM_BKCR_EN_Pos + Channel)));
	}
	else if (PWM_OUT_MODE_5SYNC == OutMode)
	{
		assert_param(Channel == PWM_CHANNEL_0);
		CLEAR_BIT(hpwm->Instance->BKCR, (PWM_CHANNEL_ALL << PWM_BKCR_EN_Pos));
		SET_BIT(hpwm->Instance->CR, PWM_CR_ALLSYNCEN);
	}
	else if (PWM_OUT_MODE_2SYNC == OutMode)
	{
		assert_param((Channel == PWM_CHANNEL_0) || (Channel == PWM_CHANNEL_2));
		CLEAR_BIT(hpwm->Instance->BKCR, (PWM_CHANNEL_ALL << PWM_BKCR_EN_Pos));
		CLEAR_BIT(hpwm->Instance->CR, PWM_CR_ALLSYNCEN);
		SET_BIT(hpwm->Instance->CR, (1 << (PWM_CR_TWOSYNCEN_Pos + Channel / 2)));
	}
	else if (PWM_OUT_MODE_2COMPLEMENTARY == OutMode)
	{
		assert_param((Channel == PWM_CHANNEL_0) || (Channel == PWM_CHANNEL_2));
		CLEAR_BIT(hpwm->Instance->BKCR, (PWM_CHANNEL_ALL << PWM_BKCR_EN_Pos));
		CLEAR_BIT(hpwm->Instance->CR, PWM_CR_ALLSYNCEN);
		CLEAR_BIT(hpwm->Instance->CR, (1 << (PWM_CR_TWOSYNCEN_Pos + Channel / 2)));
		SET_BIT(hpwm->Instance->CR, (1 << (PWM_CR_2COMPLEMENTARY_Pos + Channel / 2)));
		if (Channel == PWM_CHANNEL_0)
		{
			MODIFY_REG(hpwm->Instance->DTCR, (PWM_DTCR_DTDIV | PWM_DTCR_DTCNT01), (PWM_DTCR_DTEN01 | hpwm->Init.Dtdiv |  hpwm->Init.Dtcnt));
		}
		else if (Channel == PWM_CHANNEL_2)
		{
			MODIFY_REG(hpwm->Instance->DTCR, (PWM_DTCR_DTDIV | PWM_DTCR_DTCNT23), (PWM_DTCR_DTEN23 | hpwm->Init.Dtdiv |  hpwm->Init.Dtcnt));
		}
	}
	else if (PWM_OUT_MODE_INDEPENDENT == OutMode)
	{
		CLEAR_BIT(hpwm->Instance->BKCR, (PWM_CHANNEL_ALL << PWM_BKCR_EN_Pos));
		CLEAR_BIT(hpwm->Instance->CR, PWM_CR_ALLSYNCEN);
		if (Channel != PWM_CHANNEL_4)
		{
			CLEAR_BIT(hpwm->Instance->CR, (1 << (PWM_CR_TWOSYNCEN_Pos + Channel / 2)));
			CLEAR_BIT(hpwm->Instance->CR, (1 << (PWM_CR_2COMPLEMENTARY_Pos + Channel / 2)));
		}
	}
	
	return HAL_OK;
}

HAL_StatusTypeDef __PWM_CountType_Config(PWM_HandleTypeDef *hpwm, uint32_t Channel, uint32_t CounterMode)
{
	if (Channel == PWM_CHANNEL_4)
	{
		MODIFY_REG(hpwm->Instance->CH4CR2, PWM_CH4CR2_CNTTYPE, (CounterMode << PWM_CH4CR2_CNTTYPE_Pos));
	}
	else
	{
		MODIFY_REG(hpwm->Instance->CR, (0x0FF << (PWM_CR_CNTTYPE0_Pos + Channel * 2)), 
					(CounterMode << (PWM_CR_CNTTYPE0_Pos + Channel * 2)));
	}
	
	return HAL_OK;
}

HAL_StatusTypeDef __PWM_Freq_Config(PWM_HandleTypeDef *hpwm, uint32_t Channel, uint32_t Prescaler, uint32_t Period)
{
	if (hpwm->Channel == PWM_CHANNEL_0)
	{
		MODIFY_REG(hpwm->Instance->CLKDIV01, PWM_CLKDIV01_CH0, hpwm->Init.Prescaler);
		MODIFY_REG(hpwm->Instance->PERIOD, PWM_PERIOD_CH0, hpwm->Init.Period);
	}
	else if (hpwm->Channel == PWM_CHANNEL_1)
	{
		MODIFY_REG(hpwm->Instance->CLKDIV01, PWM_CLKDIV01_CH1, (hpwm->Init.Prescaler << PWM_CLKDIV01_CH1_Pos));
		MODIFY_REG(hpwm->Instance->PERIOD, PWM_PERIOD_CH1, (hpwm->Init.Period << PWM_PERIOD_CH1_Pos));
	}
	else if (hpwm->Channel == PWM_CHANNEL_2)
	{
		MODIFY_REG(hpwm->Instance->CLKDIV23, PWM_CLKDIV23_CH2, hpwm->Init.Prescaler);
		MODIFY_REG(hpwm->Instance->PERIOD, PWM_PERIOD_CH2, (hpwm->Init.Period << PWM_PERIOD_CH2_Pos));
	}
	else if (hpwm->Channel == PWM_CHANNEL_3)
	{
		MODIFY_REG(hpwm->Instance->CLKDIV23, PWM_CLKDIV23_CH3, (hpwm->Init.Prescaler << PWM_CLKDIV23_CH3_Pos));
		MODIFY_REG(hpwm->Instance->PERIOD, PWM_PERIOD_CH3, (hpwm->Init.Period << PWM_PERIOD_CH3_Pos));
	}
	else if (hpwm->Channel == PWM_CHANNEL_4)
	{
		MODIFY_REG(hpwm->Instance->CH4CR1, (PWM_CH4CR1_DIV | PWM_CH4CR1_PRD), 
					((hpwm->Init.Prescaler << PWM_CH4CR1_DIV_Pos) | (hpwm->Init.Period << PWM_CH4CR1_PRD_Pos)));
	}
	
	return HAL_OK; 
}

HAL_StatusTypeDef __PWM_Duty_Config(PWM_HandleTypeDef *hpwm, uint32_t Channel, uint32_t Pulse)
{
	if (Channel == PWM_CHANNEL_0)
	{
		MODIFY_REG(hpwm->Instance->CMPDAT, PWM_CMPDAT_CH0, Pulse);
	}
	else if (Channel == PWM_CHANNEL_1)
	{
		MODIFY_REG(hpwm->Instance->CMPDAT, PWM_CMPDAT_CH1, (Pulse << PWM_CMPDAT_CH1_Pos));
	}
	else if (Channel == PWM_CHANNEL_2)
	{
		MODIFY_REG(hpwm->Instance->CMPDAT, PWM_CMPDAT_CH2, (Pulse << PWM_CMPDAT_CH2_Pos));
	}
	else if (Channel == PWM_CHANNEL_3)
	{
		MODIFY_REG(hpwm->Instance->CMPDAT, PWM_CMPDAT_CH3, (Pulse << PWM_CMPDAT_CH3_Pos));
	}
	else if (Channel == PWM_CHANNEL_4)
	{
		MODIFY_REG(hpwm->Instance->CH4CR2, PWM_CH4CR2_CMP, (Pulse << PWM_CH4CR2_CMP_Pos));
	}
	
	return HAL_OK;
}

HAL_StatusTypeDef __PWM_AutoReload_Config(PWM_HandleTypeDef *hpwm, uint32_t Channel, uint32_t AutoReloadPreload)
{
	if (Channel == PWM_CHANNEL_4)
	{
		MODIFY_REG(hpwm->Instance->CH4CR2, PWM_CH4CR2_CNTMODE, (AutoReloadPreload << PWM_CH4CR2_CNTMODE_Pos));
	}
	else
	{
		MODIFY_REG(hpwm->Instance->CR, (0x01 << (PWM_CR_CNTMODE_Pos + Channel)), 
					(AutoReloadPreload << (PWM_CR_CNTMODE_Pos + Channel)));
	}
	
	return HAL_OK;
}

HAL_StatusTypeDef __PWM_OutInverse_Config(PWM_HandleTypeDef *hpwm, uint32_t Channel, uint32_t OutInverse)
{
	if (Channel == PWM_CHANNEL_4)
	{
		MODIFY_REG(hpwm->Instance->CH4CR2, PWM_CH4CR2_PINV, OutInverse);
	}
	else
	{
		MODIFY_REG(hpwm->Instance->CR, (0x01 << (PWM_CR_PINV_Pos + Channel)), 
					(OutInverse << (PWM_CR_PINV_Pos + Channel)));
	}
	
	return HAL_OK;
}

HAL_StatusTypeDef __PWM_OutEn_Config(PWM_HandleTypeDef *hpwm, uint32_t Channel)
{
	if (Channel == PWM_CHANNEL_0)
	{
		CLEAR_BIT(hpwm->Instance->CR, PWM_CR_POEN);
	}
	if (Channel == PWM_CHANNEL_4)
	{
		CLEAR_BIT(hpwm->Instance->CH4CR3, PWM_CH4CR3_POEN);
	}
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_PWM_Init(PWM_HandleTypeDef *hpwm)
{
	if (hpwm == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_PWM_INSTANCE(hpwm->Instance));
	assert_param(IS_PWM_CHANNELS(hpwm->Channel));
	assert_param(IS_PWM_PRESCALER(hpwm->Init.Prescaler));
	assert_param(IS_PWM_COUNTER_MODE(hpwm->Init.CounterMode));
	assert_param(IS_PWM_PERIOD(hpwm->Init.Period));
	assert_param(IS_PWM_PULSE(hpwm->Init.Pulse));
	assert_param(IS_PWM_AUTORELOADPRELOAD(hpwm->Init.AutoReloadPreload));
	assert_param(IS_PWM_OUTMODE(hpwm->Init.OutMode));
	assert_param(IS_PWM_OUTINVERSE(hpwm->Init.OutInverse));
	if (hpwm->Init.OutMode == PWM_OUT_MODE_2COMPLEMENTARY)
	{
		assert_param(IS_PWM_DTDIV(hpwm->Init.Dtdiv));
		assert_param(IS_PWM_DTCNT(hpwm->Init.Dtcnt));
	}
	
	HAL_PWM_MspInit(hpwm);
	__PWM_OutMode_Config(hpwm, hpwm->Channel, hpwm->Init.OutMode);
	__PWM_CountType_Config(hpwm, hpwm->Channel, hpwm->Init.CounterMode);
	__PWM_Freq_Config(hpwm, hpwm->Channel, hpwm->Init.Prescaler, hpwm->Init.Period);
	__PWM_Duty_Config(hpwm, hpwm->Channel, hpwm->Init.Pulse);
	__PWM_AutoReload_Config(hpwm, hpwm->Channel, hpwm->Init.AutoReloadPreload);
	__PWM_OutInverse_Config(hpwm, hpwm->Channel, hpwm->Init.OutInverse);
	__PWM_OutEn_Config(hpwm, hpwm->Channel);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_PWM_DeInit(PWM_HandleTypeDef *hpwm)
{
	assert_param(IS_PWM_INSTANCE(hpwm->Instance));
	
	HAL_PWM_MspDeInit(hpwm);
	
	return HAL_OK;
}

__attribute__((weak)) void HAL_PWM_MspInit(PWM_HandleTypeDef *hpwm)
{
	UNUSED(hpwm);
}

__attribute__((weak)) void HAL_PWM_MspDeInit(PWM_HandleTypeDef *hpwm)
{
	UNUSED(hpwm);
}

HAL_StatusTypeDef HAL_PWM_Start(PWM_HandleTypeDef *hpwm, uint32_t channel)
{
	if (hpwm == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_PWM_INSTANCE(hpwm->Instance));
	assert_param(IS_PWM_CHANNELS(channel));
	
	SET_BIT(hpwm->Instance->CR, (0x01 << (PWM_CR_CNTEN_Pos + channel)));
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_PWM_Stop(PWM_HandleTypeDef *hpwm, uint32_t Channel)
{
	if (hpwm == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_PWM_INSTANCE(hpwm->Instance));
	assert_param(IS_PWM_CHANNELS(Channel));
	
	CLEAR_BIT(hpwm->Instance->CR, (0x01 << (PWM_CR_CNTEN_Pos + Channel)));
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_PWM_Duty_Set(PWM_HandleTypeDef *hpwm, uint32_t Channel, uint32_t Duty)
{
	if (hpwm == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_PWM_INSTANCE(hpwm->Instance));
	assert_param(IS_PWM_CHANNELS(Channel));
	assert_param(IS_PWM_PULSE(Duty));
	
	__PWM_Duty_Config(hpwm, Channel, Duty);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_PWM_Freq_Set(PWM_HandleTypeDef *hpwm, uint32_t Channel, uint32_t Prescaler, uint32_t Period)
{
	if (hpwm == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_PWM_INSTANCE(hpwm->Instance));
	assert_param(IS_PWM_CHANNELS(Channel));
	assert_param(IS_PWM_PULSE(Period));
	
	__PWM_Freq_Config(hpwm, Channel, Prescaler, Period);
	
	return HAL_OK;
}

