#include "AIR32F1_GPIO.h"
#include "AIR32F1_common.h"

void AIR32F1_GPIO_platformEnable(PikaObj* self) {
	char* pin = obj_getStr(self, "pin");
	char* mode = obj_getStr(self, "mode");
	GPIO_InitTypeDef GPIO_InitStructure;

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
	
	GPIOMode_TypeDef pinMode = getPinMode(mode);
	
	
	GPIO_InitStructure.GPIO_Pin = gpioPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = pinMode;	 
	GPIO_Init(gpioPort, &GPIO_InitStructure); 
	AIR32F1_GPIO_platformHigh(self);
}

void AIR32F1_GPIO_platformHigh(PikaObj* self) {
	char* pin = obj_getStr(self, "pin");
	GPIO_TypeDef* gpioPort = getGpioPort(pin);
	uint16_t gpioPin = getGpioPin(pin);

	GPIO_SetBits(gpioPort, gpioPin);
}

void AIR32F1_GPIO_platformLow(PikaObj* self) {
	char* pin = obj_getStr(self, "pin");
	GPIO_TypeDef* gpioPort = getGpioPort(pin);
	uint16_t gpioPin = getGpioPin(pin);

	GPIO_ResetBits(gpioPort, gpioPin);
}

void AIR32F1_GPIO_platformRead(PikaObj* self) {
	char* pin = obj_getStr(self, "pin");
	GPIO_TypeDef* gpioPort = getGpioPort(pin);
	uint16_t gpioPin = getGpioPin(pin);

	obj_setInt(self, "readBuff", GPIO_ReadOutputDataBit(gpioPort, gpioPin));
}

void AIR32F1_GPIO_platformSetMode(PikaObj* self) {
	AIR32F1_GPIO_platformEnable(self);
}
