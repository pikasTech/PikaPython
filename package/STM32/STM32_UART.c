#include "STM32_UART.h"
#include <stdint.h>
#include "BaseObj.h"
#include "STM32_common.h"
#include "dataStrs.h"

extern PikaObj* pikaMain;

static pika_uart_t* getPikaUart(uint8_t id) {
    return NULL;
}

static void setUartObj(uint8_t id, PikaObj* obj) {
    pika_uart_t* pika_uart = getPikaUart(id);
    pika_uart->obj = obj;
}

static PikaObj* getUartObj(uint8_t id) {
    pika_uart_t* pika_uart = getPikaUart(id);
    if (NULL == pika_uart) {
        return NULL;
    }
    return pika_uart->obj;
}

static USART_TypeDef* getUartInstance(uint8_t id) {
    if (1 == id) {
        return USART1;
    }
    if (2 == id) {
        return USART2;
    }
    return NULL;
}

static uint8_t getUartId(UART_HandleTypeDef* huart) {
    return 0;
}

static UART_HandleTypeDef* getUartHandle(uint8_t id) {
    pika_uart_t* pika_uart = getPikaUart(id);
    if (NULL == pika_uart) {
        return NULL;
    }
    return &(pika_uart->huart);
}

static char* getUartRxBuff(uint8_t id) {
    pika_uart_t* pika_uart = getPikaUart(id);
    if (NULL == pika_uart) {
        return NULL;
    }
    return pika_uart->rxBuff;
}

static uint8_t USART_UART_Init(uint32_t baudRate, uint8_t id) {
    uint8_t errCode = 0;
    USART_TypeDef* USARTx = getUartInstance(id);
    if (NULL == USARTx) {
        errCode = 5;
        goto exit;
    }

exit:
    return errCode;
}

static void UART_GPIO_init(USART_TypeDef* UARTx) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (UARTx == USART1) {
        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
    if (UARTx == USART2) {
        /* USART2 clock enable */
        __HAL_RCC_USART2_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART2 GPIO Configuration
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/* Msp handle interrupt */
#ifdef UART1_EXIST
void __PIKA_USART1_IRQHandler(void) {}
#endif
#ifdef UART2_EXIST
void USART2_IRQHandler(void) {
//    HAL_UART_IRQHandler(&pika_uart2.huart);
}
#endif

void STM32_UART_platformEnable(PikaObj* self) {
    int id = obj_getInt(self, "id");
    int baudRate = obj_getInt(self, "baudRate");
    setUartObj(id, self);
    /* uart 1 is inited by hardward */
    if (1 == id) {
        return;
    }
    USART_TypeDef* UARTx = getUartInstance(id);
    UART_GPIO_init(UARTx);
    int errCode = USART_UART_Init(baudRate, id);
    if (0 != errCode) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] uart init faild.");
        return;
    }
    HAL_UART_Receive_IT(getUartHandle(id), (uint8_t*)getUartRxBuff(id), 1);
}

void STM32_UART_platformRead(PikaObj* self) {
    int id = obj_getInt(self, "id");
    int length = obj_getInt(self, "length");
    Args* buffs = New_strBuff();
    char* readBuff = NULL;
    pika_uart_t* pika_uart = getPikaUart(id);
    if (length >= pika_uart->rxBuffOffset) {
        /* not enough str */
        length = pika_uart->rxBuffOffset;
    }
    readBuff = args_getBuff(buffs, length);
    memcpy(readBuff, pika_uart->rxBuff, length);
    obj_setStr(self, "readBuff", readBuff);
    readBuff = obj_getStr(self, "readBuff");

    /* update rxBuff */
    memcpy(pika_uart->rxBuff, pika_uart->rxBuff + length,
           pika_uart->rxBuffOffset - length);
    pika_uart->rxBuffOffset -= length;
    pika_uart->rxBuff[pika_uart->rxBuffOffset] = 0;

    UART_Start_Receive_IT(
        &pika_uart->huart,
        (uint8_t*)(pika_uart->rxBuff + pika_uart->rxBuffOffset), 1);
exit:
    args_deinit(buffs);
    obj_setStr(self, "readData", readBuff);
}

void STM32_UART_platformWrite(PikaObj* self) {
    char* data = obj_getStr(self, "data");
    int id = obj_getInt(self, "id");
    HAL_UART_Transmit(getUartHandle(id), (uint8_t*)data, strGetSize(data), 100);
}

void STM32_UART_clearRxBuff(pika_uart_t* pika_uart) {
    pika_uart->rxBuffOffset = 0;
    pika_uart->rxBuff[pika_uart->rxBuffOffset] = 0;
    UART_Start_Receive_IT(
        &pika_uart->huart,
        (uint8_t*)(pika_uart->rxBuff + pika_uart->rxBuffOffset), 1);
}

char pikaShell[RX_BUFF_LENGTH] = {0};
uint8_t pikaShellRxOk = 0;

/* Recive Interrupt Handler */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
    uint8_t id = getUartId(huart);
    pika_uart_t* pika_uart = getPikaUart(id);
    char inputChar = pika_uart->rxBuff[pika_uart->rxBuffOffset];

    if ((id == 1) && ('\n' == inputChar)) {
#ifdef Code_ENABLE
        uint8_t res = STM32_Code_reciveHandler(pika_uart->rxBuff,
                                               pika_uart->rxBuffOffset + 1);
        /* handler is working */
        if (0 != res) {
            STM32_UART_clearRxBuff(pika_uart);
            return;
        }
        /* run as shell */
        memset(pikaShell, 0, RX_BUFF_LENGTH);
        strGetLastLine(pikaShell, pika_uart->rxBuff);
        pikaShellRxOk = 1;
#endif
    }
    /* avoid recive buff overflow */
    if (pika_uart->rxBuffOffset + 2 > RX_BUFF_LENGTH) {
        memmove(pika_uart->rxBuff, pika_uart->rxBuff + 1, RX_BUFF_LENGTH);
        UART_Start_Receive_IT(
            huart, (uint8_t*)(pika_uart->rxBuff + pika_uart->rxBuffOffset), 1);
        return;
    }

    /* recive next char */
    pika_uart->rxBuffOffset++;
    pika_uart->rxBuff[pika_uart->rxBuffOffset] = 0;
    UART_Start_Receive_IT(
        huart, (uint8_t*)(pika_uart->rxBuff + pika_uart->rxBuffOffset), 1);
}
