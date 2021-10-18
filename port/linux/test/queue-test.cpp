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

TEST(queue, arg) {
    Queue* q = New_queue();
    Arg* arg1 = arg_setInt(NULL, (char*)"", 1);
    Arg* arg2 = arg_setFloat(NULL, (char*)"", 1.2);
    Arg* arg3 = arg_setStr(NULL, (char*)"", (char*)"ij");
    queue_pushArg(q, arg1);
    queue_pushArg(q, arg2);
    queue_pushArg(q, arg3);
    EXPECT_EQ(arg_getInt(queue_popArg(q)), 1);
    EXPECT_FLOAT_EQ(arg_getFloat(queue_popArg(q)), 1.2);
    EXPECT_STREQ(arg_getStr(queue_popArg(q)), (char*)"ij");
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
    QueueObj* q = TinyObj(NULL);
    queueObj_init(q);
    obj_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(queueObj, INT) {
    QueueObj* q = TinyObj(NULL);
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
    QueueObj* q = TinyObj(NULL);
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
    QueueObj* q = TinyObj(NULL);
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

TEST(queueObj, obj) {
    QueueObj* q = TinyObj(NULL);
    queueObj_init(q);
    queueObj_pushObj(q, (char*)"type1");
    queueObj_pushObj(q, (char*)"type2");
    queueObj_pushObj(q, (char*)"type3");
    PikaObj* obj1 = queueObj_popObj(q);
    PikaObj* obj2 = queueObj_popObj(q);
    PikaObj* obj3 = queueObj_popObj(q);
    PikaObj* obj4 = queueObj_popObj(q);
    EXPECT_NE((int64_t)obj1, 0);
    EXPECT_NE((int64_t)obj2, 0);
    EXPECT_NE((int64_t)obj3, 0);
    EXPECT_EQ((int64_t)obj4, 0);
    obj_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(queueObj, currentObj) {
    QueueObj* q = TinyObj(NULL);
    queueObj_init(q);

    queueObj_pushObj(q, (char*)"type1");
    obj_setInt(queueObj_getCurrentObj(q), (char*)"test", 1);

    queueObj_pushObj(q, (char*)"type2");
    obj_setInt(queueObj_getCurrentObj(q), (char*)"test", 2);

    queueObj_pushObj(q, (char*)"type3");
    obj_setInt(queueObj_getCurrentObj(q), (char*)"test", 3);

    PikaObj* obj1 = queueObj_popObj(q);
    PikaObj* obj2 = queueObj_popObj(q);
    PikaObj* obj3 = queueObj_popObj(q);
    PikaObj* obj4 = queueObj_popObj(q);
    EXPECT_NE((int64_t)obj1, 0);
    EXPECT_NE((int64_t)obj2, 0);
    EXPECT_NE((int64_t)obj3, 0);
    EXPECT_EQ((int64_t)obj4, 0);
    EXPECT_EQ(obj_getInt(obj1, (char*)"test"), 1);
    EXPECT_EQ(obj_getInt(obj2, (char*)"test"), 2);
    EXPECT_EQ(obj_getInt(obj3, (char*)"test"), 3);
    obj_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}