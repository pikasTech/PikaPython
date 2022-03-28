#include "wm_tim.h"

HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim)
{
	uint32_t offset = 0;
	wm_sys_clk sysclk;
	
	if (htim == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_TIM_INSTANCE(htim->Instance));
	assert_param(IS_TIM_UNIT(htim->Init.Unit));
	assert_param(IS_TIM_AUTORELOAD(htim->Init.AutoReload));
	
	if (htim->State == HAL_TIM_STATE_RESET)
	{
		htim->Lock = HAL_UNLOCKED;
		HAL_TIM_Base_MspInit(htim);
	}
	htim->State = HAL_TIM_STATE_BUSY;
	
	SystemClock_Get(&sysclk);
	WRITE_REG(TIM->TMR_CONFIG, sysclk.apbclk-1);
	offset = htim->Instance - TIM0;
	MODIFY_REG(TIM->CR, (TIM_CR_TIM_MODE(offset) | TIM_CR_TIM_UNIT(offset)), 
				((htim->Init.AutoReload << TIM_CR_TIM_MODE_Pos(offset)) | (htim->Init.Unit << TIM_CR_TIM_UNIT_Pos(offset))));
					
	WRITE_REG(*(uint32_t *)(&(TIM->TIM0_PRD) + offset), htim->Init.Period);
	htim->State = HAL_TIM_STATE_READY;
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_DeInit(TIM_HandleTypeDef *htim)
{
	assert_param(IS_TIM_INSTANCE(htim->Instance));
	
	htim->State = HAL_TIM_STATE_BUSY;
	__HAL_TIM_DISABLE(htim);
	HAL_TIM_Base_MspDeInit(htim);
	htim->State = HAL_TIM_STATE_RESET;
	__HAL_UNLOCK(htim);
	
	return HAL_OK;
}

__attribute__((weak)) void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	UNUSED(htim);
}

__attribute__((weak)) void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
	UNUSED(htim);
}

HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim)
{
	assert_param(IS_TIM_INSTANCE(htim->Instance));
	
	if (htim->State != HAL_TIM_STATE_READY)
	{
		return HAL_ERROR;
	}
	
	htim->State = HAL_TIM_STATE_BUSY;
	__HAL_TIM_ENABLE(htim);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef *htim)
{
	assert_param(IS_TIM_INSTANCE(htim->Instance));
	
	__HAL_TIM_DISABLE(htim);
	
	htim->State = HAL_TIM_STATE_READY;
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim)
{
	assert_param(IS_TIM_INSTANCE(htim->Instance));
	
	if (htim->State != HAL_TIM_STATE_READY)
	{
		return HAL_ERROR;
	}
	htim->State = HAL_TIM_STATE_BUSY;
	__HAL_TIM_ENABLE_IT(htim);
	__HAL_TIM_ENABLE(htim);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef *htim)
{
	assert_param(IS_TIM_INSTANCE(htim->Instance));
	
	__HAL_TIM_DISABLE_IT(htim);
	__HAL_TIM_DISABLE(htim);
	htim->State = HAL_TIM_STATE_READY;
	
	return HAL_OK;
}

HAL_TIM_StateTypeDef HAL_TIM_Base_GetState(TIM_HandleTypeDef *htim)
{
	return htim->State;
}

__attribute__((weak)) void HAL_TIM_Callback(TIM_HandleTypeDef *htim)
{
	UNUSED(htim);
}

void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim)
{
	if (__HAL_TIM_GET_FLAG(htim) != RESET)
	{
		__HAL_TIM_CLEAR_IT(htim);
		HAL_TIM_Callback(htim);
	}
}








