#include "bflb_adc.h"
#include "pika_hal_bflb_common.h"

static int8_t _pin2ch(uint32_t pin) {
    switch (pin) {
        case 0:
            return ADC_CHANNEL_9;
        case 1:
            return ADC_CHANNEL_8;
        case 2:
            return ADC_CHANNEL_2;
        case 3:
            return ADC_CHANNEL_3;
        case 10:
            return ADC_CHANNEL_7;
        case 12:
            return ADC_CHANNEL_6;
        case 13:
            return ADC_CHANNEL_5;
        case 14:
            return ADC_CHANNEL_4;
        case 19:
            return ADC_CHANNEL_1;
        case 20:
            return ADC_CHANNEL_0;
        case 27:
            return ADC_CHANNEL_10;
        case 28:
            return ADC_CHANNEL_11;
        default:
            return -1;
    }
}

typedef struct platform_adc_t {
    struct bflb_device_s* device;
    struct bflb_adc_config_s config;
    struct bflb_adc_channel_s channel;
} platform_adc_t;

int pika_hal_platform_ADC_open(pika_dev* dev, char* name) {
    /* Support P1/P2 ... */
    if (name[0] == 'P') {
        int8_t ch = _pin2ch(fast_atoi(name + 1));
        if (ch < 0) {
            pika_platform_printf("ADC channel not support: %s \r\n", name);
            return -1;
        }
        platform_adc_t* platform_adc =
            (platform_adc_t*)pikaMalloc(sizeof(platform_adc_t));
        memset(platform_adc, 0, sizeof(platform_adc_t));
        dev->platform_data = platform_adc;
        platform_adc->channel.pos_chan = ch;
        platform_adc->channel.neg_chan = ADC_CHANNEL_GND;
        platform_adc->device = bflb_device_get_by_name("adc");
        return 0;
    }
    return -1;
}

int pika_hal_platform_ADC_close(pika_dev* dev) {
    platform_adc_t* platform_adc = (platform_adc_t*)dev->platform_data;
    if (NULL != platform_adc) {
        pikaFree(platform_adc, sizeof(platform_adc_t));
        dev->platform_data = NULL;
    }
    return 0;
}

int pika_hal_platform_ADC_ioctl_config(pika_dev* dev,
                                       pika_hal_ADC_config* cfg) {
    platform_adc_t* platform_adc = (platform_adc_t*)dev->platform_data;
    platform_adc->config.clk_div = ADC_CLK_DIV_32;
    platform_adc->config.scan_conv_mode = true;
    platform_adc->config.vref = ADC_VREF_3P2V;
    switch (cfg->continue_or_single) {
        case PIKA_HAL_ADC_CONTINUOU:
            platform_adc->config.continuous_conv_mode = true;
            break;
        case PIKA_HAL_ADC_SINGLE:
            platform_adc->config.continuous_conv_mode = false;
            break;
        default:
            platform_adc->config.continuous_conv_mode = false;
            break;
    }
    switch (cfg->sampling_resolution) {
        case PIKA_HAL_ADC_RESOLUTION_12:
            platform_adc->config.resolution = ADC_RESOLUTION_12B;
            break;
        case PIKA_HAL_ADC_RESOLUTION_14:
            platform_adc->config.resolution = ADC_RESOLUTION_14B;
            break;
        case PIKA_HAL_ADC_RESOLUTION_16:
            platform_adc->config.resolution = ADC_RESOLUTION_16B;
            break;
        default:
            platform_adc->config.resolution = ADC_RESOLUTION_16B;
            break;
    }
    cfg->max = 3200;
    cfg->vref = (pika_float)3.3;
    return 0;
}

int pika_hal_platform_ADC_read(pika_dev* dev, void* buf, size_t count) {
    int ret = 0;
    platform_adc_t* platform_adc = (platform_adc_t*)dev->platform_data;
    bflb_adc_start_conversion(platform_adc->device);
    while (bflb_adc_get_count(platform_adc->device) < 1) {
        bflb_mtimer_delay_ms(1);
    }
    uint32_t raw_data = bflb_adc_read_raw(platform_adc->device);
    bflb_adc_stop_conversion(platform_adc->device);
    struct bflb_adc_result_s result;
    bflb_adc_parse_result(platform_adc->device, &raw_data, &result, 1);
    ret = result.millivolt;
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
    platform_adc_t* platform_adc = (platform_adc_t*)dev->platform_data;
    if (!dev->is_enabled) {
        bflb_adc_init(platform_adc->device, &platform_adc->config);
        bflb_adc_channel_config(platform_adc->device, &platform_adc->channel,
                                1);
        return 0;
    }
    return -1;
}

int pika_hal_platform_ADC_ioctl_disable(pika_dev* dev) {
    platform_adc_t* platform_adc = (platform_adc_t*)dev->platform_data;
    if (dev->is_enabled) {
        bflb_adc_deinit(platform_adc->device);
        return 0;
    }
    return -1;
}
