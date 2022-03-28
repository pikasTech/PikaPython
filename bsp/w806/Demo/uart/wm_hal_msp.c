
#include "wm_hal.h"

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
	if (huart->Instance == UART1)
	{
		__HAL_RCC_UART1_CLK_ENABLE();
		__HAL_RCC_GPIO_CLK_ENABLE();
		
	//	PB6: UART1_TX
	//	PB7: UART1_RX
		__HAL_AFIO_REMAP_UART1_TX(GPIOB, GPIO_PIN_6);
		__HAL_AFIO_REMAP_UART1_RX(GPIOB, GPIO_PIN_7);
		HAL_NVIC_SetPriority(UART1_IRQn, 0);
		HAL_NVIC_EnableIRQ(UART1_IRQn);
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
	if (huart->Instance == UART1)
	{
		__HAL_RCC_UART1_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);
	}

}