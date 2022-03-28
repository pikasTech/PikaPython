/*
 * i2c.c
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure I2C                                                            */
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
void MY_I2C_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIO_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_SET);
}

/* USER CODE BEGIN 2 */
void I2C_Start(void)
{

	OLED_SCLK_Set() ;
	OLED_SDIN_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}

void I2C_Stop(void)
{
	OLED_SCLK_Set() ;
//	OLED_SCLK_Clr();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
}

void I2C_Wait_Ack()
{
	OLED_SCLK_Set() ;
	OLED_SCLK_Clr();
}

void Write_I2C_Byte(unsigned char I2C_Byte)
{
	unsigned char i;
	unsigned char m,da;
	da = I2C_Byte;
	OLED_SCLK_Clr();
	for(i=0;i<8;i++)
	{
		m=da;
		//	OLED_SCLK_Clr();
		m=m&0x80;
		if(m==0x80)
		{OLED_SDIN_Set();}
		else OLED_SDIN_Clr();
		da=da<<1;
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
	}
}

void Write_I2C_Command(unsigned char I2C_Command)
{
	I2C_Start();
	Write_I2C_Byte(0x78);            //Slave address,SA0=0
	I2C_Wait_Ack();
	Write_I2C_Byte(0x00);			//write command
	I2C_Wait_Ack();
	Write_I2C_Byte(I2C_Command);
	I2C_Wait_Ack();
	I2C_Stop();
}

void Write_I2C_Data(unsigned char I2C_Data)
{
	I2C_Start();
	Write_I2C_Byte(0x78);			//D/C#=0; R/W#=0
	I2C_Wait_Ack();
	Write_I2C_Byte(0x40);			//write data
	I2C_Wait_Ack();
	Write_I2C_Byte(I2C_Data);
	I2C_Wait_Ack();
	I2C_Stop();
}

/* USER CODE END 2 */

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/
