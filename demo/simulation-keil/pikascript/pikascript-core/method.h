#ifndef METHOD__H
#define METHOD__H
#include "dataArgs.h"
void method_returnStr(Args *args, char *val);
void method_returnInt(Args *args, int32_t val);
void method_returnFloat(Args *args, float val);
void method_returnPtr(Args *args, void *val);
int32_t method_getInt(Args *args, char *argName);
float method_getFloat(Args *args, char *argName);
char *method_getStr(Args *args, char *argName);
#endif