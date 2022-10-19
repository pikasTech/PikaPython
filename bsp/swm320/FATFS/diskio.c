/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h" /* FatFs lower layer API */
#include "dev_sdio.h"
#include "sfud.h"

/* 为每个设备定义一个物理编号 */
#define DEV_FLASH 0 /* Example: Map SPI FLASH to physical drive 0 */
#define DEV_MMC 1   /* Example: Map MMC/SD card to physical drive 1 */

#define FLASH_SECTOR_SIZE 4096
/*-----------------------------------------------------------------------*/
/* 设备初始化                                                            */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize(
    BYTE pdrv /* 物理编号 */
)
{
    DSTATUS status = STA_NOINIT;

    switch (pdrv)
    {
    case DEV_MMC: /* SD CARD */
        if (sdcard_init() == SD_RES_OK)
        {
            status = RES_OK;
        }
        break;
    case DEV_FLASH: /* SPI FLASH */
        if (sfud_init() == SFUD_SUCCESS)
        {
            status = RES_OK;
        }
        status = RES_OK;
        break;
    default:
        break;
    }

    return status;
}

/*-----------------------------------------------------------------------*/
/* 获取设备状态                                                          */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status(
    BYTE pdrv /* 物理编号 */
)
{
    DSTATUS status = STA_NOINIT;

    switch (pdrv)
    {
    case DEV_MMC: /* SD CARD */
        status &= ~STA_NOINIT;
        break;
    case DEV_FLASH: /* SPI FLASH */
        status &= ~STA_NOINIT;
        break;
    default:
        break;
    }

    return status;
}
/*-----------------------------------------------------------------------*/
/* 读扇区：读取扇区内容到指定存储区                                              */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(
    BYTE pdrv,    /* 设备物理编号(0..) */
    BYTE *buff,   /* 数据缓存区 */
    DWORD sector, /* 扇区首地址 */
    UINT count    /* 扇区个数(1..128) */
)
{
    DRESULT status = RES_PARERR;

    switch (pdrv)
    {
    case DEV_MMC: /* SD CARD */
        if (count == 1)
        {
            SDIO_BlockRead(sector, (uint32_t *)buff);
        }
        else
        {
            SDIO_MultiBlockRead(sector, count, (uint32_t *)buff);
        }
        status = RES_OK;
        break;
    case DEV_FLASH: /* SPI FLASH */
    {
        sfud_err result = SFUD_SUCCESS;
        sfud_flash *spi_flash = sfud_get_device(SFUD_W25_DEVICE_INDEX);
        result = sfud_read(spi_flash, sector * FLASH_SECTOR_SIZE, count * FLASH_SECTOR_SIZE, buff);
        if (result == SFUD_SUCCESS)
        {
            status = RES_OK;
        }
        else
        {
            status = RES_ERROR;
        }
    }
    break;
    default:
        break;
    }

    return status;
}

DRESULT disk_write(
    BYTE pdrv,        /* 设备物理编号(0..) */
    const BYTE *buff, /* 欲写入数据的缓存区 */
    DWORD sector,     /* 扇区首地址 */
    UINT count        /* 扇区个数(1..128) */
)
{
    DRESULT status = RES_PARERR;

    if (!count)
    {
        return RES_PARERR; /* Check parameter */
    }

    switch (pdrv)
    {
    case DEV_MMC: /* SD CARD */
        if (count == 1)
        {
            SDIO_BlockWrite(sector, (uint32_t *)buff);
        }
        else
        {
            SDIO_MultiBlockWrite(sector, count, (uint32_t *)buff);
        }
        status = RES_OK;
        break;
    case DEV_FLASH: /* SPI FLASH */
    {
        sfud_err result = SFUD_SUCCESS;
        sfud_flash *spi_flash = sfud_get_device(SFUD_W25_DEVICE_INDEX);
        sfud_erase_write(spi_flash, sector * FLASH_SECTOR_SIZE, count * FLASH_SECTOR_SIZE, buff);
        if (result == SFUD_SUCCESS)
        {
            status = RES_OK;
        }
        else
        {
            status = RES_ERROR;
        }
    }
    break;
    default:
        break;
    }

    return status;
}

DRESULT disk_ioctl(
    BYTE pdrv, /* 物理编号 */
    BYTE cmd,  /* 控制指令 */
    void *buff /* 写入或者读取数据地址指针 */
)
{
    DRESULT status = RES_PARERR;
    switch (pdrv)
    {
    case DEV_MMC: /* SD CARD */
        switch (cmd)
        {
        // Get R/W sector size (WORD)
        case GET_SECTOR_SIZE:
            *(WORD *)buff = 512;
            break;

        // Get erase block size in unit of sector (DWORD)
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = SD_cardInfo.CardBlockSize;
            break;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = SD_cardInfo.CardCapacity / 512;
            break;

        case CTRL_SYNC:
        default:
            break;
        }

        status = RES_OK;
        break;
    case DEV_FLASH: /* SPI FLASH */
        switch (cmd)
        {
        // Get R/W sector size (WORD)
        case GET_SECTOR_SIZE:
            *(WORD *)buff = FLASH_SECTOR_SIZE;
            break;

        // Get erase block size in unit of sector (DWORD)
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            break;

        case GET_SECTOR_COUNT:
        {
            sfud_flash *spi_flash = sfud_get_device(SFUD_W25_DEVICE_INDEX);
            *(DWORD *)buff = spi_flash->chip.capacity / FLASH_SECTOR_SIZE;
        }
        break;

        case CTRL_SYNC:
        default:
            break;
        }

        status = RES_OK;
        break;
    default:
        status = RES_PARERR;
    }

    return status;
}
