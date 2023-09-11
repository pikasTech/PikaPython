#include "pika_hal_stm32_common.h"

/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2023-2023, PikaPython Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-05     zylx         first version
 * 2018-12-12     greedyhao    Porting for stm32f7xx
 * 2019-02-01     yuneizhilin  fix the stm32_adc_init function initialization issue
 * 2020-06-17     thread-liu   Porting for stm32mp1xx
 * 2020-10-14     Dozingfiretruck   Porting for stm32wbxx
 * 2022-05-22     Stanley Lwin Add stm32_adc_get_vref
 * 2022-12-26     wdfk-prog    Change the order of configuration channels and calibration functions
 * 2023-08-21     lyon         port for PikaPython
 */

#ifndef PIKA_HAL
#include <board.h>
#endif


#if defined(BSP_USING_ADC1) || defined(BSP_USING_ADC2) || defined(BSP_USING_ADC3)
#include "pika_drv_config.h"

//#define DRV_DEBUG

#define LOG_TAG             "drv.adc"

#ifndef PIKA_HAL
#include <drv_log.h>
#endif

static ADC_HandleTypeDef adc_config[] =
{
#ifdef BSP_USING_ADC1
    ADC1_CONFIG,
#endif

#ifdef BSP_USING_ADC2
    ADC2_CONFIG,
#endif

#ifdef BSP_USING_ADC3
    ADC3_CONFIG,
#endif
};

struct stm32_adc
{
    ADC_HandleTypeDef ADC_Handler;
    struct rt_adc_device stm32_adc_device;
};

static struct stm32_adc stm32_adc_obj[sizeof(adc_config) / sizeof(adc_config[0])];

static rt_err_t stm32_adc_get_channel(rt_int8_t rt_channel, uint32_t *stm32_channel)
{
    switch (rt_channel)
    {
    case  0:
        *stm32_channel = ADC_CHANNEL_0;
        break;
    case  1:
        *stm32_channel = ADC_CHANNEL_1;
        break;
    case  2:
        *stm32_channel = ADC_CHANNEL_2;
        break;
    case  3:
        *stm32_channel = ADC_CHANNEL_3;
        break;
    case  4:
        *stm32_channel = ADC_CHANNEL_4;
        break;
    case  5:
        *stm32_channel = ADC_CHANNEL_5;
        break;
    case  6:
        *stm32_channel = ADC_CHANNEL_6;
        break;
    case  7:
        *stm32_channel = ADC_CHANNEL_7;
        break;
    case  8:
        *stm32_channel = ADC_CHANNEL_8;
        break;
    case  9:
        *stm32_channel = ADC_CHANNEL_9;
        break;
    case 10:
        *stm32_channel = ADC_CHANNEL_10;
        break;
    case 11:
        *stm32_channel = ADC_CHANNEL_11;
        break;
    case 12:
        *stm32_channel = ADC_CHANNEL_12;
        break;
    case 13:
        *stm32_channel = ADC_CHANNEL_13;
        break;
    case 14:
        *stm32_channel = ADC_CHANNEL_14;
        break;
    case 15:
        *stm32_channel = ADC_CHANNEL_15;
        break;
#ifdef ADC_CHANNEL_16
    case 16:
        *stm32_channel = ADC_CHANNEL_16;
        break;
#endif /* ADC_CHANNEL_16 */
    case 17:
        *stm32_channel = ADC_CHANNEL_17;
        break;
#ifdef ADC_CHANNEL_18
    case 18:
        *stm32_channel = ADC_CHANNEL_18;
        break;
#endif /* ADC_CHANNEL_18 */
#ifdef ADC_CHANNEL_19
    case 19:
        *stm32_channel = ADC_CHANNEL_19;
        break;
#endif /* ADC_CHANNEL_19 */
#ifdef ADC_CHANNEL_VREFINT
    case RT_ADC_INTERN_CH_VREF:
        *stm32_channel = ADC_CHANNEL_VREFINT;
        break;
#endif /* ADC_CHANNEL_VREFINT */
#ifdef ADC_CHANNEL_VBAT
    case RT_ADC_INTERN_CH_VBAT:
        *stm32_channel = ADC_CHANNEL_VBAT;
        break;
#endif /* ADC_CHANNEL_VBAT */
#ifdef ADC_CHANNEL_TEMPSENSOR
    case RT_ADC_INTERN_CH_TEMPER:
        *stm32_channel = ADC_CHANNEL_TEMPSENSOR;
        break;
#endif /* ADC_CHANNEL_TEMPSENSOR */
    default:
        return -RT_EINVAL;
    }

    return RT_EOK;
}

#define ADCx_CLK_ENABLE         __HAL_RCC_ADC1_CLK_ENABLE

static void adcx_clock_enable(ADC_HandleTypeDef *adch) {
    #if defined(STM32F0) || defined(STM32F1) || defined(STM32F4) || defined(STM32F7) || defined(STM32L1)
    ADCx_CLK_ENABLE();
    #elif defined(STM32H7A3xx) || defined(STM32H7A3xxQ) || defined(STM32H7B3xx) || defined(STM32H7B3xxQ)
    __HAL_RCC_ADC12_CLK_ENABLE();
    __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
    #elif defined(STM32G0)
    __HAL_RCC_ADC_CLK_ENABLE();
    #elif defined(STM32G4)
    __HAL_RCC_ADC12_CLK_ENABLE();
    #elif defined(STM32H5)
    __HAL_RCC_ADC_CLK_ENABLE();
    #elif defined(STM32H7)
    if (adch->Instance == ADC3) {
        __HAL_RCC_ADC3_CLK_ENABLE();
    } else {
        __HAL_RCC_ADC12_CLK_ENABLE();
    }
    __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
    #elif defined(STM32L4) || defined(STM32WB)
    if (__HAL_RCC_GET_ADC_SOURCE() == RCC_ADCCLKSOURCE_NONE) {
        __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_SYSCLK);
    }
    __HAL_RCC_ADC_CLK_ENABLE();
    #else
    #error Unsupported processor
    #endif
}

static rt_err_t stm32_adc_enabled(struct rt_adc_device *device, rt_int8_t channel, rt_bool_t enabled)
{
    ADC_HandleTypeDef *stm32_adc_handler;
    RT_ASSERT(device != RT_NULL);
    stm32_adc_handler = device->parent.user_data;

    if (enabled)
    {
        ADC_ChannelConfTypeDef ADC_ChanConf;
        rt_memset(&ADC_ChanConf, 0, sizeof(ADC_ChanConf));

        if(stm32_adc_get_channel(channel, &ADC_ChanConf.Channel) != RT_EOK)
        {
            LOG_E("ADC channel illegal: %d", channel);
            return -RT_EINVAL;
        }


#if defined(SOC_SERIES_STM32MP1) || defined (SOC_SERIES_STM32H7) || defined (SOC_SERIES_STM32WB) || defined(SOC_SERIES_STM32U5)
        ADC_ChanConf.Rank = ADC_REGULAR_RANK_1;
#else
        ADC_ChanConf.Rank = 1;
#endif

#if defined(SOC_SERIES_STM32F0)
        ADC_ChanConf.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
#elif defined(SOC_SERIES_STM32F1)
        ADC_ChanConf.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
#elif defined(SOC_SERIES_STM32F2) || defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32F7)
        ADC_ChanConf.SamplingTime = ADC_SAMPLETIME_112CYCLES;
#elif defined(SOC_SERIES_STM32L4)
        ADC_ChanConf.SamplingTime = ADC_SAMPLETIME_247CYCLES_5;
#elif defined(SOC_SERIES_STM32MP1)
        ADC_ChanConf.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
#elif defined(SOC_SERIES_STM32H7)
        ADC_ChanConf.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
#elif defined(SOC_SERIES_STM32U5)
        ADC_ChanConf.SamplingTime = ADC_SAMPLETIME_814CYCLES;
    #elif defined (SOC_SERIES_STM32WB)
        ADC_ChanConf.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
#endif

#if defined(SOC_SERIES_STM32F2) || defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32F7) || defined(SOC_SERIES_STM32L4) || defined (SOC_SERIES_STM32WB)
        ADC_ChanConf.Offset = 0;
#endif

#if defined(SOC_SERIES_STM32L4)
        ADC_ChanConf.OffsetNumber = ADC_OFFSET_NONE;
        ADC_ChanConf.SingleDiff = LL_ADC_SINGLE_ENDED;
#elif defined(SOC_SERIES_STM32MP1) || defined(SOC_SERIES_STM32H7) || defined (SOC_SERIES_STM32WB) || defined(SOC_SERIES_STM32U5)
        ADC_ChanConf.OffsetNumber = ADC_OFFSET_NONE;  /* ADC channel affected to offset number */
        ADC_ChanConf.Offset       = 0;
        ADC_ChanConf.SingleDiff   = ADC_SINGLE_ENDED; /* ADC channel differential mode */
#endif

    /* enable the analog power domain before configuring channel */
#if defined(SOC_SERIES_STM32U5)
        __HAL_RCC_PWR_CLK_ENABLE();
        HAL_PWREx_EnableVddA();
#endif /* defined(SOC_SERIES_STM32U5) */

    if(HAL_ADC_ConfigChannel(stm32_adc_handler, &ADC_ChanConf) != HAL_OK)
    {
        LOG_E("Failed to configure ADC channel %d", channel);
    }

    /* perform an automatic ADC calibration to improve the conversion accuracy */
#if defined(SOC_SERIES_STM32L4) || defined (SOC_SERIES_STM32WB)
        if (HAL_ADCEx_Calibration_Start(stm32_adc_handler, ADC_ChanConf.SingleDiff) != HAL_OK)
        {
            LOG_E("ADC calibration error!\n");
            return -RT_ERROR;
        }
#elif defined(SOC_SERIES_STM32MP1) || defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32U5)
        /* Run the ADC linear calibration in single-ended mode */
        if (HAL_ADCEx_Calibration_Start(stm32_adc_handler, ADC_CALIB_OFFSET_LINEARITY, ADC_ChanConf.SingleDiff) != HAL_OK)
        {
            LOG_E("ADC open linear calibration error!\n");
            /* Calibration Error */
            return -RT_ERROR;
        }
#endif
        HAL_ADC_Start(stm32_adc_handler);
    }
    else
    {
        HAL_ADC_Stop(stm32_adc_handler);
    }

    return RT_EOK;
}

static rt_uint8_t stm32_adc_get_resolution(struct rt_adc_device *device)
{
#if defined(SOC_SERIES_STM32F1) || defined(SOC_SERIES_STM32F3)
    return 12;
#else
    ADC_HandleTypeDef *stm32_adc_handler = device->parent.user_data;

    RT_ASSERT(device != RT_NULL);

    switch(stm32_adc_handler->Init.Resolution)
    {
#ifdef SOC_SERIES_STM32H7
        case ADC_RESOLUTION_16B:
            return 16;
#endif /* SOC_SERIES_STM32H7 */
#if defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32U5)
        case ADC_RESOLUTION_14B:
            return 14;
#endif /* defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32U5) */
        case ADC_RESOLUTION_12B:
            return 12;
        case ADC_RESOLUTION_10B:
            return 10;
        case ADC_RESOLUTION_8B:
            return 8;
#if (defined(SOC_SERIES_STM32H7)|| defined(SOC_SERIES_STM32U5)) && (defined(ADC_RESOLUTION_6B))
        case ADC_RESOLUTION_6B:
            return 6;
#endif /* defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32U5) */
        default:
            return 0;
    }
#endif /* defined(SOC_SERIES_STM32F1) || defined(SOC_SERIES_STM32F3) */
}

static rt_int16_t stm32_adc_get_vref (struct rt_adc_device *device)
{
    if(device == RT_NULL)
      return -RT_ERROR;

    rt_uint16_t vref_mv;
#ifdef __LL_ADC_CALC_VREFANALOG_VOLTAGE
    rt_err_t ret = RT_EOK;
    rt_uint32_t vref_value;

    ADC_HandleTypeDef *stm32_adc_handler = device->parent.user_data;

    ret = rt_adc_enable(device, RT_ADC_INTERN_CH_VREF);
    if (ret != RT_EOK) return (rt_int16_t)ret;
    vref_value = rt_adc_read(device, RT_ADC_INTERN_CH_VREF);
    ret = rt_adc_disable(device, RT_ADC_INTERN_CH_VREF);
    if (ret != RT_EOK) return (rt_int16_t)ret;

#ifdef SOC_SERIES_STM32U5
    vref_mv = __LL_ADC_CALC_VREFANALOG_VOLTAGE(stm32_adc_handler->Instance, vref_value, stm32_adc_handler->Init.Resolution);
#else
    vref_mv = __LL_ADC_CALC_VREFANALOG_VOLTAGE(vref_value, stm32_adc_handler->Init.Resolution);
#endif

#else
    vref_mv = 3300;
#endif /* __LL_ADC_CALC_VREFANALOG_VOLTAGE */
    return vref_mv;
}


static  void adc_wait_for_eoc_or_timeout(ADC_HandleTypeDef *adcHandle, int32_t timeout) {
    uint32_t tickstart = HAL_GetTick();
    #if defined(STM32F4) || defined(STM32F1) || defined(STM32F7) || defined(STM32L1)
    while ((adcHandle->Instance->SR & ADC_FLAG_EOC) != ADC_FLAG_EOC) {
    #elif defined(STM32F0) || defined(STM32G0) || defined(STM32G4) || defined(STM32H5) || defined(STM32H7) || defined(STM32L4) || defined(STM32WB)
    while (READ_BIT(adcHandle->Instance->ISR, ADC_FLAG_EOC) != ADC_FLAG_EOC) {
    #else
    #error Unsupported processor
        #endif
        if (((HAL_GetTick() - tickstart) > timeout)) {
            break; // timeout
        }
    }
}

static rt_err_t stm32_adc_get_value(struct rt_adc_device *device, rt_int8_t channel, rt_uint32_t *value_)
{
    ADC_HandleTypeDef *stm32_adc_handler;

    RT_ASSERT(device != RT_NULL);
    RT_ASSERT(value != RT_NULL);

    stm32_adc_handler = device->parent.user_data;

    /* Wait for the ADC to convert */
    uint32_t value;
    #if defined(STM32G4)
    // For STM32G4 there is errata 2.7.7, "Wrong ADC result if conversion done late after
    // calibration or previous conversion".  According to the errata, this can be avoided
    // by performing two consecutive ADC conversions and keeping the second result.
    for (uint8_t i = 0; i < 2; i++)
    #endif
    {
        HAL_ADC_Start(stm32_adc_handler);
        adc_wait_for_eoc_or_timeout(stm32_adc_handler, 10);
        value = stm32_adc_handler->Instance->DR;
    }
    HAL_ADC_Stop(stm32_adc_handler);
    /* get ADC value */
    *value_ = value;
    return RT_EOK;
}

static const struct rt_adc_ops stm_adc_ops =
{
    .enabled = stm32_adc_enabled,
    .convert = stm32_adc_get_value,
    .get_resolution = stm32_adc_get_resolution,
    .get_vref = stm32_adc_get_vref,
};


static rt_err_t rt_hw_adc_register(rt_adc_device_t device, const char *name, const struct rt_adc_ops *ops, const void *user_data)
{
    rt_err_t result = RT_EOK;
    RT_ASSERT(ops != RT_NULL && ops->convert != RT_NULL);

    device->parent.type = RT_Device_Class_ADC;
    device->parent.rx_indicate = RT_NULL;
    device->parent.tx_complete = RT_NULL;

#ifdef RT_USING_DEVICE_OPS
    device->parent.ops         = NULL;
#else
    device->parent.init        = RT_NULL;
    device->parent.open        = RT_NULL;
    device->parent.close       = RT_NULL;
    device->parent.read        = _adc_read;
    device->parent.write       = RT_NULL;
    device->parent.control     = _adc_control;
#endif
    device->ops = ops;
    device->parent.user_data = (void *)user_data;

    // result = rt_device_register(&device->parent, name, RT_DEVICE_FLAG_RDWR);

    return RT_EOK;
}


volatile static int adc_inited = 0;
static int stm32_adc_init(void)
{
    int result = RT_EOK;
    /* save adc name */
    char name_buf[5] = {'a', 'd', 'c', '0', 0};
    int i = 0;

    for (i = 0; i < sizeof(adc_config) / sizeof(adc_config[0]); i++)
    {
        /* ADC init */
        name_buf[3] = '0';
        stm32_adc_obj[i].ADC_Handler = adc_config[i];
#if defined(ADC1)
        if (stm32_adc_obj[i].ADC_Handler.Instance == ADC1)
        {
            name_buf[3] = '1';
        }
#endif
#if defined(ADC2)
        if (stm32_adc_obj[i].ADC_Handler.Instance == ADC2)
        {
            name_buf[3] = '2';
        }
#endif
#if defined(ADC3)
        if (stm32_adc_obj[i].ADC_Handler.Instance == ADC3)
        {
            name_buf[3] = '3';
        }
#endif
        // Enable clk
        adcx_clock_enable(&stm32_adc_obj[i].ADC_Handler);
        if (HAL_ADC_Init(&stm32_adc_obj[i].ADC_Handler) != HAL_OK)
        {
            LOG_E("%s init failed", name_buf);
            result = -RT_ERROR;
        }
        else
        {
            /* register ADC device */
            if (rt_hw_adc_register(&stm32_adc_obj[i].stm32_adc_device, name_buf, &stm_adc_ops, &stm32_adc_obj[i].ADC_Handler) == RT_EOK)
            {
                LOG_D("%s init success", name_buf);
            }
            else
            {
                LOG_E("%s register failed", name_buf);
                result = -RT_ERROR;
            }
        }
    }

    return result;
}
// INIT_BOARD_EXPORT(stm32_adc_init);

typedef struct platform_data_ADC{
    uint32_t pin;
    rt_int8_t rt_channel;
}platform_data_ADC;

#define rt_adc_device_adc1 (&stm32_adc_obj[0].stm32_adc_device)

int pika_hal_platform_ADC_open(pika_dev* dev, char* name) {
    if(!adc_inited){
        stm32_adc_init();
        adc_inited = 1;
    }
    platform_data_ADC* data = pikaMalloc(sizeof(platform_data_ADC));
    data->pin = _stm32_pin_get(name);
    switch((uintptr_t)PIN_STPORT(data->pin)){
        case (uintptr_t)GPIOA:
            switch(PIN_NO(data->pin)){
                case 0:
                    data->rt_channel = 0;
                    break;
                case 1:
                    data->rt_channel = 1;
                    break;
                case 2:
                    data->rt_channel = 2;
                    break;
                case 3:
                    data->rt_channel = 3;
                    break;
                case 4:
                    data->rt_channel = 4;
                    break;
                case 5:
                    data->rt_channel = 5;
                    break;
                case 6:
                    data->rt_channel = 6;
                    break;
                case 7:
                    data->rt_channel = 7;
                    break;
                default:
                    pika_platform_printf("Error: pin: %s not support\r\n", name);
                    return -1;
            }
            break;
        case (uintptr_t)GPIOB:
            switch(PIN_NO(data->pin)){
                case 0:
                    data->rt_channel = 8;
                    break;
                case 1:
                    data->rt_channel = 9;
                    break;
                default:
                    pika_platform_printf("Error: pin: %s not support\r\n", name);
                    return -1;
            }
            break;
        case (uintptr_t)GPIOC:
            switch(PIN_NO(data->pin)){
                case 0:
                    data->rt_channel = 10;
                    break;
                case 1:
                    data->rt_channel = 11;
                    break;
                case 2:
                    data->rt_channel = 12;
                    break;
                case 3:
                    data->rt_channel = 13;
                    break;
                case 4:
                    data->rt_channel = 14;
                    break;
                case 5:
                    data->rt_channel = 15;
                    break;
                default:
                    pika_platform_printf("Error: pin: %s not support\r\n", name);
                    return -1;
            }
            break;
        default:
            pika_platform_printf("Error: pin: %s not support\r\n", name);
            return -1;
    }
    dev->platform_data = data;
    return 0;
}

int pika_hal_platform_ADC_close(pika_dev* dev) {
    if (NULL != dev->platform_data) {
        pikaFree(dev->platform_data, sizeof(platform_data_ADC));
        dev->platform_data = NULL;
    }
    return 0;
}

int pika_hal_platform_ADC_ioctl_config(pika_dev* dev,
                                       pika_hal_ADC_config* cfg) {
    return -1;
}

int pika_hal_platform_ADC_read(pika_dev* dev, void* buf, size_t count) {
    platform_data_ADC* data = dev->platform_data;
    rt_uint32_t raw_value = rt_adc_read(rt_adc_device_adc1, data->rt_channel);
    *((uint32_t*)buf) = raw_value;
    return 0;
}

int pika_hal_platform_ADC_write(pika_dev* dev, void* buf, size_t count) {
    return -1;
}


int pika_hal_platform_ADC_ioctl_enable(pika_dev* dev) {
    platform_data_ADC* data = dev->platform_data;
    
    mp_hal_gpio_clock_enable(PIN_STPORT(data->pin));
    /* init GPIO */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = PIN_STPIN(data->pin);
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(PIN_STPORT(data->pin), &GPIO_InitStruct);
    
    rt_err_t ret = rt_adc_enable(rt_adc_device_adc1, data->rt_channel);
    if(RT_EOK != ret){
        pika_platform_printf("Error: ADC enable failed\r\n"); 
        return -1;
    }
    pika_hal_ADC_config *cfg = dev->ioctl_config;
    
    /* init ADC config */
    rt_uint8_t resolution = stm32_adc_get_resolution(rt_adc_device_adc1);
    cfg->max = 1 << resolution;
    return 0;
}

int pika_hal_platform_ADC_ioctl_disable(pika_dev* dev) {
    return -1;
}

#endif /* BSP_USING_ADC */
