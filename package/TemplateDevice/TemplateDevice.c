#include "TemplateDevice.h"

PikaEventListener* g_pika_device_event_listener;
void TemplateDevice___del__(PikaObj *self){
    pks_eventLisener_deinit(&g_pika_device_event_listener);
}
