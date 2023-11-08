#include "PikaStdData_Tuple.h"
#include "PikaVM.h"
#include "dataStrs.h"

int PikaStdData_Tuple_len(PikaObj* self) {
    return pikaList_getSize(self);
}

Arg* PikaStdData_Tuple_get(PikaObj* self, int i) {
    return arg_copy(pikaList_get(self, i));
}

void PikaStdData_Tuple___init__(PikaObj* self) {
    pikaList_init(self);
}

Arg* PikaStdData_Tuple___iter__(PikaObj* self) {
    obj_setInt(self, "__iter_i", 0);
    return arg_newRef(self);
}

Arg* PikaStdData_Tuple___next__(PikaObj* self) {
    int __iter_i = args_getInt(self->list, "__iter_i");
    Arg* res = PikaStdData_Tuple_get(self, __iter_i);
    if (NULL == res) {
        return arg_newNone();
    }
    args_setInt(self->list, "__iter_i", __iter_i + 1);
    return res;
}

Arg* PikaStdData_Tuple___getitem__(PikaObj* self, Arg* __key) {
    int i = arg_getInt(__key);
    if (i < 0 || i >= pikaList_getSize(self)) {
        obj_setErrorCode(self, PIKA_RES_ERR_INDEX);
        obj_setSysOut(self, "IndexError: index out of range");
        return NULL;
    }
    return PikaStdData_Tuple_get(self, i);
}

void PikaStdData_Tuple___del__(PikaObj* self) {
    if (0 == obj_getInt(self, "needfree")) {
        return;
    }
    Args* list = _OBJ2LIST(self);
    args_deinit(list);
}

char* builtins_str(PikaObj* self, Arg* arg);
typedef struct {
    Arg* buf;
    int count;
} TupleToStrContext;

int32_t tupleToStrEachHandle(PikaObj* self,
                             int itemIndex,
                             Arg* itemEach,
                             void* context) {
    TupleToStrContext* ctx = (TupleToStrContext*)context;

    char* item_str = builtins_str(self, itemEach);
    if (ctx->count != 0) {
        ctx->buf = arg_strAppend(ctx->buf, ", ");
    }
    if (arg_getType(itemEach) == ARG_TYPE_STRING) {
        ctx->buf = arg_strAppend(ctx->buf, "'");
    }
    ctx->buf = arg_strAppend(ctx->buf, item_str);
    if (arg_getType(itemEach) == ARG_TYPE_STRING) {
        ctx->buf = arg_strAppend(ctx->buf, "'");
    }

    ctx->count++;

    return 0;
}

char* PikaStdData_Tuple___str__(PikaObj* self) {
    TupleToStrContext context;
    context.buf = arg_newStr("(");
    context.count = 0;

    pikaTuple_forEach(self, tupleToStrEachHandle, &context);

    if (context.count == 1) {
        context.buf = arg_strAppend(context.buf, ",");
    }

    context.buf = arg_strAppend(context.buf, ")");
    obj_setStr(self, "_buf", arg_getStr(context.buf));
    arg_deinit(context.buf);
    return obj_getStr(self, "_buf");
}

int PikaStdData_Tuple___len__(PikaObj* self) {
    return PikaStdData_Tuple_len(self);
}

PikaObj* PikaStdData_Tuple___add__(PikaObj* self, PikaObj* others) {
    PikaObj* res = New_PikaTuple();
    for (size_t i = 0; i < pikaList_getSize(self); i++) {
        Arg* arg = pikaList_get(self, i);
        pikaList_append(res, arg_copy(arg));
    }
    for (size_t i = 0; i < pikaList_getSize(others); i++) {
        Arg* arg = pikaList_get(others, i);
        pikaList_append(res, arg_copy(arg));
    }
    return res;
}

int PikaStdData_Tuple___eq__(PikaObj* self, Arg* other) {
    if (!arg_isObject(other)) {
        return 0;
    }
    PikaObj* oOther = arg_getObj(other);
    if (self->constructor != oOther->constructor) {
        return 0;
    }
    if (obj_getSize(self) != obj_getSize(oOther)) {
        return 0;
    }
    for (size_t i = 0; i < pikaList_getSize(self); i++) {
        Arg* arg = pikaList_get(self, i);
        Arg* otherArg = pikaList_get(oOther, i);
        if (!arg_isEqual(arg, otherArg)) {
            return 0;
        }
    }
    return 1;
}

PikaObj* PikaStdData_Tuple___mul__(PikaObj* self, int n) {
    PikaObj* res = New_PikaTuple();
    for (int i = 0; i < n; i++) {
        for (size_t j = 0; j < pikaList_getSize(self); j++) {
            Arg* arg = pikaList_get(self, j);
            pikaList_append(res, arg_copy(arg));
        }
    }
    return res;
}
