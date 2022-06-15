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
