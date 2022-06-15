#include "pika_lvgl.h"
#include "BaseObj.h"
#include "lvgl.h"
#include "pika_lvgl_arc.h"
#include "pika_lvgl_lv_obj.h"

PikaObj* pika_lvgl_scr_act(PikaObj* self) {
    PikaObj* new_obj = newNormalObj(New_TinyObj);
    lv_obj_t* lv_obj = lv_scr_act();
    obj_setPtr(new_obj, "lv_obj", lv_obj);
    return new_obj;
}

void pika_lvgl___init__(PikaObj* self) {
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
}

PikaObj* pika_lvgl_obj(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_obj_create(lv_parent);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_obj);
    obj_setPtr(new_obj, "lv_obj", lv_obj);
    return new_obj;
}
