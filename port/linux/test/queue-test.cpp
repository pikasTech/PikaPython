#include "gtest/gtest.h"
extern "C" {
#include "BaseObj.h"
#include "dataMemory.h"
#include "dataQueue.h"
#include "dataQueueObj.h"
}
static int mem;

TEST(queue, NEW) {
    Queue* q = New_queue();
    queue_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(queue, INT) {
    Queue* q = New_queue();
    queue_pushInt(q, 1);
    queue_pushInt(q, 2);
    queue_pushInt(q, 3);
    EXPECT_EQ(queue_popInt(q), 1);
    EXPECT_EQ(queue_popInt(q), 2);
    EXPECT_EQ(queue_popInt(q), 3);
    queue_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(queue, FLOAT) {
    Queue* q = New_queue();
    queue_pushFloat(q, 1.1f);
    queue_pushFloat(q, 2.2f);
    queue_pushFloat(q, 3.3f);
    EXPECT_EQ(queue_popFloat(q), 1.1f);
    EXPECT_EQ(queue_popFloat(q), 2.2f);
    EXPECT_EQ(queue_popFloat(q), 3.3f);
    queue_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(queue, str) {
    Queue* q = New_queue();
    queue_pushStr(q, (char*)"abc");
    queue_pushStr(q, (char*)"123");
    queue_pushStr(q, (char*)"xyz");
    EXPECT_STREQ(queue_popStr(q), (char*)"abc");
    EXPECT_STREQ(queue_popStr(q), (char*)"123");
    EXPECT_STREQ(queue_popStr(q), (char*)"xyz");
    queue_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(queueObj, init) {
    QueueObj* q = New_BaseObj(NULL);
    queueObj_init(q);
    obj_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(queueObj, INT) {
    QueueObj* q = New_BaseObj(NULL);
    queueObj_init(q);
    queueObj_pushInt(q, 1);
    queueObj_pushInt(q, 2);
    queueObj_pushInt(q, 3);
    EXPECT_EQ(queueObj_popInt(q), 1);
    EXPECT_EQ(queueObj_popInt(q), 2);
    EXPECT_EQ(queueObj_popInt(q), 3);
    obj_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(queueObj, FLOAT) {
    QueueObj* q = New_BaseObj(NULL);
    queueObj_init(q);
    queueObj_pushFloat(q, 1.1f);
    queueObj_pushFloat(q, 2.2f);
    queueObj_pushFloat(q, 3.3f);
    EXPECT_EQ(queueObj_popFloat(q), 1.1f);
    EXPECT_EQ(queueObj_popFloat(q), 2.2f);
    EXPECT_EQ(queueObj_popFloat(q), 3.3f);
    obj_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(queueObj, str) {
    QueueObj* q = New_BaseObj(NULL);
    queueObj_init(q);
    queueObj_pushStr(q, (char*)"abc");
    queueObj_pushStr(q, (char*)"123");
    queueObj_pushStr(q, (char*)"xyz");
    EXPECT_STREQ(queueObj_popStr(q), (char*)"abc");
    EXPECT_STREQ(queueObj_popStr(q), (char*)"123");
    EXPECT_STREQ(queueObj_popStr(q), (char*)"xyz");
    obj_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}