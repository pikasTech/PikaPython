/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various existing       */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "diskio.h"          /* Declarations of disk functions */

#if defined(USB_FATFS_ENABLE) && USB_FATFS_ENABLE
#include "./usb/hpm_fatfs_usb.h"
#endif

#if defined(RAM_FLASH_FATFS_ENABLE) && RAM_FLASH_FATFS_ENABLE
#include "./ram_flash/hpm_ram_flash.h"
#endif

#if defined(SD_FATFS_ENABLE) && SD_FATFS_ENABLE

#if defined(SD_SPI_ENABLE) && (SD_SPI_ENABLE)
#include "./spi_sd/hpm_spi_sd_disk.h"
#else
#include "./sdxc/hpm_sdmmc_disk.h"
#endif

#endif

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status(
        BYTE pdrv   /* Physical drive number to identify the drive */
)
{
    DSTATUS stat = STA_NOINIT;

    switch (pdrv) {
    case DEV_RAM :
        stat = flash_disk_status(pdrv);
        break;

    case DEV_MMC :
#if defined(MMC_FATFS_ENABLE) && MMC_FATFS_ENABLE
        stat = emmc_disk_status(pdrv);
#endif
        break;

#if defined(USB_FATFS_ENABLE) && USB_FATFS_ENABLE
    case DEV_USB :
    case DEV_USB_MSC_0 :
    case DEV_USB_MSC_1 :
        stat = usb_disk_status(pdrv);
        break;
#endif

#if defined(SD_FATFS_ENABLE) && SD_FATFS_ENABLE
    case DEV_SD:
#if defined(SD_SPI_ENABLE) && (SD_SPI_ENABLE)
        stat = spi_sd_disk_status(pdrv);
#else
        stat = sd_disk_status(pdrv);
#endif
        break;
#endif

    default:
        break;
    }

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
void disk_deinitialize(
        BYTE pdrv   /* Physical drive number to identify the drive */
)
{
    switch (pdrv) {
    case DEV_RAM :
        break;

    case DEV_MMC :
#if defined(MMC_FATFS_ENABLE) && MMC_FATFS_ENABLE
        emmc_disk_deinitialize(pdrv);
#endif
        break;

#if defined(USB_FATFS_ENABLE) && USB_FATFS_ENABLE
    case DEV_USB :
    case DEV_USB_MSC_0 :
    case DEV_USB_MSC_1 :
        usb_disk_deinitialize();
        break;
#endif

#if defined(SD_FATFS_ENABLE) && SD_FATFS_ENABLE
    case DEV_SD:
#if defined(SD_SPI_ENABLE) && (SD_SPI_ENABLE)
        spi_sd_disk_deinitialize(pdrv);
#else
        sd_disk_deinitialize(pdrv);
#endif
        break;
#endif
    default:
        break;
    }
}

DSTATUS disk_initialize(
        BYTE pdrv   /* Physical drive number to identify the drive */
)
{
    DSTATUS stat = STA_NOINIT;

    switch (pdrv) {
    case DEV_RAM :
        stat = flash_disk_initialize(pdrv);
        break;

    case DEV_MMC :
#if defined(MMC_FATFS_ENABLE) && MMC_FATFS_ENABLE
        stat = emmc_disk_initialize(pdrv);
#endif
        break;

#if defined(USB_FATFS_ENABLE) && USB_FATFS_ENABLE
    case DEV_USB :
    case DEV_USB_MSC_0 :
    case DEV_USB_MSC_1 :
        stat = usb_disk_initialize(pdrv);
        break;
#endif

#if defined(SD_FATFS_ENABLE) && SD_FATFS_ENABLE
    case DEV_SD:
#if defined(SD_SPI_ENABLE) && (SD_SPI_ENABLE)
        stat = spi_sd_disk_initialize(pdrv);
#else
        stat = sd_disk_initialize(pdrv);
#endif
        break;
#endif
    default:
        break;
    }

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(
        BYTE pdrv,      /* Physical drive number to identify the drive */
        BYTE *buff,     /* Data buffer to store read data */
        LBA_t sector,   /* Start sector in LBA */
        UINT count      /* Number of sectors to read */
)
{
    DRESULT res = RES_ERROR;

    switch (pdrv) {
    case DEV_RAM :
        res = flash_disk_read(pdrv, buff, sector, count);
        break;

    case DEV_MMC :
#if defined(MMC_FATFS_ENABLE) && MMC_FATFS_ENABLE
        res = emmc_disk_read(pdrv, buff, sector, count);
#endif
        break;

#if defined(USB_FATFS_ENABLE) && USB_FATFS_ENABLE
    case DEV_USB :
    case DEV_USB_MSC_0 :
    case DEV_USB_MSC_1 :
        res = usb_disk_read(pdrv, buff, sector, count);
        break;
#endif

#if defined(SD_FATFS_ENABLE) && SD_FATFS_ENABLE
    case DEV_SD:
#if defined(SD_SPI_ENABLE) && (SD_SPI_ENABLE)
        res = spi_sd_disk_read(pdrv, buff, sector, count);
#else
        res = sd_disk_read(pdrv, buff, sector, count);
#endif
        break;
#endif
    default:
        res = RES_PARERR;
        break;
    }

    return res;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
        BYTE pdrv,          /* Physical drive number to identify the drive */
        const BYTE *buff,   /* Data to be written */
        LBA_t sector,       /* Start sector in LBA */
        UINT count          /* Number of sectors to write */
)
{
    DRESULT res = RES_ERROR;

    switch (pdrv) {
    case DEV_RAM :
        res = flash_disk_write(pdrv, buff, sector, count);
        break;

    case DEV_MMC :
#if defined(MMC_FATFS_ENABLE) && MMC_FATFS_ENABLE
        res = emmc_disk_write(pdrv, buff, sector, count);
#endif
        break;

#if defined(USB_FATFS_ENABLE) && USB_FATFS_ENABLE
    case DEV_USB :
    case DEV_USB_MSC_0 :
    case DEV_USB_MSC_1 :
        res = usb_disk_write(pdrv, buff, sector, count);
        break;
#endif

#if defined(SD_FATFS_ENABLE) && SD_FATFS_ENABLE
    case DEV_SD:
#if defined(SD_SPI_ENABLE) && (SD_SPI_ENABLE)
        res = spi_sd_disk_write(pdrv, buff, sector, count);
#else
        res = sd_disk_write(pdrv, buff, sector, count);
#endif
        break;
#endif
    default:
        res = RES_PARERR;
        break;
    }

    return res;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl(
        BYTE pdrv,      /* Physical drive number (0..) */
        BYTE cmd,       /* Control code */
        void *buff      /* Buffer to send/receive control data */
)
{
    DRESULT res = RES_ERROR;

    switch (pdrv) {
    case DEV_RAM :
        res = flash_disk_ioctl(pdrv, cmd ,buff);
        break;

    case DEV_MMC :
#if defined(MMC_FATFS_ENABLE) && MMC_FATFS_ENABLE
        res = emmc_disk_ioctl(pdrv, cmd, buff);
#endif
        break;

#if defined(USB_FATFS_ENABLE) && USB_FATFS_ENABLE
    case DEV_USB :
    case DEV_USB_MSC_0 :
    case DEV_USB_MSC_1 :
        res = usb_disk_ioctl(pdrv, cmd ,buff);
        break;
#endif

#if defined(SD_FATFS_ENABLE) && SD_FATFS_ENABLE
    case DEV_SD:
#if defined(SD_SPI_ENABLE) && (SD_SPI_ENABLE)
        res = spi_sd_disk_ioctl(pdrv, cmd, buff);
#else
        res = sd_disk_ioctl(pdrv, cmd, buff);
#endif
        break;
#endif

    default:
        res = RES_ERROR;
        break;
    }

    return res;
}
