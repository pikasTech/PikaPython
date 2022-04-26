#include "BaseObj.h"
#include "PikaStdLib_RangeObj.h"
#include "PikaStdLib_StringObj.h"
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

void PikaStdLib_SysObj_type(PikaObj* self, Arg* arg) {
    if (NULL == arg) {
        obj_setSysOut(self, "[error] type: arg no found.");
        obj_setErrorCode(self, 1);
        return;
    }
    ArgType type = arg_getType(arg);
    if (ARG_TYPE_INT == type) {
        obj_setSysOut(self, "int");
        return;
    }
    if (ARG_TYPE_FLOAT == type) {
        obj_setSysOut(self, "float");
        return;
    }
    if (ARG_TYPE_STRING == type) {
        obj_setSysOut(self, "string");
        return;
    }
    if (ARG_TYPE_OBJECT == type) {
        obj_setSysOut(self, "pointer");
        return;
    }
    if (ARG_TYPE_MATE_OBJECT == type) {
        obj_setSysOut(self, "mate_object");
        return;
    }
    if (ARG_TYPE_NATIVE_METHOD == type) {
        obj_setSysOut(self, "method");
        return;
    }
}

float PikaStdLib_SysObj_float(PikaObj* self, Arg* arg) {
    ArgType type = arg_getType(arg);
    if (ARG_TYPE_INT == type) {
        return (float)arg_getInt(arg);
    }
    if (ARG_TYPE_FLOAT == type) {
        return (float)arg_getFloat(arg);
    }
    obj_setSysOut(self, "[error] convert to float type faild.");
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
    obj_setSysOut(self, "[error] convert to int type faild.");
    obj_setErrorCode(self, 1);
    return -999999999;
}

char* PikaStdLib_SysObj_str(PikaObj* self, Arg* arg) {
    ArgType type = arg_getType(arg);
    Args buffs = {0};
    char* res = NULL;
    do {
        if (ARG_TYPE_INT == type) {
            int val = arg_getInt(arg);
            res = strsFormat(&buffs, 11, "%d", val);
            break;
        }
        if (ARG_TYPE_FLOAT == type) {
            float val = arg_getFloat(arg);
            res = strsFormat(&buffs, 11, "%f", val);
            break;
        }
    } while (0);
    obj_setStr(self, "__strtmp", res);
    strsDeinit(&buffs);
    return obj_getStr(self, "__strtmp");
}

Arg* PikaStdLib_SysObj_iter(PikaObj* self, Arg* arg) {
    /* a String, return a StringObj */
    if (ARG_TYPE_STRING == arg_getType(arg)) {
        obj_setStr(self, "_sobj", arg_getStr(arg));
        return arg_setMetaObj("", "PikaStdLib_StringObj",
                              New_PikaStdLib_StringObj);
    }
    /* a MATE object, return itself */
    if (ARG_TYPE_MATE_OBJECT == arg_getType(arg)) {
        return arg_copy(arg);
    }
    /* object */
    if (ARG_TYPE_OBJECT == arg_getType(arg)) {
        PikaObj* arg_obj = arg_getPtr(arg);
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
    obj_setInt(self, "_r1", a1);
    obj_setInt(self, "_r2", a2);
    return arg_setMetaObj("", "PikaStdLib_RangeObj", New_PikaStdLib_RangeObj);
}

Arg* PikaStdLib_SysObj___get__(PikaObj* self, Arg* key, Arg* obj) {
    ArgType obj_type = arg_getType(obj);
    if (ARG_TYPE_STRING == obj_type) {
        int index = arg_getInt(key);
        char* str_pyload = arg_getStr(obj);
        char char_buff[] = " ";
        char_buff[0] = str_pyload[index];
        return arg_setStr(NULL, "", char_buff);
    }
    if ((ARG_TYPE_OBJECT == obj_type) || (ARG_TYPE_OBJECT == obj_type)) {
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

void PikaStdLib_SysObj___set__(PikaObj* self,
                               Arg* key,
                               Arg* obj,
                               char* obj_str,
                               Arg* val) {
    ArgType obj_type = arg_getType(obj);
    if (ARG_TYPE_STRING == obj_type) {
        int index = arg_getInt(key);
        char* str_val = arg_getStr(val);
        char* str_pyload = arg_getStr(obj);
        str_pyload[index] = str_val[0];
        obj_setStr(self, obj_str, str_pyload);
    }
    if ((ARG_TYPE_OBJECT == obj_type) || (ARG_TYPE_OBJECT == obj_type)) {
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
    }
}

int PikaStdLib_SysObj_len(PikaObj* self, Arg* arg) {
    if (ARG_TYPE_STRING == arg_getType(arg)) {
        return strGetSize(arg_getStr(arg));
    }
    obj_setErrorCode(self, 1);
    __platform_printf("[Error] len: arg type not support\r\n");
    return -1;
}

Arg* PikaStdLib_SysObj_list(PikaObj* self) {
#if PIKA_BUILTIN_LIST_ENABLE
    PikaObj* New_PikaStdData_List(Args * args);
    return obj_newObjInPackage(New_PikaStdData_List);
#endif
    obj_setErrorCode(self, 1);
    __platform_printf("[Error] built-in list is not enabled.\r\n");
    return arg_setNull(NULL);
}

Arg* PikaStdLib_SysObj_dict(PikaObj* self) {
#if PIKA_BUILTIN_DICT_ENABLE
    PikaObj* New_PikaStdData_Dict(Args * args);
    return obj_newObjInPackage(New_PikaStdData_Dict);
#endif
    obj_setErrorCode(self, 1);
    __platform_printf("[Error] built-in dist is not enabled.\r\n");
    return arg_setNull(NULL);
}
