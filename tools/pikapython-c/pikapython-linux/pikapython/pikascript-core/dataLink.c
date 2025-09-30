/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include "dataLink.h"
#include "dataArg.h"
#include "dataLinkNode.h"
#include "dataMemory.h"

void _link_deinit_pyload(Link* self, pika_bool is_object);
void link_deinit_ex(Link* self, pika_bool is_object) {
    pika_assert(self != NULL);
    _link_deinit_pyload(self, is_object);
    pikaFree(self, sizeof(Link));
}

void link_deinit(Link* self) {
    link_deinit_ex(self, 0);
}

void link_deinit_stack(Link* self) {
    _link_deinit_pyload(self, 0);
}

void link_addNode(Link* self, void* content) {
    // old first node become new second node
    LinkNode* secondNode = self->firstNode;

    self->firstNode = content;
    // change the first node to new node
    arg_setNext((Arg*)content, (Arg*)secondNode);
}

static void _link_removeNode(Link* self,
                             void* content,
                             uint8_t is_deinit_node) {
    LinkNode* nodeToDelete = NULL;
    LinkNode* nodeNow = self->firstNode;
    LinkNode* priorNode = NULL;
    LinkNode* nextNode;
    while (1) {
        if (nodeNow == content) {
            nodeToDelete = nodeNow;
            break;
        }
        if (nodeNow == NULL) {
            // error, node no found
            goto __exit;
        }
        priorNode = nodeNow;
        nodeNow = (LinkNode*)arg_getNext((Arg*)nodeNow);
    }

    nextNode = (LinkNode*)arg_getNext((Arg*)nodeToDelete);
    if (nodeToDelete == self->firstNode) {
        self->firstNode = (LinkNode*)arg_getNext((Arg*)nodeToDelete);
    }
    if (NULL == priorNode) {
        self->firstNode = nextNode;
        goto __exit;
    }
    arg_setNext((Arg*)priorNode, (Arg*)nextNode);
    goto __exit;

// deinit the node
__exit:
    if (is_deinit_node) {
        pika_assert(NULL != nodeToDelete);
        linkNode_deinit(nodeToDelete);
    }
    return;
}

void link_removeNode(Link* self, void* content) {
    _link_removeNode(self, content, 1);
}

void link_removeNode_notDeinitNode(Link* self, void* content) {
    _link_removeNode(self, content, 0);
}

int32_t link_getSize(Link* self) {
    LinkNode* NowNode;
    int32_t size = 0;
    NowNode = self->firstNode;
    while (NULL != NowNode) {
        size++;
        NowNode = (LinkNode*)arg_getNext((Arg*)NowNode);
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
