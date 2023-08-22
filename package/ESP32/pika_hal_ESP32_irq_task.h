#ifndef __ESP32__IRQ__TASK__H
#define __ESP32__IRQ__TASK__H

#include "../PikaStdDevice/pika_hal.h"
#include "driver/gpio.h"

int _ESP32_irq_start(void);
void _ESP32_gpio_isr_handler(void *arg);

#endif