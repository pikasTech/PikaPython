#include "CH58x_common.h"
#include "CH582_GPIO.h"
#include "ch582_utils.h"


void CH582_GPIO_platformDisable(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");

    if (gpio_get_group(pin) == 0)
    {
        GPIOA_ModeCfg(gpio_get_pin(pin), GPIO_ModeIN_PU);
    }
    else
    {
        GPIOB_ModeCfg(gpio_get_pin(pin), GPIO_ModeIN_PU);
    }
}

void CH582_GPIO_platformEnable(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");
    char *mode = obj_getStr(self, "mode");
    char *pull = obj_getStr(self, "pull");
    GPIOModeTypeDef pmode = gpio_get_mode(mode, pull);

    if (gpio_get_group(pin) == 0)
    {
        GPIOA_ModeCfg(gpio_get_pin(pin), pmode);
    }
    else
    {
        GPIOB_ModeCfg(gpio_get_pin(pin), pmode);
    }
}

void CH582_GPIO_platformHigh(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");

    if (gpio_get_group(pin) == 0)
    {
        GPIOA_SetBits(gpio_get_pin(pin));
    }
    else
    {
        GPIOB_SetBits(gpio_get_pin(pin));
    }
}

void CH582_GPIO_platformLow(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");

    if (gpio_get_group(pin) == 0)
    {
        GPIOA_ResetBits(gpio_get_pin(pin));
    }
    else
    {
        GPIOB_ResetBits(gpio_get_pin(pin));
    }
}

void CH582_GPIO_platformRead(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");
    if (gpio_get_group(pin) == 0)
    {
        obj_setInt(self, "readBuff", GPIOA_ReadPortPin(gpio_get_pin(pin)));
    }
    else
    {
        obj_setInt(self, "readBuff", GPIOB_ReadPortPin(gpio_get_pin(pin)));
    }
}

void CH582_GPIO_platformSetMode(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");
    char *mode = obj_getStr(self, "mode");
    GPIOModeTypeDef pmode = gpio_get_mode(mode, NULL);

    if (gpio_get_group(pin) == 0)
    {
        GPIOA_ModeCfg(gpio_get_pin(pin), pmode);
    }
    else
    {
        GPIOB_ModeCfg(gpio_get_pin(pin), pmode);
    }
}