/*
 * pwm.c
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Includes ------------------------------------------------------------------*/
#include "pwm.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure PWM                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
PWM_HandleTypeDef hpwm;
uint32_t ch = PWM_CHANNEL_0;
/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MY_PWM_Init(void)
{
	/* 输出波形的频率： f = 40MHz / Prescaler / (Period + 1)；
	* 输出波形的占空比： 
	*     沿对齐模式（递减）：(Pulse + 1) / (Period + 1)
	*                         Pulse >= Period：PWM输出一直为高电平
	*                         Pulse < Period ：PWM输出高电平宽度为(Pulse + 1)，低电平宽度为(Period - Pulse)
	*                         Pulse = 0      ：PWM输出高电平宽度为(1)，低电平宽度为(Period)
	* 
	*    中间对齐模式       ：(2 * Pulse + 1) / (2 * (Period + 1))
	*                         Pulse > Period ：PWM输出一直为高电平
	*                         Pulse <= Period：PWM输出高电平宽度为(2 * Pulse + 1)，低电平宽度为(2 * (Period - Pulse) + 1)
	*                         Pulse = 0      ：PWM输出高电平宽度为(1)，低电平宽度为(2 * Period + 1)
	*/
	//输出200KHZ,占空比40%的波形
	hpwm.Instance = PWM;
	hpwm.Init.AutoReloadPreload = PWM_AUTORELOAD_PRELOAD_ENABLE;
	hpwm.Init.CounterMode = PWM_COUNTERMODE_EDGEALIGNED_DOWN;
	hpwm.Init.Prescaler = 4;
	hpwm.Init.Period = 2000 -1;	// 40M / 4 / 1000 = 10KHZ
	hpwm.Init.Pulse = 50 -1;	// 50% DUTY
	hpwm.Init.OutMode = PWM_OUT_MODE_5SYNC;
	hpwm.Channel = ch;
	
	HAL_PWM_Init(&hpwm);
	
	HAL_PWM_Start(&hpwm, ch);
	HAL_PWM_Duty_Set(&hpwm, ch, 50);
}

/* USER CODE BEGIN 2 */
void HAL_PWM_MspInit(PWM_HandleTypeDef *hpwm)
{
	__HAL_RCC_PWM_CLK_ENABLE();
	__HAL_AFIO_REMAP_PWM0(GPIOB, GPIO_PIN_12);
}

void HAL_PWM_MspDeInit(PWM_HandleTypeDef *hpwm)
{
	__HAL_RCC_PWM_CLK_DISABLE();
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12);
}
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/
