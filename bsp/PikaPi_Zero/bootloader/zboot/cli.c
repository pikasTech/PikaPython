#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "zboot_misc.h"
#include "xprintf.h"
#include "iap.h"
#include "flash_eeprom.h"

#define MSG_LEN 128

#if _USE_EEPROM
const char* str_help =
    " empty: empty check app area.\n\
 erase_all: erase app.\n\
 eeprom: eeprom [read|write|readall|eraseall] EEPROM ops.\n\
 reboot: reboot system.\n\
 read: read flash/sram/periph.\n\
 help: show help.\n\
 sysinfo: show system info.\n";

static void EEPROM_View(unsigned short addr, unsigned short total_size)
{
//    int count = 0, size;
    int size;
    unsigned char buf[16];

    addr &= ~1;
    while(total_size > 0) {
        size = 16;
        if(total_size < size)
            size = total_size;
        xprintf("0x%03lX  ", addr);
        FLASH_EEPROM_ReadData(addr, buf, size);
        for(int i = 0; i < size; i++) {
            xprintf("%02X ", buf[i]);
        }
        xputc('\n');
        addr += size;
        total_size -= size;
    }
}

#else
const char* str_help =
    " empty: empty check app area.\n\
 erase_all: erase app.\n\
 reboot: reboot system.\n\
 read: read flash/sram/periph.\n\
 help: show help.\n\
 sysinfo: show system info.\n";
#endif

static void Done(void)
{
    xprintf("Done.\n");
}

static void Parse(char* const tokens[], int count)
{
    IAP_Sysinfo_t* inf = IAP_GetInfo();

    if(strcasecmp(tokens[0], "HELP") == 0)
        xputs(str_help);
#if _USE_EEPROM
    else if(strcasecmp(tokens[0], "EEPROM") == 0) {
        if(strcasecmp(tokens[1], "GETSIZE") == 0) {
            xprintf("%u Bytes\n", FLASH_EEPROM_GetSize());
        }
        else if(strcasecmp(tokens[1], "READ") == 0) {
            unsigned short addr = strtoul(tokens[2], NULL, 16);
            unsigned short size = strtoul(tokens[3], NULL, 10);
            EEPROM_View(addr, size);
        }
        else if(strcasecmp(tokens[1], "READALL") == 0) {
            EEPROM_View(0, FLASH_EEPROM_GetSize());
        }
        else if(strcasecmp(tokens[1], "ERASEALL") == 0) {
            FLASH_EEPROM_EraseAll();
            Done();
        }
        else if(strcasecmp(tokens[1], "WRITE") == 0) {
            unsigned short addr = strtoul(tokens[2], NULL, 16);
            unsigned short data = strtoul(tokens[3], NULL, 16);
            if(addr % 2 != 0 || addr > FLASH_EEPROM_GetSize()) {
                xprintf("Invalid addr!\n");
                return;
            }
            FLASH_EEPROM_WriteWord(addr, data);
            Done();
        }
    }
#endif
    else if(strcasecmp(tokens[0], "READ") == 0) {
        unsigned long addr = strtoul(tokens[1], NULL, 16);
        int size = strtoul(tokens[2], NULL, 10);
        int count = 0;
        while(size > 0) {
            unsigned long data = *(unsigned long*)addr;
            for(int i = 0; i < (size < 4 ? size : 4); i++) {
                if(count % 16 == 0)
                    xprintf("0x%08lX  ", addr);
                xprintf("%02X ", (unsigned char)(data >> (i * 8)));
                count++;
                if(count % 16 == 0)
                    xputc('\n');
            }
            addr += 4;
            size -= 4;
        }
    }
    else if(strcasecmp(tokens[0], "SYSINFO") == 0) {

        xprintf("FLASH/SRAM/PAGE Sizes: %uKB/%uKB/%uKB\n", inf->flash_size,
            inf->sram_size, FLASH_PAGE_SIZE / 1024);
        xprintf("Bootloader/APP Sizes:  %uB(%uKB)/%uKB\n",
            inf->bootloader_size,
            (inf->bootloader_size - 1) / 1024 + 1, inf->max_app_size / 1024);
#if _USE_EEPROM
        xprintf("EEPROM/APP Base addr:  0x%08lX/0x%08lx\n", inf->eeprom_base, inf->app_base);
#else
        xprintf("APP Base addr:  0x%08lx\n", inf->app_base);
#endif
    }
    else if(strcasecmp(tokens[0], "EMPTY") == 0) {
        if(IAP_CheckEmpty((unsigned long*)inf->app_base, inf->max_app_size))
            xprintf("APP empty.\n");
        else
            xprintf("APP NOT empty.\n");
    }
    else if(strcasecmp(tokens[0], "ERASE_ALL") == 0) {
        IAP_EraseApp();
        Done();
    }
    else if(strcasecmp(tokens[0], "REBOOT") == 0) {
        Done();
        NVIC_SystemReset();
    }
    xputc('\n');
}

void CLI_Parse(const char* msg, int size)
{
    char *tokens[8], *token;
    char seps[] = "#? ,\r\n", string[MSG_LEN];
    int i, count = 0; //, len;

    if(size >= MSG_LEN) return;

    for(i = 0; i < size; i++)
        string[i] = msg[i];
    for(i = size; i < MSG_LEN; i++)
        string[i] = 0;

    for(i = 0; i < size; i++) {
        if(isalpha((int)string[i]))
            string[i] = toupper((int)string[i]);
    }
    token = strtok(string, seps);
    while(token != NULL) {
        tokens[count] = token;
        count++;
        token = strtok(NULL, seps); // Get next token:
    }
    if(count == 0)
        return;
    Parse(tokens, count);
}
