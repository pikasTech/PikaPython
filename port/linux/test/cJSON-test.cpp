#include "gtest/gtest.h"
#include "test_common.h"
extern "C" {
#include "PikaMain.h"
#include "PikaParser.h"
#include "PikaStdLib_MemChecker.h"
#include "PikaVM.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataStrs.h"
#include "pikaScript.h"
#include "pika_config_gtest.h"
}

extern PikaMemInfo pikaMemInfo;
/* the log_buff of printf */
extern char log_buff[LOG_BUFF_MAX][LOG_SIZE];

TEST(cJSON, parse_print) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    char testjson[] =
        "{\n"
        "\"name\": \"mculover666\",\n"
        "\"age\": 22,\n"
        "\"weight\": 55.5,\n"
        "\"address\":\n"
        "{\n"
        "    \"country\": \"China\",\n"
        "    \"zip-code\": 111111\n"
        "},\n"
        "\"skill\": [\"c\", \"Java\", \"Python\"],\n"
        "\"student\": false\n"
        "}\n";
    /* run */
    obj_setStr(pikaMain, "testjson", testjson);
    obj_run(pikaMain,
            "a = pika_cjson.Parse(testjson)\n"
            "a.print()\n");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(cJSON, getItem) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    char testjson[] =
        "{\n"
        "\"name\": \"mculover666\",\n"
        "\"age\": 22,\n"
        "\"weight\": 55.5,\n"
        "\"address\":\n"
        "{\n"
        "    \"country\": \"China\",\n"
        "    \"zip-code\": 111111\n"
        "},\n"
        "\"skill\": [\"c\", \"Java\", \"Python\"],\n"
        "\"student\": false\n"
        "}\n";
    /* run */
    obj_setStr(pikaMain, "testjson", testjson);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "a = pika_cjson.Parse(testjson)\n"
            "age = a.getObjectItem('age')\n"
            "age.print()\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "22\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(cJSON, next) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    char testjson[] =
        "{\n"
        "\"name\": \"mculover666\",\n"
        "\"age\": 22,\n"
        "\"weight\": 55.5,\n"
        "\"address\":\n"
        "{\n"
        "    \"country\": \"China\",\n"
        "    \"zip-code\": 111111\n"
        "},\n"
        "\"skill\": [\"c\", \"Java\", \"Python\"],\n"
        "\"student\": false\n"
        "}\n";
    /* run */
    obj_setStr(pikaMain, "testjson", testjson);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "a = pika_cjson.Parse(testjson)\n"
            "node = a.getChild()\n"
            "for i in range(0, 3):\n"
            "    node.print()\n"
            "    node = node.getNext()\n"
            "\n");
    /* collect */
    EXPECT_STREQ(log_buff[3], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[2], "\"mculover666\"\r\n");
    EXPECT_STREQ(log_buff[1], "22\r\n");
    EXPECT_STREQ(log_buff[0], "55.5\r\n");
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(cJSON, next_get_value) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    char testjson[] =
        "{\n"
        "\"name\": \"mculover666\",\n"
        "\"age\": 22,\n"
        "\"weight\": 55.5,\n"
        "\"address\":\n"
        "{\n"
        "    \"country\": \"China\",\n"
        "    \"zip-code\": 111111\n"
        "},\n"
        "\"skill\": [\"c\", \"Java\", \"Python\"],\n"
        "\"student\": false\n"
        "}\n";
    /* run */
    obj_setStr(pikaMain, "testjson", testjson);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "a = pika_cjson.Parse(testjson)\n"
            "node = a.getChild()\n"
            "val = PikaStdData.List()\n"
            "for i in range(0, 3):\n"
            "    val.append(node.getValue())\n"
            "    node = node.getNext()\n"
            "for item in val:\n"
            "    print(item)\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[3], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[2], "mculover666\r\n");
    EXPECT_STREQ(log_buff[1], "22.000000\r\n");
    EXPECT_STREQ(log_buff[0], "55.500000\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(cJSON, item) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "a1 = pika_cjson.Null()\n"
            "a2 = pika_cjson.True_()\n"
            "a3 = pika_cjson.False_()\n"
            "a4 = pika_cjson.Bool(True)\n"
            "a5 = pika_cjson.Number(23)\n"
            "a6 = pika_cjson.String('test')\n"
            "a7 = pika_cjson.Raw('qqq')\n"
            "a8 = pika_cjson.Array()\n"
            "a9 = pika_cjson.Object()\n"
            "\n"
            "a1.print()\n"
            "a2.print()\n"
            "a3.print()\n"
            "a4.print()\n"
            "a5.print()\n"
            "a6.print()\n"
            "a7.print()\n"
            "a8.print()\n"
            "a9.print()\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[9], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[8], "null\r\n");
    EXPECT_STREQ(log_buff[7], "true\r\n");
    EXPECT_STREQ(log_buff[6], "false\r\n");
    EXPECT_STREQ(log_buff[5], "true\r\n");
    EXPECT_STREQ(log_buff[4], "23\r\n");
    EXPECT_STREQ(log_buff[3], "\"test\"\r\n");
    EXPECT_STREQ(log_buff[2], "qqq\r\n");
    EXPECT_STREQ(log_buff[1], "[]\r\n");
    EXPECT_STREQ(log_buff[0], "{\n}\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(cJSON, construct) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "import pika_cjson as cjson\n"
            "root = cjson.Object()\n"
            "root.addItemToObject('name', cjson.String('mculover666'))\n"
            "root.addItemToObject('age', cjson.Number(22))\n"
            "root.addItemToObject('weight', cjson.Number(55.5))\n"
            "address = cjson.Object()\n"
            "address.addItemToObject('country', cjson.String('China'))\n"
            "address.addItemToObject('zip-code', cjson.String('111111'))\n"
            "root.addItemToObject('address', address)\n"
            "skill = cjson.Array()\n"
            "skill.addItemToArray(cjson.String('c'))\n"
            "skill.addItemToArray(cjson.String('Java'))\n"
            "skill.addItemToArray(cjson.String('Python'))\n"
            "root.addItemToObject('skill', skill)\n"
            "root.addItemToObject('student', cjson.False_())\n"
            "root.print()\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(
        log_buff[0],
        "{\n\t\"name\":\t\"mculover666\",\n\t\"age\":\t22,\n\t\"weight\":\t55."
        "5,\n\t\"address\":\t{\n\t\t\"country\":\t\"China\",\n\t\t\"zip-code\":"
        "\t\"111111\"\n\t},\n\t\"skill\":\t[\"c\", \"Java\", "
        "\"Python\"],\n\t\"student\":\tfalse\n}\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);

    EXPECT_EQ(pikaMemNow(), 0);
}
