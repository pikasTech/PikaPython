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

void pika_lvgl_lv_obj_set_style_size(PikaObj *self, int value, int selector){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_size(lv_obj, value, selector);
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

PikaObj* eventListener_getHandler(PikaObj* self, uintptr_t event_id) {
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
        eventListener_getHandler(pika_lv_event_listener_g, (uintptr_t)target);
    PikaObj* evt = obj_getObj(event_handler, "_event_evt");
    obj_setPtr(evt, "lv_event", e);
    obj_run(event_handler, "_event_cb(_event_evt)");
}

void eventListener_registEvent(PikaObj* self,
                              uintptr_t event_id,
                              PikaObj* event_handler) {
    Args buffs = {0};
    char* event_name =
        strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, "%d", event_id);
    obj_newDirectObj(self, event_name, New_TinyObj);
    PikaObj* event_item = obj_getObj(self, event_name);
    obj_setPtr(event_item, "handler", event_handler);
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
    eventListener_registEvent(pika_lv_event_listener_g, (uintptr_t)lv_obj, self);
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

void pika_lvgl_lv_obj_del_(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    Args buffs = {0};
    char* event_name =
        strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, "%d", (uintptr_t)lv_obj);
    obj_removeArg(pika_lv_event_listener_g, event_name);
    strsDeinit(&buffs);
    lv_obj_del(lv_obj);
}


/*
"""
void lv_obj_scroll_to_view(struct _lv_obj_t * obj, lv_anim_enable_t anim_en);
void lv_obj_scroll_to_view_recursive(struct _lv_obj_t * obj, lv_anim_enable_t anim_en);
void lv_obj_set_style_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_min_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_max_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_min_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_max_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_x(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_y(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_align(struct _lv_obj_t * obj, lv_align_t value, lv_style_selector_t selector);
void lv_obj_set_style_transform_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_transform_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_translate_x(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_translate_y(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_transform_zoom(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_transform_angle(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_transform_pivot_x(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_transform_pivot_y(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_pad_top(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_pad_bottom(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_pad_left(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_pad_right(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_pad_row(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_pad_column(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_grad_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_grad_dir(struct _lv_obj_t * obj, lv_grad_dir_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_main_stop(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_grad_stop(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_grad(struct _lv_obj_t * obj, const lv_grad_dsc_t * value, lv_style_selector_t selector);
void lv_obj_set_style_bg_dither_mode(struct _lv_obj_t * obj, lv_dither_mode_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_img_src(struct _lv_obj_t * obj, const void * value, lv_style_selector_t selector);
void lv_obj_set_style_bg_img_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_img_recolor(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_img_recolor_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_img_tiled(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector);
void lv_obj_set_style_border_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_border_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_border_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_border_side(struct _lv_obj_t * obj, lv_border_side_t value, lv_style_selector_t selector);
void lv_obj_set_style_border_post(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector);
void lv_obj_set_style_outline_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_outline_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_outline_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_outline_pad(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_shadow_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_shadow_ofs_x(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_shadow_ofs_y(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_shadow_spread(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_shadow_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_shadow_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_img_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_img_recolor(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_img_recolor_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_line_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_line_dash_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_line_dash_gap(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_line_rounded(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector);
void lv_obj_set_style_line_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_line_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_arc_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_arc_rounded(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector);
void lv_obj_set_style_arc_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_arc_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_arc_img_src(struct _lv_obj_t * obj, const void * value, lv_style_selector_t selector);
void lv_obj_set_style_text_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_text_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_text_font(struct _lv_obj_t * obj, const lv_font_t * value, lv_style_selector_t selector);
void lv_obj_set_style_text_letter_space(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_text_line_space(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_text_decor(struct _lv_obj_t * obj, lv_text_decor_t value, lv_style_selector_t selector);
void lv_obj_set_style_text_align(struct _lv_obj_t * obj, lv_text_align_t value, lv_style_selector_t selector);
void lv_obj_set_style_radius(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_clip_corner(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector);
void lv_obj_set_style_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_color_filter_dsc(struct _lv_obj_t * obj, const lv_color_filter_dsc_t * value,
                                       lv_style_selector_t selector);
void lv_obj_set_style_color_filter_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_anim(struct _lv_obj_t * obj, const lv_anim_t * value, lv_style_selector_t selector);
void lv_obj_set_style_anim_time(struct _lv_obj_t * obj, uint32_t value, lv_style_selector_t selector);
void lv_obj_set_style_anim_speed(struct _lv_obj_t * obj, uint32_t value, lv_style_selector_t selector);
void lv_obj_set_style_transition(struct _lv_obj_t * obj, const lv_style_transition_dsc_t * value,
                                 lv_style_selector_t selector);
void lv_obj_set_style_blend_mode(struct _lv_obj_t * obj, lv_blend_mode_t value, lv_style_selector_t selector);
void lv_obj_set_style_layout(struct _lv_obj_t * obj, uint16_t value, lv_style_selector_t selector);
void lv_obj_set_style_base_dir(struct _lv_obj_t * obj, lv_base_dir_t value, lv_style_selector_t selector);
"""

class lv_obj:
    def __init__(self, parent: lv_obj): ...
    def add_state(self, state: int): ...
    def add_flag(self, flag: int): ...
    def clear_flag(self, flag: int): ...
    def add_event_cb(self, event_cb: any, filter: int, user_data: pointer): ...
    def add_style(self, style: style_t, selector: int): ...
    def set_pos(self, x: int, y: int): ...
    def set_x(self, x: int): ...
    def set_y(self, y: int): ...
    def set_size(self, w: int, h: int): ...
    def refr_size(self) -> int: ...
    def set_width(self, w: int): ...
    def set_height(self, h: int): ...
    def set_content_width(self, w: int): ...
    def set_content_height(self, h: int): ...
    def set_layout(self, layout: int): ...
    def is_layout_positioned(self) -> int: ...
    def mark_layout_as_dirty(self): ...
    def update_layout(self): ...
    def set_align(self, align: int): ...
    def align(self, align: int, x_ofs: int, y_ofs: int): ...
    def align_to(self, base: lv_obj, align: int, x_ofs: int, y_ofs: int): ...
    def center(self): ...
    # def get_coords(self, coords: lv_area_t): ...
    def get_x(self) -> int: ...
    def get_x2(self) -> int: ...
    def get_y(self) -> int: ...
    def get_y2(self) -> int: ...
    def get_x_aligned(self) -> int: ...
    def get_y_aligned(self) -> int: ...
    def get_width(self) -> int: ...
    def get_height(self) -> int: ...
    def get_content_width(self) -> int: ...
    def get_content_height(self) -> int: ...
    # def get_content_coords(self, area: lv_area_t): ...
    def get_self_width(self) -> int: ...
    def get_self_height(self) -> int: ...
    def refresh_self_size(self) -> int: ...
    def refr_pos(self): ...
    def move_to(self, x: int, y: int): ...
    def move_children_by(self, x_diff: int, y_diff: int,
                         ignore_floating: int): ...

    def transform_point(self, p: point_t, recursive: int, inv: int): ...
    # def get_transformed_area(self, area: lv_area_t, recursive: int, inv: int): ...
    # def invalidate_area(self, area: lv_area_t): ...
    def invalidate(self): ...
    # def area_is_visible(self, area: lv_area_t) -> int: ...
    def is_visible(self) -> int: ...
    def set_ext_click_area(self, size: int): ...
    def set_style_size(self, value: int, selector: int): ...
    # def get_click_area(self, area: lv_area_t): ...
    def hit_test(self, point: point_t) -> int: ...
    def set_flex_flow(self, flow: int): ...
    def set_flex_grow(self, value: int): ...
    def set_flex_align(self, main_place: int,
                       cross_place: int, align: int): ...

    def set_id(self, id: str): ...
    def get_id(self) -> str: ...
    def clean(self): ...
    def del_(self): ...
    def scroll_to_view(self, anim_en: int): ...
    def scroll_to_view_recursive(self, anim_en: int): ...
    def set_style_width(self, value: int, selector: int): ...
    def set_style_min_width(self, value: int, selector: int): ...
    def set_style_max_width(self, value: int, selector: int): ...
    def set_style_height(self, value: int, selector: int): ...
    def set_style_min_height(self, value: int, selector: int): ...
    def set_style_max_height(self, value: int, selector: int): ...
    def set_style_x(self, value: int, selector: int): ...
    def set_style_y(self, value: int, selector: int): ...
    def set_style_align(self, value: int, selector: int): ...
    def set_style_transform_width(self, value: int, selector: int): ...
    def set_style_transform_height(self, value: int, selector: int): ...
    def set_style_translate_x(self, value: int, selector: int): ...
    def set_style_translate_y(self, value: int, selector: int): ... 
    def set_style_transform_zoom(self, value: int, selector: int): ...
    def set_style_transform_angle(self, value: int, selector: int): ...
    def set_style_transform_pivot_x(self, value: int, selector: int): ...
    def set_style_transform_pivot_y(self, value: int, selector: int): ...
    def set_style_pad_top(self, value: int, selector: int): ...
    def set_style_pad_bottom(self, value: int, selector: int): ...
    def set_style_pad_left(self, value: int, selector: int): ...
    def set_style_pad_right(self, value: int, selector: int): ...
    def set_style_pad_row(self, value: int, selector: int): ...
    def set_style_pad_column(self, value: int, selector: int): ...
    def set_style_bg_color(self, value: lv_color_t, selector: int): ...
    def set_style_bg_opa(self, value: int, selector: int): ...
    def set_style_bg_grad_color(self, value: lv_color_t, selector: int): ...
    def set_style_bg_grad_dir(self, value: int, selector: int): ...
    def set_style_bg_main_stop(self, value: int, selector: int): ...
    def set_style_bg_grad_stop(self, value: int, selector: int): ...
    # def set_style_bg_grad(self, value: lv_grad_dsc_t, selector: int): ...
    def set_style_bg_dither_mode(self, value: int, selector: int): ...
    def set_style_bg_img_src(self, value: bytes, selector: int): ...
    def set_style_bg_img_opa(self, value: int, selector: int): ...
    def set_style_bg_img_recolor(self, value: lv_color_t, selector: int): ...
    def set_style_bg_img_recolor_opa(self, value: int, selector: int): ...
    def set_style_bg_img_tiled(self, value: int, selector: int): ...
    def set_style_border_color(self, value: lv_color_t, selector: int): ...
    def set_style_border_opa(self, value: int, selector: int): ...
    def set_style_border_width(self, value: int, selector: int): ...
    def set_style_border_side(self, value: int, selector: int): ...
    def set_style_border_post(self, value: int, selector: int): ...
    def set_style_outline_width(self, value: int, selector: int): ...
    def set_style_outline_color(self, value: lv_color_t, selector: int): ...
    def set_style_outline_opa(self, value: int, selector: int): ...
    def set_style_outline_pad(self, value: int, selector: int): ...
    def set_style_shadow_width(self, value: int, selector: int): ...
    def set_style_shadow_ofs_x(self, value: int, selector: int): ...
    def set_style_shadow_ofs_y(self, value: int, selector: int): ...
    def set_style_shadow_spread(self, value: int, selector: int): ...
    def set_style_shadow_color(self, value: lv_color_t, selector: int): ...
    def set_style_shadow_opa(self, value: int, selector: int): ...
    def set_style_img_opa(self, value: int, selector: int): ...
    def set_style_img_recolor(self, value: lv_color_t, selector: int): ...
    def set_style_img_recolor_opa(self, value: int, selector: int): ...
    def set_style_line_width(self, value: int, selector: int): ...
    def set_style_line_dash_width(self, value: int, selector: int): ...
    def set_style_line_dash_gap(self, value: int, selector: int): ...
    def set_style_line_rounded(self, value: int, selector: int): ...
    def set_style_line_color(self, value: lv_color_t, selector: int): ...
    def set_style_line_opa(self, value: int, selector: int): ...
    def set_style_arc_width(self, value: int, selector: int): ...   
    def set_style_arc_rounded(self, value: int, selector: int): ...
    def set_style_arc_color(self, value: lv_color_t, selector: int): ...
    def set_style_arc_opa(self, value: int, selector: int): ...
    def set_style_arc_img_src(self, value: bytes, selector: int): ...
    def set_style_text_color(self, value: lv_color_t, selector: int): ...
    def set_style_text_opa(self, value: int, selector: int): ...
    # def set_style_text_font(self, value: lv_font_t, selector: int): ...
    def set_style_text_letter_space(self, value: int, selector: int): ...
    def set_style_text_line_space(self, value: int, selector: int): ...
    def set_style_text_decor(self, value: int, selector: int): ...
    def set_style_text_align(self, value: int, selector: int): ...
    def set_style_radius(self, value: int, selector: int): ...
    def set_style_clip_corner(self, value: int, selector: int): ...
    def set_style_opa(self, value: int, selector: int): ...
    # def set_style_color_filter_dsc(self, value: lv_color_filter_dsc_t, selector: int): ...
    def set_style_color_filter_opa(self, value: int, selector: int): ...
    def set_style_anim(self, value: int, selector: int): ...
    def set_style_anim_time(self, value: int, selector: int): ...
    def set_style_anim_speed(self, value: int, selector: int): ...
    # def set_style_transition(self, value: lv_style_transition_dsc_t, selector: int): ...
    def set_style_blend_mode(self, value: int, selector: int): ...
    def set_style_layout(self, value: int, selector: int): ...
    def set_style_base_dir(self, value: int, selector: int): ...
*/

void pika_lvgl_lv_obj_scroll_to_view(PikaObj* self, int anim_en) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_scroll_to_view(lv_obj, anim_en);
}

void pika_lvgl_lv_obj_scroll_to_view_recursive(PikaObj* self, int anim_en) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_scroll_to_view_recursive(lv_obj, anim_en);
}

void pika_lvgl_lv_obj_set_style_width(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_width(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_min_width(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_min_width(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_max_width(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_max_width(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_height(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_height(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_min_height(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_min_height(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_max_height(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_max_height(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_x(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_x(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_y(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_y(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_align(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_align(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_transform_width(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_transform_width(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_transform_height(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_transform_height(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_translate_x(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_translate_x(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_translate_y(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_translate_y(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_transform_zoom(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_transform_zoom(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_transform_angle(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_transform_angle(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_transform_pivot_x(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_transform_pivot_x(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_transform_pivot_y(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_transform_pivot_y(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_pad_top(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_pad_top(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_pad_bottom(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_pad_bottom(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_pad_left(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_pad_left(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_pad_right(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_pad_right(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_pad_row(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_pad_row(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_pad_column(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_pad_column(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_bg_color(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_obj_set_style_bg_color(lv_obj, *lv_color, selector);
}

void pika_lvgl_lv_obj_set_style_bg_opa(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_bg_opa(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_bg_grad_color(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_obj_set_style_bg_grad_color(lv_obj, *lv_color, selector);
}

void pika_lvgl_lv_obj_set_style_bg_grad_dir(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_bg_grad_dir(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_bg_main_stop(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_bg_main_stop(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_bg_grad_stop(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_bg_grad_stop(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_bg_dither_mode(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_bg_dither_mode(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_bg_img_src(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_img_dsc_t* lv_img_dsc = obj_getPtr(value, "lv_img_dsc");
    lv_obj_set_style_bg_img_src(lv_obj, lv_img_dsc, selector);
}

void pika_lvgl_lv_obj_set_style_bg_img_opa(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_bg_img_opa(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_bg_img_recolor(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_obj_set_style_bg_img_recolor(lv_obj, *lv_color, selector);
}

void pika_lvgl_lv_obj_set_style_bg_img_recolor_opa(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_bg_img_recolor_opa(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_bg_img_tiled(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_bg_img_tiled(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_border_color(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_obj_set_style_border_color(lv_obj, *lv_color, selector);
}

void pika_lvgl_lv_obj_set_style_border_opa(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_border_opa(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_border_width(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_border_width(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_border_side(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_border_side(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_border_post(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_border_post(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_outline_width(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_outline_width(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_outline_color(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_obj_set_style_outline_color(lv_obj, *lv_color, selector);
}

void pika_lvgl_lv_obj_set_style_outline_opa(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_outline_opa(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_outline_pad(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_outline_pad(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_shadow_width(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_shadow_width(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_shadow_ofs_x(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_shadow_ofs_x(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_shadow_ofs_y(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_shadow_ofs_y(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_shadow_spread(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_shadow_spread(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_shadow_color(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_obj_set_style_shadow_color(lv_obj, *lv_color, selector);
}

void pika_lvgl_lv_obj_set_style_shadow_opa(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_shadow_opa(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_img_opa(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_img_opa(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_img_recolor(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_obj_set_style_img_recolor(lv_obj, *lv_color, selector);
}

void pika_lvgl_lv_obj_set_style_img_recolor_opa(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_img_recolor_opa(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_line_width(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_line_width(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_line_dash_width(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_line_dash_width(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_line_dash_gap(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_line_dash_gap(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_line_rounded(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_line_rounded(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_line_color(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_obj_set_style_line_color(lv_obj, *lv_color, selector);
}

void pika_lvgl_lv_obj_set_style_line_opa(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_line_opa(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_arc_width(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_arc_width(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_arc_rounded(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_arc_rounded(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_arc_color(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_obj_set_style_arc_color(lv_obj, *lv_color, selector);
}

void pika_lvgl_lv_obj_set_style_arc_opa(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_arc_opa(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_arc_img_src(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_img_dsc_t* lv_img_dsc = obj_getPtr(value, "lv_img_dsc");
    lv_obj_set_style_arc_img_src(lv_obj, lv_img_dsc, selector);
}

void pika_lvgl_lv_obj_set_style_text_color(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_obj_set_style_text_color(lv_obj, *lv_color, selector);
}

void pika_lvgl_lv_obj_set_style_text_opa(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_text_opa(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_text_font(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_font_t* lv_font = obj_getPtr(value, "lv_font");
    lv_obj_set_style_text_font(lv_obj, lv_font, selector);
}

void pika_lvgl_lv_obj_set_style_text_letter_space(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_text_letter_space(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_text_line_space(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_text_line_space(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_text_decor(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_text_decor(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_text_align(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_text_align(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_radius(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_radius(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_clip_corner(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_clip_corner(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_opa(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_opa(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_color_filter_dsc(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_color_filter_dsc_t* lv_color_filter_dsc = obj_getPtr(value, "lv_color_filter_dsc");
    lv_obj_set_style_color_filter_dsc(lv_obj, lv_color_filter_dsc, selector);
}

void pika_lvgl_lv_obj_set_style_color_filter_opa(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_color_filter_opa(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_anim(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_anim_t* lv_anim = obj_getPtr(value, "lv_anim");
    lv_obj_set_style_anim(lv_obj, lv_anim, selector);
}

void pika_lvgl_lv_obj_set_style_anim_time(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_anim_time(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_anim_speed(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_anim_speed(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_transition(PikaObj* self, PikaObj* value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_style_transition_dsc_t* lv_style_transition_dsc = obj_getPtr(value, "lv_style_transition_dsc");
    lv_obj_set_style_transition(lv_obj, lv_style_transition_dsc, selector);
}

void pika_lvgl_lv_obj_set_style_blend_mode(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_blend_mode(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_layout(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_layout(lv_obj, value, selector);
}

void pika_lvgl_lv_obj_set_style_base_dir(PikaObj* self, int value, int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_base_dir(lv_obj, value, selector);
}


#endif
