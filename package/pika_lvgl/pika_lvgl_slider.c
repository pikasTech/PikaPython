#include "pika_lvgl_slider.h"
#include "lvgl.h"

void pika_lvgl_slider___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_slider_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}
