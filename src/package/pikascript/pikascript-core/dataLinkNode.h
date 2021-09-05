#ifndef _linkNode__H
#define _linkNode__H
#include "dataMemory.h"
typedef uint8_t LinkNode;

void linkNode_deinit(LinkNode *self);
void linkNode_init(LinkNode *self, void *args);

LinkNode *New_linkNode(void *args);
#endif
