#include "st7789_serial.h"

void S_Back_On(void)
{
	HAL_GPIO_WritePin(S_LEDA1_PORT, S_LEDA1_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(S_LEDA2_PORT, S_LEDA2_PIN, GPIO_PIN_SET);
}

void S_Back_Off(void)
{
	HAL_GPIO_WritePin(S_LEDA1_PORT, S_LEDA1_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(S_LEDA2_PORT, S_LEDA2_PIN, GPIO_PIN_RESET);
}

static void S_WaitTe(void)
{
	while ((S_FMARK_PORT->DATA & S_FMARK_PIN) == 0);
}

void S_WriteReg(uint8_t reg)
{
	S_CD_LOW;
	S_CS_LOW;
	HAL_SPI_Transmit(&hspi, &reg, 1, 100);
	S_CS_HIGH;
	S_CD_HIGH;
}

void S_WriteData8(uint8_t data)
{
	S_CS_LOW;
	HAL_SPI_Transmit(&hspi, &data, 1, 100);
	S_CS_HIGH;
}

void S_WriteData16(uint16_t data)
{
	uint8_t temp[2];
	
	temp[0] = data >> 8;
	temp[1] = data;
	S_CS_LOW;
	HAL_SPI_Transmit(&hspi, temp, 2, 100);
	S_CS_HIGH;
}

#define DMA_Channel0		((DMA_Channel_TypeDef *)DMA_Channel0_BASE)
void HAL_SPI_Transmit_dma(SPI_HandleTypeDef *hspi, uint8_t *pData, uint32_t Size)
{
	uint32_t block_cnt = 0, tx_block_cnt = 0, tx_size = 0;
	
	__HAL_RCC_DMA_CLK_ENABLE();
	block_cnt = Size / BLOCK_SIZE;
	while (tx_block_cnt <= block_cnt)
	{
		if (tx_block_cnt < block_cnt)
		{
			tx_size = BLOCK_SIZE;
		}
		else
		{
			tx_size = Size % BLOCK_SIZE;
		}
		__HAL_SPI_CLEAR_FIFO(hspi);
		
		DMA_Channel0->MODE = 0x15;
		DMA_Channel0->SA = (uint32_t)(pData + (BLOCK_SIZE * tx_block_cnt));
		DMA_Channel0->DA = (uint32_t)(&(hspi->Instance->TXDATA));
		DMA_Channel0->CR2 = ((tx_size << 8) | 0x42);
		hspi->Instance->MODE_CFG = 0x01;
		
		__HAL_SPI_SET_CLK_NUM(hspi, tx_size * 8);
		DMA_Channel0->CR1 |= 0x1;
		__HAL_SPI_SET_START(hspi);
		while (DMA_Channel0->CR1 & 0x01);
		while (hspi->Instance->STATUS & (1 << 12));
		__HAL_SPI_CELAR_FLAG(hspi, 0xFF);
		*((uint32_t *)(0x40000804)) |= 0x03;
		tx_block_cnt++;
	}
}

void S_WriteData(uint8_t *data, uint32_t len)
{
	S_CS_LOW;
	HAL_SPI_Transmit(&hspi, data, len, 1000);
//	HAL_SPI_Transmit_dma(&hspi, data, len);
	S_CS_HIGH;
}