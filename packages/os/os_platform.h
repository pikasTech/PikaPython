/* SPEC: PJ2026-050114 runtime ecosystem packages v0.1; filesystem port. */
#ifndef PIKA_OS_PLATFORM_H
#define PIKA_OS_PLATFORM_H

#include "pika_status.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PIKA_OS_PATH_LIMIT 512u
#define PIKA_OS_READ_LIMIT 256u
#define PIKA_OS_DIRECTORY_ENTRY_LIMIT 16u
#define PIKA_OS_DIRECTORY_NAME_LIMIT 128u

typedef struct {
    char names[PIKA_OS_DIRECTORY_ENTRY_LIMIT][PIKA_OS_DIRECTORY_NAME_LIMIT];
    size_t lengths[PIKA_OS_DIRECTORY_ENTRY_LIMIT];
    size_t count;
} PikaOsDirectory;

PikaStatus pika_os_platform_getcwd(char* output, size_t capacity, size_t* length);
PikaStatus pika_os_platform_chdir(const char* path);
PikaStatus pika_os_platform_mkdir(const char* path, int64_t mode);
PikaStatus pika_os_platform_rmdir(const char* path);
PikaStatus pika_os_platform_listdir(const char* path, PikaOsDirectory* output);
PikaStatus pika_os_platform_remove(const char* path);
PikaStatus pika_os_platform_rename(const char* source, const char* destination);
PikaStatus pika_os_platform_open(const char* path, int64_t flags, int64_t mode, int64_t* descriptor);
PikaStatus pika_os_platform_close(int64_t descriptor);
PikaStatus pika_os_platform_read(int64_t descriptor, uint8_t* output, size_t capacity, size_t* length);
PikaStatus pika_os_platform_write(int64_t descriptor, const uint8_t* data, size_t length, size_t* written);
PikaStatus pika_os_platform_lseek(int64_t descriptor, int64_t position, int64_t how, int64_t* result);
PikaStatus pika_os_platform_stat_kind(const char* path, int64_t* kind);

#ifdef __cplusplus
}
#endif

#endif
