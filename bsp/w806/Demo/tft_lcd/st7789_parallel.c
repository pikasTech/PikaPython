#include "st7789_parallel.h"

void P_Back_On(void)
{
	HAL_GPIO_WritePin(P_LEDA_PORT, P_LEDA_PIN, GPIO_PIN_SET);
}

void P_Back_Off(void)
{
	HAL_GPIO_WritePin(P_LEDA_PORT, P_LEDA_PIN, GPIO_PIN_RESET);
}

static void P_WaitTe(void)
{
	while ((P_FMARK_PORT->DATA & P_FMARK_PIN) == 0);
}

void P_WriteReg(uint8_t reg)
{
	P_CS_LOW;
	P_CD_LOW;
	P_WR_LOW;
	MODIFY_REG(P_DATA_PORT->DATA, P_DATA_PIN, reg);
	P_WR_HIGH;
	P_CD_HIGH;
	P_CS_HIGH;
}

void P_WriteData8(uint8_t data)
{
	P_CS_LOW;
	P_WR_LOW;
	MODIFY_REG(P_DATA_PORT->DATA, P_DATA_PIN, data);
	P_WR_HIGH;
	P_CS_HIGH;
}

void P_WriteData16(uint16_t data)
{
	P_CS_LOW;
	P_WR_LOW;
	MODIFY_REG(P_DATA_PORT->DATA, P_DATA_PIN, (data >> 8));
	P_WR_HIGH;
	P_WR_LOW;
	MODIFY_REG(P_DATA_PORT->DATA, P_DATA_PIN, (data & 0x00FF));
	P_WR_HIGH;
	P_CS_HIGH;
}

void P_WriteData(uint8_t *data, uint32_t len)
{
	int i = 0;
	
	P_CS_LOW;
	P_DATA_PORT->DATA_B_EN &= P_DATA_PIN;
	P_WR_PORT->DATA_B_EN &= P_WR_PIN;
	for (i = 0; i < len; i ++)
	{
		P_WR_PORT->DATA = 0;
		P_DATA_PORT->DATA = data[i];
		P_WR_PORT->DATA = P_WR_PIN;
	}
	P_WR_PORT->DATA_B_EN |= ~P_WR_PIN;
	P_DATA_PORT->DATA_B_EN |= ~P_DATA_PIN;
	P_CS_HIGH;
}
