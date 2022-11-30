#include <hosal_dac.h>
#include "../PikaStdDevice/pika_hal.h"

int pika_hal_platform_DAC_open(pika_dev* dev, char* name) {
    hosal_dac_dev_t* platform_dac = pikaMalloc(sizeof(hosal_dac_dev_t));
    memset(platform_dac, 0, sizeof(hosal_dac_dev_t));
    dev->platform_data = platform_dac;
    /* Support P1/P2 ... */
    if (name[0] == 'P') {
        int pin = fast_atoi(name + 1);
        if (pin != 13 && pin != 14) {
            /* bl602, only support P13, P14*/
            __platform_printf("DAC only support P13, P14\r\n");
            return -1;
        }
        platform_dac->config.pin = pin;
        platform_dac->port = 0;
#if PIKA_DEBUG_ENABLE
        __platform_printf("DAC: Open DAC on P%d\r\n", platform_dac->config.pin);
#endif
        return 0;
    }
#if PIKA_DEBUG_ENABLE
    __platform_printf("Failed to open DAC: %s\r\n", name);
#endif
    return -1;
}

int pika_hal_platform_DAC_close(pika_dev* dev) {
    hosal_dac_dev_t* platform_dac = (hosal_dac_dev_t*)dev->platform_data;
    if (NULL != platform_dac) {
        pikaFree(platform_dac, sizeof(hosal_dac_dev_t));
    }
    return 0;
}

int pika_hal_platform_DAC_ioctl_config(pika_dev* dev,
                                       pika_hal_DAC_config* cfg) {
    hosal_dac_dev_t* platform_dac = (hosal_dac_dev_t*)dev->platform_data;
    /* only support 8k,16k,32k */
    platform_dac->config.freq = cfg->speed;
    platform_dac->config.dma_enable = 0;
    cfg->max = 3300000;
    cfg->vref = (pika_float)3.3;
    return 0;
}

int pika_hal_platform_DAC_read(pika_dev* dev, void* buf, size_t count) {
    int ret = 0;
    hosal_dac_dev_t* platform_dac = (hosal_dac_dev_t*)dev->platform_data;
    ret = hosal_dac_get_value(platform_dac);
#if PIKA_DEBUG_ENABLE
    __platform_printf("DAC: Read %d\r\n", ret);
#endif
    if (ret < 0) {
        return ret;
    }
    *(uint32_t*)buf = ret;
    return 0;
}

int pika_hal_platform_DAC_write(pika_dev* dev, void* buf, size_t count) {
    hosal_dac_dev_t* platform_dac = (hosal_dac_dev_t*)dev->platform_data;
#if PIKA_DEBUG_ENALBE
    __platform_printf("DAC: Write %d\r\n", *(uint32_t*)buf);
#endif
    return hosal_dac_set_value(platform_dac, *(uint32_t*)buf);
}

int pika_hal_platform_DAC_ioctl_enable(pika_dev* dev) {
    hosal_dac_dev_t* platform_dac = (hosal_dac_dev_t*)dev->platform_data;
    if (!dev->is_enabled) {
        hosal_dac_init(platform_dac);
#if PIKA_DEBUG_ENABLE
        __platform_printf("DAC: Enable DAC, freq: %d\r\n",
                          platform_dac->config.freq);
#endif
        return 0;
    }
    return -1;
}

int pika_hal_platform_DAC_ioctl_disable(pika_dev* dev) {
    hosal_dac_dev_t* platform_dac = (hosal_dac_dev_t*)dev->platform_data;
    if (dev->is_enabled) {
        hosal_dac_finalize(platform_dac);
        return 0;
    }
    return -1;
}
