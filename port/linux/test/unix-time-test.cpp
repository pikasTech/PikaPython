#include "test_common.h"
TEST_START
#include "time.h"

extern "C" {
typedef struct tm _tm;
extern int64_t time_mktime(const _tm* this_tm, int locale);
extern void time_gmtime(double unix_time, _tm* this_tm);
extern void time_asctime(const _tm* this_tm);
void time_struct_format(const _tm* this_tm, char* str);
}

extern PikaMemInfo g_PikaMemInfo;
/* the log_buff of printf */
extern char log_buff[LOG_BUFF_MAX][LOG_SIZE];

#if 0
TEST(unix_time, time) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(self, pikaModules_py_a);
    obj_run(self,
            "import time\n"
            "t1= time.time()\n"
            "t2= time.time()\n");
    /* 获取数据比对 */
    float t1 = obj_getFloat(self, "t1");
    float t2 = obj_getFloat(self, "t2");
    /* assert */
    EXPECT_FLOAT_EQ(0.05, t2 - t1);
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

int compare(const _tm* t1, const _tm* t2) {
    int size = 8;  // 只比对前面8个数据
    int* it1 = (int*)t1;
    int* it2 = (int*)t2;
    for (int i = 0; i < size; i++) {
        // printf("t1=%d,t2=%d\n",it1[i],it2[i]);
        if (it1[i] != it2[i]) {
            // printf("mytime:  ");
            // time_asctime(t1);
            // printf("ctime:  ");
            // time_asctime(t2);
            return 1;
        }
    }
    return 0;
}

#if PIKA_FLOAT_TYPE_DOUBLE
TEST(unix_time, iteration_form_1970_to_2070) {
    /* init */
    _tm temp1, *temp2;
    int64_t tint1;
    int64_t r = 123456;
    int flag = 1;
    char str[200];
    /* run */

    /* 获取数据比对 */
    int test_num = 365 * 100;
    int record = test_num;

    while (test_num--) {
        // r=randL2();
        r += 24 * 60 * 60;
        time_gmtime(r, &temp1);
        tint1 = time_mktime(&temp1, 0);
        temp2 = gmtime(&r);
        temp1.tm_yday -= 1;
        temp1.tm_isdst = 0;
        temp2->tm_year += 1900;
        if (compare(&temp1, temp2)) {
            printf("error!\n");
            // 格式化字符
            time_struct_format(&temp1, str);
            printf("%s\n", str);
            time_struct_format(temp2, str);
            printf("%s\n", str);
            flag = 0;
            break;
        }
        if (tint1 != r) {
            printf("\n error!tint1 = %ld ,r = %ld \n", tint1, r);
            flag = 0;
            break;
        }
        // printf("\n\n");
    }
    printf("Had passed %d times test !\r\n", record - test_num - 1);
    /* assert */
    EXPECT_EQ(flag, 1);
    /* deinit */
}
#endif

TEST(timetest, sleep) {
    char* lines =
        "import time\n"
        "t = PikaStdDevice.Time()\n"
        "t.sleep(0.1)\n";
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");

    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST_RUN_SINGLE_FILE_EXCEPT_OUTPUT(time,
                                   test1,
                                   "test/python/time/time_test1.py",
                                   "PASS\r\n")

TEST_END
