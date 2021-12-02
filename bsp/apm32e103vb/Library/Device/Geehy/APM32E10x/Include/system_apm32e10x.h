/*!
 * @file        system_apm32e10x.h
 *
 * @brief       CMSIS Cortex-M3 Device Peripheral Access Layer System Source File
 *
 * @version     V1.0.0
 *
 * @date        2021-07-26
 *
 */

#ifndef __SYSTEM_APM32E10X_H
#define __SYSTEM_APM32E10X_H

#ifdef __cplusplus
 extern "C" {
#endif

extern uint32_t SystemCoreClock;

extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_APM32E10X_H */

