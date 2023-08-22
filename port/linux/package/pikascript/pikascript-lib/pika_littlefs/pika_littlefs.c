#include "PikaObj.h"

#ifndef PIKA_LITTLEFS_ENABLE
#define PIKA_LITTLEFS_ENABLE 0
#endif

#if PIKA_LITTLEFS_ENABLE

#include "lfs.h"

PIKA_WEAK lfs_t pika_lfs_handle = {0};

static int __lfs_fmodeflags(const char* mode) {
    int flag = 0;
    if (strstr(mode, "w")) {
        flag |= LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC;
    }
    if (strstr(mode, "r")) {
        flag |= LFS_O_RDONLY;
    }
    if (strstr(mode, "a")) {
        flag |= LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND;
    }
    if (strstr(mode, "+")) {
        flag |= LFS_O_RDWR;
    }
    return flag;
}

FILE* pika_platform_fopen(const char* filename, const char* modes) {
    lfs_file_t* _pika_lfs_file = pika_platform_malloc(sizeof(lfs_file_t));
    int mode = __lfs_fmodeflags(modes);
    if (lfs_file_open(&pika_lfs_handle, _pika_lfs_file, filename, mode) ==
        LFS_ERR_OK) {
        return (FILE*)_pika_lfs_file;
    } else {
        pika_platform_free(_pika_lfs_file);
        return NULL;
    }
}

int pika_platform_fclose(FILE* stream) {
    if (lfs_file_close(&pika_lfs_handle, (lfs_file_t*)stream) == LFS_ERR_OK) {
        pika_platform_free(stream);
        return 0;
    } else {
        return -1;
    }
}

size_t pika_platform_fwrite(const void* ptr,
                            size_t size,
                            size_t n,
                            FILE* stream) {
    return (size_t)lfs_file_write(&pika_lfs_handle, (lfs_file_t*)stream, ptr,
                                  (lfs_size_t)(size * n));
}

size_t pika_platform_fread(void* ptr, size_t size, size_t n, FILE* stream) {
    return (size_t)lfs_file_read(&pika_lfs_handle, (lfs_file_t*)stream, ptr,
                                 (lfs_size_t)(size * n));
}

int pika_platform_fseek(FILE* stream, long offset, int whence) {
    return (int)lfs_file_seek(&pika_lfs_handle, (lfs_file_t*)stream,
                              (lfs_soff_t)offset, whence);
}

long pika_platform_ftell(FILE* stream) {
    return (long)lfs_file_tell(&pika_lfs_handle, (lfs_file_t*)stream);
}

#endif
