#ifndef __WM_I2C_H__
#define __WM_I2C_H__

#include "wm_hal.h"

typedef struct
{
	GPIO_TypeDef *SCL_Port;
	uint32_t SCL_Pin;
	GPIO_TypeDef *SDA_Port;
	uint32_t SDA_Pin;
	uint8_t deviceAddr;
	uint8_t readBuffer[128];
} I2C_HandleTypeDef;

#define I2C_SDA_H(HANDLE)	HAL_GPIO_WritePin(HANDLE->SDA_Port, HANDLE->SDA_Pin, GPIO_PIN_SET)

#define I2C_SDA_L(HANDLE)	HAL_GPIO_WritePin(HANDLE->SDA_Port, HANDLE->SDA_Pin, GPIO_PIN_RESET)

#define I2C_SCL_H(HANDLE)	HAL_GPIO_WritePin(HANDLE->SCL_Port, HANDLE->SCL_Pin, GPIO_PIN_SET)

#define I2C_SCL_L(HANDLE)	HAL_GPIO_WritePin(HANDLE->SCL_Port, HANDLE->SCL_Pin, GPIO_PIN_RESET)

#define I2C_SDA_OUT(HANDLE)	SET_BIT(HANDLE->SDA_Port->DIR, HANDLE->SDA_Pin)

#define I2C_SDA_IN(HANDLE)	CLEAR_BIT(HANDLE->SDA_Port->DIR, HANDLE->SDA_Pin)

#define I2C_SDA_GET(HANDLE)	HAL_GPIO_ReadPin(HANDLE->SDA_Port, HANDLE->SDA_Pin)

HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef HAL_I2C_Write(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Read(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size);

#endif