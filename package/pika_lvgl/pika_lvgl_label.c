#include "pika_lvgl_label.h"
#include "lvgl.h"

void pika_lvgl_label___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_label_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_label_set_long_mode(PikaObj* self, int mode) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_label_set_long_mode(lv_obj, mode);
}

void pika_lvgl_label_set_recolor(PikaObj* self, int en) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_label_set_recolor(lv_obj, en);
}

void pika_lvgl_label_set_text(PikaObj* self, char* txt) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_label_set_text(lv_obj, txt);
}

void pika_lvgl_label_set_style_text_align(PikaObj* self,
                                          int value,
                                          int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_text_align(lv_obj, value, selector);
}
