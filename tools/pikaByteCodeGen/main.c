#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"
#include "PikaParser.h"
#include "dataStrs.h"

void main() {
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

    FILE* file_byte_code = fopen("pikaByteCode.txt", "w");
    fputs(pika_byte_code, file_byte_code);
    args_deinit(buffs);
    printf("[ OK ]: write file to 'pikaByteCode.txt'. \r\n");

    return;
}


/* portable */
void __platformDisableIrqHandle(void) {
    /* disable irq to support thread */
}
void __platformEnableIrqHandle(void) {
    /* disable irq to support thread */
}
void* __platformMalloc(size_t size) {
    return malloc(size);
}
void __platformFree(void* ptr) {
    free(ptr);
}
void* __impl_pikaMalloc(size_t size) {
    return __platformMalloc(size);
}
void __impl_pikaFree(void* ptrm, size_t size) {
    __platformFree(ptrm);
}
void __platformPoolInit(void) {}
uint8_t __is_quick_malloc(void) {
    return 0;
}
void __quick_malloc_enable(void) {}
void __quick_malloc_disable(void) {}
uint8_t __isLocked_pikaMemory(void) {
    return 0;
}
void __platformPrintf(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
char* __platformLoadPikaAsm(void) {
    /* faild */
    return NULL;
}
int32_t __platformSavePikaAsm(char* PikaAsm) {
    /* faild */
    return 1;
}
uint8_t __platformAsmIsToFlash(char* pyMultiLine) {
    /* not to flash */
    return 0;
}
int32_t __platformSavePikaAsmEOF(void) {
    return 1;
}

