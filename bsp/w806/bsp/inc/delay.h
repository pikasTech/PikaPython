/*
 * delay.h
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DELAY_H__
#define __DELAY_H__

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

extern void delay_us(uint32_t nus);
extern void delay_ms(uint16_t nms);
extern void HAL_Delayms(uint32_t ms);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */
#endif /*__ DELAY_H__ */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/