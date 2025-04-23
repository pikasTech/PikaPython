/*-----------------------------------------------------------------------/
/  Low level disk interface module include file   (C)ChaN, 2019          /
/-----------------------------------------------------------------------*/

/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#include "ff.h"     /* Obtains integer types */
#include <stdbool.h>

/* Status of Disk Functions */
typedef BYTE DSTATUS;

/* Results of Disk Functions */
typedef enum {
    RES_OK = 0,     /* 0: Successful */
    RES_ERROR,      /* 1: R/W Error */
    RES_WRPRT,      /* 2: Write Protected */
    RES_NOTRDY,     /* 3: Not Ready */
    RES_PARERR      /* 4: Invalid Parameter */
} DRESULT;

/* Definitions of physical drive number for each drive */
#define DEV_SD      (3U)    /* Example: Map SD Disk to physical drive 3 */
#define DEV_RAM     (2U)    /* Example: Map Ramdisk to physical drive 2 */
#define DEV_MMC     (1U)    /* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB     (0U)    /* Example: Map USB MSD to physical drive 0 */
#define DEV_USB_MSC_0   (5U)    /* Example: Map USB MSD to physical drive 5 */
#define DEV_USB_MSC_1   (6U)    /* Example: Map USB MSD to physical drive 6 */

/* Definition of logic unit number for each drive */
#define LUN_USB	    (0U)    /* lun 0 of usb drive */

/* Disk Status Bits (DSTATUS) */
#define STA_NOINIT  (0x01)   /* Drive not initialized */
#define STA_NODISK  (0x02)   /* No medium in the drive */
#define STA_PROTECT (0x04)   /* Write protected */

/* Command code for disk_ioctrl fucntion */

/* Generic command (Used by FatFs) */
#define CTRL_SYNC           (0U)    /* Complete pending write process (needed at FF_FS_READONLY == 0) */
#define GET_SECTOR_COUNT    (1U)    /* Get media size (needed at FF_USE_MKFS == 1) */
#define GET_SECTOR_SIZE     (2U)    /* Get sector size (needed at FF_MAX_SS != FF_MIN_SS) */
#define GET_BLOCK_SIZE      (3U)    /* Get erase block size (needed at FF_USE_MKFS == 1) */
#define CTRL_TRIM           (4U)    /* Inform device that the data on the block of sectors is no longer used (needed at FF_USE_TRIM == 1) */

/* Generic command (Not used by FatFs) */
#define CTRL_POWER          (5U)    /* Get/Set power status */
#define CTRL_LOCK           (6U)    /* Lock/Unlock media removal */
#define CTRL_EJECT          (7U)    /* Eject media */
#define CTRL_FORMAT         (8U)    /* Create physical format on the media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE        (10U)   /* Get card type */
#define MMC_GET_CSD         (11U)   /* Get CSD */
#define MMC_GET_CID         (12U)   /* Get CID */
#define MMC_GET_OCR         (13U)   /* Get OCR */
#define MMC_GET_SDSTAT      (14U)   /* Get SD status */
#define ISDIO_READ          (55U)   /* Read data form SD iSDIO register */
#define ISDIO_WRITE         (56U)   /* Write data to SD iSDIO register */
#define ISDIO_MRITE         (57U)   /* Masked write data to SD iSDIO register */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV         (20U)	/* Get F/W revision */
#define ATA_GET_MODEL       (21U)	/* Get model name */
#define ATA_GET_SN          (22U)	/* Get serial number */

#ifdef __cplusplus
extern "C" {
#endif
/*---------------------------------------*/
/* Prototypes for disk control functions */
void    disk_deinitialize (BYTE pdrv);
DSTATUS disk_initialize (BYTE pdrv);
DSTATUS disk_status (BYTE pdrv);
DRESULT disk_read (BYTE pdrv, BYTE* buff, LBA_t sector, UINT count);
DRESULT disk_write (BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count);
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);

static inline bool disk_is_ready(BYTE pdrv)
{
    return ((disk_status(pdrv) & (STA_NOINIT | STA_NODISK)) == 0);
}


#ifdef __cplusplus
}
#endif

#endif
