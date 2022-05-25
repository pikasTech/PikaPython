#include "BaseObj.h"
#include "PikaObj.h"

void PikaStdData_List_append(PikaObj* self, Arg* arg) {
    int top = obj_getInt(self, "top");
    char buff[11];
    char* topStr = fast_itoa(buff, top);
    PikaObj* pyload = obj_getObj(self, "pyload");
    obj_setArg(pyload, topStr, arg);
    /* top++ */
    obj_setInt(self, "top", top + 1);
}

int PikaStdData_List_len(PikaObj* self) {
    return obj_getInt(self, "top");
}

Arg* PikaStdData_List_get(PikaObj* self, int i) {
    char buff[11];
    char* index = fast_itoa(buff, i);
    PikaObj* pyload = obj_getObj(self, "pyload");
    return arg_copy(obj_getArg(pyload, index));
}
void PikaStdData_List___init__(PikaObj* self) {
    /* set top index for append */
    obj_setInt(self, "top", 0);
    obj_newObj(self, "pyload", "", New_TinyObj);
}

void PikaStdData_List_set(PikaObj* self, Arg* arg, int i) {
    char buff[11];
    char* i_str = fast_itoa(buff, i);
    int top = obj_getInt(self, "top");
    if (i > top) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error]: index exceeded lengh of list.");
    }
    PikaObj* pyload = obj_getObj(self, "pyload");
    obj_setArg(pyload, i_str, arg);
}

Arg* PikaStdData_List___iter__(PikaObj* self) {
    obj_setInt(self, "__iter_i", 0);
    return arg_setRef(NULL, "", self);
}

Arg* PikaStdData_List___next__(PikaObj* self) {
    int __iter_i = args_getInt(self->list, "__iter_i");
    Arg* res = PikaStdData_List_get(self, __iter_i);
    if (NULL == res) {
        return arg_setNull(NULL);
    }
    args_setInt(self->list, "__iter_i", __iter_i + 1);
    return res;
}

Arg* PikaStdData_List___get__(PikaObj* self) {
    return PikaStdData_List_get(self, obj_getInt(self, "__key"));
}
void PikaStdData_List___set__(PikaObj* self) {
    PikaStdData_List_set(self, obj_getArg(self, "__val"),
                         obj_getInt(self, "__key"));
}

void PikaStdData_ByteArray_fromString(PikaObj* self, char* s) {
    for (uint32_t i = 0; i < strGetSize(s); i++) {
        obj_setInt(self, "__val", (int)s[i]);
        PIKA_PYTHON_BEGIN
        /* clang-format off */
        PIKA_PYTHON(
            append(__val)
        )
        /* clang-format on */
        const uint8_t bytes[] = {
            0x08, 0x00, /* instruct array size */
            0x10, 0x81, 0x01, 0x00, 0x00, 0x02, 0x07, 0x00, /* instruct array */
            0x0e, 0x00, /* const pool size */
            0x00, 0x5f, 0x5f, 0x76, 0x61, 0x6c, 0x00, 0x61,
            0x70, 0x70, 0x65, 0x6e, 0x64, 0x00, /* const pool */
        };
        pikaVM_runByteCode(self, (uint8_t*)bytes);
        PIKA_PYTHON_END
    }
}
