#include "dataArg.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataString.h"
#include "stdlib.h"
#include <stdint.h>

void arg_deinit(Arg *self)
{
    if (NULL != self->content)
    {
        pikaFree(self->content, self->contentSize);
        self->content = NULL;
        self->contentSize = 0;
    }
    if (NULL != self->name)
    {
        pikaFree(self->name, strGetSize((char *)self->name) + 1);
        self->name = NULL;
    }
    if (NULL != self->type)
    {
        pikaFree(self->type, strGetSize((char *)self->type) + 1);
        self->type = NULL;
    }

    pikaFree(self, self->memSize);
    self = NULL;
}

void arg_newcontent(Arg *self, uint32_t size)
{
    self->content = pikaMalloc(size);
    self->contentSize = size;
    for (uint32_t i = 0; i < size; i++)
    {
        self->content[i] = 0;
    }
}

void arg_setcontent(Arg *self, uint8_t *content, uint32_t size)
{
    if (NULL != self->content)
    {
        pikaFree(self->content, self->contentSize);
        self->content = NULL;
        self->contentSize = 0;
    }
    self->content = pikaMalloc(size);
    self->contentSize = size;
    memcpy(self->content, content, size);
}

void arg_setName(Arg *self, char *name)
{
    uint32_t size = strGetSize(name);
    if (NULL != self->name)
    {
        pikaFree(self->name, strGetSize((char *)self->name) + 1);
    }
    self->name = pikaMalloc(size + 1);
    // size + 1 to contain \0
    memcpy(self->name, name, size + 1);
}

void arg_setType(Arg *self, char *type)
{
    uint32_t size = strGetSize(type);
    if (NULL != self->type)
    {
        pikaFree(self->type, strGetSize((char *)self->type) + 1);
        self->type = NULL;
    }
    self->type = pikaMalloc(size + 1);
    memcpy(self->type, type, size + 1);
}

uint8_t *arg_getcontent(Arg *self)
{
    return self->content;
}

void arg_setInt(Arg *self, int64_t val)
{
    int64_t int64Temp = val;
    uint8_t contentBuff[8];
    for (uint32_t i = 0; i < 8; i++)
    {
        // add 0x30 to void \0
        contentBuff[i] = int64Temp;
        int64Temp = int64Temp >> 8;
    }
    arg_setcontent(self, contentBuff, 8);
}

void arg_setFloat(Arg *self, float val)
{
    uint8_t contentBuff[4];
    uint8_t *valPtr = (uint8_t *)&val;
    for (uint32_t i = 0; i < 4; i++)
    {
        // add 0x30 to void \0
        contentBuff[i] = valPtr[i];
    }
    arg_setcontent(self, contentBuff, 4);
}

float arg_getFloat(Arg *self)
{
    if (NULL == arg_getcontent(self))
    {
        return -999.999;
    }
    float valOut = 0;
    uint8_t *valOutPtr = (uint8_t *)(&valOut);
    uint8_t *valPtr = arg_getcontent(self);
    for (uint32_t i = 0; i < 4; i++)
    {
        valOutPtr[i] = valPtr[i];
    }
    return valOut;
}

void arg_setPtr(Arg *self, void *pointer)
{
    uint64_t pointerTemp = (uint64_t)pointer;
    uint8_t contentBuff[8];
    for (uint32_t i = 0; i < 8; i++)
    {
        // aboid \0
        contentBuff[i] = pointerTemp;
        pointerTemp = pointerTemp >> 8;
    }
    arg_setcontent(self, contentBuff, 8);
}

void arg_setStr(Arg *self, char *string)
{
    arg_setcontent(self, (uint8_t *)string, strGetSize(string) + 1);
}

int64_t arg_getInt(Arg *self)
{
    if (NULL == arg_getcontent(self))
    {
        return -999999;
    }
    int64_t int64Temp = 0;
    for (int32_t i = 7; i > -1; i--)
    {
        // add 0x30 to avoid 0
        int64Temp = (int64Temp << 8);
        int64Temp += arg_getcontent(self)[i];
    }
    return int64Temp;
}

void *arg_getPtr(Arg *self)
{
    void *pointer = NULL;
    uint64_t pointerTemp = 0;
    if (NULL == arg_getcontent(self))
    {
        return NULL;
    }
    uint8_t *content = arg_getcontent(self);
    for (int32_t i = 7; i > -1; i--)
    {
        // avoid \0
        uint8_t val = content[i];
        pointerTemp = (pointerTemp << 8);
        pointerTemp += val;
    }
    pointer = (void *)pointerTemp;
    return pointer;
}
char *arg_getStr(Arg *self)
{
    return (char *)arg_getcontent(self);
}
void arg_init(Arg *self, void *voidPointer)
{
    /* attribute */
    self->content = NULL;
    self->name = NULL;
    self->type = NULL;
    self->contentSize = 0;
}

char *arg_getName(Arg *self)
{
    return (char *)self->name;
}

char *arg_getType(Arg *self)
{
    return (char *)self->type;
}

Arg *New_arg(void *voidPointer)
{
    Arg *self = pikaMalloc(sizeof(Arg));
    self->memSize = sizeof(Arg);
    arg_init(self, voidPointer);
    return self;
}

Arg *arg_copy(Arg *argToBeCopy)
{
    Arg *argCopied = New_arg(NULL);
    arg_setcontent(argCopied, arg_getcontent(argToBeCopy), argToBeCopy->contentSize);
    arg_setName(argCopied, arg_getName(argToBeCopy));
    arg_setType(argCopied, arg_getType(argToBeCopy));
    return argCopied;
}
