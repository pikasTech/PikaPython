#include "weakref.h"

Arg* weakref_ref(PikaObj* self, Arg* obj) {
    if (argType_isObject(arg_getType(obj))) {
        Arg* ret = arg_copy(obj);
        obj_refcntDec((PikaObj*)arg_getPtr(ret));
        arg_setIsWeakRef(ret, PIKA_TRUE);
        return ret;
    }
    return NULL;
}
