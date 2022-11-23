#include "GTestTask.h"
#include "GTestTask_ProxyTest.h"
#include "GTestTask_Task.h"
#include "PikaStdData_Dict.h"

static volatile uint64_t tick_ms = 0;
void GTestTask_Task_platformGetTick(PikaObj* self) {
    tick_ms += 50;
    obj_setInt(self, "tick", tick_ms);
}

void PikaStdTask_Task___init__(PikaObj* self);
void GTestTask_Task___init__(PikaObj *self){
    PikaStdTask_Task___init__(self);
    tick_ms = 0;
}

void GTestTask___init__(PikaObj* self) {
    if (!obj_isArgExist(self, "testval")) {
        obj_setInt(self, "testval", 8848);
    } else {
        obj_setInt(self, "testval", obj_getInt(self, "testval") + 1);
    }
}

char* GTestTask_Task_returnNullString(PikaObj* self) {
    obj_setErrorCode(self, 3);
    __platform_printf("returnNullString\n");
    return NULL;
}

void GTestTask_test(PikaObj* self, int a, PikaObj* b) {
    __platform_printf("GTestTask_test: %d, %lx\r\n", a, (uintptr_t)b);
}

PikaObj* GTestTask_test_dict(PikaObj* self) {
    PikaObj* dict = newNormalObj(New_PikaStdData_Dict);
    PikaStdData_Dict___init__(dict);
    Arg* para1 = arg_newInt(1);
    Arg* para2 = arg_newInt(2);
    PikaStdData_Dict_set(dict, "para1", para1);
    PikaStdData_Dict_set(dict, "para2", para2);
    arg_deinit(para1);
    arg_deinit(para2);
    return dict;
}

int64_t GTestTask_test64(PikaObj* self, int64_t a, int64_t b) {
    return a * b;
}

Arg* GTestTask_ProxyTest___getattribute__(PikaObj* self, char* __name) {
    return arg_newStr(__name);
}

void GTestTask_ProxyTest___setattr__(PikaObj* self,
                                     char* __name,
                                     Arg* __value) {
    __platform_printf("GTestTask_ProxyTest___setattr__: %s, %s\r\n", __name,
                      arg_getStr(__value));
}

void GTestTask_test_raise(PikaObj *self){
    obj_setErrorCode(self, 1);
}

void GTestTask__test___init__(PikaObj *self){
    obj_setStr(self, "a", NULL);
}
