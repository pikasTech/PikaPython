#include <stdint.h>
#include "BaseObj.h"
#include "dataStrs.h"
#include "driver/gpio.h"
#include "pika_hal_ESP32_common.h"

int pika_hal_platform_GPIO_open(pika_dev* dev, char* name) {
    platform_data_GPIO* gpio =
        (platform_data_GPIO*)pikaMalloc(sizeof(platform_data_GPIO));
    if (NULL == gpio) {
        return -1;
    }
    memset(gpio, 0, sizeof(platform_data_GPIO));
    gpio->gpioPort = getGpioPin(name);
    if (-1 == gpio->gpioPort) {
        pikaFree(gpio, sizeof(platform_data_GPIO));
        return -1;
    }
    dev->platform_data = gpio;
    return 0;
}

int pika_hal_platform_GPIO_close(pika_dev* dev) {
    platform_data_GPIO* gpio = (platform_data_GPIO*)dev->platform_data;
    if (NULL == gpio) {
        return -1;
    }
    pikaFree(gpio, sizeof(platform_data_GPIO));
    dev->platform_data = NULL;
    return 0;
}

int pika_hal_platform_GPIO_read(pika_dev* dev, void* buf, size_t count) {
    platform_data_GPIO* gpio = (platform_data_GPIO*)dev->platform_data;
    if (NULL == gpio) {
        return -1;
    }
    *((uint32_t*)buf) = (uint32_t)gpio_get_level(gpio->gpioPort);
    return 0;
}

int pika_hal_platform_GPIO_write(pika_dev* dev, void* buf, size_t count) {
    platform_data_GPIO* gpio = (platform_data_GPIO*)dev->platform_data;
    if (NULL == gpio) {
        return -1;
    }
    gpio_set_level(gpio->gpioPort, *((uint32_t*)buf));
    return 0;
}

int pika_hal_platform_GPIO_ioctl_enable(pika_dev* dev) {
    platform_data_GPIO* gpio = (platform_data_GPIO*)dev->platform_data;
    if (NULL == gpio) {
        return -1;
    }
    gpio_config(&gpio->io_conf);
    return 0;
}

int pika_hal_platform_GPIO_ioctl_disable(pika_dev* dev) {
    platform_data_GPIO* gpio = (platform_data_GPIO*)dev->platform_data;
    if (NULL == gpio) {
        return -1;
    }
    gpio_reset_pin(gpio->gpioPort);
    return 0;
}

int pika_hal_platform_GPIO_ioctl_config(pika_dev* dev,
                                        pika_hal_GPIO_config* cfg) {
    platform_data_GPIO* gpio = (platform_data_GPIO*)dev->platform_data;
    if (NULL == gpio) {
        return -1;
    }
    if (!dev->is_enabled) {
        /* init config */
        gpio->io_conf.intr_type = GPIO_INTR_DISABLE;
        switch (cfg->dir) {
            case PIKA_HAL_GPIO_DIR_IN:
                gpio->io_conf.mode = GPIO_MODE_INPUT;
                break;
            case PIKA_HAL_GPIO_DIR_OUT:
                gpio->io_conf.mode = GPIO_MODE_OUTPUT;
                break;
            default:
                return -1;
        }
        gpio->io_conf.pin_bit_mask = 1 << gpio->gpioPort;
        switch (cfg->pull) {
            case PIKA_HAL_GPIO_PULL_UP:
                gpio->io_conf.pull_up_en = 1;
                gpio->io_conf.pull_down_en = 0;
                break;
            case PIKA_HAL_GPIO_PULL_DOWN:
                gpio->io_conf.pull_up_en = 0;
                gpio->io_conf.pull_down_en = 1;
                break;
            case PIKA_HAL_GPIO_PULL_NONE:
                gpio->io_conf.pull_up_en = 0;
                gpio->io_conf.pull_down_en = 0;
                break;
            default:
                return -1;
        }
    }
    if (dev->is_enabled) {
        /* config after init */
        switch (cfg->dir) {
            case PIKA_HAL_GPIO_DIR_IN:
                gpio_set_direction(gpio->gpioPort, GPIO_MODE_INPUT);
                break;
            case PIKA_HAL_GPIO_DIR_OUT:
                gpio_set_direction(gpio->gpioPort, GPIO_MODE_OUTPUT);
                break;
            default:
                return -1;
        }
        switch (cfg->pull) {
            case PIKA_HAL_GPIO_PULL_UP:
                gpio_set_pull_mode(gpio->gpioPort, GPIO_PULLUP_ONLY);
                break;
            case PIKA_HAL_GPIO_PULL_DOWN:
                gpio_set_pull_mode(gpio->gpioPort, GPIO_PULLDOWN_ONLY);
                break;
            case PIKA_HAL_GPIO_PULL_NONE:
                gpio_set_pull_mode(gpio->gpioPort, GPIO_FLOATING);
                break;
            default:
                return -1;
        }
    }
    return 0;
}
