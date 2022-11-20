#include "pika_hal.h"
#include "PikaObj.h"

PIKA_WEAK int pika_gpio_open(pika_dev* dev) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_gpio_close(pika_dev* dev) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_gpio_read(pika_dev* dev, uint8_t* buf, size_t count) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_gpio_write(pika_dev* dev, uint8_t* buf, size_t count) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_gpio_ioctl(pika_dev* dev, pika_dev_cmd cmd, uintptr_t arg) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_uart_open(pika_dev* dev) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_uart_close(pika_dev* dev) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_uart_read(pika_dev* dev, uint8_t* buf, size_t count) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_uart_write(pika_dev* dev, uint8_t* buf, size_t count) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_uart_ioctl(pika_dev* dev, pika_dev_cmd cmd, uintptr_t arg) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_i2c_open(pika_dev* dev) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_i2c_close(pika_dev* dev) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_i2c_read(pika_dev* dev, uint8_t* buf, size_t count) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_i2c_write(pika_dev* dev, uint8_t* buf, size_t count) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_i2c_ioctl(pika_dev* dev, pika_dev_cmd cmd, uintptr_t arg) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_spi_open(pika_dev* dev) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_spi_close(pika_dev* dev) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_spi_read(pika_dev* dev, uint8_t* buf, size_t count) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_spi_write(pika_dev* dev, uint8_t* buf, size_t count) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_spi_ioctl(pika_dev* dev, pika_dev_cmd cmd, uintptr_t arg) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_pwm_open(pika_dev* dev) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_pwm_close(pika_dev* dev) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_pwm_read(pika_dev* dev, uint8_t* buf, size_t count) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_pwm_write(pika_dev* dev, uint8_t* buf, size_t count) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_pwm_ioctl(pika_dev* dev, pika_dev_cmd cmd, uintptr_t arg) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_adc_open(pika_dev* dev) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_adc_close(pika_dev* dev) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_adc_read(pika_dev* dev, uint8_t* buf, size_t count) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_adc_write(pika_dev* dev, uint8_t* buf, size_t count) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_adc_ioctl(pika_dev* dev, pika_dev_cmd cmd, uintptr_t arg) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_dac_open(pika_dev* dev) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_dac_close(pika_dev* dev) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_dac_read(pika_dev* dev, uint8_t* buf, size_t count) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_dac_write(pika_dev* dev, uint8_t* buf, size_t count) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK int pika_dac_ioctl(pika_dev* dev, pika_dev_cmd cmd, uintptr_t arg) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

static const pika_dev_impl pika_dev_impl_list[PIKA_DEV_TYPE_MAX] = {
    [PIKA_DEV_TYPE_GPIO] =
        {
            .open = pika_gpio_open,
            .close = pika_gpio_close,
            .read = pika_gpio_read,
            .write = pika_gpio_write,
            .ioctl = pika_gpio_ioctl,
        },
    [PIKA_DEV_TYPE_UART] =
        {
            .open = pika_uart_open,
            .close = pika_uart_close,
            .read = pika_uart_read,
            .write = pika_uart_write,
            .ioctl = pika_uart_ioctl,
        },
    [PIKA_DEV_TYPE_I2C] =
        {
            .open = pika_i2c_open,
            .close = pika_i2c_close,
            .read = pika_i2c_read,
            .write = pika_i2c_write,
            .ioctl = pika_i2c_ioctl,
        },
    [PIKA_DEV_TYPE_SPI] =
        {
            .open = pika_spi_open,
            .close = pika_spi_close,
            .read = pika_spi_read,
            .write = pika_spi_write,
            .ioctl = pika_spi_ioctl,
        },
    [PIKA_DEV_TYPE_PWM] =
        {
            .open = pika_pwm_open,
            .close = pika_pwm_close,
            .read = pika_pwm_read,
            .write = pika_pwm_write,
            .ioctl = pika_pwm_ioctl,
        },
    [PIKA_DEV_TYPE_ADC] =
        {
            .open = pika_adc_open,
            .close = pika_adc_close,
            .read = pika_adc_read,
            .write = pika_adc_write,
            .ioctl = pika_adc_ioctl,
        },
    [PIKA_DEV_TYPE_DAC] =
        {
            .open = pika_dac_open,
            .close = pika_dac_close,
            .read = pika_dac_read,
            .write = pika_dac_write,
            .ioctl = pika_dac_ioctl,
        },
};

static pika_dev_impl* _pika_dev_get_impl(pika_dev_type type) {
    if (type >= PIKA_DEV_TYPE_MAX) {
        return NULL;
    }
    return (pika_dev_impl*)&pika_dev_impl_list[type];
}

pika_dev* pika_hal_open(pika_dev_type dev_type, char* name) {
    if (dev_type >= PIKA_DEV_TYPE_MAX) {
        return NULL;
    }
    pika_dev_impl* impl = _pika_dev_get_impl(dev_type);
    if (impl->open == NULL) {
        return NULL;
    }
    pika_dev* dev = (pika_dev*)pikaMalloc(sizeof(pika_dev));
    if (dev == NULL) {
        return NULL;
    }
    dev->type = dev_type;
    if (impl->open(dev) != 0) {
        pikaFree(dev, sizeof(pika_dev));
        return NULL;
    }
    return dev;
}

int pika_hal_close(pika_dev* dev) {
    if (dev == NULL) {
        return -1;
    }
    pika_dev_impl* impl = _pika_dev_get_impl(dev->type);
    if (impl->close == NULL) {
        return -1;
    }
    int ret = impl->close(dev);
    pikaFree(dev, sizeof(pika_dev));
    return ret;
}

int pika_hal_read(pika_dev* dev, uint8_t* buf, size_t len) {
    if (dev == NULL) {
        return -1;
    }
    pika_dev_impl* impl = _pika_dev_get_impl(dev->type);
    if (impl->read == NULL) {
        return -1;
    }
    return impl->read(dev, buf, len);
}

int pika_hal_write(pika_dev* dev, uint8_t* buf, size_t len) {
    if (dev == NULL) {
        return -1;
    }
    pika_dev_impl* impl = _pika_dev_get_impl(dev->type);
    if (impl->write == NULL) {
        return -1;
    }
    return impl->write(dev, buf, len);
}

static const int _pika_hal_cmd_arg_cnt[PIKA_DEV_CMD_MAX] = {
    [PIKA_DEV_CMD_ENABLE] = 0,      [PIKA_DEV_CMD_DISABLE] = 0,
    [PIKA_DEV_CMD_GPIO_MODE] = 1,   [PIKA_DEV_CMD_GPIO_PULL] = 1,
    [PIKA_DEV_CMD_GPIO_TOGGLE] = 0,
};

static int _pika_hal_get_arg_cnt(pika_dev_cmd cmd) {
    if (cmd >= PIKA_DEV_CMD_MAX) {
        return -1;
    }
    return _pika_hal_cmd_arg_cnt[cmd];
}

int pika_hal_ioctl(pika_dev* dev, pika_dev_cmd cmd, ...) {
    if (dev == NULL) {
        return -1;
    }
    int arg_cnt = _pika_hal_get_arg_cnt(cmd);
    if (arg_cnt < 0) {
        return -1;
    }
    pika_dev_impl* impl = _pika_dev_get_impl(dev->type);
    if (impl->ioctl == NULL) {
        return -1;
    }
    va_list args;
    va_start(args, arg_cnt);
    uintptr_t arg = va_arg(args, uintptr_t);
    int ret = impl->ioctl(dev, cmd, arg);
    va_end(args);
    return ret;
}
