#include "bflb_gpio.h"
#include "pika_hal_bflb_common.h"


int pika_hal_platform_GPIO_open(pika_dev* dev, char* name) {
    dev->platform_data = pikaMalloc(sizeof(platform_gpio_t));
    memset(dev->platform_data, 0, sizeof(platform_gpio_t));
    platform_gpio_t* gpio = (platform_gpio_t*)dev->platform_data;
    /* Support P1/P2 ... */
    gpio->device = bflb_device_get_by_name("gpio");
    gpio->pin = fast_atoi(name + 1);
    return 0;
}

int pika_hal_platform_GPIO_close(pika_dev* dev) {
    pikaFree(dev->platform_data, sizeof(platform_gpio_t));
    return 0;
}

int pika_hal_platform_GPIO_read(pika_dev* dev, void* buf, size_t count) {
    platform_gpio_t* gpio = (platform_gpio_t*)dev->platform_data;
    bool val = bflb_gpio_read(gpio->device, gpio->pin);
    *(uint32_t*)buf = val;
    return 0;
}

int pika_hal_platform_GPIO_write(pika_dev* dev, void* buf, size_t count) {
    platform_gpio_t* gpio = (platform_gpio_t*)dev->platform_data;
    bool val = *(uint32_t*)buf;
    if (val) {
        bflb_gpio_set(gpio->device, gpio->pin);
    } else {
        bflb_gpio_reset(gpio->device, gpio->pin);
    }
    return 0;
}

int pika_hal_platform_GPIO_ioctl_enable(pika_dev* dev) {
    platform_gpio_t* gpio = (platform_gpio_t*)dev->platform_data;
    bflb_gpio_init(gpio->device, gpio->pin, gpio->config);
    return 0;
}

int pika_hal_platform_GPIO_ioctl_disable(pika_dev* dev) {
    platform_gpio_t* gpio = (platform_gpio_t*)dev->platform_data;
    bflb_gpio_deinit(gpio->device, gpio->pin);
    return 0;
}

static void _gpio_irq_hanlder(int irq, void* arg) {
    static int i = 0;
    // get the device
    pika_dev* dev = (pika_dev*)arg;
    // get the gpio
    platform_gpio_t* gpio = (platform_gpio_t*)dev->platform_data;
    // get the config
    pika_hal_GPIO_config* cfg = (pika_hal_GPIO_config*)dev->ioctl_config;
    // get the int status
    bool intstatus = bflb_gpio_get_intstatus(gpio->device, gpio->pin);
    if (intstatus) {
        // call the callback
        cfg->event_callback(dev, cfg->event_callback_filter);
        // clear the int status
        bflb_gpio_int_clear(gpio->device, gpio->pin);
    }
}

int pika_hal_platform_GPIO_ioctl_config(pika_dev* dev,
                                        pika_hal_GPIO_config* cfg) {
    platform_gpio_t* gpio = (platform_gpio_t*)dev->platform_data;
    switch (cfg->dir) {
        case PIKA_HAL_GPIO_DIR_IN:
            gpio->config |= GPIO_INPUT;
            break;
        case PIKA_HAL_GPIO_DIR_OUT:
            gpio->config |= GPIO_OUTPUT;
            break;
        default:
            return -1;
    }
    switch (cfg->pull) {
        case PIKA_HAL_GPIO_PULL_NONE:
            gpio->config |= GPIO_FLOAT;
            break;
        case PIKA_HAL_GPIO_PULL_UP:
            gpio->config |= GPIO_PULLUP;
            break;
        case PIKA_HAL_GPIO_PULL_DOWN:
            gpio->config |= GPIO_PULLDOWN;
            break;
        default:
            return -1;
    }

    /* support event callback */
    if (NULL != cfg->event_callback &&
        PIKA_HAL_EVENT_CALLBACK_ENA_ENABLE == cfg->event_callback_ena) {
        switch (cfg->event_callback_filter) {
            case PIKA_HAL_GPIO_EVENT_SIGNAL_RISING:
                gpio->config |= GPIO_INT_TRIG_MODE_SYNC_RISING_EDGE;
                break;
            case PIKA_HAL_GPIO_EVENT_SIGNAL_FALLING:
                gpio->config |= GPIO_INT_TRIG_MODE_SYNC_FALLING_EDGE;
                break;

            default:
                __platform_printf(
                    "Error: not supported event callback filter %d\r\n",
                    cfg->event_callback_filter);
                return -1;
        }
        /* register the irq */
        bflb_gpio_int_mask(gpio->device, gpio->pin, false);
        bflb_irq_attach(gpio->device->irq_num, _gpio_irq_hanlder, dev);
        bflb_irq_enable(gpio->device->irq_num);
    }

    return 0;
}
