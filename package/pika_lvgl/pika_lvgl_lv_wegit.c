#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../../lvgl.h"
#endif

#ifdef PIKASCRIPT
#include "BaseObj.h"
#include "PikaStdData_Dict.h"
#include "pika_lvgl.h"
#include "pika_lvgl_CHART_AXIS.h"
#include "pika_lvgl_CHART_DRAW_PART.h"
#include "pika_lvgl_CHART_TYPE.h"
#include "pika_lvgl_CHART_UPDATE_MODE.h"
#include "pika_lvgl_KEYBOARD_MODE.h"
#include "pika_lvgl_arc.h"
#include "pika_lvgl_bar.h"
#include "pika_lvgl_btn.h"
#include "pika_lvgl_cf_t.h"
#include "pika_lvgl_chart.h"
#include "pika_lvgl_chart_cursor_t.h"
#include "pika_lvgl_chart_series_t.h"
#include "pika_lvgl_checkbox.h"
#include "pika_lvgl_coord_t.h"
#include "pika_lvgl_dropdown.h"
#include "pika_lvgl_img.h"
#include "pika_lvgl_img_dsc_t.h"
#include "pika_lvgl_label.h"
#include "pika_lvgl_lv_obj.h"
#include "pika_lvgl_meter.h"
#include "pika_lvgl_meter_indicator_t.h"
#include "pika_lvgl_meter_scale_t.h"
#include "pika_lvgl_point_t.h"
#include "pika_lvgl_roller.h"
#include "pika_lvgl_slider.h"
#include "pika_lvgl_switch.h"
#include "pika_lvgl_table.h"
#include "pika_lvgl_textarea.h"
#include "pika_lvgl_common.h"

void pika_lvgl_arc___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = PIKA_LV_OBJ(parent);
    lv_obj_t* lv_obj = lv_arc_create(lv_parent);
    PIKA_LV_OBJ_SET(self, lv_obj);
    obj_setInt(self, "MODE_NORMAL", LV_ARC_MODE_NORMAL);
    obj_setInt(self, "MODE_SYMMETRICAL", LV_ARC_MODE_SYMMETRICAL);
    obj_setInt(self, "MODE_REVERSE", LV_ARC_MODE_REVERSE);
}

void pika_lvgl_arc_set_end_angle(PikaObj* self, int angle) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_arc_set_end_angle(lv_obj, angle);
}

void pika_lvgl_arc_set_bg_angles(PikaObj* self, int start, int end) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_arc_set_bg_angles(lv_obj, start, end);
}

void pika_lvgl_arc_set_angles(PikaObj* self, int start, int end) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_arc_set_angles(lv_obj, start, end);
}

int pika_lvgl_arc_get_angle_end(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_arc_get_angle_end(lv_obj);
}
int pika_lvgl_arc_get_angle_start(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_arc_get_angle_start(lv_obj);
}
int pika_lvgl_arc_get_bg_angle_end(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_arc_get_bg_angle_end(lv_obj);
}
int pika_lvgl_arc_get_bg_angle_start(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_arc_get_bg_angle_start(lv_obj);
}
int pika_lvgl_arc_get_max_value(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_arc_get_max_value(lv_obj);
}
int pika_lvgl_arc_get_min_value(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_arc_get_min_value(lv_obj);
}
int pika_lvgl_arc_get_mode(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_arc_get_mode(lv_obj);
}
// int pika_lvgl_arc_get_rotation(PikaObj *self){
//     lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
//     return lv_arc_get_rotation(lv_obj);
// }
int pika_lvgl_arc_get_value(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_arc_get_value(lv_obj);
}
void pika_lvgl_arc_set_mode(PikaObj* self, int mode) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_arc_set_mode(lv_obj, mode);
}
void pika_lvgl_arc_set_range(PikaObj* self, int min, int max) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_arc_set_range(lv_obj, min, max);
}
void pika_lvgl_arc_set_rotation(PikaObj* self, int rotation) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_arc_set_rotation(lv_obj, rotation);
}
void pika_lvgl_arc_set_start_angle(PikaObj* self, int start) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_arc_set_start_angle(lv_obj, start);
}
void pika_lvgl_arc_set_value(PikaObj* self, int value) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_arc_set_value(lv_obj, value);
}
void pika_lvgl_arc_set_bg_end_angle(PikaObj* self, int angle) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_arc_set_bg_end_angle(lv_obj, angle);
}
void pika_lvgl_arc_set_bg_start_angle(PikaObj* self, int start) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_arc_set_bg_start_angle(lv_obj, start);
}

void pika_lvgl_arc_set_change_rate(PikaObj* self, int rate) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_arc_set_change_rate(lv_obj, rate);
}

void pika_lvgl_bar___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = PIKA_LV_OBJ(parent);
    lv_obj_t* lv_obj = lv_bar_create(lv_parent);
    PIKA_LV_OBJ_SET(self, lv_obj);
}

void pika_lvgl_bar_set_value(PikaObj* self, int value, int anim) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_bar_set_value(lv_obj, value, anim);
}

int pika_lvgl_bar_get_max_value(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_bar_get_max_value(lv_obj);
}
int pika_lvgl_bar_get_min_value(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_bar_get_min_value(lv_obj);
}
int pika_lvgl_bar_get_mode(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_bar_get_mode(lv_obj);
}
int pika_lvgl_bar_get_start_value(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_bar_get_start_value(lv_obj);
}
int pika_lvgl_bar_get_value(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_bar_get_value(lv_obj);
}
void pika_lvgl_bar_set_mode(PikaObj* self, int mode) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_bar_set_mode(lv_obj, mode);
}
void pika_lvgl_bar_set_range(PikaObj* self, int min, int max) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_bar_set_range(lv_obj, min, max);
}
void pika_lvgl_bar_set_start_value(PikaObj* self, int start_value, int anim) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_bar_set_start_value(lv_obj, start_value, anim);
}

void pika_lvgl_btn___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = PIKA_LV_OBJ(parent);
    lv_obj_t* lv_obj = lv_btn_create(lv_parent);
    PIKA_LV_OBJ_SET(self, lv_obj);
}

void pika_lvgl_checkbox___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = PIKA_LV_OBJ(parent);
    lv_obj_t* lv_obj = lv_checkbox_create(lv_parent);
    PIKA_LV_OBJ_SET(self, lv_obj);
}

void pika_lvgl_checkbox_set_text(PikaObj* self, char* txt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_checkbox_set_text(lv_obj, txt);
}

void pika_lvgl_checkbox_set_text_static(PikaObj* self, char* txt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_checkbox_set_text_static(lv_obj, txt);
}

char* pika_lvgl_checkbox_get_text(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return (char*)lv_checkbox_get_text(lv_obj);
}

void pika_lvgl_dropdown___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = PIKA_LV_OBJ(parent);
    lv_obj_t* lv_obj = lv_dropdown_create(lv_parent);
    PIKA_LV_OBJ_SET(self, lv_obj);
}

void pika_lvgl_dropdown_set_options(PikaObj* self, char* options) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_dropdown_set_options(lv_obj, options);
}

void pika_lvgl_dropdown_add_option(PikaObj* self, char* options, int pos) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_dropdown_add_option(lv_obj, options, pos);
}
void pika_lvgl_dropdown_clear_options(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_dropdown_clear_options(lv_obj);
}
void pika_lvgl_dropdown_close(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_dropdown_close(lv_obj);
}
int pika_lvgl_dropdown_get_dir(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_dropdown_get_dir(lv_obj);
}
// PikaObj* pika_lvgl_dropdown_get_list(PikaObj *self){
//     lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
//     return obj_getObj(lv_dropdown_get_list(lv_obj));
// }
int pika_lvgl_dropdown_get_option_cnt(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_dropdown_get_option_cnt(lv_obj);
}
int pika_lvgl_dropdown_get_option_index(PikaObj* self, char* txt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_dropdown_get_option_index(lv_obj, txt);
}
char* pika_lvgl_dropdown_get_options(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return (char*)lv_dropdown_get_options(lv_obj);
}
int pika_lvgl_dropdown_get_selected(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_dropdown_get_selected(lv_obj);
}
int pika_lvgl_dropdown_get_selected_highlight(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_dropdown_get_selected_highlight(lv_obj);
}

char* pika_lvgl_dropdown_get_selected_str(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    obj_setBytes(self, "_buff", NULL, 128);
    char* _buff = (char*)obj_getBytes(self, "_buff");
    lv_dropdown_get_selected_str(lv_obj, _buff, 128);
    return _buff;
}
char* pika_lvgl_dropdown_get_symbol(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return (char*)lv_dropdown_get_symbol(lv_obj);
}
char* pika_lvgl_dropdown_get_text(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return (char*)lv_dropdown_get_text(lv_obj);
}
int pika_lvgl_dropdown_is_open(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_dropdown_is_open(lv_obj);
}
void pika_lvgl_dropdown_open(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_dropdown_open(lv_obj);
}
void pika_lvgl_dropdown_set_dir(PikaObj* self, int dir) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_dropdown_set_dir(lv_obj, dir);
}
void pika_lvgl_dropdown_set_selected(PikaObj* self, int sel_opt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_dropdown_set_selected(lv_obj, sel_opt);
}
void pika_lvgl_dropdown_set_selected_hightlight(PikaObj* self, int en) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_dropdown_set_selected_highlight(lv_obj, en);
}
void pika_lvgl_dropdown_set_symbol(PikaObj* self, char* symbol) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_dropdown_set_symbol(lv_obj, symbol);
}
void pika_lvgl_dropdown_set_text(PikaObj* self, char* txt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_dropdown_set_text(lv_obj, txt);
}

/*
class label(lv_obj):
    def __init__(self, parent: lv_obj): ...
    def set_text(self, txt: str): ...
#    def set_text_fmt(self, fmt: str, *args): ...
    def set_text_static(self, txt: str): ...
    def set_long_mode(self, long_mode: int): ...
    def set_recolor(self, en: int): ...
    def set_text_sel_start(self, index: int): ...
    def set_text_sel_end(self, index: int): ...
    def get_text(self) -> str: ...
    def get_long_mode(self) -> int: ...
    def get_recolor(self) -> int: ...
    def get_letter_pos(self, char_id: int) -> point_t: ...
    def get_letter_on(self, pos_in: point_t) -> int: ...
    def is_char_under_pos(self, pos: point_t) -> int: ...
    def get_text_selection_start(self) -> int: ...
    def get_text_selection_end(self) -> int: ...
    def ins_text(self, pos: int, txt: str): ...
    def cut_text(self, pos: int, cnt: int): ...
*/

void pika_lvgl_label___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = PIKA_LV_OBJ(parent);
    lv_obj_t* lv_obj = lv_label_create(lv_parent);
    PIKA_LV_OBJ_SET(self, lv_obj);
}

void pika_lvgl_label_set_text(PikaObj* self, char* txt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_label_set_text(lv_obj, txt);
}

void pika_lvgl_label_set_text_static(PikaObj* self, char* txt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_label_set_text_static(lv_obj, txt);
}

void pika_lvgl_label_set_long_mode(PikaObj* self, int long_mode) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_label_set_long_mode(lv_obj, long_mode);
}

void pika_lvgl_label_set_recolor(PikaObj* self, int en) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_label_set_recolor(lv_obj, en);
}

void pika_lvgl_label_set_text_sel_start(PikaObj* self, int index) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_label_set_text_sel_start(lv_obj, index);
}

void pika_lvgl_label_set_text_sel_end(PikaObj* self, int index) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_label_set_text_sel_end(lv_obj, index);
}

char* pika_lvgl_label_get_text(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return (char*)lv_label_get_text(lv_obj);
}

int pika_lvgl_label_get_long_mode(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_label_get_long_mode(lv_obj);
}

int pika_lvgl_label_get_recolor(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_label_get_recolor(lv_obj);
}

PikaObj* pika_lvgl_label_get_letter_pos(PikaObj* self, int char_id) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_point_t);
    lv_point_t lv_point = {0};
    lv_label_get_letter_pos(lv_obj, char_id, &lv_point);
    obj_setStruct(new_obj, "lv_point_struct", lv_point);
    obj_setPtr(new_obj, "lv_point", obj_getStruct(new_obj, "lv_point_struct"));
    return new_obj;
}

int pika_lvgl_label_get_letter_on(PikaObj* self, PikaObj* pos_in) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_point_t* lv_pos_in = obj_getPtr(pos_in, "lv_point_t");
    return lv_label_get_letter_on(lv_obj, lv_pos_in);
}

int pika_lvgl_label_is_char_under_pos(PikaObj* self, PikaObj* pos) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_point_t* lv_pos = obj_getPtr(pos, "lv_point_t");
    return lv_label_is_char_under_pos(lv_obj, lv_pos);
}

int pika_lvgl_label_get_text_selection_start(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_label_get_text_selection_start(lv_obj);
}

int pika_lvgl_label_get_text_selection_end(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_label_get_text_selection_end(lv_obj);
}

void pika_lvgl_label_ins_text(PikaObj* self, int pos, char* txt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_label_ins_text(lv_obj, pos, txt);
}

void pika_lvgl_label_cut_text(PikaObj* self, int pos, int cnt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_label_cut_text(lv_obj, pos, cnt);
}

void pika_lvgl_roller___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = PIKA_LV_OBJ(parent);
    lv_obj_t* lv_obj = lv_roller_create(lv_parent);
    PIKA_LV_OBJ_SET(self, lv_obj);
}

void pika_lvgl_roller_set_options(PikaObj* self, char* options, int mode) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_roller_set_options(lv_obj, options, mode);
}

void pika_lvgl_roller_set_visible_row_count(PikaObj* self, int row_cnt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_roller_set_visible_row_count(lv_obj, row_cnt);
}

void pika_lvgl_slider___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = PIKA_LV_OBJ(parent);
    lv_obj_t* lv_obj = lv_slider_create(lv_parent);
    PIKA_LV_OBJ_SET(self, lv_obj);
}

void pika_lvgl_switch___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = PIKA_LV_OBJ(parent);
    lv_obj_t* lv_obj = lv_switch_create(lv_parent);
    PIKA_LV_OBJ_SET(self, lv_obj);
}

void pika_lvgl_table___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = PIKA_LV_OBJ(parent);
    lv_obj_t* lv_obj = lv_table_create(lv_parent);
    PIKA_LV_OBJ_SET(self, lv_obj);
}

void pika_lvgl_table_set_cell_value(PikaObj* self,
                                    int row,
                                    int col,
                                    char* txt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_table_set_cell_value(lv_obj, row, col, txt);
}

/*
class textarea(lv_obj):
    def __init__(self, parent: lv_obj): ...
    def add_char(self, c: int): ...
    def add_text(self, txt: str): ...
    def del_char(self): ...
    def del_char_forward(self): ...
    def set_text(self, txt: str): ...
    def set_placeholder_text(self, txt: str): ...
    def set_cursor_pos(self, pos: int): ...
    def set_cursor_click_pos(self, en: int): ...
    def set_password_mode(self, en: int): ...
    def set_one_line(self, en: int): ...
    def set_accepted_chars(self, list: str): ...
    def set_max_length(self, num: int): ...
    def set_insert_replace(self, txt: str): ...
    def set_text_selection(self, en: int): ...
    def set_password_show_time(self, time: int): ...
    def set_align(self, align: int): ...
    def get_text(self) -> str: ...
    def get_placeholder_text(self) -> str: ...
    def get_label(self) -> lv_obj: ...
    def get_cursor_pos(self) -> int: ...
    def get_cursor_click_pos(self) -> int: ...
    def get_password_mode(self) -> int: ...
    def get_one_line(self) -> int: ...
    def get_accepted_chars(self) -> str: ...
    def get_max_length(self) -> int: ...
    def text_is_selected(self) -> int: ...
    def get_text_selection(self) -> int: ...
    def get_password_show_time(self) -> int: ...
    def clear_selection(self): ...
    def cursor_right(self): ...
    def cursor_left(self): ...
    def cursor_down(self): ...
    def cursor_up(self): ...
*/

void pika_lvgl_textarea___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = PIKA_LV_OBJ(parent);
    lv_obj_t* lv_obj = lv_textarea_create(lv_parent);
    PIKA_LV_OBJ_SET(self, lv_obj);
}

void pika_lvgl_textarea_add_char(PikaObj* self, int c) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_add_char(lv_obj, c);
}

void pika_lvgl_textarea_add_text(PikaObj* self, char* txt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_add_text(lv_obj, txt);
}

void pika_lvgl_textarea_del_char(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_del_char(lv_obj);
}

void pika_lvgl_textarea_del_char_forward(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_del_char_forward(lv_obj);
}

void pika_lvgl_textarea_set_text(PikaObj* self, char* txt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_set_text(lv_obj, txt);
}

void pika_lvgl_textarea_set_placeholder_text(PikaObj* self, char* txt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_set_placeholder_text(lv_obj, txt);
}

void pika_lvgl_textarea_set_cursor_pos(PikaObj* self, int pos) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_set_cursor_pos(lv_obj, pos);
}

void pika_lvgl_textarea_set_cursor_click_pos(PikaObj* self, int en) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_set_cursor_click_pos(lv_obj, en);
}

void pika_lvgl_textarea_set_password_mode(PikaObj* self, int en) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_set_password_mode(lv_obj, en);
}

void pika_lvgl_textarea_set_one_line(PikaObj* self, int en) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_set_one_line(lv_obj, en);
}

void pika_lvgl_textarea_set_accepted_chars(PikaObj* self, char* list) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_set_accepted_chars(lv_obj, list);
}

void pika_lvgl_textarea_set_max_length(PikaObj* self, int num) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_set_max_length(lv_obj, num);
}

void pika_lvgl_textarea_set_insert_replace(PikaObj* self, char* txt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_set_insert_replace(lv_obj, txt);
}

void pika_lvgl_textarea_set_text_selection(PikaObj* self, int en) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_set_text_selection(lv_obj, en);
}

void pika_lvgl_textarea_set_password_show_time(PikaObj* self, int time) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_set_password_show_time(lv_obj, time);
}

void pika_lvgl_textarea_set_align(PikaObj* self, int align) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_set_align(lv_obj, align);
}

char* pika_lvgl_textarea_get_text(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return (char*)lv_textarea_get_text(lv_obj);
}

char* pika_lvgl_textarea_get_placeholder_text(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return (char*)lv_textarea_get_placeholder_text(lv_obj);
}

PikaObj* pika_lvgl_textarea_get_label(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_label);
    PIKA_LV_OBJ_SET(new_obj, lv_textarea_get_label(lv_obj));
    return new_obj;
}

int pika_lvgl_textarea_get_cursor_pos(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_textarea_get_cursor_pos(lv_obj);
}

int pika_lvgl_textarea_get_cursor_click_pos(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_textarea_get_cursor_click_pos(lv_obj);
}

int pika_lvgl_textarea_get_password_mode(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_textarea_get_password_mode(lv_obj);
}

int pika_lvgl_textarea_get_one_line(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_textarea_get_one_line(lv_obj);
}

char* pika_lvgl_textarea_get_accepted_chars(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return (char*)lv_textarea_get_accepted_chars(lv_obj);
}

int pika_lvgl_textarea_get_max_length(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_textarea_get_max_length(lv_obj);
}

int pika_lvgl_textarea_text_is_selected(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_textarea_text_is_selected(lv_obj);
}

int pika_lvgl_textarea_get_text_selection(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_textarea_get_text_selection(lv_obj);
}

int pika_lvgl_textarea_get_password_show_time(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_textarea_get_password_show_time(lv_obj);
}

void pika_lvgl_textarea_clear_selection(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_clear_selection(lv_obj);
}

void pika_lvgl_textarea_cursor_right(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_cursor_right(lv_obj);
}

void pika_lvgl_textarea_cursor_left(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_cursor_left(lv_obj);
}

void pika_lvgl_textarea_cursor_down(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_cursor_down(lv_obj);
}

void pika_lvgl_textarea_cursor_up(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_textarea_cursor_up(lv_obj);
}

void pika_lvgl_cf_t___init__(PikaObj* self) {
    obj_setInt(self, "RAW", LV_IMG_CF_RAW);
    obj_setInt(self, "RAW_ALPHA", LV_IMG_CF_RAW_ALPHA);
    obj_setInt(self, "RAW_CHROMA_KEYED", LV_IMG_CF_RAW_CHROMA_KEYED);
    obj_setInt(self, "TRUE_COLOR", LV_IMG_CF_TRUE_COLOR);
    obj_setInt(self, "TRUE_COLOR_ALPHA", LV_IMG_CF_TRUE_COLOR_ALPHA);
    obj_setInt(self, "TRUE_COLOR_CHROMA_KEYED",
               LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED);
    obj_setInt(self, "INDEXED_1BIT", LV_IMG_CF_INDEXED_1BIT);
    obj_setInt(self, "INDEXED_2BIT", LV_IMG_CF_INDEXED_2BIT);
    obj_setInt(self, "INDEXED_4BIT", LV_IMG_CF_INDEXED_4BIT);
    obj_setInt(self, "INDEXED_8BIT", LV_IMG_CF_INDEXED_8BIT);
    obj_setInt(self, "ALPHA_1BIT", LV_IMG_CF_ALPHA_1BIT);
    obj_setInt(self, "ALPHA_2BIT", LV_IMG_CF_ALPHA_2BIT);
    obj_setInt(self, "ALPHA_4BIT", LV_IMG_CF_ALPHA_4BIT);
    obj_setInt(self, "ALPHA_8BIT", LV_IMG_CF_ALPHA_8BIT);
    obj_setInt(self, "RGB888", LV_IMG_CF_RGB888);
    obj_setInt(self, "RGBA8888", LV_IMG_CF_RGBA8888);
    obj_setInt(self, "RGBX8888", LV_IMG_CF_RGBX8888);
    obj_setInt(self, "RGB565", LV_IMG_CF_RGB565);
    obj_setInt(self, "RGBA5658", LV_IMG_CF_RGBA5658);
    obj_setInt(self, "RGB565A8", LV_IMG_CF_RGB565A8);
}

void pika_lvgl_img___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = PIKA_LV_OBJ(parent);
    lv_obj_t* lv_obj = lv_img_create(lv_parent);
    PIKA_LV_OBJ_SET(self, lv_obj);
}

int pika_lvgl_img_get_angle(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_img_get_angle(lv_obj);
}

int pika_lvgl_img_get_antialias(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_img_get_antialias(lv_obj);
}

int pika_lvgl_img_get_offset_x(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_img_get_offset_x(lv_obj);
}

int pika_lvgl_img_get_offset_y(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_img_get_offset_y(lv_obj);
}

int pika_lvgl_img_get_size_mode(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_img_get_size_mode(lv_obj);
}

int pika_lvgl_img_get_zoom(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_img_get_zoom(lv_obj);
}

void pika_lvgl_img_set_angle(PikaObj* self, int angle) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_img_set_angle(lv_obj, angle);
}

void pika_lvgl_img_set_antialias(PikaObj* self, int antialias) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_img_set_antialias(lv_obj, antialias);
}

void pika_lvgl_img_set_offset_x(PikaObj* self, int x) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_img_set_offset_x(lv_obj, x);
}

void pika_lvgl_img_set_offset_y(PikaObj* self, int y) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_img_set_offset_y(lv_obj, y);
}

void pika_lvgl_img_set_pivot(PikaObj* self, int x, int y) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_img_set_pivot(lv_obj, x, y);
}

void pika_lvgl_img_set_size_mode(PikaObj* self, int mode) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_img_set_size_mode(lv_obj, mode);
}

void pika_lvgl_img_dsc_t___init__(PikaObj* self, PikaObj* dsc_dict) {
    obj_setRef(self, "dsc_dict", dsc_dict);
    PikaDict* dsc_dict_ = obj_getPtr(dsc_dict, "dict");
    uint8_t* data = pikaDict_getBytes(dsc_dict_, "data");
    unsigned char wtmp[4] = {'0'};
    unsigned char htmp[4] = {'0'};
    memcpy(&wtmp, data + 16, 4);
    memcpy(&htmp, data + 20, 4);
    int w = ((int)(unsigned char)wtmp[2]) * 256 + (int)(unsigned char)wtmp[3];
    int h = ((int)(unsigned char)htmp[2]) * 256 + (int)(unsigned char)htmp[3];
    lv_img_dsc_t img_dsc = {
        .data = data,
        .data_size = pikaDict_getInt(dsc_dict_, "data_size"),
        .header =
            {
                .always_zero = 0,
                .cf = LV_IMG_CF_RAW_ALPHA,
                .w = w,
                .h = h,
            },
    };
    obj_setStruct(self, "img_dsc", img_dsc);
}

void pika_lvgl_img_set_src(PikaObj* self, PikaObj* src) {
    obj_setRef(self, "src", src);
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_img_dsc_t* img_dsc = obj_getStruct(src, "img_dsc");
    lv_img_set_src(lv_obj, img_dsc);
}

PikaObj* pika_lvgl_img_get_src(PikaObj* self) {
    return obj_getPtr(self, "src");
}

void pika_lvgl_img_set_zoom(PikaObj* self, int zoom) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_img_set_zoom(lv_obj, zoom);
}

/*
class chart(lv_obj):
    TYPE: CHART_TYPE
    AXIS: CHART_AXIS
    UPDATE_MODE: CHART_UPDATE_MODE
    DRAW_PART: CHART_DRAW_PART
    def __init__(self, parent: lv_obj): ...
    def set_point_count(self, cnt: int): ...
    def set_range(self, axis: int, min: int, max: int): ...
    def set_zoom_x(self, zoom_x: int): ...
    def set_zoom_y(self, zoom_y: int): ...
    def add_series(self, color: lv_color_t, axis: int) -> chart_series_t: ...
    def remove_series(self, series: chart_series_t): ...
    def hide_series(self, series: chart_series_t, hide: int): ...
    def set_series_color(self, series: chart_series_t, color: lv_color_t): ...
    def refresh(self): ...
    def set_type(self, type: int): ...
    def set_update_mode(self, update_mode: int): ...
    def set_div_line_count(self, hdiv: int, vdiv: int): ...
    def set_axis_tick(self, axis: int, major_len: int, minor_len: int,
                      major_cnt: int, minor_cnt: int, label_en: int, draw_size:
int): ...

    def get_type(self) -> int: ...
    def get_point_count(self) -> int: ...
    def get_x_start_point(self, ser: chart_series_t) -> int: ...
    def get_point_pos_by_id(self, ser: chart_series_t, id: int) -> point_t: ...
    def get_zoom_x(self) -> int: ...
    def get_zoom_y(self) -> int: ...
    def get_series_next(self, ser: chart_series_t) -> chart_series_t: ...
    def set_x_start_point(self, ser: chart_series_t, id: int): ...
    def add_cursor(self, color: lv_color_t, dir: int): ...
    def set_cursor_pos(self, cursor: chart_cursor_t, pos: point_t): ...
    def set_cursor_point(
        self, cursor: chart_cursor_t, ser: chart_series_t, point_id: int): ...

    def get_cursor_point(self, cursor: chart_cursor_t) -> point_t: ...
    def set_all_value(self, ser: chart_series_t, value: int): ...
    def set_next_value(self, ser: chart_series_t, value: int): ...
    def set_next_value2(self, ser: chart_series_t,
                        x_value: int, y_value: int): ...

    def set_value_by_id(self, ser: chart_series_t, id: int, value: int): ...
    def set_value_by_id2(self, ser: chart_series_t, id: int,
                         x_value: int, y_value: int): ...

    def set_ext_x_array(self, ser: chart_series_t, array: any): ...
    def set_ext_y_array(self, ser: chart_series_t, array: any): ...
    def get_y_array(self, ser: chart_series_t) -> any: ...
    def get_x_array(self, ser: chart_series_t) -> any: ...
    def get_pressed_point(self) -> int: ...
*/

void pika_lvgl_chart___init__(PikaObj* self, PikaObj* parent) {
    if (NULL != parent) {
        lv_obj_t* lv_obj = lv_chart_create(PIKA_LV_OBJ(parent));
        PIKA_LV_OBJ_SET(self, lv_obj);
    }
    obj_newDirectObj(self, "TYPE", New_pika_lvgl_CHART_TYPE);
    obj_newDirectObj(self, "AXIS", New_pika_lvgl_CHART_AXIS);
    obj_newDirectObj(self, "UPDATE_MODE", New_pika_lvgl_CHART_UPDATE_MODE);
    obj_newDirectObj(self, "DRAW_PART", New_pika_lvgl_CHART_DRAW_PART);
    obj_runMethod0(obj_getObj(self, "TYPE"), "__init__");
    obj_runMethod0(obj_getObj(self, "AXIS"), "__init__");
    obj_runMethod0(obj_getObj(self, "UPDATE_MODE"), "__init__");
    obj_runMethod0(obj_getObj(self, "DRAW_PART"), "__init__");
}

PikaObj* pika_lvgl_chart_add_series(PikaObj* self, PikaObj* color, int axis) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_color_t* lv_color = obj_getPtr(color, "lv_color");
    lv_chart_series_t* ser = lv_chart_add_series(lv_obj, *lv_color, axis);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_chart_series_t);
    obj_setPtr(new_obj, "series", ser);
    return new_obj;
}

void pika_lvgl_chart_remove_series(PikaObj* self, PikaObj* ser) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* series = obj_getPtr(ser, "lv_chart_series_t");
    lv_chart_remove_series(lv_obj, series);
}

void pika_lvgl_chart_hide_series(PikaObj* self, PikaObj* ser, int hide) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* series = obj_getPtr(ser, "lv_chart_series_t");
    lv_chart_hide_series(lv_obj, series, hide);
}

void pika_lvgl_chart_set_series_color(PikaObj* self,
                                      PikaObj* ser,
                                      PikaObj* color) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* series = obj_getPtr(ser, "lv_chart_series_t");
    lv_color_t* lv_color = obj_getPtr(color, "lv_color");
    lv_chart_set_series_color(lv_obj, series, *lv_color);
}

PikaObj* pika_lvgl_chart_get_series_next(PikaObj* self, PikaObj* ser) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* series = obj_getPtr(ser, "lv_chart_series_t");
    lv_chart_series_t* next = lv_chart_get_series_next(lv_obj, series);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_chart_series_t);
    obj_setPtr(new_obj, "series", next);
    return new_obj;
}

void pika_lvgl_chart_refresh(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_refresh(lv_obj);
}

void pika_lvgl_chart_set_point_count(PikaObj* self, int cnt) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_set_point_count(lv_obj, cnt);
}

void pika_lvgl_chart_set_range(PikaObj* self, int axis, int min, int max) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_set_range(lv_obj, axis, min, max);
}

void pika_lvgl_chart_set_zoom_x(PikaObj* self, int zoom_x) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_set_zoom_x(lv_obj, zoom_x);
}

void pika_lvgl_chart_set_zoom_y(PikaObj* self, int zoom_y) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_set_zoom_y(lv_obj, zoom_y);
}

void pika_lvgl_chart_set_type(PikaObj* self, int type) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_set_type(lv_obj, type);
}

void pika_lvgl_chart_set_update_mode(PikaObj* self, int update_mode) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_set_update_mode(lv_obj, update_mode);
}

void pika_lvgl_chart_set_div_line_count(PikaObj* self, int hdiv, int vdiv) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_set_div_line_count(lv_obj, hdiv, vdiv);
}

void pika_lvgl_chart_set_axis_tick(PikaObj* self,
                                   int axis,
                                   int major_len,
                                   int minor_len,
                                   int major_cnt,
                                   int minor_cnt,
                                   int label_en,
                                   int draw_size) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_set_axis_tick(lv_obj, axis, major_len, minor_len, major_cnt,
                           minor_cnt, label_en, draw_size);
}

int pika_lvgl_chart_get_type(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_chart_get_type(lv_obj);
}

int pika_lvgl_chart_get_point_count(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_chart_get_point_count(lv_obj);
}

int pika_lvgl_chart_get_x_start_point(PikaObj* self, PikaObj* ser) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* series = obj_getPtr(ser, "series");
    return lv_chart_get_x_start_point(lv_obj, series);
}

int pika_lvgl_chart_get_zoom_x(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_chart_get_zoom_x(lv_obj);
}

int pika_lvgl_chart_get_zoom_y(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_chart_get_zoom_y(lv_obj);
}

void pika_lvgl_chart_set_x_start_point(PikaObj* self, PikaObj* ser, int id) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* series = obj_getPtr(ser, "series");
    lv_chart_set_x_start_point(lv_obj, series, id);
}

PikaObj* pika_lvgl_chart_add_cursor(PikaObj* self, PikaObj* color, int dir) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_color_t* lv_color = obj_getPtr(color, "lv_color");
    lv_chart_cursor_t* cursor = lv_chart_add_cursor(lv_obj, *lv_color, dir);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_chart_cursor_t);
    obj_setPtr(new_obj, "cursor", cursor);
    return new_obj;
}

void pika_lvgl_chart_set_cursor_pos(PikaObj* self,
                                    PikaObj* cursor,
                                    PikaObj* pos) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_cursor_t* lv_cursor = obj_getPtr(cursor, "cursor");
    lv_point_t* lv_pos = obj_getPtr(pos, "lv_point");
    lv_chart_set_cursor_pos(lv_obj, lv_cursor, lv_pos);
}

void pika_lvgl_chart_set_cursor_point(PikaObj* self,
                                      PikaObj* cursor,
                                      PikaObj* ser,
                                      int point_id) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_cursor_t* lv_cursor = obj_getPtr(cursor, "cursor");
    lv_chart_series_t* lv_ser = obj_getPtr(ser, "series");
    lv_chart_set_cursor_point(lv_obj, lv_cursor, lv_ser, point_id);
}

PikaObj* pika_lvgl_chart_get_cursor_point(PikaObj* self, PikaObj* cursor) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_cursor_t* lv_cursor = obj_getPtr(cursor, "cursor");
    lv_point_t lv_pos = lv_chart_get_cursor_point(lv_obj, lv_cursor);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_point_t);
    obj_setStruct(new_obj, "lv_point_struct", lv_pos);
    obj_setPtr(self, "lv_point", obj_getStruct(new_obj, "lv_point_struct"));
    return new_obj;
}

PikaObj* pika_lvgl_chart_get_point_pos_by_id(PikaObj* self,
                                             PikaObj* ser,
                                             int id) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* lv_ser = obj_getPtr(ser, "series");
    lv_point_t lv_pos = {0};
    lv_chart_get_point_pos_by_id(lv_obj, lv_ser, id, &lv_pos);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_point_t);
    obj_setStruct(new_obj, "lv_point_struct", lv_pos);
    obj_setPtr(self, "lv_point", obj_getStruct(new_obj, "lv_point_struct"));
    return new_obj;
}

void pika_lvgl_chart_set_all_value(PikaObj* self, PikaObj* ser, int value) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* lv_ser = obj_getPtr(ser, "series");
    lv_chart_set_all_value(lv_obj, lv_ser, value);
}

void pika_lvgl_chart_set_next_value(PikaObj* self, PikaObj* ser, int value) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* lv_ser = obj_getPtr(ser, "series");
    lv_chart_set_next_value(lv_obj, lv_ser, value);
}

void pika_lvgl_chart_set_next_value2(PikaObj* self,
                                     PikaObj* ser,
                                     int x_value,
                                     int y_value) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* lv_ser = obj_getPtr(ser, "series");
    lv_chart_set_next_value2(lv_obj, lv_ser, x_value, y_value);
}

void pika_lvgl_chart_set_value_by_id(PikaObj* self,
                                     PikaObj* ser,
                                     int id,
                                     int value) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* lv_ser = obj_getPtr(ser, "series");
    lv_chart_set_value_by_id(lv_obj, lv_ser, id, value);
}

void pika_lvgl_chart_set_value_by_id2(PikaObj* self,
                                      PikaObj* ser,
                                      int id,
                                      int x_value,
                                      int y_value) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* lv_ser = obj_getPtr(ser, "series");
    lv_chart_set_value_by_id2(lv_obj, lv_ser, id, x_value, y_value);
}

void pika_lvgl_chart_set_ext_x_array(PikaObj* self, PikaObj* ser, Arg* array) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* lv_ser = obj_getPtr(ser, "series");
    if (arg_getType(array) == ARG_TYPE_INT) {
        lv_coord_t* arr = (lv_coord_t*)arg_getInt(array);
        lv_chart_set_ext_x_array(lv_obj, lv_ser, arr);
        pika_debug("set ext x array, arr: %p", arr);
    }
}

void pika_lvgl_chart_set_ext_y_array(PikaObj* self, PikaObj* ser, Arg* array) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* series = obj_getPtr(ser, "series");
    if (arg_getType(array) == ARG_TYPE_INT) {
        lv_coord_t* arr = (lv_coord_t*)arg_getInt(array);
        lv_chart_set_ext_y_array(lv_obj, series, arr);
        pika_debug("set ext y array, arr: %p", arr);
    }
}

Arg* pika_lvgl_chart_get_y_array(PikaObj* self, PikaObj* ser) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* lv_ser = obj_getPtr(ser, "series");
    lv_coord_t* arr = lv_chart_get_y_array(lv_obj, lv_ser);
    return arg_newInt((uintptr_t)arr);
}

Arg* pika_lvgl_chart_get_x_array(PikaObj* self, PikaObj* ser) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_chart_series_t* lv_ser = obj_getPtr(ser, "series");
    lv_coord_t* arr = lv_chart_get_x_array(lv_obj, lv_ser);
    return arg_newInt((uintptr_t)arr);
}

int pika_lvgl_chart_get_pressed_point(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_chart_get_pressed_point(lv_obj);
}

/*
typedef struct {
    lv_obj_t obj;
    char ** map;
    uint16_t tab_cnt;
    uint16_t tab_cur;
    lv_dir_t tab_pos;
} lv_tabview_t;

extern const lv_obj_class_t lv_tabview_class;

lv_obj_t * lv_tabview_create(lv_obj_t * parent, lv_dir_t tab_pos, lv_coord_t
tab_size); lv_obj_t * lv_tabview_add_tab(lv_obj_t * tv, const char * name);
lv_obj_t * lv_tabview_get_content(lv_obj_t * tv);
lv_obj_t * lv_tabview_get_tab_btns(lv_obj_t * tv);
void lv_tabview_set_act(lv_obj_t * obj, uint32_t id, lv_anim_enable_t anim_en);
uint16_t lv_tabview_get_tab_act(lv_obj_t * tv);

class tabview(lv_obj):
    def __init__(self, parent: lv_obj, tab_pos: int, tab_size: int): ...
    def add_tab(self, name: str) -> lv_obj: ...
    def get_content(self) -> lv_obj: ...
    def get_tab_btns(self) -> lv_obj: ...
    def set_act(self, id: int, anim_en: int): ...
    def get_tab_act(self) -> int: ...
*/

void pika_lvgl_tabview___init__(PikaObj* self,
                                PikaObj* parent,
                                int tab_pos,
                                int tab_size) {
    lv_obj_t* lv_obj =
        lv_tabview_create(PIKA_LV_OBJ(parent), tab_pos, tab_size);
    PIKA_LV_OBJ_SET(self, lv_obj);
}

PikaObj* pika_lvgl_tabview_add_tab(PikaObj* self, char* name) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_obj_t* tab = lv_tabview_add_tab(lv_obj, name);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_obj);
    PIKA_LV_OBJ_SET(new_obj, tab);
    return new_obj;
}

PikaObj* pika_lvgl_tabview_get_content(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_obj_t* content = lv_tabview_get_content(lv_obj);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_obj);
    PIKA_LV_OBJ_SET(new_obj, content);
    return new_obj;
}

PikaObj* pika_lvgl_tabview_get_tab_btns(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_obj_t* btns = lv_tabview_get_tab_btns(lv_obj);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_obj);
    PIKA_LV_OBJ_SET(new_obj, btns);
    return new_obj;
}

void pika_lvgl_tabview_set_act(PikaObj* self, int id, int anim_en) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_tabview_set_act(lv_obj, id, anim_en);
}

int pika_lvgl_tabview_get_tab_act(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_tabview_get_tab_act(lv_obj);
}

/*
class meter_indicator_t:
    def __init__(self): ...


class meter_scale_t:
    def __init__(self): ...
*/

void pika_lvgl_meter_indicator_t___init__(PikaObj* self) {
    lv_meter_indicator_t indicator_t = {0};
    obj_setStruct(self, "indicator_t", indicator_t);
    obj_setPtr(self, "indicator", obj_getStruct(self, "indicator_t"));
}

void pika_lvgl_meter_scale_t___init__(PikaObj* self) {
    lv_meter_scale_t scale_t = {0};
    obj_setStruct(self, "scale_t", scale_t);
    obj_setPtr(self, "scale", obj_getStruct(self, "scale_t"));
}

/*
class meter(lv_obj):
    """
    lv_obj_t * lv_meter_create(lv_obj_t * parent);
    meter_scale_t * lv_meter_add_scale(lv_obj_t * obj);
    void lv_meter_set_scale_ticks(lv_obj_t * obj, meter_scale_t * scale,
uint16_t cnt, uint16_t width, uint16_t len, lv_color_t color); void
lv_meter_set_scale_major_ticks(lv_obj_t * obj, meter_scale_t * scale,
uint16_t nth, uint16_t width, uint16_t len, lv_color_t color, int16_t
label_gap); void lv_meter_set_scale_range(lv_obj_t * obj, meter_scale_t *
scale, int32_t min, int32_t max, uint32_t angle_range, uint32_t rotation);
    meter_indicator_t * lv_meter_add_needle_line(lv_obj_t * obj,
meter_scale_t * scale, uint16_t width, lv_color_t color, int16_t r_mod);

    meter_indicator_t * lv_meter_add_needle_img(lv_obj_t * obj,
meter_scale_t * scale, const void * src, lv_coord_t pivot_x, lv_coord_t
pivot_y); meter_indicator_t * lv_meter_add_arc(lv_obj_t * obj,
meter_scale_t * scale, uint16_t width, lv_color_t color, int16_t r_mod);
    meter_indicator_t * lv_meter_add_scale_lines(lv_obj_t * obj,
meter_scale_t * scale, lv_color_t color_start, lv_color_t color_end, bool
local, int16_t width_mod); void lv_meter_set_indicator_value(lv_obj_t * obj,
meter_indicator_t * indic, int32_t value); void
lv_meter_set_indicator_start_value(lv_obj_t * obj, meter_indicator_t * indic,
int32_t value); void lv_meter_set_indicator_end_value(lv_obj_t * obj,
meter_indicator_t * indic, int32_t value);
    """

    def __init__(self, parent: lv_obj): ...
    def add_scale(self) -> meter_scale_t: ...

    def set_scale_ticks(self, scale: meter_scale_t, cnt: int, width: int,
                        len: int, color: lv_color_t): ...

    def set_scale_major_ticks(self, scale: meter_scale_t, nth: int, width:
int, len: int, color: lv_color_t, label_gap: int): ...

    def set_scale_range(self, scale: meter_scale_t, min: int, max: int,
                        angle_range: int, rotation: int): ...

    def add_needle_line(self, scale: meter_scale_t, width: int,
                        color: lv_color_t, r_mod: int) -> meter_indicator_t:
...

    def add_needle_img(self, scale: meter_scale_t, src: any,
                       pivot_x: int, pivot_y: int) -> meter_indicator_t: ...

    def add_arc(self, scale: meter_scale_t, width: int, color: lv_color_t,
                r_mod: int) -> meter_indicator_t: ...
    def add_scale_lines(self, scale: meter_scale_t, color_start: lv_color_t,
                        color_end: lv_color_t, local: int, width_mod: int) ->
meter_indicator_t: ...

    def set_indicator_value(self, indic: meter_indicator_t, value: int): ...

    def set_indicator_start_value(
        self, indic: meter_indicator_t, value: int): ...
    def set_indicator_end_value(
        self, indic: meter_indicator_t, value: int): ...
*/

void pika_lvgl_meter___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_obj = lv_meter_create(PIKA_LV_OBJ(parent));
    PIKA_LV_OBJ_SET(self, lv_obj);
}

PikaObj* pika_lvgl_meter_add_scale(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_meter_scale_t* scale = lv_meter_add_scale(lv_obj);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_meter_scale_t);
    obj_setPtr(new_obj, "scale", scale);
    return new_obj;
}

void pika_lvgl_meter_set_scale_ticks(PikaObj* self,
                                     PikaObj* scale,
                                     int cnt,
                                     int width,
                                     int len,
                                     PikaObj* color) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_meter_scale_t* lv_scale = obj_getPtr(scale, "scale");
    lv_color_t* lv_color = obj_getPtr(color, "lv_color");
    lv_meter_set_scale_ticks(lv_obj, lv_scale, cnt, width, len, *lv_color);
}

void pika_lvgl_meter_set_scale_major_ticks(PikaObj* self,
                                           PikaObj* scale,
                                           int nth,
                                           int width,
                                           int len,
                                           PikaObj* color,
                                           int label_gap) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_meter_scale_t* lv_scale = obj_getPtr(scale, "scale");
    lv_color_t* lv_color = obj_getPtr(color, "lv_color");
    lv_meter_set_scale_major_ticks(lv_obj, lv_scale, nth, width, len, *lv_color,
                                   label_gap);
}

void pika_lvgl_meter_set_scale_range(PikaObj* self,
                                     PikaObj* scale,
                                     int min,
                                     int max,
                                     int angle_range,
                                     int rotation) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_meter_scale_t* lv_scale = obj_getPtr(scale, "scale");
    lv_meter_set_scale_range(lv_obj, lv_scale, min, max, angle_range, rotation);
}

PikaObj* pika_lvgl_meter_add_needle_line(PikaObj* self,
                                         PikaObj* scale,
                                         int width,
                                         PikaObj* color,
                                         int r_mod) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_meter_scale_t* lv_scale = obj_getPtr(scale, "scale");
    lv_color_t* lv_color = obj_getPtr(color, "lv_color");
    lv_meter_indicator_t* indic =
        lv_meter_add_needle_line(lv_obj, lv_scale, width, *lv_color, r_mod);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_meter_indicator_t);
    obj_setPtr(new_obj, "indicator", indic);
    return new_obj;
}

PikaObj* pika_lvgl_meter_add_arc(PikaObj* self,
                                 PikaObj* scale,
                                 int width,
                                 PikaObj* color,
                                 int r_mod) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_meter_scale_t* lv_scale = obj_getPtr(scale, "scale");
    lv_color_t* lv_color = obj_getPtr(color, "lv_color");
    lv_meter_indicator_t* indic =
        lv_meter_add_arc(lv_obj, lv_scale, width, *lv_color, r_mod);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_meter_indicator_t);
    obj_setPtr(new_obj, "indicator", indic);
    return new_obj;
}

PikaObj* pika_lvgl_meter_add_scale_lines(PikaObj* self,
                                         PikaObj* scale,
                                         PikaObj* color_start,
                                         PikaObj* color_end,
                                         int local,
                                         int width_mod) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_meter_scale_t* lv_scale = obj_getPtr(scale, "scale");
    lv_color_t* lv_color_start = obj_getPtr(color_start, "lv_color");
    lv_color_t* lv_color_end = obj_getPtr(color_end, "lv_color");
    lv_meter_indicator_t* indic = lv_meter_add_scale_lines(
        lv_obj, lv_scale, *lv_color_start, *lv_color_end, local, width_mod);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_meter_indicator_t);
    obj_setPtr(new_obj, "indicator", indic);
    return new_obj;
}

void pika_lvgl_meter_set_indicator_value(PikaObj* self,
                                         PikaObj* indic,
                                         int value) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_meter_indicator_t* lv_indic = obj_getPtr(indic, "indicator");
    lv_meter_set_indicator_value(lv_obj, lv_indic, value);
}

void pika_lvgl_meter_set_indicator_start_value(PikaObj* self,
                                               PikaObj* indic,
                                               int value) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_meter_indicator_t* lv_indic = obj_getPtr(indic, "indicator");
    lv_meter_set_indicator_start_value(lv_obj, lv_indic, value);
}

void pika_lvgl_meter_set_indicator_end_value(PikaObj* self,
                                             PikaObj* indic,
                                             int value) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_meter_indicator_t* lv_indic = obj_getPtr(indic, "indicator");
    lv_meter_set_indicator_end_value(lv_obj, lv_indic, value);
}

/*
class KEYBOARD_MODE:
    TEXT_LOWER: int
    TEXT_UPPER: int
    SPECIAL: int
    NUMBER: int
    USER_1: int
    USER_2: int
    USER_3: int
    USER_4: int
    def __init__(self): ...
*/

void pika_lvgl_KEYBOARD_MODE___init__(PikaObj* self) {
    obj_setInt(self, "TEXT_LOWER", LV_KEYBOARD_MODE_TEXT_LOWER);
    obj_setInt(self, "TEXT_UPPER", LV_KEYBOARD_MODE_TEXT_UPPER);
    obj_setInt(self, "SPECIAL", LV_KEYBOARD_MODE_SPECIAL);
    obj_setInt(self, "NUMBER", LV_KEYBOARD_MODE_NUMBER);
    obj_setInt(self, "USER_1", LV_KEYBOARD_MODE_USER_1);
    obj_setInt(self, "USER_2", LV_KEYBOARD_MODE_USER_2);
    obj_setInt(self, "USER_3", LV_KEYBOARD_MODE_USER_3);
    obj_setInt(self, "USER_4", LV_KEYBOARD_MODE_USER_4);
}

/*
class keyboard(lv_obj):
    def __init__(self, parent: lv_obj): ...
    def set_textarea(self, ta: lv_obj): ...
    def set_mode(self, mode: int): ...
    def set_popovers(self, en: int): ...
    # def set_map(self, mode: int, map: any, ctrl_map: any): ...
    def get_textarea(self) -> lv_obj: ...
    def get_mode(self) -> int: ...
    # def get_popovers(self) -> int: ...
    # def get_map_array(self) -> any: ...
    def get_selected_btn(self) -> int: ...
    def get_btn_text(self, btn_id: int) -> str: ...
*/

lv_obj_t* pinyin_ime = NULL;
void pika_lvgl_keyboard___init__(PikaObj* self, PikaObj* parent) {
    if (NULL != parent) {
        lv_obj_t* lv_obj = lv_keyboard_create(PIKA_LV_OBJ(parent));
        PIKA_LV_OBJ_SET(self, lv_obj);
#if defined(LV_USE_IME_PINYIN) && (LV_USE_IME_PINYIN == 1) && 0
        pinyin_ime = lv_ime_pinyin_create(lv_scr_act());
        lv_ime_pinyin_set_keyboard(pinyin_ime, lv_obj);
        lv_ime_pinyin_set_mode(pinyin_ime, LV_IME_PINYIN_MODE_K26);
        lv_obj_set_style_text_font(pinyin_ime, &lv_font_simsun_16_cjk, 0);
        /*Get the cand_panel, and adjust its size and position*/
        lv_obj_t* cand_panel = lv_ime_pinyin_get_cand_panel(pinyin_ime);
        lv_obj_set_size(cand_panel, 300, LV_PCT(10));
        lv_obj_align_to(cand_panel, lv_obj, LV_ALIGN_OUT_TOP_MID, 0, 0);
#else
        static lv_style_t style;
        lv_style_init(&style);
        lv_style_set_text_font(&style, &lv_font_montserrat_14);
        lv_obj_add_style(lv_obj, &style, 0);
#endif
    }

    obj_newDirectObj(self, "MODE", New_pika_lvgl_KEYBOARD_MODE);
    obj_runMethod0(obj_getObj(self, "MODE"), "__init__");
}

void pika_lvgl_keyboard_set_textarea(PikaObj* self, PikaObj* ta) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_obj_t* lv_ta = PIKA_LV_OBJ(ta);
    lv_keyboard_set_textarea(lv_obj, lv_ta);
}

void pika_lvgl_keyboard_set_mode(PikaObj* self, int mode) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_keyboard_set_mode(lv_obj, mode);
}

void pika_lvgl_keyboard_set_popovers(PikaObj* self, int en) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_keyboard_set_popovers(lv_obj, en);
}

PikaObj* pika_lvgl_keyboard_get_textarea(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_obj_t* lv_ta = lv_keyboard_get_textarea(lv_obj);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_obj);
    PIKA_LV_OBJ_SET(new_obj, lv_ta);
    return new_obj;
}

int pika_lvgl_keyboard_get_mode(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_keyboard_get_mode(lv_obj);
}

int pika_lvgl_keyboard_get_selected_btn(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    return lv_keyboard_get_selected_btn(lv_obj);
}


/*
class ime_pinyin(lv_obj):
    def __init__(self, parent: lv_obj): ...
    def set_keyboard(self, kb: lv_obj): ...
    def set_mode(self, mode: int): ...
    def get_kb(self) -> lv_obj: ...
    def get_cand_panel(self) -> lv_obj: ...
*/

void pika_lvgl_ime_pinyin___init__(PikaObj* self, PikaObj* parent) {
    if (NULL != parent) {
        lv_obj_t* lv_obj = lv_ime_pinyin_create(PIKA_LV_OBJ(parent));
        PIKA_LV_OBJ_SET(self, lv_obj);
    }
}

void pika_lvgl_ime_pinyin_set_keyboard(PikaObj* self, PikaObj* kb) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_obj_t* lv_kb = PIKA_LV_OBJ(kb);
    lv_ime_pinyin_set_keyboard(lv_obj, lv_kb);
}

void pika_lvgl_ime_pinyin_set_mode(PikaObj* self, int mode) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_ime_pinyin_set_mode(lv_obj, mode);
}

PikaObj* pika_lvgl_ime_pinyin_get_kb(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_obj_t* lv_kb = lv_ime_pinyin_get_kb(lv_obj);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_obj);
    PIKA_LV_OBJ_SET(new_obj, lv_kb);
    return new_obj;
}

PikaObj* pika_lvgl_ime_pinyin_get_cand_panel(PikaObj* self) {
    lv_obj_t* lv_obj = PIKA_LV_OBJ(self);
    lv_obj_t* lv_panel = lv_ime_pinyin_get_cand_panel(lv_obj);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_obj);
    PIKA_LV_OBJ_SET(new_obj, lv_panel);
    return new_obj;
}


#endif
