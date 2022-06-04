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

TEST(unix_time, time) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    obj_run(self,
            "mytime = TemplateDevice.Time()\n"
            "t1= mytime.time()\n"
            "t2= mytime.time()\n");
    /* 获取数据比对 */
    float t1 = obj_getFloat(self, "t1");
    float t2 = obj_getFloat(self, "t2");
    /* assert */
    EXPECT_FLOAT_EQ(t1, 0.05);
    EXPECT_FLOAT_EQ(t2, 0.1);
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(unix_time, unix_time) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    obj_run(self,
            "mytime = PikaStdDevice.Time()\n"
            "mytime.localtime(0.0)\n");
    /* 获取数据比对 */
    int tm_sec = obj_getInt(self, "mytime.tm_sec");
    int tm_min = obj_getInt(self, "mytime.tm_min");
    int tm_hour = obj_getInt(self, "mytime.tm_hour");
    int tm_mday = obj_getInt(self, "mytime.tm_mday");
    int tm_mon = obj_getInt(self, "mytime.tm_mon");
    int tm_year = obj_getInt(self, "mytime.tm_year");
    int tm_wday = obj_getInt(self, "mytime.tm_wday");
    int tm_yday = obj_getInt(self, "mytime.tm_yday");
    int tm_isdst = obj_getInt(self, "mytime.tm_isdst");
    /* assert */
    EXPECT_EQ(tm_sec, 0);
    EXPECT_EQ(tm_min, 0);
    EXPECT_EQ(tm_hour, 8);
    EXPECT_EQ(tm_mday, 1);
    EXPECT_EQ(tm_mon, 0);  // 1月
    EXPECT_EQ(tm_year, 1970);
    EXPECT_EQ(tm_wday, 4);  //周四
    EXPECT_EQ(tm_yday, 1);
    EXPECT_EQ(tm_isdst, -1);
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
