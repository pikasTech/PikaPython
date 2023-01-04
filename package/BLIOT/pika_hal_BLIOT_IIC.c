#include <hosal_i2c.h>
#include "../PikaStdDevice/pika_hal.h"

int _num2scl_sda(int num, uint8_t* scl, uint8_t* sda) {
    /**********   BL602  ************
     *    IIC0  ----->     SCL:P0, SDA: P1
     *    IIC1  ----->     SCL:P2, SDA: P3
     *    IIC2  ----->     SCL:P4, SDA: P5
     *    IIC3  ----->     SCL:P12, SDA: P11
     *    IIC4  ----->     SCL:P14, SDA: P17
     *    IIC5  ----->     SCL:P20, SDA: P21
     */
    switch (num) {
        case 0:
            *scl = 0;
            *sda = 1;
            break;
        case 1:
            *scl = 2;
            *sda = 3;
            break;
        case 2:
            *scl = 4;
            *sda = 5;
            break;
        case 3:
            *scl = 12;
            *sda = 11;
            break;
        case 4:
            *scl = 14;
            *sda = 17;
            break;
        case 5:
            *scl = 20;
            *sda = 21;
            break;
        default:
            return -1;
    }
#if PIKA_DEBUG_ENABLE
    __platform_printf("IIC: Open IIC%d on SCL:P%d, SDA:P%d\r\n", num, *scl,
                      *sda);
#endif
    return 0;
}

int pika_hal_platform_IIC_open(pika_dev* dev, char* name) {
    hosal_i2c_dev_t* platform_i2c = pikaMalloc(sizeof(hosal_i2c_dev_t));
    memset(platform_i2c, 0, sizeof(hosal_i2c_dev_t));
    dev->platform_data = platform_i2c;
    /* Support IIC0, IIC1, IIC2 */
    if (name[0] == 'I' && name[1] == 'I' && name[2] == 'C') {
        int iic_num = fast_atoi(name + 3);
        platform_i2c->port = 0;
        if (0 == _num2scl_sda(iic_num, &platform_i2c->config.scl,
                              &platform_i2c->config.sda)) {
            return 0;
        } else {
            __platform_printf("IIC: Open IIC%d failed\r\n", iic_num);
            return -1;
        }
    }
    return -1;
}

int pika_hal_platform_IIC_close(pika_dev* dev) {
    hosal_i2c_dev_t* platform_i2c = (hosal_i2c_dev_t*)dev->platform_data;
    if (NULL != platform_i2c) {
        pikaFree(platform_i2c, sizeof(hosal_i2c_dev_t));
    }
    return 0;
}

int pika_hal_platform_IIC_ioctl_config(pika_dev* dev,
                                       pika_hal_IIC_config* cfg) {
    hosal_i2c_dev_t* platform_i2c = (hosal_i2c_dev_t*)dev->platform_data;
    if (!dev->is_enabled) {
        platform_i2c->config.freq = cfg->speed;
        switch (cfg->address_width) {
            case PIKA_HAL_IIC_ADDRESS_WIDTH_7BIT:
                platform_i2c->config.address_width =
                    HOSAL_I2C_ADDRESS_WIDTH_7BIT;
                break;
            case PIKA_HAL_IIC_ADDRESS_WIDTH_10BIT:
                platform_i2c->config.address_width =
                    HOSAL_I2C_ADDRESS_WIDTH_10BIT;
                break;
            default:
                return -1;
        }
        switch (cfg->master_or_slave) {
            case PIKA_HAL_IIC_MASTER:
                platform_i2c->config.mode = HOSAL_I2C_MODE_MASTER;
                break;
            case PIKA_HAL_IIC_SLAVE:
                platform_i2c->config.mode = HOSAL_I2C_MODE_SLAVE;
                break;
            default:
                return -1;
        }
#if PIKA_DEBUG_ENABLE
        __platform_printf(
            "IIC: Config IIC%d, speed:%d, address_width:%d,master_or_slave: "
            "%d\r\n",
            platform_i2c->port, platform_i2c->config.freq,
            platform_i2c->config.address_width, platform_i2c->config.mode);
#endif
    }
    return 0;
}

int pika_hal_platform_IIC_ioctl_enable(pika_dev* dev) {
    if (!dev->is_enabled) {
        hosal_i2c_dev_t* platform_i2c = (hosal_i2c_dev_t*)dev->platform_data;
#if PIKA_DEBUG_ENABLE
        __platform_printf("IIC: Enable IIC%d on SCL:P%d, SDA:P%d\r\n",
                          platform_i2c->port, platform_i2c->config.scl,
                          platform_i2c->config.sda);
#endif
        hosal_i2c_init(platform_i2c);
        return 0;
    }
    return -1;
}

int pika_hal_platform_IIC_ioctl_disable(pika_dev* dev) {
    if (dev->is_enabled) {
        hosal_i2c_dev_t* platform_i2c = (hosal_i2c_dev_t*)dev->platform_data;
        hosal_i2c_finalize(platform_i2c);
        return 0;
    }
    return -1;
}

int pika_hal_platform_IIC_write(pika_dev* dev, void* buf, size_t count) {
    hosal_i2c_dev_t* platform_i2c = (hosal_i2c_dev_t*)dev->platform_data;
    pika_hal_IIC_config* cfg = (pika_hal_IIC_config*)dev->ioctl_config;
    if (cfg->mem_addr_ena == PIKA_HAL_IIC_MEM_ADDR_ENA_DISABLE) {
#if PIKA_DEBUG_ENABLE
        __platform_printf("IIC: Write %d bytes to 0x%02x\r\n", count,
                          cfg->slave_addr);
#endif
        return hosal_i2c_master_send(platform_i2c, cfg->slave_addr, buf, count,
                                     cfg->timeout);
    } else {
#if PIKA_DEBUG_ENABLE
        __platform_printf("IIC: Write %d bytes to 0x%02x, mem_addr:0x%02x\r\n",
                          count, cfg->slave_addr, cfg->mem_addr);
#endif
        return hosal_i2c_mem_write(platform_i2c, cfg->slave_addr, cfg->mem_addr,
                                   cfg->mem_addr_size, buf, count,
                                   cfg->timeout);
    }
}

int pika_hal_platform_IIC_read(pika_dev* dev, void* buf, size_t count) {
    hosal_i2c_dev_t* platform_i2c = (hosal_i2c_dev_t*)dev->platform_data;
    pika_hal_IIC_config* cfg = (pika_hal_IIC_config*)dev->ioctl_config;
    if (cfg->mem_addr_ena == PIKA_HAL_IIC_MEM_ADDR_ENA_DISABLE) {
#if PIKA_DEBUG_ENABLE
        __platform_printf("IIC: Read %d bytes from 0x%02x\r\n", count,
                          cfg->slave_addr);
#endif
        return hosal_i2c_master_recv(platform_i2c, cfg->slave_addr, buf, count,
                                     cfg->timeout);
    } else {
#if PIKA_DEBUG_ENABLE
        __platform_printf("IIC: Read %d bytes from 0x%02x, mem_addr:0x%02x\r\n",
                          count, cfg->slave_addr, cfg->mem_addr);
#endif
        return hosal_i2c_mem_read(platform_i2c, cfg->slave_addr, cfg->mem_addr,
                                  cfg->mem_addr_size, buf, count, cfg->timeout);
    }
}
