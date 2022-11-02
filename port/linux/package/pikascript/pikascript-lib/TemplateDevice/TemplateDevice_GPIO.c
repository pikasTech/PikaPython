#include "TemplateDevice_GPIO.h"

void TemplateDevice_GPIO_platformDisable(PikaObj* self) {}
void TemplateDevice_GPIO_platformEnable(PikaObj* self) {}
void TemplateDevice_GPIO_platformHigh(PikaObj* self) {}
void TemplateDevice_GPIO_platformLow(PikaObj* self) {}
void TemplateDevice_GPIO_platformRead(PikaObj* self) {}
void TemplateDevice_GPIO_platformSetMode(PikaObj* self) {}

const uint32_t GPIO_PA8_EVENT_ID = 0x08;
void TemplateDevice_GPIO_platformGetEventId(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    if (strEqu(pin, "PA8")) {
        obj_setInt(self, "eventId", GPIO_PA8_EVENT_ID);
    }
}


extern PikaEventListener* g_pika_device_event_listener;
#define EVENT_SIGAL_IO_RISING_EDGE 0x01
#define EVENT_SIGAL_IO_FALLING_EDGE 0x02
#define GPIO_PA8_EVENT_ID 0x08

void TemplateDevice_GPIO_eventTest(PikaObj *self){
pks_eventLisener_sendSignal(g_pika_device_event_listener, GPIO_PA8_EVENT_ID,
                            EVENT_SIGAL_IO_FALLING_EDGE);
}
