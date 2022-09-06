#include "Arm2D_common.h"
#include "PikaVM.h"

int32_t __foreach_ElementList_update(Arg* elem, Args* buffs) {
    ArgType type = arg_getType(elem);
    if (ARG_TYPE_OBJECT == type) {
        PikaObj* elemObj = arg_getPtr(elem);
        obj_runNativeMethod(elemObj, "update", NULL);
    }
    return 0;
}

void Arm2D_ElementList_update(PikaObj* self) {
    args_foreach(self->list, __foreach_ElementList_update, NULL);
}
