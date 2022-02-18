#include "wm_hal.h"
#include "lcd.h"

void HAL_MspInit(void)
{

}

#if ST7789_SPI
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
	__HAL_RCC_SPI_CLK_ENABLE();
	__HAL_AFIO_REMAP_SPI_CS(S_CS_PORT, S_CS_PIN);
	__HAL_AFIO_REMAP_SPI_CLK(S_SCL_PORT, S_SCL_PIN);
	__HAL_AFIO_REMAP_SPI_MOSI(S_SDA_PORT, S_SDA_PIN);
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
	__HAL_RCC_SPI_CLK_DISABLE();
	HAL_GPIO_DeInit(S_CS_PORT, S_CS_PIN);
	HAL_GPIO_DeInit(S_SCL_PORT, S_SCL_PIN);
	HAL_GPIO_DeInit(S_SDA_PORT, S_SDA_PIN);
}
#endif