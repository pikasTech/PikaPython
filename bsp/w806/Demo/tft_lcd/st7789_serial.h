#ifndef __ST7789_SERIAL_H__
#define __ST7789_SERIAL_H__

#include "wm_hal.h"

#define S_LEDA1_PORT	GPIOB
#define S_LEDA1_PIN		GPIO_PIN_25
#define S_LEDA2_PORT	GPIOB
#define S_LEDA2_PIN		GPIO_PIN_26
#define S_FMARK_PORT	GPIOB
#define S_FMARK_PIN		GPIO_PIN_18
#define S_SDA_PORT		GPIOB
#define S_SDA_PIN		GPIO_PIN_17
#define S_CD_PORT		GPIOB
#define S_CD_PIN		GPIO_PIN_16
#define S_SCL_PORT		GPIOB
#define S_SCL_PIN		GPIO_PIN_15
#define S_CS_PORT		GPIOB
#define S_CS_PIN		GPIO_PIN_14
#define S_RESET_PORT	GPIOB
#define S_RESET_PIN		GPIO_PIN_13

extern SPI_HandleTypeDef hspi;

#define S_CD_LOW		HAL_GPIO_WritePin(S_CD_PORT, S_CD_PIN, GPIO_PIN_RESET)
#define S_CD_HIGH		HAL_GPIO_WritePin(S_CD_PORT, S_CD_PIN, GPIO_PIN_SET)
#define S_CS_LOW		__HAL_SPI_SET_CS_LOW(&hspi)
#define S_CS_HIGH		__HAL_SPI_SET_CS_HIGH(&hspi)
#define S_RESET_LOW		HAL_GPIO_WritePin(S_RESET_PORT, S_RESET_PIN, GPIO_PIN_RESET)
#define S_RESET_HIGH	HAL_GPIO_WritePin(S_RESET_PORT, S_RESET_PIN, GPIO_PIN_SET)

void S_Back_On(void);
void S_Back_Off(void);
void S_WriteReg(uint8_t reg);
void S_WriteData8(uint8_t data);
void S_WriteData16(uint16_t data);
void S_WriteData(uint8_t *data, uint32_t len);

#endif
