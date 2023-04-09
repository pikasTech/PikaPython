#include "pika_bsp.h"
#include "pika_config.h"
#include "main.h"
#include <stdio.h>
/* support interrupt */

void __platform_disable_irq_handle(){
    __disable_irq();
}
void __platform_enable_irq_handle(){
    __enable_irq();
}



/* support delay_us */

void delay_us(uint32_t udelay) {
    uint32_t startval, tickn, delays, wait;

    startval = SysTick->VAL;
    tickn = HAL_GetTick();
    delays = udelay * 64;  // delay 1us when delays = 64
    if (delays > startval) {
        while (HAL_GetTick() == tickn) {
        }
        wait = 64000 + startval - delays;
        while (wait < SysTick->VAL) {
        }
    } else {
        wait = startval - delays;
        while (wait < SysTick->VAL && HAL_GetTick() == tickn) {
        }
    }
}


uint32_t globalWriteAddress = 0;
uint32_t GetPage(uint32_t Addr) {
    return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
}

/* support download python script by uart1 */
uint8_t pika_memory_lock = 0;
uint8_t __is_locked_pikaMemory(void){
    return pika_memory_lock;
}

CodeHeap codeHeap;
void STM32_Code_Init(void) {
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
            codeHeap.content = malloc(FLASH_SCRIPT_END_ADDR - FLASH_SCRIPT_START_ADDR);
        }
    }
    if (1 == codeHeap.ena) {
        if(!pika_memory_lock){
            pika_memory_lock = 1;
        }
        codeHeap.reciveTime = uwTick;
        codeHeap.oldSize = codeHeap.size;
        codeHeap.size += rxSize;
//        codeHeap.content = realloc(codeHeap.content, codeHeap.size + 1);
        memcpy(codeHeap.content + codeHeap.oldSize, data, rxSize);
        codeHeap.content[codeHeap.size] = 0;
        /* reciving code */
        return 1;
    }
    /* not work */
    return 0;
}

void STM32_Code_flashHandler(void) {
    if (!codeHeap.ena) {
        /* recive not activate */
        return;
    }
    if (uwTick - codeHeap.reciveTime < 200) {
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
    for (int i = 0; i < strGetSize(codeHeap.content); i++) {
        if ('\n' == codeHeap.content[i] && '\r' != codeHeap.content[i -1]) {
            fputc('\r', (FILE*)!NULL);
        }
        fputc(codeHeap.content[i], (FILE*)!NULL);
    }
    printf("----[code in flash]-----\r\n");
    printf("\r\n");

    if (!strEqu(codeInFlash, codeHeap.content)) {
        printf("[error]: Check flash faild.\r\n");
        printf("\r\n");

        printf("\r\n\r\n");
        printf("---------[code in heap]----------\r\n");
        printf("\r\n");
        for (int i = 0; i < strGetSize(codeHeap.content); i++) {
            if ('\n' == codeHeap.content[i]) {
                fputc('\r', (FILE*)!NULL);
            }
            fputc(codeHeap.content[i], (FILE*)!NULL);
        }
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
