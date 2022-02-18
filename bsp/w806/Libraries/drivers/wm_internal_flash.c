#include "wm_internal_flash.h"

FLASH_ProcessTypeDef pFlash;

static vu32 read_first_value(void)
{
    return M32(RSA_BASE);
}

static void writeEnable(void)
{
	FLASH->CMD_INFO = 0x6;
	FLASH->CMD_START = FLASH_CMD_START_CMD;
}

static unsigned char readRID(void)
{
	FLASH->CMD_INFO = 0x2c09F;
	FLASH->CMD_START = FLASH_CMD_START_CMD;
    return read_first_value() & 0xFF;
}

static void writeBpBit_for_1wreg(char cmp, char bp4, char bp3, char bp2, char bp1, char bp0)
{
    int status = 0;
    int bpstatus = 0;

    FLASH->CMD_INFO = 0x0C005;
    FLASH->CMD_START = FLASH_CMD_START_CMD;
    status =  read_first_value() & 0xFF;

    FLASH->CMD_INFO = 0x0C035;
    FLASH->CMD_START = FLASH_CMD_START_CMD;
    status  |=  (read_first_value() & 0xFF) << 8;

    /*Write Enable*/
    FLASH->CMD_INFO = 0x6;
    FLASH->CMD_START = FLASH_CMD_START_CMD;

    bpstatus  = (bp4 << 6) | (bp3 << 5) | (bp2 << 4) | (bp1 << 3) | (bp0 << 2);
    status      = (status & 0xBF83) | bpstatus | (cmp << 14);

    M32(RSA_BASE)  = status;
    FLASH->CMD_INFO = 0x1A001;
    FLASH->CMD_START = FLASH_CMD_START_CMD;
}

static void writeBpBit_for_2wreg(char cmp, char bp4, char bp3, char bp2, char bp1, char bp0)
{
    int status = 0;
    int bpstatus = 0;

    FLASH->CMD_INFO = 0x0C005;
    FLASH->CMD_START = FLASH_CMD_START_CMD;
    status =  read_first_value() & 0xFF;

    FLASH->CMD_INFO = 0x0C035;
    FLASH->CMD_START = FLASH_CMD_START_CMD;
    status  |=  (read_first_value() & 0xFF) << 8;

    /*Write Enable*/
    FLASH->CMD_INFO = 0x6;
    FLASH->CMD_START = FLASH_CMD_START_CMD;

    bpstatus  = (bp4 << 6) | (bp3 << 5) | (bp2 << 4) | (bp1 << 3) | (bp0 << 2);
    bpstatus      = (status & 0x83) | bpstatus;

    M32(RSA_BASE)  = bpstatus;
    FLASH->CMD_INFO = 0xA001;
    FLASH->CMD_START = FLASH_CMD_START_CMD;


    FLASH->CMD_INFO = 0x6;
    FLASH->CMD_START = FLASH_CMD_START_CMD;

    status      = ((status>>8) & 0xBF) | (cmp << 6);
    M32(RSA_BASE)   = status;
    FLASH->CMD_INFO  = 0xA031;
    FLASH->CMD_START = FLASH_CMD_START_CMD;	
}


static void writeESMTBpBit(char cmp, char bp4, char bp3, char bp2, char bp1, char bp0)
{
    int status = 0;
    int bpstatus = 0;

    FLASH->CMD_INFO = 0x0C005;
    FLASH->CMD_START = FLASH_CMD_START_CMD;
    status =  read_first_value() & 0xFF;
    bpstatus  = (bp4 << 6) | (bp3 << 5) | (bp2 << 4) | (bp1 << 3) | (bp0 << 2);
    status      = (status & 0x83) | bpstatus;

    /*Write Enable*/
    FLASH->CMD_INFO = 0x6;
    FLASH->CMD_START = FLASH_CMD_START_CMD;

    bpstatus  = (bp4 << 6) | (bp3 << 5) | (bp2 << 4) | (bp1 << 3) | (bp0 << 2);
    status      = (status & 0x83) | bpstatus | (cmp << 14);

    M32(RSA_BASE)  = status;
    FLASH->CMD_INFO = 0x0A001;
    FLASH->CMD_START = FLASH_CMD_START_CMD;


    FLASH->CMD_INFO = 0x0C085;
    FLASH->CMD_START = FLASH_CMD_START_CMD;
    status  =  read_first_value() & 0xFF;

    /*Write Enable*/
    FLASH->CMD_INFO = 0x6;
    FLASH->CMD_START = FLASH_CMD_START_CMD;

    status		= (status & 0xBF) | (cmp << 6);
    M32(RSA_BASE)  = status;
    FLASH->CMD_INFO = 0x0A0C1;
    FLASH->CMD_START = FLASH_CMD_START_CMD;
}

static int flashunlock(void)
{
    switch(readRID())
    {
    case SPIFLASH_MID_GD:
	case SPIFLASH_MID_TSINGTENG:
        writeBpBit_for_1wreg(0, 0, 0, 0, 0, 0);
        break;
    case SPIFLASH_MID_PUYA:
	case SPIFLASH_MID_XTX:
	case SPIFLASH_MID_BOYA:
	case SPIFLASH_MID_FUDANMICRO:
	case SPIFLASH_MID_XMC:
		writeBpBit_for_2wreg(0, 0, 0, 0, 0, 0);
		break;
    case SPIFLASH_MID_ESMT:
        writeESMTBpBit(0, 0, 0, 0, 0, 0);
        break;
    default:
        return -1;
    }
    return 0;
}

static int flashlock(void)
{
    switch(readRID())
    {
    case SPIFLASH_MID_GD:
	case SPIFLASH_MID_TSINGTENG:		
        writeBpBit_for_1wreg(0, 1, 1, 0, 1, 0);
		break;
    case SPIFLASH_MID_PUYA:
	case SPIFLASH_MID_XTX:
	case SPIFLASH_MID_BOYA:
	case SPIFLASH_MID_FUDANMICRO:
	case SPIFLASH_MID_XMC:
		writeBpBit_for_2wreg(0, 1, 1, 0, 1, 0);
        break;
    case SPIFLASH_MID_ESMT:
        writeESMTBpBit(0, 1, 1, 0, 1, 0);
        break;
    default:
        return -1;/*do not clear QIO Mode*/
    }
    return 0;
}

static int programSR(unsigned int  cmd, unsigned long addr, unsigned char *buf,  unsigned int sz)
{
    unsigned long base_addr = 0;
    unsigned int size = 0;


    if (sz > INSIDE_FLS_PAGE_SIZE)
    {
        sz = INSIDE_FLS_PAGE_SIZE;
    }

    base_addr = RSA_BASE;
    size = sz;
    while(size)
    {
        M32(base_addr) = *((unsigned long *)buf);
        base_addr += 4;
        buf += 4;
        size -= 4;
    }

    writeEnable();
    FLASH->CMD_INFO = cmd | ((sz - 1) << 16);
    FLASH->ADDR = (addr & 0x1FFFFFF);
    FLASH->CMD_START = FLASH_CMD_START_CMD;

    return 0;
}


static int programPage (unsigned long adr, unsigned long sz, unsigned char *buf)
{
    programSR(0x80009002, adr, buf, sz);
    return(0);
}

static int eraseSR(unsigned int cmd, unsigned long addr)
{
    /*Write Enable*/
    writeEnable();
    FLASH->CMD_INFO = cmd;
    FLASH->ADDR = (addr & 0x1FFFFFF);
    FLASH->CMD_START = FLASH_CMD_START_CMD;

    return 0;
}

static int eraseSector (unsigned long adr)
{
    eraseSR(0x80000820, adr);

    return (0);                                  				// Finished without Errors
}

static unsigned int getFlashDensity(void)
{
    unsigned char density = 0;

    FLASH->CMD_INFO = 0x2c09F;
    FLASH->CMD_START = FLASH_CMD_START_CMD;

    density = ((read_first_value() & 0xFFFFFF) >> 16) & 0xFF;
    //	printf("density %x\n", density);
    if (density && (density <= 0x21))  /*just limit to (1<<33UL) Byte*/
    {
        return (1 << density);
    }

    return 0;
}

/*sr end*/

static int __readByCMD(unsigned char cmd, unsigned long addr, unsigned char *buf, unsigned long sz)
{
    int i = 0;
    int word = sz / 4;
    int byte = sz % 4;
    unsigned long addr_read;
	if (!(FLASH->FLASH_CR&0x1))/*non-QIO mode, only single line command can be used*/
	{
		if (cmd > 0x0B)
		{
			cmd = 0x0B;
		}
	}
	
    switch (cmd)
    {
    case 0x03:
        FLASH->CMD_INFO = 0x8000C003 | (((sz - 1) & 0x3FF) << 16);
        FLASH->ADDR = addr & 0x1FFFFFF;
        FLASH->CMD_START = FLASH_CMD_START_CMD;
        break;
    case 0x0B:
        if((FLASH->FLASH_CR & 0x2) == 0x2)
        {
            FLASH->CMD_INFO = 0xB400C00B | (((sz - 1) & 0x3FF) << 16);
        }
        else
        {
            FLASH->CMD_INFO = 0xBC00C00B | (((sz - 1) & 0x3FF) << 16);
        }
        FLASH->ADDR = addr & 0x1FFFFFF;
        FLASH->CMD_START = FLASH_CMD_START_CMD;
        break;
    case 0xBB:
        FLASH->CMD_INFO = 0xE400C0BB | (((sz - 1) & 0x3FF) << 16);
        FLASH->ADDR = addr & 0x1FFFFFF;
        FLASH->CMD_START = FLASH_CMD_START_CMD;
        break;

    case 0xEB:
        FLASH->CMD_INFO = 0xEC00C0EB | (((sz - 1) & 0x3FF) << 16);
        FLASH->ADDR = addr & 0x1FFFFFF;
        FLASH->CMD_START = FLASH_CMD_START_CMD;
        break;


    default:
        return -1;
    }

    //	printf("delay delay delay delay\n");
    //	dumpUint32("readByCMD RSA_BASE", RSA_BASE, sz/4);
    addr_read = RSA_BASE;
    for(i = 0; i < word; i ++)
    {
        M32(buf) = M32(addr_read);
        buf += 4;
        addr_read += 4;
    }

    if(byte > 0)
    {
        M32(buf) = M32(addr_read);
        buf += 3;							//point last byte
        byte = 4 - byte;
        while(byte)
        {
            *buf = 0;
            buf --;
            byte --;
        }
    }
    return 0;
}

static int flashRead(unsigned long addr, unsigned char *buf, unsigned long sz)
{

    unsigned int flash_addr;
    unsigned int sz_pagenum = 0;
    unsigned int sz_remain = 0;
    int i = 0;
    int page_offset = addr & (INSIDE_FLS_PAGE_SIZE - 1);

    static char cache[INSIDE_FLS_PAGE_SIZE] = {0};
	
    flash_addr = addr & ~(INSIDE_FLS_PAGE_SIZE - 1);
    __readByCMD(0xEB, flash_addr, (unsigned char *)cache, INSIDE_FLS_PAGE_SIZE);
    if (sz > INSIDE_FLS_PAGE_SIZE - page_offset)
    {
        memcpy(buf, cache + page_offset, INSIDE_FLS_PAGE_SIZE - page_offset);
        buf += INSIDE_FLS_PAGE_SIZE - page_offset;
        flash_addr 	+= INSIDE_FLS_PAGE_SIZE;

        sz_pagenum = (sz - (INSIDE_FLS_PAGE_SIZE - page_offset)) / INSIDE_FLS_PAGE_SIZE;
        sz_remain = (sz - (INSIDE_FLS_PAGE_SIZE - page_offset)) % INSIDE_FLS_PAGE_SIZE;
        for (i = 0; i < sz_pagenum; i++)
        {

            __readByCMD(0xEB, flash_addr, (unsigned char *)cache, INSIDE_FLS_PAGE_SIZE);
            memcpy(buf, cache, INSIDE_FLS_PAGE_SIZE);
            buf 		+= INSIDE_FLS_PAGE_SIZE;
            flash_addr 	+= INSIDE_FLS_PAGE_SIZE;
        }

        if (sz_remain)
        {
            __readByCMD(0xEB, flash_addr, (unsigned char *)cache, sz_remain);
            memcpy(buf, cache, sz_remain);
        }
    }
    else
    {
        memcpy(buf, cache + page_offset, sz);
    }

    return 0;
}

/**
 * @brief           This function is used to unlock flash protect area [0x0~0x2000].
 *
 * @param	   None
 *
 * @return         None
 *
 * @note           None
 */
int tls_flash_unlock(void)
{
    return flashunlock();
}

/**
 * @brief           This function is used to lock flash protect area [0x0~0x2000].
 *
 * @param	   None
 *
 * @return         None
 *
 * @note           None
 */
int tls_flash_lock(void)
{
    return flashlock();
}

/**
 * @brief          This function is used to read data from the flash.
 *
 * @param[in]      addr                 is byte offset addr for read from the flash.
 * @param[in]      buf                   is user for data buffer of flash read
 * @param[in]      len                   is byte length for read.
 *
 * @retval         HAL_OK	    	if read sucsess
 * @retval         HAL_ERROR	    if read fail
 *
 * @note           None
 */
HAL_StatusTypeDef HAL_FLASH_Read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    int err;

    if (((addr & (INSIDE_FLS_BASE_ADDR - 1)) >=  getFlashDensity()) || (len == 0) || (buf == NULL))
    {
        return HAL_ERROR;
    }

    __HAL_LOCK(&pFlash);

    flashRead(addr, buf, len);

    err = HAL_OK;
    __HAL_UNLOCK(&pFlash);
    return err;
}

/**
 * @brief          This function is used to write data to the flash.
 *
 * @param[in]      addr     is byte offset addr for write to the flash
 * @param[in]      buf       is the data buffer want to write to flash
 * @param[in]      len       is the byte length want to write
 *
 * @retval         HAL_OK	           	if write flash success
 * @retval         HAL_ERROR	    	if argument is invalid
 *
 * @note           None
 */
HAL_StatusTypeDef HAL_FLASH_Write(uint32_t addr, uint8_t *buf, uint32_t len)
{
    static uint8_t cache[INSIDE_FLS_SECTOR_SIZE] = {0};
    unsigned int secpos;
    unsigned int secoff;
    unsigned int secremain;
    unsigned int i;
    unsigned int offaddr;

    if (((addr & (INSIDE_FLS_BASE_ADDR - 1)) >=  getFlashDensity()) || (len == 0) || (buf == NULL))
    {
        return HAL_ERROR;
    }

    __HAL_LOCK(&pFlash);

    offaddr = addr & (INSIDE_FLS_BASE_ADDR - 1);			
    secpos = offaddr / INSIDE_FLS_SECTOR_SIZE;				
    secoff = (offaddr % INSIDE_FLS_SECTOR_SIZE);			
    secremain = INSIDE_FLS_SECTOR_SIZE - secoff;    
    if(len <= secremain)
    {
        secremain = len;								
    }
    while (1)
    {
        flashRead(secpos * INSIDE_FLS_SECTOR_SIZE, cache, INSIDE_FLS_SECTOR_SIZE);

        eraseSector(secpos * INSIDE_FLS_SECTOR_SIZE);
        for (i = 0; i < secremain; i++) 
        {
            cache[i + secoff] = buf[i];
        }
        for (i = 0; i < (INSIDE_FLS_SECTOR_SIZE / INSIDE_FLS_PAGE_SIZE); i++)
        {
            programPage(secpos * INSIDE_FLS_SECTOR_SIZE + i * INSIDE_FLS_PAGE_SIZE, INSIDE_FLS_PAGE_SIZE, &cache[i * INSIDE_FLS_PAGE_SIZE]);	
        }
        if(len == secremain)
        {
            break;              
        }
        else                    
        {
            secpos++;           
            secoff = 0;         
            buf += secremain;   
            len -= secremain;
            if(len > (INSIDE_FLS_SECTOR_SIZE))
                secremain = INSIDE_FLS_SECTOR_SIZE; 
            else
                secremain = len;					
        }
    }

    __HAL_UNLOCK(&pFlash);
    return HAL_OK;
}


