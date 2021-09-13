#ifndef _link2__H
#define _link2__H
#include "dataLinkNode.h"
#include "dataMemory.h"

typedef struct Class_link Link;
struct Class_link {
  LinkNode* firstNode;
};

void link_deinit(Link* self);
void link_init(Link* self, void* args);
void link_addNode(Link* self, void* content);
void link_removeNode(Link* self, void* content);
LinkNode* link_getNode(Link* self, int64_t id);
int32_t link_getSize(Link* self);

Link* New_link(void* args);
#endif
