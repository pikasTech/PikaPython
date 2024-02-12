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
    return 0;
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

/* IIC mem read/write utils */
int pika_hal_utils_IIC_mem_read(pika_dev* dev,
                                uint32_t mem_addr,
                                uint32_t mem_addr_size,
                                uint8_t* data,
                                uint32_t size) {
    pika_hal_IIC_config* iic_config = (pika_hal_IIC_config*)dev->ioctl_config;
    iic_config->mem_addr = mem_addr;
    iic_config->mem_addr_size = mem_addr_size;
    iic_config->mem_addr_ena = PIKA_HAL_IIC_MEM_ADDR_ENA_ENABLE;

    if (pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, dev->ioctl_config) < 0) {
        return -1;
    }

    return pika_hal_read(dev, data, size);
}

/* IIC mem read/write utils */
int pika_hal_utils_IIC_mem_write(pika_dev* dev,
                                 uint32_t mem_addr,
                                 uint32_t mem_addr_size,
                                 uint8_t* data,
                                 uint32_t size) {
    pika_hal_IIC_config* iic_config = (pika_hal_IIC_config*)dev->ioctl_config;
    iic_config->mem_addr = mem_addr;
    iic_config->mem_addr_size = mem_addr_size;
    iic_config->mem_addr_ena = PIKA_HAL_IIC_MEM_ADDR_ENA_ENABLE;

    if (pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, dev->ioctl_config) < 0) {
        return -1;
    }

    return pika_hal_write(dev, data, size);
}

int pika_hal_utils_IIC_read(pika_dev* dev, uint8_t* data, uint32_t size) {
    pika_hal_IIC_config* iic_config = (pika_hal_IIC_config*)dev->ioctl_config;
    iic_config->mem_addr_ena = PIKA_HAL_IIC_MEM_ADDR_ENA_DISABLE;
    if (pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, dev->ioctl_config) < 0) {
        return -1;
    }

    return pika_hal_read(dev, data, size);
}

int pika_hal_utils_IIC_write(pika_dev* dev, uint8_t* data, uint32_t size) {
    pika_hal_IIC_config* iic_config = (pika_hal_IIC_config*)dev->ioctl_config;
    iic_config->mem_addr_ena = PIKA_HAL_IIC_MEM_ADDR_ENA_DISABLE;

    if (pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, dev->ioctl_config) < 0) {
        return -1;
    }

    return pika_hal_write(dev, data, size);
}

int pika_hal_utils_IIC_set_slave_addr(pika_dev* dev, uint32_t slave_addr) {
    pika_hal_IIC_config* iic_config = (pika_hal_IIC_config*)dev->ioctl_config;
    iic_config->slave_addr = slave_addr;
    return pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, dev->ioctl_config);
}

/* SPI utils */
int pika_hal_utils_SPI_config(pika_dev* dev, pika_hal_SPI_config* cfg) {
    int ret = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, cfg);
    if (ret < 0) {
        pika_hal_utils_raise_error(ret);
    }
    return ret;
}

int pika_hal_utils_SPI_transfer(pika_dev* dev,
                                uint8_t* txbuff,
                                uint8_t* rxbuff,
                                uint32_t size) {
    // Modify parameter check logic: allow unidirectional transfer
    if (dev == NULL || (txbuff == NULL && rxbuff == NULL)) {
        return -1;  // Parameter check failed
    }

    pika_hal_SPI_config* cfg = (pika_hal_SPI_config*)dev->ioctl_config;
    uint32_t transferred = 0;   // Amount of data transferred
    uint32_t remaining = size;  // Amount of data remaining
    uint32_t chunkSize;         // Amount of data for the current batch transfer

    while (remaining > 0) {
        chunkSize = remaining > PIKA_HAL_SPI_RX_BUFFER_SIZE
                        ? PIKA_HAL_SPI_RX_BUFFER_SIZE
                        : remaining;

        // Call SPI_write to transfer data only if txbuff is not NULL
        if (txbuff != NULL) {
            pika_hal_write(dev, &txbuff[transferred], chunkSize);
        }

        // Call hal_read instead of directly retrieving data from
        // transfer_rx_buffer only if rxbuff is not NULL
        if (rxbuff != NULL) {
            if (txbuff == NULL) {
                // Perform data reception only
                pika_hal_read(dev, &rxbuff[transferred], chunkSize);
            } else {
                // Handle both data sending and receiving, assuming hal_write
                // already includes reading logic
                for (uint32_t i = 0; i < chunkSize; i++) {
                    rxbuff[transferred + i] =
                        cfg->tranfer_rx_buffer[i % PIKA_HAL_SPI_RX_BUFFER_SIZE];
                }
            }
        }

        transferred += chunkSize;  // Update the amount of data transferred
        remaining -= chunkSize;    // Update the amount of data remaining
    }

    return 0;  // Successfully completed the transfer
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
