/*
 * i2c.c
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IIC_H__
#define __IIC_H__

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)//SCL
#define OLED_SCLK_Set() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET)//SDA
#define OLED_SDIN_Set() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET)


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
/* USER CODE END Private defines */

void MY_I2C_Init(void);

/* USER CODE BEGIN Prototypes */
void I2C_Start(void);
void I2C_Stop(void);
void Write_I2C_Command(unsigned char I2C_Command);
void Write_I2C_Data(unsigned char I2C_Data);
void Write_I2C_Byte(unsigned char I2C_Byte);
void I2C_Wait_Ack(void);
/* USER CODE END Prototypes */
#endif /*__ I2C_H__ */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/