#include <stdint.h>
#include "BaseObj.h"
#include "STM32_common.h"
#include "dataStrs.h"
#include <stdlib.h>
CodeHeap codeHeap;

void STM32_Code_Init() {
    codeHeap.size = 0;
    codeHeap.content = pikaMalloc(codeHeap.size + 1);
    codeHeap.ena = 0;
}

uint8_t STM32_Code_reciveHandler(char* data, uint32_t rxSize) {
    char buff[RX_BUFF_LENGTH] = {0};    
    if (0 == codeHeap.ena) {
        char* strLine = strGetLastLine(buff, data);
        if (strIsStartWith(strLine, "import ")) {
            codeHeap.reciveTime = uwTick;
            codeHeap.ena = 1;
            data = strLine;
            rxSize = strGetSize(strLine);
        }
    }
    if (1 == codeHeap.ena) {
        codeHeap.reciveTime = uwTick;
        codeHeap.oldSize = codeHeap.size;
        codeHeap.size += rxSize;
        codeHeap.content = realloc(codeHeap.content , codeHeap.size + 1);
        memcpy(codeHeap.content + codeHeap.oldSize, data, rxSize);
        codeHeap.content[codeHeap.size] = 0;
        /* reciving code */
        return 1;
    }
    /* not work */
    return 0;
}

uint32_t GetPage(uint32_t Addr) {
    return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
}

#if (defined Code_ENABLE) 
void STM32_Code_flashHandler() {
    if (!codeHeap.ena){
        /* recive not activate */
        return;
    }
    if ( uwTick - codeHeap.reciveTime < 200 ){
        /* still reciving */
        return;
    }
    
    /* transmite is finished */
    STM32_Code_reciveHandler("\n\n", 2);

    uint32_t FirstPage = 0, NbOfPages = 0;
    uint32_t PageError = 0;
    __IO uint32_t data32 = 0, MemoryProgramStatus = 0;
    static FLASH_EraseInitTypeDef EraseInitStruct = {0};

    printf("==============[Programer]==============\r\n");
    printf("[info]: Recived byte: %d\r\n", codeHeap.size);
    printf("[info]: Programing... \r\n");
    HAL_FLASH_Unlock();
    /* Get the 1st page to erase */
    FirstPage = GetPage(FLASH_SCRIPT_START_ADDR);

    /* Get the number of pages to erase from 1st page */
    NbOfPages = GetPage(FLASH_SCRIPT_END_ADDR) - FirstPage + 1;

    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page = FirstPage;
    EraseInitStruct.NbPages = NbOfPages;
    printf("[info]: Erasing flash... \r\n");

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
        printf("[error]: Erase faild! \r\n");
        while (1) {
        }
    }
    printf("[ OK ]: Erase flash ok! \r\n");

    printf("[info]: Writing flash... \r\n");
    uint32_t baseAddress = FLASH_SCRIPT_START_ADDR;
    uint32_t writeAddress = 0;
    uint64_t writeData64 = 0;
    while (writeAddress < codeHeap.size + 1) {
        writeData64 = 0;
        for (int i = 7; i >= 0; i--) {
            char ch = codeHeap.content[writeAddress + i];
            writeData64 = writeData64 << 8;
            writeData64 += ch;
        }
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                              baseAddress + writeAddress,
                              writeData64) == HAL_OK) {
            writeAddress = writeAddress + 8;
        } else {
            printf("[error]: Write flash faild. \r\n");
            while (1) {
            }
        }
    }
    HAL_FLASH_Lock();
    printf("[ OK ]: Write flash ok! \r\n");

    baseAddress = FLASH_SCRIPT_START_ADDR;
    MemoryProgramStatus = 0x0;

    printf("[info]: Checking flash... \r\n");
    char* codeInFlash = (char*)baseAddress;
    printf("\r\n");
    printf("----[code in flash]-----\r\n");
    printf("%s", codeInFlash);
    printf("----[code in flash]-----\r\n");
    printf("\r\n");
    
    if (!strEqu(codeInFlash, codeHeap.content)) {
        printf("[error]: Check flash faild.\r\n");
        printf("\r\n");

        printf("\r\n\r\n");
        printf("---------[code in heap]----------\r\n");
        printf("\r\n");
        printf("%s", codeHeap.content);
        printf("\r\n\r\n");
        printf("---------[code in heap]----------\r\n");
        while (1) {
        }
    }
    printf("[ OK ]: Checking flash ok! \r\n");
    printf("[ OK ]: Programing ok! \r\n");
    printf("[info]: Restarting... \r\n");
    printf("==============[Programer]==============\r\n");

    printf("\r\n");
    HAL_NVIC_SystemReset();
}

int32_t __eriseSelecttedFlash(uint32_t flashStart, uint32_t flashEnd){
    uint32_t FirstPage = 0, NbOfPages = 0;
    uint32_t PageError = 0;
    __IO uint32_t data32 = 0, MemoryProgramStatus = 0;
    static FLASH_EraseInitTypeDef EraseInitStruct = {0};

    HAL_FLASH_Unlock();
    /* Get the 1st page to erase */
    FirstPage = GetPage(flashStart);

    /* Get the number of pages to erase from 1st page */
    NbOfPages = GetPage(flashEnd) - FirstPage + 1;

    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page = FirstPage;
    EraseInitStruct.NbPages = NbOfPages;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
        return 1;
    }
    return 0;
}

uint32_t globalWriteAddress = 0;

uint32_t flash_write_char(uint32_t bassAddr, uint32_t flash_addr, char ch_input){
    static uint8_t writeBuff[8] = {0};
    static uint8_t offset = 0; 
    if(offset > 7){
        offset = 0;
        uint64_t writeData64 = 0;
        for (int i = 7; i >= 0; i--) {
            char ch = writeBuff[i];
            writeData64 = writeData64 << 8;
            writeData64 += ch;
        }
        __platformDisableIrqHandle();
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                              bassAddr + flash_addr,
                              writeData64) != HAL_OK) {
            while(1){
                offset = 0;
            }
        }
        __platformDisableIrqHandle();
        flash_addr = flash_addr + 8;
    }
    writeBuff[offset] = ch_input;
    offset ++;
    return flash_addr;
}

uint8_t __platformAsmIsToFlash(char *pyMultiLine){
    if (strCountSign(pyMultiLine, '\n') > 1){
        return 1;
    }
    return 0;
}

int32_t __saveStrToFlash(char *str, uint32_t flashStart, uint32_t flashEnd, uint32_t *writeAddress_p){
    uint32_t size = strGetSize(str);
    for(int i = 0; i < size; i++) {
        (*writeAddress_p) = flash_write_char(flashStart, (*writeAddress_p), str[i]);
    }
    return 0;
}

char* __platformLoadPikaAsm(){
    return (char *)FLASH_PIKA_ASM_START_ADDR;
}

int32_t __platformSavePikaAsm(char *PikaAsm){
    if (0 == globalWriteAddress){
        __eriseSelecttedFlash(FLASH_PIKA_ASM_START_ADDR, FLASH_PIKA_ASM_END_ADDR);
    }
    return __saveStrToFlash(PikaAsm, FLASH_PIKA_ASM_START_ADDR, FLASH_PIKA_ASM_END_ADDR, &globalWriteAddress);
}

int32_t __platformSavePikaAsmEOF(){
    for(int i = 0; i < 16; i++) {
        globalWriteAddress = flash_write_char(FLASH_PIKA_ASM_START_ADDR, globalWriteAddress, '\0');
    }
    return 0;
}

#endif