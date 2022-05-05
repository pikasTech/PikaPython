#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"
#include "PikaParser.h"
#include "dataStrs.h"
#include "PikaCompiler.h"
#include "libpikabinder.h"

PikaObj* __pikaMain;

void main() {
    /* run pika_binder to bind C modules */
    pika_binder();


    FILE* file_mian_py = fopen("main.py", "rb");
    if (NULL == file_mian_py) {
        printf("[error]: main.py no found. \r\n");
        return;
    }
    fseek(file_mian_py, 0, SEEK_END);
    long fsize = ftell(file_mian_py);
    fseek(file_mian_py, 0, SEEK_SET); /* same as rewind(f); */
    char* pyText = malloc(fsize + 1);
    fread(pyText, 1, fsize, file_mian_py);
    fclose(file_mian_py);
    pyText[fsize] = 0;

    Args* buffs = New_strBuff();
    char* pika_byte_code = Parser_multiLineToAsm(buffs, pyText);

    FILE* file_byte_code = fopen("pikaAsm.txt", "w");
    fputs(pika_byte_code, file_byte_code);
    args_deinit(buffs);
    printf("[ OK ]: write file to 'pikaAsm.txt'. \r\n");

    pikaCompileFile("main.py");

    Parser_compilePyToBytecodeArray(pyText);
    printf("\r\n\r\npress any key to exit...\r\n");
    getchar();

    return;
}
