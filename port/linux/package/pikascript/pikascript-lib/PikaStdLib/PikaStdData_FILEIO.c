#include "PikaStdData_FILEIO.h"
#include <stdio.h>
#include "PikaStdData_List.h"

int PikaStdData_FILEIO_init(PikaObj* self, char* path, char* mode) {
    if (obj_isArgExist(self, "_f")) {
        /* already initialized */
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
    FILE* f = obj_getPtr(self, "_f");
    if (f == NULL) {
        return;
    }
    __platform_fclose(f);
    obj_setPtr(self, "_f", NULL);
}

Arg* PikaStdData_FILEIO_read(PikaObj* self, PikaTuple* size_) {
    int size = 0;
    if (pikaTuple_getSize(size_) == 0) {
        size = -1;
    }else{
        size = pikaTuple_getInt(size_, 0);
    }
    if (size <= 0) {
        /* read all */
        size = PIKA_READ_FILE_BUFF_SIZE;
    }
    FILE* f = obj_getPtr(self, "_f");
    if (f == NULL) {
        return NULL;
    }
    Arg* buf_arg = arg_newBytes(NULL, size);
    uint8_t* buf = arg_getBytes(buf_arg);
    /* read */
    int n = __platform_fread(buf, 1, size, f);
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
    FILE* f = obj_getPtr(self, "_f");
    int res = -1;
    if (f == NULL) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO);
        __platform_printf("Error: can't write to file\n");
        return res;
    }
    char* mode = obj_getStr(self, "_mode");
    if (strIsContain(mode, 'b')) {
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
        __platform_printf("Error: can't seek in file\n");
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
        __platform_printf("Error: can't tell in file\n");
        return -1;
    }
    return __platform_ftell(f);
}

char* PikaStdData_FILEIO_readline(PikaObj* self) {
    FILE* f = obj_getPtr(self, "_f");
    if (f == NULL) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO);
        __platform_printf("Error: can't read line from file\n");
        return NULL;
    }
    obj_setBytes(self, "_line_buff", NULL, PIKA_LINE_BUFF_SIZE);
    char* line_buff = (char*)obj_getBytes(self, "_line_buff");
    while (1) {
        char char_buff[2] = {0};
        int n = __platform_fread(char_buff, 1, 1, f);
        if (n == 0) {
            /* EOF */
            return NULL;
        }
        if (char_buff[0] == '\n') {
            /* end of line */
            strAppend(line_buff, char_buff);
            return line_buff;
        }
        if (strGetSize(line_buff) >= PIKA_LINE_BUFF_SIZE) {
            /* line too long */
            obj_setErrorCode(self, PIKA_RES_ERR_IO);
            __platform_printf("Error: line too long\n");
            return NULL;
        }
        strAppend(line_buff, char_buff);
    }
}

PikaObj* PikaStdData_FILEIO_readlines(PikaObj* self) {
    FILE* f = obj_getPtr(self, "_f");
    if (f == NULL) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO);
        __platform_printf("Error: can't read lines from file\n");
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
        __platform_printf("Error: can't write lines to file\n");
        return;
    }
    PikaList* list = obj_getPtr(lines, "list");
    if (list == NULL) {
        obj_setErrorCode(self, PIKA_RES_ERR_IO);
        __platform_printf("Error: can't write lines to file\n");
        return;
    }
    for (size_t i = 0; i < pikaList_getSize(list); i++) {
        char* line = pikaList_getStr(list, i);
        Arg* arg_str = arg_newStr(line);
        PikaStdData_FILEIO_write(self, arg_str);
        arg_deinit(arg_str);
    }
    return;
}
