#include "wm_wdg.h"

HAL_StatusTypeDef HAL_WDG_Init(WDG_HandleTypeDef *hwdg)
{
	wm_sys_clk sysclk;
	
	if (hwdg == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_WDG_ALL_INSTANCE(hwdg->Instance));
	assert_param(IS_WDG_COUNTER(hwdg->Init.Reload));
	
	HAL_WDG_MspInit(hwdg);
	
	SystemClock_Get(&sysclk);
	WRITE_REG(hwdg->Instance->LD, (sysclk.apbclk * hwdg->Init.Reload));
	WRITE_REG(hwdg->Instance->CR, 0x03);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_WDG_DeInit(WDG_HandleTypeDef *hwdg)
{
	HAL_NVIC_DisableIRQ(WDG_IRQn);
	WRITE_REG(hwdg->Instance->CR, 0);
	__HAL_WDG_CLEAR_FLAG(hwdg, WDG_CLR);
	
	return HAL_OK;
}

__attribute__((weak)) void HAL_WDG_MspInit(WDG_HandleTypeDef *hwdg)
{
	UNUSED(hwdg);
}

void HAL_WDG_IRQHandler(WDG_HandleTypeDef *hwdg)
{
	__HAL_WDG_CLEAR_FLAG(hwdg, WDG_CLR);	// 如果不清除中断，不会立即复位，在下一个周期中断到来时才复位。
											// 例如定时5s，第一次来中断时没清除，则5s后下一个中断到来才复位
}