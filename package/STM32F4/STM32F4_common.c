#include "STM32F4_common.h"
#include "dataStrs.h"

void __platformDisableIrqHandle(){
    __disable_irq();
}
void __platformEnableIrqHandle(){
    __enable_irq();
}

void delay_unit(uint32_t delays) {
    /* one unit is 1/64 us */
    uint32_t startval, tickn, wait;

    startval = SysTick->VAL;
    tickn = HAL_GetTick();
    if (delays > startval) {
        while (HAL_GetTick() == tickn) {
        }
        wait = 64000 + startval - delays;
        while (wait < SysTick->VAL) {
        }
    } else {
        wait = startval - delays;
        while (wait < SysTick->VAL && HAL_GetTick() == tickn) {
        }
    }
}

void delay_us(uint32_t udelay) {
    uint32_t startval, tickn, delays, wait;

    startval = SysTick->VAL;
    tickn = HAL_GetTick();
    delays = udelay * 64;  // delay 1us when delays = 64
    if (delays > startval) {
        while (HAL_GetTick() == tickn) {
        }
        wait = 64000 + startval - delays;
        while (wait < SysTick->VAL) {
        }
    } else {
        wait = startval - delays;
        while (wait < SysTick->VAL && HAL_GetTick() == tickn) {
        }
    }
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
        gpioPin = GPIO_PIN_0;
        goto exit;
    }
    if (strEqu(pin, "1")) {
        gpioPin = GPIO_PIN_1;
        goto exit;
    }
    if (strEqu(pin, "2")) {
        gpioPin = GPIO_PIN_2;
        goto exit;
    }
    if (strEqu(pin, "3")) {
        gpioPin = GPIO_PIN_3;
        goto exit;
    }
    if (strEqu(pin, "4")) {
        gpioPin = GPIO_PIN_4;
        goto exit;
    }
    if (strEqu(pin, "5")) {
        gpioPin = GPIO_PIN_5;
        goto exit;
    }
    if (strEqu(pin, "6")) {
        gpioPin = GPIO_PIN_6;
        goto exit;
    }
    if (strEqu(pin, "7")) {
        gpioPin = GPIO_PIN_7;
        goto exit;
    }
    if (strEqu(pin, "8")) {
        gpioPin = GPIO_PIN_8;
        goto exit;
    }
    if (strEqu(pin, "9")) {
        gpioPin = GPIO_PIN_9;
        goto exit;
    }
    if (strEqu(pin, "10")) {
        gpioPin = GPIO_PIN_10;
        goto exit;
    }
    if (strEqu(pin, "11")) {
        gpioPin = GPIO_PIN_11;
        goto exit;
    }
    if (strEqu(pin, "12")) {
        gpioPin = GPIO_PIN_12;
        goto exit;
    }
    if (strEqu(pin, "13")) {
        gpioPin = GPIO_PIN_13;
        goto exit;
    }
    if (strEqu(pin, "14")) {
        gpioPin = GPIO_PIN_14;
        goto exit;
    }
    if (strEqu(pin, "15")) {
        gpioPin = GPIO_PIN_15;
        goto exit;
    }

exit:
    args_deinit(buffs);
    return gpioPin;
}

uint32_t getPinMode(char* mode) {
    if (strEqu(mode, "out")) {
        return GPIO_MODE_OUTPUT_PP;
    }
    if (strEqu(mode, "in")) {
        return GPIO_MODE_INPUT;
    }
    return NULL;
}

uint8_t enableClk(char* pin) {
    if (strIsStartWith(pin, "PA")) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        return 0;
    }
    if (strIsStartWith(pin, "PB")) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        return 0;
    }
    if (strIsStartWith(pin, "PC")) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        return 0;
    }
    if (strIsStartWith(pin, "PD")) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
        return 0;
    }
    return 1;
}
