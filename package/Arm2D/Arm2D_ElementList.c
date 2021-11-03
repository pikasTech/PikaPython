#include "Arm2D_common.h"
#include "PikaVM.h"

int32_t __foreach_ElementList_update(Arg* elem, Args* buffs) {
    char* type = arg_getType(elem);
    if (strIsStartWith(type, "_class")) {
        PikaObj* elemObj = arg_getPtr(elem);
        args_deinit(pikaVM_runAsm(elemObj, "0 RUN update\n"));
    }
    return 0;
}

void Arm2D_ElementList_update(PikaObj* self) {
    args_foreach(self->attributeList, __foreach_ElementList_update, NULL);
}
