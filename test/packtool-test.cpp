#include "test_common.h"
TEST_START

#include "PikaCompiler.h"
// TEST(packtool, unpack) {

     // PIKA_RES res = pikafs_unpack_files("test/out/packout/0424.pack", "test/out/unpackout/");

//     EXPECT_EQ(res, PIKA_RES_OK);
// }

TEST(packtool, packfiles) {

    PikaMaker* maker = New_PikaMaker();
    PIKA_RES ret = PIKA_RES_OK;

    pikaMaker_linkRaw_New(maker, "test/out/file3.txt", "/txt-file");
    pikaMaker_linkRaw_New(maker, "test/out/G.bmp", "/bmp-file");

    ret = pikaMaker_linkCompiledModulesFullPath(maker, "./test/out/packout/0424.pack");

    pikaMaker_deinit(maker);
    EXPECT_EQ(ret, PIKA_RES_OK);


}

TEST(packtool, packread) {
    size_t n = 0;
    //Arg* fileArg = NULL;
     pikafs_FILE* pack_file = pikafs_fopen_pack("test/out/packout/0424.pack", "file3.txt");
   // pikafs_FILE* pack_file = pikafs_fopen_pack_new(&fileArg, "test/assets/a.pack", "main.py");
    FILE* file = pika_platform_fopen("test/out/unpackout/file3_test.txt", "wb+");
    if (NULL == file) {
        pika_platform_printf("open file: %s error\r\n", "test/out/unpackout/file3_test.txt");
    }

     if (NULL == pack_file) {
        pika_platform_printf("open file: %s error\r\n", "test/out/packout/0424.pack");
     }

    n = pika_platform_fwrite(pack_file->addr, pack_file->size, 1, file);
    EXPECT_NE(n, 0);

    //arg_deinit(fileArg);
    pikaFree(pack_file, sizeof(pikafs_FILE));
    pika_platform_fclose(file);
    pack_file = NULL;

}

TEST_END