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
} PIKA_HAL_DEV_TYPE;

typedef struct {
    PIKA_HAL_DEV_TYPE type;
    void* ioctl_config;
    void* platform_data;
} pika_dev;

typedef enum {
    PIKA_HAL_IOCTL_CONFIG,
    PIKA_HAL_IOCTL_ENABLE,
    PIKA_HAL_IOCTL_DISABLE,
} PIKA_HAL_IOCTL_CMD;

/* posix file like API */
pika_dev* pika_hal_open(PIKA_HAL_DEV_TYPE dev_type, char* name);
int pika_hal_close(pika_dev* dev);
int pika_hal_read(pika_dev* dev, void* buf, size_t len);
int pika_hal_write(pika_dev* dev, void* buf, size_t len);
int pika_hal_ioctl(pika_dev* dev, PIKA_HAL_IOCTL_CMD cmd, ...);

typedef enum {
    _PIKA_HAL_GPIO_DIR_UNUSED = 0,
    PIKA_HAL_GPIO_DIR_IN,
    PIKA_HAL_GPIO_DIR_OUT,
} PIKA_HAL_GPIO_DIR;

typedef enum {
    _PIKA_HAL_GPIO_PULL_UNUSED = 0,
    PIKA_HAL_GPIO_PULL_NONE,
    PIKA_HAL_GPIO_PULL_UP,
    PIKA_HAL_GPIO_PULL_DOWN,
} PIKA_HAL_GPIO_PULL;

typedef enum {
    _PIKA_HAL_GPIO_SPEED_UNUSED = 0,
    PIKA_HAL_GPIO_SPEED_1M = 1000000,
    PIKA_HAL_GPIO_SPEED_2M = 2000000,
    PIKA_HAL_GPIO_SPEED_5M = 5000000,
    PIKA_HAL_GPIO_SPEED_10M = 10000000,
    PIKA_HAL_GPIO_SPEED_20M = 20000000,
    PIKA_HAL_GPIO_SPEED_50M = 50000000,
    PIKA_HAL_GPIO_SPEED_100M = 100000000,
} PIKA_HAL_GPIO_SPEED;

typedef struct {
    PIKA_HAL_GPIO_DIR dir;
    PIKA_HAL_GPIO_PULL pull;
    PIKA_HAL_GPIO_SPEED speed;
    void (*event_callback_rising)(pika_dev* dev);
    void (*event_callback_falling)(pika_dev* dev);
} pika_hal_GPIO_config;

typedef enum {
    _PIKA_HAL_UART_BAUDRATE_UNUSED = 0,
    PIKA_HAL_UART_BAUDRATE_9600 = 9600,
    PIKA_HAL_UART_BAUDRATE_19200 = 19200,
    PIKA_HAL_UART_BAUDRATE_38400 = 38400,
    PIKA_HAL_UART_BAUDRATE_57600 = 57600,
    PIKA_HAL_UART_BAUDRATE_115200 = 115200,
    PIKA_HAL_UART_BAUDRATE_2000000 = 2000000,
    PIKA_HAL_UART_BAUDRATE_10000000 = 10000000,
} PIKA_HAL_UART_BAUDRATE;

typedef enum {
    _PIKA_HAL_UART_DATA_BITS_UNUSED = 0,
    PIKA_HAL_UART_DATA_BITS_5 = 5,
    PIKA_HAL_UART_DATA_BITS_6 = 6,
    PIKA_HAL_UART_DATA_BITS_7 = 7,
    PIKA_HAL_UART_DATA_BITS_8 = 8,
} PIKA_HAL_UART_DATA_BITS;

typedef enum {
    _PIKA_HAL_UART_STOP_BITS_UNUSED = 0,
    PIKA_HAL_UART_STOP_BITS_1 = 1,
    PIKA_HAL_UART_STOP_BITS_2 = 2,
} PIKA_HAL_UART_STOP_BITS;

typedef enum {
    _PIKA_HAL_UART_PARITY_UNUSED = 0,
    PIKA_HAL_UART_PARITY_NONE,
    PIKA_HAL_UART_PARITY_ODD,
    PIKA_HAL_UART_PARITY_EVEN,
} PIKA_HAL_UART_PARITY;

typedef struct {
    PIKA_HAL_UART_BAUDRATE baudrate;
    PIKA_HAL_UART_DATA_BITS data_bits;
    PIKA_HAL_UART_STOP_BITS stop_bits;
    PIKA_HAL_UART_PARITY parity;
    void (*event_callback_rx)(pika_dev* dev);
} pika_hal_UART_config;

typedef uint32_t PIKA_HAL_IIC_SLAVE_ADDR;

typedef enum {
    _PIKA_HAL_IIC_SPEED_UNUSED = 0,
    PIKA_HAL_IIC_SPEED_100K = 100000,
    PIKA_HAL_IIC_SPEED_400K = 400000,
    PIKA_HAL_IIC_SPEED_1M = 1000000,
} PIKA_HAL_IIC_SPEED;

typedef struct {
    PIKA_HAL_IIC_SLAVE_ADDR addr;
    PIKA_HAL_IIC_SPEED speed;
} pika_hal_IIC_config;

typedef enum {
    _PIKA_HAL_SPI_LSB_OR_MSB_UNUSED = 0,
    PIKA_HAL_SPI_LSB,
    PIKA_HAL_SPI_MSB,
} PIKA_HAL_SPI_LSB_OR_MSB;

typedef enum {
    _PIKA_HAL_SPI_MASTER_OR_SLAVE_UNUSED = 0,
    PIKA_HAL_SPI_MASTER,
    PIKA_HAL_SPI_SLAVE,
} PIKA_HAL_SPI_MASTER_OR_SLAVE;

typedef enum {
    _PIKA_HAL_SPI_MODE_UNUSED = 0,
    PIKA_HAL_SPI_MODE_0,
    PIKA_HAL_SPI_MODE_1,
    PIKA_HAL_SPI_MODE_2,
    PIKA_HAL_SPI_MODE_3,
} PIKA_HAL_SPI_MODE;

typedef enum {
    _PIKA_HAL_SPI_DATA_UNUSED = 0,
    PIKA_HAL_SPI_DATA_WIDTH_8 = 8,
    PIKA_HAL_SPI_DATA_WIDTH_16 = 16,
} PIKA_HAL_SPI_DATA_WIDTH;

typedef enum {
    _PIKA_HAL_SPI_SPEED_UNUSED = 0,
    PIKA_HAL_SPI_SPEED_1M = 1000000,
    PIKA_HAL_SPI_SPEED_2M = 2000000,
    PIKA_HAL_SPI_SPEED_5M = 5000000,
    PIKA_HAL_SPI_SPEED_10M = 10000000,
    PIKA_HAL_SPI_SPEED_20M = 20000000,
    PIKA_HAL_SPI_SPEED_50M = 50000000,
    PIKA_HAL_SPI_SPEED_100M = 100000000,
} PIKA_HAL_SPI_SPEED;

typedef struct {
    PIKA_HAL_SPI_LSB_OR_MSB lsb_or_msb;
    PIKA_HAL_SPI_MASTER_OR_SLAVE master_or_slave;
    PIKA_HAL_SPI_MODE mode;
    PIKA_HAL_SPI_DATA_WIDTH data_width;
    PIKA_HAL_SPI_SPEED speed;
} pika_hal_SPI_config;

typedef enum {
    _PIKA_HAL_ADC_CHANNEL_UNUSED = 0,
    PIKA_HAL_ADC_CHANNEL_0,
    PIKA_HAL_ADC_CHANNEL_1,
    PIKA_HAL_ADC_CHANNEL_2,
    PIKA_HAL_ADC_CHANNEL_3,
    PIKA_HAL_ADC_CHANNEL_4,
    PIKA_HAL_ADC_CHANNEL_5,
    PIKA_HAL_ADC_CHANNEL_6,
    PIKA_HAL_ADC_CHANNEL_7,
    PIKA_HAL_ADC_CHANNEL_TEMP,
    PIKA_HAL_ADC_CHANNEL_VBAT,
} PIKA_HAL_ADC_CHANNEL;

typedef enum {
    _PIKA_HAL_ADC_RESOLUTION_UNUSED = 0,
    PIKA_HAL_ADC_RESOLUTION_8 = 8,
    PIKA_HAL_ADC_RESOLUTION_10 = 10,
    PIKA_HAL_ADC_RESOLUTION_12 = 12,
    PIKA_HAL_ADC_RESOLUTION_14 = 14,
    PIKA_HAL_ADC_RESOLUTION_16 = 16,
} PIKA_HAL_ADC_RESOLUTION;

typedef struct {
    PIKA_HAL_ADC_CHANNEL channel;
    PIKA_HAL_ADC_RESOLUTION resolution;
} pika_hal_ADC_config;

typedef enum {
    _PIKA_HAL_DAC_UNUSED = 0,
    PIKA_HAL_PWM_CHANNEL_0,
    PIKA_HAL_PWM_CHANNEL_1,
    PIKA_HAL_PWM_CHANNEL_2,
    PIKA_HAL_PWM_CHANNEL_3,
    PIKA_HAL_PWM_CHANNEL_4,
    PIKA_HAL_PWM_CHANNEL_5,
    PIKA_HAL_PWM_CHANNEL_6,
    PIKA_HAL_PWM_CHANNEL_7,
} PIKA_HAL_PWM_CHANNEL;

typedef enum {
    PIKA_HAL_PWM_PERIOD_1NS = 1,
    PIKA_HAL_PWM_PERIOD_1US = 1000,
    PIKA_HAL_PWM_PERIOD_1MS = 1000000,
    PIKA_HAL_PWM_PERIOD_1S = 1000000000,
} PIKA_HAL_PWM_PERIOD;

typedef enum {
    PIKA_HAL_PWM_DUTY_1NS = 1,
    PIKA_HAL_PWM_DUTY_1US = 1000,
    PIKA_HAL_PWM_DUTY_1MS = 1000000,
    PIKA_HAL_PWM_DUTY_1S = 1000000000,
} PIKA_HAL_PWM_DUTY;

typedef struct {
    PIKA_HAL_PWM_CHANNEL channel;
    PIKA_HAL_PWM_PERIOD period;
    PIKA_HAL_PWM_DUTY duty;
} pika_hal_PWM_config;

typedef struct pika_dev_impl {
    int (*open)(pika_dev* dev, char* name);
    int (*close)(pika_dev* dev);
    int (*read)(pika_dev* dev, void* buf, size_t count);
    int (*write)(pika_dev* dev, void* buf, size_t count);
    int (*ioctl)(pika_dev* dev, PIKA_HAL_IOCTL_CMD cmd, void* cfg);
} pika_dev_impl;

/* platform API */
#define PIKA_HAL_TABLE_PLATFORM_API
#include "pika_hal_table.h"

/* config merge headers */
#define PIKA_HAL_TABLE_IOCTL_MERGE_CONFIG_HEADER
#include "pika_hal_table.h"

#endif
