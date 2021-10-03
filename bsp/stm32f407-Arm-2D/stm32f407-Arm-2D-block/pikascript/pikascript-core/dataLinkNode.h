#ifndef _linkNode__H
#define _linkNode__H
#include "dataMemory.h"
typedef struct Class_linkNode LinkNode;
struct Class_linkNode
{
    /* attribute */
    DMEM *mem;
    LinkNode *priorNode;
    LinkNode *nextNode;
    int64_t id;

    /* virtual operation */
    void (*_contantDinit)(void *contant);

    /* object */
    void *contant;
};

void linkNode_deinit(LinkNode *self);
void linkNode_init(LinkNode *self, void *args);
int64_t linkNode_getId(LinkNode *self);
int32_t linkNode_isId(LinkNode *self, int64_t id);

LinkNode *New_linkNode(void *args);
#endif
