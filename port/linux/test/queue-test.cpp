#include "test_common.h"
TEST_START

TEST(queue, NEW) {
    Queue* q = New_queue();
    queue_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(queue, ARG_TYPE_INT) {
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
    Arg* arg1 = arg_newInt(1);
    Arg* arg2 = arg_newFloat(1.2);
    Arg* arg3 = arg_newStr("ij");
    queue_pushArg(q, arg1);
    queue_pushArg(q, arg2);
    queue_pushArg(q, arg3);
    EXPECT_EQ(queue_popInt(q), 1);
    EXPECT_FLOAT_EQ(queue_popFloat(q), 1.2);
    EXPECT_STREQ(queue_popStr(q), "ij");
    queue_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(queue, ARG_TYPE_FLOAT) {
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
    queue_pushStr(q, "abc");
    queue_pushStr(q, "123");
    queue_pushStr(q, "xyz");
    EXPECT_STREQ(queue_popStr(q), "abc");
    EXPECT_STREQ(queue_popStr(q), "123");
    EXPECT_STREQ(queue_popStr(q), "xyz");
    queue_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(queueObj, init) {
    QueueObj* q = New_TinyObj(NULL);
    queueObj_init(q);
    obj_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(queueObj, ARG_TYPE_INT) {
    QueueObj* q = New_TinyObj(NULL);
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

TEST(queueObj, ARG_TYPE_FLOAT) {
    QueueObj* q = New_TinyObj(NULL);
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
    QueueObj* q = New_TinyObj(NULL);
    queueObj_init(q);
    queueObj_pushStr(q, "abc");
    queueObj_pushStr(q, "123");
    queueObj_pushStr(q, "xyz");
    EXPECT_STREQ(queueObj_popStr(q), "abc");
    EXPECT_STREQ(queueObj_popStr(q), "123");
    EXPECT_STREQ(queueObj_popStr(q), "xyz");
    obj_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(queueObj, obj) {
    QueueObj* q = New_TinyObj(NULL);
    queueObj_init(q);
    queueObj_pushObj(q, "type1");
    queueObj_pushObj(q, "type2");
    queueObj_pushObj(q, "type3");
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
    QueueObj* q = New_TinyObj(NULL);
    queueObj_init(q);

    queueObj_pushObj(q, "type1");
    PikaObj* currentObj = queueObj_getCurrentObj(q);
    EXPECT_TRUE(currentObj != NULL);
    ArgType type = args_getType(q->list, "0");
    EXPECT_EQ(ARG_TYPE_OBJECT, type);
    obj_setInt(queueObj_getCurrentObj(q), "test", 1);

    queueObj_pushObj(q, "type2");
    obj_setInt(queueObj_getCurrentObj(q), "test", 2);

    queueObj_pushObj(q, "type3");
    obj_setInt(queueObj_getCurrentObj(q), "test", 3);

    PikaObj* obj1 = queueObj_popObj(q);
    PikaObj* obj2 = queueObj_popObj(q);
    PikaObj* obj3 = queueObj_popObj(q);
    PikaObj* obj4 = queueObj_popObj(q);
    EXPECT_NE((int64_t)obj1, 0);
    EXPECT_NE((int64_t)obj2, 0);
    EXPECT_NE((int64_t)obj3, 0);
    EXPECT_EQ((int64_t)obj4, 0);
    EXPECT_EQ(obj_getInt(obj1, "test"), 1);
    EXPECT_EQ(obj_getInt(obj2, "test"), 2);
    EXPECT_EQ(obj_getInt(obj3, "test"), 3);
    obj_deinit(q);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST_END
