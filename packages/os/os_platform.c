/* SPEC: PJ2026-050114 runtime ecosystem packages v0.1; weak filesystem port. */
#include "os_platform.h"
#include "pika_platform.h"

#define PIKA_OS_WEAK_FAILURE(name, parameters) \
    PIKA_WEAK PikaStatus name parameters { return PIKA_STATUS_OS_ERROR; }

PIKA_OS_WEAK_FAILURE(pika_os_platform_getcwd,
                     (char* output, size_t capacity, size_t* length))
PIKA_OS_WEAK_FAILURE(pika_os_platform_chdir, (const char* path))
PIKA_OS_WEAK_FAILURE(pika_os_platform_mkdir, (const char* path, int64_t mode))
PIKA_OS_WEAK_FAILURE(pika_os_platform_rmdir, (const char* path))
PIKA_OS_WEAK_FAILURE(pika_os_platform_listdir,
                     (const char* path, PikaOsDirectory* output))
PIKA_OS_WEAK_FAILURE(pika_os_platform_remove, (const char* path))
PIKA_OS_WEAK_FAILURE(pika_os_platform_rename,
                     (const char* source, const char* destination))
PIKA_OS_WEAK_FAILURE(pika_os_platform_open,
                     (const char* path, int64_t flags, int64_t mode,
                      int64_t* descriptor))
PIKA_OS_WEAK_FAILURE(pika_os_platform_close, (int64_t descriptor))
PIKA_OS_WEAK_FAILURE(pika_os_platform_read,
                     (int64_t descriptor, uint8_t* output, size_t capacity,
                      size_t* length))
PIKA_OS_WEAK_FAILURE(pika_os_platform_write,
                     (int64_t descriptor, const uint8_t* data, size_t length,
                      size_t* written))
PIKA_OS_WEAK_FAILURE(pika_os_platform_lseek,
                     (int64_t descriptor, int64_t position, int64_t how,
                      int64_t* result))
PIKA_OS_WEAK_FAILURE(pika_os_platform_stat_kind,
                     (const char* path, int64_t* kind))
