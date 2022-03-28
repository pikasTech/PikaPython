#include "wm_hal.h"

HAL_StatusTypeDef HAL_Init(void)
{
	HAL_MspInit();
	return HAL_OK;
}

HAL_StatusTypeDef HAL_DeInit(void)
{
	return HAL_OK;
}

__attribute__((weak)) void HAL_MspInit(void)
{
	
}

__attribute__((weak)) void HAL_MspDeInit(void)
{
	
}
