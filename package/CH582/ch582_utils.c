#include "CH58x_common.h"
#include "ch582_utils.h"
#include "string.h"
#include "stdint.h"

uint8_t gpio_get_group(char *pin)
{
    return strncmp(pin, "PA", 2) == 0 ? 0 : 1;
}

uint32_t gpio_get_pin(char *pin)
{
    uint32_t num = 0;
    if (strlen(pin) == 3)
    {
        num = pin[2] - 48;
    }
    else if (strlen(pin) > 3 && strlen(pin) < 5)
    {
        num = 10 * (pin[2] - 48) + (pin[3] - 48);
    }
    else
    {
        return 0;
    }
    return (uint32_t)1ull << num;
}

uint32_t gpio_get_mode(char *mode, char *pull)
{
    if (strcmp(mode, "out") == 0)
    {
        return GPIO_ModeOut_PP_5mA;
    }
    else if (strcmp(mode, "in") == 0)
    {
        if (strcmp(pull, "up") == 0)
        {
            return GPIO_ModeIN_PU;
        }
        else if (strcmp(pull, "down") == 0)
        {
            return GPIO_ModeIN_PD;
        }
        else
        {
            return GPIO_ModeIN_Floating;
        }
    }
    else
    {
        return GPIO_ModeIN_Floating;
    }
}
