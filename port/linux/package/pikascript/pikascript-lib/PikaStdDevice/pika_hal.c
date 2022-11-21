#include "pika_hal.h"
#include "PikaObj.h"

#define PIKA_HAL_TABLE_FILE_API
#include "pika_hal_table.h"

static const pika_dev_impl pika_dev_impl_list[] = {
#define PIKA_HAL_TABLE_IMPL
#include "pika_hal_table.h"
};

static pika_dev_impl* _pika_dev_get_impl(PIKA_HAL_DEV_TYPE type) {
    if (type >= _PIKA_DEV_TYPE_MAX) {
        return NULL;
    }
    return (pika_dev_impl*)&pika_dev_impl_list[type];
}

pika_dev* pika_hal_open(PIKA_HAL_DEV_TYPE dev_type, char* name) {
    if (dev_type >= _PIKA_DEV_TYPE_MAX) {
        __platform_printf("Error: dev_type invalied.\r\n");
        return NULL;
    }
    pika_dev_impl* impl = _pika_dev_get_impl(dev_type);
    if (impl->open == NULL) {
        __platform_printf("Error: dev_open_impl not implamented.\r\n");
        return NULL;
    }
    pika_dev* dev = (pika_dev*)pikaMalloc(sizeof(pika_dev));
    if (dev == NULL) {
        return NULL;
    }
    dev->type = dev_type;
    if (impl->open(dev, name) != 0) {
        pikaFree(dev, sizeof(pika_dev));
        __platform_printf("Error: dev_open failed.\r\n");
        return NULL;
    }
    return dev;
}

int pika_hal_close(pika_dev* dev) {
    if (dev == NULL) {
        return -1;
    }
    pika_dev_impl* impl = _pika_dev_get_impl(dev->type);
    if (impl->close == NULL) {
        return -1;
    }
    int ret = impl->close(dev);
    pikaFree(dev, sizeof(pika_dev));
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

static const int _pika_hal_cmd_arg_cnt[_PIKA_HAL_IOCTL_MAX] = {
    [PIKA_HAL_IOCTL_ENABLE] = 0,
    [PIKA_HAL_IOCTL_CONFIG] = 1,
    [PIKA_HAL_IOCTL_DISABLE] = 0,
};

static int _pika_hal_get_arg_cnt(PIKA_HAL_IOCTL_CMD cmd) {
    if (cmd >= _PIKA_HAL_IOCTL_MAX) {
        return -1;
    }
    return _pika_hal_cmd_arg_cnt[cmd];
}

int pika_hal_ioctl(pika_dev* dev, PIKA_HAL_IOCTL_CMD cmd, ...) {
    if (dev == NULL) {
        return -1;
    }
    int arg_cnt = _pika_hal_get_arg_cnt(cmd);
    if (arg_cnt < 0) {
        return -1;
    }
    pika_dev_impl* impl = _pika_dev_get_impl(dev->type);
    if (impl->ioctl == NULL) {
        return -1;
    }
    va_list args;
    va_start(args, arg_cnt);
    uintptr_t arg = va_arg(args, uintptr_t);
    int ret = impl->ioctl(dev, cmd, arg);
    va_end(args);
    return ret;
}
