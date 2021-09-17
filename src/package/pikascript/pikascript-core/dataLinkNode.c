/*
  Author: lyon 
  Tencent QQ: 645275593 
*/

#include "dataLinkNode.h"
#include "dataArg.h"
#include "dataMemory.h"

void linkNode_deinit(LinkNode* self) {
  arg_deinit(self);
}

void linkNode_init(LinkNode* self, void* args) {
  /* attribute */
}

LinkNode* New_linkNode(void* args) {
  return NULL;
}
