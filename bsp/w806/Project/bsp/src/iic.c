/*
 * iic.c
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Includes ------------------------------------------------------------------*/
#include "iic.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure IIC                                                            */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
I2C_HandleTypeDef hi2c;
/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_IIC_Init(void)
{
	hi2c.SCL_Port = GPIOA;
	hi2c.SCL_Pin = GPIO_PIN_1;
	hi2c.SDA_Port = GPIOA;
	hi2c.SDA_Pin = GPIO_PIN_4;

	HAL_I2C_Init(&hi2c);
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/
