#include "RGB_ASM.h"
#include "STM32_common.h"

static void RGB_setUp() {
    GPIOB->BSRR = GPIO_PIN_12;  // set
    int i;
    i++;
    i++;
    i++;
    i++;
    i++;
    i++;
    i++;
    i++;
    GPIOB->BRR = GPIO_PIN_12;  // reset
}

static void RGB_setDown() {
    GPIOB->BSRR = GPIO_PIN_12;  // set
    int i;
    i++;
    i++;
    i++;
    GPIOB->BRR = GPIO_PIN_12;
}

void RGB_set(uint32_t G8R8B8) {
    int i;
    uint8_t byte = 0;
    for (i = 23; i >= 0; i--) {
        byte = ((G8R8B8 >> i) & 0x01);
        if (byte) {
            RGB_setUp();
        } else {
            RGB_setDown();
        }
    }
}