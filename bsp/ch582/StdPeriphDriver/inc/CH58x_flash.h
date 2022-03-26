/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_flash.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH58x_FLASH_H__
#define __CH58x_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ∂¡»°Flash-ROM
 *
 * @param   StartAddr   - read address
 * @param   Buffer      - read buffer
 * @param   len         - read len
 */
void FLASH_ROM_READ(uint32_t StartAddr, void *Buffer, uint32_t len);

void FLASH_ROM_READ(UINT32 StartAddr, PVOID Buffer, UINT32 len); /* ∂¡»°Flash-ROM */

UINT8 UserOptionByteConfig(FunctionalState RESET_EN, FunctionalState BOOT_PIN, FunctionalState UART_NO_KEY_EN,
                           UINT32 FLASHProt_Size);

UINT8 UserOptionByteClose_SWD(void);

void UserOptionByte_Active(void);

#ifdef __cplusplus
}
#endif

#endif // __CH58x_FLASH_H__
