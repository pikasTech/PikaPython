#include "dataLinkNode.h"
#include "dataMemory.h"

void linkNode_deinit(LinkNode *self)
{
    self->_contentDinit(self->content);
    // DynMemPut(self->mem);
    pikaFree(self, self->memSize);
    self = NULL;
}

int64_t linkNode_getId(LinkNode *self)
{
    return self->id;
}

int32_t linkNode_isId(LinkNode *self, int64_t id)
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
    self->nextNode = NULL;
    self->priorNode = NULL;
    self->id = 0;

    /* object */
    self->content = NULL;

    /* override */
    self->_contentDinit = NULL;
}

LinkNode *New_linkNode(void *args)
{
    LinkNode *self = pikaMalloc(sizeof(LinkNode));
    self->memSize = sizeof(LinkNode);
    linkNode_init(self, args);
    return self;
}
