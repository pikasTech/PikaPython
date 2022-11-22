#include <stdint.h>
#include "BaseObj.h"
#include "STM32G0_GPIO.h"
#include "STM32G0_common.h"
#include "dataStrs.h"
#include "pika_hal.h"

typedef struct platform_data_GPIO {
    GPIO_TypeDef* gpioPort;
    uint16_t gpioPin;
    uint32_t pinMode;
    uint32_t gpioPull;
} platform_data_GPIO;

int pika_hal_platform_GPIO_open(pika_dev* dev, char* name) {
    __platform_printf("Open: %s \r\n", name);
    GPIO_TypeDef* gpioPort = GPIO_get_Group(name);
    if (NULL == gpioPort) {
        __platform_printf("Error: get GPIO group failed.\r\n");
        return -1;
    }
    uint16_t gpioPin = GPIO_get_pin(name);
    if (0 == gpioPin) {
        __platform_printf("Error: get GPIO Pin failed.\r\n");
        return -1;
    }
    platform_data_GPIO* data = pikaMalloc(sizeof(platform_data_GPIO));
    data->gpioPin = gpioPin;
    data->gpioPort = gpioPort;
    dev->platform_data = data;
    return 0;
}

int pika_hal_platform_GPIO_close(pika_dev* dev) {
    if (NULL != dev->platform_data) {
        pikaFree(dev->platform_data, sizeof(platform_data_GPIO));
        dev->platform_data = NULL;
    }
    return 0;
}

int pika_hal_platform_GPIO_read(pika_dev* dev, void* buf, size_t count) {
    platform_data_GPIO* data = dev->platform_data;
    uint32_t level = LL_GPIO_IsInputPinSet(data->gpioPort, data->gpioPin);
    if (level != 1 && level != 0) {
        return -1;
    }
    memcpy(buf, &level, count);
    return 0;
}

int pika_hal_platform_GPIO_write(pika_dev* dev, void* buf, size_t count) {
    platform_data_GPIO* data = dev->platform_data;
    uint32_t level = 0;
    memcpy(&level, buf, count);
    if (level == 0) {
        LL_GPIO_ResetOutputPin(data->gpioPort, data->gpioPin);
        return 0;
    }
    if (level == 1) {
        LL_GPIO_SetOutputPin(data->gpioPort, data->gpioPin);
        return 0;
    }
    return -1;
}

static int _enable_gpio_clk(GPIO_TypeDef* gpio_group) {
    if (gpio_group == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        return 0;
    }
    if (gpio_group == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        return 0;
    }
    if (gpio_group == GPIOC) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        return 0;
    }
    return -1;
}

int pika_hal_platform_GPIO_ioctl_enable(pika_dev* dev) {
    platform_data_GPIO* data = dev->platform_data;

    if (0 != _enable_gpio_clk(data->gpioPort)) {
        return -1;
    }

    /*Configure GPIO pin Output Level */
    LL_GPIO_ResetOutputPin(data->gpioPort, data->gpioPin);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO*/
    GPIO_InitStruct.Pin = data->gpioPin;
    GPIO_InitStruct.Mode = data->pinMode;
    GPIO_InitStruct.Pull = data->gpioPull;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(data->gpioPort, &GPIO_InitStruct);
    return 0;
}

int pika_hal_platform_GPIO_ioctl_disable(pika_dev* dev) {
    platform_data_GPIO* data = dev->platform_data;
    LL_GPIO_DeInit(data->gpioPort);
    return 0;
}

int pika_hal_platform_GPIO_ioctl_config(pika_dev* dev,
                                        pika_hal_GPIO_config* cfg) {
    platform_data_GPIO* data = dev->platform_data;
    switch (cfg->dir) {
        case PIKA_HAL_GPIO_DIR_IN:
            data->pinMode = LL_GPIO_MODE_INPUT;
        case PIKA_HAL_GPIO_DIR_OUT:
            data->pinMode = LL_GPIO_MODE_OUTPUT;
        default:
            data->pinMode = LL_GPIO_MODE_OUTPUT;
    }
    switch (cfg->pull) {
        case PIKA_HAL_GPIO_PULL_UP:
            data->gpioPull = LL_GPIO_PULL_UP;
        case PIKA_HAL_GPIO_PULL_DOWN:
            data->gpioPull = LL_GPIO_PULL_DOWN;
        default:
            data->gpioPull = LL_GPIO_PULL_NO;
    }
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO*/
    GPIO_InitStruct.Pin = data->gpioPin;
    GPIO_InitStruct.Mode = data->pinMode;
    GPIO_InitStruct.Pull = data->gpioPull;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(data->gpioPort, &GPIO_InitStruct);
    return 0;
}

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
