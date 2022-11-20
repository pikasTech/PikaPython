#include "STM32G0_GPIO.h"
#include <stdint.h>
#include "BaseObj.h"
#include "STM32G0_common.h"
#include "dataStrs.h"
#include "pika_hal.h"

typedef struct pika_GPIO_data_STM32G0{
    GPIO_TypeDef* gpioPort;
    uint16_t gpioPin;
    uint32_t pinMode;
    uint32_t gpioPull;
}pika_GPIO_data_STM32G0;

int pika_GPIO_open(pika_dev* dev, char* name){
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
    pika_GPIO_data_STM32G0 *data = pikaMalloc(sizeof(pika_GPIO_data_STM32G0));
    data->gpioPin = gpioPin;
    data->gpioPort = gpioPort;
    dev->user_data = data;   
    return 0;
}

int pika_GPIO_close(pika_dev* dev){
    if(NULL != dev->user_data){
        pikaFree(dev->user_data, sizeof(pika_GPIO_data_STM32G0));
        dev->user_data = NULL;
    }
    return 0;
}

int pika_GPIO_read(pika_dev* dev, uint8_t* buf, size_t count){
    pika_GPIO_data_STM32G0 *data = dev->user_data;
    uint32_t level = LL_GPIO_IsInputPinSet(data->gpioPort, data->gpioPin);
    if(level != 1 && level !=0){
        return -1;
    }
    memcpy(buf, &level, count);
    return 0;
}

int pika_GPIO_write(pika_dev* dev, uint8_t* buf, size_t count){
    pika_GPIO_data_STM32G0 *data = dev->user_data;
    uint32_t level = 0;
    memcpy(&level, buf, count);
    if(level == 0){
        LL_GPIO_ResetOutputPin(data->gpioPort, data->gpioPin);
        return 0;
    }
    if(level == 1){
        LL_GPIO_SetOutputPin(data->gpioPort, data->gpioPin);
        return 0;
    }
    return -1;
}

static int _enable_gpio_clk(GPIO_TypeDef* gpio_group){
    if(gpio_group == GPIOA){
        __HAL_RCC_GPIOA_CLK_ENABLE();
        return 0;
    }
    if(gpio_group == GPIOB){
        __HAL_RCC_GPIOB_CLK_ENABLE();
        return 0;
    }
    if(gpio_group == GPIOC){
        __HAL_RCC_GPIOC_CLK_ENABLE();
        return 0;
    }
    return -1;
}

int pika_GPIO_ioctl_enable(pika_dev* dev){
    pika_GPIO_data_STM32G0 *data = dev->user_data;

    if (0 != _enable_gpio_clk(data->gpioPort)){
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

int pika_GPIO_ioctl_disable(pika_dev* dev){
    pika_GPIO_data_STM32G0 *data = dev->user_data;
    LL_GPIO_DeInit(data->gpioPort);
    return 0;
}

int pika_GPIO_ioctl_config(pika_dev* dev, pika_GPIO_config* cfg){
    pika_GPIO_data_STM32G0 *data = dev->user_data;
    switch (cfg->dir){
        case PIKA_GPIO_DIR_IN:
            data->pinMode = LL_GPIO_MODE_INPUT;
        case PIKA_GPIO_DIR_OUT:
            data->pinMode = LL_GPIO_MODE_OUTPUT;
        default:
            data->pinMode = LL_GPIO_MODE_OUTPUT;
    }
    switch (cfg->pull){
        case PIKA_GPIO_PULL_UP:
            data->gpioPull = LL_GPIO_PULL_UP;
        case PIKA_GPIO_PULL_DOWN:
            data->gpioPull = LL_GPIO_PULL_DOWN;
        default:
            data->gpioPull = LL_GPIO_PULL_NO;
    }
    return 0;
}

int STM32G0_lowLevel_readPin(PikaObj* self, char* pin) {
    GPIO_TypeDef* gpioPort = GPIO_get_Group(pin);
    if (NULL == gpioPort) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }
    uint16_t gpioPin = GPIO_get_pin(pin);
    if (0 == gpioPin) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }
    return LL_GPIO_IsInputPinSet(gpioPort, gpioPin);
}
