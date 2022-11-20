#ifndef _PIKA_DEV_HAL_H
#define _PIKA_DEV_HAL_H
#include <stddef.h>
#include <stdint.h>

/*
 * pika_hal is a C Device HAL lib for PikaScript modules.
 * the API of pika_hal is similar to the posix file API.
 */

typedef enum pika_dev_type {
#define PIKA_HAL_TABLE_DEV_TYPE
#include "pika_hal_table.h"
    _PIKA_DEV_TYPE_MAX,
} pika_dev_type;

typedef enum pika_ioctl_cmd {
    PIKA_IOCTL_ENABLE,
    PIKA_IOCTL_DISABLE,
    PIKA_IOCTL_CONFIG,
    _PIKA_IOCTL_MAX,
} pika_ioctl_cmd;

typedef enum pika_gpio_dir {
    PIKA_GPIO_DIR_IN,
    PIKA_GPIO_DIR_OUT,
    _PIKA_GPIO_DIR_MAX,
} pika_gpio_dir;

typedef enum pika_gpio_pull {
    PIKA_GPIO_PULL_NONE,
    PIKA_GPIO_PULL_UP,
    PIKA_GPIO_PULL_DOWN,
    _PIKA_GPIO_PULL_MAX,
} pika_gpio_pull;

typedef struct pika_GPIO_config {
    pika_gpio_dir dir;
    pika_gpio_pull pull;
} pika_GPIO_config;

typedef struct pika_UART_config {
} pika_UART_config;

typedef struct pika_I2C_config {
} pika_I2C_config;

typedef struct pika_SPI_config {
} pika_SPI_config;

typedef struct pika_ADC_config {
} pika_ADC_config;

typedef struct pika_PWM_config {
} pika_PWM_config;

typedef struct pika_dev {
    pika_dev_type type;
    void* user_data;
} pika_dev;

typedef struct pika_dev_impl {
    int (*open)(pika_dev* dev, char* name);
    int (*close)(pika_dev* dev);
    int (*read)(pika_dev* dev, void* buf, size_t count);
    int (*write)(pika_dev* dev, void* buf, size_t count);
    int (*ioctl)(pika_dev* dev, pika_ioctl_cmd cmd, uintptr_t cfg);
} pika_dev_impl;

/* posix file like API */
pika_dev* pika_hal_open(pika_dev_type dev_type, char* name);
int pika_hal_close(pika_dev* dev);
int pika_hal_read(pika_dev* dev, void* buf, size_t len);
int pika_hal_write(pika_dev* dev, void* buf, size_t len);
int pika_hal_ioctl(pika_dev* dev, pika_ioctl_cmd cmd, ...);

#endif
