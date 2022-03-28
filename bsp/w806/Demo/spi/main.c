
#include <stdio.h>
#include "wm_hal.h"

void Error_Handler(void);

SPI_HandleTypeDef hspi;
static void SPI_Init(void);
#define data_len (20*1024)
uint8_t tx_data[data_len] = {0};
uint8_t rx_data[data_len] = {0};

int main(void)
{
	int i = 0;
	
	SystemClock_Config(CPU_CLK_160M);
	printf("enter main\r\n");
	SPI_Init();
	
	for(i = 0; i < data_len; i++)
	{
		tx_data[i] = i;
	}
	while (1)
	{
		memset(rx_data, 0, data_len);
		__HAL_SPI_SET_CS_LOW(&hspi);
	//	HAL_SPI_Transmit(&hspi, tx_data, data_len, 1000);
	//	HAL_SPI_Receive(&hspi, rx_data, data_len, 1000);
		HAL_SPI_TransmitReceive(&hspi, tx_data, rx_data, data_len, 1000);
		__HAL_SPI_SET_CS_HIGH(&hspi);
		for (i = 0; i < data_len; i++)
		{
			if (rx_data[i] != tx_data[i])
			{
				printf("err\r\n");
				break;
			}
		}
		HAL_Delay(1000);
	}
}

static void SPI_Init(void)
{
	hspi.Instance = SPI;
	hspi.Init.Mode = SPI_MODE_MASTER;
	hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi.Init.NSS = SPI_NSS_SOFT;
	hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_20;
	hspi.Init.FirstByte = SPI_LITTLEENDIAN;
	
	if (HAL_SPI_Init(&hspi) != HAL_OK)
	{
		Error_Handler();
	}
}

void Error_Handler(void)
{
	while (1)
	{
	}
}

void assert_failed(uint8_t *file, uint32_t line)
{
	printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}