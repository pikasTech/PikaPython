#ifndef PIKA_HAL_CIRCULAR_PTR_QUEUE_H
#define PIKA_HAL_CIRCULAR_PTR_QUEUE_H
#include "PikaObj.h"
#include "pika_hal.h"

// Define the structure for the circular pointer queue
typedef struct pika_hal_CircularPtrQueue {
    void** buffer;    // Buffer to store elements
    size_t head;      // Head pointer
    size_t tail;      // Tail pointer
    size_t count;     // Current element count
    size_t capacity;  // Buffer capacity
#if PIKA_HAL_CIRCULAR_QUEUE_MUTEX_ENABLE
    pika_platform_thread_mutex_t mutex;  // Mutex
#endif
} pika_hal_CircularPtrQueue;

// Create a new circular pointer queue
pika_hal_CircularPtrQueue* pika_hal_circularPtrQueue_create(size_t capacity);

// Add an element to the queue
int pika_hal_circularPtrQueue_enqueue(pika_hal_CircularPtrQueue* cb,
                                      void* data);

// Remove an element from the queue
int pika_hal_circularPtrQueue_dequeue(pika_hal_CircularPtrQueue* cb,
                                      void** value);

// Destroy the queue and release related resources
int pika_hal_circularPtrQueue_deinit(pika_hal_CircularPtrQueue* cb);

// Get the number of elements in the queue
size_t pika_hal_circularPtrQueue_getCount(pika_hal_CircularPtrQueue* cb);

// Check if the queue is empty
int pika_hal_circularPtrQueue_isEmpty(pika_hal_CircularPtrQueue* cb);

// Check if the queue is full
int pika_hal_circularPtrQueue_isFull(pika_hal_CircularPtrQueue* cb);

// Peek at the front element of the queue without removing it
int pika_hal_circularPtrQueue_peek(pika_hal_CircularPtrQueue* cb, void** value);

#endif  // PIKA_HAL_CIRCULAR_PTR_QUEUE_H
