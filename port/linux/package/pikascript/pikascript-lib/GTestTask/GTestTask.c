#include "GTestTask_Task.h"

volatile uint64_t tick_ms = 0;
void GTestTask_Task_platformGetTick(PikaObj* self) {
    tick_ms += 50;
    obj_setInt(self, "tick", tick_ms);
}
