#include "weakref.h"

Arg* weakref_ref(PikaObj* self, Arg* aObj) {
    if (arg_isObject(aObj)) {
        PikaObj* obj = arg_getPtr(aObj);
        Arg* ret = arg_copy(aObj);
        obj_refcntDec(obj);
        arg_setIsWeakRef(ret, PIKA_TRUE);
        return ret;
    }
    return NULL;
}
