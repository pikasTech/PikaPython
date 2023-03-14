#include <stdint.h>
#include "BaseObj.h"
#include "STM32G0_common.h"
#include "dataStrs.h"

typedef struct platform_data_SPI {
    SPI_HandleTypeDef* hspi;
    pika_dev* cs;
} platform_data_SPI;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

platform_data_SPI platform_data_SPI_spi1 = {
    .hspi = &hspi1,
};

int pika_hal_platform_SPI_open(pika_dev* dev, char* name) {
    if (strEqu(name, "spi1")) {
        dev->platform_data = &platform_data_SPI_spi1;
        return 0;
    }
    return -1;
}

int pika_hal_platform_SPI_ioctl_config(pika_dev* dev,
                                       pika_hal_SPI_config* cfg) {
    return -1;
}

#define SPI_DMA

void MX_SPI1_Init(void) {
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_1LINE;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
    hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    HAL_SPI_Init(&hspi1);
}

static void MX_DMA_Init(void) {
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Channel1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    hdma_spi1_tx.Instance = DMA1_Channel1;
    hdma_spi1_tx.Init.Request = DMA_REQUEST_SPI1_TX;
    hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi1_tx.Init.Mode = DMA_NORMAL;
    hdma_spi1_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    HAL_DMA_Init(&hdma_spi1_tx);
    __HAL_LINKDMA(hspi, hdmatx, hdma_spi1_tx);
}

void DMA1_Channel1_IRQHandler(void) {
    HAL_DMA_IRQHandler(&hdma_spi1_tx);
}

int pika_hal_platform_SPI_ioctl_enable(pika_dev* dev) {
    platform_data_SPI* data = dev->platform_data;
    /* init GPIO CS */
    data->cs = pika_hal_open(PIKA_HAL_GPIO, "PB1");
    pika_hal_GPIO_config cfg = {
        .dir = PIKA_HAL_GPIO_DIR_OUT,
        .pull = PIKA_HAL_GPIO_PULL_NONE,
    };
    pika_hal_ioctl(data->cs, PIKA_HAL_IOCTL_CONFIG, &cfg);
    uint32_t val = 0;
    pika_hal_write(data->cs, &val, sizeof(val));
    /* init SPI */
#ifndef SPI_SOFT
    MX_DMA_Init();
    MX_SPI1_Init();
#endif
    return 0;
}

int pika_hal_platform_SPI_read(pika_dev* dev, void* buf, size_t count) {
    return -1;
}

int pika_hal_platform_SPI_write(pika_dev* dev, void* buf, size_t count) {
#ifdef SPI_SOFT
    for (int i = 0; i < size; i++) {
        SPI_Write_u8(pData[i]);
    }
#else
#ifdef SPI_DMA
    /* wait spi ready */
    while (hspi1.hdmatx->State != HAL_DMA_STATE_READY ||
           hspi1.State != HAL_SPI_STATE_READY) {
    };
    HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)buf, count);
    /* wait spi ready */
    while (hspi1.hdmatx->State != HAL_DMA_STATE_READY ||
           hspi1.State != HAL_SPI_STATE_READY) {
    };
#else
    HAL_SPI_Transmit(&hspi1, pData, size, HAL_MAX_DELAY);
#endif
#endif
    return 0;
}

int pika_hal_platform_SPI_ioctl_disable(pika_dev* dev) {
    platform_data_SPI* data = dev->platform_data;
    uint32_t val = 1;
    pika_hal_write(data->cs, &val, sizeof(val));
    return 0;
}

int pika_hal_platform_SPI_close(pika_dev* dev) {
    platform_data_SPI* data = dev->platform_data;
    pika_hal_close(data->cs);
    return 0;
}
