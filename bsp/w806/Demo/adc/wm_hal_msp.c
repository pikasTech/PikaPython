#include "wm_hal.h"

void HAL_MspInit(void)
{

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance == ADC)
	{
		__HAL_RCC_ADC_CLK_ENABLE();
		__HAL_RCC_GPIO_CLK_ENABLE();
		
		//ADC_CHANNEL_0 : PA1
		//ADC_CHANNEL_1 : PA4
		//ADC_CHANNEL_2 : PA3
		//ADC_CHANNEL_3 : PA2
		//ADC_CHANNEL_0_1 : PA1 and PA4
		//ADC_CHANNEL_2_3 : PA3 and PA2
		if (hadc->Init.channel == ADC_CHANNEL_0)
		{
			__HAL_AFIO_REMAP_ADC(GPIOA, GPIO_PIN_1);
		}
		else if (hadc->Init.channel == ADC_CHANNEL_1)
		{
			__HAL_AFIO_REMAP_ADC(GPIOA, GPIO_PIN_4);
		}
		else if (hadc->Init.channel == ADC_CHANNEL_2)
		{
			__HAL_AFIO_REMAP_ADC(GPIOA, GPIO_PIN_3);
		}
		else if (hadc->Init.channel == ADC_CHANNEL_3)
		{
			__HAL_AFIO_REMAP_ADC(GPIOA, GPIO_PIN_2);
		}
		else if (hadc->Init.channel == ADC_CHANNEL_0_1)
		{
			__HAL_AFIO_REMAP_ADC(GPIOA, GPIO_PIN_1);
			__HAL_AFIO_REMAP_ADC(GPIOA, GPIO_PIN_4);
		}
		else if (hadc->Init.channel == ADC_CHANNEL_2_3)
		{
			__HAL_AFIO_REMAP_ADC(GPIOA, GPIO_PIN_3);
			__HAL_AFIO_REMAP_ADC(GPIOA, GPIO_PIN_2);
		}
		
		// 如果用到中断方式需要使能中断
		HAL_NVIC_SetPriority(ADC_IRQn, 0);
		HAL_NVIC_EnableIRQ(ADC_IRQn);
	}
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
	__HAL_RCC_ADC_CLK_DISABLE();
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
	HAL_NVIC_DisableIRQ(ADC_IRQn);
}

