/*
 * Copyright (c) 2021-2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SDMMC_DISK_H
#define HPM_SDMMC_DISK_H

#include "ff.h"
#include "diskio.h"



#define MAX_ALIGNED_BUF_SIZE (16384U)

#ifdef __cplusplus
extern "C" {
#endif

DSTATUS sd_disk_initialize(BYTE pdrv);

DSTATUS sd_disk_deinitialize(BYTE pdrv);

DSTATUS sd_disk_status(BYTE pdrv);

DSTATUS sd_disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count);

DSTATUS sd_disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count);

DRESULT sd_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff);

DSTATUS emmc_disk_initialize(BYTE pdrv);

DSTATUS emmc_disk_deinitialize(BYTE pdrv);

DSTATUS emmc_disk_status(BYTE pdrv);

DSTATUS emmc_disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count);

DSTATUS emmc_disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count);

DRESULT emmc_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff);


#ifdef __cplusplus
}
#endif

#endif /* HPM_SDMMC_DISK_H */
