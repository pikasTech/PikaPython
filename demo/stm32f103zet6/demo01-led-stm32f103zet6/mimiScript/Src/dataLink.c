#include "dataLink.h"
#include "dataLinkNode.h"
#include "dataMemory.h"

void link_deinit(Link *self)
{
    DynMemPut(self->mem);
    LinkNode *nowNode = self->firstNode;
    while (NULL != nowNode)
    {
        linkNode_deinit(nowNode);
        nowNode = nowNode->nextNode;
    }
}

void link_addNode(Link *self, void *contant, void (*_contantDinit)(void *contant))
{
    LinkNode *NewNode = New_linkNode(NULL);
    NewNode->contant = contant;
    NewNode->_contantDinit = _contantDinit;
    NewNode->id = self->TopId;
    self->TopId++;

    // old first node become new second node
    LinkNode *secondNode = self->firstNode;

    // change the first node to new node
    self->firstNode = NewNode;

    // link the new first node and second node
    if (NULL != secondNode)
    {
        secondNode->priorNode = self->firstNode;
    }
    self->firstNode->nextNode = secondNode;
}

void link_removeNode(Link *self, void *contant)
{
    LinkNode *nodeToDelete = NULL;
    LinkNode *nodeNow = self->firstNode;
    while (1)
    {
        if (nodeNow->contant == contant)
        {
            nodeToDelete = nodeNow;
            break;
        }
        if (nodeNow->nextNode == NULL)
        {
            // error, node no found
            return;
        }
        nodeNow = nodeNow->nextNode;
    }

    LinkNode *nextNode = nodeToDelete->nextNode;
    LinkNode *priorNode = nodeToDelete->priorNode;
    if(nodeToDelete == self->firstNode)
    {
        self->firstNode = nodeToDelete->nextNode;
    }

    if (NULL != priorNode)
    {
        priorNode->nextNode = nextNode;
    }

    if (NULL != nextNode)
    {
        nextNode->priorNode = priorNode;
    }

    // deinit the node
    linkNode_deinit(nodeToDelete);
    return;
}

int link_getSize(Link *self)
{
    LinkNode *NowNode;
    int size = 0;
    NowNode = self->firstNode;
    while (NULL != NowNode)
    {
        size++;
        NowNode = NowNode->nextNode;
    }
    return size;
}

LinkNode *link_getNode(Link *self, long long id)
{
    LinkNode *nodeNow = self->firstNode;
    while (1)
    {
        if (nodeNow->id == id)
        {
            return nodeNow;
        }
        if (nodeNow->nextNode == NULL)
        {
            return NULL;
        }
        nodeNow = nodeNow->nextNode;
    }
}

void link_init(Link *self, void *args)
{
    /* attribute */
    self->firstNode = NULL;
    self->TopId = 0;

    /* object */

    /* override */
}

Link *New_link(void *args)
{
    DMEM *mem = DynMemGet(sizeof(Link));
    Link *self = (void *)(mem->addr);
    self->mem = mem;
    link_init(self, args);
    return self;
}
