#include "PikaObj.h"

void _UpdateHandle(PikaObj *self)
{
    // override the handle function here
}

void _beforDinit(PikaObj *self)
{
    /* override in user code */
}

PikaObj *New_TinyObj(Args *args)
{
    PikaObj *self = pikaMalloc(sizeof(PikaObj));
    self->memSize = sizeof(PikaObj);

    /* List */
    self->attributeList = New_args(NULL);

    /* attribute */
    obj_setPtr(self, "__context", self);
    obj_setStr(self, "__name", "root");

    /* load */
    if (NULL != args)
    {
        obj_load(self, args, "__name");
        obj_load(self, args, "__context");
    }

    /* hard attribute */
    self->name = obj_getStr(self, "__name");
    return self;
}