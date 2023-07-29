#include <stdint.h>
#include "BaseObj.h"
#include "STM32G0_common.h"
#include "dataStrs.h"

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
            break;
        case PIKA_HAL_GPIO_DIR_OUT:
            data->pinMode = LL_GPIO_MODE_OUTPUT;
            break;
        default:
            data->pinMode = LL_GPIO_MODE_OUTPUT;
    }
    switch (cfg->pull) {
        case PIKA_HAL_GPIO_PULL_UP:
            data->gpioPull = LL_GPIO_PULL_UP;
            break;
        case PIKA_HAL_GPIO_PULL_DOWN:
            data->gpioPull = LL_GPIO_PULL_DOWN;
            break;
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

