#include "pika_lvgl_table.h"
#include "lvgl.h"

void pika_lvgl_table___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_table_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_table_set_cell_value(PikaObj* self,
                                    int row,
                                    int col,
                                    char* txt) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_table_set_cell_value(lv_obj, row, col, txt);
}
