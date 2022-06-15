#include "GTestTask.h"
#include "GTestTask_Task.h"

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
