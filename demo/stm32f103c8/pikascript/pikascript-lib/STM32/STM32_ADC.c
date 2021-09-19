#include "STM32_ADC.h"
#include <stdint.h>
#include "BaseObj.h"
#include "STM32_common.h"
#include "dataStrs.h"

ADC_HandleTypeDef pika_hadc1 = {0};

uint16_t Get_Adc(ADC_HandleTypeDef* hadc, uint32_t ch) {
    ADC_ChannelConfTypeDef ADC_ChanConf;
    ADC_ChanConf.Channel = ch; 
    ADC_ChanConf.Rank = ADC_REGULAR_RANK_1;
    #if (defined STM32G070xx) || (defined STM32G030xx)  
        ADC_ChanConf.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;  
    #endif
    #ifdef STM32F103xB
        ADC_ChanConf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
    #endif
    HAL_ADC_ConfigChannel(hadc, &ADC_ChanConf);             
    HAL_ADC_Start(hadc);                                    
    HAL_ADC_PollForConversion(hadc, 10);                    
    return (uint16_t)HAL_ADC_GetValue(hadc);  
}

void STM32_ADC_platformEnable(PikaObj* self, char* pin) {
    if (!strIsStartWith(pin, "PA")) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match adc pin.");
        return;
    }

    /* MSP Init */
    #if (defined STM32G070xx) || (defined STM32G030xx)  
        __HAL_RCC_ADC_CLK_ENABLE();
    #endif
    #ifdef STM32F103xB
        __HAL_RCC_ADC1_CLK_ENABLE();
    #endif
    if (0 != enableClk(pin)) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match gpio port.");
        return;
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = getGpioPin(pin);
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(getGpioPort(pin), &GPIO_InitStruct);

    /* init ADC */
    pika_hadc1.Instance = ADC1;
    #if (defined STM32G070xx) || (defined STM32G030xx)
        pika_hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
        pika_hadc1.Init.Resolution = ADC_RESOLUTION_12B;
        pika_hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
        pika_hadc1.Init.LowPowerAutoWait = DISABLE;
        pika_hadc1.Init.LowPowerAutoPowerOff = DISABLE;  
    #endif  
    pika_hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    pika_hadc1.Init.ContinuousConvMode = DISABLE;
    pika_hadc1.Init.DiscontinuousConvMode = DISABLE;
    pika_hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    pika_hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;  
    pika_hadc1.Init.NbrOfConversion = 1;  
    #if (defined STM32G070xx) || (defined STM32G030xx)
        pika_hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
        pika_hadc1.Init.DMAContinuousRequests = DISABLE;
        pika_hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
        pika_hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_1CYCLE_5;
        pika_hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_1CYCLE_5;
        pika_hadc1.Init.OversamplingMode = DISABLE;
        pika_hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
    #endif

    HAL_StatusTypeDef state = HAL_ADC_Init(&pika_hadc1);
    if (state != HAL_OK) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] adc init faild.");
    return;
    }

    /* Run the ADC calibration */
    if (HAL_ADCEx_Calibration_Start(&pika_hadc1) != HAL_OK) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] adc calibratie faild.");
    return;
    }
}

uint32_t getChannel(char* pin) {
    Args* buffs = New_strBuff();
    uint32_t channel = 0;

    pin = strsCopy(buffs, pin + 2);
    if (strEqu(pin, "0")) {
        channel = ADC_CHANNEL_0;
        goto exit;
    }
    if (strEqu(pin, "1")) {
        channel = ADC_CHANNEL_1;
        goto exit;
    }
    if (strEqu(pin, "2")) {
        channel = ADC_CHANNEL_2;
        goto exit;
    }
    if (strEqu(pin, "3")) {
        channel = ADC_CHANNEL_3;
        goto exit;
    }
    if (strEqu(pin, "4")) {
        channel = ADC_CHANNEL_4;
        goto exit;
    }
    if (strEqu(pin, "5")) {
        channel = ADC_CHANNEL_5;
        goto exit;
    }
    if (strEqu(pin, "6")) {
        channel = ADC_CHANNEL_6;
        goto exit;
    }
    if (strEqu(pin, "7")) {
        channel = ADC_CHANNEL_7;
        goto exit;
    }
    if (strEqu(pin, "8")) {
        channel = ADC_CHANNEL_8;
        goto exit;
    }
    if (strEqu(pin, "9")) {
        channel = ADC_CHANNEL_9;
        goto exit;
    }
    if (strEqu(pin, "10")) {
        channel = ADC_CHANNEL_10;
        goto exit;
    }
    if (strEqu(pin, "11")) {
        channel = ADC_CHANNEL_11;
        goto exit;
    }
    if (strEqu(pin, "12")) {
        channel = ADC_CHANNEL_12;
        goto exit;
    }
    if (strEqu(pin, "13")) {
        channel = ADC_CHANNEL_13;
        goto exit;
    }
    if (strEqu(pin, "14")) {
        channel = ADC_CHANNEL_14;
        goto exit;
    }
    if (strEqu(pin, "15")) {
        channel = ADC_CHANNEL_15;
        goto exit;
    }
exit:
    args_deinit(buffs);
    return channel;
}

float STM32_ADC_platformRead(PikaObj* self, char* pin) {
    return 3.3f * Get_Adc(&pika_hadc1, getChannel(pin)) / 4096.0f;
}
