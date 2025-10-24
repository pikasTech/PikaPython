#include "PikaStdData_FILEIO.h"
#include <stdio.h>
#include "PikaCompiler.h"
#include "PikaStdData_List.h"

int PikaStdData_FILEIO_init(PikaObj* self, char* path, char* mode) {
    if (obj_isArgExist(self, "_f")) {
        /* already initialized */
        return 0;
    }
    if (strIsStartWith(path, "/pikafs/")) {
        pikafs_FILE* f = pikafs_fopen(path + 8, "rb");
        if (f == NULL) {
            return 1;
        }
        obj_setInt(self, "pikafs", PIKA_TRUE);
        obj_setPtr(self, "_f", f);
        obj_setStr(self, "_mode", mode);
        return 0;
    }
    FILE* f = __platform_fopen(path, mode);
    if (f == NULL) {
        return 1;
    }
    obj_setPtr(self, "_f", f);
    obj_setStr(self, "_mode", mode);
    return 0;
}

void PikaStdData_FILEIO_close(PikaObj* self) {
    if (PIKA_TRUE == obj_getInt(self, "pikafs")) {
        pikafs_FILE* f = obj_getPtr(self, "_f");
        if (NULL == f) {
            return;
        }
        pikafs_fclose(f);
        obj_setPtr(self, "_f", NULL);
        return;
    }
    FILE* f = obj_getPtr(self, "_f");
    if (NULL == f) {
        return;
    }
    __platform_fclose(f);
    obj_setPtr(self, "_f", NULL);
}

Arg* PikaStdData_FILEIO_read(PikaObj* self, PikaTuple* size_) {
    int size = 0;
    if (pikaTuple_getSize(size_) == 0) {
        size = -1;
    } else {
        size = pikaTuple_getInt(size_, 0);
    }
    if (size <= 0) {
        /* read all */
        size = PIKA_READ_FILE_BUFF_SIZE;
    }
    Arg* buf_arg = arg_newBytes(NULL, size);
    uint8_t* buf = arg_getBytes(buf_arg);
    int n = 0;
    /* read */
    if (PIKA_TRUE == obj_getInt(self, "pikafs")) {
        pikafs_FILE* f = obj_getPtr(self, "_f");
        if (NULL == f) {
            return NULL;
        }
        n = pikafs_fread(buf, 1, size, f);
    } else {
        FILE* f = obj_getPtr(self, "_f");
        if (f == NULL) {
            return NULL;
        }
        n = __platform_fread(buf, 1, size, f);
    }
    if (n < size) {
        /* EOF */
        buf[n] = '\0';
    }
    char* mode = obj_getStr(self, "_mode");
    if (strIsContain(mode, 'b')) {
        /* binary */
        Arg* res = arg_newBytes(buf, n);
        arg_deinit(buf_arg);
        return res;
    } else {
        /* text */
        Arg* res = arg_newStr((char*)buf);
        arg_deinit(buf_arg);
        return res;
    }
}

int PikaStdData_FILEIO_write(PikaObj* self, Arg* s) {
    if (PIKA_TRUE == obj_getInt(self, "pikafs")) {
        return 1;
    }
    FILE* f = obj_getPtr(self, "_f");
    int res = -1;
    if (f == NULL) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO);
        obj_setSysOut(self, "Error: can't write to file");
        return res;
    }
    char* mode = obj_getStr(self, "_mode");
    if (strIsContain(mode, 'b')) {
        if (arg_getType(s) != ARG_TYPE_BYTES) {
            obj_setSysOut(
                self, "TypeError: a bytes-like object is required, not 'str'");
            obj_setErrorCode(self, PIKA_RES_ERR_INVALID_PARAM);
            return -1;
        }
        /* binary */
        res = __platform_fwrite(arg_getBytes(s), 1, arg_getBytesSize(s), f);
    } else {
        /* text */
        char* str = arg_getStr(s);
        res = __platform_fwrite(str, 1, strlen(str), f);
    }
    return res;
}

int PikaStdData_FILEIO_seek(PikaObj* self, int offset, PikaTuple* fromwhere) {
    FILE* f = obj_getPtr(self, "_f");
    if (f == NULL) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO);
        obj_setSysOut(self, "Error: can't seek in file");
        return -1;
    }
    if (pikaTuple_getSize(fromwhere) == 1) {
        int whence = pikaTuple_getInt(fromwhere, 0);
        __platform_fseek(f, offset, whence);
        return __platform_ftell(f);
    }
    __platform_fseek(f, offset, 0);
    return __platform_ftell(f);
}

int PikaStdData_FILEIO_tell(PikaObj* self) {
    FILE* f = obj_getPtr(self, "_f");
    if (f == NULL) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO);
        obj_setSysOut(self, "Error: can't tell in file");
        return -1;
    }
    return __platform_ftell(f);
}

char* PikaStdData_FILEIO_readline(PikaObj* self) {
    FILE* f = obj_getPtr(self, "_f");
    if (f == NULL) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO);
        obj_setSysOut(self, "Error: can't read line from file");
        return NULL;
    }
    int line_buff_size = 16;
    int line_size = 0;
    char* line_buff = (char*)pika_platform_malloc(line_buff_size);
    pika_platform_memset(line_buff, 0, line_buff_size);
    while (1) {
        char char_buff[2] = {0};
        int n = __platform_fread(char_buff, 1, 1, f);
        if (n == 0) {
            /* EOF */
            if (strGetSize(line_buff) == 0) {
                pika_platform_free(line_buff);
                return NULL;
            }
            obj_setStr(self, "@sc", line_buff);
            pika_platform_free(line_buff);
            return obj_getStr(self, "@sc");
        }
        line_size++;
        if (line_size >= line_buff_size) {
            /* line too long, double buff and realloc */
            line_buff_size *= 2;
            line_buff = (char*)pika_reallocn(line_buff, line_buff_size / 2,
                                             line_buff_size);
            pika_platform_memset(line_buff + line_size, 0,
                                 line_buff_size - line_size);
        }
        if (char_buff[0] == '\n') {
            /* end of line */
            line_buff[line_size - 1] = '\n';
            obj_setStr(self, "@sc", line_buff);
            pika_platform_free(line_buff);
            return obj_getStr(self, "@sc");
        }
        line_buff[line_size - 1] = char_buff[0];
    }
}

PikaObj* PikaStdData_FILEIO_readlines(PikaObj* self) {
    FILE* f = obj_getPtr(self, "_f");
    if (f == NULL) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO);
        obj_setSysOut(self, "Error: can't read lines from file");
        return NULL;
    }
    PikaObj* line_list = newNormalObj(New_PikaStdData_List);
    PikaStdData_List___init__(line_list);
    while (1) {
        char* line = PikaStdData_FILEIO_readline(self);
        if (line == NULL) {
            break;
        }
        Arg* arg_str = arg_newStr(line);
        PikaStdData_List_append(line_list, arg_str);
        arg_deinit(arg_str);
    }
    return line_list;
}

void PikaStdData_FILEIO_writelines(PikaObj* self, PikaObj* lines) {
    FILE* f = obj_getPtr(self, "_f");
    if (f == NULL) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO);
        obj_setSysOut(self, "Error: can't write lines to file");
        return;
    }
    for (size_t i = 0; i < pikaList_getSize(lines); i++) {
        char* line = pikaList_getStr(lines, i);
        Arg* arg_str = arg_newStr(line);
        PikaStdData_FILEIO_write(self, arg_str);
        arg_deinit(arg_str);
    }
    return;
}
