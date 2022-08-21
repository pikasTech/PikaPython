#include "pika_lvgl_textarea.h"
#include "lvgl.h"

void pika_lvgl_textarea___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_textarea_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_textarea_set_one_line(PikaObj* self, int en) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_textarea_set_one_line(lv_obj, en);
}
