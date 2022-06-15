#include "lvgl.h"
#include "pika_lvgl_lv_event.h"

int pika_lvgl_lv_event_get_code(PikaObj *self){
    lv_event_t *lv_event = obj_getPtr(self, "lv_event");
    return lv_event_get_code(lv_event);
}

