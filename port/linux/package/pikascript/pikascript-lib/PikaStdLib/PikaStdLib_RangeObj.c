#include "PikaObj.h"

Arg* PikaStdLib_RangeObj___next__(PikaObj* self) {
    int start = args_getInt(self->list, "_start");
    int end = args_getInt(self->list, "_end");
    int step = args_getInt(self->list, "_step");
    /* only start and end */
    /* start */
    int iter_i = args_getInt(self->list, "iter_i");
    /* iter_i is not inited */
    if (-999999999 == iter_i) {
        iter_i = start;
    }
    /* exit */
    if (iter_i >= end) {
        return arg_newNull();
    }
    args_setInt(self->list, "iter_i", iter_i + step);
    return arg_newInt(iter_i);
}
