#include "dataStrs.h"
#include "W801Device_ADC.h"
#include "wm_hal.h"
#include "W801_common.h"


ADC_HandleTypeDef hadc;

enum adc_channel
{
	adc_channel_0 = 0,
	adc_channel_1,
	adc_channel_2,
	adc_channel_3,
	adc_max_channel
};

int adc_value[adc_max_channel] = {0,};
double value[adc_max_channel] = {0.0,};

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


		HAL_NVIC_SetPriority(ADC_IRQn, 0);
		HAL_NVIC_EnableIRQ(ADC_IRQn);
	}
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
	__HAL_RCC_ADC_CLK_DISABLE();
	if (hadc->Init.channel == ADC_CHANNEL_0)
	{
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
	}
	else if (hadc->Init.channel == ADC_CHANNEL_1)
	{
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);
	}
	else if (hadc->Init.channel == ADC_CHANNEL_2)
	{
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);
	}
	else if (hadc->Init.channel == ADC_CHANNEL_3)
	{
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
	}
	HAL_NVIC_DisableIRQ(ADC_IRQn);
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	
}
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */
void Adc_GetValue(ADC_HandleTypeDef* hadc)
{
	
	//ADC_CHANNEL_0 : PA1
	//ADC_CHANNEL_1 : PA4
	//ADC_CHANNEL_2 : PA3
	//ADC_CHANNEL_3 : PA2
	
	if (hadc->Init.channel == ADC_CHANNEL_0)
	{
		adc_value[adc_channel_0] = HAL_ADC_GET_INPUT_VOLTAGE(hadc);
		value[adc_channel_0] = HAL_ADC_GET_INPUT_VOLTAGE(hadc) / 1000.0;
		printf("adc_value[%d] = %dmv\r\n", adc_channel_0 , adc_value[adc_channel_0]);
		printf("value[%d] = %.2fv\r\n", adc_channel_0 , value[adc_channel_0]);
	}
	else if (hadc->Init.channel == ADC_CHANNEL_1)
	{
		adc_value[adc_channel_1] = HAL_ADC_GET_INPUT_VOLTAGE(hadc);
		value[adc_channel_1] = HAL_ADC_GET_INPUT_VOLTAGE(hadc) / 1000.0;
		printf("adc_value[%d] = %dmv\r\n", adc_channel_1 , adc_value[adc_channel_1]);
		printf("value[%d] = %.2fv\r\n", adc_channel_1 , value[adc_channel_1]);
	}
	else if (hadc->Init.channel == ADC_CHANNEL_2)
	{
		adc_value[adc_channel_2] = HAL_ADC_GET_INPUT_VOLTAGE(hadc);
		value[adc_channel_2] = HAL_ADC_GET_INPUT_VOLTAGE(hadc) / 1000.0;
		printf("adc_value[%d] = %dmv\r\n", adc_channel_2 , adc_value[adc_channel_2]);
		printf("value[%d] = %.2fv\r\n", adc_channel_2 , value[adc_channel_2]);
	}
	else if (hadc->Init.channel == ADC_CHANNEL_3)
	{
		adc_value[adc_channel_3] = HAL_ADC_GET_INPUT_VOLTAGE(hadc);
		value[adc_channel_3] = HAL_ADC_GET_INPUT_VOLTAGE(hadc) / 1000.0;
		printf("adc_value[%d] = %dmv\r\n", adc_channel_3 , adc_value[adc_channel_3]);
		printf("value[%d] = %.2fv\r\n", adc_channel_3 , value[adc_channel_3]);
	}
	
}

void W801Device_ADC___init__(PikaObj *self)
{
	W801Device_ADC_init(self);
}

void W801Device_ADC_init(PikaObj *self)
{
	
}

float W801Device_ADC_read(PikaObj* self) {
    obj_run(self, "platformRead()");
    return obj_getFloat(self, "val");
}


void W801Device_ADC_setChannel(PikaObj *self ,int channel)
{
	obj_setInt(self, "channel", channel);
}

void W801Device_ADC_enable(PikaObj *self)
{
	 obj_run(self, "platformEnable()");
}

void W801Device_ADC_platformEnable(PikaObj *self)
{
	int channel=0;
	
	channel=obj_getInt(self,"channel");
	
	hadc.Instance = ADC;
	hadc.Init.channel = channel;
	hadc.Init.freq = 1000;
	
	if (HAL_ADC_Init(&hadc) != HAL_OK)
	{
		Error_Handler();
	}
	
	
}

void W801Device_ADC_platformRead(PikaObj *self)
{
	Adc_GetValue(&hadc);
	
	int channel=obj_getInt(self,"channel");
	
    obj_setFloat(self, "val", value[channel]);
}