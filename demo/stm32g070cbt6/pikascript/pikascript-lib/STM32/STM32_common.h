#ifndef __STM32__COMMON__H
#define __STM32__COMMON__H

#ifdef STM32G070xx
#include "stm32g0xx_hal.h"
#endif


GPIO_TypeDef *getGpioPort(char *pin);
uint16_t getGpioPin(char *pin);
uint32_t getPinMode(char *mode);
uint8_t enableClk(char *pin);

#endif