/*
 * tim.c
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H__
#define __TIM_H__

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

extern void MY_TIM_Init(void);

/* USER CODE BEGIN Prototypes */
extern TIM_HandleTypeDef htim0;
extern TIM_HandleTypeDef htim1;
/* USER CODE END Prototypes */
#endif /*__ TIM_H__ */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/