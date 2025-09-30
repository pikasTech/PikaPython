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
#include "dataQueueObj.h"
#include "BaseObj.h"
#include "dataQueue.h"
QueueObj* New_queueObj(void) {
    PikaObj* self = New_PikaObj(NULL);
    queueObj_init(self);
    return self;
}

int32_t queueObj_init(QueueObj* self) {
    obj_setInt(self, "top", 0);
    obj_setInt(self, "bottom", 0);
    return 0;
}

int32_t queueObj_pushObj(QueueObj* self, char* className) {
    uint64_t top = obj_getInt(self, "top");
    char buff[11];
    char* topStr = fast_itoa(buff, top);
    /* add top */
    obj_setInt(self, "top", top + 1);
    return obj_newObj(self, topStr, className, New_TinyObj);
}

PikaObj* queueObj_getCurrentObj(QueueObj* self) {
    uint64_t current = obj_getInt(self, "top") - 1;
    char buff[11];
    char* currentStr = fast_itoa(buff, current);
    return obj_getObj(self, currentStr);
}

PikaObj* queueObj_popObj(QueueObj* self) {
    uint64_t bottom = obj_getInt(self, "bottom");
    char buff[11];
    char* bottomStr = fast_itoa(buff, bottom);
    /* add bottom */
    obj_setInt(self, "bottom", bottom + 1);
    PikaObj* res = obj_getObj(self, bottomStr);
    return res;
}

int32_t queueObj_pushInt(QueueObj* self, int val) {
    uint64_t top = obj_getInt(self, "top");
    char buff[11];
    char* topStr = fast_itoa(buff, top);
    /* add top */
    obj_setInt(self, "top", top + 1);
    return obj_setInt(self, topStr, val);
}

int64_t queueObj_popInt(QueueObj* self) {
    uint64_t bottom = obj_getInt(self, "bottom");
    char buff[11];
    char* bottomStr = fast_itoa(buff, bottom);
    /* add bottom */
    obj_setInt(self, "bottom", bottom + 1);
    int64_t res = obj_getInt(self, bottomStr);
    obj_removeArg(self, bottomStr);
    return res;
}

int32_t queueObj_pushFloat(QueueObj* self, pika_float val) {
    uint64_t top = obj_getInt(self, "top");
    char buff[11];
    char* topStr = fast_itoa(buff, top);
    /* add top */
    obj_setInt(self, "top", top + 1);
    return obj_setFloat(self, topStr, val);
}

pika_float queueObj_popFloat(QueueObj* self) {
    uint64_t bottom = obj_getInt(self, "bottom");
    char buff[11];
    char* bottomStr = fast_itoa(buff, bottom);
    /* add bottom */
    obj_setInt(self, "bottom", bottom + 1);
    pika_float res = obj_getFloat(self, bottomStr);
    obj_removeArg(self, bottomStr);
    return res;
}

int32_t queueObj_pushStr(QueueObj* self, char* str) {
    uint64_t top = obj_getInt(self, "top");
    char buff[11];
    char* topStr = fast_itoa(buff, top);
    /* add top */
    obj_setInt(self, "top", top + 1);
    return obj_setStr(self, topStr, str);
}

char* queueObj_popStr(QueueObj* self) {
    uint64_t bottom = obj_getInt(self, "bottom");
    char buff[11];
    char* bottomStr = fast_itoa(buff, bottom);
    /* add bottom */
    obj_setInt(self, "bottom", bottom + 1);
    return obj_getStr(self, bottomStr);
}
