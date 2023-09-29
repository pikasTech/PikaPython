#include "PikaVM.h"
#include "dataStrs.h"

void PikaDebug_Debuger___init__(PikaObj* self) {
    /* global enable contral */
    obj_setInt(self, "enable", 1);
}

void PikaDebug_Debuger_set_trace(PikaObj* self) {
    pika_debug_set_trace(self);
}

void PikaDebug_set_trace(PikaObj* self) {
    PikaDebug_Debuger_set_trace(self);
}

void PikaDebug_set_break(PikaObj* self, char* module, int pc_break) {
    pika_debug_set_break(module, pc_break);
}

void PikaDebug_reset_break(PikaObj* self, char* module, int pc_break) {
    pika_debug_reset_break(module, pc_break);
}
