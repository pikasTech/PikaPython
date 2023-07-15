#include "os.h"
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"
#include "PikaStdData_List.h"
#include "TinyObj.h"
#include "os_fileStat.h"
#include "os_platform.h"

void os___init__(PikaObj* self) {
    // obj_newDirectObj(self,"os",New_TinyObj);

    obj_setInt(self, "O_RDONLY", FILE_RDONLY);
    obj_setInt(self, "O_WRONLY", FILE_WRONLY);
    obj_setInt(self, "O_RDWR", FILE_RDWR);
    obj_setInt(self, "O_APPEND", FILE_APPEND);
    obj_setInt(self, "O_CREAT", FILE_CREAT);
}
//#undef _WIN32

int os_fileStat_st_size(PikaObj* self) {
    int size = obj_getInt(self, "st_size");
    return size;
}

PikaObj* os_fstat(PikaObj* self, PikaObj* fd) {
    int size = 0;
    size = os_getFileSize(fd);

    PikaObj* stat_obj = newNormalObj(New_os_fileStat);

    obj_setInt(stat_obj, "st_size", size);
    return stat_obj;
}

PikaObj* os_open(PikaObj* self, char* filename, int flags) {
    return os_open_platform(filename, flags);
}

char* os_read(PikaObj* self, PikaObj* fd, int len) {
    return os_read_platform(self, fd, len);
}

int os_write(PikaObj* self, PikaObj* fd, Arg* buf) {
    if (arg_getType(buf) != ARG_TYPE_BYTES) {
        obj_setErrorCode(self, PIKA_RES_ERR_INVALID_PARAM);
        pika_platform_printf(
            "TypeError: a bytes-like object is required, not 'str'\r\n");
        return -1;
    }
    return os_write_platform(arg_getBytes(buf), arg_getBytesSize(buf), fd);
}

int os_lseek(PikaObj* self, PikaObj* fd, int how, int pos) {
    return os_lseek_platform(fd, how, pos);
}

void os_close(PikaObj* self, PikaObj* fd) {
    if (os_close_platform(fd) < 0) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO_ERROR);
        pika_platform_printf("close file error\r\n");
    }
}

char* os_getcwd(PikaObj* self) {
    return os_getcwd_platform(self);
}

PikaObj* os_listdir(PikaObj* self, char* path) {
    return os_listdir_platform(path);
}

void os_mkdir(PikaObj* self, char* path, PikaTuple* mode) {
    int iMode = 0;
    if (pikaTuple_getSize(mode) == 0) {
        iMode = 0777;
    } else {
        iMode = pikaTuple_getInt(mode, 0);
    }
    if (os_mkdir_platform(iMode, path) < 0) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO_ERROR);
        pika_platform_printf("mkdir error\r\n");
    }
}

void os_chdir(PikaObj* self, char* path) {
    if (os_chdir_platform(path) < 0) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO_ERROR);
        pika_platform_printf("chdir error\r\n");
    }
}

void os_rmdir(PikaObj* self, char* path) {
    if (os_rmdir_platform(path) < 0) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO_ERROR);
        pika_platform_printf("rmdir error\r\n");
    }
}

void os_remove(PikaObj* self, char* filename) {
    if (os_remove_platform(filename) < 0) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO_ERROR);
        pika_platform_printf("remove error\r\n");
    }
}

void os_rename(PikaObj* self, char* old, char* new) {
    if (os_rename_platform(old, new) < 0) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO_ERROR);
        pika_platform_printf("rename error\r\n");
    }
}
