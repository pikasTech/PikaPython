#include <stdint.h>
#include "BaseObj.h"
#include "ESP32_common.h"
#include "dataStrs.h"

void ESP32_GPIO_platformDisable(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");
    int gpioPort = getGpioPin(pin);
    if (-1 == gpioPort)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
        return;
    }
    gpio_reset_pin(gpioPort);
}

void ESP32_GPIO_platformEnable(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");
    char *mode = obj_getStr(self, "mode");

    int gpioPort = getGpioPin(pin);
    if (-1 == gpioPort)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
        return;
    }

    uint32_t pinMode = getPinMode(mode);

    uint8_t gpioPullUp = 0;
    uint8_t gpioPullDown = 0;
    char *pull = obj_getStr(self, "pull");
    if (strEqu(pull, "up"))
    {
        gpioPullUp = 1;
    }
    else if (strEqu(pull, "down"))
    {
        gpioPullDown = 1;
    }

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = pinMode;
    io_conf.pin_bit_mask = 1 << gpioPort;
    io_conf.pull_down_en = gpioPullDown;
    io_conf.pull_up_en = gpioPullUp;
    gpio_config(&io_conf);
}

void ESP32_GPIO_platformHigh(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");
    int gpioPort = getGpioPin(pin);
    if (-1 == gpioPort)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
        return;
    }
    gpio_set_level(gpioPort, 1);
}
void ESP32_GPIO_platformLow(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");
    int gpioPort = getGpioPin(pin);
    if (-1 == gpioPort)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
        return;
    }
    gpio_set_level(gpioPort, 0);
}
void ESP32_GPIO_platformRead(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");
    int gpioPort = getGpioPin(pin);
    if (-1 == gpioPort)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
        return;
    }
    obj_setInt(self, "readBuff", gpio_get_level(gpioPort));
}
void ESP32_GPIO_platformSetMode(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");
    char* mode = obj_getStr(self, "mode");
    int gpioPort = getGpioPin(pin);
    if (-1 == gpioPort)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
        return;
    }

    uint32_t pinMode = getPinMode(mode);

    gpio_set_direction(gpioPort, pinMode);
}
