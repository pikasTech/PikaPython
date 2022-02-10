#define USE_STDPERIPH_DRIVER
#include "stm32_p103.h"
#include "stm32_p103.h"

int main(void)
{
    int last_button_state, new_button_state;

    init_led();
    init_button();

    /* Infinite loop - when the button changes state, toggle the LED.
     */
    last_button_state = GPIOA->IDR & 0x00000001;
    while(1) {
        new_button_state = GPIOA->IDR & 0x00000001;
        if(new_button_state ^ last_button_state) {
            if(new_button_state) {
                GPIOC->ODR = GPIOC->ODR ^ 0x00001000;
            }
        }
        last_button_state = new_button_state;
    }
}
