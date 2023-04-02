#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "board.h"
#include "pika_hal_bflb_common.h"

typedef struct platform_data_IIC {
    struct bflb_device_s *i2c_dev;
    uint32_t speed;
} platform_data_IIC;

int pika_hal_platform_IIC_open(pika_dev* dev, char* name) {
    platform_data_IIC *platform_i2c = (platform_data_IIC *)pikaMalloc(sizeof(platform_data_IIC));
    memset(platform_i2c, 0, sizeof(platform_data_IIC));
    dev->platform_data = platform_i2c;

    platform_i2c->i2c_dev = bflb_device_get_by_name(name);
    if (platform_i2c->i2c_dev != NULL) {
        return 0;
    } else {
        return -1;
    }
}

int pika_hal_platform_IIC_close(pika_dev* dev) {
    platform_data_IIC *platform_i2c = (platform_data_IIC *)dev->platform_data;
    if (platform_i2c != NULL) {
        pikaFree(platform_i2c, sizeof(platform_data_IIC));
    }
    return 0;
}

int pika_hal_platform_IIC_ioctl_config(pika_dev* dev, pika_hal_IIC_config* cfg) {
    platform_data_IIC *platform_i2c = (platform_data_IIC *)dev->platform_data;
    platform_i2c->speed = cfg->speed;
    return 0;
}

int pika_hal_platform_IIC_ioctl_enable(pika_dev* dev) {
    platform_data_IIC *platform_i2c = (platform_data_IIC *)dev->platform_data;
    bflb_i2c_init(platform_i2c->i2c_dev, platform_i2c->speed);
    return 0;
}

int pika_hal_platform_IIC_ioctl_disable(pika_dev* dev) {
    platform_data_IIC *platform_i2c = (platform_data_IIC *)dev->platform_data;
    bflb_i2c_deinit(platform_i2c->i2c_dev);
    return 0;
}

int pika_hal_platform_IIC_write(pika_dev* dev, void* buf, size_t count) {
    platform_data_IIC *platform_i2c = (platform_data_IIC *)dev->platform_data;
    pika_hal_IIC_config *cfg = (pika_hal_IIC_config *)dev->ioctl_config;
    struct bflb_i2c_msg_s msg;

    msg.addr = cfg->slave_addr;
    msg.flags = 0;
    msg.buffer = buf;
    msg.length = count;

    return bflb_i2c_transfer(platform_i2c->i2c_dev, &msg, 1);
}

int pika_hal_platform_IIC_read(pika_dev* dev, void* buf, size_t count) {
    platform_data_IIC *platform_i2c = (platform_data_IIC *)dev->platform_data;
    pika_hal_IIC_config *cfg = (pika_hal_IIC_config *)dev->ioctl_config;
    struct bflb_i2c_msg_s msg;msg.addr = cfg->slave_addr;
    msg.flags = I2C_M_READ;
    msg.buffer = buf;
    msg.length = count;
    return bflb_i2c_transfer(platform_i2c->i2c_dev, &msg, 1);
}
