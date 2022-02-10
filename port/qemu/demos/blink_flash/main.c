#define USE_STDPERIPH_DRIVER
#include "stm32_p103.h"
#include "stm32_p103.h"

void busyLoop(uint32_t delay )
{
  while(delay) delay--;
}

int main(void)
{
    init_led();

    while(1) {
       GPIOC->BRR = 0x00001000;
       busyLoop(500000);
       GPIOC->BSRR = 0x00001000;
       busyLoop(500000);
    }
}
