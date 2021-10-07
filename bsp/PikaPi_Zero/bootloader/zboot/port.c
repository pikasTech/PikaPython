#include "zboot_misc.h"
#include "xprintf.h"

#if defined (STM32F401xx) || defined (STM32F40_41xxx)

void FLASH_ErasePage(unsigned long addr)
{
//    xprintf("%08X\n", addr);
    addr -= FLASH_BASE;
    if(addr >= 0 && addr < 0x4000)
        FLASH_EraseSector(FLASH_Sector_0, VoltageRange_3);
    else if(addr >= 0x4000 && addr < 0x8000)
        FLASH_EraseSector(FLASH_Sector_1, VoltageRange_3);
    else if(addr >= 0x8000 && addr < 0xc000)
        FLASH_EraseSector(FLASH_Sector_2, VoltageRange_3);
    else if(addr >= 0xc000 && addr < 0x10000)
        FLASH_EraseSector(FLASH_Sector_3, VoltageRange_3);
    else if(addr >= 0x10000 && addr < 0x20000)
        FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3);
    else if(addr >= 0x20000 && addr < 0x40000)
        FLASH_EraseSector(FLASH_Sector_5, VoltageRange_3);
}

#endif
