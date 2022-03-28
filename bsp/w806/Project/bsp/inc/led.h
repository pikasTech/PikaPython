/*
 * led.c
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H__
#define __LED_H__

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

extern void MY_LED_Init(void);
extern void Led1_Control(uint8_t sw);
extern void Led2_Control(uint8_t sw);
extern void Led3_Control(uint8_t sw);
extern void Led1_Toggle(void);
extern void Led2_Toggle(void);
extern void Led3_Toggle(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */
#endif /*__ LED_H__ */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/