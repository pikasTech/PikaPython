#ifndef _CH582_UTILS_H_
#define _CH582_UTILS_H_
#include "stdint.h"

uint8_t gpio_get_group(char *pin);
uint32_t gpio_get_pin(char *pin);
uint32_t gpio_get_mode(char *mode, char *pull);

#endif
