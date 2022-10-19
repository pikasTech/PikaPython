#include "main.h"

uint32_t sdcard_init(void)
{
    PORT_Init(PORTB, PIN1, PORTB_PIN1_SD_CLK, 0);
    PORT_Init(PORTB, PIN2, PORTB_PIN2_SD_CMD, 1);
    PORT_Init(PORTB, PIN3, PORTB_PIN3_SD_D0,  1);
    PORT_Init(PORTB, PIN4, PORTB_PIN4_SD_D1,  1);
    PORT_Init(PORTB, PIN5, PORTB_PIN5_SD_D2,  1);
    PORT_Init(PORTB, PIN6, PORTB_PIN6_SD_D3,  1);
    return SDIO_Init(30000000);
}
