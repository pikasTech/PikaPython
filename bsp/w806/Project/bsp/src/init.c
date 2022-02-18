/*
 * init.c
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Includes ------------------------------------------------------------------*/
#include "init.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure MY_BSP_Init                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MY_BSP_Init(void)
{
	APP_INT(MY_KEY);
	APP_INT(MY_LED);
	APP_INT(MY_TIM);
	APP_INT(MY_ADC);
	APP_INT(MY_I2C);
	APP_INT(MY_OLED);
	APP_INT(MY_PWM);
	APP_INT(MY_MOTOR);
	APP_INT(MY_RGB);
	//APP_INT(MY_PMU);
}

/* USER CODE BEGIN 2 */
void MY_APP_Test(void)
{
	APP_TEST(MY_OLED);
	APP_TEST(MY_MOTOR);
}
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/
