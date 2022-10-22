#include "PikaObj.h"
/* simulate the posixfs on linux */
#include <fcntl.h>
#include <unistd.h>

#define sys_open open
#define sys_close close
#define sys_read read
#define sys_write write
#define sys_lseek lseek

int open(const char* __path, int __oflag, ...);
int close(int __fd);
ssize_t read(int __fd, void* __buf, size_t __nbytes);
ssize_t write(int __fd, const void* __buf, size_t __nbytes);

typedef struct _INNER_FILE {
    int fd;
} _INNER_FILE;

/* private prototypes */
int __fmodeflags(const char* mode);
FILE* __fdopen(int fd, const char* mode);

/* public prototypes */
FILE* posixfs_fopen(const char* filename, const char* modes);
size_t posixfs_fwrite(const void* ptr, size_t size, size_t n, FILE* stream);
size_t posixfs_fread(void* ptr, size_t size, size_t n, FILE* stream);
int posixfs_fclose(FILE* stream);
int posixfs_fseek(FILE* stream, long offset, int whence);
long posixfs_ftell(FILE* stream);

#ifndef PIKA_POSIXFS_ENABLE
#define PIKA_POSIXFS_ENABLE 0
#endif

#if PIKA_POSIXFS_ENABLE
FILE* __platform_fopen(const char* filename, const char* modes) {
    return posixfs_fopen(filename, modes);
}
int __platform_fclose(FILE* stream) {
    return posixfs_fclose(stream);
}
size_t __platform_fwrite(const void* ptr, size_t size, size_t n, FILE* stream) {
    return posixfs_fwrite(ptr, size, n, stream);
}
size_t __platform_fread(void* ptr, size_t size, size_t n, FILE* stream) {
    return posixfs_fread(ptr, size, n, stream);
}
int __platform_fseek(FILE* stream, long offset, int whence) {
    return posixfs_fseek(stream, offset, whence);
}
long __platform_ftell(FILE* stream) {
    return posixfs_ftell(stream);
}
#endif

FILE* posixfs_fopen(const char* filename, const char* modes) {
    FILE* f;
    int fd;
    int flags;

    /* Check for valid initial mode character */
    if (!strchr("rwa", *modes)) {
        return 0;
    }

    /* Compute the flags to pass to open() */
    flags = __fmodeflags(modes);

    fd = sys_open(filename, flags, 0666);
    if (fd < 0) {
        return 0;
    }

    f = __fdopen(fd, modes);
    if (f) {
        return f;
    }

    sys_close(fd);
    return 0;
}

size_t posixfs_fwrite(const void* ptr, size_t size, size_t n, FILE* stream) {
    _INNER_FILE* _f = (_INNER_FILE*)stream;
    return sys_write(_f->fd, ptr, n * size);
}

size_t posixfs_fread(void* ptr, size_t size, size_t n, FILE* stream) {
    _INNER_FILE* _f = (_INNER_FILE*)stream;
    return sys_read(_f->fd, ptr, n * size);
}

int posixfs_fclose(FILE* stream) {
    _INNER_FILE* _f = (_INNER_FILE*)stream;
    int fd = _f->fd;
    free(_f);
    return sys_close(fd);
}

int posixfs_fseek(FILE* stream, long offset, int whence) {
    _INNER_FILE* _f = (_INNER_FILE*)stream;
    return sys_lseek(_f->fd, offset, whence);
}

long posixfs_ftell(FILE* stream) {
    _INNER_FILE* _f = (_INNER_FILE*)stream;
    return sys_lseek(_f->fd, 0, 1);
}

int __fmodeflags(const char* mode) {
    int flags;
    if (strchr(mode, '+'))
        flags = O_RDWR;
    else if (*mode == 'r')
        flags = O_RDONLY;
    else
        flags = O_WRONLY;
#if 0
    if (strchr(mode, 'x'))
        flags |= O_EXCL;
    if (strchr(mode, 'e'))
        flags |= O_CLOEXEC;
#endif
    if (*mode != 'r')
        flags |= O_CREAT;
    if (*mode == 'w')
        flags |= O_TRUNC;
    if (*mode == 'a')
        flags |= O_APPEND;
    return flags;
}

FILE* __fdopen(int fd, const char* mode) {
    _INNER_FILE* _f;
    /* Allocate FILE+buffer or fail */
    if (!(_f = malloc(sizeof(*_f))))
        return 0;

    /* Zero-fill only the struct, not the buffer */
    memset(_f, 0, sizeof(*_f));
    _f->fd = fd;
    FILE* f = (FILE*)_f;
    return f;
}
