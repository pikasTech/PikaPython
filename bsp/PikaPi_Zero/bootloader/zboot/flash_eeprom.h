#ifndef _FLASH_EEPROM_H
#define _FLASH_EEPROM_H

void FLASH_EEPROM_Config(unsigned long base_addr, unsigned short page_size);
void FLASH_EEPROM_WriteWord(unsigned short addr, unsigned short data);
unsigned short FLASH_EEPROM_ReadWord(unsigned short addr);

void FLASH_EEPROM_WriteData(unsigned short addr, void* data, int num);
void FLASH_EEPROM_ReadData(unsigned short addr, void* data, int num);

unsigned short FLASH_EEPROM_GetSize(void);
void FLASH_EEPROM_EraseAll(void);

#endif
