/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon 李昂 liang6516@outlook.com
 * Copyright (c) 2023 Gorgon Meducer embedded_zhuroan@hotmail.comByte
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
#ifdef __cplusplus
extern "C" {
#endif
#ifndef __DATA_QUEUE__H
#define __DATA_QUEUE__H
#include "dataArgs.h"

/*! \NOTE: Make sure #include "__pika_ooc.h" is close to the class definition
 */
#if defined(__DATA_QUEUE_CLASS_IMPLEMENT__)
    #define __PLOOC_CLASS_IMPLEMENT__
    #undef __DATA_QUEUE_CLASS_IMPLEMENT__
#endif
#include "__pika_ooc.h"

typedef struct ByteQueue ByteQueue;
struct ByteQueue {
private_member(
    uint8_t *buffer;
    uint16_t buffer_size;

    uint16_t head;
    uint16_t tail;
    uint16_t peek;

    uint16_t count;
    uint16_t peek_count;
)
};

typedef Args Queue;
Queue* New_queue(void);

int32_t queue_deinit(Queue* queue);
int32_t queue_pushInt(Queue* queue, int val);
int32_t queue_pushFloat(Queue* queue, pika_float val);
int32_t queue_pushStr(Queue* queue, char* str);
int32_t queue_pushArg(Queue* queue, Arg* arg);
char* fast_itoa(char* buf, uint32_t val);

int64_t queue_popInt(Queue* queue);
pika_float queue_popFloat(Queue* queue);
char* queue_popStr(Queue* queue);
Arg* queue_popArg(Queue* queue);
Arg* queue_popArg_notDeinitArg(Queue* queue);
int32_t queue_deinit_stack(Queue* queue);
void queue_init(Queue* queue);

ByteQueue *byteQueue_init( ByteQueue *queue, 
                            void *buffer, 
                            uint_fast16_t size, 
                            PIKA_BOOL is_queue_full);
PIKA_BOOL byteQueue_readOne(ByteQueue *queue, uint8_t *byte_ptr);
PIKA_BOOL byteQueue_peekOne(ByteQueue *queue, uint8_t *byte_ptr);
void byteQueue_resetPeek(ByteQueue *queue);
void byteQueue_dropAllPeeked(ByteQueue *queue);
uint_fast16_t byteQueue_getPeekedNumber(ByteQueue *queue);
uint_fast16_t byteQueue_peekAvailableCount(ByteQueue *queue);
PIKA_BOOL byteQueue_writeOne(ByteQueue *queue, uint8_t byte);

#endif
#ifdef __cplusplus
}
#endif