#ifndef __AIR32F1_COMMON_H
#define __AIR32F1_COMMON_H
#include "air32f10x.h"
#include "delay.h"

uint8_t enableClk(char* pin);
GPIO_TypeDef* getGpioPort(char* pin);
uint16_t getGpioPin(char* pin);
GPIOMode_TypeDef getPinMode(char* mode);

#endif
