#include "PikaStdData_Tuple.h"
#include "PikaVM.h"
#include "dataStrs.h"

int PikaStdData_Tuple_len(PikaObj* self) {
    return objList_getSize(self);
}

Arg* PikaStdData_Tuple_get(PikaObj* self, int i) {
    return arg_copy(objList_get(self, i));
}

void PikaStdData_Tuple___init__(PikaObj* self) {
    objList_init(self);
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
    PikaList* list = obj_getPtr(self, "list");
    if (i < 0 || i >= pikaList_getSize(list)) {
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
    Args* list = obj_getPtr(self, "list");
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

    objTuple_forEach(self, tupleToStrEachHandle, &context);

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

int PikaStdData_Tuple___contains__(PikaObj* self, Arg* val) {
    PikaList* list = obj_getPtr(self, "list");
    for (size_t i = 0; i < pikaList_getSize(list); i++) {
        Arg* arg = pikaList_getArg(list, i);
        if (arg_isEqual(arg, val)) {
            return 1;
        }
    }
    return 0;
}
