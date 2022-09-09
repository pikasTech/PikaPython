#include "GTestTask.h"
#include "GTestTask_Task.h"
#include "PikaStdData_Dict.h"

static volatile uint64_t tick_ms = 0;
void GTestTask_Task_platformGetTick(PikaObj* self) {
    tick_ms += 50;
    obj_setInt(self, "tick", tick_ms);
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
