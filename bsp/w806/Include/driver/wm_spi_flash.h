#ifndef __WM_SPI_FLASH_H__
#define __WM_SPI_FLASH_H__

#include "wm_hal.h"

#define PAGE_SIZE	256
#define SECTOR_SIZE	4096

#define EXFLASH_ID				(0x9F)
#define EXFLASH_READ_DATA		(0x03)
#define EXFLASH_PAGE_PROGRAM	(0x02)
#define	EXFLASH_SECTOR_ERASE	(0x20)
#define EXFLASH_CIHP_ERASE		(0xC7)
#define EXFLASH_WRITE_ENABLE	(0x06)
#define EXFLASH_READ_SR1		(0x05)
#define EXFLASH_READ_SR2		(0x35)

#define EXFLASH_STATUS_BUSY		(1 << 0)
#define EXFLASH_STATUS_WEL		(1 << 1)

#define swap32(a) (((a & 0xFF) << 24) | ((a & 0xFF00) << 8) | ((a & 0xFF0000) >> 8) | (a >> 24))

int SPIFLS_Init(void);

int SPIFLS_Read_ID(uint8_t *id);

int SPIFLS_Read(uint32_t addr, uint8_t *buf, uint32_t len);

int SPIFLS_Write(uint32_t addr, uint8_t *buf, uint32_t len);

int SPIFLS_Erase(uint32_t sector);

int SPIFLS_Chip_Erase(void);

#endif