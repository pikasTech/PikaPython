#include "PikaObj.h"

Arg* PikaStdLib_RangeObj___next__(PikaObj* self) {
    int a1 = args_getInt(self->list, "a1");
    int a2 = args_getInt(self->list, "a2");
    int a3 = args_getInt(self->list, "a3");
    int start = 0;
    int end = 0;
    int foot = 1;
    /* only start and end */
    if (-999999999 == a3) {
        start = a1;
        end = a2;
    }
    /* start */
    int iter_i = args_getInt(self->list, "iter_i");
    /* iter_i is not inited */
    if (-999999999 == iter_i) {
        iter_i = start;
    }
    /* exit */
    if (iter_i >= end) {
        return arg_setNull(NULL);
    }
    args_setInt(self->list, "iter_i", iter_i + foot);
    return arg_setInt(NULL, "", iter_i);
}
