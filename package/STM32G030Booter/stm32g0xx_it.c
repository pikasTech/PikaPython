/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_it.h"
#include "main.h"
#include "stm32g030_pika_msp.h"
char UART1_RxBuff[RX_BUFF_LENGTH] = {0};
uint16_t UART1_RXBuff_offset = 0;
extern char Shell_Buff[RX_BUFF_LENGTH];
extern uint8_t Shell_Ready;

void NMI_Handler(void) {
    while (1) {
    }
}

void HardFault_Handler(void) {
    while (1) {
    }
}

void SVC_Handler(void) {}

void PendSV_Handler(void) {}

void SysTick_Handler(void) {
    HAL_IncTick();
    STM32_Code_flashHandler();
}

__attribute__((weak)) void __PIKA_USART1_IRQHandler(char rx_char) {}

void USART1_IRQHandler(void) {
    if (LL_USART_IsActiveFlag_RXNE(USART1)) {
        uint8_t rx_char = LL_USART_ReceiveData8(USART1);
        __PIKA_USART1_IRQHandler(rx_char);
        /* clear buff when overflow */
        if (UART1_RXBuff_offset >= RX_BUFF_LENGTH) {
            UART1_RXBuff_offset = 0;
            memset(UART1_RxBuff, 0, sizeof(UART1_RxBuff));
        }
        /* recive char */
        UART1_RxBuff[UART1_RXBuff_offset] = rx_char;
        UART1_RXBuff_offset++;
        if ('\n' == rx_char) {
            /* handle python script download */
            if (STM32_Code_reciveHandler(UART1_RxBuff, UART1_RXBuff_offset)) {
                goto line_exit;
            }
            /* handle python shell invoke */
            memcpy(Shell_Buff, UART1_RxBuff, sizeof(Shell_Buff));
            Shell_Ready = 1;
            goto line_exit;
        line_exit:
            UART1_RXBuff_offset = 0;
            memset(UART1_RxBuff, 0, sizeof(UART1_RxBuff));
            return;
        }
    }
}
