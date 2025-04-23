/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_RAM_FLASH_DISK_H
#define HPM_RAM_FLASH_DISK_H

#include "ff.h"
#include "diskio.h"


#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

DSTATUS flash_disk_initialize(BYTE pdrv);

DSTATUS flash_disk_status(BYTE pdrv);

DSTATUS flash_disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count);

DSTATUS flash_disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count);

DRESULT flash_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* HPM_SPI_SD_DISK_H */
