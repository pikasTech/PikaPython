#ifdef __cplusplus
extern "C" {
#endif

#ifndef __PIKA_ADAPTER_RTT_H__
#include <stdint.h>
#include "PikaPlatform.h"
#define __PIKA_ADAPTER_RTT_H__

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

#define rt_int32_t int32_t
#define rt_uint32_t uint32_t
#define rt_int16_t int16_t
#define rt_uint16_t uint16_t
#define rt_int8_t int8_t
#define rt_uint8_t uint8_t
#define rt_int64_t int64_t
#define rt_uint64_t uint64_t
#define rt_size_t size_t
#define rt_bool_t uint8_t

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

#endif

#ifdef __cplusplus
}
#endif