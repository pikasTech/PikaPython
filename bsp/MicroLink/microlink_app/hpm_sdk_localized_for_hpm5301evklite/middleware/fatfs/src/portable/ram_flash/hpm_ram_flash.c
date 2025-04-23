#include "hpm_ram_flash.h"
#include "hpm_romapi.h"
#include "diskio.h"
#include "board.h"

xpi_nor_config_t s_xpi_nor_config;
static uint32_t flash_size;
static uint32_t sector_size;
static uint32_t page_size;
DSTATUS flash_disk_initialize(BYTE pdrv)
{
    /* the spi hardware has been initialized before the sd disk init*/

    hpm_stat_t sta;
    if (pdrv != DEV_RAM) {
        return STA_NOINIT;
    }
    xpi_nor_config_option_t option;
    option.header.U = BOARD_APP_XPI_NOR_CFG_OPT_HDR;
    option.option0.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT0;
    option.option1.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT1;
    sta = rom_xpi_nor_auto_config(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, &option);
    rom_xpi_nor_get_property(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, xpi_nor_property_total_size, &flash_size);
    rom_xpi_nor_get_property(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, xpi_nor_property_sector_size, &sector_size);
    rom_xpi_nor_get_property(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, xpi_nor_property_page_size, &page_size);
    return RES_OK;
}

DSTATUS flash_disk_status(BYTE pdrv)
{
    if (pdrv != DEV_RAM) {
        return STA_NOINIT;
    }
    return RES_OK;
}

DSTATUS flash_disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_RAM) {
        return RES_PARERR;
    }

    if (rom_xpi_nor_read(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, (uint32_t *)buff, (FAT_ON_CHIP_FLASH_OFFSET + sector_size*sector), count * sector_size) != status_success) {
        return RES_ERROR;
    }

    return RES_OK;
}

DSTATUS flash_disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_RAM) {
        return RES_PARERR;
    }
    if (rom_xpi_nor_erase(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config,(FAT_ON_CHIP_FLASH_OFFSET + sector_size*sector), count * sector_size) != status_success) {
        return RES_ERROR;
    }
    if (rom_xpi_nor_program(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, (uint32_t *)buff, (FAT_ON_CHIP_FLASH_OFFSET + sector_size*sector), count * sector_size) != status_success) {
        return RES_ERROR;
    }

    return RES_OK;
}

DRESULT flash_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    DRESULT result = RES_PARERR;
    do {
        HPM_BREAK_IF((pdrv != DEV_RAM) || ((cmd != CTRL_SYNC) && (buff == NULL)));
        result = RES_OK;
        switch (cmd) {
        case GET_SECTOR_COUNT:
            *(uint32_t *) buff = FAT_ON_CHIP_FLASH_BLOCK_NUM;
            break;
        case GET_SECTOR_SIZE:
            *(uint32_t *) buff = 4096;
            break;
        case GET_BLOCK_SIZE:
            *(uint32_t *) buff = 4096;
            break;
        case CTRL_SYNC:
            result = RES_OK;
            break;
        default:
            result = RES_PARERR;
            break;
        }

    } while (false);

    return result;
}
