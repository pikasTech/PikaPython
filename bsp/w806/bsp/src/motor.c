/*
 * motor.c
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Includes ------------------------------------------------------------------*/
#include "motor.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure MOTOR                                                            */
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
void MY_MOTOR_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIO_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_13 | GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	Motor_Stop();
}

/* USER CODE BEGIN 2 */
//启动
void Motor_Start(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);    //1- ON
}

  //停止
void Motor_Stop(void)
{
	R_LED(ON);
	G_LED(OFF);
	B_LED(OFF);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);    //0- OFF
}

//左转
void Motor_Left(void)
{
	Motor_Start();
	R_LED(OFF);
	G_LED(ON);
	B_LED(OFF);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);

}

//右转
void Motor_Right(void)
{
	Motor_Start();
	R_LED(OFF);
	G_LED(OFF);
	B_LED(ON);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

}
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */
void MY_MOTOR_Test(void)
{
	Motor_Right();
}
/* USER CODE END 3 */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/
