#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"
#include "PikaCompiler.h"
#include "PikaObj.h"
#include "PikaParser.h"
#include "dataStrs.h"
#include "libpikabinder.h"

void help(char* argv0) {
    Args buffs = {0};
    char* exe = argv0;
    printf(
        "Usage:\r\n"
        "    %s"
        " - [Binding C modules and compile all from main.py]\r\n"
        "    %s test.py"
        " - [Compile all from test.py]\r\n"
        "    %s test.py -o out.a"
        " - [Compile all from test.py and link to out.a]\r\n"
        "    %s -c test.py"
        " - [Only compile test.py to test.py.o]\r\n"
        "    %s -c test.py -o out.o"
        " - [Only compile test.py to out.o]\r\n",
        exe, exe, exe, exe, exe);
    strsDeinit(&buffs);
}

/* fake implement */
PikaObj* __pikaMain;
void New_PikaStdLib_SysObj(void) {}
void New_PikaStdData_List(void) {}
void New_PikaStdData_Dict(void) {}
void New_PikaStdData_Tuple(void) {}
void New_PikaStdData_String(void) {}
void New_PikaStdData_ByteArray(void) {}
char* string_slice(Args* outBuffs, char* str, int start, int end) {
    return NULL;
}

int main(int argc, char** argv) {
    int parc = argc - 1;
    PikaMaker* maker = New_PikaMaker();

    /* --add-file xxx --add-file yyy */
    for (int i = 1; i < argc; i++) {
        // __platform_printf("%s\r\n", argv[i]);
        if (0 == strcmp(argv[i], "--add-file")) {
            // __platform_printf("add file: %s\r\n", argv[i + 1]);
            if (i + 1 < argc) {
                pikaMaker_linkRaw(maker, argv[i + 1]);
            }
        }
    }

    /* delete --xxx yyy */
    for (int i = 1; i < argc; i++) {
        if (0 == strcmp(argv[i], "--add-file")) {
            // printf("before delete: %d\r\n", parc);
            // for (int j = 0; j < parc; j++) {
            //     printf("%s\r\n", argv[j + 1]);
            // }
            parc -= 2;
            for (int j = i; j < argc - 2; j++) {
                argv[j] = argv[j + 2];
            }
            // printf("after delete: %d\r\n", parc);
            // for (int j = 0; j < parc; j++) {
            //     printf("%s\r\n", argv[j + 1]);
            // }
        }
    }

    if (0 == parc) {
        /* no input, default to main.py */
        /* run pika_binder to bind C modules */
        pika_binder();
        pikaMaker_compileModuleWithDepends(maker, "main");
        PIKA_RES res = pikaMaker_linkCompiledModules(maker, "pikaModules.py.a");
        pikaMaker_deinit(maker);
        return res;
    }

    /* example: ./rust-msc-latest-linux -h | --help */
    if (1 == parc) {
        if (0 == strcmp(argv[1], "-h") || 0 == strcmp(argv[1], "--help")) {
            help(argv[0]);
            return 0;
        }
    }

    /* example: ./rust-msc-latest-linux main.py */
    if (1 == parc) {
        char* module_entry = argv[1];
        /* remove subfix */
        char* subfix = strrchr(module_entry, '.');
        if (subfix) {
            *subfix = '\0';
        }
        pikaMaker_compileModuleWithDepends(maker, module_entry);
        PIKA_RES res = pikaMaker_linkCompiledModules(maker, "pikaModules.py.a");
        pikaMaker_deinit(maker);
        return res;
    }

    /* example ./rust-msc-latest-linux main.py -o out.a */
    if (3 == parc) {
        if (0 == strcmp(argv[2], "-o")) {
            char* module_entry = argv[1];
            /* remove subfix */
            char* subfix = strrchr(module_entry, '.');
            if (subfix) {
                *subfix = '\0';
            }
            pikaMaker_compileModuleWithDepends(maker, module_entry);
            PIKA_RES res = pikaMaker_linkCompiledModules(maker, argv[3]);
            pikaMaker_deinit(maker);
            return res;
        }
    }

    /* example: ./rust-msc-latest-linux -c main.py */
    if (2 == parc) {
        if (0 == strcmp(argv[1], "-c")) {
            Args buffs = {0};
            /* compile only */
            char* module_entry = argv[2];
            char* module_out = strsCopy(&buffs, module_entry);
            char* subfix = strrchr(module_out, '.');
            if (subfix) {
                *subfix = '\0';
            }
            module_out = strsAppend(&buffs, module_out, ".py.o");
            printf("compiling %s to %s...\r\n", module_entry, module_out);
            PIKA_RES res =
                pikaCompileFileWithOutputName(module_out, module_entry);
            strsDeinit(&buffs);
            return res;
        }
    }

    /* example: ./rust-msc-latest-linux -c main.py -o main.py.o */
    if (4 == parc) {
        if (0 == strcmp(argv[1], "-c") && 0 == strcmp(argv[3], "-o")) {
            /* compile only */
            char* module_entry = argv[2];
            printf("compiling %s to %s...\r\n", module_entry, argv[4]);
            PIKA_RES res = pikaCompileFileWithOutputName(argv[4], module_entry);
            return res;
        }
    }

    /* no valid input */
    printf("Invalid input.\r\n");
    help(argv[0]);

    return -1;
}
