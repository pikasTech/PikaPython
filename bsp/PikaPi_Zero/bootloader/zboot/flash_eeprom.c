#include "flash_eeprom.h"
#include "zboot_misc.h"

static struct {
    unsigned long base;
    unsigned short page_size;
} g;

#define MAIN_ADDR(x) (g.base + (x))
#define EEPROM_SIZE (g.page_size / 4)   // 384/768bytes for 1k/2k page
#define MAX_DUMMY_ADDR (EEPROM_SIZE - 2)
#define LOAD_ADDR(x) (*(unsigned short*)MAIN_ADDR(x))
#define LOAD_DATA(x) (*(unsigned short*)MAIN_ADDR((x) + 2))

static int GetFirstEmptyAddr(void)
{
    unsigned short i;
    for(i = 0; i < g.page_size; i += 4) {
        if(*(unsigned long*)MAIN_ADDR(i) == 0xffffffff)
            return i;
    }
    return -1;  // Full
}

static void Rotate(void)
{
    unsigned short buf[FLASH_PAGE_SIZE / 8]; // 128 for 1k, 256 for 2k, 2k for 16k
    FLASH_EEPROM_ReadData(0, buf, EEPROM_SIZE);

    FLASH_Unlock();
    FLASH_ErasePage(g.base);
    for(int i = 0; i < EEPROM_SIZE; i += 2) {
        FLASH_ProgramHalfWord(MAIN_ADDR(i * 2), i);
        FLASH_ProgramHalfWord(MAIN_ADDR(i * 2) + 2, buf[i / 2]);
    }
    FLASH_Lock();
}

void FLASH_EEPROM_WriteWord(unsigned short dummy_addr, unsigned short data)
{
    if(dummy_addr > MAX_DUMMY_ADDR) {
        return;     // do nothing
    }
    int addr = GetFirstEmptyAddr();
    if(addr == -1) {            // Page full, rotate needed
        Rotate();
        addr = GetFirstEmptyAddr();
    }
    unsigned org_data = FLASH_EEPROM_ReadWord(dummy_addr);
    if(org_data == data)
        return;            // skip writing if data unchanged

    FLASH_Unlock();
    FLASH_ProgramHalfWord(MAIN_ADDR(addr), dummy_addr);
    FLASH_ProgramHalfWord(MAIN_ADDR(addr) + 2, data);
    FLASH_Lock();
}

unsigned short FLASH_EEPROM_ReadWord(unsigned short dummy_addr)
{
    int addr = GetFirstEmptyAddr();
    if(addr == 0 || addr == -1)
        addr = g.page_size - 4; // position to the end
    while(addr >= 0) {
        if(LOAD_ADDR(addr) == dummy_addr)
            return LOAD_DATA(addr);
        addr -= 4;
    }
    return 0xffff;              // Dummy_addr not found
}

void FLASH_EEPROM_WriteData(unsigned short addr, void* data, int nbytes)
{
    if(nbytes % 2 != 0)
        nbytes++;
    while(nbytes > 0) {
        FLASH_EEPROM_WriteWord(addr, *(unsigned short*)data);
        addr += 2;
        data += 2;
        nbytes -= 2;
    }
}

void FLASH_EEPROM_ReadData(unsigned short addr, void* data, int nbytes)
{
    addr &= (~1);
    if(nbytes % 2 != 0)
        nbytes++;
    while(nbytes > 0) {
        *(unsigned short*)data = FLASH_EEPROM_ReadWord(addr);
        addr += 2;
        data += 2;
        nbytes -= 2;
    }
}

unsigned short FLASH_EEPROM_GetSize(void)
{
    return EEPROM_SIZE;
}

void FLASH_EEPROM_Config(unsigned long base_addr, unsigned short page_size)
{
    g.base = base_addr;
    g.page_size = page_size;
}

void FLASH_EEPROM_EraseAll(void)
{
    FLASH_Unlock();
    FLASH_ErasePage(g.base);
    FLASH_Lock();
}
