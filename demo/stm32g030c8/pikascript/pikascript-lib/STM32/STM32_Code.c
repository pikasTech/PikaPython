#include <stdint.h>
#include "BaseObj.h"
#include "STM32_common.h"
#include "dataStrs.h"

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
            rxSize = strGetSize(data);
        }
    }
    if (1 == codeHeap.ena) {
        codeHeap.reciveTime = uwTick;
        codeHeap.oldContent = codeHeap.content;
        codeHeap.oldSize = codeHeap.size;
        codeHeap.size += rxSize;
        codeHeap.content = pikaMalloc(codeHeap.size + 1);
        memcpy(codeHeap.content, codeHeap.oldContent, codeHeap.oldSize);
        memcpy(codeHeap.content + codeHeap.oldSize, data, rxSize);
        pikaFree(codeHeap.oldContent, codeHeap.oldSize + 1);
        codeHeap.content[codeHeap.size] = 0;
        /* reciving code */
        return 1;
    }
    /* not work */
    return 0;
}

void STM32_Code_flashHandler() {
    if (codeHeap.ena) {
        /* transmite is finished */
        if (uwTick - codeHeap.reciveTime > 200) {
            uint32_t FirstPage = 0, NbOfPages = 0;
            uint32_t PageError = 0;
            __IO uint32_t data32 = 0, MemoryProgramStatus = 0;
            uint64_t writeData64 = 0;
            static FLASH_EraseInitTypeDef EraseInitStruct = {0};

            printf("==============[Programer]==============\r\n");
            printf("[info]: Recived byte: %d\r\n", codeHeap.size);
            printf("[info]: Programing... \r\n");
            HAL_FLASH_Unlock();
            /* Get the 1st page to erase */
            FirstPage = GetPage(FLASH_USER_START_ADDR);

            /* Get the number of pages to erase from 1st page */
            NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;

            /* Fill EraseInit structure*/
            EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
            EraseInitStruct.Page = FirstPage;
            EraseInitStruct.NbPages = NbOfPages;
            printf("    [info]: Erasing flash... \r\n");

            if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
                printf("    [error]: Erase faild! \r\n");
                while (1) {
                }
            }
            printf("    [ OK ]: Erase flash ok! \r\n");

            printf("    [info]: Writing flash... \r\n");
            uint32_t baseAddress = FLASH_USER_START_ADDR;
            uint32_t writeAddress = 0;
            while (writeAddress < codeHeap.size) {
                for (int i = 7; i >= 0; i--) {
                    writeData64 = writeData64 << 8;
                    writeData64 += codeHeap.content[writeAddress + i];
                }
                if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                                      baseAddress + writeAddress,
                                      writeData64) == HAL_OK) {
                    writeAddress = writeAddress + 8;
                } else {
                    printf("    [error]: Write flash faild. \r\n");
                    while (1) {
                    }
                }
            }
            HAL_FLASH_Lock();
            printf("    [ OK ]: Write flash ok! \r\n");

            baseAddress = FLASH_USER_START_ADDR;
            MemoryProgramStatus = 0x0;

            printf("    [info]: Checking flash... \r\n");
            char* codeInFlash = (char*)baseAddress;
            printf("---------[code in flash]----------\r\n");
            printf("\r\n");
            printf("%s", codeInFlash);
            printf("\r\n\r\n");
            printf("---------[code in flash]----------\r\n");
            
            if (!strEqu(codeInFlash, codeHeap.content)) {
                printf("    [error]: Check flash faild.\r\n");
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
            printf("    [ OK ]: Checking flash ok! \r\n");
            printf("[ OK ]: Programing ok! \r\n");
            printf("==============[Programer]==============\r\n");

            printf("[info]: Restarting... \r\n");
            printf("\r\n");
            HAL_NVIC_SystemReset();
        }
    }
}