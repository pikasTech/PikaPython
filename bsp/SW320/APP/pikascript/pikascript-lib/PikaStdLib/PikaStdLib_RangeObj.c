#include "PikaObj.h"

Arg* PikaStdLib_RangeObj___next__(PikaObj* self) {
    RangeData* _ = (RangeData*)args_getStruct(self->list, "_");
    int end = _->end;
    int step = _->step;
    /* exit */
    if (_->i >= end) {
        return arg_newNull();
    }
    Arg* res = arg_newInt(_->i);
    _->i += step;
    return res;
}
