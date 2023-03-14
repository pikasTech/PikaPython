/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon 李昂 liang6516@outlook.com
 * Copyright (c) 2023 Gorgon Meducer embedded_zhuroan@hotmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#define __DATA_QUEUE_CLASS_IMPLEMENT__
#include "dataQueue.h"
#include "PikaPlatform.h"
#include "dataArgs.h"

void queue_init(Queue* queue) {
    args_setInt(queue, "__t", 0);
    args_setInt(queue, "__b", 0);
}

Queue* New_queue(void) {
    Args* args = New_args(NULL);
    queue_init(args);
    return (Queue*)args;
}

int32_t queue_deinit(Queue* queue) {
    Args* args = queue;
    args_deinit(args);
    return 0;
}

int32_t queue_pushArg(Queue* queue, Arg* arg) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "__t");
    /* add top */
    args_setInt(args, "__t", top + 1);
    char buff[11];
    arg = arg_setName(arg, fast_itoa(buff, top));
    return args_setArg(args, arg);
}

Arg* __queue_popArg_noRmoveArg(Queue* queue) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "__t");
    uint64_t bottom = args_getInt(args, "__b");
    if (top - bottom < 1) {
        return NULL;
    }
    /* add bottom */
    args_setInt(args, "__b", bottom + 1);
    char buff[11];
    Arg* res = args_getArg(args, fast_itoa(buff, bottom));
    /* not deinit arg to keep str buff */
    return res;
}

int32_t queue_pushInt(Queue* queue, int val) {
    return queue_pushArg(queue, arg_newInt(val));
}

int64_t queue_popInt(Queue* queue) {
    return arg_getInt(__queue_popArg_noRmoveArg(queue));
}

int32_t queue_pushFloat(Queue* queue, pika_float val) {
    return queue_pushArg(queue, arg_newFloat(val));
}

pika_float queue_popFloat(Queue* queue) {
    return arg_getFloat(__queue_popArg_noRmoveArg(queue));
}

int32_t queue_pushStr(Queue* queue, char* str) {
    return queue_pushArg(queue, arg_newStr(str));
}

char* queue_popStr(Queue* queue) {
    return arg_getStr(__queue_popArg_noRmoveArg(queue));
}

ByteQueue* byteQueue_init(ByteQueue* queue,
                          void* buffer,
                          uint_fast16_t size,
                          PIKA_BOOL is_queue_full) {
    pika_assert(NULL != queue);
    pika_assert(NULL != buffer);
    pika_assert(size > 0);

    pika_platform_memset(queue, 0, sizeof(ByteQueue));

    queue->buffer = buffer;
    queue->buffer_size = size;
    if (is_queue_full) {
        queue->count = size;
        queue->peek_count = size;
    }

    return queue;
}

PIKA_BOOL byteQueue_readOne(ByteQueue* queue, uint8_t* byte_ptr) {
    pika_assert(NULL != queue);
    uint8_t byte;
    PIKA_BOOL result = PIKA_FALSE;

    /* ------------------atomicity sensitive start---------------- */
    do {
        if ((queue->head == queue->tail) && (0 == queue->count)) {
            /* empty */
            break;
        }

        byte = queue->buffer[queue->head++];
        queue->count--;
        if (queue->head >= queue->buffer_size) {
            queue->head = 0;
        }

        /* reset peek */
        queue->peek_count = queue->count;
        queue->peek = queue->head;

        if (NULL != byte_ptr) {
            *byte_ptr = byte;
        }
        result = PIKA_TRUE;
    } while (0);
    /* ------------------atomicity sensitive end  ---------------- */

    return result;
}

PIKA_BOOL byteQueue_peekOne(ByteQueue* queue, uint8_t* byte_ptr) {
    pika_assert(NULL != queue);
    uint8_t byte;
    PIKA_BOOL result = PIKA_FALSE;

    /* ------------------atomicity sensitive start---------------- */
    do {
        if ((queue->peek == queue->tail) && (0 == queue->peek_count)) {
            /* empty */
            break;
        }

        byte = queue->buffer[queue->peek++];
        queue->peek_count--;
        if (queue->peek >= queue->buffer_size) {
            queue->peek = 0;
        }

        if (NULL != byte_ptr) {
            *byte_ptr = byte;
        }
        result = PIKA_TRUE;
    } while (0);
    /* ------------------atomicity sensitive end  ---------------- */

    return result;
}

void byteQueue_resetPeek(ByteQueue* queue) {
    pika_assert(NULL != queue);
    /* ------------------atomicity sensitive start---------------- */
    queue->peek_count = queue->count;
    queue->peek = queue->head;
    /* ------------------atomicity sensitive end  ---------------- */
}

uint_fast16_t byteQueue_getPeekedNumber(ByteQueue* queue) {
    return queue->count - queue->peek_count;
}

uint_fast16_t byteQueue_peekAvailableCount(ByteQueue* queue) {
    return queue->peek_count;
}

void byteQueue_dropAllPeeked(ByteQueue* queue) {
    pika_assert(NULL != queue);
    /* ------------------atomicity sensitive start---------------- */
    queue->count = queue->peek_count;
    queue->head = queue->peek;
    /* ------------------atomicity sensitive end  ---------------- */
}

PIKA_BOOL byteQueue_writeOne(ByteQueue* queue, uint8_t byte) {
    pika_assert(NULL != queue);
    PIKA_BOOL result = PIKA_FALSE;

    /* ------------------atomicity sensitive start---------------- */
    do {
        if ((queue->head == queue->tail) && (0 != queue->count)) {
            /* full */
            break;
        }

        queue->buffer[queue->tail++] = byte;
        queue->count++;
        queue->peek_count++;
        if (queue->tail >= queue->buffer_size) {
            queue->tail = 0;
        }

        result = PIKA_TRUE;
    } while (0);
    /* ------------------atomicity sensitive end  ---------------- */

    return result;
}
