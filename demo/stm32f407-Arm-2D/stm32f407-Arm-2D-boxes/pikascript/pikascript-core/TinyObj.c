#include "MimiObj.h"

void _UpdateHandle(MimiObj *self)
{
    // override the handle function here
}

void _beforDinit(MimiObj *self)
{
    /* override in user code */
}

MimiObj *New_TinyObj(Args *args)
{
    /* request memory */
    DMEM *mem = DynMemGet(sizeof(MimiObj));
    if (NULL == mem)
    {
        //printf("[error] memory is empty!");
        while (1)
            ;
    }
    MimiObj *self = (void *)(mem->addr);
    self->mem = mem;

    /* List */
    self->attributeList = New_args(NULL);

    /* override */
    self->_updateHandle = _UpdateHandle;
    self->_beforDinit = _beforDinit;

    /* attribute */
    obj_setPtr(self, "_ctx", self);
    obj_setStr(self, "_n", "root");

    /* load */
    if (NULL != args)
    {
        obj_load(self, args, "_n");
        obj_load(self, args, "_ctx");
    }

    /* hard attribute */
    self->name = obj_getStr(self, "_n");
    return self;
}