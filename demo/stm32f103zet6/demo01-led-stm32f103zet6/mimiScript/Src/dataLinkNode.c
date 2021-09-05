#include "dataLinkNode.h"
#include "dataMemory.h"

void linkNode_deinit(LinkNode *self)
{
    DynMemPut(self->mem);
    self->_contantDinit(self->contant);
}

long long linkNode_getId(LinkNode *self)
{
    return self->id;
}

int linkNode_isId(LinkNode *self, long long id)
{
    if (id == linkNode_getId(self))
    {
        return 1;
    }
    return 0;
}

void linkNode_init(LinkNode *self, void *args)
{
    /* attribute */
    self->priorNode = NULL;
    self->nextNode = NULL;
    self->id = 0;


    /* object */
    self->contant = NULL;

    /* override */
    self->_contantDinit = NULL;
}

LinkNode *New_linkNode(void *args)
{
    DMEM *mem = DynMemGet(sizeof(LinkNode));
    LinkNode *self = (void *)(mem->addr);
    self->mem = mem;
    linkNode_init(self, args);
    return self;
}
