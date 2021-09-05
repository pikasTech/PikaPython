#include "dataLinkNode.h"
#include "dataMemory.h"
#include "dataArg.h"

void linkNode_deinit(LinkNode *self)
{
    arg_deinit(self);
}

void linkNode_init(LinkNode *self, void *args)
{
    /* attribute */
}

LinkNode *New_linkNode(void *args)
{
    return NULL;
}
