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
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _link2__H
#define _link2__H
#include "dataLinkNode.h"
#include "dataMemory.h"

enum LINK_IS_DEINIT_SELF {
    LINK_IS_DEINIT_SELF_ENABLE,
    LINK_IS_DEINIT_SELF_DISABLE,
};

typedef struct Link Link;
struct Link {
    LinkNode* firstNode;
};

void link_deinit(Link* self);
void link_deinit_ex(Link* self, pika_bool is_object);
void link_deinit_stack(Link* self);
void link_init(Link* self, void* args);
void link_addNode(Link* self, void* content);
void link_removeNode(Link* self, void* content);
void link_removeNode_notDeinitNode(Link* self, void* content);
LinkNode* link_getNode(Link* self, int64_t id);
int32_t link_getSize(Link* self);
Link* New_link(void* args);
#endif

#ifdef __cplusplus
}
#endif
