#ifndef _PIKA_DEV_HAL_H
#define _PIKA_DEV_HAL_H
#include <stddef.h>
#include <stdint.h>
#include "PikaObj.h"

/*
 * pika_hal is a C Device HAL lib for PikaScript modules.
 * the API of pika_hal is similar to the posix file API.
 */

#define PIKA_HAL
#ifndef PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
#define PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE 0
#endif

#define PIKA_HAL_OBJ2DEV(_self) obj_getPtr((_self), "pika_dev")

typedef enum {
#define PIKA_HAL_TABLE_DEV_TYPE
#include "pika_hal_table.h"
} PIKA_HAL_DEV_TYPE;

typedef struct {
    PIKA_HAL_DEV_TYPE type;
    PIKA_BOOL is_enabled;
    void* ioctl_config;
    void* platform_data;
} pika_dev;

typedef enum {
    PIKA_HAL_IOCTL_CONFIG,
    PIKA_HAL_IOCTL_ENABLE,
    PIKA_HAL_IOCTL_DISABLE,
    PIKA_HAL_IOCTL_NET_GET_ACTIVE,
    PIKA_HAL_IOCTL_NET_GET_STATUS,
    PIKA_HAL_IOCTL_WIFI_SCAN,
    PIKA_HAL_IOCTL_WIFI_CONNECT,
    PIKA_HAL_IOCTL_WIFI_DISCONNECT,
    PIKA_HAL_IOCTL_NET_SET_IFCONFIG,
    PIKA_HAL_IOCTL_NET_GET_IFCONFIG,
    PIKA_HAL_IOCTL_CAM_CAPTURE,
    _ = 0xFFFFFFFF,  // make sure it is 4 byte width
} PIKA_HAL_IOCTL_CMD;

#define PIKA_HAL_IOCTL_WIFI_GET_ACTIVE PIKA_HAL_IOCTL_NET_GET_ACTIVE
#define PIKA_HAL_IOCTL_WIFI_GET_STATUS PIKA_HAL_IOCTL_NET_GET_STATUS
#define PIKA_HAL_IOCTL_WIFI_SET_IFCONFIG PIKA_HAL_IOCTL_NET_SET_IFCONFIG
#define PIKA_HAL_IOCTL_WIFI_GET_IFCONFIG PIKA_HAL_IOCTL_NET_GET_IFCONFIG

#define PIKA_HAL_IOCTL_LAN_GET_ACTIVE PIKA_HAL_IOCTL_NET_GET_ACTIVE
#define PIKA_HAL_IOCTL_LAN_GET_STATUS PIKA_HAL_IOCTL_NET_GET_STATUS
#define PIKA_HAL_IOCTL_LAN_SET_IFCONFIG PIKA_HAL_IOCTL_NET_SET_IFCONFIG
#define PIKA_HAL_IOCTL_LAN_GET_IFCONFIG PIKA_HAL_IOCTL_NET_GET_IFCONFIG

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

typedef enum {
    _PIKA_HAL_EVENT_CALLBACK_ENA_UNUSED = 0,
    PIKA_HAL_EVENT_CALLBACK_ENA_ENABLE,
    PIKA_HAL_EVENT_CALLBACK_ENA_DISABLE,
} PIKA_HAL_EVENT_CALLBACK_ENA;

typedef enum {
    _PIKA_HAL_GPIO_EVENT_SIGNAL_UNUSED = 0,
    PIKA_HAL_GPIO_EVENT_SIGNAL_RISING,
    PIKA_HAL_GPIO_EVENT_SIGNAL_FALLING,
    PIKA_HAL_GPIO_EVENT_SIGNAL_ANY,
} PIKA_HAL_GPIO_EVENT_SIGNAL;

typedef struct {
    PIKA_HAL_GPIO_DIR dir;
    PIKA_HAL_GPIO_PULL pull;
    PIKA_HAL_GPIO_SPEED speed;
    void (*event_callback)(pika_dev* dev, PIKA_HAL_GPIO_EVENT_SIGNAL signal);
    PIKA_HAL_GPIO_EVENT_SIGNAL event_callback_filter;
    PIKA_HAL_EVENT_CALLBACK_ENA event_callback_ena;
    void* user_data;
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
    PIKA_HAL_UART_STOP_BITS_1_5 = 3,
} PIKA_HAL_UART_STOP_BITS;

typedef enum {
    _PIKA_HAL_UART_PARITY_UNUSED = 0,
    PIKA_HAL_UART_PARITY_NONE,
    PIKA_HAL_UART_PARITY_ODD,
    PIKA_HAL_UART_PARITY_EVEN,
} PIKA_HAL_UART_PARITY;

typedef enum {
    _PIKA_HAL_UART_EVENT_SIGNAL_UNUSED = 0,
    PIKA_HAL_UART_EVENT_SIGNAL_RX,
    PIKA_HAL_UART_EVENT_SIGNAL_TX,
    PIKA_HAL_UART_EVENT_SIGNAL_ANY,
} PIKA_HAL_UART_EVENT_SIGNAL;

typedef enum {
    _PIKA_HAL_UART_FLOW_CONTROL_UNUSED = 0,
    PIKA_HAL_UART_FLOW_CONTROL_NONE,
    PIKA_HAL_UART_FLOW_CONTROL_RTS,
    PIKA_HAL_UART_FLOW_CONTROL_CTS,
    PIKA_HAL_UART_FLOW_CONTROL_RTS_CTS,
} PIKA_HAL_UART_FLOW_CONTROL;

typedef struct {
    PIKA_HAL_UART_BAUDRATE baudrate;
    PIKA_HAL_UART_DATA_BITS data_bits;
    PIKA_HAL_UART_STOP_BITS stop_bits;
    PIKA_HAL_UART_PARITY parity;
    PIKA_HAL_UART_FLOW_CONTROL flow_control;
    void (*event_callback)(pika_dev* dev, PIKA_HAL_UART_EVENT_SIGNAL signal);
    PIKA_HAL_UART_EVENT_SIGNAL event_callback_filter;
    PIKA_HAL_EVENT_CALLBACK_ENA event_callback_ena;
    pika_dev* TX;
    pika_dev* RX;
    pika_dev* RTS;
    pika_dev* CTS;
    void* user_data;
} pika_hal_UART_config;

typedef uint32_t PIKA_HAL_IIC_SLAVE_ADDR;
typedef uint32_t PIKA_HAL_IIC_MEM_ADDR;

typedef enum PIKA_HAL_IIC_MEM_ADDR_SIZE {
    _PIKA_HAL_IIC_MEM_ADDR_SIZE_UNUSED = 0,
    PIKA_HAL_IIC_MEM_ADDR_SIZE_8BIT = 1,
    PIKA_HAL_IIC_MEM_ADDR_SIZE_16BIT = 2,
    PIKA_HAL_IIC_MEM_ADDR_SIZE_24BIT = 3,
    PIKA_HAL_IIC_MEM_ADDR_SIZE_32BIT = 4,
} PIKA_HAL_IIC_MEM_ADDR_SIZE;

typedef enum PIKA_HAL_IIC_MEM_ADDR_ENA {
    _PIKA_HAL_IIC_MEM_ADDR_ENA_UNUSED = 0,
    PIKA_HAL_IIC_MEM_ADDR_ENA_DISABLE,
    PIKA_HAL_IIC_MEM_ADDR_ENA_ENABLE,
} PIKA_HAL_IIC_MEM_ADDR_ENA;

typedef enum {
    _PIKA_HAL_IIC_SPEED_UNUSED = 0,
    PIKA_HAL_IIC_SPEED_100K = 100000,
    PIKA_HAL_IIC_SPEED_400K = 400000,
    PIKA_HAL_IIC_SPEED_1M = 1000000,
} PIKA_HAL_IIC_SPEED;

typedef enum PIKA_HAL_IIC_MASTER_OR_SLAVE {
    _PIKA_HAL_IIC_MASTER_OR_SLAVE_UNUSED = 0,
    PIKA_HAL_IIC_MASTER,
    PIKA_HAL_IIC_SLAVE,
} PIKA_HAL_IIC_MASTER_OR_SLAVE;

typedef enum PIKA_HAL_IIC_ADDRESS_WIDTH {
    _PIKA_HAL_IIC_ADDRESS_WIDTH_UNUSED = 0,
    PIKA_HAL_IIC_ADDRESS_WIDTH_7BIT = 7,
    PIKA_HAL_IIC_ADDRESS_WIDTH_10BIT = 10,
} PIKA_HAL_IIC_ADDRESS_WIDTH;

typedef enum PIKA_HAL_IIC_TIMEOUT {
    _PIKA_HAL_IIC_TIMEOUT_UNUSED = 0,
    PIKA_HAL_IIC_TIMEOUT_1MS = 1,
    PIKA_HAL_IIC_TIMEOUT_2MS = 2,
    PIKA_HAL_IIC_TIMEOUT_5MS = 5,
    PIKA_HAL_IIC_TIMEOUT_10MS = 10,
    PIKA_HAL_IIC_TIMEOUT_20MS = 20,
    PIKA_HAL_IIC_TIMEOUT_50MS = 50,
    PIKA_HAL_IIC_TIMEOUT_100MS = 100,
    PIKA_HAL_IIC_TIMEOUT_200MS = 200,
    PIKA_HAL_IIC_TIMEOUT_500MS = 500,
    PIKA_HAL_IIC_TIMEOUT_1000MS = 1000,
} PIKA_HAL_IIC_TIMEOUT;

typedef struct {
    PIKA_HAL_IIC_ADDRESS_WIDTH address_width;
    PIKA_HAL_IIC_MASTER_OR_SLAVE master_or_slave;
    PIKA_HAL_IIC_SLAVE_ADDR slave_addr;
    PIKA_HAL_IIC_MEM_ADDR_ENA mem_addr_ena;
    PIKA_HAL_IIC_MEM_ADDR mem_addr;
    PIKA_HAL_IIC_MEM_ADDR_SIZE mem_addr_size;
    PIKA_HAL_IIC_SPEED speed;
    PIKA_HAL_IIC_TIMEOUT timeout;
    void* user_data;
} pika_hal_IIC_config;

typedef struct {
    PIKA_HAL_IIC_ADDRESS_WIDTH address_width;
    PIKA_HAL_IIC_MASTER_OR_SLAVE master_or_slave;
    PIKA_HAL_IIC_SLAVE_ADDR slave_addr;
    PIKA_HAL_IIC_MEM_ADDR_ENA mem_addr_ena;
    PIKA_HAL_IIC_MEM_ADDR mem_addr;
    PIKA_HAL_IIC_MEM_ADDR_SIZE mem_addr_size;
    PIKA_HAL_IIC_SPEED speed;
    PIKA_HAL_IIC_TIMEOUT timeout;
    void* user_data;
    // Uper must be the same with IIC_config;
    pika_dev* SDA;
    pika_dev* SCL;
} pika_hal_SOFT_IIC_config;

typedef enum PIKA_HAL_SPI_TIMEOUT {
    _PIKA_HAL_SPI_TIMEOUT_UNUSED = 0,
    PIKA_HAL_SPI_TIMEOUT_1MS = 1,
    PIKA_HAL_SPI_TIMEOUT_2MS = 2,
    PIKA_HAL_SPI_TIMEOUT_5MS = 5,
    PIKA_HAL_SPI_TIMEOUT_10MS = 10,
    PIKA_HAL_SPI_TIMEOUT_20MS = 20,
    PIKA_HAL_SPI_TIMEOUT_50MS = 50,
    PIKA_HAL_SPI_TIMEOUT_100MS = 100,
    PIKA_HAL_SPI_TIMEOUT_200MS = 200,
    PIKA_HAL_SPI_TIMEOUT_500MS = 500,
    PIKA_HAL_SPI_TIMEOUT_1000MS = 1000,
} PIKA_HAL_SPI_TIMEOUT;

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
    PIKA_HAL_SPI_SPEED_40M = 40000000,
    PIKA_HAL_SPI_SPEED_50M = 50000000,
    PIKA_HAL_SPI_SPEED_80M = 80000000,
    PIKA_HAL_SPI_SPEED_100M = 100000000,
} PIKA_HAL_SPI_SPEED;

#define PIKA_HAL_SPI_RX_BUFFER_SIZE 32

typedef struct {
    PIKA_HAL_SPI_LSB_OR_MSB lsb_or_msb;
    PIKA_HAL_SPI_MASTER_OR_SLAVE master_or_slave;
    PIKA_HAL_SPI_MODE mode;
    PIKA_HAL_SPI_DATA_WIDTH data_width;
    PIKA_HAL_SPI_SPEED speed;
    PIKA_HAL_SPI_TIMEOUT timeout;
    pika_dev* CS;    // Just for MCU with pin mux like ESP32
    pika_dev* SCK;   // Just for MCU with pin mux like ESP32
    pika_dev* MOSI;  // Just for MCU with pin mux like ESP32
    pika_dev* MISO;  // Just for MCU with pin mux like ESP32
    uint8_t tranfer_rx_buffer[PIKA_HAL_SPI_RX_BUFFER_SIZE];
    void* user_data;
} pika_hal_SPI_config;

typedef struct {
    PIKA_HAL_SPI_LSB_OR_MSB lsb_or_msb;
    PIKA_HAL_SPI_MASTER_OR_SLAVE master_or_slave;
    PIKA_HAL_SPI_MODE mode;
    PIKA_HAL_SPI_DATA_WIDTH data_width;
    PIKA_HAL_SPI_SPEED speed;
    PIKA_HAL_SPI_TIMEOUT timeout;
    pika_dev* CS;
    pika_dev* SCK;
    pika_dev* MOSI;
    pika_dev* MISO;
    uint8_t tranfer_rx_buffer[PIKA_HAL_SPI_RX_BUFFER_SIZE];
    void* user_data;
    // Uper must be the same with SPI_config
} pika_hal_SOFT_SPI_config;

typedef enum {
    _PIKA_HAL_ADC_RESOLUTION_UNUSED = 0,
    PIKA_HAL_ADC_RESOLUTION_8 = 8,
    PIKA_HAL_ADC_RESOLUTION_10 = 10,
    PIKA_HAL_ADC_RESOLUTION_12 = 12,
    PIKA_HAL_ADC_RESOLUTION_14 = 14,
    PIKA_HAL_ADC_RESOLUTION_16 = 16,
} PIKA_HAL_ADC_RESOLUTION;

typedef enum PIKA_HAL_ADC_SAMPLING_FREQ {
    _PIKA_HAL_ADC_SAMPLING_FREQ_UNUSED = 0,
    PIKA_HAL_ADC_SAMPLING_FREQ_100 = 100,
    PIKA_HAL_ADC_SAMPLING_FREQ_1K = 1000,
    PIKA_HAL_ADC_SAMPLING_FREQ_10K = 10000,
    PIKA_HAL_ADC_SAMPLING_FREQ_100K = 100000,
    PIKA_HAL_ADC_SAMPLING_FREQ_1M = 1000000,
    PIKA_HAL_ADC_SAMPLING_FREQ_10M = 10000000,
} PIKA_HAL_ADC_SAMPLING_FREQ;

typedef enum PIKA_HAL_ADC_CONTINUOU_OR_SINGLE {
    _PIKA_HAL_ADC_CONTINUOU_OR_SINGLE_UNUSED = 0,
    PIKA_HAL_ADC_SINGLE,
    PIKA_HAL_ADC_CONTINUOU,
} PIKA_HAL_ADC_CONTINUOU_OR_SINGLE;

typedef pika_float PIKA_HAL_ADC_VREF;
typedef uint32_t PIKA_HAL_ADC_MAX;

typedef struct {
    PIKA_HAL_ADC_RESOLUTION sampling_resolution;
    PIKA_HAL_ADC_SAMPLING_FREQ sampling_freq;
    PIKA_HAL_ADC_CONTINUOU_OR_SINGLE continue_or_single;
    PIKA_HAL_ADC_MAX max;
    PIKA_HAL_ADC_VREF vref;
    void* user_data;
} pika_hal_ADC_config;

typedef enum {
    _PIKA_HAL_DAC_RESOLUTION_UNUSED = 0,
    PIKA_HAL_DAC_RESOLUTION_8 = 8,
    PIKA_HAL_DAC_RESOLUTION_10 = 10,
    PIKA_HAL_DAC_RESOLUTION_12 = 12,
    PIKA_HAL_DAC_RESOLUTION_14 = 14,
    PIKA_HAL_DAC_RESOLUTION_16 = 16,
} PIKA_HAL_DAC_RESOLUTION;

typedef enum PIKA_HAL_DAC_SPEED {
    _PIKA_HAL_DAC_SPEED_UNUSED = 0,
    PIKA_HAL_DAC_SPEED_100 = 100,
    PIKA_HAL_DAC_SPEED_1K = 1000,
    PIKA_HAL_DAC_SPEED_10K = 10000,
    PIKA_HAL_DAC_SPEED_100K = 100000,
    PIKA_HAL_DAC_SPEED_1M = 1000000,
    PIKA_HAL_DAC_SPEED_10M = 10000000,
    PIKA_HAL_DAC_SPEED_20M = 20000000,
    PIKA_HAL_DAC_SPEED_50M = 50000000,
    PIKA_HAL_DAC_SPEED_100M = 100000000,
} PIKA_HAL_DAC_SPEED;

typedef pika_float PIKA_HAL_DAC_VREF;
typedef uint32_t PIKA_HAL_DAC_MAX;

typedef struct pika_hal_DAC_config {
    PIKA_HAL_DAC_RESOLUTION sampling_resolution;
    PIKA_HAL_DAC_SPEED speed;
    PIKA_HAL_DAC_MAX max;
    PIKA_HAL_DAC_VREF vref;
    void* user_data;
} pika_hal_DAC_config;

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
    PIKA_HAL_PWM_PERIOD period;
    PIKA_HAL_PWM_DUTY duty;
    void* user_data;
} pika_hal_PWM_config;

typedef enum {
    _PIKA_HAL_WIFI_MODE_UNUSED = 0,
    PIKA_HAL_WIFI_MODE_STA,
    PIKA_HAL_WIFI_MODE_AP,
} PIKA_HAL_WIFI_MODE;

typedef enum {
    _PIKA_HAL_WIFI_STATUS_UNUSED = 0,
    PIKA_HAL_WIFI_STATUS_IDLE,
    PIKA_HAL_WIFI_STATUS_CONNECTING,
    PIKA_HAL_WIFI_STATUS_WRONG_PASSWORD,
    PIKA_HAL_WIFI_STATUS_NO_AP_FOUND,
    PIKA_HAL_WIFI_STATUS_CONNECT_FAIL,
    PIKA_HAL_WIFI_STATUS_GOT_IP,
} PIKA_HAL_WIFI_STATUS;

typedef enum {
    _PIKA_HAL_LAN_STATUS_UNUSED = 0,
    PIKA_HAL_LAN_STATUS_IDLE,
    PIKA_HAL_LAN_STATUS_CONNECTING,
    PIKA_HAL_LAN_STATUS_GOT_IP,
} PIKA_HAL_LAN_STATUS;

typedef enum {
    _PIKA_HAL_WIFI_CHANNEL_UNUSED = 0,
    PIKA_HAL_WIFI_CHANNEL_0,
    PIKA_HAL_WIFI_CHANNEL_1,
    PIKA_HAL_WIFI_CHANNEL_2,
    PIKA_HAL_WIFI_CHANNEL_3,
    PIKA_HAL_WIFI_CHANNEL_4,
    PIKA_HAL_WIFI_CHANNEL_5,
    PIKA_HAL_WIFI_CHANNEL_6,
    PIKA_HAL_WIFI_CHANNEL_7,
    PIKA_HAL_WIFI_CHANNEL_8,
    PIKA_HAL_WIFI_CHANNEL_9,
    PIKA_HAL_WIFI_CHANNEL_10,
    PIKA_HAL_WIFI_CHANNEL_11,
} PIKA_HAL_WIFI_CHANNEL;

typedef enum {
    _PIKA_HAL_WIFI_MAX_CONNECTION = 0,
    PIKA_HAL_WIFI_MAX_CONNECTION_1,
    PIKA_HAL_WIFI_MAX_CONNECTION_2,
    PIKA_HAL_WIFI_MAX_CONNECTION_3,
    PIKA_HAL_WIFI_MAX_CONNECTION_4,
    PIKA_HAL_WIFI_MAX_CONNECTION_5,
    PIKA_HAL_WIFI_MAX_CONNECTION_6,
    PIKA_HAL_WIFI_MAX_CONNECTION_7,
    PIKA_HAL_WIFI_MAX_CONNECTION_8,
    PIKA_HAL_WIFI_MAX_CONNECTION_9,
    PIKA_HAL_WIFI_MAX_CONNECTION_10
} PIKA_HAL_WIFI_MAX_CONNECTION;

#define PIKA_HAL_WIFI_PARAM_MAX_LEN 32
typedef struct pika_hal_WIFI_config {
    PIKA_HAL_WIFI_MODE mode;
    PIKA_HAL_WIFI_CHANNEL channel;
    PIKA_HAL_WIFI_MAX_CONNECTION max_connection;
    char ap_ssid[PIKA_HAL_WIFI_PARAM_MAX_LEN];
    char ap_bssid[PIKA_HAL_WIFI_PARAM_MAX_LEN];
    char ap_password[PIKA_HAL_WIFI_PARAM_MAX_LEN];
    void* user_data;
} pika_hal_WIFI_config;

typedef struct pika_hal_WIFI_connect_config {
    char ssid[PIKA_HAL_WIFI_PARAM_MAX_LEN];
    char bssid[PIKA_HAL_WIFI_PARAM_MAX_LEN];
    char password[PIKA_HAL_WIFI_PARAM_MAX_LEN];
} pika_hal_WIFI_connect_config;

typedef struct pika_hal_NET_ifconfig {
    char ip[PIKA_HAL_WIFI_PARAM_MAX_LEN];
    char netmask[PIKA_HAL_WIFI_PARAM_MAX_LEN];
    char gateway[PIKA_HAL_WIFI_PARAM_MAX_LEN];
    char dns[PIKA_HAL_WIFI_PARAM_MAX_LEN];
} pika_hal_NET_ifconfig;

#define pika_hal_WIFI_ifconfig pika_hal_NET_ifconfig
#define pika_hal_LAN_ifconfig pika_hal_NET_ifconfig

typedef struct pika_hal_WIFI_record {
    char ssid[PIKA_HAL_WIFI_PARAM_MAX_LEN];
    uint8_t bssid[PIKA_HAL_WIFI_PARAM_MAX_LEN];
    size_t bssid_len;
    int channel;
    int rssi;
    int authmode;
} pika_hal_WIFI_record;

typedef struct pika_hal_WIFI_scan_result {
    int count;
    pika_hal_WIFI_record records[];
} pika_hal_WIFI_scan_result;

typedef struct pika_hal_LAN_config {
    void* user_data;
} pika_hal_LAN_config;

typedef enum {
    _PIKA_HAL_PWM_PERIOD_UNUSED = 0,
    PIKA_HAL_TIM_PERIOD_1NS = 1,
    PIKA_HAL_TIM_PERIOD_1US = 1000,
    PIKA_HAL_TIM_PERIOD_1MS = 1000000,
    PIKA_HAL_TIM_PERIOD_1S = 1000000000,
} PIKA_HAL_TIM_PERIOD;

typedef enum {
    _PIKA_HAL_TIM_MODE_UNUSED = 0,
    PIKA_HAL_TIM_MODE_ONESHOT,
    PIKA_HAL_TIM_MODE_CONTINUOUS,
} PIKA_HAL_TIM_MODE;

typedef enum {
    _PIKA_HAL_TIM_EVENT_SIGNAL_UNUSED = 0,
    PIKA_HAL_TIM_EVENT_SIGNAL_TIMEOUT,
    PIKA_HAL_TIM_EVENT_SIGNAL_ANY,
} PIKA_HAL_TIM_EVENT_SIGNAL;

typedef struct pika_hal_TIM_config {
    PIKA_HAL_TIM_PERIOD period;
    PIKA_HAL_TIM_MODE mode;
    PIKA_HAL_EVENT_CALLBACK_ENA event_callback_ena;
    PIKA_HAL_TIM_EVENT_SIGNAL event_callback_filter;
    void (*event_callback)(pika_dev* dev, PIKA_HAL_TIM_EVENT_SIGNAL signal);
} pika_hal_TIM_config;

typedef struct pika_hal_SOFT_TIM_config {
    PIKA_HAL_TIM_PERIOD period;
    PIKA_HAL_TIM_MODE mode;
    PIKA_HAL_EVENT_CALLBACK_ENA event_callback_ena;
    PIKA_HAL_TIM_EVENT_SIGNAL event_callback_filter;
    void (*event_callback)(pika_dev* dev, PIKA_HAL_TIM_EVENT_SIGNAL signal);
    // Uper must be the same with TIM_config
} pika_hal_SOFT_TIM_config;

typedef enum {
    _PIKA_HAL_CAM_PIXFORMAT_UNUSED = 0,
    PIKA_HAL_CAM_PIXFORMAT_RGB565,
    PIKA_HAL_CAM_PIXFORMAT_YUV422,
    PIKA_HAL_CAM_PIXFORMAT_YUV420,
    PIKA_HAL_CAM_PIXFORMAT_GRAYSCALE,
    PIKA_HAL_CAM_PIXFORMAT_JPEG,
    PIKA_HAL_CAM_PIXFORMAT_RGB888,
    PIKA_HAL_CAM_PIXFORMAT_RAW,
    PIKA_HAL_CAM_PIXFORMAT_RGB444,
    PIKA_HAL_CAM_PIXFORMAT_RGB555,
} PIKA_HAL_CAM_PIXFORMAT;

typedef enum {
    _PIKA_HAL_CAM_FRAMESIZE_UNUSED = 0,
    PIKA_HAL_CAM_FRAMESIZE_96X96,    // 96x96
    PIKA_HAL_CAM_FRAMESIZE_QQVGA,    // 160x120
    PIKA_HAL_CAM_FRAMESIZE_QCIF,     // 176x144
    PIKA_HAL_CAM_FRAMESIZE_HQVGA,    // 240x176
    PIKA_HAL_CAM_FRAMESIZE_240X240,  // 240x240
    PIKA_HAL_CAM_FRAMESIZE_QVGA,     // 320x240
    PIKA_HAL_CAM_FRAMESIZE_CIF,      // 400x296
    PIKA_HAL_CAM_FRAMESIZE_HVGA,     // 480x320
    PIKA_HAL_CAM_FRAMESIZE_VGA,      // 640x480
    PIKA_HAL_CAM_FRAMESIZE_SVGA,     // 800x600
    PIKA_HAL_CAM_FRAMESIZE_XGA,      // 1024x768
    PIKA_HAL_CAM_FRAMESIZE_HD,       // 1280x720
    PIKA_HAL_CAM_FRAMESIZE_SXGA,     // 1280x1024
    PIKA_HAL_CAM_FRAMESIZE_UXGA,     // 1600x1200
    PIKA_HAL_CAM_FRAMESIZE_FHD,      // 1920x1080
    PIKA_HAL_CAM_FRAMESIZE_P_HD,     //  720x1280
    PIKA_HAL_CAM_FRAMESIZE_P_3MP,    //  864x1536
    PIKA_HAL_CAM_FRAMESIZE_QXGA,     // 2048x1536
    PIKA_HAL_CAM_FRAMESIZE_QHD,      // 2560x1440
    PIKA_HAL_CAM_FRAMESIZE_WQXGA,    // 2560x1600
    PIKA_HAL_CAM_FRAMESIZE_P_FHD,    // 1080x1920
    PIKA_HAL_CAM_FRAMESIZE_QSXGA,    // 2560x1920
} PIKA_HAL_CAM_FRAMESIZE;

typedef struct pika_hal_CAM_config {
    PIKA_HAL_CAM_PIXFORMAT format;
    PIKA_HAL_CAM_FRAMESIZE framesize;
    int buff_len;
} pika_hal_CAM_config;

/* Signal Generator */

typedef enum {
    _PIKA_HAL_SG_WAVEFORM_UNUSED = 0,
    PIKA_HAL_SG_WAVEFORM_SINE,
    PIKA_HAL_SG_WAVEFORM_SQUARE,
    PIKA_HAL_SG_WAVEFORM_TRIANGLE,
} PIKA_HAL_SG_WAVEFORM;

typedef enum {
    _PIKA_HAL_SG_EVENT_SIGNAL_UNUSED = 0,
    PIKA_HAL_SG_EVENT_SIGNAL_READY_FOR_OUTPUT,
    PIKA_HAL_SG_EVENT_SIGNAL_ANY,
} PIKA_HAL_SG_EVENT_SIGNAL;

typedef enum {
    _PIKA_HAL_SG_NONBLOCKING_UNUSED = 0,
    PIKA_HAL_SG_NONBLOCKING_ENABLE,
    PIKA_HAL_SG_NONBLOCKING_DISABLE,
} PIKA_HAL_SG_NONBLOCKING;

typedef struct {
    PIKA_HAL_SG_WAVEFORM waveform;
    pika_float frequency;  // Frequency in Hz
    pika_float amplitude;  // Amplitude in Volts
    pika_float offset;     // Offset in Volts
    pika_float seconds;    // Seconds to output
    PIKA_HAL_SG_NONBLOCKING nonblocking;
    void (*event_callback)(pika_dev* dev, PIKA_HAL_SG_EVENT_SIGNAL signal);
    PIKA_HAL_SG_EVENT_SIGNAL event_callback_filter;
    PIKA_HAL_EVENT_CALLBACK_ENA event_callback_ena;
    void* user_data;
} pika_hal_SG_config;

typedef struct {
    /* parent start */
    PIKA_HAL_GPIO_DIR dir;
    PIKA_HAL_GPIO_PULL pull;
    PIKA_HAL_GPIO_SPEED speed;
    void (*event_callback)(pika_dev* dev, PIKA_HAL_GPIO_EVENT_SIGNAL signal);
    PIKA_HAL_GPIO_EVENT_SIGNAL event_callback_filter;
    PIKA_HAL_EVENT_CALLBACK_ENA event_callback_ena;
    void* user_data;
    /* parent end */
    pika_dev* iic_port;
    uint32_t slave_addr;
} pika_hal_GPIO_PCA9555_config;

/* impl typedef */

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

typedef struct {
    uint8_t* buffer;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t count;
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_t mutex;
#endif
} pika_hal_CircularQueue;

pika_hal_CircularQueue* pika_hal_circularQueue_create(size_t capacity);
int pika_hal_circularQueue_enqueue(pika_hal_CircularQueue* cb, uint8_t ch);
int pika_hal_circularQueue_dequeue(pika_hal_CircularQueue* cb, uint8_t* value);
int pika_hal_circularQueue_deinit(pika_hal_CircularQueue* cb);
size_t pika_hal_circularQueue_getCount(pika_hal_CircularQueue* cb);
int pika_hal_circularQueue_isEmpty(pika_hal_CircularQueue* cb);
int pika_hal_circularQueue_isFull(pika_hal_CircularQueue* cb);
int pika_hal_circularQueue_peek(pika_hal_CircularQueue* cb, uint8_t* value);

#endif
