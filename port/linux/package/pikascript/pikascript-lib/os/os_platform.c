#include "os_platform.h"

int os_getFileSize(PikaObj* fd) {
    FILE* fp = obj_getPtr(fd, "fd");
    if (fp != NULL) {
        int ret = pika_platform_fseek(fp, 0, SEEK_END);
        if (ret == 0) {
            ret = pika_platform_ftell(fp);
            return ret;
        }
    }
    return -2;
}

PikaObj* os_open_platform(char* filename, int flags) {
    char file_flag[4] = {0};
    int index = 0;
    char dirpath[256] = {0};
    pika_platform_memcpy(dirpath + strlen(dirpath), filename, strlen(filename));

    if (FILE_RDONLY == (flags & FILE_RDONLY)) {
        file_flag[0] = 'r';
        if (FILE_CREAT == (flags & FILE_CREAT)) {
            file_flag[1] = 'w';
        }
    }

    if (FILE_RDWR == (flags & FILE_RDWR)) {
        pika_platform_memcpy(file_flag, "r+", 2);
        if (FILE_CREAT == (flags & FILE_CREAT)) {
            pika_platform_memcpy(file_flag, "w+", 2);
        }
    }

    if (FILE_WRONLY == (flags & FILE_WRONLY))
        file_flag[index] = 'w';

    if (FILE_APPEND == (flags & FILE_APPEND))
        memcpy(file_flag, "a+", 2);

    FILE* fp = pika_platform_fopen(dirpath, file_flag);
    if (fp != NULL) {
        PikaObj* file_obj = newNormalObj(New_TinyObj);
        obj_setPtr(file_obj, "fd", fp);
        return file_obj;
    } else {
        return NULL;
    }
}

char* os_read_platform(PikaObj* self, PikaObj* fd, int len) {
    char* buf = NULL;
    int size = 0;
    FILE* fp = obj_getPtr(fd, "fd");
    if (fp != NULL) {
        buf = pika_platform_malloc(len);
        pika_platform_memset(buf, 0x00, len);
        size = pika_platform_fread(buf, 1, len, fp);

        if (size > 0) {
            obj_setStr(self, "os_file_read", buf);
            pika_platform_free(buf);
            return obj_getStr(self, "os_file_read");
        }
        pika_platform_free(buf);
    }

    return "";
}

int os_write_platform(uint8_t* buf, size_t len, PikaObj* fd) {
    int size = 0;
    FILE* fp = obj_getPtr(fd, "fd");
    if (fp != NULL) {
        size = pika_platform_fwrite(buf, 1, len, fp);
        return size;
    }
    return 0;
}

int os_lseek_platform(PikaObj* fd, int how, int pos) {
    FILE* fp = obj_getPtr(fd, "fd");
    if (fp != NULL) {
        int ret = pika_platform_fseek(fp, pos, how);
        return ret;

    } else
        return -2;
}

int os_close_platform(PikaObj* fd) {
    FILE* fp = obj_getPtr(fd, "fd");
    if (fp != NULL) {
        int ret = pika_platform_fclose(fp);
        return ret;

    } else
        return -2;
}

char* os_getcwd_platform(PikaObj* self) {
    char dirpath[256] = {0};
    if (pika_platform_getcwd(dirpath, sizeof(dirpath)) == NULL) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO_ERROR);
        obj_setStr(self, "os_current_path", "");
        return NULL;
    }
    obj_setStr(self, "os_current_path", dirpath);
    return obj_getStr(self, "os_current_path");
}

PikaObj* os_listdir_platform(char* path) {
    int count = 0;
    char** filenames = NULL;

    filenames = pika_platform_listdir(path, &count);
    PikaObj* list = newNormalObj(New_PikaStdData_List);
    PikaStdData_List___init__(list);

    for (int i = 0; i < count; i++) {
        Arg* arg = arg_setStr(NULL, "", filenames[i]);
        PikaStdData_List_append(list, arg);
        arg_deinit(arg);
        pika_platform_free(filenames[i]);
    }
    pika_platform_free(filenames);

    return list;
}

int os_mkdir_platform(int mode, char* path) {
    char dirpath[256] = {0};
    int ret = 0;
    pika_platform_memcpy(dirpath + strlen(dirpath), path, strlen(path));
    ret = pika_platform_mkdir(dirpath, mode);
    return ret;
}

int os_chdir_platform(char* path) {
    int ret = 0;
    ret = pika_platform_chdir(path);
    if (ret == 0)
        ret = PIKA_TRUE;
    else
        ret = PIKA_FALSE;
    return ret;
}

int os_rmdir_platform(char* path) {
    int ret = 0;
    char dirpath[256] = {0};
    pika_platform_memcpy(dirpath + strlen(dirpath), path, strlen(path));

    ret = pika_platform_rmdir(dirpath);
    return ret;
}

int os_remove_platform(char* filename) {
    int ret = 0;
    char dirpath[256] = {0};
    pika_platform_memcpy(dirpath + strlen(dirpath), filename, strlen(filename));
    ret = pika_platform_remove(dirpath);
    return ret;
}

int os_rename_platform(char* old, char* new) {
    if (NULL == old || NULL == new) {
        return -1;
    }
    if (0 != pika_platform_rename(old, new)) {
        return -1;
    }
    return 0;
}
