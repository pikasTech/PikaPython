#include <stdint.h>
#include "BaseObj.h"
#include "STM32F4_common.h"
#include "dataStrs.h"

extern PikaObj* pikaMain;

#ifdef UART1_EXIST
pika_uart_t pika_uart1;
#endif
#ifdef UART2_EXIST
pika_uart_t pika_uart2;
#endif
#ifdef UART3_EXIST
pika_uart_t pika_uart3;
#endif
#ifdef UART4_EXIST
pika_uart_t pika_uart4;
#endif

/* support by booter */
extern UART_HandleTypeDef huart1;

static pika_uart_t* getPikaUart(uint8_t id) {
#ifdef UART1_EXIST
    if (1 == id) {
        return &pika_uart1;
    }
#endif
#ifdef UART2_EXIST
    if (2 == id) {
        return &pika_uart2;
    }
#endif
#ifdef UART3_EXIST
    if (3 == id) {
        return &pika_uart3;
    }
#endif
#ifdef UART4_EXIST
    if (4 == id) {
        return &pika_uart4;
    }
#endif
    return NULL;
}

static void setUartObj(uint8_t id, PikaObj* obj) {
    pika_uart_t* pika_uart = getPikaUart(id);
    pika_uart->obj = obj;
}

static USART_TypeDef* getUartInstance(uint8_t id) {
#ifdef UART1_EXIST
    if (1 == id) {
        return USART1;
    }
#endif
#ifdef UART2_EXIST
    if (2 == id) {
        return USART2;
    }
#endif
#ifdef UART3_EXIST
    if (3 == id) {
        return USART3;
    }
#endif
#ifdef UART4_EXIST
    if (4 == id) {
        return USART4;
    }
#endif
    return NULL;
}

static uint8_t getUartId(UART_HandleTypeDef* huart) {
#ifdef UART1_EXIST
    if (huart == &pika_uart1.huart) {
        return 1;
    }
#endif
#ifdef UART2_EXIST
    if (huart == &pika_uart2.huart) {
        return 2;
    }
#endif
#ifdef UART3_EXIST
    if (huart == &pika_uart3.huart) {
        return 3;
    }
#endif
#ifdef UART4_EXIST
    if (huart == &pika_uart4.huart) {
        return 4;
    }
#endif
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
    UART_HandleTypeDef* huart = getUartHandle(id);
    huart->Instance = getUartInstance(id);
    if (NULL == huart->Instance) {
        errCode = 5;
        goto exit;
    }
    huart->Init.BaudRate = baudRate;
    huart->Init.WordLength = UART_WORDLENGTH_8B;
    huart->Init.StopBits = UART_STOPBITS_1;
    huart->Init.Parity = UART_PARITY_NONE;
    huart->Init.Mode = UART_MODE_TX_RX;
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(huart) != HAL_OK) {
        errCode = 1;
        goto exit;
    }
exit:
    return errCode;
}

static void UART_MspInit(UART_HandleTypeDef* uartHandle) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
#ifdef UART1_EXIST
    if (uartHandle->Instance == USART1) {
        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
/**USART1 GPIO Configuration
PA9     ------> USART1_TX
PA10     ------> USART1_RX
*/
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
#endif
#ifdef UART2_EXIST
    if (uartHandle->Instance == USART2) {
        /* USART2 clock enable */
        __HAL_RCC_USART2_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
/**USART2 GPIO Configuration
PA2     ------> USART2_TX
PA3     ------> USART2_RX
*/

        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        /* USART2 interrupt Init */
        HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }
#endif
#ifdef UART3_EXIST
    if (uartHandle->Instance == USART3) {
        /* USART3 clock enable */
        __HAL_RCC_USART3_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
/**USART3 GPIO Configuration
PA5     ------> USART3_TX
PB0     ------> USART3_RX
*/

        /**USART3 GPIO Configuration
            PB10     ------> USART3_TX
            PB11     ------> USART3_RX
            */
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USART3 interrupt Init */
        HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART3_IRQn);
    }
#endif

}

/* Msp handle interrupt */
#ifdef UART1_EXIST
#if 0
void USART1_IRQHandler(void) {
    HAL_UART_IRQHandler(&pika_uart1.huart);
}
#endif
#endif
#ifdef UART2_EXIST
void USART2_IRQHandler(void) {
    HAL_UART_IRQHandler(&pika_uart2.huart);
}
#endif
#ifdef UART3_EXIST
void USART3_IRQHandler(void) {
    HAL_UART_IRQHandler(&pika_uart3.huart);
}
#endif

#if (defined UART3_EXIST) && (defined UART4_EXIST)
#if defined STM32G070xx
void USART3_4_IRQHandler(void) {
    HAL_UART_IRQHandler(&pika_uart3.huart);
    HAL_UART_IRQHandler(&pika_uart4.huart);
}
#endif
#endif

void STM32F4_UART_platformEnable(PikaObj* self) {
    int id = obj_getInt(self, "id");
    int baudRate = obj_getInt(self, "baudRate");
    setUartObj(id, self);
    /* uart 1 is inited by hardward */
    if (1 == id) {
        return;
    }
    UART_HandleTypeDef* huart = getUartHandle(id);
    huart->Instance = getUartInstance(id);
    UART_MspInit(huart);
    int errCode = USART_UART_Init(baudRate, id);
    if (0 != errCode) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] uart init faild.");
        return;
    }
    HAL_UART_Receive_IT(getUartHandle(id), (uint8_t*)getUartRxBuff(id), 1);
}

void STM32F4_UART_platformRead(PikaObj* self) {
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
    args_deinit(buffs);
    obj_setStr(self,"readData", readBuff);
}

void STM32F4_UART_platformWrite(PikaObj* self) {
    char *data = obj_getStr(self, "writeData");
    int id = obj_getInt(self, "id");
    HAL_UART_Transmit(getUartHandle(id), (uint8_t*)data, strGetSize(data), 100);
}

void STM32F4_UART_clearRxBuff(pika_uart_t* pika_uart) {
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

