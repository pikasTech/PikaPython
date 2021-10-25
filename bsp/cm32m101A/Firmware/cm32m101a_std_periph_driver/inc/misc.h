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
 * @file misc.h
 * @author CMIOT
 * @version v1.0.0
 *
 * @COPYRIGHT(c) 2020, China Mobile IOT. All rights reserved.
 */
#ifndef __MISC_H__
#define __MISC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cm32m101a.h"

/** @addtogroup cm32m101a_StdPeriph_Driver
 * @{
 */

/** @addtogroup MISC
 * @{
 */

/** @addtogroup MISC_Exported_Types
 * @{
 */
 
#if (__MPU_PRESENT == 1)
/** @defgroup CORTEX_MPU_Region_Initialization_Structure_definition MPU Region Initialization Structure Definition
  * @brief  MPU Region initialization structure 
  * @{
  */
typedef struct
{
  uint8_t                Enable;                /*!< Specifies the status of the region. 
                                                     This parameter can be a value of @ref CORTEX_MPU_Region_Enable                 */
  uint8_t                Number;                /*!< Specifies the number of the region to protect. 
                                                     This parameter can be a value of @ref CORTEX_MPU_Region_Number                 */
  uint32_t               BaseAddress;           /*!< Specifies the base address of the region to protect.                           */
  uint8_t                Size;                  /*!< Specifies the size of the region to protect. 
                                                     This parameter can be a value of @ref CORTEX_MPU_Region_Size                   */
  uint8_t                SubRegionDisable;      /*!< Specifies the number of the subregion protection to disable. 
                                                     This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF    */
  uint8_t                TypeExtField;          /*!< Specifies the TEX field level.
                                                     This parameter can be a value of @ref CORTEX_MPU_TEX_Levels                    */
  uint8_t                AccessPermission;      /*!< Specifies the region access permission type. 
                                                     This parameter can be a value of @ref CORTEX_MPU_Region_Permission_Attributes  */
  uint8_t                DisableExec;           /*!< Specifies the instruction access status. 
                                                     This parameter can be a value of @ref CORTEX_MPU_Instruction_Access            */
  uint8_t                IsShareable;           /*!< Specifies the shareability status of the protected region. 
                                                     This parameter can be a value of @ref CORTEX_MPU_Access_Shareable              */
  uint8_t                IsCacheable;           /*!< Specifies the cacheable status of the region protected. 
                                                     This parameter can be a value of @ref CORTEX_MPU_Access_Cacheable              */
  uint8_t                IsBufferable;          /*!< Specifies the bufferable status of the protected region. 
                                                     This parameter can be a value of @ref CORTEX_MPU_Access_Bufferable             */
}MPU_Region_InitTypeDef;
/**
  * @}
  */
#endif /* __MPU_PRESENT */

/**
 * @brief  NVIC Init Structure definition
 */

typedef struct
{
    uint8_t NVIC_IRQChannel; /*!< Specifies the IRQ channel to be enabled or disabled.
                                  This parameter can be a value of @ref IRQn_Type
                                  (For the complete cm32m101a Devices IRQ Channels list, please
                                   refer to cm32m101a.h file) */

    uint8_t
        NVIC_IRQChannelPreemptionPriority; /*!< Specifies the pre-emption priority for the IRQ channel
                                                specified in NVIC_IRQChannel. This parameter can be a value
                                                between 0 and 15 as described in the table @ref NVIC_Priority_Table */

    uint8_t NVIC_IRQChannelSubPriority; /*!< Specifies the subpriority level for the IRQ channel specified
                                             in NVIC_IRQChannel. This parameter can be a value
                                             between 0 and 15 as described in the table @ref NVIC_Priority_Table */

    FunctionalState NVIC_IRQChannelCmd; /*!< Specifies whether the IRQ channel defined in NVIC_IRQChannel
                                             will be enabled or disabled.
                                             This parameter can be set either to ENABLE or DISABLE */
} NVIC_InitType;

/**
 * @}
 */

/** @addtogroup NVIC_Priority_Table
 * @{
 */

/**
@code
 The table below gives the allowed values of the pre-emption priority and subpriority according
 to the Priority Grouping configuration performed by NVIC_PriorityGroupConfig function
  ============================================================================================================================
    NVIC_PriorityGroup   | NVIC_IRQChannelPreemptionPriority | NVIC_IRQChannelSubPriority  | Description
  ============================================================================================================================
   NVIC_PriorityGroup_0  |                0                  |            0-15             |   0 bits for pre-emption
priority |                                   |                             |   4 bits for subpriority
  ----------------------------------------------------------------------------------------------------------------------------
   NVIC_PriorityGroup_1  |                0-1                |            0-7              |   1 bits for pre-emption
priority |                                   |                             |   3 bits for subpriority
  ----------------------------------------------------------------------------------------------------------------------------
   NVIC_PriorityGroup_2  |                0-3                |            0-3              |   2 bits for pre-emption
priority |                                   |                             |   2 bits for subpriority
  ----------------------------------------------------------------------------------------------------------------------------
   NVIC_PriorityGroup_3  |                0-7                |            0-1              |   3 bits for pre-emption
priority |                                   |                             |   1 bits for subpriority
  ----------------------------------------------------------------------------------------------------------------------------
   NVIC_PriorityGroup_4  |                0-15               |            0                |   4 bits for pre-emption
priority |                                   |                             |   0 bits for subpriority
  ============================================================================================================================
@endcode
*/

/**
 * @}
 */

/** @addtogroup MISC_Exported_Constants
 * @{
 */

/** @addtogroup Vector_Table_Base
 * @{
 */

#define NVIC_VectTab_RAM         ((uint32_t)0x20000000)
#define NVIC_VectTab_FLASH       ((uint32_t)0x08000000)
#define IS_NVIC_VECTTAB(VECTTAB) (((VECTTAB) == NVIC_VectTab_RAM) || ((VECTTAB) == NVIC_VectTab_FLASH))
/**
 * @}
 */

/** @addtogroup System_Low_Power
 * @{
 */

#define NVIC_LP_SEVONPEND   ((uint8_t)0x10)
#define NVIC_LP_SLEEPDEEP   ((uint8_t)0x04)
#define NVIC_LP_SLEEPONEXIT ((uint8_t)0x02)
#define IS_NVIC_LP(LP)      (((LP) == NVIC_LP_SEVONPEND) || ((LP) == NVIC_LP_SLEEPDEEP) || ((LP) == NVIC_LP_SLEEPONEXIT))
/**
 * @}
 */

/** @addtogroup Preemption_Priority_Group
 * @{
 */

#define NVIC_PriorityGroup_0                                                                                           \
    ((uint32_t)0x700) /*!< 0 bits for pre-emption priority                                                             \
                           4 bits for subpriority */
#define NVIC_PriorityGroup_1                                                                                           \
    ((uint32_t)0x600) /*!< 1 bits for pre-emption priority                                                             \
                           3 bits for subpriority */
#define NVIC_PriorityGroup_2                                                                                           \
    ((uint32_t)0x500) /*!< 2 bits for pre-emption priority                                                             \
                           2 bits for subpriority */
#define NVIC_PriorityGroup_3                                                                                           \
    ((uint32_t)0x400) /*!< 3 bits for pre-emption priority                                                             \
                           1 bits for subpriority */
#define NVIC_PriorityGroup_4                                                                                           \
    ((uint32_t)0x300) /*!< 4 bits for pre-emption priority                                                             \
                           0 bits for subpriority */

#define IS_NVIC_PRIORITY_GROUP(GROUP)                                                                                  \
    (((GROUP) == NVIC_PriorityGroup_0) || ((GROUP) == NVIC_PriorityGroup_1) || ((GROUP) == NVIC_PriorityGroup_2)       \
     || ((GROUP) == NVIC_PriorityGroup_3) || ((GROUP) == NVIC_PriorityGroup_4))

#define IS_NVIC_PREEMPTION_PRIORITY(PRIORITY) ((PRIORITY) < 0x10)

#define IS_NVIC_SUB_PRIORITY(PRIORITY) ((PRIORITY) < 0x10)

#define IS_NVIC_OFFSET(OFFSET) ((OFFSET) < 0x000FFFFF)

/**
 * @}
 */
 
#if (__MPU_PRESENT == 1)

/** @addtogroup MPU_Region_Group
 * @{
 */
#define IS_MPU_REGION_ENABLE(STATE) (((STATE) == MPU_REGION_ENABLE) || \
                                     ((STATE) == MPU_REGION_DISABLE))

#define IS_MPU_INSTRUCTION_ACCESS(STATE) (((STATE) == MPU_INSTRUCTION_ACCESS_ENABLE) || \
                                          ((STATE) == MPU_INSTRUCTION_ACCESS_DISABLE))

#define IS_MPU_ACCESS_SHAREABLE(STATE)   (((STATE) == MPU_ACCESS_SHAREABLE) || \
                                          ((STATE) == MPU_ACCESS_NOT_SHAREABLE))

#define IS_MPU_ACCESS_CACHEABLE(STATE)   (((STATE) == MPU_ACCESS_CACHEABLE) || \
                                          ((STATE) == MPU_ACCESS_NOT_CACHEABLE))

#define IS_MPU_ACCESS_BUFFERABLE(STATE)   (((STATE) == MPU_ACCESS_BUFFERABLE) || \
                                          ((STATE) == MPU_ACCESS_NOT_BUFFERABLE))

#define IS_MPU_TEX_LEVEL(TYPE) (((TYPE) == MPU_TEX_LEVEL0)  || \
                                ((TYPE) == MPU_TEX_LEVEL1)  || \
                                ((TYPE) == MPU_TEX_LEVEL2)  || \
                                ((TYPE) == MPU_TEX_LEVEL4))

#define IS_MPU_REGION_PERMISSION_ATTRIBUTE(TYPE) (((TYPE) == MPU_REGION_NO_ACCESS)   || \
                                                  ((TYPE) == MPU_REGION_PRIV_RW)     || \
                                                  ((TYPE) == MPU_REGION_PRIV_RW_URO) || \
                                                  ((TYPE) == MPU_REGION_FULL_ACCESS) || \
                                                  ((TYPE) == MPU_REGION_PRIV_RO)     || \
                                                  ((TYPE) == MPU_REGION_PRIV_RO_URO))

#define IS_MPU_REGION_NUMBER(NUMBER)    (((NUMBER) == MPU_REGION_NUMBER0) || \
                                         ((NUMBER) == MPU_REGION_NUMBER1) || \
                                         ((NUMBER) == MPU_REGION_NUMBER2) || \
                                         ((NUMBER) == MPU_REGION_NUMBER3) || \
                                         ((NUMBER) == MPU_REGION_NUMBER4) || \
                                         ((NUMBER) == MPU_REGION_NUMBER5) || \
                                         ((NUMBER) == MPU_REGION_NUMBER6) || \
                                         ((NUMBER) == MPU_REGION_NUMBER7))

#define IS_MPU_REGION_SIZE(SIZE)    (((SIZE) == MPU_REGION_SIZE_32B)   || \
                                     ((SIZE) == MPU_REGION_SIZE_64B)   || \
                                     ((SIZE) == MPU_REGION_SIZE_128B)  || \
                                     ((SIZE) == MPU_REGION_SIZE_256B)  || \
                                     ((SIZE) == MPU_REGION_SIZE_512B)  || \
                                     ((SIZE) == MPU_REGION_SIZE_1KB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_2KB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_4KB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_8KB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_16KB)  || \
                                     ((SIZE) == MPU_REGION_SIZE_32KB)  || \
                                     ((SIZE) == MPU_REGION_SIZE_64KB)  || \
                                     ((SIZE) == MPU_REGION_SIZE_128KB) || \
                                     ((SIZE) == MPU_REGION_SIZE_256KB) || \
                                     ((SIZE) == MPU_REGION_SIZE_512KB) || \
                                     ((SIZE) == MPU_REGION_SIZE_1MB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_2MB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_4MB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_8MB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_16MB)  || \
                                     ((SIZE) == MPU_REGION_SIZE_32MB)  || \
                                     ((SIZE) == MPU_REGION_SIZE_64MB)  || \
                                     ((SIZE) == MPU_REGION_SIZE_128MB) || \
                                     ((SIZE) == MPU_REGION_SIZE_256MB) || \
                                     ((SIZE) == MPU_REGION_SIZE_512MB) || \
                                     ((SIZE) == MPU_REGION_SIZE_1GB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_2GB)   || \
                                     ((SIZE) == MPU_REGION_SIZE_4GB))

#define IS_MPU_SUB_REGION_DISABLE(SUBREGION)  ((SUBREGION) < (uint16_t)0x00FF)

/**
 * @}
 */
 
/** @defgroup CORTEX_MPU_HFNMI_PRIVDEF_Control CORTEX MPU HFNMI and PRIVILEGED Access control
  * @{
  */
#define  MPU_HFNMI_PRIVDEF_NONE           0x00000000U
#define  MPU_HARDFAULT_NMI                (MPU_CTRL_HFNMIENA_Msk)
#define  MPU_PRIVILEGED_DEFAULT           (MPU_CTRL_PRIVDEFENA_Msk)
#define  MPU_HFNMI_PRIVDEF                (MPU_CTRL_HFNMIENA_Msk | MPU_CTRL_PRIVDEFENA_Msk)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Region_Enable CORTEX MPU Region Enable
  * @{
  */
#define  MPU_REGION_ENABLE           ((uint8_t)0x01)
#define  MPU_REGION_DISABLE          ((uint8_t)0x00)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Instruction_Access CORTEX MPU Instruction Access
  * @{
  */
#define  MPU_INSTRUCTION_ACCESS_ENABLE      ((uint8_t)0x00)
#define  MPU_INSTRUCTION_ACCESS_DISABLE     ((uint8_t)0x01)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Access_Shareable CORTEX MPU Instruction Access Shareable
  * @{
  */
#define  MPU_ACCESS_SHAREABLE        ((uint8_t)0x01)
#define  MPU_ACCESS_NOT_SHAREABLE    ((uint8_t)0x00)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Access_Cacheable CORTEX MPU Instruction Access Cacheable
  * @{
  */
#define  MPU_ACCESS_CACHEABLE        ((uint8_t)0x01)
#define  MPU_ACCESS_NOT_CACHEABLE    ((uint8_t)0x00)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Access_Bufferable CORTEX MPU Instruction Access Bufferable
  * @{
  */
#define  MPU_ACCESS_BUFFERABLE       ((uint8_t)0x01)
#define  MPU_ACCESS_NOT_BUFFERABLE   ((uint8_t)0x00)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_TEX_Levels CORTEX MPU TEX Levels
  * @{
  */
#define  MPU_TEX_LEVEL0              ((uint8_t)0x00)
#define  MPU_TEX_LEVEL1              ((uint8_t)0x01)
#define  MPU_TEX_LEVEL2              ((uint8_t)0x02)
#define  MPU_TEX_LEVEL4              ((uint8_t)0x04)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Region_Size CORTEX MPU Region Size
  * @{
  */
#define   MPU_REGION_SIZE_32B        ((uint8_t)0x04)
#define   MPU_REGION_SIZE_64B        ((uint8_t)0x05)
#define   MPU_REGION_SIZE_128B       ((uint8_t)0x06)
#define   MPU_REGION_SIZE_256B       ((uint8_t)0x07)
#define   MPU_REGION_SIZE_512B       ((uint8_t)0x08)
#define   MPU_REGION_SIZE_1KB        ((uint8_t)0x09)
#define   MPU_REGION_SIZE_2KB        ((uint8_t)0x0A)
#define   MPU_REGION_SIZE_4KB        ((uint8_t)0x0B)
#define   MPU_REGION_SIZE_8KB        ((uint8_t)0x0C)
#define   MPU_REGION_SIZE_16KB       ((uint8_t)0x0D)
#define   MPU_REGION_SIZE_32KB       ((uint8_t)0x0E)
#define   MPU_REGION_SIZE_64KB       ((uint8_t)0x0F)
#define   MPU_REGION_SIZE_128KB      ((uint8_t)0x10)
#define   MPU_REGION_SIZE_256KB      ((uint8_t)0x11)
#define   MPU_REGION_SIZE_512KB      ((uint8_t)0x12)
#define   MPU_REGION_SIZE_1MB        ((uint8_t)0x13)
#define   MPU_REGION_SIZE_2MB        ((uint8_t)0x14)
#define   MPU_REGION_SIZE_4MB        ((uint8_t)0x15)
#define   MPU_REGION_SIZE_8MB        ((uint8_t)0x16)
#define   MPU_REGION_SIZE_16MB       ((uint8_t)0x17)
#define   MPU_REGION_SIZE_32MB       ((uint8_t)0x18)
#define   MPU_REGION_SIZE_64MB       ((uint8_t)0x19)
#define   MPU_REGION_SIZE_128MB      ((uint8_t)0x1A)
#define   MPU_REGION_SIZE_256MB      ((uint8_t)0x1B)
#define   MPU_REGION_SIZE_512MB      ((uint8_t)0x1C)
#define   MPU_REGION_SIZE_1GB        ((uint8_t)0x1D)
#define   MPU_REGION_SIZE_2GB        ((uint8_t)0x1E)
#define   MPU_REGION_SIZE_4GB        ((uint8_t)0x1F)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Region_Permission_Attributes CORTEX MPU Region Permission Attributes 
  * @{
  */
#define  MPU_REGION_NO_ACCESS        ((uint8_t)0x00)
#define  MPU_REGION_PRIV_RW          ((uint8_t)0x01)
#define  MPU_REGION_PRIV_RW_URO      ((uint8_t)0x02)
#define  MPU_REGION_FULL_ACCESS      ((uint8_t)0x03)
#define  MPU_REGION_PRIV_RO          ((uint8_t)0x05)
#define  MPU_REGION_PRIV_RO_URO      ((uint8_t)0x06)
/**
  * @}
  */

/** @defgroup CORTEX_MPU_Region_Number CORTEX MPU Region Number
  * @{
  */
#define  MPU_REGION_NUMBER0          ((uint8_t)0x00)
#define  MPU_REGION_NUMBER1          ((uint8_t)0x01)
#define  MPU_REGION_NUMBER2          ((uint8_t)0x02)
#define  MPU_REGION_NUMBER3          ((uint8_t)0x03)
#define  MPU_REGION_NUMBER4          ((uint8_t)0x04)
#define  MPU_REGION_NUMBER5          ((uint8_t)0x05)
#define  MPU_REGION_NUMBER6          ((uint8_t)0x06)
#define  MPU_REGION_NUMBER7          ((uint8_t)0x07)
/**
  * @}
  */
#endif /* __MPU_PRESENT */ 

/** @addtogroup SysTick_clock_source
 * @{
 */

#define SysTick_CLKSource_HCLK_Div8 ((uint32_t)0xFFFFFFFB)
#define SysTick_CLKSource_HCLK      ((uint32_t)0x00000004)
#define IS_SYSTICK_CLK_SOURCE(SOURCE)                                                                                  \
    (((SOURCE) == SysTick_CLKSource_HCLK) || ((SOURCE) == SysTick_CLKSource_HCLK_Div8))
/**
 * @}
 */

/**
 * @}
 */

/** @addtogroup MISC_Exported_Macros
 * @{
 */

/**
 * @}
 */

/** @addtogroup MISC_Exported_Functions
 * @{
 */

void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);
void NVIC_Init(NVIC_InitType* NVIC_InitStruct);
void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset);
void NVIC_SystemLPConfig(uint8_t LowPowerMode, FunctionalState Cmd);
void SysTick_CLKSourceConfig(uint32_t SysTick_CLKSource);

#if (__MPU_PRESENT == 1)
void MPU_Enable(uint32_t MPU_Control);
void MPU_Disable(void);
void MPU_ConfigRegion(MPU_Region_InitTypeDef *MPU_Init);
#endif /* __MPU_PRESENT */

#ifdef __cplusplus
}
#endif

#endif /* __MISC_H__ */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
