#include "../PikaStdDevice/pika_hal.h"

int pika_hal_platform_GPIO_open(pika_dev* dev, char* name) {
    return -1;
}

int pika_hal_platform_GPIO_close(pika_dev* dev) {
    return -1;
}

int pika_hal_platform_GPIO_read(pika_dev* dev, void* buf, size_t count) {
    return -1;
}

int pika_hal_platform_GPIO_write(pika_dev* dev, void* buf, size_t count) {
    return -1;
}

int pika_hal_platform_GPIO_ioctl_enable(pika_dev* dev) {
    return -1;
}

int pika_hal_platform_GPIO_ioctl_disable(pika_dev* dev) {
    return -1;
}

int pika_hal_platform_GPIO_ioctl_config(pika_dev* dev,
                                        pika_hal_GPIO_config* cfg) {
    return -1;
}
