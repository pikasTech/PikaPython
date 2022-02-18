#include "wm_pmu.h"

HAL_StatusTypeDef HAL_PMU_Init(PMU_HandleTypeDef *hpmu)
{
	if (hpmu == NULL)
	{
		return HAL_ERROR;
	}
	assert_param(IS_PMU_ALL_INSTANCE(hpmu->Instance));
//	assert_param(IS_PMU_CLKSOURCE(SOURCE));
	
	HAL_PMU_MspInit(hpmu);
	
	if (hpmu->ClkSource == PMU_CR_32KRC_CAL_EN)
	{
		if (READ_BIT(hpmu->Instance->CR, PMU_CR_32KRC_CAL_EN) != PMU_CR_32KRC_CAL_EN)
		{
			SET_BIT(hpmu->Instance->CR, PMU_CR_32KRC_CAL_EN);
		}
	}
	else
	{
		if (READ_BIT(hpmu->Instance->CR, PMU_CR_32KRCBYPASS) != PMU_CR_32KRCBYPASS)
		{
			SET_BIT(hpmu->Instance->CR, PMU_CR_32KRCBYPASS);
		}
	}
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_PMU_DeInit(PMU_HandleTypeDef *hpmu)
{
	HAL_PMU_MspDeInit(hpmu);
	SET_BIT(hpmu->Instance->IF, (PMU_IF_SLEEP | PMU_IF_STANDBY | PMU_IF_TIM0 | PMU_IF_IO_WAKE | PMU_IF_RTC));
	CLEAR_BIT(hpmu->Instance->TIMER0, PMU_TIMER0_EN);
	CLEAR_BIT(hpmu->Instance->RTCCR0, PMU_RTCCR0_TIMING_EN);
	CLEAR_BIT(hpmu->Instance->RTCCR1, PMU_RTCCR1_EN);
	CLEAR_BIT(hpmu->Instance->CR, (PMU_CR_SLEEP_EN | PMU_CR_STANDBY_EN));
	
	return HAL_OK;
}

__attribute__((weak)) void HAL_PMU_MspInit(PMU_HandleTypeDef *hpmu)
{
	UNUSED(hpmu);
}

__attribute__((weak)) void HAL_PMU_MspDeInit(PMU_HandleTypeDef *hpmu)
{
	UNUSED(hpmu);
}

void HAL_PMU_Enter_Sleep(PMU_HandleTypeDef *hpmu)
{
	SET_BIT(hpmu->Instance->CR, PMU_CR_SLEEP_EN);
}

void HAL_PMU_Enter_Standby(PMU_HandleTypeDef *hpmu)
{
	SET_BIT(hpmu->Instance->CR, PMU_CR_STANDBY_EN);
}

/* Period: this parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF. The unit is seconds. */
HAL_StatusTypeDef HAL_PMU_TIMER0_Start(PMU_HandleTypeDef *hpmu, uint32_t Period)
{
	if (hpmu == NULL)
	{
		return HAL_ERROR;
	}
	assert_param(IS_PMU_ALL_INSTANCE(hpmu->Instance));
	assert_param(IS_PMU_TIMPERIOD(Period));
	
	WRITE_REG(hpmu->Instance->TIMER0, (PMU_TIMER0_EN | Period));
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_PMU_TIMER0_Stop(PMU_HandleTypeDef *hpmu)
{
	if (hpmu == NULL)
	{
		return HAL_ERROR;
	}
	assert_param(IS_PMU_ALL_INSTANCE(hpmu->Instance));
	
	CLEAR_BIT(hpmu->Instance->TIMER0, PMU_TIMER0_EN);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_PMU_RTC_GetTime(PMU_HandleTypeDef *hpmu, RTC_TimeTypeDef *Time)
{
	uint32_t cr0, cr1;
	
	if ((hpmu == NULL) | (Time == NULL))
	{
		return HAL_ERROR;
	}
	assert_param(IS_RTC_ALL_INSTANCE(hpmu->Instance));
	
	cr0 = READ_REG(hpmu->Instance->RTCCR0);
	cr1 = READ_REG(hpmu->Instance->RTCCR1);
	
	Time->Year = ((cr1 & PMU_RTCCR1_YEAR) >> PMU_RTCCR1_YEAR_Pos);
	Time->Month = ((cr1 & PMU_RTCCR1_MONTH) >> PMU_RTCCR1_MONTH_Pos);
	
	Time->Date = ((cr0 & PMU_RTCCR0_DATE) >> PMU_RTCCR0_DATE_Pos);
	Time->Hours = ((cr0 & PMU_RTCCR0_HOUR) >> PMU_RTCCR0_HOUR_Pos);
	Time->Minutes = ((cr0 & PMU_RTCCR0_MINUTE) >> PMU_RTCCR0_MINUTE_Pos);
	Time->Seconds = ((cr0 & PMU_RTCCR0_SECOND) >> PMU_RTCCR0_SECOND_Pos);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_PMU_RTC_Start(PMU_HandleTypeDef *hpmu, RTC_TimeTypeDef *Time)
{
	if (hpmu == NULL)
	{
		return HAL_ERROR;
	}
	assert_param(IS_RTC_ALL_INSTANCE(hpmu->Instance));
	
	CLEAR_BIT(hpmu->Instance->RTCCR1, PMU_RTCCR1_EN);
	MODIFY_REG(hpmu->Instance->RTCCR0, (PMU_RTCCR0_DATE | PMU_RTCCR0_HOUR | PMU_RTCCR0_MINUTE | PMU_RTCCR0_SECOND),
					((Time->Date << PMU_RTCCR0_DATE_Pos) | (Time->Hours << PMU_RTCCR0_HOUR_Pos) | 
					(Time->Minutes << PMU_RTCCR0_MINUTE_Pos) | (Time->Seconds << PMU_RTCCR0_SECOND_Pos)));
					
	MODIFY_REG(hpmu->Instance->RTCCR1, (PMU_RTCCR1_YEAR | PMU_RTCCR1_MONTH),
					((Time->Year << PMU_RTCCR1_YEAR_Pos) | (Time->Month << PMU_RTCCR1_MONTH_Pos)));
					
	SET_BIT(hpmu->Instance->RTCCR1, PMU_RTCCR1_EN);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_PMU_RTC_Stop(PMU_HandleTypeDef *hpmu)
{
	if (hpmu == NULL)
	{
		return HAL_ERROR;
	}
	assert_param(IS_RTC_ALL_INSTANCE(hpmu->Instance));
	
	CLEAR_BIT(hpmu->Instance->RTCCR1, PMU_RTCCR1_EN);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_PMU_RTC_Alarm_Enable(PMU_HandleTypeDef *hpmu, RTC_TimeTypeDef *Time)
{
	if (hpmu == NULL)
	{
		return HAL_ERROR;
	}
	assert_param(IS_RTC_ALL_INSTANCE(hpmu->Instance));
	
	SET_BIT(hpmu->Instance->IF, PMU_IF_RTC);
	
	MODIFY_REG(hpmu->Instance->RTCCR1, (PMU_RTCCR1_YEAR | PMU_RTCCR1_MONTH),
					((Time->Year << PMU_RTCCR1_YEAR_Pos) | (Time->Month << PMU_RTCCR1_MONTH_Pos)));
					
	MODIFY_REG(hpmu->Instance->RTCCR0, (PMU_RTCCR0_DATE | PMU_RTCCR0_HOUR | PMU_RTCCR0_MINUTE | PMU_RTCCR0_SECOND),
					((Time->Date << PMU_RTCCR0_DATE_Pos) | (Time->Hours << PMU_RTCCR0_HOUR_Pos) | 
					(Time->Minutes << PMU_RTCCR0_MINUTE_Pos) | (Time->Seconds << PMU_RTCCR0_SECOND_Pos) | PMU_RTCCR0_TIMING_EN));
						
	return HAL_OK;
}

HAL_StatusTypeDef HAL_PMU_RTC_Alarm_Disable(PMU_HandleTypeDef *hpmu)
{
	if (hpmu == NULL)
	{
		return HAL_ERROR;
	}
	assert_param(IS_RTC_ALL_INSTANCE(hpmu->Instance));
	
	CLEAR_BIT(hpmu->Instance->RTCCR0, PMU_RTCCR0_TIMING_EN);
	
	return HAL_OK;
}

__attribute__((weak)) void HAL_PMU_Tim0_Callback(PMU_HandleTypeDef *hpmu)
{
	UNUSED(NULL);
}

__attribute__((weak)) void HAL_PMU_IO_Callback(PMU_HandleTypeDef *hpmu)
{
	UNUSED(NULL);
}

__attribute__((weak)) void HAL_PMU_RTC_Callback(PMU_HandleTypeDef *hpmu)
{
	UNUSED(NULL);
}

void HAL_PMU_IRQHandler(PMU_HandleTypeDef *hpmu)
{
	uint32_t flag = READ_REG(hpmu->Instance->IF);
	
	SET_BIT(hpmu->Instance->IF, (PMU_IF_SLEEP | PMU_IF_STANDBY | PMU_IF_TIM0 | PMU_IF_IO_WAKE | PMU_IF_RTC));
	if ((flag & PMU_IF_TIM0) == PMU_IF_TIM0)
	{
		HAL_PMU_Tim0_Callback(hpmu);
	}
	if ((flag & PMU_IF_IO_WAKE) == PMU_IF_IO_WAKE)
	{
		HAL_PMU_IO_Callback(hpmu);
	}
	if ((flag & PMU_IF_RTC) == PMU_IF_RTC)
	{
		HAL_PMU_RTC_Callback(hpmu);
	}
}
