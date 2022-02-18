/*
 * led.c
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Includes ------------------------------------------------------------------*/
#include "led.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure LED                                                            */
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
void MY_LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIO_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_SET);
}

/* USER CODE BEGIN 2 */
void Led1_Control(uint8_t sw)
{
	if(sw == ON)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	}
}

void Led2_Control(uint8_t sw)
{
	if(sw == ON)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	}
}

void Led3_Control(uint8_t sw)
{
	if(sw == ON)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
	}
}

void Led1_Toggle(void)
{
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
}

void Led2_Toggle(void)
{
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
}

void Led3_Toggle(void)
{
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
}
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/
