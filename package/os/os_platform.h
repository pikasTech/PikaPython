#ifndef _OS_INTERFACE_H
#define _OS_INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"
#include "PikaStdData_List.h"
#include "TinyObj.h"
#ifdef _WIN32
#include <io.h>
#include <direct.h>
#include <windows.h>
#elif defined(__linux) || PIKA_LINUX_COMPATIBLE
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "unistd.h"
#endif
enum {
    FILE_RDONLY = 0x00,
    FILE_WRONLY = 0x01,
    FILE_RDWR = 0x02,
    FILE_CREAT = 0x0100,
    FILE_APPEND = 0x02000,
};

PikaObj* os_open_platform(char* filename, int flags);
char* os_read_platform(PikaObj* self, PikaObj* fd, int len);
int os_write_platform(uint8_t* buf, size_t len, PikaObj* fd);
int os_lseek_platform(PikaObj* fd, int how, int pos);
int os_close_platform(PikaObj* fd);
char* os_getcwd_platform(PikaObj* self);
PikaObj* os_listdir_platform(char* path);
int os_mkdir_platform(int mode, char* path);
int os_chdir_platform(char* path);
int os_rmdir_platform(char* path);
int os_remove_platform(char* filename);
int os_getFileSize(PikaObj* fd);
int os_rename_platform(char* old, char* new);

#endif
