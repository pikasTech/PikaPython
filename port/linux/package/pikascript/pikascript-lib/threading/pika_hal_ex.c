
#include "pika_hal_ex.h"

pika_hal_CircularPtrQueue* pika_hal_circularPtrQueue_create(size_t capacity) {
    pika_hal_CircularPtrQueue* cb = (pika_hal_CircularPtrQueue*)pikaMalloc(
        sizeof(pika_hal_CircularPtrQueue));
    if (NULL == cb) {
        return NULL;
    }
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_init(&cb->mutex);
#endif
    cb->capacity = capacity;
    cb->buffer = (void**)pikaMalloc(capacity *
                                    sizeof(void*));  // 分配足够的空间来存储指针
    if (NULL == cb->buffer) {
        pikaFree(cb, sizeof(pika_hal_CircularPtrQueue));
        return NULL;
    }
    return cb;
}

int pika_hal_circularPtrQueue_enqueue(pika_hal_CircularPtrQueue* cb,
                                      void* data) {
    int ret = 0;
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_lock(&cb->mutex);
#endif
    if (cb->count == cb->capacity) {
        ret = -1;
        goto __exit;
    }

    cb->buffer[cb->tail] = data;
    cb->tail = (cb->tail + 1) % cb->capacity;
    cb->count++;
__exit:
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_unlock(&cb->mutex);
#endif
    return ret;
}

int pika_hal_circularPtrQueue_dequeue(pika_hal_CircularPtrQueue* cb,
                                      void** value) {
    int ret = 0;
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_lock(&cb->mutex);
#endif
    if (cb->count == 0) {
        ret = -1;
        goto __exit;
    }

    *value = cb->buffer[cb->head];
    cb->head = (cb->head + 1) % cb->capacity;
    cb->count--;
__exit:
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_unlock(&cb->mutex);
#endif
    return ret;
}

int pika_hal_circularPtrQueue_deinit(pika_hal_CircularPtrQueue* cb) {
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_lock(&cb->mutex);
#endif
    pikaFree(cb->buffer, cb->capacity * sizeof(void*));  // 释放指针数组
    cb->buffer = NULL;
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    cb->capacity = 0;
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_destroy(&cb->mutex);
#endif
    pikaFree(cb, sizeof(pika_hal_CircularPtrQueue));
    return 0;
}

size_t pika_hal_circularPtrQueue_getCount(pika_hal_CircularPtrQueue* cb) {
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_lock(&cb->mutex);
#endif
    size_t count = cb->count;
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_unlock(&cb->mutex);
#endif
    return count;
}

int pika_hal_circularPtrQueue_isEmpty(pika_hal_CircularPtrQueue* cb) {
    return cb->count == 0;
}

int pika_hal_circularPtrQueue_isFull(pika_hal_CircularPtrQueue* cb) {
    return cb->count == cb->capacity;
}

int pika_hal_circularPtrQueue_peek(pika_hal_CircularPtrQueue* cb,
                                   void** value) {
    if (cb->count == 0) {
        return -1;
    }

#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_lock(&cb->mutex);
#endif
    *value = cb->buffer[cb->head];
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_unlock(&cb->mutex);
#endif

    return 0;
}

// 以下待测试
int pika_hal_circularPtrQueue_enqueueHead(pika_hal_CircularPtrQueue* cb,
                                          void* data) {
    int ret = 0;
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_lock(&cb->mutex);
#endif
    if (cb->count == cb->capacity) {
        ret = -1;  // 队列已满
        goto __exit;
    }

    // 更新 head 指针前的位置，然后更新 head
    cb->head = (cb->head - 1 + cb->capacity) % cb->capacity;
    cb->buffer[cb->head] = data;
    cb->count++;
__exit:
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_unlock(&cb->mutex);
#endif
    return ret;
}

int pika_hal_circularPtrQueue_dequeueTail(pika_hal_CircularPtrQueue* cb,
                                          void** value) {
    int ret = 0;
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_lock(&cb->mutex);
#endif
    if (cb->count == 0) {
        ret = -1;  // 队列为空
        goto __exit;
    }

    *value = cb->buffer[cb->tail - 1];
    cb->tail = (cb->tail - 1 + cb->capacity) % cb->capacity;
    cb->count--;
__exit:
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_unlock(&cb->mutex);
#endif
    return ret;
}

int pika_hal_circularPtrQueue_peekTail(pika_hal_CircularPtrQueue* cb,
                                       void** value) {
    if (cb->count == 0) {
        return -1;  // 队列为空
    }

#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_lock(&cb->mutex);
#endif
    *value = cb->buffer[(cb->tail - 1 + cb->capacity) % cb->capacity];
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_unlock(&cb->mutex);
#endif

    return 0;
}
