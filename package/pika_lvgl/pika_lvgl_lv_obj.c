#include "pika_lvgl_lv_obj.h"
#include "BaseObj.h"
#include "lvgl.h"
#include "pika_lvgl.h"
#include "pika_lvgl_arc.h"

void pika_lvgl_lv_obj_center(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_center(lv_obj);
}

void pika_lvgl_lv_obj_set_size(PikaObj* self, int size_x, int size_y) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_size(lv_obj, size_x, size_y);
}

void pika_lvgl_lv_obj_align(PikaObj* self, int align, int x_ofs, int y_ofs) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_align(lv_obj, align, x_ofs, y_ofs);
}

void pika_lvgl_lv_obj_set_hight(PikaObj* self, int h) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_height(lv_obj, h);
}

void pika_lvgl_lv_obj_update_layout(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_update_layout(lv_obj);
}

void pika_lvgl_lv_obj_set_width(PikaObj* self, int w) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_width(lv_obj, w);
}

void pika_lvgl_lv_obj_add_state(PikaObj* self, int state) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_add_state(lv_obj, state);
}
