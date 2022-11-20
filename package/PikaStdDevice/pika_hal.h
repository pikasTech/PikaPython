#ifndef _PIKA_DEV_HAL_H
#define _PIKA_DEV_HAL_H
#include <stddef.h>
#include <stdint.h>

/*
 * pika_dev_hal is a C HAL lib for PikaScript modules.
 * the api of pika_dev_hal is similar to the api of posix file API.
 */
typedef enum pika_dev_cmd pika_dev_cmd;
typedef struct pika_dev pika_dev;

typedef enum pika_dev_type {
    PIKA_DEV_TYPE_NONE = 0,
    PIKA_DEV_TYPE_GPIO,
    PIKA_DEV_TYPE_UART,
    PIKA_DEV_TYPE_I2C,
    PIKA_DEV_TYPE_SPI,
    PIKA_DEV_TYPE_PWM,
    PIKA_DEV_TYPE_ADC,
    PIKA_DEV_TYPE_DAC,
    PIKA_DEV_TYPE_MAX,
} pika_dev_type;

/* posix file like API */
pika_dev* pika_hal_open(pika_dev_type dev_type, char* name);
int pika_hal_close(pika_dev* dev);
int pika_hal_read(pika_dev* dev, uint8_t* buf, size_t len);
int pika_hal_write(pika_dev* dev, uint8_t* buf, size_t len);
int pika_hal_ioctl(pika_dev* dev, pika_dev_cmd cmd, ...);

typedef enum pika_dev_cmd {
    PIKA_DEV_CMD_ENABLE,
    PIKA_DEV_CMD_DISABLE,
    PIKA_DEV_CMD_SET_CONFIG,
    PIKA_DEV_CMD_GET_CONFIG,
    PIKA_DEV_CMD_MAX,
} pika_dev_cmd;

typedef enum pika_dev_gpio_mode {
    PIKA_DEV_GPIO_MODE_INPUT,
    PIKA_DEV_GPIO_MODE_OUTPUT,
    PIKA_DEV_GPIO_MODE_MAX,
} pika_dev_gpio_mode;

typedef enum pika_dev_gpio_pull {
    PIKA_DEV_GPIO_PULL_NONE,
    PIKA_DEV_GPIO_PULL_UP,
    PIKA_DEV_GPIO_PULL_DOWN,
    PIKA_DEV_GPIO_PULL_MAX,
} pika_dev_gpio_pull;

typedef struct pika_dev_impl {
    int (*open)(pika_dev* dev);
    int (*close)(pika_dev* dev);
    int (*read)(pika_dev* dev, uint8_t* buf, size_t count);
    int (*write)(pika_dev* dev, uint8_t* buf, size_t count);
    int (*ioctl)(pika_dev* dev, pika_dev_cmd cmd, uintptr_t cfg);
} pika_dev_impl;

struct pika_dev {
    pika_dev_type type;
};

#endif
