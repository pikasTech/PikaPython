/*
 * adc.h
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H__
#define __ADC_H__

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
enum channel
{
	adc_channel_0 = 0,
	adc_channel_1,
	adc_channel_2,
	adc_channel_3,
	adc_max_channel
};

extern ADC_HandleTypeDef hadc;
/* USER CODE END Private defines */

extern void MY_ADC_Init(void);
extern void Adc_GetValue(ADC_HandleTypeDef* hadc);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */
#endif /*__ ADC_H__ */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/