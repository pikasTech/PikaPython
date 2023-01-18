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
    pika_dev_impl* impl = _pika_dev_get_impl(dev_type);
    dev = (pika_dev*)pikaMalloc(sizeof(pika_dev));
    if (dev == NULL) {
        goto __exit;
    }
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
    if (dev->ioctl_config) {
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
    if (dev == NULL) {
        goto __exit;
    }
    pika_dev_impl* impl = _pika_dev_get_impl(dev->type);
    if (impl->close == NULL) {
        goto __exit;
    }
    ret = impl->close(dev);
__exit:
    if (NULL != dev->ioctl_config) {
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
        return -1;
    }
    pika_dev_impl* impl = _pika_dev_get_impl(dev->type);
    if (impl->read == NULL) {
        return -1;
    }
    return impl->read(dev, buf, len);
}

int pika_hal_write(pika_dev* dev, void* buf, size_t len) {
    if (dev == NULL) {
        return -1;
    }
    pika_dev_impl* impl = _pika_dev_get_impl(dev->type);
    if (impl->write == NULL) {
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
    if (cmd >= _PIKA_HAL_CMD_ARG_CNT_MAX) {
        return -1;
    }
    return _pika_hal_cmd_arg_cnt[cmd];
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
        return -1;
    }
    cmd = _pika_hal_get_arg_cnt(cmd_origin);
    if (cmd < 0) {
        return -1;
    }
    pika_dev_impl* impl = _pika_dev_get_impl(dev->type);
    if (impl->ioctl == NULL) {
        return -1;
    }
    void* config_in = NULL;
    if (cmd != 0) {
        va_list args;
        va_start(args, cmd);
        config_in = va_arg(args, void*);
        ret = _pika_hal_ioctl_merge_config(dev, config_in);
        va_end(args);
        if (0 != ret) {
            return ret;
        }
    }
    ret = impl->ioctl(dev, cmd_origin, dev->ioctl_config);
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
    return 0;
}

int pika_hal_PWM_ioctl_merge_config(pika_hal_PWM_config* dst,
                                    pika_hal_PWM_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(period, PIKA_HAL_PWM_PERIOD_1MS * 10);
    // _IOCTL_CONFIG_USE_DEFAULT(duty, 0);
    dst->duty = src->duty;
    return 0;
}

int pika_hal_ADC_ioctl_merge_config(pika_hal_ADC_config* dst,
                                    pika_hal_ADC_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(sampling_resolution, PIKA_HAL_ADC_RESOLUTION_12);
    _IOCTL_CONFIG_USE_DEFAULT(sampling_freq, PIKA_HAL_ADC_SAMPLING_FREQ_100);
    _IOCTL_CONFIG_USE_DEFAULT(continue_or_single, PIKA_HAL_ADC_SINGLE);
    _IOCTL_CONFIG_USE_DEFAULT(vref, (pika_float)3.3);
    _IOCTL_CONFIG_USE_DEFAULT(max, 8192);
    return 0;
}

int pika_hal_DAC_ioctl_merge_config(pika_hal_DAC_config* dst,
                                    pika_hal_DAC_config* src) {
    _IOCTL_CONFIG_USE_DEFAULT(speed, PIKA_HAL_DAC_SPEED_1K * 8);
    _IOCTL_CONFIG_USE_DEFAULT(sampling_resolution, PIKA_HAL_DAC_RESOLUTION_12);
    _IOCTL_CONFIG_USE_DEFAULT(vref, (pika_float)3.3);
    _IOCTL_CONFIG_USE_DEFAULT(max, 3300000);
    return 0;
}
