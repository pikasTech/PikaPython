#define USE_STDPERIPH_DRIVER
#include "stm32_p103.h"

void USART2_IRQHandler(void)
{
    /* Make sure the interrupt was triggered by a transmit.  This should
     * always be true.
     */
    if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET) {
        /* Toggle the LED just to show that progress is being made. */
        GPIOC->ODR = GPIOC->ODR ^ 0x00001000;

        /* Send the next byte */
        USART_SendData(USART2, 'X');
    }
}

int main(void)
{
    init_led();

    init_button();

    init_rs232();
    enable_rs232_interrupts();
    enable_rs232();

    /* Send the first byte and enable the interrupt. */
    USART_SendData(USART2, 'x');
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);

    /* Infinite loop - when the previous finishes, the interrupt will trigger.
     * The interrupt will send the next byte.
     */
    while(1);
}
