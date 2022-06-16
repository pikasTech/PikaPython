#include "pika_lvgl.h"
#include "BaseObj.h"
#include "lvgl.h"
#include "pika_lvgl_arc.h"
#include "pika_lvgl_lv_color_t.h"
#include "pika_lvgl_lv_obj.h"

PikaObj* pika_lv_event_listener_g;

PikaObj* pika_lvgl_scr_act(PikaObj* self) {
    PikaObj* new_obj = newNormalObj(New_TinyObj);
    lv_obj_t* lv_obj = lv_scr_act();
    obj_setPtr(new_obj, "lv_obj", lv_obj);
    return new_obj;
}

void pika_lvgl___init__(PikaObj* self) {
    obj_newDirectObj(self, "lv_event_listener", New_TinyObj);
    pika_lv_event_listener_g = obj_getObj(self, "lv_event_listener");
    obj_newDirectObj(self, "ALIGN", New_TinyObj);
    obj_setInt(self, "ALIGN.CENTER", LV_ALIGN_CENTER);
    obj_setInt(self, "ALIGN.DEFAULT", LV_ALIGN_DEFAULT);
    obj_setInt(self, "ALIGN.TOP_LEFT", LV_ALIGN_TOP_LEFT);
    obj_setInt(self, "ALIGN.TOP_MID", LV_ALIGN_TOP_MID);
    obj_setInt(self, "ALIGN.TOP_RIGHT", LV_ALIGN_TOP_RIGHT);
    obj_setInt(self, "ALIGN.BOTTOM_LEFT", LV_ALIGN_BOTTOM_LEFT);
    obj_setInt(self, "ALIGN.BOTTOM_MID", LV_ALIGN_BOTTOM_MID);
    obj_setInt(self, "ALIGN.BOTTOM_RIGHT", LV_ALIGN_BOTTOM_RIGHT);
    obj_setInt(self, "ALIGN.LEFT_MID", LV_ALIGN_LEFT_MID);
    obj_setInt(self, "ALIGN.RIGHT_MID", LV_ALIGN_RIGHT_MID);
    obj_setInt(self, "ALIGN.OUT_TOP_LEFT", LV_ALIGN_OUT_TOP_LEFT);
    obj_setInt(self, "ALIGN.OUT_TOP_MID", LV_ALIGN_OUT_TOP_MID);
    obj_setInt(self, "ALIGN.OUT_TOP_RIGHT", LV_ALIGN_OUT_TOP_RIGHT);
    obj_setInt(self, "ALIGN.OUT_BOTTOM_LEFT", LV_ALIGN_OUT_BOTTOM_LEFT);
    obj_setInt(self, "ALIGN.OUT_BOTTOM_MID", LV_ALIGN_OUT_BOTTOM_MID);
    obj_setInt(self, "ALIGN.OUT_BOTTOM_RIGHT", LV_ALIGN_OUT_BOTTOM_RIGHT);
    obj_setInt(self, "ALIGN.OUT_LEFT_TOP", LV_ALIGN_OUT_LEFT_TOP);
    obj_setInt(self, "ALIGN.OUT_LEFT_MID", LV_ALIGN_OUT_LEFT_MID);
    obj_setInt(self, "ALIGN.OUT_LEFT_BOTTOM", LV_ALIGN_OUT_LEFT_BOTTOM);
    obj_setInt(self, "ALIGN.OUT_RIGHT_TOP", LV_ALIGN_OUT_RIGHT_TOP);
    obj_setInt(self, "ALIGN.OUT_RIGHT_MID", LV_ALIGN_OUT_RIGHT_MID);
    obj_setInt(self, "ALIGN.OUT_RIGHT_BOTTOM", LV_ALIGN_OUT_RIGHT_BOTTOM);

    obj_newDirectObj(self, "PALETTE", New_TinyObj);
    obj_setInt(self, "PALETTE.RED", LV_PALETTE_RED);
    obj_setInt(self, "PALETTE.PINK", LV_PALETTE_PINK);
    obj_setInt(self, "PALETTE.PURPLE", LV_PALETTE_PURPLE);
    obj_setInt(self, "PALETTE.DEEP_PURPLE", LV_PALETTE_DEEP_PURPLE);
    obj_setInt(self, "PALETTE.INDIGO", LV_PALETTE_INDIGO);
    obj_setInt(self, "PALETTE.BLUE", LV_PALETTE_BLUE);
    obj_setInt(self, "PALETTE.LIGHT_BLUE", LV_PALETTE_LIGHT_BLUE);
    obj_setInt(self, "PALETTE.CYAN", LV_PALETTE_CYAN);
    obj_setInt(self, "PALETTE.TEAL", LV_PALETTE_TEAL);
    obj_setInt(self, "PALETTE.GREEN", LV_PALETTE_GREEN);
    obj_setInt(self, "PALETTE.LIGHT_GREEN", LV_PALETTE_LIGHT_GREEN);
    obj_setInt(self, "PALETTE.LIME", LV_PALETTE_LIME);
    obj_setInt(self, "PALETTE.YELLOW", LV_PALETTE_YELLOW);
    obj_setInt(self, "PALETTE.AMBER", LV_PALETTE_AMBER);
    obj_setInt(self, "PALETTE.ORANGE", LV_PALETTE_ORANGE);
    obj_setInt(self, "PALETTE.DEEP_ORANGE", LV_PALETTE_DEEP_ORANGE);
    obj_setInt(self, "PALETTE.BROWN", LV_PALETTE_BROWN);
    obj_setInt(self, "PALETTE.BLUE_GREY", LV_PALETTE_BLUE_GREY);
    obj_setInt(self, "PALETTE.GREY", LV_PALETTE_GREY);
    obj_setInt(self, "PALETTE.NONE", LV_PALETTE_NONE);

    obj_newDirectObj(self, "ANIM", New_TinyObj);
    obj_setInt(self, "ANIM.ON", LV_ANIM_OFF);
    obj_setInt(self, "ANIM.OFF", LV_ANIM_ON);

    obj_newDirectObj(self, "STATE", New_TinyObj);
    obj_setInt(self, "STATE.DEFAULT", LV_STATE_DEFAULT);
    obj_setInt(self, "STATE.CHECKED", LV_STATE_CHECKED);
    obj_setInt(self, "STATE.FOCUSED", LV_STATE_FOCUSED);
    obj_setInt(self, "STATE.FOCUS_KEY", LV_STATE_FOCUS_KEY);
    obj_setInt(self, "STATE.EDITED", LV_STATE_EDITED);
    obj_setInt(self, "STATE.HOVERED", LV_STATE_HOVERED);
    obj_setInt(self, "STATE.PRESSED", LV_STATE_PRESSED);
    obj_setInt(self, "STATE.SCROLLED", LV_STATE_SCROLLED);
    obj_setInt(self, "STATE.DISABLED", LV_STATE_DISABLED);
    obj_setInt(self, "STATE.USER_1", LV_STATE_USER_1);
    obj_setInt(self, "STATE.USER_2", LV_STATE_USER_2);
    obj_setInt(self, "STATE.USER_3", LV_STATE_USER_3);
    obj_setInt(self, "STATE.USER_4", LV_STATE_USER_4);
    obj_setInt(self, "STATE.ANY", LV_STATE_ANY);

    obj_newDirectObj(self, "EVENT", New_TinyObj);
    obj_setInt(self, "EVENT.ALL", LV_EVENT_ALL);
    obj_setInt(self, "EVENT.PRESSED", LV_EVENT_PRESSED);
    obj_setInt(self, "EVENT.PRESSING", LV_EVENT_PRESSING);
    obj_setInt(self, "EVENT.PRESS_LOST", LV_EVENT_PRESS_LOST);
    obj_setInt(self, "EVENT.SHORT_CLICKED", LV_EVENT_SHORT_CLICKED);
    obj_setInt(self, "EVENT.LONG_PRESSED", LV_EVENT_LONG_PRESSED);
    obj_setInt(self, "EVENT.LONG_PRESSED_REPEAT", LV_EVENT_LONG_PRESSED_REPEAT);
    obj_setInt(self, "EVENT.CLICKED", LV_EVENT_CLICKED);
    obj_setInt(self, "EVENT.RELEASED", LV_EVENT_RELEASED);
    obj_setInt(self, "EVENT.SCROLL_BEGIN", LV_EVENT_SCROLL_BEGIN);
    obj_setInt(self, "EVENT.SCROLL_END", LV_EVENT_SCROLL_END);
    obj_setInt(self, "EVENT.SCROLL", LV_EVENT_SCROLL);
    obj_setInt(self, "EVENT.GESTURE", LV_EVENT_GESTURE);
    obj_setInt(self, "EVENT.KEY", LV_EVENT_KEY);
    obj_setInt(self, "EVENT.FOCUSED", LV_EVENT_FOCUSED);
    obj_setInt(self, "EVENT.DEFOCUSED", LV_EVENT_DEFOCUSED);
    obj_setInt(self, "EVENT.LEAVE", LV_EVENT_LEAVE);
    obj_setInt(self, "EVENT.HIT_TEST", LV_EVENT_HIT_TEST);
    obj_setInt(self, "EVENT.COVER_CHECK", LV_EVENT_COVER_CHECK);
    obj_setInt(self, "EVENT.REFR_EXT_DRAW_SIZE", LV_EVENT_REFR_EXT_DRAW_SIZE);
    obj_setInt(self, "EVENT.DRAW_MAIN_BEGIN", LV_EVENT_DRAW_MAIN_BEGIN);
    obj_setInt(self, "EVENT.DRAW_MAIN", LV_EVENT_DRAW_MAIN);
    obj_setInt(self, "EVENT.DRAW_MAIN_END", LV_EVENT_DRAW_MAIN_END);
    obj_setInt(self, "EVENT.DRAW_POST_BEGIN", LV_EVENT_DRAW_POST_BEGIN);
    obj_setInt(self, "EVENT.DRAW_POST", LV_EVENT_DRAW_POST);
    obj_setInt(self, "EVENT.DRAW_POST_END", LV_EVENT_DRAW_POST_END);
    obj_setInt(self, "EVENT.DRAW_PART_BEGIN", LV_EVENT_DRAW_PART_BEGIN);
    obj_setInt(self, "EVENT.DRAW_PART_END", LV_EVENT_DRAW_PART_END);
    obj_setInt(self, "EVENT.VALUE_CHANGED", LV_EVENT_VALUE_CHANGED);
    obj_setInt(self, "EVENT.INSERT", LV_EVENT_INSERT);
    obj_setInt(self, "EVENT.REFRESH", LV_EVENT_REFRESH);
    obj_setInt(self, "EVENT.READY", LV_EVENT_READY);
    obj_setInt(self, "EVENT.CANCEL", LV_EVENT_CANCEL);
    obj_setInt(self, "EVENT.DELETE", LV_EVENT_DELETE);
    obj_setInt(self, "EVENT.CHILD_CHANGED", LV_EVENT_CHILD_CHANGED);
    obj_setInt(self, "EVENT.CHILD_CREATED", LV_EVENT_CHILD_CREATED);
    obj_setInt(self, "EVENT.CHILD_DELETED", LV_EVENT_CHILD_DELETED);
    obj_setInt(self, "EVENT.SCREEN_UNLOAD_START", LV_EVENT_SCREEN_UNLOAD_START);
    obj_setInt(self, "EVENT.SCREEN_LOAD_START", LV_EVENT_SCREEN_LOAD_START);
    obj_setInt(self, "EVENT.SCREEN_LOADED", LV_EVENT_SCREEN_LOADED);
    obj_setInt(self, "EVENT.SCREEN_UNLOADED", LV_EVENT_SCREEN_UNLOADED);
    obj_setInt(self, "EVENT.SIZE_CHANGED", LV_EVENT_SIZE_CHANGED);
    obj_setInt(self, "EVENT.STYLE_CHANGED", LV_EVENT_STYLE_CHANGED);
    obj_setInt(self, "EVENT.LAYOUT_CHANGED", LV_EVENT_LAYOUT_CHANGED);
    obj_setInt(self, "EVENT.GET_SELF_SIZE", LV_EVENT_GET_SELF_SIZE);
    obj_setInt(self, "EVENT.PREPROCESS", LV_EVENT_PREPROCESS);

    obj_newDirectObj(self, "OPA", New_TinyObj);
    obj_setInt(self, "OPA.TRANSP", LV_OPA_TRANSP);
    obj_setInt(self, "OPA.COVER", LV_OPA_COVER);
}

PikaObj* pika_lvgl_obj(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_obj_create(lv_parent);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_obj);
    obj_setPtr(new_obj, "lv_obj", lv_obj);
    return new_obj;
}

PikaObj* pika_lvgl_palette_lighten(PikaObj* self, int lvl, int p) {
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_color_t);
    lv_color_t lv_color = lv_palette_lighten(p, lvl);
    args_setStruct(new_obj->list, "lv_color_struct", lv_color);
    lv_color_t* plv_color = args_getStruct(new_obj->list, "lv_color_struct");
    obj_setPtr(new_obj, "lv_color",plv_color);
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
