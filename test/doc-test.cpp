#include "test_common.h"
TEST_START

#if !PIKA_NANO_ENABLE

#define TEST_FILE2DOC(_test_name_, _file_name_)                   \
    TEST(doc, _test_name_) {                                      \
        /* init */                                                \
        g_PikaMemInfo.heapUsedMax = 0;                            \
        /* run */                                                 \
        Parser* parser = New_parser();                            \
        char* docString = parser_file2Doc(parser, (_file_name_)); \
        printf("%s", docString);                                  \
        /* collect */                                             \
        /* assert */                                              \
        /* deinit */                                              \
        parser_deinit(parser);                                    \
        EXPECT_EQ(pikaMemNow(), 0);                               \
    }

#define TEST_FILE2DOCFILE(_test_name_, _file_name_, _doc_name_)   \
    TEST(doc, _test_name_) {                                      \
        /* init */                                                \
        g_PikaMemInfo.heapUsedMax = 0;                            \
        /* run */                                                 \
        Parser* parser = New_parser();                            \
        parser_file2DocFile(parser, (_file_name_), (_doc_name_)); \
        /* collect */                                             \
        /* assert */                                              \
        /* deinit */                                              \
        parser_deinit(parser);                                    \
        EXPECT_EQ(pikaMemNow(), 0);                               \
    }

TEST_FILE2DOC(_modbus, "package/pikascript/_modbus.pyi")

TEST_FILE2DOC(modbus, "package/pikascript/modbus.py")

TEST_FILE2DOC(mqtt, "package/pikascript/mqtt.py")

TEST_FILE2DOC(_thread, "package/pikascript/_thread.pyi")

TEST_FILE2DOC(PikaStdLib, "package/pikascript/PikaStdLib.pyi")

TEST_FILE2DOC(PikaStdDevice, "package/pikascript/PikaStdDevice.pyi")

TEST_FILE2DOCFILE(PikaStdDevice_file,
                  "package/pikascript/PikaStdDevice.pyi",
                  "test/out/PikaStdDevice.md")

TEST_FILE2DOCFILE(modbus_file,
                  "package/pikascript/modbus.py",
                  "test/out/modbus.md")

TEST_FILE2DOCFILE(configparser,
                  "package/pikascript/configparser.py",
                  "test/out/configparser.md")

TEST_FILE2DOCFILE(mqtt_file, "package/pikascript/mqtt.py", "test/out/mqtt.md")
TEST_FILE2DOCFILE(requests_file, "package/pikascript/requests.py", "test/out/requests.md")
TEST_FILE2DOCFILE(PikaCV_file, "package/pikascript/PikaCV.pyi", "test/out/PikaCV.md")

#endif

TEST_END
