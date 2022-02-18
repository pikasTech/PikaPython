/*
 * pmu.c
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PMU_H__
#define __PMU_H__

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

extern void MY_PMU_Init(void);
extern void PMU_Sleep_Tim0(void);
extern void PMU_Sleep_IO(void);
extern void PMU_Standby_Tim0(void);
extern void PMU_Standby_IO(void);

/* USER CODE BEGIN Prototypes */
extern PMU_HandleTypeDef hpmu;
/* USER CODE END Prototypes */
#endif /*__ PMU_H__ */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/