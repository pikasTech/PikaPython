/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SPI_SD_DISK_H
#define HPM_SPI_SD_DISK_H

#include "ff.h"
#include "diskio.h"
#include "hpm_spi_sdcard.h"
#include "hpm_sdmmc_card.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

DSTATUS spi_sd_disk_initialize(BYTE pdrv);

DSTATUS spi_sd_disk_deinitialize(BYTE pdrv);

DSTATUS spi_sd_disk_status(BYTE pdrv);

DSTATUS spi_sd_disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count);

DSTATUS spi_sd_disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count);

DRESULT spi_sd_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* HPM_SPI_SD_DISK_H */
