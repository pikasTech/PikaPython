#include "STM32_GPIO.h"
#include <stdint.h>
#include "BaseObj.h"
#include "STM32_common.h"
#include "dataStrs.h"


void STM32_GPIO_platformDisable(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    char* mode = obj_getStr(self, "mode");

    GPIO_TypeDef* gpioPort = getGpioPort(pin);

    if (NULL == gpioPort) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }

    uint16_t gpioPin = getGpioPin(pin);

    if (0 == gpioPin) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }

    HAL_GPIO_DeInit(gpioPort, gpioPin);
}

void STM32_GPIO_platformEnable(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    char* mode = obj_getStr(self, "mode");

    if (0 != enableClk(pin)) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
        return;
    }

    GPIO_TypeDef* gpioPort = getGpioPort(pin);

    if (NULL == gpioPort) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }

    uint16_t gpioPin = getGpioPin(pin);

    if (0 == gpioPin) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_RESET);

    uint32_t pinMode = getPinMode(mode);

    uint32_t gpioPull = GPIO_NOPULL;
    char *pull = obj_getStr(self, "pull");
    if(strEqu(pull, "up")){
        gpioPull = GPIO_PULLUP;
    }else if(strEqu(pull, "down")){
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
void STM32_GPIO_platformLow(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    GPIO_TypeDef* gpioPort = getGpioPort(pin);

    if (NULL == gpioPort) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }

    uint16_t gpioPin = getGpioPin(pin);

    if (0 == gpioPin) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }
    HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_RESET);
}
void STM32_GPIO_platformHigh(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    GPIO_TypeDef* gpioPort = getGpioPort(pin);

    if (NULL == gpioPort) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }

    uint16_t gpioPin = getGpioPin(pin);

    if (0 == gpioPin) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }
    HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_SET);
}
void STM32_GPIO_platformSetMode(PikaObj* self, char* mode) {
    char* pin = obj_getStr(self, "pin");

    if (0 != enableClk(pin)) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
        return;
    }

    GPIO_TypeDef* gpioPort = getGpioPort(pin);

    if (NULL == gpioPort) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }

    uint16_t gpioPin = getGpioPin(pin);

    if (0 == gpioPin) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_RESET);

    uint32_t pinMode = getPinMode(mode);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO*/
    GPIO_InitStruct.Pin = gpioPin;
    GPIO_InitStruct.Mode = pinMode;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(gpioPort, &GPIO_InitStruct);
}

int STM32_GPIO_platformRead(PikaObj *self){
    char* pin = obj_getStr(self, "pin");
    GPIO_TypeDef* gpioPort = getGpioPort(pin);
    if (NULL == gpioPort) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }
    uint16_t gpioPin = getGpioPin(pin);
    if (0 == gpioPin) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }
    return HAL_GPIO_ReadPin(gpioPort,gpioPin);
}

int STM32_lowLevel_readPin(PikaObj *self, char * pin){
    GPIO_TypeDef* gpioPort = getGpioPort(pin);
    if (NULL == gpioPort) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }
    uint16_t gpioPin = getGpioPin(pin);
    if (0 == gpioPin) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }
    return HAL_GPIO_ReadPin(gpioPort,gpioPin);
}