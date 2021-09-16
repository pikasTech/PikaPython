#ifndef __STM32__COMMON__H
#define __STM32__COMMON__H

#ifdef STM32G070xx
  #include "stm32g0xx_hal.h"
  #define UART1_EXIST
  #define UART2_EXIST
  #define UART3_EXIST
  #define UART4_EXIST
#endif

#ifdef STM32F103xB
#include "stm32f1xx_hal.h"
#endif

#ifdef STM32G030xx
  #include "stm32g0xx_hal.h"
  #define UART1_EXIST
  #define UART2_EXIST
#endif

GPIO_TypeDef* getGpioPort(char* pin);
uint16_t getGpioPin(char* pin);
uint32_t getPinMode(char* mode);
uint8_t enableClk(char* pin);

#endif