/*
 * rgb.h
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RGB_H__
#define __RGB_H__

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MY_RGB_Init(void);

/* USER CODE BEGIN Prototypes */
void R_LED(uint8_t sw);
void G_LED(uint8_t sw);
void B_LED(uint8_t sw);
/* USER CODE END Prototypes */
#endif /*__ RGB_H__ */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/