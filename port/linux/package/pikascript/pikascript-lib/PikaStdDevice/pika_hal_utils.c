#include "pika_hal_utils.h"

/* error handler */
PIKA_WEAK void pika_hal_utils_error_handler(int err,
                                            const char* file,
                                            int line,
                                            const char* func) {
    pika_platform_printf("Error code %d: %s()\r\n", err, func);
    pika_platform_printf("AT: %s:%d\r\n", file, line);
}

#define pika_hal_utils_raise_error(err) \
    pika_hal_utils_error_handler(err, __FILE__, __LINE__, __func__)

/* common utils */
int pika_hal_utils_enable(pika_dev* dev) {
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_ENABLE);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

int pika_hal_utils_disable(pika_dev* dev) {
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_DISABLE);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

/* GPIO utils */

int pika_hal_utils_GPIO_config(pika_dev* dev, pika_hal_GPIO_config* cfg) {
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

int pika_hal_utils_GPIO_set_mode(pika_dev* dev,
                                 PIKA_HAL_GPIO_DIR dir,
                                 PIKA_HAL_GPIO_PULL pull) {
    pika_hal_GPIO_config cfg = {0};
    cfg.dir = dir;
    cfg.pull = pull;
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

int pika_hal_utils_GPIO_set_dir(pika_dev* dev, PIKA_HAL_GPIO_DIR dir) {
    pika_hal_GPIO_config cfg = {0};
    cfg.dir = dir;
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

int pika_hal_utils_GPIO_set_pull(pika_dev* dev, PIKA_HAL_GPIO_PULL pull) {
    pika_hal_GPIO_config cfg = {0};
    cfg.pull = pull;
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

int pika_hal_utils_GPIO_write(pika_dev* dev, uint32_t val) {
    int ret = pika_hal_write(dev, &val, sizeof(uint32_t));
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

uint32_t pika_hal_utils_GPIO_read(pika_dev* dev) {
    uint32_t val = 0;
    int ret = pika_hal_read(dev, &val, sizeof(uint32_t));
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
        return 0;
    }
    return val;
}

/* UART utils */
int pika_hal_utils_UART_config(pika_dev* dev, pika_hal_UART_config* cfg) {
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

/* IIC utils */
int pika_hal_utils_IIC_config(pika_dev* dev, pika_hal_IIC_config* cfg) {
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

/* SPI utils */
int pika_hal_utils_SPI_config(pika_dev* dev, pika_hal_SPI_config* cfg) {
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

/* ADC utils */
int pika_hal_utils_ADC_config(pika_dev* dev, pika_hal_ADC_config* cfg) {
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

uint32_t pika_hal_utils_ADC_read(pika_dev* dev) {
    uint32_t val = 0;
    int ret = pika_hal_read(dev, &val, sizeof(uint32_t));
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
        return 0;
    }
    return val;
}

/* DAC utils */
int pika_hal_utils_DAC_config(pika_dev* dev, pika_hal_DAC_config* cfg) {
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

int pika_hal_utils_DAC_write(pika_dev* dev, uint32_t val) {
    int ret = pika_hal_write(dev, &val, sizeof(uint32_t));
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

/* PWM utils */
int pika_hal_utils_PWM_config(pika_dev* dev, pika_hal_PWM_config* cfg) {
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

int pika_hal_utils_PWM_set_period(pika_dev* dev,
                                  PIKA_HAL_PWM_PERIOD period_ns) {
    pika_hal_PWM_config cfg = {0};
    cfg.period = period_ns;
    /* keep duty unchanged because duty has no unused field */
    cfg.duty = ((pika_hal_PWM_config*)dev->ioctl_config)->duty;
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

int pika_hal_utils_PWM_set_duty(pika_dev* dev, PIKA_HAL_PWM_DUTY duty_ns) {
    pika_hal_PWM_config cfg = {0};
    cfg.duty = duty_ns;
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

/* SOFT_SPI utils */
int pika_hal_utils_SOFT_SPI_config(pika_dev* dev,
                                   pika_hal_SOFT_SPI_config* cfg) {
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

/* SOFT_IIC utils */
int pika_hal_utils_SOFT_IIC_config(pika_dev* dev,
                                   pika_hal_SOFT_IIC_config* cfg) {
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

/* WIFI utils */
int pika_hal_utils_WIFI_config(pika_dev* dev, pika_hal_WIFI_config* cfg) {
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

/* SG utils */
int pika_hal_utils_SG_config(pika_dev* dev, pika_hal_SG_config* cfg) {
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

pika_float pika_hal_utils_SG_read(pika_dev* dev) {
    pika_float val = 0;
    int ret = pika_hal_read(dev, &val, sizeof(pika_float));
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
        return -1;
    }
    return val;
}
