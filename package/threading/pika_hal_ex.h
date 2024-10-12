#ifndef PIKA_HAL_CIRCULAR_PTR_QUEUE_H
#define PIKA_HAL_CIRCULAR_PTR_QUEUE_H
#include "pika_hal.h"
#include "PikaObj.h"

// 定义循环指针队列的结构体
typedef struct pika_hal_CircularPtrQueue {
    void** buffer;    // 存储元素的缓冲区
    size_t head;      // 队头指针
    size_t tail;      // 队尾指针
    size_t count;     // 当前元素数量
    size_t capacity;  // 缓冲区容量
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_t mutex;  // 互斥锁
#endif
} pika_hal_CircularPtrQueue;

// 创建一个新的循环指针队列
pika_hal_CircularPtrQueue* pika_hal_circularPtrQueue_create(size_t capacity);

// 向队列中添加一个元素
int pika_hal_circularPtrQueue_enqueue(pika_hal_CircularPtrQueue* cb,
                                      void* data);

// 从队列中移除一个元素
int pika_hal_circularPtrQueue_dequeue(pika_hal_CircularPtrQueue* cb,
                                      void** value);

// 销毁队列并释放相关资源
int pika_hal_circularPtrQueue_deinit(pika_hal_CircularPtrQueue* cb);

// 获取队列中的元素数量
size_t pika_hal_circularPtrQueue_getCount(pika_hal_CircularPtrQueue* cb);

// 检查队列是否为空
int pika_hal_circularPtrQueue_isEmpty(pika_hal_CircularPtrQueue* cb);

// 检查队列是否已满
int pika_hal_circularPtrQueue_isFull(pika_hal_CircularPtrQueue* cb);

// 查看队列头部的元素，但不移除它
int pika_hal_circularPtrQueue_peek(pika_hal_CircularPtrQueue* cb, void** value);

#endif  // PIKA_HAL_CIRCULAR_PTR_QUEUE_H
