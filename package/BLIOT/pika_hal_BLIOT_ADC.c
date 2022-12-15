#include <hosal_adc.h>
#include "../PikaStdDevice/pika_hal.h"

static uint8_t _pin2ch(uint32_t pin) {
    /**********   BL602  ************
     *    channel0   ----->     gpio12
     *    channel1   ----->     gpio4
     *    channel2   ----->     gpio14
     *    channel3   ----->     gpio13
     *    channel4   ----->     gpio5
     *    channel5   ----->     gpio6
     *    channel7   ----->     gpio9
     *    channel9   ----->     gpio10
     *    channel10  ----->     gpio11
     *    channel11  ----->     gpio15
     */
    switch (pin) {
        case 12:
            return 0;
        case 4:
            return 1;
        case 14:
            return 2;
        case 13:
            return 3;
        case 5:
            return 4;
        case 6:
            return 5;
        case 9:
            return 7;
        case 10:
            return 9;
        case 11:
            return 10;
        case 15:
            return 11;
        default:
            return 0;
    }
}

int pika_hal_platform_ADC_open(pika_dev* dev, char* name) {
    hosal_adc_dev_t* platform_adc = pikaMalloc(sizeof(hosal_adc_dev_t));
    memset(platform_adc, 0, sizeof(hosal_adc_dev_t));
    dev->platform_data = platform_adc;
    /* Support P1/P2 ... */
    if (name[0] == 'P') {
        platform_adc->config.pin = fast_atoi(name + 1);
        platform_adc->port = 0;
#if PIKA_DEBUG_ENABLE
        __platform_printf("ADC: Open ADC on P%d\r\n", platform_adc->config.pin);
#endif
        return 0;
    }
#if PIKA_DEBUG_ENABLE
    __platform_printf("Failed to open ADC: %s\r\n", name);
#endif
    return -1;
}

int pika_hal_platform_ADC_close(pika_dev* dev) {
    hosal_adc_dev_t* platform_adc = (hosal_adc_dev_t*)dev->platform_data;
    if (NULL != platform_adc) {
        pikaFree(platform_adc, sizeof(hosal_adc_dev_t));
    }
    return 0;
}

int pika_hal_platform_ADC_ioctl_config(pika_dev* dev,
                                       pika_hal_ADC_config* cfg) {
    hosal_adc_dev_t* platform_adc = (hosal_adc_dev_t*)dev->platform_data;
    platform_adc->config.sampling_freq = cfg->sampling_freq;
    switch (cfg->continue_or_single) {
        case PIKA_HAL_ADC_CONTINUOU:
            platform_adc->config.mode = HOSAL_ADC_CONTINUE;
            break;
        case PIKA_HAL_ADC_SINGLE:
            platform_adc->config.mode = HOSAL_ADC_ONE_SHOT;
            break;
        default:
            platform_adc->config.mode = HOSAL_ADC_ONE_SHOT;
            break;
    }
    platform_adc->config.sample_resolution = cfg->sampling_resolution;
    cfg->max = 3200;
    cfg->vref = (pika_float)3.3;
#if PIKA_DEBUG_ENABLE
    __platform_printf("ADC: Config ADC on P%d, freq: %d, mode: %d\r\n",
                      platform_adc->config.pin,
                      platform_adc->config.sampling_freq,
                      platform_adc->config.mode);
#endif
    return 0;
}

int pika_hal_platform_ADC_read(pika_dev* dev, void* buf, size_t count) {
    int ret = 0;
    hosal_adc_dev_t* platform_adc = (hosal_adc_dev_t*)dev->platform_data;
    ret = hosal_adc_value_get(platform_adc, _pin2ch(platform_adc->config.pin),
                              0xFFFFFFFF);
#if PIKA_DEBUG_ENABLE
    __platform_printf("ADC: Read ADC on P%d, channel: %d, value: %d\r\n",
                      platform_adc->config.pin,
                      _pin2ch(platform_adc->config.pin), ret);
#endif
    if (ret < 0) {
        return ret;
    }
    *(uint32_t*)buf = ret;
    return 0;
}

int pika_hal_platform_ADC_write(pika_dev* dev, void* buf, size_t count) {
    /* Not support */
    return -1;
}

int pika_hal_platform_ADC_ioctl_enable(pika_dev* dev) {
    hosal_adc_dev_t* platform_adc = (hosal_adc_dev_t*)dev->platform_data;
    if (!dev->is_enabled) {
        hosal_dma_init();
        hosal_adc_init(platform_adc);
        hosal_adc_add_channel(platform_adc, _pin2ch(platform_adc->config.pin));
#if PIKA_DEBUG_ENABLE
        __platform_printf(
            "ADC: Enable ADC on P%d, freq: %d, mode: %d, ch: %d\r\n",
            platform_adc->config.pin, platform_adc->config.sampling_freq,
            platform_adc->config.mode, _pin2ch(platform_adc->config.pin));
#endif
        return 0;
    }
    return -1;
}

int pika_hal_platform_ADC_ioctl_disable(pika_dev* dev) {
    hosal_adc_dev_t* platform_adc = (hosal_adc_dev_t*)dev->platform_data;
    if (dev->is_enabled) {
        hosal_adc_finalize(platform_adc);
        return 0;
    }
    return -1;
}
