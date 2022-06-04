#include "TemplateDevice_Time.h"

void TemplateDevice_Time_sleep_ms(PikaObj *self, int ms){

}
void TemplateDevice_Time_sleep_s(PikaObj *self, int s){

}

static volatile uint64_t tick_ms = 0;
void TemplateDevice_Time_platformGetTick(PikaObj *self){
    tick_ms += 50;
    obj_setInt(self, "tick", tick_ms);
}
