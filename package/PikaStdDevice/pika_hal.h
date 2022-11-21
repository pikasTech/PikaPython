#ifndef _PIKA_DEV_HAL_H
#define _PIKA_DEV_HAL_H
#include <stddef.h>
#include <stdint.h>

/*
 * pika_hal is a C Device HAL lib for PikaScript modules.
 * the API of pika_hal is similar to the posix file API.
 */

typedef enum {
#define PIKA_HAL_TABLE_DEV_TYPE
#include "pika_hal_table.h"
    _PIKA_DEV_TYPE_MAX,
} PIKA_HAL_DEV_TYPE;

typedef enum {
    PIKA_HAL_IOCTL_ENABLE,
    PIKA_HAL_IOCTL_DISABLE,
    PIKA_HAL_IOCTL_CONFIG,
    _PIKA_HAL_IOCTL_MAX,
} PIKA_HAL_IOCTL_CMD;

typedef enum {
    PIKA_HAL_GPIO_DIR_IN,
    PIKA_HAL_GPIO_DIR_OUT,
    _PIKA_HAL_GPIO_DIR_UNUSED = 0,
    _PIKA_HAL_GPIO_DIR_MAX,
} PIKA_HAL_GPIO_DIR;

typedef enum {
    PIKA_HAL_GPIO_PULL_NONE,
    PIKA_HAL_GPIO_PULL_UP,
    PIKA_HAL_GPIO_PULL_DOWN,
    _PIKA_HAL_GPIO_PULL_UNUSED = 0,
    _PIKA_HAL_GPIO_PULL_MAX,
} PIKA_HAL_GPIO_PULL;

typedef struct {
    PIKA_HAL_GPIO_DIR dir;
    PIKA_HAL_GPIO_PULL pull;
} pika_hal_GPIO_config;

typedef struct {
} pika_hal_UART_config;

typedef struct {
} pika_hal_I2C_config;

typedef enum {
    PIKA_HAL_SPI_LSB,
    PIKA_HAL_SPI_MSB,
    _PIKA_HAL_SPI_LSB_OR_MSB_UNUSED = 0,
    _PIKA_HAL_SPI_LSB_OR_MSB_MAX,
} PIKA_HAL_SPI_LSB_OR_MSB;

typedef enum {
    PIKA_HAL_SPI_MASTER,
    PIKA_HAL_SPI_SLAVE,
    _PIKA_HAL_SPI_MASTER_OR_SLAVE_UNUSED = 0,
    _PIKA_HAL_SPI_MASTER_OR_SLAVE_MAX,
} PIKA_HAL_SPI_MASTER_OR_SLAVE;

typedef enum {
    PIKA_HAL_SPI_MODE_0,
    PIKA_HAL_SPI_MODE_1,
    PIKA_HAL_SPI_MODE_2,
    PIKA_HAL_SPI_MODE_3,
    _PIKA_HAL_SPI_MODE_UNUSED = 0,
    _PIKA_HAL_SPI_MODE_MAX,
} PIKA_HAL_SPI_MODE;

typedef enum {
    PIKA_HAL_SPI_DATA_WIDTH_8BIT,
    PIKA_HAL_SPI_DATA_WIDTH_16BIT,
    _PIKA_HAL_SPI_DATA_UNUSED = 0,
    _PIKA_HAL_SPI_DATA_WIDTH_MAX,
} PIKA_HAL_SPI_DATA_WIDTH;

typedef enum {
    PIKA_HAL_SPI_SPEED_LOW,
    PIKA_HAL_SPI_SPEED_MEDIUM,
    PIKA_HAL_SPI_SPEED_HIGH,
    PIKA_HAL_SPI_SPEED_VERY_HIGH,
    _PIKA_HAL_SPI_SPEED_UNUSED = 0,
    _PIKA_HAL_SPI_SPEED_MAX,
} PIKA_HAL_SPI_SPEED;

typedef struct {
    PIKA_HAL_SPI_LSB_OR_MSB lsb_or_msb;
    PIKA_HAL_SPI_MASTER_OR_SLAVE master_or_slave;
    PIKA_HAL_SPI_MODE mode;
    PIKA_HAL_SPI_DATA_WIDTH data_width;
    PIKA_HAL_SPI_SPEED speed;
} pika_hal_SPI_config;

typedef struct {
} pika_hal_ADC_config;

typedef struct {
} pika_hal_PWM_config;

typedef struct {
    PIKA_HAL_DEV_TYPE type;
    void* platform_data;
} pika_dev;

typedef struct pika_dev_impl {
    int (*open)(pika_dev* dev, char* name);
    int (*close)(pika_dev* dev);
    int (*read)(pika_dev* dev, void* buf, size_t count);
    int (*write)(pika_dev* dev, void* buf, size_t count);
    int (*ioctl)(pika_dev* dev, PIKA_HAL_IOCTL_CMD cmd, uintptr_t cfg);
} pika_dev_impl;

/* posix file like API */
pika_dev* pika_hal_open(PIKA_HAL_DEV_TYPE dev_type, char* name);
int pika_hal_close(pika_dev* dev);
int pika_hal_read(pika_dev* dev, void* buf, size_t len);
int pika_hal_write(pika_dev* dev, void* buf, size_t len);
int pika_hal_ioctl(pika_dev* dev, PIKA_HAL_IOCTL_CMD cmd, ...);

/* platform API */
#define PIKA_HAL_TABLE_PLATFORM_API
#include "pika_hal_table.h"

#endif
