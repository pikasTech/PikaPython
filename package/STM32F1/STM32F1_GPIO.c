#include <stdint.h>
#include "BaseObj.h"
#include "STM32F1_common.h"
#include "dataStrs.h"
#include "stm32f1xx_ll_gpio.h"

void STM32F1_GPIO_platformDisable(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    char* mode = obj_getStr(self, "mode");

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

    HAL_GPIO_DeInit(gpioPort,gpioPin);
}

void STM32F1_GPIO_platformEnable(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    char* mode = obj_getStr(self, "mode");

    if (0 != GPIO_enable_clock(pin)) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
        return;
    }

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

    /*Configure GPIO pin Output Level */
    LL_GPIO_ResetOutputPin(gpioPort, gpioPin);

    uint32_t pinMode = getPinMode(mode);

    uint32_t gpioPull = GPIO_NOPULL;
    char* pull = obj_getStr(self, "pull");
    if (strEqu(pull, "up")) {
        gpioPull = GPIO_PULLUP;
    } else if (strEqu(pull, "down")) {
        gpioPull = GPIO_PULLDOWN;
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO*/
    GPIO_InitStruct.Pin = gpioPin;
    GPIO_InitStruct.Mode = pinMode;
    GPIO_InitStruct.Pull = gpioPull;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(gpioPort, &GPIO_InitStruct);
}
void STM32F1_GPIO_platformLow(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
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
		
		WRITE_REG(gpioPort->BSRR,  (uint32_t)gpioPin << 16U);

}
void STM32F1_GPIO_platformHigh(PikaObj* self) {
		
    char* pin = obj_getStr(self, "pin");
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
		
		WRITE_REG(gpioPort->BSRR,  gpioPin);

}

void STM32F1_GPIO_platformSetMode(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    char* mode = obj_getStr(self, "mode");
    if (0 != GPIO_enable_clock(pin)) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
        return;
    }

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

    /*Configure GPIO pin Output Level */
    LL_GPIO_ResetOutputPin(gpioPort, gpioPin);

    uint32_t pinMode = getPinMode(mode);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO*/
    GPIO_InitStruct.Pin = gpioPin;
    GPIO_InitStruct.Mode = pinMode;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(gpioPort, &GPIO_InitStruct);
}

void STM32F1_GPIO_platformRead(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
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
    obj_setInt(self, "readBuff", LL_GPIO_IsInputPinSet(gpioPort, gpioPin));
}

int STM32F1_lowLevel_readPin(PikaObj* self, char* pin) {
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
