#include "TemplateDevice.h"

extern PikaEventListener* g_pika_device_event_listener;

void TemplateDevice___del__(PikaObj *self){
    pika_eventListener_deinit(&g_pika_device_event_listener);
}
