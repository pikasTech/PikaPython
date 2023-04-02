#include "bflb_dac.h"
#include "pika_hal_bflb_common.h"

static int8_t _pin2ch(uint32_t pin) {
    switch (pin) {
        case 3:
            return DAC_CHANNEL_A;
        default:
            return -1;
    }
}

typedef struct platform_dac_t {
    struct bflb_device_s* device;
    uint8_t channel;
    uint8_t clk_div;
} platform_dac_t;

int pika_hal_platform_DAC_open(pika_dev* dev, char* name) {
    /* Support P1/P2 ... */
    if (name[0] == 'P') {
        int pin = fast_atoi(name + 1);
        int8_t ch = _pin2ch(pin);
        if (ch < 0) {
            pika_platform_printf("DAC channel not support: %s \r\n", name);
            return -1;
        }
        platform_dac_t* platform_dac = pikaMalloc(sizeof(platform_dac_t));
        memset(platform_dac, 0, sizeof(platform_dac_t));
        dev->platform_data = platform_dac;
        platform_dac->device = bflb_device_get_by_name("dac");
        platform_dac->channel = ch;
        return 0;
    }
    return -1;
}

int pika_hal_platform_DAC_close(pika_dev* dev) {
    if (dev->platform_data) {
        pikaFree(dev->platform_data, sizeof(platform_dac_t));
        dev->platform_data = NULL;
    }
    return 0;
}

int pika_hal_platform_DAC_ioctl_config(pika_dev* dev,
                                       pika_hal_DAC_config* cfg) {
    platform_dac_t* platform_dac = (platform_dac_t*)dev->platform_data;
    platform_dac->clk_div = DAC_CLK_DIV_16;
    cfg->max = 4096;
    cfg->vref = (pika_float)1.73;
    return 0;
}

int pika_hal_platform_DAC_ioctl_enable(pika_dev* dev) {
    platform_dac_t* platform_dac = (platform_dac_t*)dev->platform_data;
    if (!dev->is_enabled) {
        bflb_dac_init(platform_dac->device, platform_dac->clk_div);
        bflb_dac_channel_enable(platform_dac->device, platform_dac->channel);
        return 0;
    }
    return -1;
}

int pika_hal_platform_DAC_read(pika_dev* dev, void* buf, size_t count) {
    return -1;
}

int pika_hal_platform_DAC_write(pika_dev* dev, void* buf, size_t count) {
    platform_dac_t* platform_dac = (platform_dac_t*)dev->platform_data;
    uint32_t value = *(uint32_t*)buf;
    pika_debug("DAC write: %d", value);
    bflb_dac_set_value(platform_dac->device, platform_dac->channel, value);
    return 0;
}

int pika_hal_platform_DAC_ioctl_disable(pika_dev* dev) {
    platform_dac_t* platform_dac = (platform_dac_t*)dev->platform_data;
    bflb_dac_channel_disable(platform_dac->device, platform_dac->channel);
    return 0;
}
