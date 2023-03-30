#include "../PikaStdDevice/pika_hal.h"

static int _GPIO_write(pika_dev* dev, uint32_t val) {
    return pika_hal_write(dev, &val, sizeof(val));
}

static uint32_t _GPIO_read(pika_dev* dev) {
    uint32_t val = 0;
    pika_hal_read(dev, &val, sizeof(val));
    return val;
}

static void _IIC_Delay(void) {
    // Delay implementation, can be modified based on hardware platform.
    // You may need to adjust the delay time to match your hardware.
}

static void _IIC_Start(pika_hal_SOFT_IIC_config* cfg) {
    _GPIO_write(cfg->SDA, 1);
    _GPIO_write(cfg->SCL, 1);
    _IIC_Delay();
    _GPIO_write(cfg->SDA, 0);
    _IIC_Delay();
    _GPIO_write(cfg->SCL, 0);
}

static void _IIC_Stop(pika_hal_SOFT_IIC_config* cfg) {
    _GPIO_write(cfg->SDA, 0);
    _GPIO_write(cfg->SCL, 1);
    _IIC_Delay();
    _GPIO_write(cfg->SDA, 1);
    _IIC_Delay();
}

static void _IIC_SendByte(pika_hal_SOFT_IIC_config* cfg, uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        _GPIO_write(cfg->SCL, 0);
        _IIC_Delay();
        if (byte & 0x80) {
            _GPIO_write(cfg->SDA, 1);
        } else {
            _GPIO_write(cfg->SDA, 0);
        }
        _IIC_Delay();
        _GPIO_write(cfg->SCL, 1);
        _IIC_Delay();
        byte <<= 1;
    }
    _GPIO_write(cfg->SCL, 0);
}

static uint8_t _IIC_ReadByte(pika_hal_SOFT_IIC_config* cfg, uint8_t ack) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        _GPIO_write(cfg->SCL, 1);
        _IIC_Delay();
        byte <<= 1;
        if (_GPIO_read(cfg->SDA)) {
            byte |= 0x01;
        }
        _GPIO_write(cfg->SCL, 0);
        _IIC_Delay();
    }
    if (ack) {
        _IIC_SendByte(cfg, 0xFF);
    } else {
        _IIC_SendByte(cfg, 0x00);
    }
    return byte;
}

static void set_SDA_input(pika_hal_SOFT_IIC_config* cfg) {
    pika_hal_GPIO_config cfg_SDA = {0};
    cfg_SDA.dir = PIKA_HAL_GPIO_DIR_IN;
    pika_hal_ioctl(cfg->SDA, PIKA_HAL_IOCTL_CONFIG, &cfg_SDA);
}

static void set_SDA_output(pika_hal_SOFT_IIC_config* cfg) {
    pika_hal_GPIO_config cfg_SDA = {0};
    cfg_SDA.dir = PIKA_HAL_GPIO_DIR_OUT;
    pika_hal_ioctl(cfg->SDA, PIKA_HAL_IOCTL_CONFIG, &cfg_SDA);
}

int pika_hal_platform_SOFT_IIC_write(pika_dev* dev, void* buf, size_t count) {
    pika_hal_SOFT_IIC_config* cfg =
        (pika_hal_SOFT_IIC_config*)dev->ioctl_config;
    uint8_t* data = (uint8_t*)buf;
    set_SDA_output(cfg);
    _IIC_Start(cfg);
    for (int i = 0; i < count; i++) {
        _IIC_SendByte(cfg, data[i]);
    }
    _IIC_Stop(cfg);
    return count;
}

int pika_hal_platform_SOFT_IIC_read(pika_dev* dev, void* buf, size_t count) {
    pika_hal_SOFT_IIC_config* cfg =
        (pika_hal_SOFT_IIC_config*)dev->ioctl_config;
    set_SDA_input(cfg);
    uint8_t* data = (uint8_t*)buf;
    _IIC_Start(cfg);
    for (int i = 0; i < count - 1; i++) {
        data[i] = _IIC_ReadByte(cfg, 1);
    }
    data[count - 1] = _IIC_ReadByte(cfg, 0);
    _IIC_Stop(cfg);
    return count;
}

int pika_hal_platform_SOFT_IIC_open(pika_dev* dev, char* name) {
    return 0;
}

int pika_hal_platform_SOFT_IIC_close(pika_dev* dev) {
    pika_hal_SOFT_IIC_config* cfg =
        (pika_hal_SOFT_IIC_config*)dev->ioctl_config;
    if (cfg->SDA != NULL) {
        pika_hal_close(cfg->SDA);
    }
    if (cfg->SCL != NULL) {
        pika_hal_close(cfg->SCL);
    }
    return 0;
}

int pika_hal_platform_SOFT_IIC_ioctl_config(pika_dev* dev,
                                            pika_hal_SOFT_IIC_config* cfg) {
    if (cfg->SDA == NULL || cfg->SCL == NULL) {
        __platform_printf(
            "Error: SOFT IIC config error, SDA and SCL must be set\r\n");
        return -1;
    }
    // 检查软件IIC配置项是否正确
    if (cfg->master_or_slave != PIKA_HAL_IIC_MASTER &&
        cfg->master_or_slave != PIKA_HAL_IIC_SLAVE) {
        __platform_printf(
            "Error: SOFT IIC config error, master_or_slave must be set\r\n");
        return -1;
    }
    if (cfg->address_width != PIKA_HAL_IIC_ADDRESS_WIDTH_7BIT &&
        cfg->address_width != PIKA_HAL_IIC_ADDRESS_WIDTH_10BIT) {
        __platform_printf(
            "Error: SOFT IIC config error, address_width must be set\r\n");
        return -1;
    }
    if (cfg->mem_addr_ena != PIKA_HAL_IIC_MEM_ADDR_ENA_ENABLE &&
        cfg->mem_addr_ena != PIKA_HAL_IIC_MEM_ADDR_ENA_DISABLE) {
        __platform_printf(
            "Error: SOFT IIC config error, mem_addr_ena must be set\r\n");
        return -1;
    }
    if (cfg->mem_addr_size != PIKA_HAL_IIC_MEM_ADDR_SIZE_8BIT &&
        cfg->mem_addr_size != PIKA_HAL_IIC_MEM_ADDR_SIZE_16BIT) {
        __platform_printf(
            "Error: SOFT IIC config error, mem_addr_size must be set\r\n");
        return -1;
    }
    // 在这里，我们暂时不检查speed和timeout，因为软件模拟的I2C可能无法精确控制速度和超时。
    return 0;
}

int pika_hal_platform_SOFT_IIC_ioctl_enable(pika_dev* dev) {
    pika_hal_SOFT_IIC_config* cfg =
        (pika_hal_SOFT_IIC_config*)dev->ioctl_config;
    if (cfg->SDA == NULL || cfg->SCL == NULL) {
        __platform_printf(
            "Error: SOFT IIC config error, SDA and SCL must be set\r\n");
        return -1;
    }
    pika_hal_GPIO_config cfg_SDA = {0};
    pika_hal_GPIO_config cfg_SCL = {0};
    cfg_SDA.dir = PIKA_HAL_GPIO_DIR_OUT;
    cfg_SCL.dir = PIKA_HAL_GPIO_DIR_OUT;
    pika_hal_ioctl(cfg->SDA, PIKA_HAL_IOCTL_CONFIG, &cfg_SDA);
    pika_hal_ioctl(cfg->SCL, PIKA_HAL_IOCTL_CONFIG, &cfg_SCL);

    pika_hal_ioctl(cfg->SDA, PIKA_HAL_IOCTL_ENABLE);
    pika_hal_ioctl(cfg->SCL, PIKA_HAL_IOCTL_ENABLE);

    _GPIO_write(cfg->SDA, 1);
    _GPIO_write(cfg->SCL, 1);
    return 0;
}

int pika_hal_platform_SOFT_IIC_ioctl_disable(pika_dev* dev) {
    pika_hal_SOFT_IIC_config* cfg =
        (pika_hal_SOFT_IIC_config*)dev->ioctl_config;
    pika_hal_ioctl(cfg->SDA, PIKA_HAL_IOCTL_DISABLE);
    pika_hal_ioctl(cfg->SCL, PIKA_HAL_IOCTL_DISABLE);
    return 0;
}
