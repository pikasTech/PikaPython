#define USE_STDPERIPH_DRIVER
#include "stm32_p103.h"


volatile char in_char;
volatile char *out_str = "Got:_\n";
volatile int curr_char;

/* Handler predefined by STM32 library to handle USART2 interrupts. */
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        GPIOC->ODR = GPIOC->ODR ^ 0x00001000;
        if(curr_char == -1) {
            in_char = USART_ReceiveData(USART2);

            USART_ITConfig(USART2, USART_IT_TXE, ENABLE);

            curr_char = 1;
            USART_SendData(USART2, out_str[0]);
        } else {
            /* Discard byte if we are still processing the last one. */
            USART_ReceiveData(USART2);
        }
    }

    if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET) {
        GPIOC->ODR = GPIOC->ODR ^ 0x00001000;

        if(curr_char == -1) {
            USART_SendData(USART2, '!'); //This should never happen
        } else if(out_str[curr_char] == '_') {
            curr_char++;
            USART_SendData(USART2, in_char);
        } else if(out_str[curr_char] == '\0') {
            curr_char = -1;
            USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
        } else {
            USART_SendData(USART2, out_str[curr_char++]);
        }
    }
}




int main(void)
{
    init_led();

    init_button();

    init_rs232();
    enable_rs232_interrupts();
    curr_char = -1;
    enable_rs232();

    while(1);
}
