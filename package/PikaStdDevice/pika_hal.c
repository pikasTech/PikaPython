#include "pika_hal.h"
#include "PikaObj.h"

#define PIKA_HAL_TABLE_FILE_API
#include "pika_hal_table.h"

static const pika_dev_impl pika_dev_impl_list[] = {
#define PIKA_HAL_TABLE_IMPL
#include "pika_hal_table.h"
};

#define _PIKA_DEV_TYPE_MAX \
    (sizeof pika_dev_impl_list / sizeof pika_dev_impl_list[0])

static pika_dev_impl* _pika_dev_get_impl(PIKA_HAL_DEV_TYPE type) {
    if (type >= _PIKA_DEV_TYPE_MAX) {
        return NULL;
    }
    return (pika_dev_impl*)&pika_dev_impl_list[type];
}

static size_t _pika_hal_dev_config_size(PIKA_HAL_DEV_TYPE dev_type) {
#define PIKA_HAL_TABLE_DEV_CONFIG_SIZE
#include "pika_hal_table.h"
    return 0;
}

pika_dev* pika_hal_open(PIKA_HAL_DEV_TYPE dev_type, char* name) {
    pika_dev_impl* impl = NULL;
    if (NULL == name) {
        __platform_printf("Error: dev_open name is NULL.\r\n");
        return NULL;
    }
    int ret = -1;
    pika_dev* dev = NULL;
    if (dev_type >= _PIKA_DEV_TYPE_MAX) {
        __platform_printf("Error: dev_type invalied.\r\n");
        goto __exit;
    }
    impl = _pika_dev_get_impl(dev_type);
    dev = (pika_dev*)pikaMalloc(sizeof(pika_dev));
    if (dev == NULL) {
        goto __exit;
    }
    pika_debug("pika_hal_open, dev[0x%p], type[%d], name[%s]", dev, dev_type,
               name);
    memset(dev, 0, sizeof(pika_dev));
    dev->type = dev_type;
    dev->ioctl_config = pikaMalloc(_pika_hal_dev_config_size(dev_type));
    if (dev->ioctl_config == NULL) {
        goto __exit;
    }
    memset(dev->ioctl_config, 0, _pika_hal_dev_config_size(dev_type));
    ret = impl->open(dev, name);
__exit:
    if (0 == ret) {
        return dev;
    }
    /* error */
    __platform_printf("Error: dev_open failed.\r\n");
    if (dev && dev->ioctl_config) {
        pikaFree(dev->ioctl_config, _pika_hal_dev_config_size(dev_type));
        dev->ioctl_config = NULL;
    }
    if (dev) {
        pikaFree(dev, sizeof(pika_dev));
    }
    return NULL;
}

int pika_hal_close(pika_dev* dev) {
    int ret = -1;
    pika_dev_impl* impl = NULL;
    if (dev == NULL) {
        goto __exit;
    }
    impl = _pika_dev_get_impl(dev->type);
    if (impl->close == NULL) {
        goto __exit;
    }
    ret = impl->close(dev);
__exit:
    if (NULL != dev && NULL != dev->ioctl_config) {
        pikaFree(dev->ioctl_config, _pika_hal_dev_config_size(dev->type));
        dev->ioctl_config = NULL;
    }
    if (NULL != dev) {
        pikaFree(dev, sizeof(pika_dev));
    }
    return ret;
}

int pika_hal_read(pika_dev* dev, void* buf, size_t len) {
    if (dev == NULL) {
        pika_platform_printf("Error: dev is NULL.\r\n");
        return -1;
    }
    if (!dev->is_enabled) {
        pika_platform_printf("Error: dev is not enabled.\r\n");
        return -1;
    }
    pika_dev_impl* impl = _pika_dev_get_impl(dev->type);
    if (impl->read == NULL) {
        pika_platform_printf("Error: read not support.\r\n");
        return -1;
    }
    return impl->read(dev, buf, len);
}

int pika_hal_write(pika_dev* dev, void* buf, size_t len) {
    if (dev == NULL) {
        pika_platform_printf("Error: dev is NULL.\r\n");
        return -1;
    }
    if (!dev->is_enabled) {
        pika_platform_printf("Error: dev is not enabled.\r\n");
        return -1;
    }
    pika_dev_impl* impl = _pika_dev_get_impl(dev->type);
    if (impl->write == NULL) {
        pika_platform_printf("Error: write not support.\r\n");
        return -1;
    }
    return impl->write(dev, buf, len);
}

static const int _pika_hal_cmd_arg_cnt[] = {
    [PIKA_HAL_IOCTL_CONFIG] = 1,
    [PIKA_HAL_IOCTL_ENABLE] = 0,
    [PIKA_HAL_IOCTL_DISABLE] = 0,
};

#define _PIKA_HAL_CMD_ARG_CNT_MAX \
    (sizeof _pika_hal_cmd_arg_cnt / sizeof _pika_hal_cmd_arg_cnt[0])

static int _pika_hal_get_arg_cnt(PIKA_HAL_IOCTL_CMD cmd) {
    return _pika_hal_cmd_arg_cnt[PIKA_HAL_IOCTL_CONFIG];
}

int _pika_hal_ioctl_merge_config(pika_dev* dev, void* config_in) {
#define PIKA_HAL_TABLE_IOCTL_MERGE_CONFIG
#include "pika_hal_table.h"
    return -1;
}

int pika_hal_ioctl(pika_dev* dev, PIKA_HAL_IOCTL_CMD cmd, ...) {
    int ret = -1;
    PIKA_HAL_IOCTL_CMD cmd_origin = cmd;
    if (dev == NULL) {
        pika_platform_printf("Error: dev is NULL.\r\n");
        return -1;
    }
    pika_debug("pika_hal_ioctl, dev[0x%p], type[%d], cmd[%d]", dev, dev->type,
               cmd);
    cmd = _pika_hal_get_arg_cnt(cmd_origin);
    pika_dev_impl* impl = _pika_dev_get_impl(dev->type);
    if (impl->ioctl == NULL) {
        pika_platform_printf("Error: ioctl not support.\r\n");
        return -1;
    }
    void* arg_in = NULL;
    if (cmd != 0) {
        va_list args;
        va_start(args, cmd);
        arg_in = va_arg(args, void*);
        if (cmd_origin == PIKA_HAL_IOCTL_CONFIG) {
            ret = _pika_hal_ioctl_merge_config(dev, arg_in);
        } else {
            ret = 0;
        }
        va_end(args);
        if (0 != ret) {
            pika_platform_printf("Error: ioctl merge config failed.\r\n");
            return ret;
        }
    }
    if (cmd_origin == PIKA_HAL_IOCTL_CONFIG) {
        ret = impl->ioctl(dev, cmd_origin, dev->ioctl_config);
    } else {
        ret = impl->ioctl(dev, cmd_origin, arg_in);
    }
    if (ret == 0) {
        if (cmd_origin == PIKA_HAL_IOCTL_ENABLE) {
            dev->is_enabled = 1;
        }
        if (cmd_origin == PIKA_HAL_IOCTL_DISABLE) {
            dev->is_enabled = 0;
        }
    }
    return ret;
}

#define _IOCTL_CONFIG_USE_DEFAULT(item, default) \
    if (src->item == 0) {                        \
        if (dst->item == 0) {                    \
            /* use default value */              \
            dst->item = default;                 \
        } else {                                 \
            /* keep exist value */               \
        }                                        \
    } else {                                     \
        /* use input value */                    \
        dst->item = src->item;                   \
    }

#define _IOCTL_CONFIG_USE_DEFAULT_STR(item, default) \
    if (src->item[0] == '\0') {                      \
        if (dst->item[0] == '\0') {                  \
            /* use default value */                  \
            strcpy(dst->item, default);              \
        } else {                                     \
            /* keep exist value */                   \
        }                                            \
    } else {                                         \
        /* use input value */                        \
        strcpy(dst->item, src->item);                \
    }

int pika_hal_GPIO_ioctl_merge_config(pika_hal_GPIO_config* dst,
                                     pika_hal_GPIO_config* src) {
    // printf("before merge: dst->dir=%d, src->dir=%d\r\n", dst->dir, src->dir);
    _IOCTL_CONFIG_USE_DEFAULT(dir, PIKA_HAL_GPIO_DIR_IN);
    // printf("after merge: dst->dir=%d, src->dir=%d\r\n", dst->dir, src->dir);
    _IOCTL_CONFIG_USE_DEFAULT(pull, PIKA_HAL_GPIO_PULL_NONE);
    _IOCTL_CONFIG_USE_DEFAULT(speed, PIKA_HAL_GPIO_SPEED_10M);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback_filter,
                              PIKA_HAL_GPIO_EVENT_SIGNAL_RISING);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback_ena,
                              PIKA_HAL_EVENT_CALLBACK_ENA_ENABLE);
    _IOCTL_CONFIG_USE_DEFAULT(user_data, NULL);
    return 0;
}

int pika_hal_GPIO_PCA9555_ioctl_merge_config(
    pika_hal_GPIO_PCA9555_config* dst,
    pika_hal_GPIO_PCA9555_config* src) {
    // printf("before merge: dst->dir=%d, src->dir=%d\r\n", dst->dir, src->dir);
    _IOCTL_CONFIG_USE_DEFAULT(dir, PIKA_HAL_GPIO_DIR_IN);
    // printf("after merge: dst->dir=%d, src->dir=%d\r\n", dst->dir, src->dir);
    _IOCTL_CONFIG_USE_DEFAULT(pull, PIKA_HAL_GPIO_PULL_NONE);
    _IOCTL_CONFIG_USE_DEFAULT(speed, PIKA_HAL_GPIO_SPEED_10M);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback_filter,
                              PIKA_HAL_GPIO_EVENT_SIGNAL_RISING);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback_ena,
                              PIKA_HAL_EVENT_CALLBACK_ENA_ENABLE);
    _IOCTL_CONFIG_USE_DEFAULT(user_data, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(iic_port, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(slave_addr, 0);
    return 0;
}

int pika_hal_UART_ioctl_merge_config(pika_hal_UART_config* dst,
                                     pika_hal_UART_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(baudrate, PIKA_HAL_UART_BAUDRATE_115200);
    _IOCTL_CONFIG_USE_DEFAULT(data_bits, PIKA_HAL_UART_DATA_BITS_8);
    _IOCTL_CONFIG_USE_DEFAULT(stop_bits, PIKA_HAL_UART_STOP_BITS_1);
    _IOCTL_CONFIG_USE_DEFAULT(parity, PIKA_HAL_UART_PARITY_NONE);
    _IOCTL_CONFIG_USE_DEFAULT(flow_control, PIKA_HAL_UART_FLOW_CONTROL_NONE);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback_filter,
                              PIKA_HAL_UART_EVENT_SIGNAL_RX);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback_ena,
                              PIKA_HAL_EVENT_CALLBACK_ENA_ENABLE);
    _IOCTL_CONFIG_USE_DEFAULT(TX, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(RX, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(RTS, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(CTS, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(user_data, NULL);
    return 0;
}

int pika_hal_SPI_ioctl_merge_config(pika_hal_SPI_config* dst,
                                    pika_hal_SPI_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(lsb_or_msb, PIKA_HAL_SPI_MSB);
    _IOCTL_CONFIG_USE_DEFAULT(master_or_slave, PIKA_HAL_SPI_MASTER);
    _IOCTL_CONFIG_USE_DEFAULT(mode, PIKA_HAL_SPI_MODE_0);
    _IOCTL_CONFIG_USE_DEFAULT(data_width, PIKA_HAL_SPI_DATA_WIDTH_8);
    _IOCTL_CONFIG_USE_DEFAULT(speed, PIKA_HAL_SPI_SPEED_2M);
    _IOCTL_CONFIG_USE_DEFAULT(timeout, PIKA_HAL_SPI_TIMEOUT_1000MS);
    _IOCTL_CONFIG_USE_DEFAULT(CS, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(SCK, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(MOSI, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(MISO, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(user_data, NULL);
    return 0;
}

int pika_hal_SOFT_SPI_ioctl_merge_config(pika_hal_SOFT_SPI_config* dst,
                                         pika_hal_SOFT_SPI_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(lsb_or_msb, PIKA_HAL_SPI_MSB);
    _IOCTL_CONFIG_USE_DEFAULT(master_or_slave, PIKA_HAL_SPI_MASTER);
    _IOCTL_CONFIG_USE_DEFAULT(mode, PIKA_HAL_SPI_MODE_0);
    _IOCTL_CONFIG_USE_DEFAULT(data_width, PIKA_HAL_SPI_DATA_WIDTH_8);
    _IOCTL_CONFIG_USE_DEFAULT(speed, PIKA_HAL_SPI_SPEED_2M);
    _IOCTL_CONFIG_USE_DEFAULT(timeout, PIKA_HAL_SPI_TIMEOUT_1000MS);
    _IOCTL_CONFIG_USE_DEFAULT(CS, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(SCK, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(MOSI, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(MISO, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(user_data, NULL);
    return 0;
}

int pika_hal_IIC_ioctl_merge_config(pika_hal_IIC_config* dst,
                                    pika_hal_IIC_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(address_width, PIKA_HAL_IIC_ADDRESS_WIDTH_7BIT);
    _IOCTL_CONFIG_USE_DEFAULT(master_or_slave, PIKA_HAL_IIC_MASTER);
    _IOCTL_CONFIG_USE_DEFAULT(slave_addr, 0);
    _IOCTL_CONFIG_USE_DEFAULT(mem_addr_ena, PIKA_HAL_IIC_MEM_ADDR_ENA_DISABLE);
    _IOCTL_CONFIG_USE_DEFAULT(mem_addr_size, PIKA_HAL_IIC_MEM_ADDR_SIZE_8BIT);
    dst->mem_addr = src->mem_addr;
    _IOCTL_CONFIG_USE_DEFAULT(speed, PIKA_HAL_IIC_SPEED_100K);
    _IOCTL_CONFIG_USE_DEFAULT(timeout, PIKA_HAL_IIC_TIMEOUT_1000MS);
    _IOCTL_CONFIG_USE_DEFAULT(user_data, NULL);
    return 0;
}

int pika_hal_SOFT_IIC_ioctl_merge_config(pika_hal_SOFT_IIC_config* dst,
                                         pika_hal_SOFT_IIC_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(SDA, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(SCL, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(address_width, PIKA_HAL_IIC_ADDRESS_WIDTH_7BIT);
    _IOCTL_CONFIG_USE_DEFAULT(master_or_slave, PIKA_HAL_IIC_MASTER);
    _IOCTL_CONFIG_USE_DEFAULT(slave_addr, 0);
    _IOCTL_CONFIG_USE_DEFAULT(mem_addr_ena, PIKA_HAL_IIC_MEM_ADDR_ENA_DISABLE);
    _IOCTL_CONFIG_USE_DEFAULT(mem_addr_size, PIKA_HAL_IIC_MEM_ADDR_SIZE_8BIT);
    dst->mem_addr = src->mem_addr;
    _IOCTL_CONFIG_USE_DEFAULT(speed, PIKA_HAL_IIC_SPEED_100K);
    _IOCTL_CONFIG_USE_DEFAULT(timeout, PIKA_HAL_IIC_TIMEOUT_1000MS);
    _IOCTL_CONFIG_USE_DEFAULT(user_data, NULL);
    return 0;
}

int pika_hal_PWM_ioctl_merge_config(pika_hal_PWM_config* dst,
                                    pika_hal_PWM_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(period, PIKA_HAL_PWM_PERIOD_1MS * 10);
    // _IOCTL_CONFIG_USE_DEFAULT(duty, 0);
    dst->duty = src->duty;
    _IOCTL_CONFIG_USE_DEFAULT(user_data, NULL);
    return 0;
}

int pika_hal_ADC_ioctl_merge_config(pika_hal_ADC_config* dst,
                                    pika_hal_ADC_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(sampling_resolution, PIKA_HAL_ADC_RESOLUTION_12);
    _IOCTL_CONFIG_USE_DEFAULT(sampling_freq, PIKA_HAL_ADC_SAMPLING_FREQ_100);
    _IOCTL_CONFIG_USE_DEFAULT(continue_or_single, PIKA_HAL_ADC_SINGLE);
    _IOCTL_CONFIG_USE_DEFAULT(vref, (pika_float)3.3);
    _IOCTL_CONFIG_USE_DEFAULT(max, 8192);
    _IOCTL_CONFIG_USE_DEFAULT(user_data, NULL);
    return 0;
}

int pika_hal_DAC_ioctl_merge_config(pika_hal_DAC_config* dst,
                                    pika_hal_DAC_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(speed, PIKA_HAL_DAC_SPEED_1K * 8);
    _IOCTL_CONFIG_USE_DEFAULT(sampling_resolution, PIKA_HAL_DAC_RESOLUTION_12);
    _IOCTL_CONFIG_USE_DEFAULT(vref, (pika_float)3.3);
    _IOCTL_CONFIG_USE_DEFAULT(max, 3300000);
    _IOCTL_CONFIG_USE_DEFAULT(user_data, NULL);
    return 0;
}

int pika_hal_WIFI_ioctl_merge_config(pika_hal_WIFI_config* dst,
                                     pika_hal_WIFI_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(mode, PIKA_HAL_WIFI_MODE_STA);
    _IOCTL_CONFIG_USE_DEFAULT(channel, PIKA_HAL_WIFI_CHANNEL_0);
    _IOCTL_CONFIG_USE_DEFAULT(max_connection, PIKA_HAL_WIFI_MAX_CONNECTION_4);
    _IOCTL_CONFIG_USE_DEFAULT_STR(ap_ssid, "pikapython.com");
    _IOCTL_CONFIG_USE_DEFAULT_STR(ap_bssid, "");
    _IOCTL_CONFIG_USE_DEFAULT_STR(ap_password, "pikapython.com");
    _IOCTL_CONFIG_USE_DEFAULT(user_data, NULL);
    return 0;
}

int pika_hal_LAN_ioctl_merge_config(pika_hal_LAN_config* dst,
                                    pika_hal_LAN_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(user_data, NULL);
    return 0;
}

int pika_hal_SG_ioctl_merge_config(pika_hal_SG_config* dst,
                                   pika_hal_SG_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(waveform, PIKA_HAL_SG_WAVEFORM_SINE);
    _IOCTL_CONFIG_USE_DEFAULT(frequency, 10);
    _IOCTL_CONFIG_USE_DEFAULT(amplitude, 0.1);
    dst->offset = src->offset;
    dst->seconds = src->seconds;
    _IOCTL_CONFIG_USE_DEFAULT(nonblocking, PIKA_HAL_SG_NONBLOCKING_ENABLE);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback, NULL);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback_filter,
                              PIKA_HAL_SG_EVENT_SIGNAL_ANY);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback_ena,
                              PIKA_HAL_EVENT_CALLBACK_ENA_ENABLE);
    _IOCTL_CONFIG_USE_DEFAULT(user_data, NULL);
    return 0;
}

int pika_hal_TIM_ioctl_merge_config(pika_hal_TIM_config* dst,
                                    pika_hal_TIM_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(period, PIKA_HAL_TIM_PERIOD_1S);
    _IOCTL_CONFIG_USE_DEFAULT(mode, PIKA_HAL_TIM_MODE_CONTINUOUS);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback_ena,
                              PIKA_HAL_EVENT_CALLBACK_ENA_ENABLE);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback, NULL);
    return 0;
}

int pika_hal_SOFT_TIM_ioctl_merge_config(pika_hal_SOFT_TIM_config* dst,
                                         pika_hal_SOFT_TIM_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(period, PIKA_HAL_TIM_PERIOD_1S);
    _IOCTL_CONFIG_USE_DEFAULT(mode, PIKA_HAL_TIM_MODE_CONTINUOUS);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback_ena,
                              PIKA_HAL_EVENT_CALLBACK_ENA_ENABLE);
    _IOCTL_CONFIG_USE_DEFAULT(event_callback, NULL);
    return 0;
}

int pika_hal_CAM_ioctl_merge_config(pika_hal_CAM_config* dst,
                                    pika_hal_CAM_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(format, PIKA_HAL_CAM_PIXFORMAT_RGB565);
    _IOCTL_CONFIG_USE_DEFAULT(framesize, PIKA_HAL_CAM_FRAMESIZE_QVGA);
    _IOCTL_CONFIG_USE_DEFAULT(buff_len, 320 * 240 * 16);
    return 0;
}

pika_hal_CircularQueue* pika_hal_circularQueue_create(size_t capacity) {
    pika_hal_CircularQueue* cb =
        (pika_hal_CircularQueue*)pikaMalloc(sizeof(pika_hal_CircularQueue));
    if (NULL == cb) {
        return NULL;
    }
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_init(&cb->mutex);
#endif
    cb->capacity = capacity;
    cb->buffer = (uint8_t*)pikaMalloc(capacity);
    if (NULL == cb->buffer) {
        pikaFree(cb, sizeof(pika_hal_CircularQueue));
        return NULL;
    }
    return cb;
}

int pika_hal_circularQueue_enqueue(pika_hal_CircularQueue* cb, uint8_t ch) {
    int ret = 0;
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_lock(&cb->mutex);
#endif
    if (cb->count == cb->capacity) {
        ret = -1;
        goto __exit;
    }

    cb->buffer[cb->tail] = ch;
    cb->tail = (cb->tail + 1) % cb->capacity;
    cb->count++;
__exit:
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_unlock(&cb->mutex);
#endif
    return ret;
}

int pika_hal_circularQueue_dequeue(pika_hal_CircularQueue* cb, uint8_t* value) {
    int ret = 0;
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_lock(&cb->mutex);
#endif
    if (cb->count == 0) {
        ret = -1;
        goto __exit;
    }

    *value = cb->buffer[cb->head];
    cb->head = (cb->head + 1) % cb->capacity;
    cb->count--;
__exit:
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_unlock(&cb->mutex);
#endif
    return ret;
}

int pika_hal_circularQueue_deinit(pika_hal_CircularQueue* cb) {
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_lock(&cb->mutex);
#endif
    pikaFree(cb->buffer, cb->capacity);
    cb->buffer = NULL;
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    cb->capacity = 0;
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_destroy(&cb->mutex);
#endif
    pikaFree(cb, sizeof(pika_hal_CircularQueue));
    return 0;
}

size_t pika_hal_circularQueue_getCount(pika_hal_CircularQueue* cb) {
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_lock(&cb->mutex);
#endif
    size_t count = cb->count;
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_unlock(&cb->mutex);
#endif
    return count;
}

int pika_hal_circularQueue_isEmpty(pika_hal_CircularQueue* cb) {
    return cb->count == 0;
}

int pika_hal_circularQueue_isFull(pika_hal_CircularQueue* cb) {
    return cb->count == cb->capacity;
}

int pika_hal_circularQueue_peek(pika_hal_CircularQueue* cb, uint8_t* value) {
    if (cb->count == 0) {
        return -1;
    }

#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_lock(&cb->mutex);
#endif
    *value = cb->buffer[cb->head];
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_unlock(&cb->mutex);
#endif

    return 0;
}
