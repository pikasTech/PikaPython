/*!
 * @file        system_apm32f0xx.h
 * @brief       CMSIS Cortex-M0 Device Peripheral Access Layer System Source File
 *
 * @details
 *
 * @version     V1.0.1
 * @author
 * @date        2021-07-01
 *
 */

#ifndef __SYSTEM_APM32F0XX_H
#define __SYSTEM_APM32F0XX_H

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */

extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_APM32F0XX_H */
