#include "PikaStdLib_SysObj.h"
#include "PikaStdData_FILEIO.h"
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
        return arg_newNull();
    }
    ArgType type = arg_getType(arg);
    if (ARG_TYPE_INT == type) {
        return arg_newStr("<class 'int'>");
    }
    if (ARG_TYPE_FLOAT == type) {
        return arg_newStr("<class 'float'>");
    }
    if (ARG_TYPE_STRING == type) {
        return arg_newStr("<class 'str'>");
    }
    if (argType_isObject(type)) {
        PikaObj* obj = arg_getPtr(arg);
        NewFun clsptr = obj_getClass(obj);
        PikaObj* New_PikaStdData_List(Args * args);
        if (clsptr == New_PikaStdData_List) {
            return arg_newStr("<class 'list'>");
        }
        /* dict */
        PikaObj* New_PikaStdData_Dict(Args * args);
        if (clsptr == New_PikaStdData_Dict) {
            return arg_newStr("<class 'dict'>");
        }
        return arg_newStr("<class 'object'>");
    }
    if (ARG_TYPE_OBJECT_META == type) {
        return arg_newStr("<class 'meta object'>");
    }
    if (ARG_TYPE_BYTES == type) {
        return arg_newStr("<class 'bytes'>");
    }
    if (ARG_TYPE_METHOD_NATIVE == type) {
        return arg_newStr("<class 'buitin_function_or_method'>");
    }
    if (ARG_TYPE_METHOD_OBJECT == type) {
        return arg_newStr("<class 'method'>");
    }
    if (ARG_TYPE_METHOD_STATIC == type) {
        return arg_newStr("<class 'function'>");
    }
    if (ARG_TYPE_NONE == type) {
        return arg_newStr("<class 'NoneType'>");
    }
    return arg_newNull();
}

pika_float PikaStdLib_SysObj_float(PikaObj* self, Arg* arg) {
    ArgType type = arg_getType(arg);
    if (ARG_TYPE_INT == type) {
        return (float)arg_getInt(arg);
    }
    if (ARG_TYPE_FLOAT == type) {
        return (float)arg_getFloat(arg);
    }
    if (ARG_TYPE_STRING == type) {
        return strtod(arg_getStr(arg), NULL);
    }
    obj_setSysOut(self, "[error] convert to pika_float type failed.");
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
    obj_setSysOut(self, "[error] convert to int type failed.");
    obj_setErrorCode(self, 1);
    return -999999999;
}

char* PikaStdLib_SysObj_str(PikaObj* self, Arg* arg) {
    obj_removeArg(self, "__buf");
    ArgType type = arg_getType(arg);
    Args buffs = {0};
    char* res = "";
    if (ARG_TYPE_INT == type) {
        int val = arg_getInt(arg);
        res = strsFormat(&buffs, 11, "%d", val);
        goto exit;
    }
    if (ARG_TYPE_FLOAT == type) {
        pika_float val = arg_getFloat(arg);
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
    /* object */
    PIKA_BOOL is_temp = 0;
    PikaObj* arg_obj = _arg_to_obj(arg, &is_temp);
    NewFun _clsptr = (NewFun)arg_obj->constructor;
    if (_clsptr == New_PikaStdLib_RangeObj) {
        /* found RangeObj, return directly */
        return arg_copy(arg);
    }
    // pikaVM_runAsm(arg_obj,
    //               "B0\n"
    //               "0 RUN __iter__\n"
    //               "0 OUT __res\n");
    const uint8_t bytes[] = {
        0x08, 0x00, 0x00, 0x00, /* instruct array size */
        0x00, 0x82, 0x01, 0x00, 0x00, 0x04, 0x0a, 0x00, /* instruct array */
        0x10, 0x00, 0x00, 0x00,                         /* const pool size */
        0x00, 0x5f, 0x5f, 0x69, 0x74, 0x65, 0x72, 0x5f,
        0x5f, 0x00, 0x5f, 0x5f, 0x72, 0x65, 0x73, 0x00, /* const pool */
    };
    pikaVM_runByteCode(arg_obj, (uint8_t*)bytes);
    Arg* res = arg_copy(args_getArg(arg_obj->list, "__res"));
    obj_removeArg(arg_obj, "__res");
    if (is_temp) {
        obj_refcntDec(arg_obj);
    }
    return res;
}

Arg* PikaStdLib_SysObj_range(PikaObj* self, PikaTuple* ax) {
    /* set template arg to create rangeObj */
    Arg* obj_arg = arg_newDirectObj(New_PikaStdLib_RangeObj);
    PikaObj* range_obj = arg_getPtr(obj_arg);
    RangeData range_data = {0};
    if (pikaTuple_getSize(ax) == 1) {
        int start = 0;
        int end = arg_getInt(pikaTuple_getArg(ax, 0));
        range_data.start = start;
        range_data.end = end;
        range_data.step = 1;
    } else if (pikaTuple_getSize(ax) == 2) {
        int start = arg_getInt(pikaTuple_getArg(ax, 0));
        int end = arg_getInt(pikaTuple_getArg(ax, 1));
        range_data.start = start;
        range_data.end = end;
        range_data.step = 1;
    } else if (pikaTuple_getSize(ax) == 3) {
        int start = arg_getInt(pikaTuple_getArg(ax, 0));
        int end = arg_getInt(pikaTuple_getArg(ax, 1));
        int step = arg_getInt(pikaTuple_getArg(ax, 2));
        range_data.start = start;
        range_data.end = end;
        range_data.step = step;
    }
    range_data.i = range_data.start;
    obj_setStruct(range_obj, "_", range_data);
    return obj_arg;
}

Arg* PikaStdLib_SysObj___getitem__(PikaObj* self, Arg* obj, Arg* key) {
    return __vm_get(NULL, self, key, obj);
}

Arg* PikaStdLib_SysObj___setitem__(PikaObj* self,
                                   Arg* obj,
                                   Arg* key,
                                   Arg* val) {
    ArgType obj_type = arg_getType(obj);
    if (ARG_TYPE_STRING == obj_type) {
        int index = arg_getInt(key);
        char* str_val = arg_getStr(val);
        char* str_pyload = arg_getStr(obj);
        str_pyload[index] = str_val[0];
        return arg_newStr(str_pyload);
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
        return arg_newBytes(bytes_pyload, bytes_len);
    }
    if (argType_isObject(obj_type)) {
        PikaObj* arg_obj = arg_getPtr(obj);
        obj_setArg(arg_obj, "__key", key);
        obj_setArg(arg_obj, "__val", val);
        /* clang-format off */
        PIKA_PYTHON(
        __setitem__(__key, __val)
        )
        /* clang-format on */
        const uint8_t bytes[] = {
            0x0c, 0x00, 0x00, 0x00, /* instruct array size */
            0x10, 0x81, 0x01, 0x00, 0x10, 0x01, 0x07, 0x00, 0x00, 0x02, 0x0d,
            0x00,
            /* instruct array */
            0x19, 0x00, 0x00, 0x00, /* const pool size */
            0x00, 0x5f, 0x5f, 0x6b, 0x65, 0x79, 0x00, 0x5f, 0x5f, 0x76, 0x61,
            0x6c, 0x00, 0x5f, 0x5f, 0x73, 0x65, 0x74, 0x69, 0x74, 0x65, 0x6d,
            0x5f, 0x5f, 0x00,
            /* const pool */
        };
        pikaVM_runByteCode(arg_obj, (uint8_t*)bytes);
        return arg_newRef(arg_obj);
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
                0x08, 0x00, 0x00, 0x00, /* instruct array size */
                0x00, 0x82, 0x01, 0x00, 0x00, 0x04, 0x09, 0x00, /* instruct
                                                                   array */
                0x0f, 0x00, 0x00, 0x00, /* const pool size */
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

Arg* PikaStdLib_SysObj_list(PikaObj* self, PikaTuple* val) {
#if PIKA_BUILTIN_STRUCT_ENABLE
    if (1 == pikaTuple_getSize(val)) {
        Arg* in = pikaTuple_getArg(val, 0);
        obj_setArg(self, "__list", in);
        /* clang-format off */
        PIKA_PYTHON(
        __res = []
        for __item in __list:
            __res.append(__item)
        del __item
        del __list
        )
        /* clang-format on */
        const uint8_t bytes[] = {
            0x3c, 0x00, 0x00, 0x00, /* instruct array size */
            0x00, 0x95, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x10, 0x81, 0x07,
            0x00, 0x00, 0x02, 0x0e, 0x00, 0x00, 0x04, 0x13, 0x00, 0x00, 0x82,
            0x17, 0x00, 0x00, 0x04, 0x24, 0x00, 0x00, 0x0d, 0x24, 0x00, 0x00,
            0x07, 0x2b, 0x00, 0x11, 0x81, 0x24, 0x00, 0x01, 0x02, 0x2d, 0x00,
            0x00, 0x86, 0x3a, 0x00, 0x00, 0x8c, 0x13, 0x00, 0x00, 0x8c, 0x24,
            0x00, 0x00, 0x8c, 0x07, 0x00,
            /* instruct array */
            0x3d, 0x00, 0x00, 0x00, /* const pool size */
            0x00, 0x5f, 0x5f, 0x72, 0x65, 0x73, 0x00, 0x5f, 0x5f, 0x6c, 0x69,
            0x73, 0x74, 0x00, 0x69, 0x74, 0x65, 0x72, 0x00, 0x24, 0x6c, 0x30,
            0x00, 0x24, 0x6c, 0x30, 0x2e, 0x5f, 0x5f, 0x6e, 0x65, 0x78, 0x74,
            0x5f, 0x5f, 0x00, 0x5f, 0x5f, 0x69, 0x74, 0x65, 0x6d, 0x00, 0x32,
            0x00, 0x5f, 0x5f, 0x72, 0x65, 0x73, 0x2e, 0x61, 0x70, 0x70, 0x65,
            0x6e, 0x64, 0x00, 0x2d, 0x31, 0x00,
            /* const pool */
        };
        pikaVM_runByteCode(self, (uint8_t*)bytes);
        return arg_copy(obj_getArg(self, "__res"));
    }
    PikaObj* New_PikaStdData_List(Args * args);
    return arg_newDirectObj(New_PikaStdData_List);
#else
    obj_setErrorCode(self, 1);
    __platform_printf("[Error] built-in list is not enabled.\r\n");
#endif
    return arg_newNull();
}

Arg* PikaStdLib_SysObj_dict(PikaObj* self, PikaTuple* val) {
#if PIKA_BUILTIN_STRUCT_ENABLE
    PikaObj* New_PikaStdData_Dict(Args * args);
    return arg_newDirectObj(New_PikaStdData_Dict);
#else
    obj_setErrorCode(self, 1);
    __platform_printf("[Error] built-in dist is not enabled.\r\n");
    return arg_newNull();
#endif
}

char* PikaStdLib_SysObj_hex(PikaObj* self, int val) {
    char buff[PIKA_SPRINTF_BUFF_SIZE] = {0};
    if (val >= 0) {
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
        Arg* bytes = arg_newBytes(NULL, size);
        return bytes;
    }
    if (ARG_TYPE_BYTES == type) {
        return arg_copy(val);
    }
    if (ARG_TYPE_STRING == type) {
        int size = strGetSize(arg_getStr(val));
        Arg* bytes = arg_newBytes((uint8_t*)arg_getStr(val), size);
        return bytes;
    }
#if !PIKA_NANO_ENABLE
    if (argType_isObject(type)) {
        PikaObj* obj = arg_getPtr(val);
        PikaObj* New_PikaStdData_List(Args * args);
        PikaObj* New_PikaStdData_Tuple(Args * args);
        if (obj->constructor == New_PikaStdData_List ||
            obj->constructor == New_PikaStdData_Tuple) {
            PikaList* list = obj_getPtr(obj, "list");
            Arg* bytes = arg_newBytes(NULL, pikaList_getSize(list));
            uint8_t* bytes_raw = arg_getBytes(bytes);
            for (size_t i = 0; i < pikaList_getSize(list); i++) {
                bytes_raw[i] = (uint8_t)pikaList_getInt(list, i);
            }
            return bytes;
        }
    }
#endif
    obj_setErrorCode(self, 1);
    __platform_printf("Error: input arg type not supported.\r\n");
    return arg_newNull();
}

static char* __print_arg(PikaObj* self, Arg* val) {
    Args buffs = {0};
    char* res = NULL;
    if (NULL == val) {
        goto __exit;
    }
    ArgType arg_type = arg_getType(val);
    if (arg_type == ARG_TYPE_BYTES) {
        res = __printBytes(self, val);
        goto __exit;
    }
    if (arg_type == ARG_TYPE_STRING) {
        res = arg_getStr(val);
        goto __exit;
    }
    if (arg_type == ARG_TYPE_NONE) {
        res = "None";
        goto __exit;
    }
    if (arg_type == ARG_TYPE_INT) {
        int64_t value = arg_getInt(val);
        res = strsFormat(&buffs, 32, "%lld", value);
        goto __exit;
    }
    if (arg_type == ARG_TYPE_FLOAT) {
        pika_float value = arg_getFloat(val);
        res = strsFormat(&buffs, 32, "%f", value);
        goto __exit;
    }
    if (arg_type == ARG_TYPE_POINTER ||
        arg_type == ARG_TYPE_METHOD_NATIVE_CONSTRUCTOR) {
        void* value = arg_getPtr(val);
        res = strsFormat(&buffs, 32, "%p", value);
        goto __exit;
    }
    if (argType_isObject(arg_type)) {
        res = obj_toStr(arg_getPtr(val));
        goto __exit;
    }
__exit:
    if (NULL == res) {
        obj_setSysOut(self, "Error: can not print val");
        obj_setErrorCode(self, 1);
    }
    if (NULL != res) {
        res = obj_cacheStr(self, res);
    }
    strsDeinit(&buffs);
    return res;
}

void PikaStdLib_SysObj_print(PikaObj* self, PikaTuple* val, PikaDict* ops) {
    int arg_size = pikaTuple_getSize(val);
    char* end = pikaDict_getStr(ops, "end");
    if (NULL == end) {
        /* default */
        end = "\r\n";
    }
    if (arg_size == 1) {
        arg_singlePrint(pikaTuple_getArg(val, 0), PIKA_FALSE, end);
        return;
    }
    Arg* print_out_arg = NULL;
    PIKA_BOOL is_get_print = PIKA_FALSE;
    for (int i = 0; i < arg_size; i++) {
        Arg* arg = pikaTuple_getArg(val, i);
        char* item = __print_arg(self, arg);
        if (NULL != item) {
            is_get_print = PIKA_TRUE;
            if (NULL == print_out_arg) {
                print_out_arg = arg_newStr("");
            }
            print_out_arg = arg_strAppend(print_out_arg, item);
            if (i < arg_size - 1) {
                print_out_arg = arg_strAppend(print_out_arg, " ");
            }
        }
    }
    if (PIKA_TRUE == is_get_print) {
        __platform_printf("%s%s", arg_getStr(print_out_arg), end);
    }
    if (NULL != print_out_arg) {
        arg_deinit(print_out_arg);
    }
}

char* PikaStdLib_SysObj_cformat(PikaObj* self, char* fmt, PikaTuple* var) {
#if PIKA_SYNTAX_FORMAT_ENABLE
    Args buffs = {0};
    pikaMemMaxReset();
    char* res = strsFormatList(&buffs, fmt, &var->super);
    obj_setStr(self, "_buf", res);
    res = obj_getStr(self, "_buf");
    strsDeinit(&buffs);
    return res;
#else
    obj_setErrorCode(self, 1);
    __platform_printf("[Error] PIKA_SYNTAX_FORMAT_ENABLE is not enabled.\r\n");
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

PikaObj* PikaStdLib_SysObj_open(PikaObj* self, char* path, char* mode) {
#if PIKA_FILEIO_ENABLE
    PikaObj* file = newNormalObj(New_PikaStdData_FILEIO);
    if (0 != PikaStdData_FILEIO_init(file, path, mode)) {
        obj_setErrorCode(self, 1);
        __platform_printf("[Error] open: can not open file.\r\n");
        obj_deinit(file);
        return NULL;
    }
    return file;
#else
    obj_setErrorCode(self, 1);
    __platform_printf("[Error] PIKA_FILEIO_ENABLE is not enabled.\r\n");
    return NULL;
#endif
}

/* __dir_each */
int32_t __dir_each(Arg* argEach, Args* context) {
    PikaObj* list = args_getPtr(context, "list");
    if (argType_isCallable(arg_getType(argEach))) {
        char name_buff[PIKA_LINE_BUFF_SIZE / 2] = {0};
        char* method_name =
            methodArg_getName(argEach, name_buff, sizeof(name_buff));
        Arg* arg_str = arg_newStr(method_name);
        __vm_List_append(list, arg_str);
        arg_deinit(arg_str);
    }
    return 0;
}

PikaObj* PikaStdLib_SysObj_dir(PikaObj* self, PikaObj* obj) {
    PikaObj* New_PikaStdData_List(Args * args);
    PikaObj* list = newNormalObj(New_PikaStdData_List);
    __vm_List___init__(list);
    Args* context = New_args(NULL);
    args_setPtr(context, "list", list);
    args_foreach(obj->list, __dir_each, context);
    args_deinit(context);
    return list;
}

void PikaStdLib_SysObj_exec(PikaObj* self, char* code) {
#if PIKA_EXEC_ENABLE
    obj_run(self, code);
#else
    obj_setErrorCode(self, 1);
    __platform_printf("[Error] PIKA_EXEC_ENABLE is not enabled.\r\n");
#endif
}

Arg* PikaStdLib_SysObj_getattr(PikaObj* self, PikaObj* obj, char* name) {
    Arg* res = NULL;
    if (NULL == obj) {
        obj_setErrorCode(self, 1);
        __platform_printf("[Error] getattr: can not get attr of NULL.\r\n");
        return NULL;
    }
    Arg* arg = obj_getArg(obj, name);
    if (NULL == arg) {
        arg = obj_getMethodArg(obj, name);
        return arg_copy(arg);
    }
    if (NULL != arg) {
        res = arg_copy(arg);
        return res;
    }
    return NULL;
}

void PikaStdLib_SysObj_setattr(PikaObj* self,
                               PikaObj* obj,
                               char* name,
                               Arg* val) {
    if (NULL == obj) {
        obj_setErrorCode(self, 1);
        __platform_printf("[Error] setattr: obj is null.\r\n");
        goto exit;
    }
    obj_setArg(obj, name, val);
exit:
    return;
}

void PikaStdLib_SysObj_exit(PikaObj* self) {
    pks_vm_exit();
}

int PikaStdLib_SysObj_hasattr(PikaObj* self, PikaObj* obj, char* name) {
    if (NULL == obj) {
        obj_setErrorCode(self, 1);
        __platform_printf("[Error] hasattr: obj is null.\r\n");
        return 0;
    }
    if (obj_isArgExist(obj, name)) {
        return 1;
    }
    Arg* method = obj_getMethodArg(obj, name);
    if (NULL != method) {
        arg_deinit(method);
        return 1;
    }
    return 0;
}

Arg* PikaStdLib_SysObj_eval(PikaObj* self, char* code) {
    Args buffs = {0};
    char* cmd = strsAppend(&buffs, "@res = ", code);
    obj_run(self, cmd);
    Arg* res = arg_copy(obj_getArg(self, "@res"));
    strsDeinit(&buffs);
    obj_removeArg(self, "@res");
    return res;
}

static enum shellCTRL __obj_shellLineHandler_input(PikaObj* self,
                                                   char* input_line,
                                                   struct ShellConfig* cfg) {
    cfg->context = arg_newStr(input_line);
    return SHELL_CTRL_EXIT;
}

char* PikaStdLib_SysObj_input(PikaObj* self, PikaTuple* info) {
    struct ShellConfig cfg = {
        .prefix = "",
        .context = NULL,
        .handler = __obj_shellLineHandler_input,
        .fn_getchar = __platform_getchar,
    };
    if (pikaTuple_getSize(info) > 0) {
        __platform_printf("%s", pikaTuple_getStr(info, 0));
    }
    _temp__do_pikaScriptShell(self, &cfg);
    char* res = obj_cacheStr(self, arg_getStr(cfg.context));
    arg_deinit(cfg.context);
    return res;
}

extern volatile PikaObj* __pikaMain;
void PikaStdLib_SysObj_help(PikaObj* self, char* name) {
    if (strEqu(name, "modules")) {
        obj_printModules((PikaObj*)__pikaMain);
    }
}
