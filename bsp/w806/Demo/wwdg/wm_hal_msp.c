#include "wm_hal.h"

void HAL_MspInit(void)
{

}

void HAL_WDG_MspInit(WDG_HandleTypeDef* hwdg)
{
	if(hwdg->Instance == WDG)
	{
		HAL_NVIC_SetPriority(WDG_IRQn, 0);
		HAL_NVIC_EnableIRQ(WDG_IRQn);
	}
}

