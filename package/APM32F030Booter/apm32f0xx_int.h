/*!
 * @file        apm32f0xx_int.h
 *
 * @brief       This file contains the headers of the interrupt handlers   
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */
#ifndef APM32F0XX_INT_H
#define APM32F0XX_INT_H

#include "apm32f0xx.h"

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
#endif
