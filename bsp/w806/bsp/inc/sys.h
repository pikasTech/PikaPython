/*
 * sys.c
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYS_H__
#define __SYS_H__

/* Includes ------------------------------------------------------------------*/
#include "wm_hal.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define ON 	1
#define OFF	0
/* USER CODE END Private defines */

extern void Error_Handler(void);
extern void assert_failed(uint8_t *file, uint32_t line);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */
#endif /*__ SYS_H__ */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/