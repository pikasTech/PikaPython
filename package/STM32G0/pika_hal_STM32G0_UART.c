#include <stdint.h>
#include "BaseObj.h"
#include "STM32G0_UART.h"
#include "STM32G0_common.h"
#include "dataStrs.h"

char* usart1_rx_buff = NULL;
char* usart2_rx_buff = NULL;
uint8_t usart1_rx_offset = 0;
uint8_t usart2_rx_offset = 0;

static char* UART_get_rx_buff(USART_TypeDef* USARTx) {
    if (USARTx == USART1) {
        /* new buff if rx_buff is NULL */
        if (NULL == usart1_rx_buff) {
            usart1_rx_buff = pikaMalloc(RX_BUFF_LENGTH);
        }
        return usart1_rx_buff;
    }
    if (USARTx == USART2) {
        /* new buff if rx_buff is NULL */
        if (NULL == usart2_rx_buff) {
            usart2_rx_buff = pikaMalloc(RX_BUFF_LENGTH);
        }
        return usart2_rx_buff;
    }
    return NULL;
}

static uint8_t* UART_get_rx_offset(USART_TypeDef* USARTx) {
    if (USARTx == USART1) {
        return &usart1_rx_offset;
    }
    if (USARTx == USART2) {
        return &usart2_rx_offset;
    }
    return NULL;
}

static USART_TypeDef* UART_get_instance(uint8_t id) {
    if (1 == id) {
        return USART1;
    }
    if (2 == id) {
        return USART2;
    }
    return NULL;
}

static uint8_t UART_hardware_init(uint32_t baudRate, uint8_t id) {
    uint8_t errCode = 0;
    USART_TypeDef* USARTx = UART_get_instance(id);
    if (NULL == USARTx) {
        errCode = 5;
        goto exit;
    }
    if (USART1 == USARTx) {
        /* USART1 is inited by pika mcu sopport package in
         * stm32g030_pika_msp.c*/
        errCode = 0;
        goto exit;
    }
    /* init USART2 */
    if (USART2 == USARTx) {
        LL_USART_InitTypeDef USART_InitStruct = {0};
        LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
        /* Peripheral clock enable */
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
        /**USART2 GPIO Configuration
        PA2   ------> USART2_TX
        PA3   ------> USART2_RX
        */
        GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        /* USART2 interrupt Init */
        NVIC_SetPriority(USART2_IRQn, 0);
        NVIC_EnableIRQ(USART2_IRQn);
        USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
        USART_InitStruct.BaudRate = baudRate;
        USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
        USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
        USART_InitStruct.Parity = LL_USART_PARITY_NONE;
        USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
        USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
        USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
        LL_USART_Init(USART2, &USART_InitStruct);
        LL_USART_ConfigAsyncMode(USART2);
        LL_USART_Enable(USART2);
        while ((!(LL_USART_IsActiveFlag_TEACK(USART2))) ||
               (!(LL_USART_IsActiveFlag_REACK(USART2)))) {
        }
        /* open interrupt */
        LL_USART_EnableIT_RXNE(USART2);
        LL_USART_EnableIT_PE(USART2);
    }
exit:
    return errCode;
}

void __callback_UART_recive(USART_TypeDef* USARTx, char rx_char) {
    uint8_t* rx_offset = UART_get_rx_offset(USARTx);
    char* rx_buff = UART_get_rx_buff(USARTx);
    rx_buff[*rx_offset] = rx_char;
    /* avoid recive buff overflow */
    if (*rx_offset + 2 > RX_BUFF_LENGTH) {
        memmove(rx_buff, rx_buff + 1, RX_BUFF_LENGTH);
        return;
    }

    /* recive next char */
    (*rx_offset)++;
    rx_buff[*rx_offset] = 0;
}

/* Msp handle interrupt */
void __PIKA_USART1_IRQHandler(char rx_char) {
    __callback_UART_recive(USART1, rx_char);
}
void USART2_IRQHandler() {
    uint8_t rx_char = LL_USART_ReceiveData8(USART2);
    __callback_UART_recive(USART2, rx_char);
}

typedef struct platform_UART {
    USART_TypeDef* instence;
    int id;
} platform_UART;

int pika_hal_platform_UART_open(pika_dev* dev, char* name) {
    if (name[0] == 'U' && name[1] == 'A' && name[2] == 'R' && name[3] == 'T') {
        platform_UART* uart = pikaMalloc(sizeof(platform_UART));
        memset(uart, 0, sizeof(platform_UART));
        dev->platform_data = uart;
        int id = fast_atoi(name + 4);
        uart->instence = UART_get_instance(id);
        uart->id = id;
        if (NULL == uart->instence) {
            pika_platform_printf("Error: UART%d not found\r\n", id);
            return -1;
        }
        return 0;
    }
    return -1;
}

int pika_hal_platform_UART_close(pika_dev* dev) {
    platform_UART* uart = (platform_UART*)dev->platform_data;
    pikaFree(uart, sizeof(platform_UART));
    return 0;
}

int pika_hal_platform_UART_ioctl_config(pika_dev* dev,
                                        pika_hal_UART_config* cfg) {
    return 0;
}

int pika_hal_platform_UART_ioctl_enable(pika_dev* dev) {
    platform_UART* uart = (platform_UART*)dev->platform_data;
    pika_hal_UART_config* cfg = (pika_hal_UART_config*)dev->ioctl_config;
    int err = UART_hardware_init(cfg->baudrate, uart->id);
    if (0 != err) {
        pika_platform_printf("Error: UART%d config failed\r\n", uart->id);
        return -1;
    }
    return 0;
}

int pika_hal_platform_UART_ioctl_disable(pika_dev* dev) {
    return -1;
}

int pika_hal_platform_UART_read(pika_dev* dev, void* buf, size_t count) {
    platform_UART* uart = (platform_UART*)dev->platform_data;
    USART_TypeDef* USARTx = uart->instence;
    uint8_t* rx_offset = UART_get_rx_offset(USARTx);
    char* rx_buff = UART_get_rx_buff(USARTx);
    if (count >= *rx_offset) {
        /* not enough str */
        count = *rx_offset;
    }
    memcpy(buf, rx_buff, count);
    /* update rxBuff */
    memcpy(rx_buff, rx_buff + count, *rx_offset - count);
    *rx_offset -= count;
    rx_buff[*rx_offset] = 0;
    return count;
}

int pika_hal_platform_UART_write(pika_dev* dev, void* buf, size_t count) {
    platform_UART* uart = (platform_UART*)dev->platform_data;
    USART_TypeDef* USARTx = uart->instence;
    char* data = (char*)buf;
    for (int i = 0; i < count; i++) {
        LL_USART_TransmitData8(USARTx, data[i]);
        while (LL_USART_IsActiveFlag_TC(USARTx) != 1)
            ;
    }
    return count;
}

char pikaShell[RX_BUFF_LENGTH] = {0};
uint8_t pikaShellRxOk = 0;
