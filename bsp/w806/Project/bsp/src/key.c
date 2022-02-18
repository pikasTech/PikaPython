/*
 * key.c
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Includes ------------------------------------------------------------------*/
#include "key.h"
#include "led.h"
#include "delay.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure KEY                                                              */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MY_KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIO_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(GPIOB_IRQn, 0);
	HAL_NVIC_EnableIRQ(GPIOB_IRQn);
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/* USER CODE BEGIN 3 */
void HAL_GPIO_EXTI_Callback(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
	if ((GPIOx == GPIOB) && (GPIO_Pin == GPIO_PIN_5))
	{
		delay_ms(20);
		if ((GPIOx == GPIOB) && (GPIO_Pin == GPIO_PIN_5))
		{
			Led1_Toggle();
		}
	}
}
/* USER CODE END 3 */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/
