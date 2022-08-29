#include "AIR32F1.h"

extern PikaEventListener* g_pika_device_event_listener;

void AIR32F1___del__(PikaObj *self){
    pks_eventLisener_deinit(&g_pika_device_event_listener);
}
