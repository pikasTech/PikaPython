/*
  ******************************************************************************
  *
  * COPYRIGHT(c) 2020, China Mobile IOT
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of China Mobile IOT nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/**
 * @file cm32m101a_pwr.h
 * @author CMIOT
 * @version v1.0.0
 *
 * @COPYRIGHT(c) 2020, China Mobile IOT. All rights reserved.
 */
#ifndef __CM32M101A_PWR_H__
#define __CM32M101A_PWR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cm32m101a.h"

/** @addtogroup cm32m101a_StdPeriph_Driver
 * @{
 */

/** @addtogroup PWR
 * @{
 */

/** @addtogroup PWR_Exported_Types
 * @{
 */

/**
 * @}
 */

/** @addtogroup PWR_Exported_Constants
 * @{
 */

/** @addtogroup PVD_detection_level
 * @{
 */

#define PWR_PVDLEVEL_2V1  ((uint32_t)0x00000000)
#define PWR_PVDLEVEL_2V25 ((uint32_t)0x0000002)
#define PWR_PVDLEVEL_2V4  ((uint32_t)0x0000004)
#define PWR_PVDLEVEL_2V55 ((uint32_t)0x0000006)
#define PWR_PVDLEVEL_2V7  ((uint32_t)0x0000008)
#define PWR_PVDLEVEL_2V85 ((uint32_t)0x000000A)
#define PWR_PVDLEVEL_2V95 ((uint32_t)0x000000C)
#define PWR_PVDLEVEL_IN   ((uint32_t)0x000000E)


#define IS_PWR_PVD_LEVEL(LEVEL)                                                                                        \
    (((LEVEL) == PWR_PVDLEVEL_2V1) || ((LEVEL) == PWR_PVDLEVEL_2V25) || ((LEVEL) == PWR_PVDLEVEL_2V4)               \
     || ((LEVEL) == PWR_PVDLEVEL_2V55) || ((LEVEL) == PWR_PVDLEVEL_2V7) || ((LEVEL) == PWR_PVDLEVEL_2V85)           \
     || ((LEVEL) == PWR_PVDLEVEL_2V95) || ((LEVEL) == PWR_PVDLEVEL_IN) )

/**
 * @}
 */

/** @addtogroup Regulator_state_is_STOP_mode
 * @{
 */

#define PWR_REGULATOR_ON            ((uint32_t)0x00000000)
#define PWR_REGULATOR_LOWPOWER      ((uint32_t)0x00000001)
#define IS_PWR_REGULATOR(REGULATOR) (((REGULATOR) == PWR_REGULATOR_ON) || ((REGULATOR) == PWR_REGULATOR_LOWPOWER))
/**
 * @}
 */

/** @defgroup SLEEP_mode_entry 
  * @{
  */
#define SLEEP_ON_EXIT              (1)
#define SLEEP_OFF_EXIT             (0)
#define PWR_SLEEPENTRY_WFI         ((uint8_t)0x01)
#define PWR_SLEEPENTRY_WFE         ((uint8_t)0x02)
#define IS_PWR_SLEEP_ENTRY(ENTRY) (((ENTRY) == PWR_SLEEPENTRY_WFI) || ((ENTRY) == PWR_SLEEPENTRY_WFE))
 
 
/**
  * @}
  */


/** @defgroup STOP_mode_entry 
  * @{
  */

#define PWR_STOPENTRY_WFI        ((uint8_t)0x01)
#define PWR_STOPENTRY_WFE        ((uint8_t)0x02)
#define IS_PWR_STOP_ENTRY(ENTRY) (((ENTRY) == PWR_STOPENTRY_WFI) || ((ENTRY) == PWR_STOPENTRY_WFE))

/**
 * @}
 */

/** @addtogroup PWR_Flag
 * @{
 */
//STS1
#define PWR_WKUP1_FLAG    ((uint32_t)0x00000001)
#define PWR_WKUP2_FLAG    ((uint32_t)0x00000002)
#define PWR_WKUP3_FLAG    ((uint32_t)0x00000004)
#define PWR_STBY_FLAG     ((uint32_t)0x00000100)
//STS2
#define PWR_LPRUN_FLAG    ((uint32_t)0x00000001)
#define PWR_MR_FLAG       ((uint32_t)0x00000002)
#define PWR_PVDO_FLAG     ((uint32_t)0x00000004)

#define IS_PWR_GET_FLAG(FLAG)                                                                                          \
    (((FLAG) == PWR_WKUP1_FLAG) || ((FLAG) == PWR_WKUP2_FLAG) || ((FLAG) == PWR_WKUP3_FLAG) || ((FLAG) == PWR_STBY_FLAG)\
      || ((FLAG) == PWR_LPRUN_FLAG) || ((FLAG) == PWR_MR_FLAG) || ((FLAG) == PWR_PVDO_FLAG))

#define IS_PWR_CLEAR_FLAG(FLAG) \
    (((FLAG) == PWR_WKUP1_FLAG) || ((FLAG) == PWR_WKUP2_FLAG) || ((FLAG) == PWR_WKUP3_FLAG) || ((FLAG) == PWR_STBY_FLAG)\
    || ((FLAG) == PWR_LPRUN_FLAG) || ((FLAG) == PWR_MR_FLAG) || ((FLAG) == PWR_PVDO_FLAG))



/** @addtogroup SRAM1o¨ªSRAM2 retention set
 * @{
 */
//#define SRAM1DIS_SRAM2DIS      0
//#define SRAM1EN_SRAM2DIS       1

//#define SRAM1DIS_SRAM2EN       2
//#define SRAM1EN_SRAM2EN        3
/** @addtogroup MR VOLTAGE
 * @{
 */
#define MR_1V0  2
#define MR_1V1  3
 
 
/**
 * @}
 */
typedef enum
{
    WAKEUPPIN1 = 1,
    WAKEUPPIN2 = 2,
	WAKEUPPIN3 = 3,
} WAKEUPPINX;
/** @addtogroup PWR_Exported_Macros
 * @{
 */

/**
 * @}
 */
#define LPRUN_SRAM_ADDR   (__IO unsigned*)(0x40001800 + 0x20)
#define CLERR_BIT25  0xfdffffff //bit25  
#define _SetLprunSramVoltage(vale) do{(*LPRUN_SRAM_ADDR) &= CLERR_BIT25;(*LPRUN_SRAM_ADDR) |= (uint32_t)(vale <<25);}while(0) //0:0.9V 1:1.1V  
#define _SetBandGapMode(vale) do{PWR->CTRL3 &= (~PWR_CTRL3_BGDTLPR);PWR->CTRL3 |= (uint32_t)(vale <<8);}while(0) //0:always on 1:duty on  
#define _SetPvdBorMode(vale) do{PWR->CTRL3 &= (~PWR_CTRL3_PBDTLPR);PWR->CTRL3 |= (uint32_t)(vale <<16);}while(0) //0:normal mode 1:standby mode  
/** @addtogroup PWR_Exported_Functions
 * @{
 */

void PWR_DeInit(void);
void PWR_BackupAccessEnable(FunctionalState Cmd);
void PWR_PvdEnable(FunctionalState Cmd);
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel);
void PWR_WakeUpPinEnable(uint8_t Pin,FunctionalState Cmd);
void PWR_EnterStopState(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry);
void PWR_EnterSLEEPMode(uint8_t SLEEPONEXIT, uint8_t PWR_STOPEntry);
void PWR_EnterSTOP2Mode(uint8_t PWR_STOPEntry,uint32_t RetentionMode);
void PWR_EnterSTANDBYMode(uint8_t PWR_STANDBYEntry,uint32_t Sam2Ret);
void PWR_EnterLowPowerRunMode(void);
void PWR_ExitLowPowerRunMode(void);
void PWR_EnterLowPowerSleepMode(uint8_t SLEEPONEXIT, uint8_t PWR_SLEEPEntry);
void PWR_EnterPDMode(void);
FlagStatus PWR_GetFlagStatus(uint8_t STS, uint32_t PWR_FLAG);
void PWR_ClearFlag(uint32_t PWR_FLAG);
void PWR_WakeUpPinConfig(void);
void SetSysClock_MSI(void);
uint8_t GetMrVoltage(void);
void PWR_MRconfig(uint8_t voltage);
#ifdef __cplusplus
}
#endif

#endif /* __CM32M101A_PWR_H__ */
       /**
        * @}
        */

/**
 * @}
 */

/**
 * @}
 */
