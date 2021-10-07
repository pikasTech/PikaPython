#include "zboot_misc.h"
#include <string.h>
#include <stdbool.h>
#include "usart.h"
#include "iap.h"

enum {
    IAP_GET_MAX_APPSIZE = 0x80, IAP_READ_FLASH = 0x81, IAP_GET_APPSIZE = 0x82,
    IAP_READ_APP_CRC32 = 0x83, IAP_ERASE_APP = 0xc0, IAP_WRITE_FLASH = 0xc1,
    IAP_WRITE_APP_CRC32 = 0xc2, IAP_JUMP_TO_APP = 0xd0,
};

typedef void (*func_app)(void);
volatile func_app func;

/*
static void* memcpy_s(void* dest, const void* src, size_t size)
{
    char *d = dest;
    const char *s = src;
    while(size--)
        *d++ = *s++;
    return dest;
}
*/

const unsigned long polynormial = 0xedb88320;
static unsigned long CRC32(unsigned long crc, unsigned char* msg, int size)
{
    for(register int i = 0; i < size; i++) {
        crc ^= msg[i];
        for(register int j = 0; j < 8; j++) {
            bool lsb = crc % 2;
            crc >>= 1;
            if(lsb)
                crc ^= polynormial;
        }
    }
    return crc;
}

static IAP_Sysinfo_t g;

int IAP_CheckEmpty(void* addr, int size)
{
    while(size > 0) {
        if(*(unsigned long*)addr != 0xffffffff)
            return 0;
        size -= 4;
        addr += 4;
    }
    return 1;
}

void IAP_Config(void)
{
    extern int _edata, _estack, _sdata, __fini_array_end;

    g.bootloader_size = ((unsigned long)&__fini_array_end - FLASH_BASE)
        + ((unsigned long)&_edata - (unsigned long)&_sdata);
#if defined (GD32F350) || defined (GD32F330) || defined (STM32F10X_HD) || defined (GD32F130_150) || defined (STM32F10X_MD_VL)
#define REG_DENSITY     *(unsigned long*)0x1ffff7e0
    g.flash_size = REG_DENSITY & 0xffff;
#elif defined (STM32F303xC) || defined (STM32F072) || defined (STM32F030) || defined (STM32F042) || defined (STM32F051) || defined (STM32F070xB) || defined (STM32F030xC) || defined (STM32G030xx)
#define REG_DENSITY     *(unsigned short*)0x1ffff7cc
    g.flash_size = REG_DENSITY;
#elif defined (STM32F401xx) || defined (STM32F40_41xxx)
#define REG_DENSITY     *(unsigned short*)0x1fff7a22
    g.flash_size = REG_DENSITY;
#endif

    g.sram_size = ((unsigned long)&_estack - (unsigned long)&_sdata) / 1024;
#if _USE_EEPROM
    g.eeprom_base = FLASH_BASE + g.bootloader_size;
    while(g.eeprom_base % FLASH_PAGE_SIZE) {
        g.eeprom_base++;
    }
    g.app_base = g.eeprom_base + FLASH_PAGE_SIZE;
#else
    g.app_base = g.eeprom_base = FLASH_BASE + g.bootloader_size;
    while(g.app_base% FLASH_PAGE_SIZE) {
        g.app_base++;
    }
#endif
    g.max_app_size = FLASH_BASE + g.flash_size * 1024 - g.app_base;
}

IAP_Sysinfo_t* IAP_GetInfo(void)
{
    return &g;
}

static unsigned long GetAppSize(void)
{
    unsigned long pos;
    for(pos = g.app_base + g.max_app_size - 1; pos >= g.app_base; pos--) {
        if(*(unsigned char*)pos != 0xff)
            break;
    }
    return pos - g.app_base + 1;
}

void IAP_JumpToApp(void)
{
    __disable_irq();
    func = (func_app)(*(volatile unsigned long*)(g.app_base + 4));
    __set_MSP(*(volatile unsigned long*)(g.app_base));
    func();
}

int IAP_IsAppValid(void)
{
    if((*(unsigned long*)g.app_base & 0x2ffe0000) == 0x20000000)
        return 1;
    else
        return 0;
}

void IAP_EraseApp(void)
{
    unsigned long size = g.max_app_size;
    size = g.max_app_size;

    FLASH_Unlock();
#if defined (STM32F401xx) || defined (STM32F40_41xxx)
    FLASH_ClearFlag(FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
#else
    FLASH_ClearFlag(FLASH_FLAG_PGERR);
#endif
    for(unsigned long pos = g.app_base; pos < g.app_base + size; pos +=
            FLASH_PAGE_SIZE) {
        if(!IAP_CheckEmpty((unsigned long*)pos, FLASH_PAGE_SIZE))
            FLASH_ErasePage(pos);
    }
    FLASH_Lock();
}

void IAP_Parse(const unsigned char* msg, int msg_size)
{
    unsigned char checksum = 0;
    const unsigned char* content;
    unsigned long pos, size, crc;

    if(msg_size < 2)
        return;
    for(int i = 0; i < msg_size; i++) {
        checksum += msg[i];
    }
    if(checksum != 0)
        return;

    switch(msg[0]) {       // func code
        case IAP_GET_MAX_APPSIZE:       // 读取可用flash空间大小
            size = g.max_app_size;
            uwrite(&size, sizeof(size));
            break;
        case IAP_GET_APPSIZE:          // 取得APP区实际大小, 向上取整到2字节
            size = GetAppSize();
            uwrite(&size, sizeof(size));
            break;
        case IAP_READ_APP_CRC32:          // 计算APP区CRC32
            size = GetAppSize();
            crc = CRC32(0xffffffff, (unsigned char*)g.app_base, size);
            crc = ~crc;
            uwrite(&crc, sizeof(crc));
            break;
        case IAP_ERASE_APP:          // 擦除APP区全部内容
            IAP_EraseApp();
            uputc(0x00);
            break;
        case IAP_WRITE_FLASH:          // 写flash
            memcpy(&size, &msg[2], sizeof(size));
            size &= 0xffff;          // 这里size只有16位!
            memcpy(&pos, &msg[4], sizeof(pos));
            pos += g.app_base;
            if(pos < g.app_base || pos + size > g.app_base + g.max_app_size) {
                uputc(0xff);          // 超过可用范围
            }
            else if((pos & 0x3) || (size & 0x3)) {
                uputc(0xfd);     // pos和size都需要4字节对齐
            }
            else {
                content = &msg[8];
                FLASH_Unlock();
#if defined (STM32F401xx) || defined (STM32F40_41xxx)
                FLASH_ClearFlag(FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
#else
                FLASH_ClearFlag(FLASH_FLAG_PGERR);
#endif
                while(size) {
                    FLASH_ProgramWord(pos, *(unsigned long*)content);
                    size -= 4;
                    content += 4;
                    pos += 4;
                }
                FLASH_Lock();
                uputc(0x00);
            }
            break;
        case IAP_JUMP_TO_APP:      // 跳至app
            uputc(0x00);
            IAP_JumpToApp();
            break;
    }
}
