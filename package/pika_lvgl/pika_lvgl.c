#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../../lvgl.h"
#endif

#ifdef PIKASCRIPT
#include "BaseObj.h"
#include "pika_lvgl.h"
#include "pika_lvgl_ALIGN.h"
#include "pika_lvgl_ANIM.h"
#include "pika_lvgl_EVENT.h"
#include "pika_lvgl_FLEX_ALIGN.h"
#include "pika_lvgl_FLEX_FLOW.h"
#include "pika_lvgl_LAYOUT_FLEX.h"
#include "pika_lvgl_SIZE.h"
#include "pika_lvgl_OPA.h"
#include "pika_lvgl_PALETTE.h"
#include "pika_lvgl_STATE.h"
#include "pika_lvgl_TEXT_DECOR.h"
#include "pika_lvgl_arc.h"
#include "pika_lvgl_flag_t.h"
#include "pika_lvgl_indev_t.h"
#include "pika_lvgl_lv_color_t.h"
#include "pika_lvgl_lv_obj.h"
#include "pika_lvgl_lv_timer_t.h"

PikaObj* pika_lv_event_listener_g;
Args* pika_lv_id_register_g;

#if !PIKASCRIPT_VERSION_REQUIRE_MINIMUN(1, 11, 7)
#error "pikascript version must be greater than 1.11.7"
#endif

void pika_lvgl_STATE___init__(PikaObj* self) {
    obj_setInt(self, "DEFAULT", LV_STATE_DEFAULT);
    obj_setInt(self, "CHECKED", LV_STATE_CHECKED);
    obj_setInt(self, "FOCUSED", LV_STATE_FOCUSED);
    obj_setInt(self, "FOCUS_KEY", LV_STATE_FOCUS_KEY);
    obj_setInt(self, "EDITED", LV_STATE_EDITED);
    obj_setInt(self, "HOVERED", LV_STATE_HOVERED);
    obj_setInt(self, "PRESSED", LV_STATE_PRESSED);
    obj_setInt(self, "SCROLLED", LV_STATE_SCROLLED);
    obj_setInt(self, "DISABLED", LV_STATE_DISABLED);
    obj_setInt(self, "USER_1", LV_STATE_USER_1);
    obj_setInt(self, "USER_2", LV_STATE_USER_2);
    obj_setInt(self, "USER_3", LV_STATE_USER_3);
    obj_setInt(self, "USER_4", LV_STATE_USER_4);
    obj_setInt(self, "ANY", LV_STATE_ANY);
}

void pika_lvgl_flag_t___init__(PikaObj* self) {
    obj_setInt(self, "HIDDEN", LV_OBJ_FLAG_HIDDEN);
    obj_setInt(self, "CLICKABLE", LV_OBJ_FLAG_CLICKABLE);
    obj_setInt(self, "CLICK_FOCUSABLE", LV_OBJ_FLAG_CLICK_FOCUSABLE);
    obj_setInt(self, "CHECKABLE", LV_OBJ_FLAG_CHECKABLE);
    obj_setInt(self, "SCROLLABLE", LV_OBJ_FLAG_SCROLLABLE);
    obj_setInt(self, "SCROLL_ELASTIC", LV_OBJ_FLAG_SCROLL_ELASTIC);
    obj_setInt(self, "SCROLL_MOMENTUM", LV_OBJ_FLAG_SCROLL_MOMENTUM);
    obj_setInt(self, "SCROLL_ONE", LV_OBJ_FLAG_SCROLL_ONE);
    obj_setInt(self, "SCROLL_CHAIN_HOR", LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    obj_setInt(self, "SCROLL_CHAIN_VER", LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    obj_setInt(self, "SCROLL_CHAIN", LV_OBJ_FLAG_SCROLL_CHAIN);
    obj_setInt(self, "SCROLL_ON_FOCUS", LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    obj_setInt(self, "SCROLL_WITH_ARROW", LV_OBJ_FLAG_SCROLL_WITH_ARROW);
    obj_setInt(self, "SNAPPABLE", LV_OBJ_FLAG_SNAPPABLE);
    obj_setInt(self, "PRESS_LOCK", LV_OBJ_FLAG_PRESS_LOCK);
    obj_setInt(self, "EVENT_BUBBLE", LV_OBJ_FLAG_EVENT_BUBBLE);
    obj_setInt(self, "GESTURE_BUBBLE", LV_OBJ_FLAG_GESTURE_BUBBLE);
    obj_setInt(self, "ADV_HITTEST", LV_OBJ_FLAG_ADV_HITTEST);
    obj_setInt(self, "IGNORE_LAYOUT", LV_OBJ_FLAG_IGNORE_LAYOUT);
    obj_setInt(self, "FLOATING", LV_OBJ_FLAG_FLOATING);
    obj_setInt(self, "OVERFLOW_VISIBLE", LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    obj_setInt(self, "LAYOUT_1", LV_OBJ_FLAG_LAYOUT_1);
    obj_setInt(self, "LAYOUT_2", LV_OBJ_FLAG_LAYOUT_2);
    obj_setInt(self, "WIDGET_1", LV_OBJ_FLAG_WIDGET_1);
    obj_setInt(self, "WIDGET_2", LV_OBJ_FLAG_WIDGET_2);
    obj_setInt(self, "USER_1", LV_OBJ_FLAG_USER_1);
    obj_setInt(self, "USER_2", LV_OBJ_FLAG_USER_2);
    obj_setInt(self, "USER_3", LV_OBJ_FLAG_USER_3);
    obj_setInt(self, "USER_4", LV_OBJ_FLAG_USER_4);
}

void pika_lvgl_TEXT_DECOR___init__(PikaObj* self) {
    obj_setInt(self, "NONE", LV_TEXT_DECOR_NONE);
    obj_setInt(self, "UNDERLINE", LV_TEXT_DECOR_UNDERLINE);
    obj_setInt(self, "STRIKETHROUGH", LV_TEXT_DECOR_STRIKETHROUGH);
}

void pika_lvgl_ANIM___init__(PikaObj* self) {
    obj_setInt(self, "ON", LV_ANIM_ON);
    obj_setInt(self, "OFF", LV_ANIM_OFF);
}

void pika_lvgl_ALIGN___init__(PikaObj* self) {
    obj_setInt(self, "CENTER", LV_ALIGN_CENTER);
    obj_setInt(self, "DEFAULT", LV_ALIGN_DEFAULT);
    obj_setInt(self, "TOP_LEFT", LV_ALIGN_TOP_LEFT);
    obj_setInt(self, "TOP_MID", LV_ALIGN_TOP_MID);
    obj_setInt(self, "TOP_RIGHT", LV_ALIGN_TOP_RIGHT);
    obj_setInt(self, "BOTTOM_LEFT", LV_ALIGN_BOTTOM_LEFT);
    obj_setInt(self, "BOTTOM_MID", LV_ALIGN_BOTTOM_MID);
    obj_setInt(self, "BOTTOM_RIGHT", LV_ALIGN_BOTTOM_RIGHT);
    obj_setInt(self, "LEFT_MID", LV_ALIGN_LEFT_MID);
    obj_setInt(self, "RIGHT_MID", LV_ALIGN_RIGHT_MID);
    obj_setInt(self, "OUT_TOP_LEFT", LV_ALIGN_OUT_TOP_LEFT);
    obj_setInt(self, "OUT_TOP_MID", LV_ALIGN_OUT_TOP_MID);
    obj_setInt(self, "OUT_TOP_RIGHT", LV_ALIGN_OUT_TOP_RIGHT);
    obj_setInt(self, "OUT_BOTTOM_LEFT", LV_ALIGN_OUT_BOTTOM_LEFT);
    obj_setInt(self, "OUT_BOTTOM_MID", LV_ALIGN_OUT_BOTTOM_MID);
    obj_setInt(self, "OUT_BOTTOM_RIGHT", LV_ALIGN_OUT_BOTTOM_RIGHT);
    obj_setInt(self, "OUT_LEFT_TOP", LV_ALIGN_OUT_LEFT_TOP);
    obj_setInt(self, "OUT_LEFT_MID", LV_ALIGN_OUT_LEFT_MID);
    obj_setInt(self, "OUT_LEFT_BOTTOM", LV_ALIGN_OUT_LEFT_BOTTOM);
    obj_setInt(self, "OUT_RIGHT_TOP", LV_ALIGN_OUT_RIGHT_TOP);
    obj_setInt(self, "OUT_RIGHT_MID", LV_ALIGN_OUT_RIGHT_MID);
    obj_setInt(self, "OUT_RIGHT_BOTTOM", LV_ALIGN_OUT_RIGHT_BOTTOM);
}

void pika_lvgl_EVENT___init__(PikaObj* self) {
    obj_setInt(self, "ALL", LV_EVENT_ALL);
    obj_setInt(self, "PRESSED", LV_EVENT_PRESSED);
    obj_setInt(self, "PRESSING", LV_EVENT_PRESSING);
    obj_setInt(self, "PRESS_LOST", LV_EVENT_PRESS_LOST);
    obj_setInt(self, "SHORT_CLICKED", LV_EVENT_SHORT_CLICKED);
    obj_setInt(self, "LONG_PRESSED", LV_EVENT_LONG_PRESSED);
    obj_setInt(self, "LONG_PRESSED_REPEAT", LV_EVENT_LONG_PRESSED_REPEAT);
    obj_setInt(self, "CLICKED", LV_EVENT_CLICKED);
    obj_setInt(self, "RELEASED", LV_EVENT_RELEASED);
    obj_setInt(self, "SCROLL_BEGIN", LV_EVENT_SCROLL_BEGIN);
    obj_setInt(self, "SCROLL_END", LV_EVENT_SCROLL_END);
    obj_setInt(self, "SCROLL", LV_EVENT_SCROLL);
    obj_setInt(self, "GESTURE", LV_EVENT_GESTURE);
    obj_setInt(self, "KEY", LV_EVENT_KEY);
    obj_setInt(self, "FOCUSED", LV_EVENT_FOCUSED);
    obj_setInt(self, "DEFOCUSED", LV_EVENT_DEFOCUSED);
    obj_setInt(self, "LEAVE", LV_EVENT_LEAVE);
    obj_setInt(self, "HIT_TEST", LV_EVENT_HIT_TEST);
    obj_setInt(self, "COVER_CHECK", LV_EVENT_COVER_CHECK);
    obj_setInt(self, "REFR_EXT_DRAW_SIZE", LV_EVENT_REFR_EXT_DRAW_SIZE);
    obj_setInt(self, "DRAW_MAIN_BEGIN", LV_EVENT_DRAW_MAIN_BEGIN);
    obj_setInt(self, "DRAW_MAIN", LV_EVENT_DRAW_MAIN);
    obj_setInt(self, "DRAW_MAIN_END", LV_EVENT_DRAW_MAIN_END);
    obj_setInt(self, "DRAW_POST_BEGIN", LV_EVENT_DRAW_POST_BEGIN);
    obj_setInt(self, "DRAW_POST", LV_EVENT_DRAW_POST);
    obj_setInt(self, "DRAW_POST_END", LV_EVENT_DRAW_POST_END);
    obj_setInt(self, "DRAW_PART_BEGIN", LV_EVENT_DRAW_PART_BEGIN);
    obj_setInt(self, "DRAW_PART_END", LV_EVENT_DRAW_PART_END);
    obj_setInt(self, "VALUE_CHANGED", LV_EVENT_VALUE_CHANGED);
    obj_setInt(self, "INSERT", LV_EVENT_INSERT);
    obj_setInt(self, "REFRESH", LV_EVENT_REFRESH);
    obj_setInt(self, "READY", LV_EVENT_READY);
    obj_setInt(self, "CANCEL", LV_EVENT_CANCEL);
    obj_setInt(self, "DELETE", LV_EVENT_DELETE);
    obj_setInt(self, "CHILD_CHANGED", LV_EVENT_CHILD_CHANGED);
    obj_setInt(self, "CHILD_CREATED", LV_EVENT_CHILD_CREATED);
    obj_setInt(self, "CHILD_DELETED", LV_EVENT_CHILD_DELETED);
    obj_setInt(self, "SCREEN_UNLOAD_START", LV_EVENT_SCREEN_UNLOAD_START);
    obj_setInt(self, "SCREEN_LOAD_START", LV_EVENT_SCREEN_LOAD_START);
    obj_setInt(self, "SCREEN_LOADED", LV_EVENT_SCREEN_LOADED);
    obj_setInt(self, "SCREEN_UNLOADED", LV_EVENT_SCREEN_UNLOADED);
    obj_setInt(self, "SIZE_CHANGED", LV_EVENT_SIZE_CHANGED);
    obj_setInt(self, "STYLE_CHANGED", LV_EVENT_STYLE_CHANGED);
    obj_setInt(self, "LAYOUT_CHANGED", LV_EVENT_LAYOUT_CHANGED);
    obj_setInt(self, "GET_SELF_SIZE", LV_EVENT_GET_SELF_SIZE);
    obj_setInt(self, "PREPROCESS", LV_EVENT_PREPROCESS);
}

void pika_lvgl_OPA___init__(PikaObj* self) {
    obj_setInt(self, "TRANSP", LV_OPA_TRANSP);
    obj_setInt(self, "COVER", LV_OPA_COVER);
}

void pika_lvgl_PALETTE___init__(PikaObj* self) {
    obj_setInt(self, "RED", LV_PALETTE_RED);
    obj_setInt(self, "PINK", LV_PALETTE_PINK);
    obj_setInt(self, "PURPLE", LV_PALETTE_PURPLE);
    obj_setInt(self, "DEEP_PURPLE", LV_PALETTE_DEEP_PURPLE);
    obj_setInt(self, "INDIGO", LV_PALETTE_INDIGO);
    obj_setInt(self, "BLUE", LV_PALETTE_BLUE);
    obj_setInt(self, "LIGHT_BLUE", LV_PALETTE_LIGHT_BLUE);
    obj_setInt(self, "CYAN", LV_PALETTE_CYAN);
    obj_setInt(self, "TEAL", LV_PALETTE_TEAL);
    obj_setInt(self, "GREEN", LV_PALETTE_GREEN);
    obj_setInt(self, "LIGHT_GREEN", LV_PALETTE_LIGHT_GREEN);
    obj_setInt(self, "LIME", LV_PALETTE_LIME);
    obj_setInt(self, "YELLOW", LV_PALETTE_YELLOW);
    obj_setInt(self, "AMBER", LV_PALETTE_AMBER);
    obj_setInt(self, "ORANGE", LV_PALETTE_ORANGE);
    obj_setInt(self, "DEEP_ORANGE", LV_PALETTE_DEEP_ORANGE);
    obj_setInt(self, "BROWN", LV_PALETTE_BROWN);
    obj_setInt(self, "BLUE_GREY", LV_PALETTE_BLUE_GREY);
    obj_setInt(self, "GREY", LV_PALETTE_GREY);
    obj_setInt(self, "NONE", LV_PALETTE_NONE);
}

PikaObj* pika_lvgl_scr_act(PikaObj* self) {
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_obj);
    lv_obj_t* lv_obj = lv_scr_act();
    obj_setPtr(new_obj, "lv_obj", lv_obj);
    return new_obj;
}

volatile g_lvgl_inited = 0;
void pika_lvgl___init__(PikaObj* self) {
    obj_newDirectObj(self, "lv_event_listener", New_TinyObj);
    pika_lv_event_listener_g = obj_getObj(self, "lv_event_listener");
    pika_lv_id_register_g = New_args(NULL);
    if (!g_lvgl_inited) {
        lv_png_init();
        g_lvgl_inited = 1;
    }
}

void pika_lvgl___del__(PikaObj* self) {
    args_deinit(pika_lv_id_register_g);
}

void pika_lvgl_obj___init__(PikaObj* self, PikaTuple* parent) {
    PikaObj* parent_obj = NULL;
    if (NULL == parent) {
        void pika_lvgl_flag_tMethod(PikaObj * self, Args * args);
        class_defineConstructor(self, "FLAG", "", pika_lvgl_flag_tMethod);
        return;
    }
    if (pikaTuple_getSize(parent) == 1) {
        parent_obj = pikaTuple_getPtr(parent, 0);
        lv_obj_t* lv_parent = obj_getPtr(parent_obj, "lv_obj");
        lv_obj_t* lv_obj = lv_obj_create(lv_parent);
        obj_setPtr(self, "lv_obj", lv_obj);
        return;
    }
}

PikaObj* pika_lvgl_palette_lighten(PikaObj* self, int p, int lvl) {
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_color_t);
    lv_color_t lv_color = lv_palette_lighten(p, lvl);
    args_setStruct(new_obj->list, "lv_color_struct", lv_color);
    lv_color_t* plv_color = args_getStruct(new_obj->list, "lv_color_struct");
    obj_setPtr(new_obj, "lv_color", plv_color);
    return new_obj;
}

PikaObj* pika_lvgl_lv_color_hex(PikaObj* self, int64_t hex) {
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_color_t);
    lv_color_t lv_color = lv_color_hex(hex);
    args_setStruct(new_obj->list, "lv_color_struct", lv_color);
    lv_color_t* plv_color = args_getStruct(new_obj->list, "lv_color_struct");
    obj_setPtr(new_obj, "lv_color", plv_color);
    return new_obj;
}

PikaObj* pika_lvgl_palette_main(PikaObj* self, int p) {
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_color_t);
    lv_color_t lv_color = lv_palette_main(p);
    args_setStruct(new_obj->list, "lv_color_struct", lv_color);
    obj_setPtr(new_obj, "lv_color",
               args_getStruct(new_obj->list, "lv_color_struct"));
    return new_obj;
}

PikaObj* pika_lvgl_indev_get_act(PikaObj* self) {
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_indev_t);
    lv_indev_t* lv_indev = lv_indev_get_act();
    obj_setPtr(new_obj, "lv_indev", lv_indev);
    return new_obj;
}

PikaObj* pika_lvgl_timer_create_basic(PikaObj* self) {
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_timer_t);
    lv_timer_t* lv_timer = lv_timer_create_basic();
    obj_setPtr(new_obj, "lv_timer", lv_timer);
    return new_obj;
}

void pika_lvgl_FLEX_FLOW___init__(PikaObj* self) {
    obj_setInt(self, "ROW", LV_FLEX_FLOW_ROW);
    obj_setInt(self, "COLUMN", LV_FLEX_FLOW_COLUMN);
    obj_setInt(self, "ROW_WRAP", LV_FLEX_FLOW_ROW_WRAP);
    obj_setInt(self, "ROW_REVERSE", LV_FLEX_FLOW_ROW_REVERSE);
    obj_setInt(self, "ROW_WRAP_REVERSE", LV_FLEX_FLOW_ROW_WRAP_REVERSE);
    obj_setInt(self, "COLUMN_WRAP", LV_FLEX_FLOW_COLUMN_WRAP);
    obj_setInt(self, "COLUMN_REVERSE", LV_FLEX_FLOW_COLUMN_REVERSE);
    obj_setInt(self, "COLUMN_WRAP_REVERSE", LV_FLEX_FLOW_COLUMN_WRAP_REVERSE);
}

void pika_lvgl_FLEX_ALIGN___init__(PikaObj* self) {
    obj_setInt(self, "START", LV_FLEX_ALIGN_START);
    obj_setInt(self, "END", LV_FLEX_ALIGN_END);
    obj_setInt(self, "CENTER", LV_FLEX_ALIGN_CENTER);
    obj_setInt(self, "SPACE_EVENLY", LV_FLEX_ALIGN_SPACE_EVENLY);
    obj_setInt(self, "SPACE_AROUND", LV_FLEX_ALIGN_SPACE_AROUND);
    obj_setInt(self, "SPACE_BETWEEN", LV_FLEX_ALIGN_SPACE_BETWEEN);
}

void pika_lvgl_LAYOUT_FLEX___init__(PikaObj* self) {
    obj_setInt(self, "value", LV_LAYOUT_FLEX);
}

void pika_lvgl_SIZE___init__(PikaObj *self){
    obj_setInt(self, "CONTENT", LV_SIZE_CONTENT);
}

int pika_lvgl_pct(PikaObj *self, int x){
    return LV_PCT(x);
}

#endif
