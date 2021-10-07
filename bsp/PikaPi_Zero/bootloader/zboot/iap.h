#ifndef _IAP_H 
#define _IAP_H

typedef struct {
    unsigned short bootloader_size;
    unsigned short flash_size;
    unsigned short sram_size;
    unsigned long eeprom_base;
    unsigned long app_base;
    unsigned long max_app_size;
} IAP_Sysinfo_t;

void IAP_Config(void);
void IAP_Parse(const unsigned char* msg, int size);
int IAP_IsAppValid(void);
void IAP_JumpToApp(void);
void IAP_EraseApp(void);
IAP_Sysinfo_t* IAP_GetInfo(void);
int IAP_CheckEmpty(void* addr, int size);

#endif
