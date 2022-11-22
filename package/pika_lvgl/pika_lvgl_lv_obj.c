#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../../lvgl.h"
#endif

#ifdef PIKASCRIPT

#include "BaseObj.h"
#include "dataStrs.h"
#include "pika_lvgl.h"
#include "pika_lvgl_arc.h"
#include "pika_lvgl_lv_event.h"
#include "pika_lvgl_lv_obj.h"

extern PikaObj* pika_lv_event_listener_g;

void pika_lvgl_lv_obj___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_obj_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_lv_obj_center(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_center(lv_obj);
}

void pika_lvgl_lv_obj_set_size(PikaObj* self, int w, int h) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_size(lv_obj, w, h);
}

void pika_lvgl_lv_obj_align(PikaObj* self, int align, int x_ofs, int y_ofs) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_align(lv_obj, align, x_ofs, y_ofs);
}

void pika_lvgl_lv_obj_set_height(PikaObj* self, int h) {
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

PikaObj* eventLisener_getHandler(PikaObj* self, uintptr_t event_id) {
    Args buffs = {0};
    char* event_name =
        strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, "%d", event_id);
    PikaObj* event_item = obj_getObj(self, event_name);
    PikaObj* event_handler = obj_getPtr(event_item, "handler");
    strsDeinit(&buffs);
    return event_handler;
}

static void __pika_event_cb(lv_event_t* e) {
    lv_obj_t* target = lv_event_get_target(e);
    PikaObj* event_handler =
        eventLisener_getHandler(pika_lv_event_listener_g, (uintptr_t)target);
    PikaObj* evt = obj_getObj(event_handler, "_event_evt");
    obj_setPtr(evt, "lv_event", e);
    obj_run(event_handler, "_event_cb(_event_evt)");
}

void eventLicener_registEvent(PikaObj* self,
                              uintptr_t event_id,
                              PikaObj* event_handler) {
    Args buffs = {0};
    char* event_name =
        strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, "%d", event_id);
    obj_newDirectObj(self, event_name, New_TinyObj);
    PikaObj* event_item = obj_getObj(self, event_name);
    obj_setRef(event_item, "handler", event_handler);
    strsDeinit(&buffs);
}

void pika_lvgl_lv_obj_add_event_cb(PikaObj* self,
                                   Arg* event_cb,
                                   int filter,
                                   void* user_data) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_add_event_cb(lv_obj, __pika_event_cb, filter, NULL);
    obj_setArg(self, "_event_cb", event_cb);
    obj_setPtr(self, "_event_user_data", user_data);
    obj_newDirectObj(self, "_event_evt", New_pika_lvgl_lv_event);
    eventLicener_registEvent(pika_lv_event_listener_g, (uintptr_t)lv_obj, self);
}

void pika_lvgl_lv_obj_add_style(PikaObj* self, PikaObj* style, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_style_t* lv_style = obj_getPtr(style, "lv_style");
    lv_obj_add_style(lv_obj, lv_style, selector);
    char sytle_ref_name[] = "_stylex";
    sytle_ref_name[sizeof(sytle_ref_name) - 2] = '0' + lv_obj->style_cnt;
    obj_setRef(self, sytle_ref_name, style);
}

int pika_lvgl_lv_obj_get_x(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_get_x(lv_obj);
}

int pika_lvgl_lv_obj_get_y(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_get_y(lv_obj);
}

void pika_lvgl_lv_obj_set_pos(PikaObj* self, int x, int y) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_pos(lv_obj, x, y);
}

void pika_lvgl_lv_obj_align_to(PikaObj* self,
                               PikaObj* base,
                               int align,
                               int x_ofs,
                               int y_ofs) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_t* lv_base = obj_getPtr(base, "lv_obj");
    lv_obj_align_to(lv_obj, lv_base, align, x_ofs, y_ofs);
}

int pika_lvgl_lv_obj_get_content_height(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_get_content_height(lv_obj);
}

int pika_lvgl_lv_obj_get_content_width(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_get_content_width(lv_obj);
}

int pika_lvgl_lv_obj_get_height(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_get_height(lv_obj);
}

int pika_lvgl_lv_obj_get_self_height(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_get_self_height(lv_obj);
}

int pika_lvgl_lv_obj_get_self_width(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_get_self_width(lv_obj);
}

int pika_lvgl_lv_obj_get_width(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_get_width(lv_obj);
}

int pika_lvgl_lv_obj_get_x2(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_get_x2(lv_obj);
}

int pika_lvgl_lv_obj_get_x_aligned(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_get_x_aligned(lv_obj);
}

int pika_lvgl_lv_obj_get_y2(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_get_y2(lv_obj);
}

int pika_lvgl_lv_obj_get_y_aligned(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_get_y_aligned(lv_obj);
}

int pika_lvgl_lv_obj_hit_test(PikaObj* self, PikaObj* point) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_point_t* lv_point = obj_getPtr(point, "lv_point");
    return lv_obj_hit_test(lv_obj, lv_point);
}

void pika_lvgl_lv_obj_invalidate(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_invalidate(lv_obj);
}

int pika_lvgl_lv_obj_is_layout_positioned(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_is_layout_positioned(lv_obj);
}

int pika_lvgl_lv_obj_is_visible(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_is_visible(lv_obj);
}

void pika_lvgl_lv_obj_mark_layout_as_dirty(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_mark_layout_as_dirty(lv_obj);
}

void pika_lvgl_lv_obj_move_to(PikaObj* self, int x, int y) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_move_to(lv_obj, x, y);
}

void pika_lvgl_lv_obj_move_children_by(PikaObj* self,
                                       int x_diff,
                                       int y_diff,
                                       int ignore_floating) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_move_children_by(lv_obj, x_diff, y_diff, ignore_floating);
}

void pika_lvgl_lv_obj_refr_pos(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_refr_pos(lv_obj);
}

int pika_lvgl_lv_obj_refr_size(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_refr_size(lv_obj);
}

int pika_lvgl_lv_obj_refresh_self_size(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_refresh_self_size(lv_obj);
}

void pika_lvgl_lv_obj_set_align(PikaObj* self, int align) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_align(lv_obj, align);
}

void pika_lvgl_lv_obj_set_content_height(PikaObj* self, int h) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_content_height(lv_obj, h);
}

void pika_lvgl_lv_obj_set_content_width(PikaObj* self, int w) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_content_width(lv_obj, w);
}

void pika_lvgl_lv_obj_set_ext_click_area(PikaObj* self, int size) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_ext_click_area(lv_obj, size);
}

void pika_lvgl_lv_obj_set_layout(PikaObj* self, int layout) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_layout(lv_obj, layout);
}

void pika_lvgl_lv_obj_set_x(PikaObj* self, int x) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_x(lv_obj, x);
}

void pika_lvgl_lv_obj_set_y(PikaObj* self, int y) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_y(lv_obj, y);
}

void pika_lvgl_lv_obj_transform_point(PikaObj* self,
                                      PikaObj* p,
                                      int recursive,
                                      int inv) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_point_t* lv_point = obj_getPtr(p, "lv_point");
    lv_obj_transform_point(lv_obj, lv_point, recursive, inv);
}

void pika_lvgl_lv_obj_add_flag(PikaObj* self, int flag) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_add_flag(lv_obj, flag);
}

void pika_lvgl_lv_obj_clear_flag(PikaObj* self, int flag) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_clear_flag(lv_obj, flag);
}

void pika_lvgl_lv_obj_set_flex_align(PikaObj* self,
                                     int main_place,
                                     int cross_place,
                                     int align) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_flex_align(lv_obj, main_place, cross_place, align);
}

void pika_lvgl_lv_obj_set_flex_flow(PikaObj* self, int flow) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_flex_flow(lv_obj, flow);
}

extern Args* pika_lv_id_register_g;
void pika_lvgl_lv_obj_set_id(PikaObj* self, char* id) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    uintptr_t id_key = (uintptr_t)lv_obj;
    Arg* id_arg = arg_newStr(id);
    id_arg->name_hash = (Hash)id_key;
    args_setArg(pika_lv_id_register_g, id_arg);
}

char* pika_lvgl_lv_obj_get_id(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    uintptr_t id_key = (uintptr_t)lv_obj;
    Arg* id_arg = args_getArg_hash(pika_lv_id_register_g, (Hash)id_key);
    if (NULL == id_arg) {
        return NULL;
    }
    return arg_getStr(id_arg);
}

void pika_lvgl_lv_obj_set_flex_grow(PikaObj *self, int value){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_flex_grow(lv_obj, value);
}

void pika_lvgl_lv_obj_clean(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_clean(lv_obj);
}

#endif
