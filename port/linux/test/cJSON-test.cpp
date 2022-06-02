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
            "a = cJSON.cJSON()\n"
            "a.parse(testjson)\n"
            "a.print()\n");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}