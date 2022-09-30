#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../../lvgl.h"
#endif

#ifdef PIKASCRIPT

#include "pika_lvgl_style_t.h"

void pika_lvgl_style_t_init(PikaObj* self) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_init(lv_style);
}

void pika_lvgl_style_t___init__(PikaObj* self) {
    lv_style_t lv_style_stack = {0};
    args_setStruct(self->list, "lv_style_struct", lv_style_stack);
    lv_style_t* lv_style = args_getStruct(self->list, "lv_style_struct");
    obj_setPtr(self, "lv_style", lv_style);
}

void pika_lvgl_style_t_set_align(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_align(lv_style, value);
}

void pika_lvgl_style_t_set_anim_speed(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_anim_speed(lv_style, value);
}

void pika_lvgl_style_t_set_anim_time(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_anim_time(lv_style, value);
}

void pika_lvgl_style_t_set_arc_color(PikaObj* self, PikaObj* value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_style_set_arc_color(lv_style, *lv_color);
}

void pika_lvgl_style_t_set_arc_img_src(PikaObj* self, uint8_t* value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_arc_img_src(lv_style, value);
}

void pika_lvgl_style_t_set_arc_opa(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_arc_opa(lv_style, value);
}

void pika_lvgl_style_t_set_arc_rounded(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_arc_rounded(lv_style, value);
}

void pika_lvgl_style_t_set_arc_width(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_arc_width(lv_style, value);
}

void pika_lvgl_style_t_set_base_dir(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_base_dir(lv_style, value);
}

void pika_lvgl_style_t_set_bg_color(PikaObj* self, PikaObj* value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_style_set_bg_color(lv_style, *lv_color);
}

void pika_lvgl_style_t_set_bg_dither_mode(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_bg_dither_mode(lv_style, value);
}

void pika_lvgl_style_t_set_bg_grad_color(PikaObj* self, PikaObj* value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_style_set_bg_grad_color(lv_style, *lv_color);
}

void pika_lvgl_style_t_set_bg_grad_dir(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_bg_grad_dir(lv_style, value);
}

void pika_lvgl_style_t_set_bg_grad_stop(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_bg_grad_stop(lv_style, value);
}

void pika_lvgl_style_t_set_bg_img_opa(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_bg_img_opa(lv_style, value);
}

void pika_lvgl_style_t_set_bg_img_recolor(PikaObj* self, PikaObj* value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_style_set_bg_img_recolor(lv_style, *lv_color);
}

void pika_lvgl_style_t_set_bg_img_recolor_opa(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_bg_img_recolor_opa(lv_style, value);
}

void pika_lvgl_style_t_set_bg_img_src(PikaObj* self, uint8_t* value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_bg_img_src(lv_style, value);
}

void pika_lvgl_style_t_set_bg_img_tiled(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_bg_img_tiled(lv_style, value);
}

void pika_lvgl_style_t_set_bg_main_stop(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_bg_main_stop(lv_style, value);
}

void pika_lvgl_style_t_set_bg_opa(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_bg_opa(lv_style, value);
}

void pika_lvgl_style_t_set_blend_mode(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_blend_mode(lv_style, value);
}

void pika_lvgl_style_t_set_border_color(PikaObj* self, PikaObj* value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_style_set_border_color(lv_style, *lv_color);
}

void pika_lvgl_style_t_set_border_opa(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_border_opa(lv_style, value);
}

void pika_lvgl_style_t_set_border_post(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_border_post(lv_style, value);
}

void pika_lvgl_style_t_set_border_side(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_border_side(lv_style, value);
}

void pika_lvgl_style_t_set_border_width(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_border_width(lv_style, value);
}

void pika_lvgl_style_t_set_clip_corner(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_clip_corner(lv_style, value);
}

void pika_lvgl_style_t_set_color_filter_opa(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_color_filter_opa(lv_style, value);
}

void pika_lvgl_style_t_set_height(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_height(lv_style, value);
}

void pika_lvgl_style_t_set_img_opa(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_img_opa(lv_style, value);
}

void pika_lvgl_style_t_set_img_recolor(PikaObj* self, PikaObj* value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_style_set_img_recolor(lv_style, *lv_color);
}

void pika_lvgl_style_t_set_img_recolor_opa(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_img_recolor_opa(lv_style, value);
}

void pika_lvgl_style_t_set_layout(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_layout(lv_style, value);
}

void pika_lvgl_style_t_set_line_color(PikaObj* self, PikaObj* value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_style_set_line_color(lv_style, *lv_color);
}

void pika_lvgl_style_t_set_line_dash_gap(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_line_dash_gap(lv_style, value);
}

void pika_lvgl_style_t_set_line_dash_width(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_line_dash_width(lv_style, value);
}

void pika_lvgl_style_t_set_line_opa(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_line_opa(lv_style, value);
}

void pika_lvgl_style_t_set_line_rounded(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_line_rounded(lv_style, value);
}

void pika_lvgl_style_t_set_line_width(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_line_width(lv_style, value);
}

void pika_lvgl_style_t_set_max_height(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_max_height(lv_style, value);
}

void pika_lvgl_style_t_set_max_width(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_max_width(lv_style, value);
}

void pika_lvgl_style_t_set_min_height(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_min_height(lv_style, value);
}

void pika_lvgl_style_t_set_min_width(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_min_width(lv_style, value);
}

void pika_lvgl_style_t_set_opa(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_opa(lv_style, value);
}

void pika_lvgl_style_t_set_outline_color(PikaObj* self, PikaObj* value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_style_set_outline_color(lv_style, *lv_color);
}

void pika_lvgl_style_t_set_outline_opa(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_outline_opa(lv_style, value);
}

void pika_lvgl_style_t_set_outline_pad(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_outline_pad(lv_style, value);
}

void pika_lvgl_style_t_set_outline_width(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_outline_width(lv_style, value);
}

void pika_lvgl_style_t_set_pad_bottom(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_pad_bottom(lv_style, value);
}

void pika_lvgl_style_t_set_pad_column(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_pad_column(lv_style, value);
}

void pika_lvgl_style_t_set_pad_left(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_pad_left(lv_style, value);
}

void pika_lvgl_style_t_set_pad_right(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_pad_right(lv_style, value);
}

void pika_lvgl_style_t_set_pad_row(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_pad_row(lv_style, value);
}

void pika_lvgl_style_t_set_pad_top(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_pad_top(lv_style, value);
}

void pika_lvgl_style_t_set_radius(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_radius(lv_style, value);
}

void pika_lvgl_style_t_set_shadow_color(PikaObj* self, PikaObj* value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_style_set_shadow_color(lv_style, *lv_color);
}

void pika_lvgl_style_t_set_shadow_ofs_x(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_shadow_ofs_x(lv_style, value);
}

void pika_lvgl_style_t_set_shadow_ofs_y(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_shadow_ofs_y(lv_style, value);
}

void pika_lvgl_style_t_set_shadow_opa(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_shadow_opa(lv_style, value);
}

void pika_lvgl_style_t_set_shadow_spread(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_shadow_spread(lv_style, value);
}

void pika_lvgl_style_t_set_shadow_width(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_shadow_width(lv_style, value);
}

void pika_lvgl_style_t_set_text_align(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_text_align(lv_style, value);
}

void pika_lvgl_style_t_set_text_color(PikaObj* self, PikaObj* value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_color_t* lv_color = obj_getPtr(value, "lv_color");
    lv_style_set_text_color(lv_style, *lv_color);
}

void pika_lvgl_style_t_set_text_decor(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_text_decor(lv_style, value);
}

void pika_lvgl_style_t_set_text_letter_space(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_text_letter_space(lv_style, value);
}

void pika_lvgl_style_t_set_text_line_space(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_text_line_space(lv_style, value);
}

void pika_lvgl_style_t_set_text_opa(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_text_opa(lv_style, value);
}

void pika_lvgl_style_t_set_transform_angle(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_transform_angle(lv_style, value);
}

void pika_lvgl_style_t_set_transform_height(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_transform_height(lv_style, value);
}

void pika_lvgl_style_t_set_transform_pivot_x(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_transform_pivot_x(lv_style, value);
}

void pika_lvgl_style_t_set_transform_pivot_y(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_transform_pivot_y(lv_style, value);
}

void pika_lvgl_style_t_set_transform_width(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_transform_width(lv_style, value);
}

void pika_lvgl_style_t_set_transform_zoom(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_transform_zoom(lv_style, value);
}

void pika_lvgl_style_t_set_translate_x(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_translate_x(lv_style, value);
}

void pika_lvgl_style_t_set_translate_y(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_translate_y(lv_style, value);
}

void pika_lvgl_style_t_set_width(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_width(lv_style, value);
}

void pika_lvgl_style_t_set_x(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_x(lv_style, value);
}

void pika_lvgl_style_t_set_y(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_y(lv_style, value);
}

int pika_lvgl_style_t_get_num_custom_props(PikaObj* self) {
    return lv_style_get_num_custom_props();
}

int pika_lvgl_style_t_prop_has_flag(PikaObj* self, int prop, int flag) {
    return lv_style_prop_has_flag(prop, flag);
}
int pika_lvgl_style_t_is_empty(PikaObj* self) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    return lv_style_is_empty(lv_style);
}
int pika_lvgl_style_t_register_prop(PikaObj* self, int flag) {
    return lv_style_register_prop(flag);
}
int pika_lvgl_style_t_remove_prop(PikaObj* self, int prop) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    return lv_style_remove_prop(lv_style, prop);
}
void pika_lvgl_style_t_reset(PikaObj* self) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_reset(lv_style);
}
void pika_lvgl_style_t_set_pad_all(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_pad_all(lv_style, value);
}
void pika_lvgl_style_t_set_pad_gap(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_pad_gap(lv_style, value);
}
void pika_lvgl_style_t_set_pad_hor(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_pad_hor(lv_style, value);
}
void pika_lvgl_style_t_set_pad_ver(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_pad_ver(lv_style, value);
}
// void pika_lvgl_style_t_set_prop_meta(PikaObj* self, int prop, int meta) {
// lv_style_t* lv_style = obj_getPtr(self, "lv_style");
// lv_style_set_prop_meta(lv_style, prop, meta);
//}
void pika_lvgl_style_t_set_size(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_size(lv_style, value);
}

void pika_lvgl_style_t_set_flex_cross_place(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_flex_cross_place(lv_style, value);
}

void pika_lvgl_style_t_set_flex_flow(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_flex_flow(lv_style, value);
}

void pika_lvgl_style_t_set_flex_grow(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_flex_grow(lv_style, value);
}

void pika_lvgl_style_t_set_flex_main_place(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_flex_main_place(lv_style, value);
}

void pika_lvgl_style_t_set_flex_track_place(PikaObj* self, int value) {
    lv_style_t* lv_style = obj_getPtr(self, "lv_style");
    lv_style_set_flex_track_place(lv_style, value);
}

#endif
