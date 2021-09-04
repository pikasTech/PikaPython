/* banchmark for memory */
#include "BaseObj.h"
#include "PikaStdLib_SysObj.h"
#include "dataStrs.h"
#include <stdio.h>

void checker_printMem(char *info, uint32_t size)
{
    printf("%s", info);
    if (size <= 1024)
    {
        printf("%d byte\r\n", size);
        return;
    }
    printf("%0.2f Kb\r\n", size / 1024.0);
    return;
}

void checker_printMemUsage(char *testName)
{
    printf("---------------------------\r\n");
    printf("Testing :%s\r\n", testName);
    checker_printMem("    max = ", pikaMemMax());
    checker_printMem("    now = ", pikaMemNow());
    printf("---------------------------\r\n");
}
void checker_memInfo(void)
{
    printf("---------------------------\r\n");
    printf("Memory pool info:\r\n");
    checker_printMem("  mem state size = ", sizeof(PikaMemInfo));
    printf("---------------------------\r\n");
}
void checker_assertMemFree()
{
    if (0 == pikaMemNow())
    {
        pikaMemMaxReset();
        return;
    }
    printf("[Error]: Memory free error.\r\n");
    while (1)
        ;
}

void checker_objMemChecker(PikaObj *(*NewFun)(Args *), char *objName)
{
    {
        /* new root object */
        PikaObj *obj = newRootObj("obj", NewFun);
        char testName[256] = {0};
        sprintf(testName, "Root %s object", objName);
        checker_printMemUsage(testName);
        obj_deinit(obj);
        checker_assertMemFree();
    }

    {
        PikaObj *obj = NewFun(NULL);
        char testName[256] = {0};
        sprintf(testName, "%s object", objName);
        checker_printMemUsage(testName);
        obj_deinit(obj);
        checker_assertMemFree();
    }
}

int32_t main()
{
    checker_objMemChecker(New_TinyObj, "tiny");
    checker_objMemChecker(New_BaseObj, "base");
    checker_objMemChecker(New_PikaStdLib_SysObj, "sys");
    {
        Arg *arg = New_arg(NULL);
        checker_printMemUsage("void arg");
        arg_deinit(arg);
        checker_assertMemFree();
    }
    {
        Link *link = New_link(NULL);
        checker_printMemUsage("void link");
        link_deinit(link);
        checker_assertMemFree();
    }
    {
        Arg *arg = New_arg(NULL);
        arg = arg_setInt(arg, "testInt1", 0);
        checker_printMemUsage("int arg");
        arg_deinit(arg);
        checker_assertMemFree();
    }
    {
        Arg *arg = New_arg(NULL);
        arg = arg_setFloat(arg, "testFloat1", 0);
        checker_printMemUsage("float arg");
        arg_deinit(arg);
        checker_assertMemFree();
    }
    {
        Arg *arg = New_arg(NULL);
        arg = arg_setStr(arg, "testStr", "test string");
        checker_printMemUsage("str arg");
        arg_deinit(arg);
        checker_assertMemFree();
    }
    {
        Args *args = New_args(NULL);
        checker_printMemUsage("void args");
        args_deinit(args);
        checker_assertMemFree();
    }
    {
        Args *args = New_args(NULL);
        args_setInt(args, "testInt1", 0);
        checker_printMemUsage("one int args");
        args_deinit(args);
        checker_assertMemFree();
    }
    {
        Args *args = New_args(NULL);
        args_setInt(args, "testInt1", 0);
        args_setInt(args, "testInt2", 0);
        checker_printMemUsage("two int args");
        args_deinit(args);
        checker_assertMemFree();
    }
    {
        Args *args = New_args(NULL);
        args_setFloat(args, "testFloat", 0);
        checker_printMemUsage("one float args");
        args_deinit(args);
        checker_assertMemFree();
    }
    {
        Args *args = New_args(NULL);
        args_setFloat(args, "testFloat", 0);
        args_setFloat(args, "testFLoat", 0);
        checker_printMemUsage("two float args");
        args_deinit(args);
        checker_assertMemFree();
    }
    checker_memInfo();
}
