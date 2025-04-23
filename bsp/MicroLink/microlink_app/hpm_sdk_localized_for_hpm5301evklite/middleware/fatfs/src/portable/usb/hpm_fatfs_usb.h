/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_FATFS_USB_H
#define HPM_FATFS_USB_H

#include "diskio.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void usb_disk_deinitialize(void);
void usb_disk_set_active_msc_class(BYTE pdrv, void *ptr);
uint8_t usb_disk_free_active_msc_class(void *ptr);
DSTATUS usb_disk_initialize(BYTE pdrv);
DSTATUS usb_disk_status(BYTE pdrv);
DRESULT usb_disk_read(BYTE pdrv, BYTE *buff, DWORD sector, BYTE count);
DRESULT usb_disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, BYTE count);
DRESULT usb_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* HPM_FATFS_USB_H */