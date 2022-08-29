#include "PikaObj.h"

#undef __WEAK
#define __WEAK      __attribute__((weak))

__WEAK
void pika_lvgl_point_t___init__(PikaObj* self) {
}


__WEAK
void pika_lvgl_lv_timer_t_set_period(PikaObj* self, int period) {
}

__WEAK
void pika_lvgl_lv_timer_t_set_cb(PikaObj* self, Arg* cb) {
}

__WEAK
void pika_lvgl_lv_timer_t__del(PikaObj* self) {
}

__WEAK
void pika_lvgl_arc___init__(PikaObj* self, PikaObj* parent) {
}

__WEAK
void pika_lvgl_arc_set_end_angle(PikaObj* self, int angle) {
}
__WEAK
void pika_lvgl_arc_set_bg_angles(PikaObj* self, int start, int end) {
}

__WEAK
void pika_lvgl_arc_set_angles(PikaObj* self, int start, int end) {
}

__WEAK
int pika_lvgl_arc_get_angle_end(PikaObj *self){
    return -1;
}

__WEAK
int pika_lvgl_arc_get_angle_start(PikaObj *self){
    return -1;
}

__WEAK
int pika_lvgl_arc_get_bg_angle_end(PikaObj *self){
    return -1;
}

__WEAK
int pika_lvgl_arc_get_bg_angle_start(PikaObj *self){
    return -1;
}

__WEAK
int pika_lvgl_arc_get_max_value(PikaObj *self){
    return -1;
}

__WEAK
int pika_lvgl_arc_get_min_value(PikaObj *self){
    return -1;
}

__WEAK
int pika_lvgl_arc_get_mode(PikaObj *self){
    return -1;
}
// int pika_lvgl_arc_get_rotation(PikaObj *self){
//     lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
//     return lv_arc_get_rotation(lv_obj);
// }
__WEAK
int pika_lvgl_arc_get_value(PikaObj *self){
    return -1;
}
__WEAK
void pika_lvgl_arc_set_mode(PikaObj *self, int mode){
}
__WEAK
void pika_lvgl_arc_set_range(PikaObj *self, int min, int max){
}
__WEAK void pika_lvgl_arc_set_rotation(PikaObj *self, int rotation){
}
__WEAK void pika_lvgl_arc_set_start_angle(PikaObj *self, int start){
}
__WEAK void pika_lvgl_arc_set_value(PikaObj *self, int value){
}
__WEAK void pika_lvgl_arc_set_bg_end_angle(PikaObj *self, int angle){
}
__WEAK void pika_lvgl_arc_set_bg_start_angle(PikaObj *self, int start){
}

__WEAK void pika_lvgl_arc_set_change_rate(PikaObj *self, int rate){
}

__WEAK void pika_lvgl_bar___init__(PikaObj* self, PikaObj* parent) {
}

__WEAK void pika_lvgl_bar_set_value(PikaObj* self, int value, int anim) {
}

__WEAK
int pika_lvgl_bar_get_max_value(PikaObj *self){
    return -1;
}
__WEAK
int pika_lvgl_bar_get_min_value(PikaObj *self){
    return -1;
}
__WEAK
int pika_lvgl_bar_get_mode(PikaObj *self){
    return -1;
}
__WEAK
int pika_lvgl_bar_get_start_value(PikaObj *self){
    return -1;
}
__WEAK
int pika_lvgl_bar_get_value(PikaObj *self){
    return -1;
}
__WEAK void pika_lvgl_bar_set_mode(PikaObj *self, int mode){
}
__WEAK void pika_lvgl_bar_set_range(PikaObj *self, int min, int max){
}
__WEAK void pika_lvgl_bar_set_start_value(PikaObj *self, int start_value, int anim){
}

__WEAK void pika_lvgl_btn___init__(PikaObj* self, PikaObj* parent) {
}

__WEAK void pika_lvgl_checkbox___init__(PikaObj* self, PikaObj* parent) {
}

__WEAK void pika_lvgl_checkbox_set_text(PikaObj* self, char* txt) {
}

__WEAK void pika_lvgl_checkbox_set_text_static(PikaObj *self, char* txt){
}

__WEAK
char* pika_lvgl_checkbox_get_text(PikaObj *self){
    return NULL;
}

__WEAK void pika_lvgl_dropdown___init__(PikaObj* self, PikaObj* parent) {;
}

__WEAK void pika_lvgl_dropdown_set_options(PikaObj* self, char* options) {
}

__WEAK void pika_lvgl_dropdown_add_option(PikaObj *self, char* options, int pos){
}
__WEAK void pika_lvgl_dropdown_clear_options(PikaObj *self){
}
__WEAK void pika_lvgl_dropdown_close(PikaObj *self){
}
__WEAK
int pika_lvgl_dropdown_get_dir(PikaObj *self){
    return -1;
}
// PikaObj* pika_lvgl_dropdown_get_list(PikaObj *self){
//     lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
//     return obj_getObj(lv_dropdown_get_list(lv_obj));
// }
__WEAK
int pika_lvgl_dropdown_get_option_cnt(PikaObj *self){
    return -1;
}
__WEAK
int pika_lvgl_dropdown_get_option_index(PikaObj *self, char* txt){
    return -1;
}
__WEAK
char* pika_lvgl_dropdown_get_options(PikaObj *self){
    return NULL;
}
__WEAK
int pika_lvgl_dropdown_get_selected(PikaObj *self){
    return -1;
}
__WEAK
int pika_lvgl_dropdown_get_selected_highlight(PikaObj *self){
    return -1;
}
__WEAK
char* pika_lvgl_dropdown_get_selected_str(PikaObj *self){
    return NULL;
}
__WEAK
int pika_lvgl_dropdown_get_symbol(PikaObj *self){
    return -1;
}

__WEAK
char* pika_lvgl_dropdown_get_text(PikaObj *self){
    return NULL;
}
__WEAK
int pika_lvgl_dropdown_is_open(PikaObj *self){
    return -1;
}
__WEAK void pika_lvgl_dropdown_open(PikaObj *self){
}
__WEAK void pika_lvgl_dropdown_set_dir(PikaObj *self, int dir){
}
__WEAK void pika_lvgl_dropdown_set_selected(PikaObj *self, int sel_opt){
}
__WEAK void pika_lvgl_dropdown_set_selected_hightlight(PikaObj *self, int en){
}
__WEAK void pika_lvgl_dropdown_set_symbol(PikaObj *self, char* symbol){
}
__WEAK void pika_lvgl_dropdown_set_text(PikaObj *self, char* txt){
}

__WEAK void pika_lvgl_label___init__(PikaObj* self, PikaObj* parent) {
}

__WEAK void pika_lvgl_label_set_long_mode(PikaObj* self, int mode) {
}

__WEAK void pika_lvgl_label_set_recolor(PikaObj* self, int en) {
}

__WEAK void pika_lvgl_label_set_text(PikaObj* self, char* txt) {
}

__WEAK void pika_lvgl_label_set_style_text_align(PikaObj* self,
                                          int value,
                                          int selector) {
}

__WEAK void pika_lvgl_roller___init__(PikaObj* self, PikaObj* parent) {
}

__WEAK void pika_lvgl_roller_set_options(PikaObj* self, char* options, int mode) {
}

__WEAK void pika_lvgl_roller_set_visible_row_count(PikaObj* self, int row_cnt) {
}

__WEAK void pika_lvgl_slider___init__(PikaObj* self, PikaObj* parent) {
}

__WEAK void pika_lvgl_switch___init__(PikaObj* self, PikaObj* parent) {
}

__WEAK void pika_lvgl_table___init__(PikaObj* self, PikaObj* parent) {
}

__WEAK void pika_lvgl_table_set_cell_value(PikaObj* self,
                                    int row,
                                    int col,
                                    char* txt) {
}

__WEAK void pika_lvgl_textarea___init__(PikaObj* self, PikaObj* parent) {
}

__WEAK void pika_lvgl_textarea_set_one_line(PikaObj* self, int en) {
}


__WEAK void pika_lvgl_STATE___init__(PikaObj* self) {
}

__WEAK void pika_lvgl_ANIM___init__(PikaObj* self) {
}

__WEAK void pika_lvgl_ALIGN___init__(PikaObj* self) {
}

__WEAK void pika_lvgl_EVENT___init__(PikaObj* self) {
}

__WEAK void pika_lvgl_OPA___init__(PikaObj* self) {
}

__WEAK void pika_lvgl_PALETTE___init__(PikaObj* self) {
}

__WEAK
PikaObj* pika_lvgl_scr_act(PikaObj* self) {
    return NULL;
}

__WEAK void pika_lvgl___init__(PikaObj* self) {
}

__WEAK
PikaObj* pika_lvgl_obj(PikaObj* self, PikaObj* parent) {
    return NULL;
}

__WEAK
PikaObj* pika_lvgl_palette_lighten(PikaObj *self, int p, int lvl){
    return NULL;
}

__WEAK
PikaObj* pika_lvgl_palette_main(PikaObj* self, int p) {
    return NULL;
}

__WEAK
PikaObj* pika_lvgl_indev_get_act(PikaObj *self){
    return NULL;
}

__WEAK
PikaObj* pika_lvgl_timer_create_basic(PikaObj *self){
    return NULL;
}

__WEAK
void pika_lvgl_indev_t_get_vect(PikaObj* self, PikaObj* point) {
}

__WEAK
int pika_lvgl_lv_event_get_code(PikaObj *self){
    return -1;
}

__WEAK
PikaObj* pika_lvgl_lv_event_get_target(PikaObj *self){
    return NULL;
}


__WEAK
void pika_lvgl_lv_obj_center(PikaObj* self) {
}

__WEAK
void pika_lvgl_lv_obj_set_size(PikaObj* self, int size_x, int size_y) {
}

__WEAK
void pika_lvgl_lv_obj_align(PikaObj* self, int align, int x_ofs, int y_ofs) {
}

__WEAK
void pika_lvgl_lv_obj_set_hight(PikaObj* self, int h) {
}
__WEAK
void pika_lvgl_lv_obj_update_layout(PikaObj* self) {
}

__WEAK
void pika_lvgl_lv_obj_set_width(PikaObj* self, int w) {
}

__WEAK
void pika_lvgl_lv_obj_add_state(PikaObj* self, int state) {
}

__WEAK
PikaObj* eventLisener_getHandler(PikaObj* self, uintptr_t event_id) {
    return NULL;
}

__WEAK
void eventLicener_registEvent(PikaObj* self,
                              uintptr_t event_id,
                              PikaObj* event_handler) {
}

__WEAK
void pika_lvgl_lv_obj_add_event_cb(PikaObj* self,
                                   Arg* event_cb,
                                   int filter,
                                   void* user_data) {
}

__WEAK
void pika_lvgl_lv_obj_add_style(PikaObj *self, PikaObj* style, int selector){
}

__WEAK
int pika_lvgl_lv_obj_get_x(PikaObj *self){
    return -1;
}

__WEAK
int pika_lvgl_lv_obj_get_y(PikaObj *self){
    return -1;
}

__WEAK
void pika_lvgl_lv_obj_set_pos(PikaObj *self, int x, int y){
}

__WEAK
void pika_lvgl_style_t_init(PikaObj* self) {
}

__WEAK
void pika_lvgl_style_t_set_bg_color(PikaObj* self, PikaObj* color) {
}

__WEAK
void pika_lvgl_style_t_set_bg_opa(PikaObj* self, int opa) {

}

__WEAK
void pika_lvgl_style_t_set_outline_color(PikaObj* self, PikaObj* color) {

}

__WEAK
void pika_lvgl_style_t_set_outline_pad(PikaObj* self, int pad) {
}

__WEAK
void pika_lvgl_style_t_set_outline_width(PikaObj* self, int w) {
}

__WEAK
void pika_lvgl_style_t_set_radius(PikaObj* self, int radius) {
}

__WEAK
void pika_lvgl_style_t___init__(PikaObj* self) {
}

__WEAK
void pika_lvgl_style_t_set_shadow_color(PikaObj *self, PikaObj* color){
}

__WEAK
void pika_lvgl_style_t_set_shadow_spread(PikaObj *self, int s){
}

__WEAK
void pika_lvgl_style_t_set_shadow_width(PikaObj *self, int w){
}

