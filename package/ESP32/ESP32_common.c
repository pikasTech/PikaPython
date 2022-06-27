#include "ESP32_common.h"
#include "dataStrs.h"

int getGpioPin(char *pin)
{
    int num = -1;
    sscanf(pin, "P%d\n", &num);
    return num;
}

uint32_t getPinMode(char *mode)
{
    if (strEqu(mode, "out"))
    {
        return GPIO_MODE_OUTPUT;
    }
    else if (strEqu(mode, "in"))
    {
        return GPIO_MODE_INPUT;
    }
    else if (strEqu(mode, "out_od"))
    {
        return GPIO_MODE_OUTPUT_OD;
    }

    return GPIO_MODE_DISABLE;
}