#include "wm_hal.h"
#include <stdint.h>
#include "BaseObj.h"
#include "dataStrs.h"
#include "W801Device_GPIO.h"

void W801Device_GPIO_platformDisable(PikaObj *self)
{
	
	char* pin = obj_getStr(self, "pin");
    char* mode = obj_getStr(self, "mode");

    GPIO_TypeDef* gpioPort = GPIO_get_Group(pin);

    if (NULL == gpioPort) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }

    uint32_t gpioPin = GPIO_get_pin(pin);

    if (0 == gpioPin) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }

   HAL_GPIO_DeInit(gpioPort,gpioPin);
}


void W801Device_GPIO_platformEnable(PikaObj *self)
{
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

    uint32_t gpioPin = GPIO_get_pin(pin);

    if (0 == gpioPin) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }

    /*Configure GPIO pin Output Level */
	
	HAL_GPIO_WritePin(gpioPort,gpioPin,GPIO_PIN_RESET);
    
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
    HAL_GPIO_Init(gpioPort, &GPIO_InitStruct);
	
}

void W801Device_GPIO_platformHigh(PikaObj *self)
{
	char* pin = obj_getStr(self, "pin");
    GPIO_TypeDef* gpioPort = GPIO_get_Group(pin);

    if (NULL == gpioPort) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }

    uint32_t gpioPin = GPIO_get_pin(pin);

    if (0 == gpioPin) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }
    HAL_GPIO_WritePin(gpioPort, gpioPin,GPIO_PIN_SET);
	
}

void W801Device_GPIO_platformLow(PikaObj *self)
{
	char* pin = obj_getStr(self, "pin");
    GPIO_TypeDef* gpioPort = GPIO_get_Group(pin);

    if (NULL == gpioPort) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }

    uint32_t gpioPin = GPIO_get_pin(pin);

    if (0 == gpioPin) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }
    HAL_GPIO_WritePin(gpioPort, gpioPin,GPIO_PIN_RESET);
}

void W801Device_GPIO_platformRead(PikaObj *self)
{
	char* pin = obj_getStr(self, "pin");
    GPIO_TypeDef* gpioPort = GPIO_get_Group(pin);
    if (NULL == gpioPort) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }
    uint32_t gpioPin = GPIO_get_pin(pin);
    if (0 == gpioPin) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }
    obj_setInt(self, "readBuff", HAL_GPIO_ReadPin(gpioPort, gpioPin));
}

void W801Device_GPIO_platformSetMode(PikaObj *self)
{
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

    uint32_t gpioPin = GPIO_get_pin(pin);

    if (0 == gpioPin) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }

    /*Configure GPIO pin Output Level */
   HAL_GPIO_WritePin(gpioPort, gpioPin,GPIO_PIN_RESET);

    uint32_t pinMode = getPinMode(mode);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO*/
    GPIO_InitStruct.Pin = gpioPin;
    GPIO_InitStruct.Mode = pinMode;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(gpioPort, &GPIO_InitStruct);
}