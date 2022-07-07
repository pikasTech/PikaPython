#include "PikaStdLib_SysObj.h"
#include "PikaStdLib_RangeObj.h"
#include "PikaStdLib_StringObj.h"
#include "PikaVM.h"
#include "dataStrs.h"

void PikaStdLib_SysObj_remove(PikaObj* self, char* argPath) {
    obj_setErrorCode(self, 0);
    int32_t res = obj_removeArg(self, argPath);
    if (1 == res) {
        obj_setSysOut(self, "[error] del: object no found.");
        obj_setErrorCode(self, 1);
        return;
    }
    if (2 == res) {
        obj_setSysOut(self, "[error] del: arg not match.");
        obj_setErrorCode(self, 2);
        return;
    }
}

Arg* PikaStdLib_SysObj_type(PikaObj* self, Arg* arg) {
    if (NULL == arg) {
        obj_setSysOut(self, "[error] type: arg no found.");
        obj_setErrorCode(self, 1);
        return arg_setNull(NULL);
    }
    ArgType type = arg_getType(arg);
    if (ARG_TYPE_INT == type) {
        return arg_setStr(NULL, "", "<class 'int'>");
    }
    if (ARG_TYPE_FLOAT == type) {
        return arg_setStr(NULL, "", "<class 'float'>");
    }
    if (ARG_TYPE_STRING == type) {
        return arg_setStr(NULL, "", "<class 'str'>");
    }
    if (argType_isObject(type)) {
        PikaObj* obj = arg_getPtr(arg);
        NewFun clsptr = obj_getClass(obj);
        PikaObj* New_PikaStdData_List(Args * args);
        if (clsptr == New_PikaStdData_List) {
            return arg_setStr(NULL, "", "<class 'list'>");
        }
        /* dict */
        PikaObj* New_PikaStdData_Dict(Args * args);
        if (clsptr == New_PikaStdData_Dict) {
            return arg_setStr(NULL, "", "<class 'dict'>");
        }
        return arg_setStr(NULL, "", "<class 'object'>");
    }
    if (ARG_TYPE_OBJECT_META == type) {
        return arg_setStr(NULL, "", "<class 'meta object'>");
    }
    if (ARG_TYPE_BYTES == type) {
        return arg_setStr(NULL, "", "<class 'bytes'>");
    }
    if (ARG_TYPE_METHOD_NATIVE == type) {
        return arg_setStr(NULL, "", "<class 'buitin_function_or_method'>");
    }
    if (ARG_TYPE_METHOD_OBJECT == type) {
        return arg_setStr(NULL, "", "<class 'method'>");
    }
    if (ARG_TYPE_METHOD_STATIC == type) {
        return arg_setStr(NULL, "", "<class 'function'>");
    }
    if (ARG_TYPE_NONE == type) {
        return arg_setStr(NULL, "", "<class 'NoneType'>");
    }
    return arg_setNull(NULL);
}

double PikaStdLib_SysObj_float(PikaObj* self, Arg* arg) {
    ArgType type = arg_getType(arg);
    if (ARG_TYPE_INT == type) {
        return (float)arg_getInt(arg);
    }
    if (ARG_TYPE_FLOAT == type) {
        return (float)arg_getFloat(arg);
    }
    obj_setSysOut(self, "[error] convert to double type faild.");
    obj_setErrorCode(self, 1);
    return -99999.99999;
}

int PikaStdLib_SysObj_int(PikaObj* self, Arg* arg) {
    ArgType type = arg_getType(arg);
    if (ARG_TYPE_INT == type) {
        return (int)arg_getInt(arg);
    }
    if (ARG_TYPE_FLOAT == type) {
        return (int)arg_getFloat(arg);
    }
    if (ARG_TYPE_STRING == type) {
        return (int)fast_atoi(arg_getStr(arg));
    }
    if (ARG_TYPE_BYTES == type) {
        size_t size = arg_getBytesSize(arg);
        if (size != 1) {
            obj_setSysOut(self, "ValueError: invalid literal for int()");
            obj_setErrorCode(self, 1);
            return -999999999;
        }
        uint8_t val = *arg_getBytes(arg);
        return val;
    }
    obj_setSysOut(self, "[error] convert to int type faild.");
    obj_setErrorCode(self, 1);
    return -999999999;
}

char* PikaStdLib_SysObj_str(PikaObj* self, Arg* arg) {
    obj_removeArg(self, "__buf");
    ArgType type = arg_getType(arg);
    Args buffs = {0};
    char* res = NULL;
    if (ARG_TYPE_INT == type) {
        int val = arg_getInt(arg);
        res = strsFormat(&buffs, 11, "%d", val);
        goto exit;
    }
    if (ARG_TYPE_FLOAT == type) {
        double val = arg_getFloat(arg);
        res = strsFormat(&buffs, 11, "%f", val);
        goto exit;
    }
    if (ARG_TYPE_BYTES == type) {
        res = (char*)arg_getBytes(arg);
        goto exit;
    }
    if (ARG_TYPE_STRING == type) {
        res = arg_getStr(arg);
    }
    if (ARG_TYPE_NONE == type) {
        res = "None";
    }
    if (argType_isObject(type)) {
        res = obj_toStr(arg_getPtr(arg));
        if (NULL != res) {
            goto exit;
        }
    }
exit:
    obj_setStr(self, "__buf", res);
    strsDeinit(&buffs);
    return obj_getStr(self, "__buf");
}

Arg* PikaStdLib_SysObj_iter(PikaObj* self, Arg* arg) {
    /* a String, return a StringObj */
    if (ARG_TYPE_STRING == arg_getType(arg)) {
        obj_setStr(self, "_sobj", arg_getStr(arg));
        return arg_newMetaObj(New_PikaStdLib_StringObj);
    }
    /* a MATE object, return itself */
    if (ARG_TYPE_OBJECT_META == arg_getType(arg)) {
        return arg_copy(arg);
    }
    /* object */
    if (argType_isObject(arg_getType(arg))) {
        PikaObj* arg_obj = arg_getPtr(arg);
        NewFun _clsptr = (NewFun)obj_getPtr(arg_obj, "_clsptr");
        if (_clsptr == New_PikaStdLib_RangeObj) {
            /* found RangeObj, return directly */
            return arg_copy(arg);
        }
        // pikaVM_runAsm(arg_obj,
        //               "B0\n"
        //               "0 RUN __iter__\n"
        //               "0 OUT __res\n");
        const uint8_t bytes[] = {
            0x08, 0x00, /* instruct array size */
            0x00, 0x82, 0x01, 0x00, 0x00, 0x04, 0x0a, 0x00, /* instruct array */
            0x10, 0x00, /* const pool size */
            0x00, 0x5f, 0x5f, 0x69, 0x74, 0x65, 0x72, 0x5f,
            0x5f, 0x00, 0x5f, 0x5f, 0x72, 0x65, 0x73, 0x00, /* const pool */
        };
        pikaVM_runByteCode(arg_obj, (uint8_t*)bytes);
        Arg* res = arg_copy(args_getArg(arg_obj->list, "__res"));
        obj_removeArg(arg_obj, "__res");
        return res;
    }
    return arg_setNull(NULL);
}

Arg* PikaStdLib_SysObj_range(PikaObj* self, int a1, int a2) {
    /* set template arg to create rangeObj */
    Arg* obj_arg = arg_newDirectObj(New_PikaStdLib_RangeObj);
    obj_setInt(arg_getPtr(obj_arg), "a1", a1);
    obj_setInt(arg_getPtr(obj_arg), "a2", a2);
    return obj_arg;
}

Arg* PikaStdLib_SysObj___get__(PikaObj* self, Arg* key, Arg* obj) {
    ArgType obj_type = arg_getType(obj);
    int index = 0;
    if (ARG_TYPE_INT == arg_getType(key)) {
        index = arg_getInt(key);
    }
    if (ARG_TYPE_STRING == obj_type) {
        char* str_pyload = arg_getStr(obj);
        char char_buff[] = " ";
        if (index < 0) {
            index = strGetSize(str_pyload) + index;
        }
        char_buff[0] = str_pyload[index];
        return arg_setStr(NULL, "", char_buff);
    }
    if (ARG_TYPE_BYTES == obj_type) {
        uint8_t* bytes_pyload = arg_getBytes(obj);
        uint8_t byte_buff[] = " ";
        if (index < 0) {
            index = arg_getBytesSize(obj) + index;
        }
        byte_buff[0] = bytes_pyload[index];
        return arg_setBytes(NULL, "", byte_buff, 1);
    }
    if (argType_isObject(obj_type)) {
        PikaObj* arg_obj = arg_getPtr(obj);
        obj_setArg(arg_obj, "__key", key);
        // pikaVM_runAsm(arg_obj,
        //               "B0\n"
        //               "1 REF __key\n"
        //               "0 RUN __get__\n"
        //               "0 OUT __res\n");
        const uint8_t bytes[] = {
            0x0c, 0x00, /* instruct array size */
            0x10, 0x81, 0x01, 0x00, 0x00, 0x02, 0x07, 0x00, 0x00, 0x04, 0x0f,
            0x00,
            /* instruct array */
            0x15, 0x00, /* const pool size */
            0x00, 0x5f, 0x5f, 0x6b, 0x65, 0x79, 0x00, 0x5f, 0x5f, 0x67, 0x65,
            0x74, 0x5f, 0x5f, 0x00, 0x5f, 0x5f, 0x72, 0x65, 0x73,
            0x00, /* const pool */
        };
        pikaVM_runByteCode(arg_obj, (uint8_t*)bytes);
        return arg_copy(args_getArg(arg_obj->list, "__res"));
    }
    return arg_setNull(NULL);
}

Arg* PikaStdLib_SysObj___set__(PikaObj* self, Arg* key, Arg* obj, Arg* val) {
    ArgType obj_type = arg_getType(obj);
    if (ARG_TYPE_STRING == obj_type) {
        int index = arg_getInt(key);
        char* str_val = arg_getStr(val);
        char* str_pyload = arg_getStr(obj);
        str_pyload[index] = str_val[0];
        return arg_setStr(NULL, "", str_pyload);
    }
    if (ARG_TYPE_BYTES == obj_type) {
        int index = arg_getInt(key);
        uint8_t byte_val = 0;
        if (ARG_TYPE_BYTES == arg_getType(val)) {
            uint8_t* bytes_val = arg_getBytes(val);
            byte_val = bytes_val[0];
        }
        if (ARG_TYPE_INT == arg_getType(val)) {
            byte_val = arg_getInt(val);
        }
        uint8_t* bytes_pyload = arg_getBytes(obj);
        size_t bytes_len = arg_getBytesSize(obj);
        bytes_pyload[index] = byte_val;
        return arg_setBytes(NULL, "", bytes_pyload, bytes_len);
    }
    if (argType_isObject(obj_type)) {
        PikaObj* arg_obj = arg_getPtr(obj);
        obj_setArg(arg_obj, "__key", key);
        obj_setArg(arg_obj, "__val", val);
        // pikaVM_runAsm(arg_obj,
        //               "B0\n"
        //               "1 REF __key\n"
        //               "1 REF __val\n"
        //               "0 RUN __set__\n");
        const uint8_t bytes[] = {
            0x0c, 0x00, /* instruct array size */
            0x10, 0x81, 0x01, 0x00, 0x10, 0x01, 0x07, 0x00, 0x00, 0x02, 0x0d,
            0x00,
            /* instruct array */
            0x15, 0x00, /* const pool size */
            0x00, 0x5f, 0x5f, 0x6b, 0x65, 0x79, 0x00, 0x5f, 0x5f, 0x76, 0x61,
            0x6c, 0x00, 0x5f, 0x5f, 0x73, 0x65, 0x74, 0x5f, 0x5f,
            0x00, /* const pool */
        };
        pikaVM_runByteCode(arg_obj, (uint8_t*)bytes);
        return arg_setRef(NULL, "", arg_obj);
    }
    return NULL;
}

int PikaStdLib_SysObj_len(PikaObj* self, Arg* arg) {
    if (ARG_TYPE_STRING == arg_getType(arg)) {
        return strGetSize(arg_getStr(arg));
    }
    if (ARG_TYPE_BYTES == arg_getType(arg)) {
        return arg_getBytesSize(arg);
    }

    if (argType_isObject(arg_getType(arg))) {
        PikaObj* arg_obj = arg_getPtr(arg);
        Arg* method_arg = obj_getMethodArg(arg_obj, "__len__");
        if (NULL != method_arg) {
            arg_deinit(method_arg);

            /* clang-format off */
        PIKA_PYTHON(
        __res = __len__()
        )
            /* clang-format on */
            const uint8_t bytes[] = {
                0x08, 0x00, /* instruct array size */
                0x00, 0x82, 0x01, 0x00, 0x00, 0x04, 0x09, 0x00, /* instruct
                                                                   array */
                0x0f, 0x00, /* const pool size */
                0x00, 0x5f, 0x5f, 0x6c, 0x65, 0x6e, 0x5f, 0x5f, 0x00,
                0x5f, 0x5f, 0x72, 0x65, 0x73, 0x00, /* const pool */
            };
            pikaVM_runByteCode(arg_obj, (uint8_t*)bytes);
            return obj_getInt(arg_obj, "__res");
        }
    }

    obj_setErrorCode(self, 1);
    __platform_printf("[Error] len: arg type not support\r\n");
    return -1;
}

Arg* PikaStdLib_SysObj_list(PikaObj* self) {
#if PIKA_BUILTIN_LIST_ENABLE
    PikaObj* New_PikaStdData_List(Args * args);
    return arg_newDirectObj(New_PikaStdData_List);
#else
    obj_setErrorCode(self, 1);
    __platform_printf("[Error] built-in list is not enabled.\r\n");
    return arg_setNull(NULL);
#endif
}

Arg* PikaStdLib_SysObj_dict(PikaObj* self) {
#if PIKA_BUILTIN_DICT_ENABLE
    PikaObj* New_PikaStdData_Dict(Args * args);
    return arg_newDirectObj(New_PikaStdData_Dict);
#else
    obj_setErrorCode(self, 1);
    __platform_printf("[Error] built-in dist is not enabled.\r\n");
    return arg_setNull(NULL);
#endif
}

char* PikaStdLib_SysObj_hex(PikaObj* self, int val) {
    char buff[PIKA_SPRINTF_BUFF_SIZE] = {0};
    if (val > 0) {
        __platform_sprintf(buff, "0x%02x", val);
    } else {
        __platform_sprintf(buff, "-0x%02x", -val);
    }
    /* load the string from stack to heap */
    obj_setStr(self, "__buf", buff);
    return obj_getStr(self, "__buf");
}

int PikaStdLib_SysObj_ord(PikaObj* self, char* val) {
    return (int)val[0];
}

char* PikaStdLib_SysObj_chr(PikaObj* self, int val) {
    char buff[PIKA_SPRINTF_BUFF_SIZE] = {0};
    char to_str[] = "0";
    to_str[0] = val;
    __platform_sprintf(buff, "%s", to_str);
    /* load the string from stack to heap */
    obj_setStr(self, "__buf", buff);
    return obj_getStr(self, "__buf");
}

Arg* PikaStdLib_SysObj_bytes(PikaObj* self, Arg* val) {
    ArgType type = arg_getType(val);
    if (ARG_TYPE_INT == type) {
        int size = arg_getInt(val);
        /* src is NULL so the bytes are all '\0' */
        Arg* bytes = arg_setBytes(NULL, "", NULL, size);
        return bytes;
    }
    if (ARG_TYPE_BYTES == type) {
        return arg_copy(val);
    }
    if (ARG_TYPE_STRING == type) {
        int size = strGetSize(arg_getStr(val));
        Arg* bytes = arg_setBytes(NULL, "", (uint8_t*)arg_getStr(val), size);
        return bytes;
    }
    obj_setErrorCode(self, 1);
    __platform_printf("Error: input arg type not supported.\r\n");
    return arg_setNull(NULL);
}

Arg* PikaStdLib_SysObj___slice__(PikaObj* self,
                                 Arg* end,
                                 Arg* obj,
                                 Arg* start,
                                 int step) {
#if PIKA_SYNTEX_SLICE_ENABLE
    /* No interger index only support __get__ */
    if (!(arg_getType(start) == ARG_TYPE_INT &&
          arg_getType(end) == ARG_TYPE_INT)) {
        return PikaStdLib_SysObj___get__(self, start, obj);
    }

    int start_i = arg_getInt(start);
    int end_i = arg_getInt(end);

    /* __slice__ is equal to __get__ */
    if (end_i - start_i == 1) {
        return PikaStdLib_SysObj___get__(self, start, obj);
    }

    if (ARG_TYPE_STRING == arg_getType(obj)) {
        size_t len = strGetSize(arg_getStr(obj));
        if (start_i < 0) {
            start_i += len;
        }
        if (end_i < 0) {
            end_i += len + 1;
        }
        Arg* sliced_arg = arg_setStr(NULL, "", "");
        for (int i = start_i; i < end_i; i++) {
            Arg* i_arg = arg_setInt(NULL, "", i);
            Arg* item_arg = PikaStdLib_SysObj___get__(self, i_arg, obj);
            sliced_arg = arg_strAppend(sliced_arg, arg_getStr(item_arg));
            arg_deinit(item_arg);
            arg_deinit(i_arg);
        }
        return sliced_arg;
    }

    if (ARG_TYPE_BYTES == arg_getType(obj)) {
        size_t len = arg_getBytesSize(obj);
        if (start_i < 0) {
            start_i += len;
        }
        if (end_i < 0) {
            end_i += len + 1;
        }
        Arg* sliced_arg = arg_setBytes(NULL, "", NULL, 0);
        for (int i = start_i; i < end_i; i++) {
            Arg* i_arg = arg_setInt(NULL, "", i);
            Arg* item_arg = PikaStdLib_SysObj___get__(self, i_arg, obj);
            uint8_t* bytes_origin = arg_getBytes(sliced_arg);
            size_t size_origin = arg_getBytesSize(sliced_arg);
            Arg* sliced_arg_new = arg_setBytes(NULL, "", NULL, size_origin + 1);
            __platform_memcpy(arg_getBytes(sliced_arg_new), bytes_origin,
                              size_origin);
            __platform_memcpy(arg_getBytes(sliced_arg_new) + size_origin,
                              arg_getBytes(item_arg), 1);
            arg_deinit(sliced_arg);
            sliced_arg = sliced_arg_new;
            arg_deinit(item_arg);
            arg_deinit(i_arg);
        }
        return sliced_arg;
    }
    return arg_setNull(NULL);
#else
    return PikaStdLib_SysObj___get__(self, start, obj);
#endif
}

static void __print_arg(PikaObj* self, Arg* val) {
    obj_setErrorCode(self, 0);
    ArgType arg_type = arg_getType(val);
    if (NULL != val) {
        if (arg_getType(val) == ARG_TYPE_BYTES) {
            arg_printBytes(val);
            return;
        }
    }
    if (argType_isObject(arg_type)) {
        char* to_str = obj_toStr(arg_getPtr(val));
        if (NULL != to_str) {
            __platform_printf("%s\r\n", to_str);
            return;
        }
    }
    Args* print_args = New_args(NULL);
    arg_setName(val, "val");
    args_setArg(print_args, arg_copy(val));
    char* res = args_print(print_args, "val");
    if (NULL == res) {
        obj_setSysOut(self, "[error] print: can not print val");
        obj_setErrorCode(self, 1);
        args_deinit(print_args);
        return;
    }
    obj_setStr(self, "_buf", res);
    args_deinit(print_args);
}

void PikaStdLib_SysObj_print(PikaObj* self, PikaTuple* val) {
    int arg_size = tuple_getSize(val);
    Arg* print_out_arg = arg_setStr(NULL, "", "");
    PIKA_BOOL is_get_print = PIKA_FALSE;
    for (int i = 0; i < arg_size; i++) {
        Arg* arg = tuple_getArg(val, i);
        __print_arg(self, arg);
        char* item = obj_getStr(self, "_buf");
        if (NULL != item) {
            is_get_print = PIKA_TRUE;
            print_out_arg = arg_strAppend(print_out_arg, item);
            if (i < arg_size - 1) {
                print_out_arg = arg_strAppend(print_out_arg, " ");
            }
            obj_removeArg(self, "_buf");
        }
    }
    if (PIKA_TRUE == is_get_print) {
        __platform_printf("%s\r\n", arg_getStr(print_out_arg));
    }
    arg_deinit(print_out_arg);
}

void PikaStdLib_SysObj_printNoEnd(PikaObj* self, Arg* val) {
    obj_setErrorCode(self, 0);
    Args* print_args = New_args(NULL);
    args_setArg(print_args, arg_copy(val));
    char* res = args_print(print_args, "val");
    if (NULL == res) {
        obj_setSysOut(self, "[error] print: can not print val");
        obj_setErrorCode(self, 1);
        args_deinit(print_args);
        return;
    }
    /* not empty */
    __platform_printf("%s", res);
    args_deinit(print_args);
}

char* PikaStdLib_SysObj_cformat(PikaObj* self, char* fmt, PikaTuple* var) {
#if PIKA_SYNTEX_FORMAT_ENABLE
    Args buffs = {0};
    pikaMemMaxReset();
    char* res = strsFormatList(&buffs, fmt, &var->super);
    obj_setStr(self, "_buf", res);
    res = obj_getStr(self, "_buf");
    strsDeinit(&buffs);
    return res;
#else
    obj_setErrorCode(self, 1);
    __platform_printf("[Error] PIKA_SYNTEX_FORMAT_ENABLE is not enabled.\r\n");
    return NULL;
#endif
}

int PikaStdLib_SysObj_id(PikaObj* self, Arg* obj) {
    uintptr_t ptr = 0;
    if (argType_isObject(arg_getType(obj))) {
        ptr = (uintptr_t)arg_getPtr(obj);
    } else {
        ptr = (uintptr_t)obj;
    }
    return ptr & (0x7FFFFFFF);
}
