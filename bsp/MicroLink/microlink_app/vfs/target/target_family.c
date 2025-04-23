/**
 * @file    target_family.c
 * @brief   Implementation of target_family.h
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2019, ARM Limited, All Rights Reserved
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
#include "stdio.h"
#include "DAP_config.h"
#include "swd_host.h"
#include "target_family.h"
#include "target_board.h"
#include "core_cm3.h"
#include "target_config.h"

target_cfg_t target_device = {
    .version                        = kTargetConfigVersion,
    .flash_regions[0].flags         = kRegionIsDefault,
    .target_vendor                  = NULL,
    .target_part_number             = NULL,
};

uint32_t get_flash_blob(void)
{
    if(tFlashBlob.target.init != 0){
        target_device.flash_dev_info                 = (flash_dev_t *)&tFlashBlob.prog_blob[(tFlashBlob.target.algo_size)/4];
        target_device.sector_info_length             = tFlashBlob.sector_info_length;
        target_device.flash_regions[0].start         = target_device.flash_dev_info->DevAdr;
        target_device.flash_regions[0].end           = target_device.flash_dev_info->DevAdr + target_device.flash_dev_info->szDev;
        target_device.flash_regions[0].flash_algo    = (program_target_t *) &tFlashBlob.target;
        target_device.ram_regions[0].start           =  tFlashBlob.target.algo_start;
        target_device.ram_regions[0].end             =  tFlashBlob.target.algo_start + RAM_SIZE;
        swd_set_reset_connect(CONNECT_UNDER_RESET);
        return 0;
    }
    return 1;
}

uint8_t target_set_state(target_state_t state)
{
    return swd_set_target_state_sw(state);
}

void swd_set_target_reset(uint8_t asserted)
{
    (asserted) ? PIN_nRESET_OUT(0) : PIN_nRESET_OUT(1);
}

uint32_t target_get_apsel()
{
    return 0;
}


uint8_t software_reset(void)
{
    if (DAP_Data.debug_port != DAP_PORT_SWD) {
        return 1;
    }
    uint8_t ret = 0;
    uint32_t val;
    ret |= swd_read_word(NVIC_AIRCR, &val);
    ret |= swd_write_word(NVIC_AIRCR, VECTKEY | (val & SCB_AIRCR_PRIGROUP_Msk) | SYSRESETREQ);
    return ret;
}