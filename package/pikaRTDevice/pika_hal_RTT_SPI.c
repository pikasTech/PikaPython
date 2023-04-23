#include "../PikaStdDevice/pika_hal.h"


int pika_hal_platform_SPI_open(pika_dev* dev, char* name) {
    return -1;
}

int pika_hal_platform_SPI_close(pika_dev* dev) {
    return -1;
}

int pika_hal_platform_SPI_ioctl_config(pika_dev* dev,
                                       pika_hal_SPI_config* cfg) {
    return -1;
}

int pika_hal_platform_SPI_ioctl_enable(pika_dev* dev) {
    return -1;
}

int pika_hal_platform_SPI_ioctl_disable(pika_dev* dev) {
    return -1;
}

int pika_hal_platform_SPI_write(pika_dev* dev, void* buf, size_t count) {
    return -1;
}

int pika_hal_platform_SPI_read(pika_dev* dev, void* buf, size_t count) {
    return -1;
}
