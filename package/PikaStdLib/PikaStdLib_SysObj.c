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
    if (TYPE_INT == type) {
        obj_setSysOut(self, "int");
        return;
    }
    if (TYPE_FLOAT == type) {
        obj_setSysOut(self, "float");
        return;
    }
    if (TYPE_STRING == type) {
        obj_setSysOut(self, "string");
        return;
    }
    if (TYPE_POINTER == type) {
        obj_setSysOut(self, "pointer");
        return;
    }
    if (TYPE_MATE_OBJECT == type) {
        obj_setSysOut(self, "mate_object");
        return;
    }
    if (TYPE_METHOD == type) {
        obj_setSysOut(self, "method");
        return;
    }
}

float PikaStdLib_SysObj_float(PikaObj* self, Arg* arg) {
    ArgType type = arg_getType(arg);
    if (TYPE_INT == type) {
        return (float)arg_getInt(arg);
    }
    if (TYPE_FLOAT == type) {
        return (float)arg_getFloat(arg);
    }
    obj_setSysOut(self, "[error] convert to float type faild.");
    obj_setErrorCode(self, 1);
    return -99999.99999;
}

int PikaStdLib_SysObj_int(PikaObj* self, Arg* arg) {
    ArgType type = arg_getType(arg);
    if (TYPE_INT == type) {
        return (int)arg_getInt(arg);
    }
    if (TYPE_FLOAT == type) {
        return (int)arg_getFloat(arg);
    }
    obj_setSysOut(self, "[error] convert to int type faild.");
    obj_setErrorCode(self, 1);
    return -999999999;
}

char* PikaStdLib_SysObj_str(PikaObj* self, Arg* arg) {
    ArgType type = arg_getType(arg);
    Args* buffs = New_strBuff();
    char* res = NULL;
    do {
        if (TYPE_INT == type) {
            int val = arg_getInt(arg);
            res = strsFormat(buffs, 11, "%d", val);
            break;
        }
        if (TYPE_FLOAT == type) {
            float val = arg_getFloat(arg);
            res = strsFormat(buffs, 11, "%f", val);
            break;
        }
    } while (0);
    obj_setStr(self, "__strtmp", res);
    args_deinit(buffs);
    return obj_getStr(self, "__strtmp");
}

Arg* PikaStdLib_SysObj_iter(PikaObj* self, Arg* arg) {
    /* a String, return a StringObj */
    if (TYPE_STRING == arg_getType(arg)) {
        obj_setStr(self, "_sobj", arg_getStr(arg));
        return arg_setMetaObj("", "PikaStdLib_StringObj",
                              New_PikaStdLib_StringObj);
    }
    /* a MATE object, return itself */
    if (TYPE_MATE_OBJECT == arg_getType(arg)) {
        return arg_copy(arg);
    }
    /* object */
    if (TYPE_POINTER == arg_getType(arg)) {
        PikaObj* arg_obj = arg_getPtr(arg);
        pikaVM_runAsm(arg_obj,
                      "B0\n"
                      "0 RUN __iter__\n"
                      "0 OUT __res\n");
        return arg_copy(args_getArg(arg_obj->list, "__res"));
    }
    return arg_setNull(NULL);
}

Arg* PikaStdLib_SysObj_next(PikaObj* self, Arg* arg) {
    PikaObj* arg_obj = arg_getPtr(arg);
    pikaVM_runAsm(arg_obj,
                  "B0\n"
                  "0 RUN __next__\n"
                  "0 OUT __res\n");
    return arg_copy(args_getArg(arg_obj->list, "__res"));
}

Arg* PikaStdLib_SysObj_range(PikaObj* self, int a1, int a2, int a3) {
    /* set template arg to create rangeObj */
    obj_setInt(self, "_r1", a1);
    obj_setInt(self, "_r2", a2);
    obj_setInt(self, "_r3", a3);
    return arg_setMetaObj("", "PikaStdLib_RangeObj", New_PikaStdLib_RangeObj);
}

Arg* PikaStdLib_SysObj___get__(PikaObj* self, Arg* key, Arg* obj) {
    ArgType obj_type = arg_getType(obj);
    if (TYPE_STRING == obj_type) {
        int index = arg_getInt(key);
        char* str_pyload = arg_getStr(obj);
        char char_buff[] = " ";
        char_buff[0] = str_pyload[index];
        return arg_setStr(NULL, "", char_buff);
    }
    if ((TYPE_OBJECT == obj_type) || (TYPE_POINTER == obj_type)) {
        PikaObj* arg_obj = arg_getPtr(obj);
        obj_setArg(arg_obj, "__key", key);
        pikaVM_runAsm(arg_obj,
                      "B0\n"
                      "1 REF __key\n"
                      "0 RUN __get__\n"
                      "0 OUT __res\n");
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
    if (TYPE_STRING == obj_type) {
        int index = arg_getInt(key);
        char* str_val = arg_getStr(val);
        char* str_pyload = arg_getStr(obj);
        str_pyload[index] = str_val[0];
        obj_setStr(self, obj_str, str_pyload);
    }
    if ((TYPE_OBJECT == obj_type) || (TYPE_POINTER == obj_type)) {
        PikaObj* arg_obj = arg_getPtr(obj);
        obj_setArg(arg_obj, "__key", key);
        obj_setArg(arg_obj, "__val", val);
        pikaVM_runAsm(arg_obj,
                      "B0\n"
                      "1 REF __key\n"
                      "1 REF __val\n"
                      "0 RUN __set__\n");
    }
}
