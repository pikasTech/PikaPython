#include "test_common.h"
TEST_START

#include "PikaCompiler.h"
TEST(packtool, unpack) {

    PIKA_RES res = LibObj_unpackFileToPath("test/assets/a.pack", "test/out/unpackout");

    EXPECT_EQ(res, PIKA_RES_OK);
}


TEST(packtool, packread) {
    size_t n = 0;
    //Arg* fileArg = NULL;
     pikafs_FILE* pack_file = pikafs_fopen_pack("test/assets/a.pack", "main.py");
   // pikafs_FILE* pack_file = pikafs_fopen_pack_new(&fileArg, "test/assets/a.pack", "main.py");
    FILE* file = pika_platform_fopen("test/out/unpackout/main2.py", "wb+");
    if (NULL == file) {
        pika_platform_printf("open file: %s error\r\n", "test/out/unpackout/main2.py");
    }

    n = pika_platform_fwrite(pack_file->addr, pack_file->size, 1, file);
    EXPECT_NE(n, 0);

    //arg_deinit(fileArg);
    pikaFree(pack_file, sizeof(pikafs_FILE));
    pika_platform_fclose(file);
    pack_file = NULL;

}

TEST_END