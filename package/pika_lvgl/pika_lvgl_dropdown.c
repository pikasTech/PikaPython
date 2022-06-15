#include "pika_lvgl_dropdown.h"
#include "lvgl.h"

void pika_lvgl_dropdown___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_dropdown_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_dropdown_set_options(PikaObj* self, char* options) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_dropdown_set_options(lv_obj, options);
}
