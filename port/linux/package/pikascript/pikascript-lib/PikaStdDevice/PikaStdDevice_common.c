#include "PikaStdDevice_common.h"

extern PikaEventListener* g_pika_device_event_listener;
void _PikaStdDevice_event_handler(pika_dev* dev, int signal) {
    pika_eventListener_sendSignal(g_pika_device_event_listener, (uintptr_t)dev,
                                  signal);
}

void _PikaStdDevice_setCallback(PikaObj* self,
                                Arg* eventCallback,
                                uint32_t eventId) {
    pika_eventListener_registEventCallback(g_pika_device_event_listener,
                                           eventId, eventCallback);
}

extern volatile PikaObj* __pikaMain;
PikaObj* PikaStdDevice_Time(PikaObj* self) {
    PikaObj* time = obj_getPtr((PikaObj*)__pikaMain, "time");
    if (NULL == time) {
        obj_setErrorCode(self, -1);
        obj_setSysOut(self, "Error: please install and import 'time' module");
        return NULL;
    }
    obj_refcntInc(time);
    return time;
}
