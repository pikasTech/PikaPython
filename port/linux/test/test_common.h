#define __OOC_CPP__
#define __OOC_DEBUG__

#include "BaseObj.h"
#include "PikaCompiler.h"
#include "PikaParser.h"
#include "PikaVM.h"
#include "TinyObj.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataStrs.h"

extern "C" {
#include <stdio.h>
#include "PikaMain.h"
#include "PikaMath_Operator.h"
#include "PikaStdLib_MemChecker.h"
#include "PikaStdLib_SysObj.h"
#include "pikaScript.h"
#include "pika_config_gtest.h"
#include "pika_hal.h"
char* PikaStdData_Dict___str__(PikaObj* self);
char* PikaStdData_List___str__(PikaObj* self);

extern PikaMemInfo g_PikaMemInfo;
/* the log_buff of printf */
extern char log_buff[LOG_BUFF_MAX][LOG_SIZE];
}

#define TEST_RUN_SINGLE_FILE(_test_suite_, _test_name_, _file_name_) \
    TEST(_test_suite_, _test_name_) {                                \
        g_PikaMemInfo.heapUsedMax = 0;                               \
        PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);    \
        extern unsigned char pikaModules_py_a[];                     \
        obj_linkLibrary(pikaMain, pikaModules_py_a);                 \
        /* run */                                                    \
        __platform_printf("BEGIN\r\n");                              \
        pikaVM_runSingleFile(pikaMain, _file_name_);                 \
        /* assert */                                                 \
        /* deinit */                                                 \
        obj_deinit(pikaMain);                                        \
        EXPECT_EQ(pikaMemNow(), 0);                                  \
    }

extern "C" {
volatile static FILE* _f_getchar_fp = NULL;
static char _f_getchar(void) {
    char c = 0;
    size_t n = fread(&c, 1, 1, (FILE*)_f_getchar_fp);
    if (n > 0) {
        return c;
    }
    pika_platform_printf("f_getchar error\r\n");
    pika_assert(0);
    return -1;
}
void pikaScriptShell_withGetchar(PikaObj* self, sh_getchar getchar_fn);
}

#define TEST_RUN_REPL_FILE(_test_suite_, _test_name_, _file_name_) \
    TEST(_test_suite_, _test_name_) {                              \
        /* init */                                                 \
        g_PikaMemInfo.heapUsedMax = 0;                             \
        PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);  \
        extern unsigned char pikaModules_py_a[];                   \
        obj_linkLibrary(pikaMain, pikaModules_py_a);               \
        /* run */                                                  \
        __platform_printf("BEGIN\r\n");                            \
        _f_getchar_fp = fopen(_file_name_, "rb");                  \
        pikaScriptShell_withGetchar(pikaMain, _f_getchar);         \
        fclose((FILE*)_f_getchar_fp);                              \
        /* collect */                                              \
        /* assert */                                               \
        /* deinit */                                               \
        obj_deinit(pikaMain);                                      \
        EXPECT_EQ(pikaMemNow(), 0);                                \
    }

#define TEST_RUN_SINGLE_FILE_PASS(_test_suite_, _test_name_, _file_name_) \
    TEST(_test_suite_, _test_name_) {                                     \
        g_PikaMemInfo.heapUsedMax = 0;                                    \
        PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);         \
        extern unsigned char pikaModules_py_a[];                          \
        obj_linkLibrary(pikaMain, pikaModules_py_a);                      \
        /* run */                                                         \
        __platform_printf("BEGIN\r\n");                                   \
        pikaVM_runSingleFile(pikaMain, _file_name_);                      \
        /* assert */                                                      \
        EXPECT_STREQ(log_buff[0], "PASS\r\n");                            \
        EXPECT_STREQ(log_buff[1], "BEGIN\r\n");                           \
        /* deinit */                                                      \
        obj_deinit(pikaMain);                                             \
        EXPECT_EQ(pikaMemNow(), 0);                                       \
    }

#define TEST_RUN_SINGLE_FILE_ASSERT(_test_suite_, _test_name_, _file_name_, \
                                    __expt__)                               \
    TEST(_test_suite_, _test_name_) {                                       \
        g_PikaMemInfo.heapUsedMax = 0;                                      \
        PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);           \
        extern unsigned char pikaModules_py_a[];                            \
        obj_linkLibrary(pikaMain, pikaModules_py_a);                        \
        /* run */                                                           \
        __platform_printf("BEGIN\r\n");                                     \
        pikaVM_runSingleFile(pikaMain, _file_name_);                        \
        /* assert */                                                        \
        EXPECT_TRUE((__expt__));                                            \
        /* deinit */                                                        \
        obj_deinit(pikaMain);                                               \
        EXPECT_EQ(pikaMemNow(), 0);                                         \
    }

#define TEST_RUN_SINGLE_FILE_EXCEPT_OUTPUT(_test_suite_, _test_name_,    \
                                           _file_name_, _except_output_) \
    TEST(_test_suite_, _test_name_) {                                    \
        g_PikaMemInfo.heapUsedMax = 0;                                   \
        PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);        \
        extern unsigned char pikaModules_py_a[];                         \
        obj_linkLibrary(pikaMain, pikaModules_py_a);                     \
        /* run */                                                        \
        __platform_printf("BEGIN\r\n");                                  \
        pikaVM_runSingleFile(pikaMain, _file_name_);                     \
        /* assert */                                                     \
        EXPECT_STREQ(log_buff[0], (_except_output_));                    \
        EXPECT_STREQ(log_buff[1], "BEGIN\r\n");                          \
        /* deinit */                                                     \
        obj_deinit(pikaMain);                                            \
        EXPECT_EQ(pikaMemNow(), 0);                                      \
    }

#define TEST_RUN_SINGLE_FILE_EXCEPT_OUTPUT2(_test_suite_, _test_name_,     \
                                            _file_name_, _except_output1_, \
                                            _except_output2_)              \
    TEST(_test_suite_, _test_name_) {                                      \
        g_PikaMemInfo.heapUsedMax = 0;                                     \
        PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);          \
        extern unsigned char pikaModules_py_a[];                           \
        obj_linkLibrary(pikaMain, pikaModules_py_a);                       \
        /* run */                                                          \
        __platform_printf("BEGIN\r\n");                                    \
        pikaVM_runSingleFile(pikaMain, _file_name_);                       \
        /* assert */                                                       \
        EXPECT_STREQ(log_buff[0], (_except_output1_));                     \
        EXPECT_STREQ(log_buff[1], (_except_output2_));                     \
        EXPECT_STREQ(log_buff[2], "BEGIN\r\n");                            \
        /* deinit */                                                       \
        obj_deinit(pikaMain);                                              \
        EXPECT_EQ(pikaMemNow(), 0);                                        \
    }

#define TEST_RUN_LINES(_test_suite_, _test_name_, _lines_)   \
    TEST(_test_suite_, _test_name_) {                        \
        PikaObj* self = newRootObj("root", New_PikaMain);    \
        extern unsigned char pikaModules_py_a[];             \
        obj_linkLibrary(self, pikaModules_py_a);             \
        obj_run(self, (_lines_)); /* collect */ /* assert */ \
        obj_deinit(self);                                    \
        EXPECT_EQ(pikaMemNow(), 0);                          \
    }

#define TEST_RUN_LINES_NO_OUTPUT(_test_suite_, _test_name_, _lines_) \
    TEST(_test_suite_, _test_name_) {                                \
        PikaObj* self = newRootObj("root", New_PikaMain);            \
        extern unsigned char pikaModules_py_a[];                     \
        obj_linkLibrary(self, pikaModules_py_a);                     \
        pika_platform_printf("BEGIN\r\n");                           \
        obj_run(self, (_lines_)); /* collect */ /* assert */         \
        EXPECT_STREQ(log_buff[0], "BEGIN\r\n");                      \
        obj_deinit(self);                                            \
        EXPECT_EQ(pikaMemNow(), 0);                                  \
    }

#define TEST_RUN_LINES_PASS(_test_suite_, _test_name_, _lines_) \
    TEST(_test_suite_, _test_name_) {                           \
        PikaObj* self = newRootObj("root", New_PikaMain);       \
        extern unsigned char pikaModules_py_a[];                \
        obj_linkLibrary(self, pikaModules_py_a);                \
        obj_run(self, (_lines_)); /* collect */ /* assert */    \
        EXPECT_STREQ(log_buff[0], "PASS\r\n");                  \
        obj_deinit(self);                                       \
        EXPECT_EQ(pikaMemNow(), 0);                             \
    }

#define TEST_RUN_LINES_EXCEPT_OUTPUT(_test_suite_, _test_name_, _lines_, \
                                     _except_output_)                    \
    TEST(_test_suite_, _test_name_) {                                    \
        PikaObj* self = newRootObj("root", New_PikaMain);                \
        extern unsigned char pikaModules_py_a[];                         \
        obj_linkLibrary(self, pikaModules_py_a);                         \
        obj_run(self, (_lines_)); /* collect */ /* assert */             \
        EXPECT_STREQ(log_buff[0], (_except_output_));                    \
        obj_deinit(self);                                                \
        EXPECT_EQ(pikaMemNow(), 0);                                      \
    }

#if USE_GOOGLE_TEST
#include "gtest/gtest.h"
#define TEST_START
#define TEST_END
#else
#include "test_purec.h"
#endif
