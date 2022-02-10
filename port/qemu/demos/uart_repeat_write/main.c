#define USE_STDPERIPH_DRIVER
#include "stm32_p103.h"


int main(void)
{
    init_led();

    init_button();

    init_rs232();
    enable_rs232_interrupts();
    enable_rs232();

    while(1) {
        send_byte('H');
        send_byte('e');
        send_byte('l');
        send_byte('l');
        send_byte('o');
        send_byte('\n');
    }
}
