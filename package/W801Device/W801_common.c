#include "W801_common.h"
#include "dataStrs.h"

//void delay_unit(uint32_t delays) {
//    /* one unit is 1/64 us */
//    uint32_t startval, tickn, wait;
//
//    startval = SysTick->VAL;
//    tickn = HAL_GetTick();
//    if (delays > startval) {
//        while (HAL_GetTick() == tickn) {
//        }
//        wait = 64000 + startval - delays;
//        while (wait < SysTick->VAL) {
//        }
//    } else {
//        wait = startval - delays;
//        while (wait < SysTick->VAL && HAL_GetTick() == tickn) {
//        }
//    }
//}

GPIO_TypeDef* GPIO_get_Group(char* pin) {
    if (strIsStartWith(pin, "PA")) {
        return GPIOA;
    }
    if (strIsStartWith(pin, "PB")) {
        return GPIOB;
    }

    return NULL;
}

uint32_t GPIO_get_pin(char* pin) {
    Args* buffs = New_strBuff();
    uint32_t gpioPin = 0;

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
	
	 if (strEqu(pin, "16")) {
        gpioPin = GPIO_PIN_16;
        goto exit;
    }
    if (strEqu(pin, "17")) {
        gpioPin = GPIO_PIN_17;
        goto exit;
    }
    if (strEqu(pin, "18")) {
        gpioPin = GPIO_PIN_18;
        goto exit;
    }
    if (strEqu(pin, "19")) {
        gpioPin = GPIO_PIN_19;
        goto exit;
    }
    if (strEqu(pin, "20")) {
        gpioPin = GPIO_PIN_20;
        goto exit;
    }
    if (strEqu(pin, "21")) {
        gpioPin = GPIO_PIN_21;
        goto exit;
    }
    if (strEqu(pin, "22")) {
        gpioPin = GPIO_PIN_22;
        goto exit;
    }
    if (strEqu(pin, "23")) {
        gpioPin = GPIO_PIN_23;
        goto exit;
    }
    if (strEqu(pin, "24")) {
        gpioPin = GPIO_PIN_24;
        goto exit;
    }
    if (strEqu(pin, "25")) {
        gpioPin = GPIO_PIN_25;
        goto exit;
    }
    if (strEqu(pin, "26")) {
        gpioPin = GPIO_PIN_26;
        goto exit;
    }
    if (strEqu(pin, "27")) {
        gpioPin = GPIO_PIN_27;
        goto exit;
    }
    if (strEqu(pin, "28")) {
        gpioPin = GPIO_PIN_28;
        goto exit;
    }
    if (strEqu(pin, "29")) {
        gpioPin = GPIO_PIN_29;
        goto exit;
    }
    if (strEqu(pin, "30")) {
        gpioPin = GPIO_PIN_30;
        goto exit;
    }
    if (strEqu(pin, "31")) {
        gpioPin = GPIO_PIN_31;
        goto exit;
    }


exit:
    args_deinit(buffs);
    return gpioPin;
}

uint32_t getPinMode(char* mode) {
    if (strEqu(mode, "out")) {
        return GPIO_MODE_OUTPUT;
    }
    if (strEqu(mode, "in")) {
        return GPIO_MODE_INPUT;
    }
    return NULL;
}

uint8_t GPIO_enable_clock(char* pin) {
   
    if (strIsStartWith(pin, "PA")) {
        __HAL_RCC_GPIO_CLK_ENABLE();
        return 0;
    }
    if (strIsStartWith(pin, "PB")) {
        __HAL_RCC_GPIO_CLK_ENABLE();
        return 0;
    }
    return 1;
}
