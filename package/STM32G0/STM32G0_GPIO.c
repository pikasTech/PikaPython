#include "STM32G0_GPIO.h"
#include <stdint.h>
#include "BaseObj.h"
#include "STM32G0_common.h"
#include "dataStrs.h"


int STM32G0_lowLevel_readPin(PikaObj* self, char* pin) {
    GPIO_TypeDef* gpioPort = GPIO_get_Group(pin);
    if (NULL == gpioPort) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }
    uint16_t gpioPin = GPIO_get_pin(pin);
    if (0 == gpioPin) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }
    return LL_GPIO_IsInputPinSet(gpioPort, gpioPin);
}
