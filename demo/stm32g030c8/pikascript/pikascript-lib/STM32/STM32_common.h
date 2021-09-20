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
  #define UART1_EXIST
  #define UART2_EXIST
  #define UART3_EXIST

  #define TIM1_EXIST
  #define TIM2_EXIST
  #define TIM3_EXIST
  #define TIM4_EXIST
#endif

#ifdef STM32G030xx
  #include "stm32g0xx_hal.h"
  #define UART1_EXIST
  #define UART2_EXIST
  
  #define TIM1_EXIST
  #define TIM3_EXIST
  #define TIM14_EXIST
  #define TIM16_EXIST
  #define TIM17_EXIST
#endif

GPIO_TypeDef* getGpioPort(char* pin);
uint16_t getGpioPin(char* pin);
uint32_t getPinMode(char* mode);
uint8_t enableClk(char* pin);
void delay_us(uint32_t delay);
void delay_unit(uint32_t delay);

#endif