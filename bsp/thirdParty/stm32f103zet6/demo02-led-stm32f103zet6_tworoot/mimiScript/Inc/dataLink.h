#ifndef _link2__H
#define _link2__H
#include "dataLinkNode.h"
#include "dataMemory.h"

typedef struct Class_link Link;
struct Class_link
{
    /* attribute */
    DMEM *mem;
    LinkNode *firstNode;
    long long TopId;
 
    /* virtual operation */

    /* object */
};

void link_deinit(Link *self);
void link_init(Link *self, void *args);
void link_addNode(Link *self, void *contant, void (*_contantDinit)(void *contant));
void link_removeNode(Link *self, void *contant);
LinkNode *link_getNode(Link *self, long long id);
int link_getSize(Link *self);

Link *New_link(void *args);
#endif
