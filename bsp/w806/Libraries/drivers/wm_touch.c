#include "wm_touch.h"

// 在使用TOUCH功能时，TOUCH0（PA7）必须复用为TOUCH功能，不可以作为其他功能！！！


HAL_StatusTypeDef HAL_TOUCH_Init(TOUCH_HandleTypeDef *htouch)
{
	int i;
	
	if (htouch == NULL)
	{
		return HAL_ERROR;
	}
	assert_param(IS_TOUCH_INSTANCE(htouch->Instance));
	assert_param(IS_TOUCH_CHANNELS(htouch->Init.Channel));
	assert_param(IS_TOUCH_WINDOW(htouch->Init.Window));
	assert_param(IS_TOUCH_SCANPERIOD(htouch->Init.ScanPeriod));
	
	HAL_TOUCH_MspInit(htouch);
	WRITE_REG(htouch->Instance->CR, 0);
	WRITE_REG(htouch->Instance->IE_IF, 0x0000FFFF);
	for (i = 0; i < 16; i++)
	{
		if (htouch->Init.Channel & (1 << i))
		{
			assert_param(IS_TOUCH_THRESHOLD(htouch->Init.Threshold[i]));
			MODIFY_REG(*((&(htouch->Instance->CH0CR)) + i), TOUCH_CH0CR_THRESHOLD, htouch->Init.Threshold[i]);
			if (htouch->Init.Irq_en  & (1 << i))
			{
				SET_BIT(htouch->Instance->IE_IF, (1 << (16 + i)));
			}
		}
	}
	WRITE_REG(htouch->Instance->CR, ((htouch->Init.Channel << TOUCH_CR_CH_SEL_Pos) | 
									 (htouch->Init.ScanPeriod << TOUCH_CR_SCAN_PERIOD_Pos) | 
									 (htouch->Init.Window << TOUCH_CR_CAPDET_CNT_Pos) | 
									 TOUCH_CR_EN));
									 
	return HAL_OK;
}

HAL_StatusTypeDef HAL_TOUCH_DeInit(TOUCH_HandleTypeDef *htouch)
{
	if (htouch == NULL)
	{
		return HAL_ERROR;
	}
	CLEAR_BIT(htouch->Instance->CR, TOUCH_CR_EN);
	HAL_TOUCH_MspDeInit(htouch);
	
	return HAL_OK;
}

__attribute__((weak)) void HAL_TOUCH_MspInit(TOUCH_HandleTypeDef *htouch)
{
	UNUSED(htouch);
}

__attribute__((weak)) void HAL_TOUCH_MspDeInit(TOUCH_HandleTypeDef *htouch)
{
	UNUSED(htouch);
}

__attribute__((weak)) void HAL_TOUCH_Callback(TOUCH_HandleTypeDef *htouch, uint16_t Flag)
{
	UNUSED(htouch);
}

void HAL_TOUCH_IRQHandler(TOUCH_HandleTypeDef *htouch)
{
	uint32_t flag = __HAL_TOUCH_GET_FLAG(htouch);
	if (flag != RESET)
	{
		__HAL_TOUCH_CLEAR_IT(htouch, flag);
		HAL_TOUCH_Callback(htouch, flag & TOUCH_IE_IF_FLAG);
	}
}
