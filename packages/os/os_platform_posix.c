/* SPEC: PJ2026-050114 runtime ecosystem packages v0.1; POSIX filesystem port. */
#include "os_platform.h"

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static PikaStatus os_posix_result(int result) {
    return result == 0 ? PIKA_STATUS_OK : PIKA_STATUS_OS_ERROR;
}

PikaStatus pika_os_platform_getcwd(char* output, size_t capacity, size_t* length) {
    char* result;
    if (output == NULL || length == NULL || capacity == 0u) return PIKA_STATUS_INVALID_ARGUMENT;
    result = getcwd(output, capacity);
    if (result == NULL) return PIKA_STATUS_OS_ERROR;
    *length = strlen(output);
    return PIKA_STATUS_OK;
}

PikaStatus pika_os_platform_chdir(const char* path) { return os_posix_result(chdir(path)); }

PikaStatus pika_os_platform_mkdir(const char* path, int64_t mode) {
    if (mode < 0 || mode > 07777) return PIKA_STATUS_VALUE_ERROR;
    return os_posix_result(mkdir(path, (mode_t)mode));
}

PikaStatus pika_os_platform_rmdir(const char* path) { return os_posix_result(rmdir(path)); }

PikaStatus pika_os_platform_listdir(const char* path, PikaOsDirectory* output) {
    DIR* directory;
    struct dirent* entry;
    if (path == NULL || output == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    memset(output, 0, sizeof(*output));
    directory = opendir(path);
    if (directory == NULL) return PIKA_STATUS_OS_ERROR;
    while ((entry = readdir(directory)) != NULL) {
        size_t length;
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        if (output->count >= PIKA_OS_DIRECTORY_ENTRY_LIMIT) {
            (void)closedir(directory);
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        length = strlen(entry->d_name);
        if (length >= PIKA_OS_DIRECTORY_NAME_LIMIT) {
            (void)closedir(directory);
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        memcpy(output->names[output->count], entry->d_name, length + 1u);
        output->lengths[output->count] = length;
        output->count += 1u;
    }
    if (closedir(directory) != 0) return PIKA_STATUS_OS_ERROR;
    return PIKA_STATUS_OK;
}

PikaStatus pika_os_platform_remove(const char* path) { return os_posix_result(unlink(path)); }

PikaStatus pika_os_platform_rename(const char* source, const char* destination) {
    return os_posix_result(rename(source, destination));
}

PikaStatus pika_os_platform_open(const char* path, int64_t flags, int64_t mode, int64_t* descriptor) {
    int native_flags = 0;
    int access = (int)(flags & 3);
    int result;
    if (descriptor == NULL || mode < 0 || mode > 07777) return PIKA_STATUS_INVALID_ARGUMENT;
    if (access == 0) native_flags |= O_RDONLY;
    else if (access == 1) native_flags |= O_WRONLY;
    else if (access == 2) native_flags |= O_RDWR;
    else return PIKA_STATUS_VALUE_ERROR;
    if ((flags & 0x100) != 0) native_flags |= O_CREAT;
    if ((flags & 0x200) != 0) native_flags |= O_TRUNC;
    if ((flags & ~((int64_t)0x303)) != 0) return PIKA_STATUS_VALUE_ERROR;
    result = open(path, native_flags, (mode_t)mode);
    if (result < 0) return PIKA_STATUS_OS_ERROR;
    *descriptor = (int64_t)result;
    return PIKA_STATUS_OK;
}

PikaStatus pika_os_platform_close(int64_t descriptor) {
    if (descriptor < 0 || descriptor > INT32_MAX) return PIKA_STATUS_VALUE_ERROR;
    return os_posix_result(close((int)descriptor));
}

PikaStatus pika_os_platform_read(int64_t descriptor, uint8_t* output, size_t capacity, size_t* length) {
    ssize_t result;
    if (descriptor < 0 || descriptor > INT32_MAX || output == NULL || length == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    result = read((int)descriptor, output, capacity);
    if (result < 0) return PIKA_STATUS_OS_ERROR;
    *length = (size_t)result;
    return PIKA_STATUS_OK;
}

PikaStatus pika_os_platform_write(int64_t descriptor, const uint8_t* data, size_t length, size_t* written) {
    ssize_t result;
    if (descriptor < 0 || descriptor > INT32_MAX || (length > 0u && data == NULL) || written == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    result = write((int)descriptor, data, length);
    if (result < 0) return PIKA_STATUS_OS_ERROR;
    *written = (size_t)result;
    return PIKA_STATUS_OK;
}

PikaStatus pika_os_platform_lseek(int64_t descriptor, int64_t position, int64_t how, int64_t* result) {
    int native_how;
    off_t offset;
    if (descriptor < 0 || descriptor > INT32_MAX || result == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    if (how == 0) native_how = SEEK_SET;
    else if (how == 1) native_how = SEEK_CUR;
    else if (how == 2) native_how = SEEK_END;
    else return PIKA_STATUS_VALUE_ERROR;
    offset = lseek((int)descriptor, (off_t)position, native_how);
    if (offset == (off_t)-1) return PIKA_STATUS_OS_ERROR;
    *result = (int64_t)offset;
    return PIKA_STATUS_OK;
}

PikaStatus pika_os_platform_stat_kind(const char* path, int64_t* kind) {
    struct stat value;
    if (path == NULL || kind == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    if (stat(path, &value) != 0) {
        if (errno == ENOENT || errno == ENOTDIR) { *kind = 0; return PIKA_STATUS_OK; }
        return PIKA_STATUS_OS_ERROR;
    }
    if (S_ISREG(value.st_mode)) *kind = 1;
    else if (S_ISDIR(value.st_mode)) *kind = 2;
    else *kind = 3;
    return PIKA_STATUS_OK;
}

#endif
