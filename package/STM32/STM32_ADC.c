#include "STM32_ADC.h"
#include <stdint.h>
#include "BaseObj.h"
#include "STM32_common.h"
#include "dataStrs.h"
#include "main.h"

uint16_t Get_Adc(uint32_t channel) {
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, channel);
    LL_ADC_SetChannelSamplingTime(ADC1, channel, LL_ADC_SAMPLINGTIME_COMMON_1);
    LL_ADC_REG_StartConversion(ADC1);
    /* wait */
    while (0 == LL_ADC_IsActiveFlag_EOC(ADC1)) {
    }
    return LL_ADC_REG_ReadConversionData12(ADC1);
}

void STM32_ADC_platformEnable(PikaObj* self) {
    /* init gpio */
    char* pin = obj_getStr(self, "pin");
    if (!strIsStartWith(pin, "PA")) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] not match adc pin.");
        return;
    }

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
    LL_ADC_InitTypeDef ADC_InitStruct = {0};
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);
    ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV2;
    ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
    ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
    LL_ADC_Init(ADC1, &ADC_InitStruct);
    LL_ADC_REG_SetSequencerConfigurable(ADC1, LL_ADC_REG_SEQ_CONFIGURABLE);
    while (LL_ADC_IsActiveFlag_CCRDY(ADC1) == 0) {
    }
    LL_ADC_ClearFlag_CCRDY(ADC1);
    ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
    ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
    LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
    LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);
    LL_ADC_SetTriggerFrequencyMode(ADC1, LL_ADC_CLOCK_FREQ_MODE_HIGH);
    LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_COMMON_1,
                                         LL_ADC_SAMPLINGTIME_160CYCLES_5);
    LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_COMMON_2,
                                         LL_ADC_SAMPLINGTIME_160CYCLES_5);
    LL_ADC_DisableIT_EOC(ADC1);
    LL_ADC_DisableIT_EOS(ADC1);
    /* Enable ADC internal voltage regulator */
    LL_ADC_EnableInternalRegulator(ADC1);
    uint32_t wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US *
                                 (SystemCoreClock / (100000 * 2))) /
                                10);
    while (wait_loop_index != 0) {
        wait_loop_index--;
    }
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0,
                                  LL_ADC_SAMPLINGTIME_COMMON_1);
//    LL_ADC_StartCalibration(ADC1);
//    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0) {
//    };
    LL_ADC_Enable(ADC1);
    while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0) {
    };
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

void STM32_ADC_platformRead(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    obj_setFloat(self, "val", 3.3f * Get_Adc(getChannel(pin)) / 4096.0f);
}
