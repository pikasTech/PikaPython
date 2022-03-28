
#include <stdio.h>
#include "wm_hal.h"

void Error_Handler(void);
static void ADC_Init(void);
ADC_HandleTypeDef hadc;

int main(void)
{
	int value;
	double voltage = 0.0;
	
	SystemClock_Config(CPU_CLK_160M);
	printf("enter main\r\n");

	ADC_Init();
	while (1)
	{
		value = HAL_ADC_GET_INPUT_VOLTAGE(&hadc);
		printf("value = %dmv\r\n", value);
		HAL_Delay(1000);
	}
}

static void ADC_Init(void)
{
	hadc.Instance = ADC;
	hadc.Init.channel = ADC_CHANNEL_0;
	hadc.Init.freq = 1000;
	
	if (HAL_ADC_Init(&hadc) != HAL_OK)
	{
		Error_Handler();
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	
}

void Error_Handler(void)
{
	while (1)
	{
	}
}

void assert_failed(uint8_t *file, uint32_t line)
{
	printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}