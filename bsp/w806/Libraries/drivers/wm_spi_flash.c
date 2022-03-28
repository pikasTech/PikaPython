#include "wm_spi_flash.h"

static SPI_HandleTypeDef hspi;
static uint32_t total_size = 0;
static uint8_t cache[SECTOR_SIZE];


static int fls_drv_read_id(uint8_t *id)
{
	uint8_t cmd = EXFLASH_ID;
	uint8_t rx[3];
	int err;
	
	if (id == NULL)
	{
		return HAL_ERROR;
	}
	
	__HAL_SPI_SET_CS_LOW(&hspi);
	err = HAL_SPI_Transmit(&hspi, &cmd, 1, 100);
	if (err != HAL_OK)
	{
		__HAL_SPI_SET_CS_HIGH(&hspi);
		return err;
	}
	err = HAL_SPI_Receive(&hspi, rx, 3, 100);
	if (err != HAL_OK)
	{
		__HAL_SPI_SET_CS_HIGH(&hspi);
		return err;
	}
	__HAL_SPI_SET_CS_HIGH(&hspi);
	
	memcpy(id, rx, 3);
	return HAL_OK;
}

static int fls_drv_init(void)
{
	uint8_t buf[3];
	int err;
	
	hspi.Instance = SPI;
	hspi.Init.Mode = SPI_MODE_MASTER;
	hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi.Init.NSS = SPI_NSS_SOFT;
	hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	
	if (HAL_SPI_Init(&hspi) != HAL_OK)
	{
		return HAL_ERROR;
	}
	err = fls_drv_read_id(buf);
	if (err != HAL_OK)
	{
		return err;
	}
	if (buf[2])
	{
		total_size = (1 << buf[2]);
	}
	else
	{
		return HAL_ERROR;
	}
	
	return HAL_OK;
}

static int fls_drv_write_enable(void)
{
	uint8_t cmd;
	int err;
	
	cmd = EXFLASH_WRITE_ENABLE;
	__HAL_SPI_SET_CS_LOW(&hspi);
	err = HAL_SPI_Transmit(&hspi, (uint8_t *)&cmd, 1, 100);
	__HAL_SPI_SET_CS_HIGH(&hspi);
	if (err != HAL_OK)
	{
		return err;
	}
	
	return HAL_OK;
}

static int fls_drv_wait_write_enable(void)
{
	uint8_t cmd, sr = 0;
	int err;
	
	cmd = EXFLASH_READ_SR1;
	do {
		__HAL_SPI_SET_CS_LOW(&hspi);
		err = HAL_SPI_Transmit(&hspi, (uint8_t *)&cmd, 1, 100);
		if (err != HAL_OK)
		{
			__HAL_SPI_SET_CS_HIGH(&hspi);
			return err;
		}
		err = HAL_SPI_Receive(&hspi, (uint8_t *)&sr, 1, 100);
		if (err != HAL_OK)
		{
			__HAL_SPI_SET_CS_HIGH(&hspi);
			return err;
		}
		__HAL_SPI_SET_CS_HIGH(&hspi);
		
		if (sr & EXFLASH_STATUS_WEL)
		{
			break;
		}
	} while (1);
	
	return HAL_OK;
}

static int fls_drv_wait_flash_ready(void)
{
	uint8_t cmd, sr = 0;
	int err;
	
	cmd = EXFLASH_READ_SR1;
	do {
		__HAL_SPI_SET_CS_LOW(&hspi);
		err = HAL_SPI_Transmit(&hspi, (uint8_t *)&cmd, 1, 100);
		if (err != HAL_OK)
		{
			__HAL_SPI_SET_CS_HIGH(&hspi);
			return err;
		}
		err = HAL_SPI_Receive(&hspi, (uint8_t *)&sr, 1, 100);
		if (err != HAL_OK)
		{
			__HAL_SPI_SET_CS_HIGH(&hspi);
			return err;
		}
		__HAL_SPI_SET_CS_HIGH(&hspi);
		
		if ((sr & EXFLASH_STATUS_BUSY) == 0x00)
		{
			break;
		}
	} while (1);
	
	return HAL_OK;
}

static int fls_drv_read(uint32_t addr, uint8_t *buf, uint32_t len)
{
	uint32_t cmd = 0;
	int err;
	
	cmd |= EXFLASH_READ_DATA;
	cmd |= (swap32(addr) & 0xFFFFFF00);
	__HAL_SPI_SET_CS_LOW(&hspi);
	err = HAL_SPI_Transmit(&hspi, (uint8_t *)&cmd, 4, 100);
	if (err != HAL_OK)
	{
		__HAL_SPI_SET_CS_HIGH(&hspi);
		return err;
	}
	err = HAL_SPI_Receive(&hspi, buf, len, 1000);
	if (err != HAL_OK)
	{
		__HAL_SPI_SET_CS_HIGH(&hspi);
		return err;
	}
	__HAL_SPI_SET_CS_HIGH(&hspi);
	
	return HAL_OK;
}

static int fls_drv_page_write(uint32_t page, uint8_t *buf)
{
	uint32_t cmd = 0;
	int err;
	
	err = fls_drv_write_enable();
	if (err != HAL_OK)
	{
		return err;
	}
	
	err = fls_drv_wait_write_enable();
	if (err != HAL_OK)
	{
		return err;
	}
	
	cmd |= EXFLASH_PAGE_PROGRAM;
	cmd |= (swap32(page * PAGE_SIZE) & 0xFFFFFF00);
	__HAL_SPI_SET_CS_LOW(&hspi);
	err = HAL_SPI_Transmit(&hspi, (uint8_t *)&cmd, 4, 100);
	if (err != HAL_OK)
	{
		__HAL_SPI_SET_CS_HIGH(&hspi);
		return err;
	}
	err = HAL_SPI_Transmit(&hspi, buf, PAGE_SIZE, 1000);
	if (err != HAL_OK)
	{
		__HAL_SPI_SET_CS_HIGH(&hspi);
		return err;
	}
	__HAL_SPI_SET_CS_HIGH(&hspi);
	
	err = fls_drv_wait_flash_ready();
	if (err != HAL_OK)
	{
		return err;
	}
	return HAL_OK;
}

static int fls_drv_erase(uint32_t sector)
{
	uint32_t cmd = 0;
	int err;
	
	err = fls_drv_write_enable();
	if (err != HAL_OK)
	{
		return err;
	}
	
	err = fls_drv_wait_write_enable();
	if (err != HAL_OK)
	{
		return err;
	}
	
	cmd |= EXFLASH_SECTOR_ERASE;
	cmd |= (swap32(sector * SECTOR_SIZE) & 0xFFFFFF00);
	__HAL_SPI_SET_CS_LOW(&hspi);
	err = HAL_SPI_Transmit(&hspi, (uint8_t *)&cmd, 4, 100);
	__HAL_SPI_SET_CS_HIGH(&hspi);
	if (err != HAL_OK)
	{
		return err;
	}
	err = fls_drv_wait_flash_ready();
	if (err != HAL_OK)
	{
		return err;
	}
	return HAL_OK;
}

static int fls_drv_chip_erase(void)
{
	uint32_t cmd = 0;
	int err;
	
	err = fls_drv_write_enable();
	if (err != HAL_OK)
	{
		return err;
	}
	
	err = fls_drv_wait_write_enable();
	if (err != HAL_OK)
	{
		return err;
	}
	cmd = EXFLASH_CIHP_ERASE;
	__HAL_SPI_SET_CS_LOW(&hspi);
	err = HAL_SPI_Transmit(&hspi, (uint8_t *)&cmd, 1, 100);
	__HAL_SPI_SET_CS_HIGH(&hspi);
	if (err != HAL_OK)
	{
		return err;
	}
	err = fls_drv_wait_flash_ready();
	if (err != HAL_OK)
	{
		return err;
	}
	return HAL_OK;
}

int SPIFLS_Init(void)
{
	int err;
	
	err = fls_drv_init();
	if (err != HAL_OK)
	{
		return err;
	}
	
	return HAL_OK;
}

int SPIFLS_Read_ID(uint8_t *id)
{
	uint8_t rx[3];
	int err;
	
	err = fls_drv_read_id(rx);
	if (err != HAL_OK)
	{
		return err;
	}
	*id = rx[0];
	
	return HAL_OK;
}

int SPIFLS_Page_Write(uint32_t page, uint8_t *buf, uint32_t page_cnt)
{
	int i = 0;
	int err;
	
	for (i = 0; i < page_cnt; i++)
	{
		err = fls_drv_page_write(page + i, buf + i * PAGE_SIZE);
		if (err != HAL_OK)
		{
			return err;
		}
	}
	
	return HAL_OK;
}

int SPIFLS_Read(uint32_t addr, uint8_t *buf, uint32_t len)
{
	int err;
	
	if ((addr > total_size) || (buf == NULL) || (len == 0) || ((addr + len) > total_size))
	{
		return HAL_ERROR;
	}
	
	err = fls_drv_read(addr, buf, len);
	if (err != HAL_OK)
	{
		return err;
	}
	
	return HAL_OK;
}

int SPIFLS_Write(uint32_t addr, uint8_t *buf, uint32_t len)
{
	uint32_t sector_addr, sector_num, i, j;
	int err;
	
	if ((addr > total_size) || (buf == NULL) || (len == 0) || ((addr + len) > total_size))
	{
		return HAL_ERROR;
	}
	
	sector_addr = addr / SECTOR_SIZE;
	sector_num = (addr + len - 1) / SECTOR_SIZE - sector_addr + 1;
	
	for (i = 0; i < sector_num; i++)
	{
		err = fls_drv_read((sector_addr + i) * SECTOR_SIZE, cache, SECTOR_SIZE);
		if (err != HAL_OK)
		{
			return err;
		}
		if (sector_num == 1)
		{
			memcpy(cache + (addr % SECTOR_SIZE), buf, len);
			buf += len;
			len = 0;
		}
		else
		{
			if (i == 0)
			{
				memcpy(cache + (addr % SECTOR_SIZE), buf, SECTOR_SIZE - (addr % SECTOR_SIZE));
				buf += (SECTOR_SIZE - (addr % SECTOR_SIZE));
				len -= (SECTOR_SIZE - (addr % SECTOR_SIZE));
			}
			else if (i == (sector_num - 1))
			{
				memcpy(cache, buf, len);
				buf += len;
				len = 0;
			}
			else
			{
				memcpy(cache, buf, SECTOR_SIZE);
				buf += SECTOR_SIZE;
				len -= SECTOR_SIZE;
			}
		}
		
		err = fls_drv_erase(sector_addr + i);
		if (err != HAL_OK)
		{
			return err;
		}
		
		for (j = 0; j < (SECTOR_SIZE / PAGE_SIZE); j++)
		{
			err = fls_drv_page_write((sector_addr + i) * (SECTOR_SIZE / PAGE_SIZE) + j, cache + j * PAGE_SIZE);
			if (err != HAL_OK)
			{
				return err;
			}
		}
	}
	
	return HAL_OK;
}

int SPIFLS_Erase(uint32_t sector)
{
	int err;
	
	if (sector > (total_size / SECTOR_SIZE))
	{
		return HAL_ERROR;
	}
	err = fls_drv_erase(sector);
	if (err != HAL_OK)
	{
		return err;
	}
	
	return HAL_OK;
}

int SPIFLS_Chip_Erase(void)
{
	int err;
	
	err = fls_drv_chip_erase();
	if (err != HAL_OK)
	{
		return err;
	}
	
	return HAL_OK;
}