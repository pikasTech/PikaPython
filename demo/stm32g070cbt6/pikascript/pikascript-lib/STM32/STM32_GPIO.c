#include "BaseObj.h"
#include "STM32_GPIO.h"
#include "dataStrs.h"

#ifdef STM32G070xx
#include "stm32g0xx_hal.h"
#include <stdint.h>
#endif

static uint8_t enableClk(char *pin)
{
    if (strIsStartWith(pin, "PA"))
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        return 0;
    }
    if (strIsStartWith(pin, "PB"))
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        return 0;
    }
    if (strIsStartWith(pin, "PC"))
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        return 0;
    }
    if (strIsStartWith(pin, "PD"))
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
        return 0;
    }
    return 1;
}

static GPIO_TypeDef *getGpioPort(char *pin)
{
    if (strIsStartWith(pin, "PA"))
    {
        return GPIOA;
    }
    if (strIsStartWith(pin, "PB"))
    {
        return GPIOB;
    }
    if (strIsStartWith(pin, "PC"))
    {
        return GPIOC;
    }
    if (strIsStartWith(pin, "PD"))
    {
        return GPIOD;
    }
    return NULL;
}

static uint16_t getGpioPin(char *pin)
{
    Args *buffs = New_strBuff();
    uint16_t gpioPin = 0;

    pin = strsCopy(buffs, pin + 2);
    if (strEqu(pin, "0"))
    {
        gpioPin = GPIO_PIN_0;
        goto exit;
    }
    if (strEqu(pin, "1"))
    {
        gpioPin = GPIO_PIN_1;
        goto exit;
    }
    if (strEqu(pin, "2"))
    {
        gpioPin = GPIO_PIN_2;
        goto exit;
    }
    if (strEqu(pin, "3"))
    {
        gpioPin = GPIO_PIN_3;
        goto exit;
    }
    if (strEqu(pin, "4"))
    {
        gpioPin = GPIO_PIN_4;
        goto exit;
    }
    if (strEqu(pin, "5"))
    {
        gpioPin = GPIO_PIN_5;
        goto exit;
    }
    if (strEqu(pin, "6"))
    {
        gpioPin = GPIO_PIN_6;
        goto exit;
    }
    if (strEqu(pin, "7"))
    {
        gpioPin = GPIO_PIN_7;
        goto exit;
    }
    if (strEqu(pin, "8"))
    {
        gpioPin = GPIO_PIN_8;
        goto exit;
    }
    if (strEqu(pin, "9"))
    {
        gpioPin = GPIO_PIN_9;
        goto exit;
    }
    if (strEqu(pin, "10"))
    {
        gpioPin = GPIO_PIN_10;
        goto exit;
    }
    if (strEqu(pin, "11"))
    {
        gpioPin = GPIO_PIN_11;
        goto exit;
    }
    if (strEqu(pin, "12"))
    {
        gpioPin = GPIO_PIN_12;
        goto exit;
    }
    if (strEqu(pin, "13"))
    {
        gpioPin = GPIO_PIN_13;
        goto exit;
    }
    if (strEqu(pin, "14"))
    {
        gpioPin = GPIO_PIN_14;
        goto exit;
    }
    if (strEqu(pin, "15"))
    {
        gpioPin = GPIO_PIN_15;
        goto exit;
    }

exit:
    args_deinit(buffs);
    return gpioPin;
}

static uint32_t getPinMode(char *mode)
{
    if (strEqu(mode, "out"))
    {
        return GPIO_MODE_OUTPUT_PP;
    }
    if (strEqu(mode, "in"))
    {
        return GPIO_MODE_INPUT;
    }
    return NULL;
}

void STM32_GPIO_platformDisable(PikaObj *self)
{

    char *pin = obj_getStr(self, "pin");
    char *mode = obj_getStr(self, "mode");

    GPIO_TypeDef *gpioPort = getGpioPort(pin);

    if (NULL == gpioPort)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }

    uint16_t gpioPin = getGpioPin(pin);

    if (0 == gpioPin)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }

    HAL_GPIO_DeInit(gpioPort, gpioPin);
}

void STM32_GPIO_platformEnable(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");
    char *mode = obj_getStr(self, "mode");
        
    if (0 != enableClk(pin))
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
        return;
    }

    GPIO_TypeDef *gpioPort = getGpioPort(pin);

    if (NULL == gpioPort)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }

    uint16_t gpioPin = getGpioPin(pin);

    if (0 == gpioPin)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_RESET);

    uint32_t pinMode = getPinMode(mode);
    if (NULL == pinMode)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio mode.");
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO*/
    GPIO_InitStruct.Pin = gpioPin;
    GPIO_InitStruct.Mode = pinMode;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(gpioPort, &GPIO_InitStruct);
}
void STM32_GPIO_platformLow(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");
    GPIO_TypeDef *gpioPort = getGpioPort(pin);

    if (NULL == gpioPort)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }

    uint16_t gpioPin = getGpioPin(pin);

    if (0 == gpioPin)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }
    HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_RESET);
}
void STM32_GPIO_platformHigh(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");
    GPIO_TypeDef *gpioPort = getGpioPort(pin);

    if (NULL == gpioPort)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }

    uint16_t gpioPin = getGpioPin(pin);

    if (0 == gpioPin)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }
    HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_SET);
}
void STM32_GPIO_platformSetMode(PikaObj *self, char *mode)
{
    char *pin = obj_getStr(self, "pin");

    if (0 != enableClk(pin))
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
        return;
    }

    GPIO_TypeDef *gpioPort = getGpioPort(pin);

    if (NULL == gpioPort)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
    }

    uint16_t gpioPin = getGpioPin(pin);

    if (0 == gpioPin)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio pin.");
    }

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_RESET);

    uint32_t pinMode = getPinMode(mode);
    if (NULL == pinMode)
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio mode.");
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO*/
    GPIO_InitStruct.Pin = gpioPin;
    GPIO_InitStruct.Mode = pinMode;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(gpioPort, &GPIO_InitStruct);
}