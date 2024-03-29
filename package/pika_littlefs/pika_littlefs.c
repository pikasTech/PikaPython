#include "PikaObj.h"

#ifndef PIKA_LITTLEFS_ENABLE
#define PIKA_LITTLEFS_ENABLE 0
#endif

#if PIKA_LITTLEFS_ENABLE

#include "lfs.h"

PIKA_WEAK lfs_t pika_lfs_handle = {0};
static char current_path[256] = "/";

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
    int lfs_whence;

    switch (whence) {
        case SEEK_SET:
            lfs_whence = LFS_SEEK_SET;
            break;
        case SEEK_CUR:
            lfs_whence = LFS_SEEK_CUR;
            break;
        case SEEK_END:
            lfs_whence = LFS_SEEK_END;
            break;
        default:
            return -1;
    }

    return (int)lfs_file_seek(&pika_lfs_handle, (lfs_file_t*)stream,
                              (lfs_soff_t)offset, lfs_whence);
}

long pika_platform_ftell(FILE* stream) {
    return (long)lfs_file_tell(&pika_lfs_handle, (lfs_file_t*)stream);
}

char* pika_platform_getcwd(char* buf, size_t size) {
    strncpy(buf, current_path, size);
    return buf;
}

int pika_platform_chdir(const char* path) {
    // In a real environment, you'd want to check if the directory exists first
    strncpy(current_path, path, sizeof(current_path));
    return 0;
}

int pika_platform_rmdir(const char* pathname) {
    return lfs_remove(&pika_lfs_handle, pathname);
}

int pika_platform_mkdir(const char* pathname, int mode) {
    // mode is not used in LittleFS
    return lfs_mkdir(&pika_lfs_handle, pathname);
}

int pika_platform_remove(const char* pathname) {
    return lfs_remove(&pika_lfs_handle, pathname);
}

int pika_platform_rename(const char* oldpath, const char* newpath) {
    return lfs_rename(&pika_lfs_handle, oldpath, newpath);
}

char** pika_platform_listdir(const char* path, int* count) {
    // LittleFS doesn't have a direct listdir function, so we'll need to
    // implement this.
    lfs_dir_t dir;
    struct lfs_info info;

    char** filenames = NULL;
    int index = 0;
    *count = 0;

    if (lfs_dir_open(&pika_lfs_handle, &dir, path) == LFS_ERR_OK) {
        while (lfs_dir_read(&pika_lfs_handle, &dir, &info) == LFS_ERR_OK) {
            if (info.name[0] != 0) {
                (*count)++;
            }
        }
        lfs_dir_close(&pika_lfs_handle, &dir);

        // Allocate space for filenames
        filenames = (char**)pika_platform_malloc(sizeof(char*) * (*count));

        // Read filenames
        lfs_dir_open(&pika_lfs_handle, &dir, path);
        while (lfs_dir_read(&pika_lfs_handle, &dir, &info) == LFS_ERR_OK) {
            if (info.name[0] != 0) {
                filenames[index] = pika_platform_strdup(info.name);
                index++;
            }
        }
        lfs_dir_close(&pika_lfs_handle, &dir);
    }

    return filenames;
}

#endif
