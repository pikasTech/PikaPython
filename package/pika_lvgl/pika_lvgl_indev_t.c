#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../../lvgl.h"
#endif

#ifdef PIKASCRIPT

#include "pika_lvgl_indev_t.h"
#include "pika_lvgl_lv_obj.h"
#include "pika_lvgl_point_t.h"

/*
class indev_t:
    def __init__(self): ...
    def enable(self, en: int): ...
    def get_type(self) -> int: ...
    def reset(self, obj: lv_obj): ...
    def reset_long_press(self): ...
    def set_cursor(self, cur_obj: lv_obj): ...
    # def set_group(self, group: lv_group): ...
    def set_button_points(self, points: point_t): ...
    def get_point(self, point: point_t): ...
    def get_gesture_dir(self) -> int: ...
    def get_key(self) -> int: ...
    def get_scroll_dir(self) -> int: ...
    def get_scroll_obj(self) -> lv_obj: ...
    def get_vect(self, point: point_t): ...
    def wait_release(self): ...
    def get_obj_act(self) -> lv_obj: ...
    # def get_read_timer(self) -> lv_timer: ...
    def search_obj(self, obj: lv_obj, point: point_t) -> lv_obj: ...
*/

void pika_lvgl_indev_t___init__(PikaObj* self) {
    lv_indev_t* lv_indev = lv_indev_get_act();
    obj_setPtr(self, "lv_indev", lv_indev);
}

void pika_lvgl_indev_t_enable(PikaObj* self, int en) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    lv_indev_enable(lv_indev, en);
}

int pika_lvgl_indev_t_get_type(PikaObj* self) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    return lv_indev_get_type(lv_indev);
}

void pika_lvgl_indev_t_reset(PikaObj* self, PikaObj* obj) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    lv_obj_t* lv_obj = obj_getPtr(obj, "lv_obj");
    lv_indev_reset(lv_indev, lv_obj);
}

void pika_lvgl_indev_t_reset_long_press(PikaObj* self) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    lv_indev_reset_long_press(lv_indev);
}

void pika_lvgl_indev_t_set_cursor(PikaObj* self, PikaObj* cur_obj) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    lv_obj_t* lv_cur_obj = obj_getPtr(cur_obj, "lv_obj");
    lv_indev_set_cursor(lv_indev, lv_cur_obj);
}

void pika_lvgl_indev_t_set_button_points(PikaObj* self, PikaObj* points) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    lv_point_t* lv_points = obj_getPtr(points, "lv_point");
    lv_indev_set_button_points(lv_indev, lv_points);
}

PikaObj* pika_lvgl_indev_t_get_point(PikaObj* self) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_point_t);
    lv_point_t lv_point = {0};
    lv_indev_get_point(lv_indev, &lv_point);
    obj_setInt(new_obj, "x", lv_point.x);
    obj_setInt(new_obj, "y", lv_point.y);
    return new_obj;
}

int pika_lvgl_indev_t_get_gesture_dir(PikaObj* self) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    return lv_indev_get_gesture_dir(lv_indev);
}

int pika_lvgl_indev_t_get_key(PikaObj* self) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    return lv_indev_get_key(lv_indev);
}

int pika_lvgl_indev_t_get_scroll_dir(PikaObj* self) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    return lv_indev_get_scroll_dir(lv_indev);
}

PikaObj* pika_lvgl_indev_t_get_scroll_obj(PikaObj* self) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    lv_obj_t* lv_obj = lv_indev_get_scroll_obj(lv_indev);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_obj);
    obj_setPtr(new_obj, "lv_obj", lv_obj);
    return new_obj;
}

void pika_lvgl_indev_t_get_vect(PikaObj* self, PikaObj* point) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    lv_point_t* lv_point = obj_getPtr(point, "lv_point");
    lv_indev_get_vect(lv_indev, lv_point);
    obj_setInt(point, "x", lv_point->x);
    obj_setInt(point, "y", lv_point->y);
}

void pika_lvgl_indev_t_wait_release(PikaObj* self) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    lv_indev_wait_release(lv_indev);
}

PikaObj* pika_lvgl_indev_t_get_obj_act(PikaObj* self, PikaObj* obj) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    lv_obj_t* lv_obj = lv_indev_get_obj_act(lv_indev);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_obj);
    obj_setPtr(new_obj, "lv_obj", lv_obj);
    return new_obj;
}

PikaObj* pika_lvgl_indev_t_search_obj(PikaObj* self, PikaObj* point) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    lv_point_t* lv_point = obj_getPtr(point, "lv_point");
    lv_obj_t* lv_obj = lv_indev_search_obj(lv_indev, lv_point);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_obj);
    obj_setPtr(new_obj, "lv_obj", lv_obj);
    return new_obj;
}

#endif
