#include "pika_lvgl_btn.h"
#include "lvgl.h"

void pika_lvgl_btn___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_btn_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}
