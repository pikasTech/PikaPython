#ifndef __STM32__COMMON__H
#define __STM32__COMMON__H
#include <stdint.h>
#include "PikaObj.h"

#include "../PikaStdDevice/pika_hal.h"
#include "driver/gpio.h"
#include "driver/uart.h"

// #if CONFIG_IDF_TARGET_ESP32

// #endif

typedef struct platform_data_GPIO {
    gpio_num_t gpioPort;
    gpio_config_t io_conf;
} platform_data_GPIO;

int getGpioPin(char* pin);
uint32_t getPinMode(char* mode);

#endif
