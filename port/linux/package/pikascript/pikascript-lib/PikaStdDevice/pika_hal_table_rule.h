#undef pika_hal_table_add
#if defined(PIKA_HAL_TABLE_FILE_API)
#define pika_hal_table_add(dev_name)                                   \
    PIKA_WEAK int pika_##dev_name##_open(pika_dev* dev) {              \
        WEAK_FUNCTION_NEED_OVERRIDE_ERROR()                            \
        return -1;                                                     \
    }                                                                  \
    PIKA_WEAK int pika_##dev_name##_close(pika_dev* dev) {             \
        WEAK_FUNCTION_NEED_OVERRIDE_ERROR()                            \
        return -1;                                                     \
    }                                                                  \
    PIKA_WEAK int pika_##dev_name##_read(pika_dev* dev, uint8_t* buf,  \
                                         size_t len) {                 \
        WEAK_FUNCTION_NEED_OVERRIDE_ERROR()                            \
        return -1;                                                     \
    }                                                                  \
    PIKA_WEAK int pika_##dev_name##_write(pika_dev* dev, uint8_t* buf, \
                                          size_t len) {                \
        WEAK_FUNCTION_NEED_OVERRIDE_ERROR()                            \
        return -1;                                                     \
    }                                                                  \
    PIKA_WEAK int pika_##dev_name##_ioctl_enable(pika_dev* dev) {      \
        WEAK_FUNCTION_NEED_OVERRIDE_ERROR()                            \
        return -1;                                                     \
    }                                                                  \
    PIKA_WEAK int pika_##dev_name##_ioctl_disable(pika_dev* dev) {     \
        WEAK_FUNCTION_NEED_OVERRIDE_ERROR()                            \
        return -1;                                                     \
    }                                                                  \
    PIKA_WEAK int pika_##dev_name##_ioctl_config(                      \
        pika_dev* dev, pika_##dev_name##_config* cfg) {                \
        WEAK_FUNCTION_NEED_OVERRIDE_ERROR()                            \
        return -1;                                                     \
    }                                                                  \
    int pika_##dev_name##_ioctl(pika_dev* dev, pika_dev_cmd cmd,       \
                                uintptr_t arg) {                       \
        if (NULL == dev) {                                             \
            return -1;                                                 \
        }                                                              \
        switch (cmd) {                                                 \
            case PIKA_DEV_CMD_ENABLE:                                  \
                return pika_##dev_name##_ioctl_enable(dev);            \
            case PIKA_DEV_CMD_DISABLE:                                 \
                return pika_##dev_name##_ioctl_disable(dev);           \
            case PIKA_DEV_CMD_CONFIG:                                  \
                return pika_##dev_name##_ioctl_config(                 \
                    dev, (pika_##dev_name##_config*)arg);              \
            default:                                                   \
                return -1;                                             \
        }                                                              \
    }

#endif

#if defined(PIKA_HAL_TABLE_DEV_TYPE)
#define pika_hal_table_add(dev_name) PIKA_HAL_##dev_name,
#endif

#if defined(PIKA_HAL_TABLE_IMPL)
#define pika_hal_table_add(dev_name)      \
    [PIKA_HAL_##dev_name] = {             \
        .open = pika_##dev_name##_open,   \
        .close = pika_##dev_name##_close, \
        .read = pika_##dev_name##_read,   \
        .write = pika_##dev_name##_write, \
        .ioctl = pika_##dev_name##_ioctl, \
    },
#endif

#undef PIKA_HAL_TABLE_FILE_API
#undef PIKA_HAL_TABLE_DEV_TYPE
#undef PIKA_HAL_TABLE_IMPL
