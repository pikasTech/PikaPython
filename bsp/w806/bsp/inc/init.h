/*
 * flash.h
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INIT_H__
#define __INIT_H__

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* USER CODE BEGIN Includes */
#include "led.h"
#include "key.h"
#include "delay.h"
#include "tim.h"
#include "adc.h"
#include "flash.h"
#include "oled.h"
#include "i2c.h"
#include "pwm.h"
#include "motor.h"
#include "freertos.h"
#include "mytask.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define APP_INT(name)	name##_Init()
#define APP_TEST(name)	name##_Test()
/* USER CODE END Private defines */

extern void MY_BSP_Init(void);
extern void MY_APP_Test(void);
/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */
#endif /*__ INIT_H__ */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/