/*
  Author: lyon
  Tencent QQ: 645275593
*/

#include "dataLink.h"
#include "dataArg.h"
#include "dataLinkNode.h"
#include "dataMemory.h"

void link_deinit(Link* self) {
    LinkNode* nowNode = self->firstNode;
    while (NULL != nowNode) {
        LinkNode* nodeNext = content_getNext(nowNode);
        linkNode_deinit(nowNode);
        nowNode = nodeNext;
    }
    // DynMemPut(self->mem);
    pikaFree(self, sizeof(Link));
    self = NULL;
}

void link_addNode(Link* self, void* content) {
    // old first node become new second node
    LinkNode* secondNode = self->firstNode;

    self->firstNode = content;
    // change the first node to new node
    content_setNext(content, secondNode);
}

void link_removeNode(Link* self, void* content) {
    LinkNode* nodeToDelete = NULL;
    LinkNode* nodeNow = self->firstNode;
    LinkNode* priorNode = NULL;
    while (1) {
        if (nodeNow == content) {
            nodeToDelete = nodeNow;
            break;
        }
        if (nodeNow == NULL) {
            // error, node no found
            goto exit;
        }
        priorNode = nodeNow;
        nodeNow = content_getNext(nodeNow);
    }

    LinkNode* nextNode = content_getNext(nodeToDelete);
    if (nodeToDelete == self->firstNode) {
        self->firstNode = content_getNext(nodeToDelete);
    }

    if (NULL == priorNode) {
        self->firstNode = nextNode;
        goto exit;
    }

    content_setNext(priorNode, nextNode);
    goto exit;

// deinit the node
exit:
    linkNode_deinit(nodeToDelete);
    return;
}

int32_t link_getSize(Link* self) {
    LinkNode* NowNode;
    int32_t size = 0;
    NowNode = self->firstNode;
    while (NULL != NowNode) {
        size++;
        NowNode = content_getNext(NowNode);
    }
    return size;
}

void link_init(Link* self, void* args) {
    self->firstNode = NULL;
}

Link* New_link(void* args) {
    Link* self = pikaMalloc(sizeof(Link));
    link_init(self, args);
    return self;
}
