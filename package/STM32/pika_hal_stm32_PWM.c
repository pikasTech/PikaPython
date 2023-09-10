#include "pika_hal_stm32_common.h"

/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-13     zylx         first version
 * 2021-01-23     thread-liu   Fix the timer clock frequency doubling problem
 * 2023-9-10      lyon         port to pika_hal
 */

#ifndef PIKA_HAL
#include <board.h>
#endif

#ifdef BSP_USING_PWM
#ifdef PIKA_HAL
#include "pika_drv_config.h"
#else
#include "drv_config.h"
#endif

#if !defined(PWM1_CONFIG)
#undef BSP_USING_PWM1
#endif

#if !defined(PWM2_CONFIG)
#undef BSP_USING_PWM2
#endif

#if !defined(PWM3_CONFIG)
#undef BSP_USING_PWM3
#endif

#if !defined(PWM4_CONFIG)
#undef BSP_USING_PWM4
#endif

#if !defined(PWM5_CONFIG)
#undef BSP_USING_PWM5
#endif

#if !defined(PWM6_CONFIG)
#undef BSP_USING_PWM6
#endif

#if !defined(PWM7_CONFIG)
#undef BSP_USING_PWM7
#endif

#if !defined(PWM8_CONFIG)
#undef BSP_USING_PWM8
#endif

#if !defined(PWM9_CONFIG)
#undef BSP_USING_PWM9
#endif

#if !defined(PWM10_CONFIG)
#undef BSP_USING_PWM10
#endif

#if !defined(PWM11_CONFIG)
#undef BSP_USING_PWM11
#endif

#if !defined(PWM12_CONFIG)
#undef BSP_USING_PWM12
#endif

#if !defined(PWM13_CONFIG)
#undef BSP_USING_PWM13
#endif

#if !defined(PWM14_CONFIG)
#undef BSP_USING_PWM14
#endif

#if !defined(PWM15_CONFIG)
#undef BSP_USING_PWM15
#endif

#if !defined(PWM16_CONFIG)
#undef BSP_USING_PWM16
#endif

#if !defined(PWM17_CONFIG)
#undef BSP_USING_PWM17
#endif

#ifndef PIKA_HAL
#include "drv_tim.h"
#include <drivers/rt_drv_pwm.h>

//#define DRV_DEBUG
#define LOG_TAG             "drv.pwm"
#include <drv_log.h>
#endif

#define MAX_PERIOD 65535
#define MIN_PERIOD 3
#define MIN_PULSE 2

enum
{
#ifdef BSP_USING_PWM1
    PWM1_INDEX,
#endif
#ifdef BSP_USING_PWM2
    PWM2_INDEX,
#endif
#ifdef BSP_USING_PWM3
    PWM3_INDEX,
#endif
#ifdef BSP_USING_PWM4
    PWM4_INDEX,
#endif
#ifdef BSP_USING_PWM5
    PWM5_INDEX,
#endif
#ifdef BSP_USING_PWM6
    PWM6_INDEX,
#endif
#ifdef BSP_USING_PWM7
    PWM7_INDEX,
#endif
#ifdef BSP_USING_PWM8
    PWM8_INDEX,
#endif
#ifdef BSP_USING_PWM9
    PWM9_INDEX,
#endif
#ifdef BSP_USING_PWM10
    PWM10_INDEX,
#endif
#ifdef BSP_USING_PWM11
    PWM11_INDEX,
#endif
#ifdef BSP_USING_PWM12
    PWM12_INDEX,
#endif
#ifdef BSP_USING_PWM13
    PWM13_INDEX,
#endif
#ifdef BSP_USING_PWM14
    PWM14_INDEX,
#endif
#ifdef BSP_USING_PWM15
    PWM15_INDEX,
#endif
#ifdef BSP_USING_PWM16
    PWM16_INDEX,
#endif
#ifdef BSP_USING_PWM17
    PWM17_INDEX,
#endif
};

struct rt_device_pwm
{
    struct rt_device parent;
    const struct rt_pwm_ops *ops;
};

struct stm32_pwm
{
    struct rt_device_pwm pwm_device;
    TIM_HandleTypeDef    tim_handle;
    rt_uint8_t channel;
    char *name;
    rt_base_t pin;
};

static struct stm32_pwm stm32_pwm_obj[] =
{
#ifdef BSP_USING_PWM1
    PWM1_CONFIG,
#endif

#ifdef BSP_USING_PWM2
    PWM2_CONFIG,
#endif

#ifdef BSP_USING_PWM3
    PWM3_CONFIG,
#endif

#ifdef BSP_USING_PWM4
    PWM4_CONFIG,
#endif

#ifdef BSP_USING_PWM5
    PWM5_CONFIG,
#endif

#ifdef BSP_USING_PWM6
    PWM6_CONFIG,
#endif

#ifdef BSP_USING_PWM7
    PWM7_CONFIG,
#endif

#ifdef BSP_USING_PWM8
    PWM8_CONFIG,
#endif

#ifdef BSP_USING_PWM9
    PWM9_CONFIG,
#endif

#ifdef BSP_USING_PWM10
    PWM10_CONFIG,
#endif

#ifdef BSP_USING_PWM11
    PWM11_CONFIG,
#endif

#ifdef BSP_USING_PWM12
    PWM12_CONFIG,
#endif

#ifdef BSP_USING_PWM13
    PWM13_CONFIG,
#endif

#ifdef BSP_USING_PWM14
    PWM14_CONFIG,
#endif

#ifdef BSP_USING_PWM15
    PWM15_CONFIG,
#endif

#ifdef BSP_USING_PWM16
    PWM16_CONFIG,
#endif

#ifdef BSP_USING_PWM17
    PWM17_CONFIG,
#endif
};

/* APBx timer clocks frequency doubler state related to APB1CLKDivider value */
static void stm32_tim_pclkx_doubler_get(rt_uint32_t *pclk1_doubler, rt_uint32_t *pclk2_doubler)
{
    rt_uint32_t flatency = 0;
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RT_ASSERT(pclk1_doubler != RT_NULL);
    RT_ASSERT(pclk1_doubler != RT_NULL);

    HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &flatency);

    *pclk1_doubler = 1;
    *pclk2_doubler = 1;

#if defined(SOC_SERIES_STM32MP1)
    if (RCC_ClkInitStruct.APB1_Div != RCC_APB1_DIV1)
    {
        *pclk1_doubler = 2;
    }
    if (RCC_ClkInitStruct.APB2_Div != RCC_APB2_DIV1)
    {
        *pclk2_doubler = 2;
    }
#else
    if (RCC_ClkInitStruct.APB1CLKDivider != RCC_HCLK_DIV1)
    {
         *pclk1_doubler = 2;
    }
#if !(defined(SOC_SERIES_STM32F0) || defined(SOC_SERIES_STM32G0))
    if (RCC_ClkInitStruct.APB2CLKDivider != RCC_HCLK_DIV1)
    {
         *pclk2_doubler = 2;
    }
#endif /* !(defined(SOC_SERIES_STM32F0) || defined(SOC_SERIES_STM32G0)) */
#endif /* defined(SOC_SERIES_STM32MP1) */
}

static void stm32_tim_enable_clock(TIM_HandleTypeDef* htim_base)
{
    RT_ASSERT(htim_base != RT_NULL);

    if(RT_FALSE);
#ifdef TIM1
    else if(htim_base->Instance==TIM1)
    {
        __HAL_RCC_TIM1_CLK_ENABLE();
    }
#endif /* TIM1 */
#ifdef TIM2
    else if(htim_base->Instance==TIM2)
    {
        __HAL_RCC_TIM2_CLK_ENABLE();
    }
#endif /* TIM2 */
#ifdef TIM3
    else if(htim_base->Instance==TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
    }
#endif /* TIM3 */
#ifdef TIM4
    else if(htim_base->Instance==TIM4)
    {
        __HAL_RCC_TIM4_CLK_ENABLE();
    }
#endif /* TIM4 */
#ifdef TIM5
    else if(htim_base->Instance==TIM5)
    {
        __HAL_RCC_TIM5_CLK_ENABLE();
    }
#endif /* TIM5 */
#ifdef TIM6
    else if(htim_base->Instance==TIM6)
    {
        __HAL_RCC_TIM6_CLK_ENABLE();
    }
#endif /* TIM6 */
#ifdef TIM7
    else if(htim_base->Instance==TIM7)
    {
        __HAL_RCC_TIM7_CLK_ENABLE();
    }
#endif /* TIM7 */
#ifdef TIM8
    else if(htim_base->Instance==TIM8)
    {
        __HAL_RCC_TIM8_CLK_ENABLE();
    }
#endif /* TIM8 */
#ifdef TIM9
    else if(htim_base->Instance==TIM9)
    {
        __HAL_RCC_TIM9_CLK_ENABLE();
    }
#endif /* TIM9 */
#ifdef TIM10
    else if(htim_base->Instance==TIM10)
    {
        __HAL_RCC_TIM10_CLK_ENABLE();
    }
#endif /* TIM10 */
#ifdef TIM11
    else if(htim_base->Instance==TIM11)
    {
        __HAL_RCC_TIM11_CLK_ENABLE();
    }
#endif /* TIM11 */
#ifdef TIM12
    else if(htim_base->Instance==TIM12)
    {
        __HAL_RCC_TIM12_CLK_ENABLE();
    }
#endif /* TIM12 */
#ifdef TIM13
    else if(htim_base->Instance==TIM13)
    {
        __HAL_RCC_TIM13_CLK_ENABLE();
    }
#endif /* TIM13 */
#ifdef TIM14
    else if(htim_base->Instance==TIM14)
    {
        __HAL_RCC_TIM14_CLK_ENABLE();
    }
#endif /* TIM14 */
#ifdef TIM15
    else if(htim_base->Instance==TIM15)
    {
        __HAL_RCC_TIM15_CLK_ENABLE();
    }
#endif /* TIM15 */
#ifdef TIM16
    else if(htim_base->Instance==TIM16)
    {
        __HAL_RCC_TIM16_CLK_ENABLE();
    }
#endif /* TIM16 */
#ifdef TIM17
    else if(htim_base->Instance==TIM17)
    {
        __HAL_RCC_TIM17_CLK_ENABLE();
    }
#endif /* TIM17 */
#ifdef TIM18
    else if(htim_base->Instance==TIM18)
    {
        __HAL_RCC_TIM18_CLK_ENABLE();
    }
#endif /* TIM18 */
#ifdef TIM19
    else if(htim_base->Instance==TIM19)
    {
        __HAL_RCC_TIM19_CLK_ENABLE();
    }
#endif /* TIM19 */
    else
    {
        RT_ASSERT(RT_TRUE);
    }
}

static rt_uint64_t tim_clock_get(TIM_HandleTypeDef *htim)
{
    rt_uint32_t pclk1_doubler, pclk2_doubler;
    rt_uint64_t tim_clock;

    stm32_tim_pclkx_doubler_get(&pclk1_doubler, &pclk2_doubler);

/* Some series may only have APBPERIPH_BASE, don't have HAL_RCC_GetPCLK2Freq */
#if defined(APBPERIPH_BASE)
    tim_clock = (rt_uint32_t)(HAL_RCC_GetPCLK1Freq() * pclk1_doubler);
#elif defined(APB1PERIPH_BASE) || defined(APB2PERIPH_BASE)
    if ((rt_uint32_t)htim->Instance >= APB2PERIPH_BASE)
    {
        tim_clock = (rt_uint32_t)(HAL_RCC_GetPCLK2Freq() * pclk2_doubler);
    }
    else
    {
        tim_clock = (rt_uint32_t)(HAL_RCC_GetPCLK1Freq() * pclk1_doubler);
    }
#endif

    return tim_clock;
}

static rt_err_t drv_pwm_control(struct rt_device_pwm *device, int cmd, void *arg);

struct rt_pwm_ops
{
    rt_err_t (*control)(struct rt_device_pwm *device, int cmd, void *arg);
};

static struct rt_pwm_ops drv_ops =
{
    drv_pwm_control
};

struct rt_pwm_configuration
{
    rt_uint32_t channel; /* 0 ~ n or 0 ~ -n, which depends on specific MCU requirements */
    rt_uint32_t period;  /* unit:ns 1ns~4.29s:1Ghz~0.23hz */
    rt_uint32_t pulse;   /* unit:ns (pulse<=period) */
    rt_uint32_t dead_time;  /* unit:ns */
    rt_uint32_t phase;  /*unit: degree, 0~360, which is the phase of pwm output, */
    /*
     * RT_TRUE  : The channel of pwm is complememtary.
     * RT_FALSE : The channel of pwm is nomal.
    */
    rt_bool_t  complementary;
};

static rt_err_t drv_pwm_enable(TIM_HandleTypeDef *htim, struct rt_pwm_configuration *configuration, rt_bool_t enable)
{
    /* Converts the channel number to the channel number of Hal library */
    rt_uint32_t channel = 0x04 * (configuration->channel - 1);

    if (!configuration->complementary)
    {
        if (!enable)
        {
            HAL_TIM_PWM_Stop(htim, channel);
        }
        else
        {
            HAL_TIM_PWM_Start(htim, channel);
        }
    }
    else if (configuration->complementary)
    {
        if (!enable)
        {
            HAL_TIMEx_PWMN_Stop(htim, channel);
        }
        else
        {
            HAL_TIMEx_PWMN_Start(htim, channel);
        }
    }

    return RT_EOK;
}

static rt_err_t drv_pwm_get(TIM_HandleTypeDef *htim, struct rt_pwm_configuration *configuration)
{
    /* Converts the channel number to the channel number of Hal library */
    rt_uint32_t channel = 0x04 * (configuration->channel - 1);
    rt_uint64_t tim_clock;

    tim_clock = tim_clock_get(htim);
    if (__HAL_TIM_GET_CLOCKDIVISION(htim) == TIM_CLOCKDIVISION_DIV2)
    {
        tim_clock = tim_clock / 2;
    }
    else if (__HAL_TIM_GET_CLOCKDIVISION(htim) == TIM_CLOCKDIVISION_DIV4)
    {
        tim_clock = tim_clock / 4;
    }

    /* Convert nanosecond to frequency and duty cycle. 1s = 1 * 1000 * 1000 * 1000 ns */
    tim_clock /= 1000000UL;
    configuration->period = (__HAL_TIM_GET_AUTORELOAD(htim) + 1) * (htim->Instance->PSC + 1) * 1000UL / tim_clock;
    configuration->pulse = (__HAL_TIM_GET_COMPARE(htim, channel) + 1) * (htim->Instance->PSC + 1) * 1000UL / tim_clock;

    return RT_EOK;
}

static rt_err_t drv_pwm_set(TIM_HandleTypeDef *htim, struct rt_pwm_configuration *configuration)
{
    rt_uint32_t period, pulse;
    rt_uint64_t tim_clock, psc;
    /* Converts the channel number to the channel number of Hal library */
    rt_uint32_t channel = 0x04 * (configuration->channel - 1);

    tim_clock = tim_clock_get(htim);
    /* Convert nanosecond to frequency and duty cycle. 1s = 1 * 1000 * 1000 * 1000 ns */
    tim_clock /= 1000000UL;
    period = (rt_uint64_t)configuration->period * tim_clock / 1000ULL ;
    psc = period / MAX_PERIOD + 1;
    period = period / psc;
    __HAL_TIM_SET_PRESCALER(htim, psc - 1);

    if (period < MIN_PERIOD)
    {
        period = MIN_PERIOD;
    }
    __HAL_TIM_SET_AUTORELOAD(htim, period - 1);

    pulse = (rt_uint64_t)configuration->pulse * tim_clock / psc / 1000ULL;
    if (pulse < MIN_PULSE)
    {
        pulse = MIN_PULSE;
    }
    /*To determine user input, output high level is required*/
    else if (pulse >= period)
    {
        pulse = period + 1;
    }
    __HAL_TIM_SET_COMPARE(htim, channel, pulse - 1);

    /* If you want the PWM setting to take effect immediately,
    please uncommon the following code, but it will cause the last PWM cycle not complete. */
    //__HAL_TIM_SET_COUNTER(htim, 0);

    //HAL_TIM_GenerateEvent(htim, TIM_EVENTSOURCE_UPDATE); /* Update frequency value */

    return RT_EOK;
}

static rt_err_t drv_pwm_set_period(TIM_HandleTypeDef *htim, struct rt_pwm_configuration *configuration)
{
    rt_uint32_t period;
    rt_uint64_t tim_clock, psc;

    tim_clock = tim_clock_get(htim);
    /* Convert nanosecond to frequency and duty cycle. 1s = 1 * 1000 * 1000 * 1000 ns */
    tim_clock /= 1000000UL;
    period = (rt_uint64_t)configuration->period * tim_clock / 1000ULL ;
    psc = period / MAX_PERIOD + 1;
    period = period / psc;
    __HAL_TIM_SET_PRESCALER(htim, psc - 1);

    if (period < MIN_PERIOD)
    {
        period = MIN_PERIOD;
    }
    __HAL_TIM_SET_AUTORELOAD(htim, period - 1);

    return RT_EOK;
}

static rt_err_t drv_pwm_set_pulse(TIM_HandleTypeDef *htim, struct rt_pwm_configuration *configuration)
{
    rt_uint32_t period, pulse;
    rt_uint64_t tim_clock;
    /* Converts the channel number to the channel number of Hal library */
    rt_uint32_t channel = 0x04 * (configuration->channel - 1);

    tim_clock = tim_clock_get(htim);
    /* Convert nanosecond to frequency and duty cycle. 1s = 1 * 1000 * 1000 * 1000 ns */
    tim_clock /= 1000000UL;

    period = (__HAL_TIM_GET_AUTORELOAD(htim) + 1) * (htim->Instance->PSC + 1) * 1000UL / tim_clock;
    pulse = (rt_uint64_t)configuration->pulse * (__HAL_TIM_GET_AUTORELOAD(htim) + 1) / period;

    if (pulse < MIN_PULSE)
    {
        pulse = MIN_PULSE;
    }
    else if (pulse > period)
    {
        pulse = period;
    }
    __HAL_TIM_SET_COMPARE(htim, channel, pulse - 1);

    return RT_EOK;
}

#ifdef PIKA_HAL
#define RT_DEVICE_CTRL_BASE(_) 0
#endif

#define PWM_CMD_ENABLE      (RT_DEVICE_CTRL_BASE(PWM) + 0)
#define PWM_CMD_DISABLE     (RT_DEVICE_CTRL_BASE(PWM) + 1)
#define PWM_CMD_SET         (RT_DEVICE_CTRL_BASE(PWM) + 2)
#define PWM_CMD_GET         (RT_DEVICE_CTRL_BASE(PWM) + 3)
#define PWMN_CMD_ENABLE     (RT_DEVICE_CTRL_BASE(PWM) + 4)
#define PWMN_CMD_DISABLE    (RT_DEVICE_CTRL_BASE(PWM) + 5)
#define PWM_CMD_SET_PERIOD  (RT_DEVICE_CTRL_BASE(PWM) + 6)
#define PWM_CMD_SET_PULSE   (RT_DEVICE_CTRL_BASE(PWM) + 7)
#define PWM_CMD_SET_DEAD_TIME  (RT_DEVICE_CTRL_BASE(PWM) + 8)
#define PWM_CMD_SET_PHASE   (RT_DEVICE_CTRL_BASE(PWM) + 9)
#define PWM_CMD_ENABLE_IRQ  (RT_DEVICE_CTRL_BASE(PWM) + 10)
#define PWM_CMD_DISABLE_IRQ  (RT_DEVICE_CTRL_BASE(PWM) + 11)

static rt_err_t drv_pwm_control(struct rt_device_pwm *device, int cmd, void *arg)
{
    struct rt_pwm_configuration *configuration = (struct rt_pwm_configuration *)arg;
    TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)device->parent.user_data;

    switch (cmd)
    {
    case PWM_CMD_ENABLE:
        return drv_pwm_enable(htim, configuration, RT_TRUE);
    case PWM_CMD_DISABLE:
        return drv_pwm_enable(htim, configuration, RT_FALSE);
    case PWM_CMD_SET:
        return drv_pwm_set(htim, configuration);
    case PWM_CMD_SET_PERIOD:
        return drv_pwm_set_period(htim, configuration);
    case PWM_CMD_SET_PULSE:
        return drv_pwm_set_pulse(htim, configuration);
    case PWM_CMD_GET:
        return drv_pwm_get(htim, configuration);
    default:
        return -RT_EINVAL;
    }
}

#if defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32L4)
static uint32_t get_pwm_pin_alternate(rt_base_t pin){
#ifdef BSP_USING_PWM1
    switch((uintptr_t)PIN_STPORT(pin)){
        case (uintptr_t)GPIOA:
            switch(PIN_NO(pin)){
                case 8:
                case 9:
                case 10:
                case 11:
                    return GPIO_AF1_TIM1;
            }
            break;
    }
#endif

#ifdef BSP_USING_PWM2
    switch((uintptr_t)PIN_STPORT(pin)){
        case (uintptr_t)GPIOA:
            switch(PIN_NO(pin)){
                case 0:
                case 1:
                case 2:
                case 3:
                    return GPIO_AF1_TIM2;
            }
            break;
    }
#endif

#ifdef BSP_USING_PWM3
    switch((uintptr_t)PIN_STPORT(pin)){
        case (uintptr_t)GPIOA:
            switch(PIN_NO(pin)){
                case 6:
                case 7:
                    return GPIO_AF2_TIM3;
            }
            break;
        case (uintptr_t)GPIOB:
            switch(PIN_NO(pin)){
                case 0:
                case 1:
                    return GPIO_AF2_TIM3;
            }
            break;
    }
#endif

#ifdef BSP_USING_PWM4
    switch((uintptr_t)PIN_STPORT(pin)){
        case (uintptr_t)GPIOB:
            switch(PIN_NO(pin)){
                case 6:
                case 7:
                case 8:
                case 9:
                    return GPIO_AF2_TIM4;
            }
            break;
    }
#endif
    return -1;  // Returning -1 for error cases as NULL for int return type doesn't make sense
}
#endif

static void pwm_pin_init(struct stm32_pwm* device){
    mp_hal_gpio_clock_enable(PIN_STPORT(device->pin));
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = PIN_STPIN(device->pin);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
#if defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32L4)
    GPIO_InitStruct.Alternate = get_pwm_pin_alternate(device->pin);
#endif
    HAL_GPIO_Init(PIN_STPORT(device->pin), &GPIO_InitStruct);
}

static rt_err_t stm32_hw_pwm_init(struct stm32_pwm *device)
{
    rt_err_t result = RT_EOK;
    TIM_HandleTypeDef *tim = RT_NULL;
    TIM_MasterConfigTypeDef master_config = {0};
    TIM_ClockConfigTypeDef clock_config = {0};

    RT_ASSERT(device != RT_NULL);

    tim = (TIM_HandleTypeDef *)&device->tim_handle;

    /* configure the timer to pwm mode */
    tim->Init.Prescaler = 0;
    tim->Init.CounterMode = TIM_COUNTERMODE_UP;
    tim->Init.Period = 0;
    tim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
#if defined(SOC_SERIES_STM32F1) || defined(SOC_SERIES_STM32L4)
    tim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
#endif
    if (HAL_TIM_Base_Init(tim) != HAL_OK)
    {
        LOG_E("%s pwm init failed", device->name);
        result = -RT_ERROR;
        goto __exit;
    }

    stm32_tim_enable_clock(tim);

    clock_config.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(tim, &clock_config) != HAL_OK)
    {
        LOG_E("%s clock init failed", device->name);
        result = -RT_ERROR;
        goto __exit;
    }

    if (HAL_TIM_PWM_Init(tim) != HAL_OK)
    {
        LOG_E("%s pwm init failed", device->name);
        result = -RT_ERROR;
        goto __exit;
    }

    if(IS_TIM_MASTER_INSTANCE(tim->Instance))
    {
        master_config.MasterOutputTrigger = TIM_TRGO_RESET;
        master_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(tim, &master_config) != HAL_OK)
        {
            LOG_E("%s master config failed", device->name);
            result = -RT_ERROR;
            goto __exit;
        }
    }

    TIM_OC_InitTypeDef oc_config = {0};
    oc_config.OCMode = TIM_OCMODE_PWM1;
    oc_config.Pulse = 0;
    oc_config.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc_config.OCFastMode = TIM_OCFAST_DISABLE;
    oc_config.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    oc_config.OCIdleState  = TIM_OCIDLESTATE_RESET;

    /* config pwm channel */
    if (device->channel & 0x01)
    {
        if (HAL_TIM_PWM_ConfigChannel(tim, &oc_config, TIM_CHANNEL_1) != HAL_OK)
        {
            LOG_E("%s channel1 config failed", device->name);
            result = -RT_ERROR;
            goto __exit;
        }
    }

    if (device->channel & 0x02)
    {
        if (HAL_TIM_PWM_ConfigChannel(tim, &oc_config, TIM_CHANNEL_2) != HAL_OK)
        {
            LOG_E("%s channel2 config failed", device->name);
            result = -RT_ERROR;
            goto __exit;
        }
    }

    if (device->channel & 0x04)
    {
        if (HAL_TIM_PWM_ConfigChannel(tim, &oc_config, TIM_CHANNEL_3) != HAL_OK)
        {
            LOG_E("%s channel3 config failed", device->name);
            result = -RT_ERROR;
            goto __exit;
        }
    }

    if (device->channel & 0x08)
    {
        if (HAL_TIM_PWM_ConfigChannel(tim, &oc_config, TIM_CHANNEL_4) != HAL_OK)
        {
            LOG_E("%s channel4 config failed", device->name);
            result = -RT_ERROR;
            goto __exit;
        }
    }

    /* pwm pin configuration */
    pwm_pin_init(device);

    /* enable update request source */
    __HAL_TIM_URS_ENABLE(tim);

__exit:
    return result;
}

#define PWM_CH1 0
#define PWM_CH2 1
#define PWM_CH3 2
#define PWM_CH4 3

int get_pwm_channel(rt_base_t pin) {
    switch ((uintptr_t)PIN_STPORT(pin)) {
        case (uintptr_t)GPIOA:
            switch (PIN_NO(pin)) {
                case 8:
                case 0:
                case 6:
                case 4:
                    return PWM_CH1;

                case 9:
                case 1:
                case 7:
                    return PWM_CH2;

                case 10:
                case 2:
                    return PWM_CH3;

                case 11:
                case 3:
                    return PWM_CH4;
            }
            break;

        case (uintptr_t)GPIOB:
            switch (PIN_NO(pin)) {
                case 6:
                    return PWM_CH1;

                case 7:
                    return PWM_CH2;

                case 8:
                    return PWM_CH3;

                case 9:
                case 0:
                case 1:
                    return PWM_CH4;
            }
            break;

        case (uintptr_t)GPIOD:
            switch (PIN_NO(pin)) {
                case 0:
                    return PWM_CH1;

                case 1:
                    return PWM_CH2;
            }
            break;
    }
    /* Channel not match */
    return -1;
}

static int get_pwm_index(rt_base_t pin) {

#ifdef BSP_USING_PWM1
    switch((uintptr_t)PIN_STPORT(pin)){
        case (uintptr_t)GPIOA:
            switch(PIN_NO(pin)){
                case 8:
                case 9:
                case 10:
                case 11:
                    return PWM1_INDEX;
            }
            break;
    }
#endif

#ifdef BSP_USING_PWM2
    switch((uintptr_t)PIN_STPORT(pin)){
        case (uintptr_t)GPIOA:
            switch(PIN_NO(pin)){
                case 0:
                case 1:
                case 2:
                case 3:
                    return PWM2_INDEX;
            }
            break;
    }
#endif

#ifdef BSP_USING_PWM3
    switch((uintptr_t)PIN_STPORT(pin)){
        case (uintptr_t)GPIOA:
            switch(PIN_NO(pin)){
                case 6:
                case 7:
                    return PWM3_INDEX;
            }
            break;
        case (uintptr_t)GPIOB:
            switch(PIN_NO(pin)){
                case 0:
                case 1:
                    return PWM3_INDEX;
            }
            break;
    }
#endif

#ifdef BSP_USING_PWM4
    switch((uintptr_t)PIN_STPORT(pin)){
        case (uintptr_t)GPIOB:
            switch(PIN_NO(pin)){
                case 6:
                case 7:
                case 8:
                case 9:
                    return PWM4_INDEX;
            }
            break;
    }
#endif
    return -1;  // Returning -1 for error cases as NULL for int return type doesn't make sense
}

static int stm32_pwm_set_channel(rt_base_t pin)
{
    int channel = get_pwm_channel(pin);
    int PWMX_INDEX = get_pwm_index(pin);

    if (PWMX_INDEX != -1 && channel != -1)
    {
        stm32_pwm_obj[PWMX_INDEX].channel |= 1 << channel;
        return 0;
    }
    return -1; // Unknown pin number or PWM not defined
}

static rt_err_t rt_device_pwm_register(struct rt_device_pwm *device, const char *name, const struct rt_pwm_ops *ops, const void *user_data)
{
    rt_err_t result = RT_EOK;

    rt_memset(device, 0, sizeof(struct rt_device_pwm));

#ifndef PIKA_HAL
#ifdef RT_USING_DEVICE_OPS
    device->parent.ops = &pwm_device_ops;
#else
    device->parent.init = RT_NULL;
    device->parent.open = RT_NULL;
    device->parent.close = RT_NULL;
    device->parent.read  = _pwm_read;
    device->parent.write = _pwm_write;
    device->parent.control = _pwm_control;
#endif /* RT_USING_DEVICE_OPS */
#endif

    device->parent.type         = RT_Device_Class_PWM;
    device->ops                 = ops;
    device->parent.user_data    = (void *)user_data;

#ifndef PIKA_HAL
    result = rt_device_register(&device->parent, name, RT_DEVICE_FLAG_RDWR);
#endif
    return result;
}

static volatile int stm32_pwm_inited = 0;
static int stm32_pwm_init(void)
{
    int i = 0;
    int result = RT_EOK;

    for (i = 0; i < sizeof(stm32_pwm_obj) / sizeof(stm32_pwm_obj[0]); i++)
    {
        LOG_D("%s init success", stm32_pwm_obj[i].name);

        /* register pwm device */
        if (rt_device_pwm_register(&stm32_pwm_obj[i].pwm_device, stm32_pwm_obj[i].name, &drv_ops, &stm32_pwm_obj[i].tim_handle) == RT_EOK)
        {
            LOG_D("%s register success", stm32_pwm_obj[i].name);
        }
        else
        {
            LOG_E("%s register failed", stm32_pwm_obj[i].name);
            result = -RT_ERROR;
        }
//        /* pwm init */
//        if (stm32_hw_pwm_init(&stm32_pwm_obj[i]) != RT_EOK)
//        {
//            LOG_E("%s init failed", stm32_pwm_obj[i].name);
//            result = -RT_ERROR;
//            goto __exit;
//        }
    }

__exit:
    return result;
}

#ifndef PIKA_HAL
INIT_DEVICE_EXPORT(stm32_pwm_init);
#endif

typedef struct platform_PWM {
    struct stm32_pwm* pwm_obj;
    struct rt_pwm_configuration pwm_configure;
} platform_PWM;

int pika_hal_platform_PWM_open(pika_dev* dev, char* name) {
    if (!stm32_pwm_inited){
        stm32_pwm_inited = 1;
        stm32_pwm_init();
    }
    rt_base_t pin = _stm32_pin_get(name);
    if(pin < 0){
        pika_platform_printf("PWM Error: Pin %s not supported\n", name);
        return -1;
    }
    int pwm_index = get_pwm_index(pin);
    if(pwm_index < 0){
        pika_platform_printf("PWM Error: Pin %s not supported\n", name);
        return -1;
    }
    int ret = stm32_pwm_set_channel(pin);
    if(ret < 0){
        pika_platform_printf("PWM Error: Pin %s not supported\n", name);
        return -1;
    }
    platform_PWM* pwm = pikaMalloc(sizeof(platform_PWM));
    memset(pwm, 0, sizeof(platform_PWM));
    pwm->pwm_obj = &stm32_pwm_obj[pwm_index];
    pwm->pwm_obj->pin = pin;
    pwm->pwm_configure.channel = get_pwm_channel(pin) + 1;
    dev->platform_data = pwm;
    return 0;
}

int pika_hal_platform_PWM_close(pika_dev* dev) {
    platform_PWM* pwm = (platform_PWM*)dev->platform_data;
    if(NULL != pwm){
        pikaFree(pwm, sizeof(platform_PWM));
        dev->platform_data = NULL;
    }
    return 0;
}

int pika_hal_platform_PWM_read(pika_dev* dev, void* buf, size_t count) {
    return -1;
}

int pika_hal_platform_PWM_write(pika_dev* dev, void* buf, size_t count) {
    return -1;
}

int pika_hal_platform_PWM_ioctl_config(pika_dev* dev,
                                       pika_hal_PWM_config* cfg) {
    platform_PWM* pwm = (platform_PWM*)dev->platform_data;
    pwm->pwm_configure.period = cfg->period;
    pwm->pwm_configure.pulse = cfg->duty;
    if (dev->is_enabled){
        drv_pwm_control(&pwm->pwm_obj->pwm_device, PWM_CMD_SET, &pwm->pwm_configure);
    }
    return 0;
}

int pika_hal_platform_PWM_ioctl_enable(pika_dev* dev) {
    platform_PWM* pwm = (platform_PWM*)dev->platform_data;
    if (!dev->is_enabled){
        stm32_hw_pwm_init(pwm->pwm_obj);
        drv_pwm_control(&pwm->pwm_obj->pwm_device, PWM_CMD_SET, &pwm->pwm_configure);
        drv_pwm_control(&pwm->pwm_obj->pwm_device, PWM_CMD_ENABLE, &pwm->pwm_configure);
    }
    return 0;
}

int pika_hal_platform_PWM_ioctl_disable(pika_dev* dev) {
    platform_PWM* pwm = (platform_PWM*)dev->platform_data;
    drv_pwm_control(&pwm->pwm_obj->pwm_device, PWM_CMD_DISABLE, &pwm->pwm_configure);
    return 0;
}

#endif /* BSP_USING_PWM */
