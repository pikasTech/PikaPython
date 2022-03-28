#include "wm_adc.h"

static uint32_t _Get_Result(void)
{
	uint32_t value;
	
	value = ADC->DR;
	value = value & 0x3FFFC;
	if (value & 0x20000)
	{
		value &= 0x1FFFF;
	}
	else
	{
		value |= 0x20000;
	}
	return value;
}

HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef* hadc)
{
	uint32_t div;
	wm_sys_clk sysclk;
	
	if (hadc == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
	assert_param(IS_ADC_CHANNEL(hadc->Init.channel));
	assert_param(IS_ADC_FREQUENCY(hadc->Init.freq));
	
	hadc->Lock = HAL_UNLOCKED;
	
	HAL_ADC_MspInit(hadc);
	
	SystemClock_Get(&sysclk);
	div = sysclk.apbclk * 1000000 / (hadc->Init.freq) / 512;
	MODIFY_REG(RCC->CLK_SEL, RCC_CLK_SEL_ADC_DIV, ((div & 0xFF) << RCC_CLK_SEL_ADC_DIV_Pos));
	SET_BIT(RCC->CLK_DIV, RCC_CLK_DIV_FREQ_EN);
	
	MODIFY_REG(hadc->Instance->ADC_CR, ADC_ADC_CR_DMAEN | ADC_ADC_CR_SWITCHTIME | ADC_ADC_CR_INITTIME, 
	(0x50 << ADC_ADC_CR_SWITCHTIME_Pos) | (0x50 << ADC_ADC_CR_INITTIME_Pos) | ADC_ADC_CR_ADCIE);
	
	MODIFY_REG(hadc->Instance->PGA_CR, ADC_PGA_CR_BPREF | ADC_PGA_CR_GAIN | ADC_PGA_CR_BP, ADC_PGA_CR_CHOPEN | ADC_PGA_CR_PGAEN);

	MODIFY_REG(hadc->Instance->ANA_CR, ADC_ANA_CR_CH | ADC_ANA_CR_PD, ADC_ANA_CR_RST | ADC_ANA_CR_LDOEN | ADC_ANA_CR_CH_OFFSET);
	
	HAL_ADC_PollForConversion(hadc);
	hadc->offset = _Get_Result();
	__HAL_ADC_DISABLE(hadc);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_DeInit(ADC_HandleTypeDef* hadc)
{
	if (hadc == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
	
	__HAL_ADC_DISABLE(hadc);
	HAL_ADC_MspDeInit(hadc);
	__HAL_UNLOCK(hadc);
	
	return HAL_OK;
}

__attribute__((weak)) void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	UNUSED(hadc);
}

__attribute__((weak)) void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
	UNUSED(hadc);
}

HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef* hadc)
{
	assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
	
	__HAL_LOCK(hadc);
	MODIFY_REG(hadc->Instance->ANA_CR, ADC_ANA_CR_CH, hadc->Init.channel);
	__HAL_ADC_CLEAR_FLAG(hadc, ADC_IF_ADC | ADC_IF_CMP);
	__HAL_ADC_ENABLE(hadc);
	__HAL_UNLOCK(hadc);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef* hadc)
{
	assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
	
	__HAL_LOCK(hadc);
	__HAL_ADC_DISABLE(hadc);
	__HAL_UNLOCK(hadc);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef* hadc)
{
	uint32_t count = 0;
	assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
	
	while (1)
	{
		if (HAL_IS_BIT_SET(hadc->Instance->IF, ADC_IF_ADC))
		{
			count++;
			__HAL_ADC_CLEAR_FLAG(hadc, ADC_IF_ADC);
			if(count == 4)
			{
				break;
			}
		}
	}
	
	return HAL_OK;
}

int HAL_ADC_GetValue(ADC_HandleTypeDef* hadc)
{
	int value = 0;
	
	assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
	
	value = _Get_Result() - hadc->offset;
	
	return value;
}


int HAL_ADC_GET_INPUT_VOLTAGE(ADC_HandleTypeDef* hadc)
{
	int value;
	double voltage = 0.0;
	
	assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
	
	HAL_ADC_Start(hadc);
	HAL_ADC_PollForConversion(hadc);
	HAL_ADC_Stop(hadc);
	value = HAL_ADC_GetValue(hadc);
	voltage = (double)value / 4.0 * (126363 / 1000.0) / 1000000 + 1.196;
	
	value = (int)(voltage * 1000);
	return value;
}

HAL_StatusTypeDef HAL_ADC_Start_IT(ADC_HandleTypeDef* hadc)
{
	assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
	
	__HAL_LOCK(hadc);
	MODIFY_REG(hadc->Instance->ANA_CR, ADC_ANA_CR_CH, hadc->Init.channel);
	__HAL_ADC_CLEAR_FLAG(hadc, ADC_IF_ADC | ADC_IF_CMP);
	__HAL_ADC_INT_ENABLE(hadc, ADC_ADC_CR_ADCIE);
	__HAL_ADC_ENABLE(hadc);
	__HAL_UNLOCK(hadc);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_Stop_IT(ADC_HandleTypeDef* hadc)
{
	assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
	
	__HAL_LOCK(hadc);
	__HAL_ADC_DISABLE(hadc);
	__HAL_ADC_INT_DISABLE(hadc, ADC_ADC_CR_ADCIE);
	__HAL_UNLOCK(hadc);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_Start_Compare_IT(ADC_HandleTypeDef* hadc)
{
	assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
	assert_param(IS_ADC_CMP_POL(hadc));
	
	__HAL_LOCK(hadc);
	__HAL_ADC_CLEAR_FLAG(hadc, ADC_IF_ADC | ADC_IF_CMP);
	MODIFY_REG(hadc->Instance->ANA_CR, ADC_ANA_CR_CH, hadc->Init.channel);
	MODIFY_REG(hadc->Instance->ADC_CR, ADC_ADC_CR_CMPPOL | ADC_ADC_CR_CMPEN, ((hadc->Init.cmp_pol) | ADC_ADC_CR_CMPEN));
	WRITE_REG(hadc->Instance->CMP_VAL, ((hadc->Init.cmp_val) & 0x3FFFF));
	__HAL_ADC_INT_ENABLE(hadc, ADC_ADC_CR_CMPIE);
	__HAL_ADC_ENABLE(hadc);
	__HAL_UNLOCK(hadc);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_Stop_Compare_IT(ADC_HandleTypeDef* hadc)
{
	assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
	
	__HAL_LOCK(hadc);
	__HAL_ADC_DISABLE(hadc);
	__HAL_ADC_INT_DISABLE(hadc, ADC_IF_CMP | ADC_ADC_CR_CMPEN);
	__HAL_UNLOCK(hadc);
	
	return HAL_OK;
}

__attribute__((weak)) void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	UNUSED(hadc);
}

__attribute__((weak)) void HAL_ADC_CompareCallback(ADC_HandleTypeDef* hadc)
{
	UNUSED(hadc);
}

void HAL_ADC_IRQHandler(ADC_HandleTypeDef* hadc)
{
	assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
	
	if (__HAL_ADC_GET_IT_SOURCE(hadc, ADC_ADC_CR_ADCIE))
	{
		if (__HAL_ADC_GET_FLAG(hadc, ADC_IF_ADC))
		{
			__HAL_ADC_CLEAR_FLAG(hadc, ADC_IF_ADC);
			HAL_ADC_ConvCpltCallback(hadc);
		}
	}
	if (__HAL_ADC_GET_IT_SOURCE(hadc, ADC_ADC_CR_CMPIE))
	{
		if (__HAL_ADC_GET_FLAG(hadc, ADC_IF_CMP))
		{
			__HAL_ADC_CLEAR_FLAG(hadc, ADC_IF_CMP);
			HAL_ADC_CompareCallback(hadc);
		}
	}
}
