#include "PikaObj.h"

Arg* PikaStdLib_StringObj___iter__(PikaObj* self) {
    return arg_setPtr(NULL, "", TYPE_OBJECT, self);
}

Arg* PikaStdLib_StringObj___next__(PikaObj* self) {
    /* start */
    int iter_i = args_getInt(self->list, "iter_i");
    /* iter_i is not inited */
    if (-999999999 == iter_i) {
        iter_i = 0;
    }
    return arg_setNull(NULL);
}