#include "PikaObj.h"
#include "dataArgs.h"

void method_returnStr(Args *args, char *val)
{
    args_setStr(args, "return", val);
}
void method_returnInt(Args *args, int32_t val)
{
    args_setInt(args, "return", val);
}
void method_returnFloat(Args *args, float val)
{
    args_setFloat(args, "return", val);
}
void method_returnPtr(Args *args, void *val)
{
    args_setPtr(args, "return", val);
}
void method_sysOut(Args *args, char *str)
{
    args_setStr(args, "sysOut", str);
}
int32_t method_getInt(Args *args, char *argName)
{
    return args_getInt(args, argName);
}
float method_getFloat(Args *args, char *argName)
{
    return args_getFloat(args, argName);
}
char *method_getStr(Args *args, char *argName)
{
    return args_getStr(args, argName);
}

void method_setErrorCode(Args *args, int32_t errCode)
{
    args_setInt(args, "errCode", 1);
}
