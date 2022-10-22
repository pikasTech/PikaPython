#include "PikaObj.h"
#include "ff.h"

typedef FIL _INNER_FILE;

/* private prototypes */
int __fmodeflags(const char* mode);

/* public prototypes */
FILE* fatfs_fopen(const char* filename, const char* modes);
size_t fatfs_fwrite(const void* ptr, size_t size, size_t n, FILE* stream);
size_t fatfs_fread(void* ptr, size_t size, size_t n, FILE* stream);
int fatfs_fclose(FILE* stream);
int fatfs_fseek(FILE* stream, long offset, int whence);
long fatfs_ftell(FILE* stream);

#ifndef PIKA_FATFS_ENABLE
#define PIKA_FATFS_ENABLE 1
#endif

#if PIKA_FATFS_ENABLE
FILE* __platform_fopen(const char* filename, const char* modes) {
    return fatfs_fopen(filename, modes);
}
int __platform_fclose(FILE* stream) {
    return fatfs_fclose(stream);
}
size_t __platform_fwrite(const void* ptr, size_t size, size_t n, FILE* stream) {
    return fatfs_fwrite(ptr, size, n, stream);
}
size_t __platform_fread(void* ptr, size_t size, size_t n, FILE* stream) {
    return fatfs_fread(ptr, size, n, stream);
}
int __platform_fseek(FILE* stream, long offset, int whence) {
    return fatfs_fseek(stream, offset, whence);
}
long __platform_ftell(FILE* stream) {
    return fatfs_ftell(stream);
}
#endif

FILE* fatfs_fopen(const char* filename, const char* modes) {
    FRESULT res;
    int flags;

    /* Check for valid initial mode character */
    if (!strchr("rwa", *modes)) {
        return 0;
    }

    /* Compute the flags to pass to open() */
    flags = __fmodeflags(modes);

    _INNER_FILE *_f = malloc(sizeof(*_f));
    if(NULL == _f){
        return NULL;
    }
    res = f_open(_f, filename, flags);
    if (res) {
        free(_f);
        return NULL;
    }

    return (FILE *)_f;
}

size_t fatfs_fwrite(const void* ptr, size_t size, size_t n, FILE* stream) {
    _INNER_FILE* _f = (_INNER_FILE*)stream;
    size_t len = 0;
    f_write(_f, ptr, n * size, &len);
    return len;
}

size_t fatfs_fread(void* ptr, size_t size, size_t n, FILE* stream) {
    _INNER_FILE* _f = (_INNER_FILE*)stream;
    size_t len = 0;
    f_read(_f, ptr, n * size, &len);
    return len;
}

int fatfs_fclose(FILE* stream) {
    _INNER_FILE* _f = (_INNER_FILE*)stream;
    f_close(_f);
    free(_f);
    return 0;
}

int fatfs_fseek(FILE* stream, long offset, int whence) {
    _INNER_FILE* _f = (_INNER_FILE*)stream;
    f_lseek(_f, offset);
    return f_tell(_f);
}

long fatfs_ftell(FILE* stream) {
    _INNER_FILE* _f = (_INNER_FILE*)stream;
    return f_tell(_f);
}

int __fmodeflags(const char* mode) {
    int flags;
    if (strchr(mode, '+'))
        flags = FA_WRITE | FA_READ;
    else if (*mode == 'r')
        flags = FA_READ;
    else
        flags = FA_WRITE;
    if (*mode == 'w')
        flags |= FA_OPEN_ALWAYS;
    if (*mode == 'a')
        flags |= FA_OPEN_APPEND;
    return flags;
}
