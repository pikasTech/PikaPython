#include "AIR32F1_common.h"
#include "pikaScript.h"

uint8_t enableClk(char* pin) {
    if (strIsStartWith(pin, "PA")) {
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
        return 0;
    }
    if (strIsStartWith(pin, "PB")) {
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
        return 0;
    }
    if (strIsStartWith(pin, "PC")) {
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 
        return 0;
    }
    if (strIsStartWith(pin, "PD")) {
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); 
        return 0;
    }
    return 1;
}

GPIO_TypeDef* getGpioPort(char* pin) {
    if (strIsStartWith(pin, "PA")) {
        return GPIOA;
    }
    if (strIsStartWith(pin, "PB")) {
        return GPIOB;
    }
    if (strIsStartWith(pin, "PC")) {
        return GPIOC;
    }
    if (strIsStartWith(pin, "PD")) {
        return GPIOD;
    }
    return NULL;
}

uint16_t getGpioPin(char* pin) {
    Args* buffs = New_strBuff();
    uint16_t gpioPin = 0;

    pin = strsCopy(buffs, pin + 2);
    if (strEqu(pin, "0")) {
        gpioPin = GPIO_Pin_0;
        goto exit;
    }
    if (strEqu(pin, "1")) {
        gpioPin = GPIO_Pin_1;
        goto exit;
    }
    if (strEqu(pin, "2")) {
        gpioPin = GPIO_Pin_2;
        goto exit;
    }
    if (strEqu(pin, "3")) {
        gpioPin = GPIO_Pin_3;
        goto exit;
    }
    if (strEqu(pin, "4")) {
        gpioPin = GPIO_Pin_4;
        goto exit;
    }
    if (strEqu(pin, "5")) {
        gpioPin = GPIO_Pin_5;
        goto exit;
    }
    if (strEqu(pin, "6")) {
        gpioPin = GPIO_Pin_6;
        goto exit;
    }
    if (strEqu(pin, "7")) {
        gpioPin = GPIO_Pin_7;
        goto exit;
    }
    if (strEqu(pin, "8")) {
        gpioPin = GPIO_Pin_8;
        goto exit;
    }
    if (strEqu(pin, "9")) {
        gpioPin = GPIO_Pin_9;
        goto exit;
    }
    if (strEqu(pin, "10")) {
        gpioPin = GPIO_Pin_10;
        goto exit;
    }
    if (strEqu(pin, "11")) {
        gpioPin = GPIO_Pin_11;
        goto exit;
    }
    if (strEqu(pin, "12")) {
        gpioPin = GPIO_Pin_12;
        goto exit;
    }
    if (strEqu(pin, "13")) {
        gpioPin = GPIO_Pin_13;
        goto exit;
    }
    if (strEqu(pin, "14")) {
        gpioPin = GPIO_Pin_14;
        goto exit;
    }
    if (strEqu(pin, "15")) {
        gpioPin = GPIO_Pin_15;
        goto exit;
    }

exit:
    args_deinit(buffs);
    return gpioPin;
}

GPIOMode_TypeDef getPinMode(char* mode) {
    if (strEqu(mode, "out")) {
        return GPIO_Mode_Out_PP;
    }
    if (strEqu(mode, "in")) {
        return GPIO_Mode_AIN;
    }
    return GPIO_Mode_AIN;
}
