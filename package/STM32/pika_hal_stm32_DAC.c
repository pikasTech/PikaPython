#include "pika_hal_stm32_common.h"

/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2023-2023, PikaPython Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2020-06-18     thread-liu        the first version
 * 2020-10-09     thread-liu   Porting for stm32h7xx
 * 2023-08-21     lyon         port for PikaPython
 */

#ifndef PIKA_HAL
#include <board.h>
#endif

#if defined(BSP_USING_DAC1) || defined(BSP_USING_DAC2)
#include "pika_drv_config.h"

//#define DRV_DEBUG
//#define LOG_TAG             "drv.dac"
//#include <drv_log.h>

static DAC_HandleTypeDef dac_config[] =
{
#ifdef BSP_USING_DAC1
    DAC1_CONFIG,
#endif

#ifdef BSP_USING_DAC2
    DAC2_CONFIG,
#endif
};

struct stm32_dac
{
    DAC_HandleTypeDef DAC_Handler;
    struct rt_dac_device stm32_dac_device;
};

static struct stm32_dac stm32_dac_obj[sizeof(dac_config) / sizeof(dac_config[0])];

static rt_uint32_t stm32_dac_get_channel(rt_uint32_t channel)
{
    rt_uint32_t stm32_channel = 0;

    switch (channel)
    {
    case  1:
        stm32_channel = DAC_CHANNEL_1;
        break;
    case  2:
        stm32_channel = DAC_CHANNEL_2;
        break;
    default:
        RT_ASSERT(0);
        break;
    }

    return stm32_channel;
}

static rt_err_t stm32_dac_enabled(struct rt_dac_device *device, rt_uint32_t channel)
{
    uint32_t dac_channel;
    DAC_HandleTypeDef *stm32_dac_handler;
    RT_ASSERT(device != RT_NULL);
    stm32_dac_handler = device->parent.user_data;

#if defined(SOC_SERIES_STM32MP1) || defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32F4)
    if ((channel <= 2) && (channel > 0))
    {
        /* set stm32 dac channel */
        dac_channel =  stm32_dac_get_channel(channel);
    }
    else
    {
      LOG_E("dac channel must be 1 or 2.");
      return -RT_ERROR;
    }
    HAL_DAC_Start(stm32_dac_handler, dac_channel);
#endif

    return RT_EOK;
}

static rt_err_t stm32_dac_disabled(struct rt_dac_device *device, rt_uint32_t channel)
{
    uint32_t dac_channel;
    DAC_HandleTypeDef *stm32_dac_handler;
    RT_ASSERT(device != RT_NULL);
    stm32_dac_handler = device->parent.user_data;

#if defined(SOC_SERIES_STM32MP1) || defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32F4)
    if ((channel <= 2) && (channel > 0))
    {
        /* set stm32 dac channel */
        dac_channel =  stm32_dac_get_channel(channel);
    }
    else
    {
      LOG_E("dac channel must be 1 or 2.");
      return -RT_ERROR;
    }
    HAL_DAC_Stop(stm32_dac_handler, dac_channel);
#endif

    return RT_EOK;
}

static rt_uint8_t stm32_dac_get_resolution(struct rt_dac_device *device)
{
    DAC_HandleTypeDef *stm32_dac_handler;

    RT_ASSERT(device != RT_NULL);

    stm32_dac_handler = device->parent.user_data;
    (void)stm32_dac_handler;

    /* Only has supported DAC_ALIGN_12B_R, so it will return 12 bits */
    return 12;
}

static rt_err_t stm32_set_dac_value(struct rt_dac_device *device, rt_uint32_t channel, rt_uint32_t *value)
{
    uint32_t dac_channel;
    DAC_ChannelConfTypeDef DAC_ChanConf;
    DAC_HandleTypeDef *stm32_dac_handler;

    RT_ASSERT(device != RT_NULL);
    RT_ASSERT(value != RT_NULL);

    stm32_dac_handler = device->parent.user_data;

    rt_memset(&DAC_ChanConf, 0, sizeof(DAC_ChanConf));

#if defined(SOC_SERIES_STM32MP1) || defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32F4)
    if ((channel <= 2) && (channel > 0))
    {
        /* set stm32 dac channel */
        dac_channel =  stm32_dac_get_channel(channel);
    }
    else
    {
      LOG_E("dac channel must be 1 or 2.");
      return -RT_ERROR;
    }
#endif

#if defined(SOC_SERIES_STM32MP1) || defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32F4)
    DAC_ChanConf.DAC_Trigger      = DAC_TRIGGER_NONE;
    DAC_ChanConf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
#endif
    /* config dac out channel*/
    if (HAL_DAC_ConfigChannel(stm32_dac_handler, &DAC_ChanConf, dac_channel) != HAL_OK)
    {
        LOG_D("Config dac out channel Error!\n");
        return -RT_ERROR;
    }
    /* set dac channel out value*/
    if (HAL_DAC_SetValue(stm32_dac_handler, dac_channel, DAC_ALIGN_12B_R, *value) != HAL_OK)
    {
        LOG_D("Setting dac channel out value Error!\n");
        return -RT_ERROR;
    }
    /* start dac */
    if (HAL_DAC_Start(stm32_dac_handler, dac_channel) != HAL_OK)
    {
        LOG_D("Start dac Error!\n");
        return -RT_ERROR;
    }

    return RT_EOK;
}

static const struct rt_dac_ops stm_dac_ops =
{
    .disabled = stm32_dac_disabled,
    .enabled  = stm32_dac_enabled,
    .convert  = stm32_set_dac_value,
    .get_resolution = stm32_dac_get_resolution,
};

#ifdef PIKA_HAL
rt_err_t rt_hw_dac_register(rt_dac_device_t device, const char *name, const struct rt_dac_ops *ops, const void *user_data)
{
    rt_err_t result = RT_EOK;
    RT_ASSERT(ops != RT_NULL && ops->convert != RT_NULL);

    device->parent.type = RT_Device_Class_DAC;
    device->parent.rx_indicate = RT_NULL;
    device->parent.tx_complete = RT_NULL;

#ifdef RT_USING_DEVICE_OPS
//    device->parent.ops         = &dac_ops;
    device->parent.ops         = RT_NULL;
#else
    device->parent.init        = RT_NULL;
    device->parent.open        = RT_NULL;
    device->parent.close       = RT_NULL;
    device->parent.read        = RT_NULL;
    device->parent.write       = _dac_write;
    device->parent.control     = _dac_control;
#endif
    device->ops = ops;
    device->parent.user_data = (void *)user_data;

    // result = rt_device_register(&device->parent, name, RT_DEVICE_FLAG_RDWR);

    return RT_EOK;
}
#endif

volatile static int dac_inited = 0;
static int stm32_dac_init(void)
{
    int result = RT_EOK;
    /* save dac name */
    char name_buf[5] = {'d', 'a', 'c', '0', 0};
    int i = 0;

    for (i = 0; i < sizeof(dac_config) / sizeof(dac_config[0]); i++)
    {
        /* dac init */
        name_buf[3] = '0';
        stm32_dac_obj[i].DAC_Handler = dac_config[i];
#if defined(DAC1)
        if (stm32_dac_obj[i].DAC_Handler.Instance == DAC1)
        {
            name_buf[3] = '1';
        }
#endif
#if defined(DAC2)
        if (stm32_dac_obj[i].dac_Handler.Instance == DAC2)
        {
            name_buf[3] = '2';
        }
#endif
        if (HAL_DAC_Init(&stm32_dac_obj[i].DAC_Handler) != HAL_OK)
        {
            LOG_E("%s init failed", name_buf);
            result = -RT_ERROR;
        }
        else
        {
            /* register dac device */
            if (rt_hw_dac_register(&stm32_dac_obj[i].stm32_dac_device, name_buf, &stm_dac_ops, &stm32_dac_obj[i].DAC_Handler) == RT_EOK)
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
// INIT_DEVICE_EXPORT(stm32_dac_init);

static rt_err_t rt_dac_write(rt_dac_device_t dev, rt_uint32_t channel, rt_uint32_t value)
{
    RT_ASSERT(dev);

    return dev->ops->convert(dev, channel, &value);
}

static rt_err_t rt_dac_enable(rt_dac_device_t dev, rt_uint32_t channel)
{
    rt_err_t result = RT_EOK;

    RT_ASSERT(dev);
    if (dev->ops->enabled != RT_NULL)
    {
        result = dev->ops->enabled(dev, channel);
    }
    else
    {
        result = -RT_ENOSYS;
    }

    return result;
}

typedef struct platform_data_DAC {
    uint32_t pin;
    rt_int8_t rt_channel;
} platform_data_DAC;
#define rt_dac_device_dac1 (&stm32_dac_obj[0].stm32_dac_device)

static void dacx_clock_enable(void){
#ifdef __HAL_RCC_DAC_CLK_ENABLE
    __HAL_RCC_DAC_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_DAC1_CLK_ENABLE
    __HAL_RCC_DAC1_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_DAC12_CLK_ENABLE
    __HAL_RCC_DAC12_CLK_ENABLE();
#endif
    __HAL_RCC_GPIOA_CLK_ENABLE();
}

int pika_hal_platform_DAC_open(pika_dev* dev, char* name) {
    if(!dac_inited){
        dacx_clock_enable(); // clock init
        stm32_dac_init();
        dac_inited = 1;
    }
    platform_data_DAC* data = pikaMalloc(sizeof(platform_data_DAC));
    data->pin = _stm32_pin_get(name);
    switch((uintptr_t)PIN_STPORT(data->pin)){
        case (uintptr_t)GPIOA:
            switch(PIN_NO(data->pin)){
                case 4:
                    data->rt_channel = 1;
                    break;
                case 5:
                    data->rt_channel = 2;
                    break;
                default:
                    pika_platform_printf("Error: pin: %s not supported, only PA4 and PA5 are supported.\r\n", name);
                    return -1;
            }
            break;
        default:
            pika_platform_printf("Error: pin: %s not supported, only PA4 and PA5 are supported.\r\n", name);
            return -1;
    }
    dev->platform_data = data;
    return 0;
}

int pika_hal_platform_DAC_close(pika_dev* dev) {
    if (dev->platform_data) {
        pikaFree(dev->platform_data, sizeof(platform_data_DAC));
        dev->platform_data = NULL;
    }
    return 0;
}

int pika_hal_platform_DAC_ioctl_config(pika_dev* dev,
                                       pika_hal_DAC_config* cfg) {
    return -1;
}

int pika_hal_platform_DAC_ioctl_enable(pika_dev* dev) {
    platform_data_DAC* data = dev->platform_data;
    
    /* init GPIO */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = PIN_STPIN(data->pin);
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(PIN_STPORT(data->pin), &GPIO_InitStruct);

    rt_err_t ret = rt_dac_enable(rt_dac_device_dac1, data->rt_channel);
    if(RT_EOK != ret){
        pika_platform_printf("Error: ADC enable failed\r\n"); 
        return -1;
    }
    pika_hal_DAC_config *cfg = dev->ioctl_config;
    
    /* init DAC config */
    rt_uint8_t resolution = stm32_dac_get_resolution(rt_dac_device_dac1);
    cfg->max = 1 << resolution;
    return 0;
}

int pika_hal_platform_DAC_read(pika_dev* dev, void* buf, size_t count) {
    return -1;
}

int pika_hal_platform_DAC_write(pika_dev* dev, void* buf, size_t count) {
    platform_data_DAC* data = dev->platform_data;
    rt_dac_write(rt_dac_device_dac1, data->rt_channel, *((uint32_t*)buf));
    return 0;
}

int pika_hal_platform_DAC_ioctl_disable(pika_dev* dev) {
    return -1;
}

#endif /* BSP_USING_DAC */
