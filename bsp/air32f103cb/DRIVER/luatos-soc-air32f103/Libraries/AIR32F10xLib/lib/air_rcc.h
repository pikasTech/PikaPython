/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __air_RCC_H
#define __air_RCC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "air32f10x.h"

uint32_t AIR_RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t RCC_PLLMul, uint8_t Latency);

#endif

