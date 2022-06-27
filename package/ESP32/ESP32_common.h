#ifndef __STM32__COMMON__H
#define __STM32__COMMON__H
#include "PikaObj.h"
#include <stdint.h>

#include "driver/gpio.h"

// #if CONFIG_IDF_TARGET_ESP32

// #endif


int getGpioPin(char *pin);
uint32_t getPinMode(char* mode);





#endif
