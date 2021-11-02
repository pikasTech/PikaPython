#include "Arm2D_common.h"

int32_t __foreach_ElementList_update(Arg *elem, Args *buffs)
{
    char *type = arg_getType(elem);
    if (strIsStartWith(type, "_class"))
    {
        PikaObj *elemObj = arg_getPtr(elem);
        obj_run(elemObj, "update()");
    }
    return 0;
}

void Arm2D_ElementList_update(PikaObj *self)
{
    args_foreach(self->attributeList, __foreach_ElementList_update, NULL);
}
