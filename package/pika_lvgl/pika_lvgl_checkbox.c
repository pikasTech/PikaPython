#include "pika_lvgl_checkbox.h"
#include "lvgl.h"

void pika_lvgl_checkbox___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_checkbox_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_checkbox_set_text(PikaObj *self, char* txt){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_checkbox_set_text(lv_obj, txt);
}