#include "STM32G0_PWM.h"
#include <stdint.h>
#include "BaseObj.h"
#include "STM32G0_common.h"
#include "dataStrs.h"

static TIM_TypeDef* PWM_get_TIM_instance(char* pin) {
#ifdef TIM1_EXIST
    if (strEqu("PA8", pin) || strEqu("PA9", pin) || strEqu("PA10", pin) ||
        strEqu("PA11", pin)) {
        return TIM1;
    }
#endif
#ifdef TIM2_EXIST
    if (strEqu("PA0", pin) || strEqu("PA1", pin) || strEqu("PA2", pin) ||
        strEqu("PA3", pin)) {
        return TIM2;
    }
#endif
#ifdef TIM3_EXIST
    if (strEqu("PA6", pin) || strEqu("PA7", pin) || strEqu("PB0", pin) ||
        strEqu("PB1", pin)) {
        return TIM3;
    }
#endif
#ifdef TIM4_EXIST
    if (strEqu("PB6", pin) || strEqu("PB7", pin) || strEqu("PB8", pin) ||
        strEqu("PB9", pin)) {
        return TIM4;
    }
#endif
#ifdef TIM14_EXIST
    if (strEqu("PA4", pin)) {
        return TIM14;
    }
#endif
#ifdef TIM16_EXIST
    if (strEqu("PD0", pin)) {
        return TIM16;
    }
#endif
#ifdef TIM17_EXIST
    if (strEqu("PD1", pin)) {
        return TIM17;
    }
#endif
    return NULL;
}

static uint32_t TIM_get_GPIO_alternate(TIM_TypeDef* TIMx) {
    if (TIM1 == TIMx) {
        return GPIO_AF2_TIM1;
    }
    if (TIM3 == TIMx) {
        return GPIO_AF1_TIM3;
    }
    if (TIM14 == TIMx) {
        return GPIO_AF4_TIM14;
    }
    if (TIM16 == TIMx) {
        return GPIO_AF2_TIM16;
    }
    if (TIM17 == TIMx) {
        return GPIO_AF2_TIM17;
    }
    return 0;
}

static void TIM_clock_enable(TIM_TypeDef* TIMx) {
    if (TIM1 == TIMx) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
        return;
    }
    if (TIM3 == TIMx) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
        return;
    }
    if (TIM14 == TIMx) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM14);
        return;
    }
    if (TIM16 == TIMx) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16);
        return;
    }
    if (TIM17 == TIMx) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17);
        return;
    }
}

uint8_t PWM_GPIO_init(char* pin) {
    TIM_TypeDef* TIMx = PWM_get_TIM_instance(pin);
    if (NULL == TIMx) {
        /* this Pin do not match any PWM generator */
        return 1;
    }
    GPIO_enable_clock(pin);
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_get_pin(pin);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = TIM_get_GPIO_alternate(TIMx);
    LL_GPIO_Init(GPIO_get_Group(pin), &GPIO_InitStruct);
    return 0;
}

uint32_t PWM_get_LL_TIM_channel(char* pin) {
    if (strEqu("PA8", pin) || strEqu("PA0", pin) || strEqu("PA6", pin) ||
        strEqu("PB6", pin) || strEqu("PA4", pin) || strEqu("PD0", pin) ||
        strEqu("PD1", pin)) {
        return LL_TIM_CHANNEL_CH1;
    }
    if (strEqu("PA9", pin) || strEqu("PA1", pin) || strEqu("PB7", pin) ||
        strEqu("PA7", pin)) {
        return LL_TIM_CHANNEL_CH2;
    }
    if (strEqu("PA10", pin) || strEqu("PA2", pin) || strEqu("PB8", pin) ||
        strEqu("PB0", pin)) {
        return LL_TIM_CHANNEL_CH3;
    }
    if (strEqu("PA11", pin) || strEqu("PA3", pin) || strEqu("PB9", pin) ||
        strEqu("PB1", pin)) {
        return LL_TIM_CHANNEL_CH4;
    }
    /* Chennel not match */
    return 99999;
}

void STM32G0_PWM_platformEnable(PikaObj* self) {
    float duty = obj_getFloat(self, "duty");
    int freq = obj_getInt(self, "freq");
    char* pin = obj_getStr(self, "pin");
    TIM_TypeDef* TIMx = PWM_get_TIM_instance(pin);

    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    LL_TIM_BDTR_InitTypeDef TIM_BDTRInitStruct = {0};
    uint32_t LL_TIM_channel = PWM_get_LL_TIM_channel(pin);
    /* Peripheral clock enable */
    TIM_clock_enable(TIMx);

    TIM_InitStruct.Prescaler = 64 - 1;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload =
        (uint32_t)((float)(1000 * 1000) / (float)freq) - 1;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    TIM_InitStruct.RepetitionCounter = 0;
    LL_TIM_Init(TIMx, &TIM_InitStruct);
    LL_TIM_DisableARRPreload(TIMx);
    LL_TIM_OC_EnablePreload(TIMx, LL_TIM_channel);
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    /* calculate pulse by duty and freq */
    TIM_OC_InitStruct.CompareValue = TIM_InitStruct.Autoreload * duty;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
    TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
    LL_TIM_OC_Init(TIMx, LL_TIM_channel, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(TIMx, LL_TIM_channel);
    LL_TIM_SetTriggerOutput(TIMx, LL_TIM_TRGO_RESET);
    LL_TIM_SetTriggerOutput2(TIMx, LL_TIM_TRGO2_RESET);
    LL_TIM_DisableMasterSlaveMode(TIMx);
    TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
    TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
    TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
    TIM_BDTRInitStruct.DeadTime = 0;
    TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
    TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
    TIM_BDTRInitStruct.BreakFilter = LL_TIM_BREAK_FILTER_FDIV1;
    TIM_BDTRInitStruct.BreakAFMode = LL_TIM_BREAK_AFMODE_INPUT;
    TIM_BDTRInitStruct.Break2State = LL_TIM_BREAK2_DISABLE;
    TIM_BDTRInitStruct.Break2Polarity = LL_TIM_BREAK2_POLARITY_HIGH;
    TIM_BDTRInitStruct.Break2Filter = LL_TIM_BREAK2_FILTER_FDIV1;
    TIM_BDTRInitStruct.Break2AFMode = LL_TIM_BREAK_AFMODE_INPUT;
    TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
    LL_TIM_BDTR_Init(TIMx, &TIM_BDTRInitStruct);
    /* init gpio */
    if (0 != PWM_GPIO_init(pin)) {
        obj_setSysOut(self, "[error]: init PWM port faild.");
        obj_setErrorCode(self, 1);
        return;
    }
    /* start */
    LL_TIM_CC_EnableChannel(TIMx, LL_TIM_channel);
    LL_TIM_EnableCounter(TIMx);
    LL_TIM_EnableAllOutputs(TIMx);
}

void PWM_set_duty(TIM_TypeDef* TIMx, uint32_t LL_TIM_channel, float duty) {
    if (LL_TIM_CHANNEL_CH1 == LL_TIM_channel) {
        LL_TIM_OC_SetCompareCH1(TIMx, LL_TIM_GetAutoReload(TIMx) * duty);
        return;
    }
    if (LL_TIM_CHANNEL_CH2 == LL_TIM_channel) {
        LL_TIM_OC_SetCompareCH2(TIMx, LL_TIM_GetAutoReload(TIMx) * duty);
        return;
    }
    if (LL_TIM_CHANNEL_CH3 == LL_TIM_channel) {
        LL_TIM_OC_SetCompareCH3(TIMx, LL_TIM_GetAutoReload(TIMx) * duty);
        return;
    }
    if (LL_TIM_CHANNEL_CH4 == LL_TIM_channel) {
        LL_TIM_OC_SetCompareCH4(TIMx, LL_TIM_GetAutoReload(TIMx) * duty);
        return;
    }
}

void STM32G0_PWM_platformSetDuty(PikaObj* self) {
    float duty = obj_getFloat(self, "duty");
    char* pin = obj_getStr(self, "pin");
    uint32_t LL_TIM_channel = PWM_get_LL_TIM_channel(pin);
    TIM_TypeDef* TIMx = PWM_get_TIM_instance(pin);
    if (NULL == TIMx) {
        obj_setSysOut(self, "[error]: can not found PWM hardware.");
        obj_setErrorCode(self, 1);
        return;
    }
    PWM_set_duty(TIMx, LL_TIM_channel, duty);
}

void STM32G0_PWM_platformSetFrequency(PikaObj* self) {
    int freq = obj_getInt(self, "freq");
    char* pin = obj_getStr(self, "pin");
    TIM_TypeDef* TIMx = PWM_get_TIM_instance(pin);
    if (NULL == TIMx) {
        obj_setSysOut(self, "[error]: can not found PWM hardware.");
        obj_setErrorCode(self, 1);
        return;
    }
    /* update frequency in run time */
    LL_TIM_SetAutoReload(TIMx,
                         (uint32_t)((float)(1000 * 1000) / (float)freq) - 1);
    float duty = obj_getFloat(self, "duty");
    uint32_t LL_TIM_channel = PWM_get_LL_TIM_channel(pin);
    PWM_set_duty(TIMx, LL_TIM_channel, duty);
}
