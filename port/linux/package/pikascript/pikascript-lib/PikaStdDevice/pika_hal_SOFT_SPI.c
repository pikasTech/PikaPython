#include "../PikaStdDevice/pika_hal.h"

static int _GPIO_write(pika_dev* dev, uint32_t val) {
    return pika_hal_write(dev, &val, sizeof(val));
}

static uint32_t _GPIO_read(pika_dev* dev) {
    uint32_t val = 0;
    pika_hal_read(dev, &val, sizeof(val));
    return val;
}

int pika_hal_platform_SOFT_SPI_open(pika_dev* dev, char* name) {
    return 0;
}

int pika_hal_platform_SOFT_SPI_close(pika_dev* dev) {
    pika_hal_SOFT_SPI_config* cfg =
        (pika_hal_SOFT_SPI_config*)dev->ioctl_config;
    if (cfg->CS != NULL) {
        pika_hal_close(cfg->CS);
    }
    if (cfg->SCK != NULL) {
        pika_hal_close(cfg->SCK);
    }
    if (cfg->MOSI != NULL) {
        pika_hal_close(cfg->MOSI);
    }
    if (cfg->MISO != NULL) {
        pika_hal_close(cfg->MISO);
    }
    return 0;
}

int pika_hal_platform_SOFT_SPI_ioctl_config(pika_dev* dev,
                                            pika_hal_SOFT_SPI_config* cfg) {
    if (cfg->SCK == NULL || cfg->MOSI == NULL) {
        __platform_printf(
            "Error: SOFT SPI config error, CS, SCK, MOSI, MISO must be "
            "set\r\n");
        return -1;
    }
    return 0;
}

int pika_hal_platform_SOFT_SPI_ioctl_enable(pika_dev* dev) {
    pika_hal_SOFT_SPI_config* cfg =
        (pika_hal_SOFT_SPI_config*)dev->ioctl_config;
    if (cfg->SCK == NULL || cfg->MOSI == NULL) {
        __platform_printf(
            "Error: SOFT SPI config error, CS, SCK, MOSI, MISO must be "
            "set\r\n");
        return -1;
    }
    pika_hal_GPIO_config cfg_CS = {0};
    pika_hal_GPIO_config cfg_SCK = {0};
    pika_hal_GPIO_config cfg_MOSI = {0};
    pika_hal_GPIO_config cfg_MISO = {0};
    cfg_CS.dir = PIKA_HAL_GPIO_DIR_OUT;
    cfg_SCK.dir = PIKA_HAL_GPIO_DIR_OUT;
    cfg_MOSI.dir = PIKA_HAL_GPIO_DIR_OUT;
    cfg_MISO.dir = PIKA_HAL_GPIO_DIR_IN;

    if (NULL != cfg->CS) {
        pika_hal_ioctl(cfg->CS, PIKA_HAL_IOCTL_CONFIG, &cfg_CS);
    }
    pika_hal_ioctl(cfg->SCK, PIKA_HAL_IOCTL_CONFIG, &cfg_SCK);
    pika_hal_ioctl(cfg->MOSI, PIKA_HAL_IOCTL_CONFIG, &cfg_MOSI);
    if (NULL != cfg->MISO) {
        pika_hal_ioctl(cfg->MISO, PIKA_HAL_IOCTL_CONFIG, &cfg_MISO);
    }

    if (NULL != cfg->CS) {
        pika_hal_ioctl(cfg->CS, PIKA_HAL_IOCTL_ENABLE);
    }
    pika_hal_ioctl(cfg->SCK, PIKA_HAL_IOCTL_ENABLE);
    pika_hal_ioctl(cfg->MOSI, PIKA_HAL_IOCTL_ENABLE);
    if (NULL != cfg->MISO) {
        pika_hal_ioctl(cfg->MISO, PIKA_HAL_IOCTL_ENABLE);
    }

    if (NULL != cfg->CS) {
        _GPIO_write(cfg->CS, 1);
    }
    _GPIO_write(cfg->SCK, 1);
    _GPIO_write(cfg->MOSI, 1);
    return 0;
}

int pika_hal_platform_SOFT_SPI_ioctl_disable(pika_dev* dev) {
    pika_hal_SOFT_SPI_config* cfg =
        (pika_hal_SOFT_SPI_config*)dev->ioctl_config;
    if (NULL != cfg->CS) {
        pika_hal_ioctl(cfg->CS, PIKA_HAL_IOCTL_DISABLE);
    }
    pika_hal_ioctl(cfg->SCK, PIKA_HAL_IOCTL_DISABLE);
    pika_hal_ioctl(cfg->MOSI, PIKA_HAL_IOCTL_DISABLE);
    if (NULL != cfg->MISO) {
        pika_hal_ioctl(cfg->MISO, PIKA_HAL_IOCTL_DISABLE);
    }
    return 0;
}

static inline int SPIv_WriteData(pika_hal_SOFT_SPI_config* cfg, uint8_t Data) {
    unsigned char i = 0;
    for (i = 8; i > 0; i--) {
        if (Data & 0x80)
            _GPIO_write(cfg->MOSI, 1);
        else
            _GPIO_write(cfg->MOSI, 0);
        _GPIO_write(cfg->SCK, 0);
        _GPIO_write(cfg->SCK, 1);
        Data <<= 1;
    }
    return 0;
}

int pika_hal_platform_SOFT_SPI_write(pika_dev* dev, void* buf, size_t count) {
    pika_hal_SOFT_SPI_config* cfg =
        (pika_hal_SOFT_SPI_config*)dev->ioctl_config;
    uint8_t* data = (uint8_t*)buf;
    if (NULL != cfg->CS) {
        _GPIO_write(cfg->CS, 0);
    }
    for (int i = 0; i < count; i++) {
        SPIv_WriteData(cfg, data[i]);
    }
    if (NULL != cfg->CS) {
        _GPIO_write(cfg->CS, 1);
    }
    return count;
}

int pika_hal_platform_SOFT_SPI_read(pika_dev* dev, void* buf, size_t count) {
    pika_hal_SOFT_SPI_config* cfg =
        (pika_hal_SOFT_SPI_config*)dev->ioctl_config;
    if (NULL == cfg->MISO) {
        __platform_printf("Error: SOFT SPI config error, MISO must be set\r\n");
        return -1;
    }
    uint8_t* data = (uint8_t*)buf;
    if (NULL != cfg->CS) {
        _GPIO_write(cfg->CS, 0);
    }
    for (int i = 0; i < count; i++) {
        data[i] = 0;
        for (int j = 0; j < 8; j++) {
            _GPIO_write(cfg->SCK, 0);
            _GPIO_write(cfg->SCK, 1);
            data[i] |= (_GPIO_read(cfg->MISO) << (7 - j));
        }
    }
    _GPIO_write(cfg->CS, 1);
    return count;
}
