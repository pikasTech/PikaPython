#ifndef PIKA_HAL_bflb_COMMON_H
#define PIKA_HAL_bflb_COMMON_H

#include "../pikascript-lib/PikaStdDevice/pika_hal.h"

typedef struct platform_gpio_t {
    struct bflb_device_s* device;
    uint8_t pin;
    uint32_t config;
} platform_gpio_t;

#endif
