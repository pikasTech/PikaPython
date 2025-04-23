/**
 * @file    target_family.h
 * @brief
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2018-2019, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TARGET_FAMILY_H
#define TARGET_FAMILY_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

//! @brief Creates a family ID from a vendor ID and family index within that vendor.
#define CREATE_FAMILY_ID(vendor, family) ((vendor) << 8 | (family))

//! @brief States into which the target can be placed.
//!
//! These enums are passed to target_set_state() and indicate the desired state into which
//! the target should be reset.
typedef enum _target_state {
    RESET_HOLD,              //!< Hold target in reset
    RESET_PROGRAM,           //!< Reset target and setup for flash programming
    RESET_RUN,               //!< Reset target and run normally
    NO_DEBUG,                //!< Disable debug on running target
    _DEBUG,                   //!< Enable debug on running target
    HALT,                    //!< Halt the target without resetting it
    RUN,                     //!< Resume the target without resetting it
    POST_FLASH_RESET,        //!< Reset target after flash programming
    POWER_ON,                //!< Poweron the target
    SHUTDOWN,                //!< Poweroff the target
} target_state_t;

//! @brief Options for reset.
typedef enum _reset_type {
    kHardwareReset = 1,
    kSoftwareReset,
} reset_type_t;

typedef enum _family_revision_id {
    STM32F1 = 0X410,
    
} family_revision_id;

typedef enum _family_device_id {
    STM32F1_low_density_FamilyID = 0X412,
    STM32F1_medium_density_FamilyID = 0X410,  
    STM32F1_high_density_FamilyID = 0X414,  
    STM32F1_XL_density_FamilyID = 0X430, 
    STM32F1_connectivity_density_FamilyID = 0X418,
    STM32F2_FamilyID = 0X411,
    STM32F302x6x8_FamilyID = 0X439, 
    STM32F302xBxC_FamilyID = 0X422,
    STM32F302xDxE_FamilyID = 0X446, 
    STM32F405x07x15x17_FamilyID = 0X413, 
    STM32F401xBxC_FamilyID = 0x423, 
    STM32F401xDxE_FamilyID = 0x433, 
    STM32F413423_FamilyID = 0X463, 
    STM32F411xCxE_FamilyID = 0X431, 
    STM32F42x3x_FamilyID = 0X419, 
    STM32G4_Category2_FamilyID = 0X468, 
    STM32G4_Category3_FamilyID = 0X469, 
    STM32G4_Category4_FamilyID = 0X479, 
    STM32F7_FamilyID = 0X449,
    STM32H7_FamilyID = 0X450,     
    GD32E50x_512_FamilyID = 0X444,   
    GD32F303_HD_FamilyID = 0X430,                           
} family_device_id;



#ifdef __cplusplus
extern "C" {
#endif

//! @brief Initialize g_target_family.
uint32_t get_flash_blob(void);

//! @brief Reset the target into a new state.
//!
//! Used to prepare the target for some operation, or release it for user control.
uint8_t target_set_state(target_state_t state);

//! @brief Controls reset of the target.
void swd_set_target_reset(uint8_t asserted);

//! @brief Get the APSEL for the AHB-AP to use for controlling the target.
uint32_t target_get_apsel(void);

#ifdef __cplusplus
}
#endif

#endif
