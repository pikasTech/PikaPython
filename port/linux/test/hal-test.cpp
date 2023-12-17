#include "test_common.h"

TEST(hal, open) {
    pika_dev* io = pika_hal_open(PIKA_HAL_GPIO, "PA10");
    pika_hal_close(io);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(hal, cq_init) {
    pika_hal_CircularQueue* cq = pika_hal_circularQueue_create(10);
    pika_hal_circularQueue_deinit(cq);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(hal, cq_enqueue_dequeue) {
    pika_hal_CircularQueue* cq = pika_hal_circularQueue_create(10);
    pika_hal_circularQueue_enqueue(cq, 1);
    pika_hal_circularQueue_enqueue(cq, 2);
    pika_hal_circularQueue_enqueue(cq, 3);
    uint8_t recv = 0;
    pika_hal_circularQueue_dequeue(cq, &recv);
    EXPECT_EQ(recv, 1);
    pika_hal_circularQueue_dequeue(cq, &recv);
    EXPECT_EQ(recv, 2);
    pika_hal_circularQueue_dequeue(cq, &recv);
    EXPECT_EQ(recv, 3);
    pika_hal_circularQueue_deinit(cq);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(hal, cq_enqueue_overflow) {
    pika_hal_CircularQueue* cq = pika_hal_circularQueue_create(10);
    for (int i = 0; i < 10; i++) {
        pika_hal_circularQueue_enqueue(cq, i);
    }
    EXPECT_EQ(pika_hal_circularQueue_enqueue(cq, 10), -1);
    pika_hal_circularQueue_deinit(cq);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(hal, cq_dequeue_empty) {
    pika_hal_CircularQueue* cq = pika_hal_circularQueue_create(10);
    uint8_t recv = 0;
    EXPECT_EQ(pika_hal_circularQueue_dequeue(cq, &recv), -1);
    pika_hal_circularQueue_deinit(cq);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(hal, cq_enqueue_dequeue_wrap) {
    pika_hal_CircularQueue* cq = pika_hal_circularQueue_create(10);
    for (int i = 0; i < 10; i++) {
        pika_hal_circularQueue_enqueue(cq, i);
    }
    uint8_t recv = 0;
    for (int i = 0; i < 10; i++) {
        pika_hal_circularQueue_dequeue(cq, &recv);
        EXPECT_EQ(recv, i);
    }
    pika_hal_circularQueue_deinit(cq);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(hal, cq_enqueue_dequeue_wrap2) {
    pika_hal_CircularQueue* cq = pika_hal_circularQueue_create(10);
    for (int i = 0; i < 10; i++) {
        pika_hal_circularQueue_enqueue(cq, i);
    }
    uint8_t recv = 0;
    for (int i = 0; i < 5; i++) {
        pika_hal_circularQueue_dequeue(cq, &recv);
        EXPECT_EQ(recv, i);
    }
    for (int i = 10; i < 15; i++) {
        pika_hal_circularQueue_enqueue(cq, i);
    }
    for (int i = 5; i < 15; i++) {
        pika_hal_circularQueue_dequeue(cq, &recv);
        EXPECT_EQ(recv, i);
    }
    pika_hal_circularQueue_deinit(cq);
    EXPECT_EQ(pikaMemNow(), 0);
}
