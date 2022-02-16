#include "pika_config.h"
#include "main.h"

/* support interrupt */

void __platform_disable_irq_handle(){
    __disable_irq();
}
void __platform_enable_irq_handle(){
    __enable_irq();
}


/* support printf */
void HARDWARE_PRINTF_Init(void) {
    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* Peripheral clock enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    /**USART1 GPIO Configuration
    PA9   ------> USART1_TX
    PA10   ------> USART1_RX
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(USART1_IRQn);

    USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART1, &USART_InitStruct);
    LL_USART_SetTXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
    LL_USART_SetRXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
    LL_USART_DisableFIFO(USART1);
    LL_USART_ConfigAsyncMode(USART1);
    LL_USART_Enable(USART1);

    /* Polling USART1 initialisation */
    while ((!(LL_USART_IsActiveFlag_TEACK(USART1))) ||
           (!(LL_USART_IsActiveFlag_REACK(USART1)))) {
    }

    /* open interrupt */
    LL_USART_EnableIT_RXNE(USART1);
    LL_USART_EnableIT_PE(USART1);
}
#pragma import(__use_no_semihosting)
void _sys_exit(int x) {
    x = x;
}

struct __FILE{
    int handle;
};

FILE __stdout;
int fputc(int ch, FILE* f) {
    LL_USART_TransmitData8(USART1, ch);
    while (LL_USART_IsActiveFlag_TC(USART1) != 1)
        ;
    return ch;
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


/* support pika Asm to flash */
uint32_t globalWriteAddress = 0;
uint32_t GetPage(uint32_t Addr) {
    return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
}

int32_t __eriseSelecttedFlash(uint32_t flashStart, uint32_t flashEnd) {
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

static uint8_t flash_writeBuff[8] = {0};
static uint8_t flash_offset = 0;
uint32_t flash_write_char(uint32_t bassAddr,
                          uint32_t flash_addr,
                          char ch_input) {
    if (flash_offset > 7) {
        flash_offset = 0;
        uint64_t writeData64 = 0;
        for (int i = 7; i >= 0; i--) {
            char ch = flash_writeBuff[i];
            writeData64 = writeData64 << 8;
            writeData64 += ch;
        }
        __platform_disable_irq_handle();
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                              bassAddr + flash_addr, writeData64) != HAL_OK) {
            while (1) {
                flash_offset = 0;
            }
        }
        __platform_disable_irq_handle();
        flash_addr = flash_addr + 8;
    }
    flash_writeBuff[flash_offset] = ch_input;
    flash_offset++;
    return flash_addr;
}

uint8_t __platform_Asm_is_to_flash(char* pyMultiLine) {
    if (strCountSign(pyMultiLine, '\n') > 10) {
        return 1;
    }
    return 0;
}

int32_t __saveStrToFlash(char* str,
                         uint32_t flashStart,
                         uint32_t flashEnd,
                         uint32_t* writeAddress_p) {
    uint32_t size = strGetSize(str);
    for (int i = 0; i < size; i++) {
        (*writeAddress_p) =
            flash_write_char(flashStart, (*writeAddress_p), str[i]);
    }
    return 0;
}

char* __platform_load_pikaAsm() {
    return (char*)FLASH_PIKA_ASM_START_ADDR;
}

int32_t __platform_save_pikaAsm(char* PikaAsm) {
    if (0 == globalWriteAddress) {
        __eriseSelecttedFlash(FLASH_PIKA_ASM_START_ADDR,
                              FLASH_PIKA_ASM_END_ADDR);
    }
    return __saveStrToFlash(PikaAsm, FLASH_PIKA_ASM_START_ADDR,
                            FLASH_PIKA_ASM_END_ADDR, &globalWriteAddress);
}

int32_t __platform_save_pikaAsm_EOF() {
    for (int i = 0; i < 16; i++) {
        globalWriteAddress = flash_write_char(FLASH_PIKA_ASM_START_ADDR,
                                              globalWriteAddress, '\0');
    }
    return 0;
}

/* support mem pool */
#if use_mem_pool
/* use mem pool */
Pool pikaPool;
void* __impl_pikaMalloc(size_t size) {
    void* mem = pool_malloc(&pikaPool, size);
    return mem;
}
void __impl_pikaFree(void* ptrm, size_t size) {
    pool_free(&pikaPool, ptrm, size);
}
#endif

/* support download python script by uart1 */
uint8_t pika_memory_lock = 0;
uint8_t __is_locked_pikaMemory(void){
    return pika_memory_lock;
}

CodeHeap codeHeap;
void STM32_Code_Init(void) {
    codeHeap.size = 0;
    codeHeap.ena = 0;
    codeHeap.auto_erase = 1;
    codeHeap.wait = 0;
}

void __erise_all(void){
    printf("[info]: erising flash... \r\n");
    /* init erise state */
    globalWriteAddress = 0;

    /* erise flash */
    __eriseSelecttedFlash(FLASH_PIKA_ASM_START_ADDR,
                              FLASH_PIKA_ASM_END_ADDR);
    printf("[ OK ]: erising flash ok! \r\n");
}

extern PikaObj* __pikaMain;
extern Arg* main_codeBuff;
uint8_t STM32_Code_reciveHandler(char* data, uint32_t rxSize) {
    char buff[RX_BUFF_LENGTH] = {0};
    if (0 == codeHeap.ena) {
        uint8_t is_launch_code_recive = 0;
        char* strLine = strGetLastLine(buff, data);
        /* auto erase mode, send python file directly */
        if (strIsStartWith(strLine, "import ")) {
            is_launch_code_recive = 1;
            codeHeap.auto_erase = 1;
            codeHeap.content = pikaMalloc(codeHeap.size + 1);
        }
        /* manual erase mode, send "import" to erase first,
           then send the python file.
        */
        if (strEqu(strLine, "import\r\n")){
            is_launch_code_recive = 1;
            codeHeap.auto_erase = 0;
            codeHeap.wait = 2;
            __disable_irq();
            obj_deinit(__pikaMain);
            codeHeap.content = malloc(FLASH_PIKA_ASM_END_ADDR - FLASH_PIKA_ASM_START_ADDR);
            if(codeHeap.content == NULL){
                printf("[error] no enough code buff, please reset the device.\r\n");
                while(1){
                }
            }
            __enable_irq();
            printf("[info] in download mode, send python please.\r\n");
        }
        /* launch the code reciver */
        if (is_launch_code_recive){
            codeHeap.reciveTime = uwTick;
            codeHeap.ena = 1;
            data = strLine;
        }
    }
    if (1 == codeHeap.ena) {
        if(!pika_memory_lock){
            pika_memory_lock = 1;
            #if use_mem_pool
            __platform_disable_irq_handle();
            pool_deinit(&pikaPool);
            __platform_enable_irq_handle();
            #endif
        }
        if(codeHeap.wait > 0){
            codeHeap.wait--;
        }
        codeHeap.reciveTime = uwTick;
        codeHeap.oldSize = codeHeap.size;
        codeHeap.size += rxSize;
        /* write to heap buff */
        if(codeHeap.auto_erase){
            codeHeap.content = realloc(codeHeap.content, codeHeap.size + 1);
        }
        memcpy(codeHeap.content + codeHeap.oldSize, data, rxSize);
        codeHeap.content[codeHeap.size] = 0;
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
    if (codeHeap.wait){
        /* still waiting */
        return;
    }
    if (uwTick - codeHeap.reciveTime < 200) {
        /* still reciving */
        return;
    }
    printf("==============[Programer]==============\r\n");
    
    /* transmite is finished */
    printf("[info]: recieved size: %d\r\n", codeHeap.size);
    /* write to flash from buff (heap.content) */
    __erise_all();
    printf("[info]: Writing flash... \r\n");
    /* init write state */
    __platform_memset(flash_writeBuff, 0, sizeof(flash_writeBuff));
    flash_offset = 0;
    /* write to flash */
    __saveStrToFlash(codeHeap.content, FLASH_PIKA_ASM_START_ADDR,
    FLASH_PIKA_ASM_END_ADDR, &globalWriteAddress);
    /* write EOF */
    __saveStrToFlash("\n\n", FLASH_PIKA_ASM_START_ADDR,
    FLASH_PIKA_ASM_END_ADDR, &globalWriteAddress);
    __platform_save_pikaAsm_EOF();
    HAL_FLASH_Lock();
    
    printf("[ OK ]: Write flash ok! \r\n");

    char* codeInFlash = (char*)FLASH_SCRIPT_START_ADDR;
    printf("\r\n");
    printf("----[code in flash]-----\r\n");
    for (int i = 0; i < strGetSize(codeInFlash); i++) {
        if ('\n' == codeInFlash[i]) {
            if(codeInFlash[i - 1] != '\r'){
                fputc('\r', (FILE*)!NULL);
            }
        }
        fputc(codeInFlash[i], (FILE*)!NULL);
    }
    printf("----[code in flash]-----\r\n");
    printf("\r\n");

    printf("[ OK ]: Programing ok! \r\n");
    printf("[info]: Restarting... \r\n");
    printf("==============[Programer]==============\r\n");

    printf("\r\n");
    HAL_NVIC_SystemReset();
}

void __platform_wait(void){
    while(1){
        pika_memory_lock ++;
    }
}
