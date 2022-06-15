#include "pika_lvgl_bar.h"
#include "lvgl.h"

void pika_lvgl_bar___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_bar_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_bar_set_value(PikaObj* self, int anim, int value) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_bar_set_value(lv_obj, value, value);
}
