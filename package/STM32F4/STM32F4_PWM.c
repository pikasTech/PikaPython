#include <stdint.h>
#include "BaseObj.h"
#include "STM32F4_common.h"
#include "dataStrs.h"

#ifdef TIM1_EXIST
TIM_HandleTypeDef pika_tim1;
#endif
#ifdef TIM2_EXIST
TIM_HandleTypeDef pika_tim2;
#endif
#ifdef TIM3_EXIST
TIM_HandleTypeDef pika_tim3;
#endif
#ifdef TIM4_EXIST
TIM_HandleTypeDef pika_tim4;
#endif
#ifdef TIM14_EXIST
TIM_HandleTypeDef pika_tim14;
#endif
#ifdef TIM16_EXIST
TIM_HandleTypeDef pika_tim16;
#endif
#ifdef TIM17_EXIST
TIM_HandleTypeDef pika_tim17;
#endif

static TIM_HandleTypeDef* getTimHandle(char* pin) {
#ifdef TIM1_EXIST
    if (strEqu("PA8", pin) || strEqu("PA9", pin) || strEqu("PA10", pin) ||
        strEqu("PA11", pin)) {
        return &pika_tim1;
    }
#endif
#ifdef TIM2_EXIST
    if (strEqu("PA0", pin) || strEqu("PA1", pin) || strEqu("PA2", pin) ||
        strEqu("PA3", pin)) {
        return &pika_tim2;
    }
#endif
#ifdef TIM3_EXIST
    if (strEqu("PA6", pin) || strEqu("PA7", pin) || strEqu("PB0", pin) ||
        strEqu("PB1", pin)) {
        return &pika_tim3;
    }
#endif
#ifdef TIM4_EXIST
    if (strEqu("PB6", pin) || strEqu("PB7", pin) || strEqu("PB8", pin) ||
        strEqu("PB9", pin)) {
        return &pika_tim3;
    }
#endif
#ifdef TIM14_EXIST
    if (strEqu("PA4", pin)) {
        return &pika_tim14;
    }
#endif
#ifdef TIM16_EXIST
    if (strEqu("PD0", pin)) {
        return &pika_tim16;
    }
#endif
#ifdef TIM17_EXIST
    if (strEqu("PD1", pin)) {
        return &pika_tim17;
    }
#endif
    return NULL;
}

static TIM_TypeDef* getTimInstance(char* pin) {
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
#ifdef TIM17_EXIST_EXIST
    if (strEqu("PD1", pin)) {
        return TIM17;
    }
#endif
    return NULL;
}

static void PWM_TimClockEnable(TIM_TypeDef* timInstance) {
#ifdef TIM1_EXIST
    if (TIM1 == timInstance) {
        __HAL_RCC_TIM1_CLK_ENABLE();
        return;
    }
#endif
#ifdef TIM2_EXIST
    if (TIM2 == timInstance) {
        __HAL_RCC_TIM2_CLK_ENABLE();
        return;
    }
#endif
#ifdef TIM3_EXIST
    if (TIM3 == timInstance) {
        __HAL_RCC_TIM3_CLK_ENABLE();
        return;
    }
#endif
#ifdef TIM4_EXIST
    if (TIM4 == timInstance) {
        __HAL_RCC_TIM4_CLK_ENABLE();
        return;
    }
#endif
#ifdef TIM14_EXIST
    if (TIM14 == timInstance) {
        __HAL_RCC_TIM14_CLK_ENABLE();
        return;
    }
#endif
#ifdef TIM16_EXIST
    if (TIM16 == timInstance) {
        __HAL_RCC_TIM16_CLK_ENABLE();
        return;
    }
#endif
#ifdef TIM17_EXIST
    if (TIM17 == timInstance) {
        __HAL_RCC_TIM17_CLK_ENABLE();
        return;
    }
#endif
}

uint8_t PWM_MspInit(char* pin) {
    TIM_TypeDef* timInstance = getTimInstance(pin);
    if (NULL == timInstance) {
        /* this Pin do not match any PWM generator */
        return 1;
    }
    enableClk(pin);
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = getGpioPin(pin);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(getGpioPort(pin), &GPIO_InitStruct);
    PWM_TimClockEnable(timInstance);
    return 0;
}

uint32_t getTimChennel(char* pin) {
    if (strEqu("PA8", pin) || strEqu("PA0", pin) || strEqu("PA6", pin) ||
        strEqu("PB6", pin) || strEqu("PA4", pin) || strEqu("PD0", pin) ||
        strEqu("PD1", pin)) {
        return TIM_CHANNEL_1;
    }
    if (strEqu("PA9", pin) || strEqu("PA1", pin) || strEqu("PB7", pin) ||
        strEqu("PA7", pin)) {
        return TIM_CHANNEL_2;
    }
    if (strEqu("PA10", pin) || strEqu("PA2", pin) || strEqu("PB8", pin) ||
        strEqu("PB0", pin)) {
        return TIM_CHANNEL_3;
    }
    if (strEqu("PA11", pin) || strEqu("PA3", pin) || strEqu("PB9", pin) ||
        strEqu("PB1", pin)) {
        return TIM_CHANNEL_4;
    }
    /* Chennel not match */
    return 99999;
}

void STM32F4_PWM_platformEnable(PikaObj* self) {
    float duty = obj_getFloat(self, "duty");
    int freq = obj_getInt(self, "freq");
    char *pin = obj_getStr(self, "pin");
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    if (0 != PWM_MspInit(pin)) {
        obj_setSysOut(self, "[error]: init PWM port faild.");
        obj_setErrorCode(self, 1);
        return;
    }

    TIM_HandleTypeDef* pika_tim = getTimHandle(pin);
    if (NULL == pika_tim) {
        obj_setSysOut(self, "[error]: can not found PWM hardware.");
        obj_setErrorCode(self, 1);
        return;
    }

    pika_tim->Instance = getTimInstance(pin);
    pika_tim->Init.Prescaler = 72 - 1;
    pika_tim->Init.CounterMode = TIM_COUNTERMODE_UP;
    /* calculate period */
    pika_tim->Init.Period = (uint32_t)((float)(1000 * 1000) / (float)freq) - 1;
    pika_tim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    pika_tim->Init.RepetitionCounter = 0;
    pika_tim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_Base_Init(pika_tim) != HAL_OK) {
        obj_setSysOut(self, "[error]: init PWM faild.");
        obj_setErrorCode(self, 1);
        return;
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(pika_tim, &sClockSourceConfig) != HAL_OK) {
        obj_setSysOut(self, "[error]: init PWM faild.");
        obj_setErrorCode(self, 1);
        return;
    }
    if (HAL_TIM_PWM_Init(pika_tim) != HAL_OK) {
        obj_setSysOut(self, "[error]: init PWM faild.");
        obj_setErrorCode(self, 1);
        return;
    }
    if (HAL_TIM_OC_Init(pika_tim) != HAL_OK) {
        obj_setSysOut(self, "[error]: init PWM faild.");
        obj_setErrorCode(self, 1);
        return;
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(pika_tim, &sMasterConfig) !=
        HAL_OK) {
        obj_setSysOut(self, "[error]: init PWM faild.");
        obj_setErrorCode(self, 1);
        return;
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    /* calculate pulse by duty and freq */
    sConfigOC.Pulse = (uint32_t)(pika_tim->Init.Period * duty);
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(pika_tim, &sConfigOC, getTimChennel(pin)) !=
        HAL_OK) {
        obj_setSysOut(self, "[error]: init PWM faild.");
        obj_setErrorCode(self, 1);
        return;
    }

    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    if (HAL_TIMEx_ConfigBreakDeadTime(pika_tim, &sBreakDeadTimeConfig) !=
        HAL_OK) {
        obj_setSysOut(self, "[error]: init PWM faild.");
        obj_setErrorCode(self, 1);
        return;
    }
    HAL_TIM_PWM_Start(pika_tim, getTimChennel(pin));
}

void STM32F4_PWM_platformSetDuty(PikaObj* self) {
    float duty = obj_getFloat(self, "duty");
    char *pin = obj_getStr(self, "pin");
    TIM_HandleTypeDef* pika_tim = getTimHandle(pin);
    if (NULL == pika_tim) {
        obj_setSysOut(self, "[error]: can not found PWM hardware.");
        obj_setErrorCode(self, 1);
        return;
    }
    /* update duty in run time */
    if (NULL != pika_tim->Instance) {
        __HAL_TIM_SET_COMPARE(pika_tim, getTimChennel(pin),
                              (uint32_t)(pika_tim->Init.Period * duty));
    }
}

void STM32F4_PWM_platformSetFrequency(PikaObj* self) {
    int freq = obj_getInt(self, "freq");
    char *pin = obj_getStr(self, "pin");
    TIM_HandleTypeDef* pika_tim = getTimHandle(pin);
    if (NULL == pika_tim) {
        obj_setSysOut(self, "[error]: can not found PWM hardware.");
        obj_setErrorCode(self, 1);
        return;
    }
    /* update frequency in run time */
    if (NULL != pika_tim->Instance) {
        __HAL_TIM_SET_AUTORELOAD(
            pika_tim, (uint32_t)((float)(1000 * 1000) / (float)freq) - 1);
        float duty = obj_getFloat(self, "duty");
        __HAL_TIM_SET_COMPARE(pika_tim, getTimChennel(pin),
                              (uint32_t)(pika_tim->Init.Period * duty));
    }
}
