#include "../PikaStdDevice/pika_hal.h"
int pika_hal_platform_PWM_open(pika_dev* dev, char* name) {
    return -1;
}

int pika_hal_platform_PWM_close(pika_dev* dev) {
    return -1;
}

int pika_hal_platform_PWM_read(pika_dev* dev, void* buf, size_t count) {
    /* not support */
    return -1;
}

int pika_hal_platform_PWM_write(pika_dev* dev, void* buf, size_t count) {
    /* not support */
    return -1;
}

int pika_hal_platform_PWM_ioctl_enable(pika_dev* dev) {
    return -1;
}

int pika_hal_platform_PWM_ioctl_disable(pika_dev* dev) {
    return -1;
}

int pika_hal_platform_PWM_ioctl_config(pika_dev* dev,
                                       pika_hal_PWM_config* cfg) {
    return -1;
}
