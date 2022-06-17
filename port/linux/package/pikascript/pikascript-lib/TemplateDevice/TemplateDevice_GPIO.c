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
