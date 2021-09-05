/*
 * Copyright (c) 2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*-----------------------------------------------------------------------------
 * File     system_MPS3_CM55.h
 * Brief    CMSIS Device System Header File for
 *          MPS3_CM55 Device (AN540)
 * Version  1.0.0
 *----------------------------------------------------------------------------*/

#ifndef SYSTEM_MPS3_CM55_H
#define SYSTEM_MPS3_CM55_H

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t SystemCoreClock;     /* System Clock Frequency (Core Clock) */


/**
  brief Setup the microcontroller system.

   Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);


/**
  brief  Update SystemCoreClock variable.

   Updates the SystemCoreClock with current core Clock retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate (void);

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define  SYS_CLOCK       (60000000UL)
#define  X32K_CLOCK      (   32768UL)     /* Oscillator frequency */

#define  SYSTEM_CLOCK    (SYS_CLOCK)
 
 

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_YAMIN_H */
