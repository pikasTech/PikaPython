#include "pika_lvgl_roller.h"
#include "lvgl.h"

void pika_lvgl_roller___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_roller_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_roller_set_options(PikaObj *self, char* options, int mode){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_roller_set_options(lv_obj, options, mode);
}

void pika_lvgl_roller_set_visible_row_count(PikaObj* self, int row_cnt) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_roller_set_visible_row_count(lv_obj, row_cnt);
}
