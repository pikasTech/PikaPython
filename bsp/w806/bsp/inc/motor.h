/*
 * motor.h
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTOR_H__
#define __MOTOR_H__

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* USER CODE BEGIN Includes */
#include "rgb.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MY_MOTOR_Init(void);

/* USER CODE BEGIN Prototypes */
void Motor_Start(void);
void Motor_Stop(void);
void Motor_Left(void);
void Motor_Right(void);
void MY_MOTOR_Test(void);
/* USER CODE END Prototypes */
#endif /*__ STYLE_H__ */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/