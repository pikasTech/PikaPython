#include "CH58x_common.h"
#include "CH582_ADC.h"
#include "ch582_utils.h"


signed short RoughCalib_Value = 0; // ADC粗调偏差值

uint32_t adc_get_channle(char *pin)
{
    uint32_t p = gpio_get_pin(pin);
    switch (p)
    {
    case GPIO_Pin_0:
        return 9;
        break;
    case GPIO_Pin_1:
        return 8;
        break;
    case GPIO_Pin_2:
        return 7;
        break;
    case GPIO_Pin_3:
        return 6;
        break;
    case GPIO_Pin_4:
        return 0;
        break;
    case GPIO_Pin_5:
        return 1;
        break;
    case GPIO_Pin_6:
        return 10;
        break;
    case GPIO_Pin_7:
        return 11;
        break;
    case GPIO_Pin_8:
        return 12;
        break;
    case GPIO_Pin_9:
        return 13;
        break;
    case GPIO_Pin_12:
        return 2;
        break;
    case GPIO_Pin_13:
        return 3;
        break;
    case GPIO_Pin_14:
        return 4;
        break;
    case GPIO_Pin_15:
        return 5;
        break;
    default:
        return -1;
        break;
    }
}

void CH582_ADC_platformDisable(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");

    ADC_ChannelCfg(0b00001111);   // 复位channel
    if (adc_get_channle(pin) < 0) // 判断一下pin是不是合理
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match adc pin.");
    }
    else
    {
        GPIOA_ModeCfg(gpio_get_pin(pin), GPIO_ModeIN_PU);
    }
}

void CH582_ADC_platformEnable(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");

    if (adc_get_channle(pin) < 0) // 判断一下pin是不是合理
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match adc pin.");
    }
    else
    {
        ADC_ExtSingleChSampInit(SampleFreq_8, ADC_PGA_0);
        GPIOA_ModeCfg(gpio_get_pin(pin), GPIO_ModeIN_Floating);
        RoughCalib_Value = ADC_DataCalib_Rough(); // 校准数据
    }
}

void CH582_ADC_platformRead(PikaObj *self)
{
    char *pin = obj_getStr(self, "pin");

    uint32_t ch = adc_get_channle(pin);
    if (ch > 0)
    {
        ADC_ChannelCfg(ch);
        obj_setFloat(self, "val", (ADC_ExcutSingleConver() + RoughCalib_Value));
    }
    else
    {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match adc pin.");
    }
}
