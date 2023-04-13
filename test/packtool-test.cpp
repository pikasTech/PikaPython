#include "test_common.h"
TEST_START

#include "PikaCompiler.h"

TEST(packtool, pack_unpack) {
    PIKA_RES res = pikafs_pack_files(
        "test/out/a.pack", 3, "test/assets/test.txt",
        "test/assets/widget_config.ini", "test/assets/test.jpg");
    pikafs_unpack_files("test/out/a.pack", "test/out/unpack");

    EXPECT_EQ(res, PIKA_RES_OK);
}

TEST(packtool, packread) {
    size_t n = 0;
    // Arg* fileArg = NULL;
    pikafs_pack_files("test/out/a2.pack", 3, "test/assets/test.txt",
                      "test/assets/widget_config.ini", "test/assets/test.jpg");
    pikafs_FILE* pack_file = pikafs_fopen_pack("test/out/a2.pack", "test.txt");
    // pikafs_FILE* pack_file = pikafs_fopen_pack_new(&fileArg,
    // "test/assets/a.pack", "main.py");
    FILE* file = pika_platform_fopen("test/out/unpack/test2.txt", "wb+");
    if (NULL == file) {
        pika_platform_printf("open file: %s error\r\n",
                             "test/out/unpack/test2.txt");
    }

    n = pika_platform_fwrite(pack_file->addr, pack_file->size, 1, file);
    EXPECT_NE(n, 0);

    // arg_deinit(fileArg);
    pikafs_fclose(pack_file);
    pika_platform_fclose(file);
    pack_file = NULL;
}

TEST_END