#ifndef __ST7789_PARALLEL_H__
#define __ST7789_PARALLEL_H__

#include "wm_hal.h"

#define P_LEDA_PORT		GPIOA
#define P_LEDA_PIN		GPIO_PIN_5
#define P_RD_PORT		GPIOA
#define P_RD_PIN		GPIO_PIN_6
#define P_WR_PORT		GPIOA
#define P_WR_PIN		GPIO_PIN_7
#define P_CD_PORT		GPIOA
#define P_CD_PIN		GPIO_PIN_8
#define P_CS_PORT		GPIOA
#define P_CS_PIN		GPIO_PIN_9
#define P_FMARK_PORT	GPIOA
#define P_FMARK_PIN		GPIO_PIN_12
#define P_RESET_PORT	GPIOA
#define P_RESET_PIN		GPIO_PIN_13
#define P_DATA_PORT		GPIOB
#define P_DATA_PIN		0xFF

#define P_CD_LOW		P_CD_PORT->DATA &= ~P_CD_PIN
#define P_CD_HIGH		P_CD_PORT->DATA |= P_CD_PIN
#define P_CS_LOW		P_CS_PORT->DATA &= ~P_CS_PIN
#define P_CS_HIGH		P_CS_PORT->DATA |= P_CS_PIN
#define P_RESET_LOW		P_RESET_PORT->DATA &= ~P_RESET_PIN
#define P_RESET_HIGH	P_RESET_PORT->DATA |= P_RESET_PIN
#define P_WR_LOW		P_WR_PORT->DATA &= ~P_WR_PIN
#define P_WR_HIGH		P_WR_PORT->DATA |= P_WR_PIN
#define P_RD_HIGH		P_RD_PORT->DATA |= P_RD_PIN

void P_Back_On(void);
void P_Back_Off(void);
void P_WriteReg(uint8_t reg);
void P_WriteData8(uint8_t data);
void P_WriteData16(uint16_t data);
void P_WriteData(uint8_t *data, uint32_t len);

#endif