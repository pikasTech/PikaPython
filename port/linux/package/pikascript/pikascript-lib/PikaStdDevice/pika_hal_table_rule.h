#undef pika_hal_table_add
#if defined(PIKA_HAL_TABLE_FILE_API)
#define pika_hal_table_add(dev_name)                                           \
    PIKA_WEAK int pika_hal_platform_##dev_name##_open(pika_dev* dev,           \
                                                      char* name) {            \
        WEAK_FUNCTION_NEED_OVERRIDE_ERROR()                                    \
        return -1;                                                             \
    }                                                                          \
    PIKA_WEAK int pika_hal_platform_##dev_name##_close(pika_dev* dev) {        \
        WEAK_FUNCTION_NEED_OVERRIDE_ERROR()                                    \
        return -1;                                                             \
    }                                                                          \
    PIKA_WEAK int pika_hal_platform_##dev_name##_read(pika_dev* dev,           \
                                                      void* buf, size_t len) { \
        WEAK_FUNCTION_NEED_OVERRIDE_ERROR()                                    \
        return -1;                                                             \
    }                                                                          \
    PIKA_WEAK int pika_hal_platform_##dev_name##_write(                        \
        pika_dev* dev, void* buf, size_t len) {                                \
        WEAK_FUNCTION_NEED_OVERRIDE_ERROR()                                    \
        return -1;                                                             \
    }                                                                          \
    PIKA_WEAK int pika_hal_platform_##dev_name##_ioctl_enable(pika_dev* dev) { \
        WEAK_FUNCTION_NEED_OVERRIDE_ERROR()                                    \
        return -1;                                                             \
    }                                                                          \
    PIKA_WEAK int pika_hal_platform_##dev_name##_ioctl_disable(                \
        pika_dev* dev) {                                                       \
        WEAK_FUNCTION_NEED_OVERRIDE_ERROR()                                    \
        return -1;                                                             \
    }                                                                          \
    PIKA_WEAK int pika_hal_platform_##dev_name##_ioctl_config(                 \
        pika_dev* dev, pika_hal_##dev_name##_config* cfg) {                    \
        WEAK_FUNCTION_NEED_OVERRIDE_ERROR()                                    \
        return -1;                                                             \
    }                                                                          \
    int pika_hal_##dev_name##_ioctl(pika_dev* dev, PIKA_HAL_IOCTL_CMD cmd,     \
                                    void* arg) {                               \
        if (NULL == dev) {                                                     \
            return -1;                                                         \
        }                                                                      \
        switch (cmd) {                                                         \
            case PIKA_HAL_IOCTL_ENABLE:                                        \
                return pika_hal_platform_##dev_name##_ioctl_enable(dev);       \
            case PIKA_HAL_IOCTL_DISABLE:                                       \
                return pika_hal_platform_##dev_name##_ioctl_disable(dev);      \
            case PIKA_HAL_IOCTL_CONFIG:                                        \
                return pika_hal_platform_##dev_name##_ioctl_config(            \
                    dev, (pika_hal_##dev_name##_config*)arg);                  \
            default:                                                           \
                return -1;                                                     \
        }                                                                      \
    }

#endif

#if defined(PIKA_HAL_TABLE_DEV_TYPE)
#define pika_hal_table_add(dev_name) PIKA_HAL_##dev_name,
#endif

#if defined(PIKA_HAL_TABLE_IMPL)
#define pika_hal_table_add(dev_name)                   \
    [PIKA_HAL_##dev_name] = {                          \
        .open = pika_hal_platform_##dev_name##_open,   \
        .close = pika_hal_platform_##dev_name##_close, \
        .read = pika_hal_platform_##dev_name##_read,   \
        .write = pika_hal_platform_##dev_name##_write, \
        .ioctl = pika_hal_##dev_name##_ioctl,          \
    },
#endif

#if defined(PIKA_HAL_TABLE_PLATFORM_API)
#define pika_hal_table_add(dev_name)                                    \
    int pika_hal_platform_##dev_name##_open(pika_dev* dev, char* name); \
    int pika_hal_platform_##dev_name##_close(pika_dev* dev);            \
    int pika_hal_platform_##dev_name##_read(pika_dev* dev, void* buf,   \
                                            size_t len);                \
    int pika_hal_platform_##dev_name##_write(pika_dev* dev, void* buf,  \
                                             size_t len);               \
    int pika_hal_platform_##dev_name##_ioctl_enable(pika_dev* dev);     \
    int pika_hal_platform_##dev_name##_ioctl_disable(pika_dev* dev);    \
    int pika_hal_platform_##dev_name##_ioctl_config(                    \
        pika_dev* dev, pika_hal_##dev_name##_config* cfg);
#endif

#if defined(PIKA_HAL_TABLE_DEV_CONFIG_SIZE)
#define pika_hal_table_add(dev_name)                 \
    if (dev_type == PIKA_HAL_##dev_name) {           \
        return sizeof(pika_hal_##dev_name##_config); \
    }
#endif

#if defined(PIKA_HAL_TABLE_IOCTL_MERGE_CONFIG)
#define pika_hal_table_add(dev_name)                                       \
    if (dev->type == PIKA_HAL_##dev_name) {                                \
        return pika_hal_##dev_name##_ioctl_merge_config(dev->ioctl_config, \
                                                        config_in);        \
    }
#endif

#if defined(PIKA_HAL_TABLE_IOCTL_MERGE_CONFIG_HEADER)
#define pika_hal_table_add(dev_name)              \
    int pika_hal_##dev_name##_ioctl_merge_config( \
        pika_hal_##dev_name##_config* dst, pika_hal_##dev_name##_config* src);
#endif

#undef PIKA_HAL_TABLE_FILE_API
#undef PIKA_HAL_TABLE_DEV_TYPE
#undef PIKA_HAL_TABLE_IMPL
#undef PIKA_HAL_TABLE_PLATFORM_API
#undef PIKA_HAL_TABLE_DEV_CONFIG_SIZE
#undef PIKA_HAL_TABLE_IOCTL_MERGE_CONFIG
#undef PIKA_HAL_TABLE_IOCTL_MERGE_CONFIG_HEADER
