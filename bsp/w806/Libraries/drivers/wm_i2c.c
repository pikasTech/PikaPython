#include "wm_i2c.h"

static void delay_us(void)
{
	uint32_t i;
	
	for (i = 0; i < (45 * 5); i++)
	{
		__NOP();
	}
}

static void I2C_Start(I2C_HandleTypeDef *hi2c)
{	
	I2C_SCL_H(hi2c);
	I2C_SDA_H(hi2c);
	delay_us();
	
	I2C_SDA_L(hi2c);
	delay_us();
	
	I2C_SCL_L(hi2c);
	delay_us();
}

static void I2C_Stop(I2C_HandleTypeDef *hi2c)
{
	I2C_SDA_L(hi2c);
	I2C_SCL_H(hi2c);
	delay_us();
	
	I2C_SDA_H(hi2c);
	delay_us();
}

static void I2C_Ack(I2C_HandleTypeDef *hi2c)
{	
	I2C_SDA_OUT(hi2c);
	I2C_SDA_L(hi2c);
	I2C_SCL_H(hi2c);
	delay_us();
	
	I2C_SCL_L(hi2c);
	delay_us();
	I2C_SDA_IN(hi2c);
}

static void I2C_Nack(I2C_HandleTypeDef *hi2c)
{
	I2C_SDA_OUT(hi2c);
	I2C_SDA_H(hi2c);
	I2C_SCL_H(hi2c);
	delay_us();
	
	I2C_SCL_L(hi2c);
	delay_us();
	I2C_SDA_IN(hi2c);
}

static uint8_t I2C_Wait_Ack(I2C_HandleTypeDef *hi2c)
{
	uint8_t ack;

	I2C_SDA_H(hi2c);
	delay_us();
	I2C_SDA_IN(hi2c);
	I2C_SCL_H(hi2c);
	delay_us();
	ack = I2C_SDA_GET(hi2c);
	I2C_SCL_L(hi2c);
	delay_us();
	I2C_SDA_OUT(hi2c);

	return ack;
}

static void I2C_Write_Byte(I2C_HandleTypeDef *hi2c, uint8_t data)
{
	int i;
	
	for (i = 0; i < 8; i ++)
	{
		if (data & 0x80)
		{
			I2C_SDA_H(hi2c);
		}
		else
		{
			I2C_SDA_L(hi2c);
		}
		I2C_SCL_H(hi2c);
		delay_us();
		I2C_SCL_L(hi2c);
		delay_us();
		data <<= 1;
	}
}

static uint8_t I2C_Read_Byte(I2C_HandleTypeDef *hi2c)
{
	uint8_t i, temp = 0;
	
	for (i = 0; i < 8; i++)
	{
		I2C_SCL_H(hi2c);
		delay_us();
		temp <<= 1;
		if (I2C_SDA_GET(hi2c))
		{
			temp |= 0x01;
		}
		I2C_SCL_L(hi2c);
		delay_us();
	}

	return temp;
}

HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Pin = hi2c->SCL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(hi2c->SCL_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = hi2c->SDA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(hi2c->SDA_Port, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(hi2c->SCL_Port, hi2c->SCL_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(hi2c->SDA_Port, hi2c->SDA_Pin, GPIO_PIN_SET);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c)
{
	HAL_GPIO_DeInit(hi2c->SCL_Port, hi2c->SCL_Pin);
	HAL_GPIO_DeInit(hi2c->SDA_Port, hi2c->SDA_Pin);
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Write(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size)
{
	uint32_t i, ret = HAL_ERROR;
	
	I2C_Start(hi2c);
	I2C_Write_Byte(hi2c, (DevAddress & 0xFE));
	if (I2C_Wait_Ack(hi2c))
	{
		goto OUT;
	}
	I2C_Write_Byte(hi2c, MemAddress);
	if (I2C_Wait_Ack(hi2c))
	{
		goto OUT;
	}
	for (i = 0; i < Size; i++)
	{
		I2C_Write_Byte(hi2c, pData[i]);
		if (I2C_Wait_Ack(hi2c))
		{
			goto OUT;
		}
	}
	ret = HAL_OK;
OUT:
	I2C_Stop(hi2c);
	return ret;
}

HAL_StatusTypeDef HAL_I2C_Read(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size)
{
	uint32_t i, ret = HAL_ERROR;
	
	I2C_Start(hi2c);
	I2C_Write_Byte(hi2c, (DevAddress & 0xFE));
	if (I2C_Wait_Ack(hi2c))
	{
		goto OUT;
	}
	I2C_Write_Byte(hi2c, MemAddress);
	if (I2C_Wait_Ack(hi2c))
	{
		goto OUT;
	}
	I2C_Start(hi2c);
	I2C_Write_Byte(hi2c, (DevAddress | 0x01));
	if (I2C_Wait_Ack(hi2c))
	{
		goto OUT;
	}
	I2C_SDA_IN(hi2c);
	for (i = 0; i < Size; i++)
	{
		pData[i] = I2C_Read_Byte(hi2c);
		if (i == (Size - 1))
		{
			I2C_Nack(hi2c);
		}
		else
		{
			I2C_Ack(hi2c);
		}
	}
	ret = HAL_OK;
OUT:
	I2C_SDA_OUT(hi2c);
	I2C_Stop(hi2c);
	return ret;
}
