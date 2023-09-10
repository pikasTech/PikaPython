#ifdef __cplusplus
extern "C" {
#endif

#ifndef __PIKA_ADAPTER_RTT_H__
#define __PIKA_ADAPTER_RTT_H__
#include <stdint.h>
#include "PikaPlatform.h"

typedef int8_t rt_int8_t;     /**<  8bit integer type */
typedef int16_t rt_int16_t;   /**< 16bit integer type */
typedef int32_t rt_int32_t;   /**< 32bit integer type */
typedef uint8_t rt_uint8_t;   /**<  8bit unsigned integer type */
typedef uint16_t rt_uint16_t; /**< 16bit unsigned integer type */
typedef uint32_t rt_uint32_t; /**< 32bit unsigned integer type */
typedef int64_t rt_int64_t;   /**< 64bit integer type */
typedef uint64_t rt_uint64_t; /**< 64bit unsigned integer type */
typedef size_t rt_size_t;     /**< Type for size number */
typedef signed long
    rt_ssize_t; /**< Used for a count of bytes or an error indication */

/* RT-Thread basic data type definitions */
typedef int rt_bool_t;            /**< boolean type */
typedef signed long rt_base_t;    /**< Nbit CPU related date type */
typedef unsigned long rt_ubase_t; /**< Nbit unsigned CPU related data type */
typedef rt_base_t rt_err_t;       /**< Type for error number */
typedef rt_uint32_t rt_time_t;    /**< Type for time stamp */
typedef rt_uint32_t rt_tick_t;    /**< Type for tick count */
typedef rt_base_t rt_flag_t;      /**< Type for flags */
typedef rt_ubase_t rt_dev_t;      /**< Type for device */
typedef rt_base_t rt_off_t;       /**< Type for offset */
#define rt_inline static __inline

#define RT_NULL 0
/* boolean type definitions */
#define RT_TRUE 1  /**< boolean true  */
#define RT_FALSE 0 /**< boolean fails */

#define rt_malloc pika_platform_malloc
#define rt_calloc pika_platform_calloc
#define rt_realloc pika_platform_realloc
#define rt_free pika_platform_free
#define rt_memset pika_platform_memset
#define rt_memcpy pika_platform_memcpy
#define rt_memcmp pika_platform_memcmp
#define rt_kprintf pika_platform_printf
#define rt_snprintf pika_snprintf
#define rt_vsnprintf pika_platform_vsnprintf
#define rt_strdup pika_platform_strdup
#define rt_tick_from_millisecond pika_platform_tick_from_millisecond
#define rt_strlen strlen

/* RT-Thread error code definitions */
#define RT_EOK 0      /**< There is no error */
#define RT_ERROR 1    /**< A generic error happens */
#define RT_ETIMEOUT 2 /**< Timed out */
#define RT_EFULL 3    /**< The resource is full */
#define RT_EEMPTY 4   /**< The resource is empty */
#define RT_ENOMEM 5   /**< No memory */
#define RT_ENOSYS 6   /**< No system */
#define RT_EBUSY 7    /**< Busy */
#define RT_EIO 8      /**< IO error */
#define RT_EINTR 9    /**< Interrupted system call */
#define RT_EINVAL 10  /**< Invalid argument */

#define LOG_E(fmt, ...) pika_platform_printf(fmt "\r\n", ##__VA_ARGS__)
#define LOG_W(...)
#define LOG_D(...)

#define RT_ASSERT(...) pika_assert(__VA_ARGS__)

/**
 * Double List structure
 */
struct rt_list_node {
    struct rt_list_node* next; /**< point to next node. */
    struct rt_list_node* prev; /**< point to prev node. */
};
typedef struct rt_list_node rt_list_t; /**< Type for lists. */

/**
 * Base structure of Kernel object
 */
struct rt_object {
#if RT_NAME_MAX > 0
    char name[RT_NAME_MAX]; /**< dynamic name of kernel object */
#else
    const char* name; /**< static name of kernel object */
#endif               /* RT_NAME_MAX > 0 */
    rt_uint8_t type; /**< type of kernel object */
    rt_uint8_t flag; /**< flag of kernel object */
    rt_list_t list;  /**< list node of kernel object */
};
typedef struct rt_object* rt_object_t; /**< Type for kernel objects. */

/**
 * device (I/O) class type
 */
enum rt_device_class_type {
    RT_Device_Class_Char = 0,      /**< character device */
    RT_Device_Class_Block,         /**< block device */
    RT_Device_Class_NetIf,         /**< net interface */
    RT_Device_Class_MTD,           /**< memory device */
    RT_Device_Class_CAN,           /**< CAN device */
    RT_Device_Class_RTC,           /**< RTC device */
    RT_Device_Class_Sound,         /**< Sound device */
    RT_Device_Class_Graphic,       /**< Graphic device */
    RT_Device_Class_I2CBUS,        /**< I2C bus device */
    RT_Device_Class_USBDevice,     /**< USB slave device */
    RT_Device_Class_USBHost,       /**< USB host bus */
    RT_Device_Class_USBOTG,        /**< USB OTG bus */
    RT_Device_Class_SPIBUS,        /**< SPI bus device */
    RT_Device_Class_SPIDevice,     /**< SPI device */
    RT_Device_Class_SDIO,          /**< SDIO bus device */
    RT_Device_Class_PM,            /**< PM pseudo device */
    RT_Device_Class_Pipe,          /**< Pipe device */
    RT_Device_Class_Portal,        /**< Portal device */
    RT_Device_Class_Timer,         /**< Timer device */
    RT_Device_Class_Miscellaneous, /**< Miscellaneous device */
    RT_Device_Class_Sensor,        /**< Sensor device */
    RT_Device_Class_Touch,         /**< Touch device */
    RT_Device_Class_PHY,           /**< PHY device */
    RT_Device_Class_Security,      /**< Security device */
    RT_Device_Class_WLAN,          /**< WLAN device */
    RT_Device_Class_Pin,           /**< Pin device */
    RT_Device_Class_ADC,           /**< ADC device */
    RT_Device_Class_DAC,           /**< DAC device */
    RT_Device_Class_WDT,           /**< WDT device */
    RT_Device_Class_PWM,           /**< PWM device */
    RT_Device_Class_Bus,           /**< Bus device */
    RT_Device_Class_Unknown        /**< unknown device */
};

typedef struct rt_device* rt_device_t;
/**
 * Device structure
 */
struct rt_device {
    struct rt_object parent;        /**< inherit from rt_object */
    enum rt_device_class_type type; /**< device type */
    rt_uint16_t flag;               /**< device flag */
    rt_uint16_t open_flag;          /**< device open flag */

    rt_uint8_t ref_count; /**< reference count */
    rt_uint8_t device_id; /**< 0 - 255 */

    /* device call back */
    rt_err_t (*rx_indicate)(rt_device_t dev, rt_size_t size);
    rt_err_t (*tx_complete)(rt_device_t dev, void* buffer);

    const struct rt_device_ops* ops;
    void* user_data; /**< device private data */
};

#endif

#ifdef __cplusplus
}
#endif
