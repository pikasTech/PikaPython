#ifndef _PIKA_LVGL_COMMON_H_
#define _PIKA_LVGL_COMMON_H_
#include "PikaObj.h"

#ifndef PIKA_LVGL_THREAD_LOCK_ENABLE
#define PIKA_LVGL_THREAD_LOCK_ENABLE 0
#endif

#define PIKA_LV_OBJ(_) obj_getPtr(_, "lv_obj")
#define PIKA_LV_OBJ_SET(_, v) obj_setPtr(_, "lv_obj", v)

#endif
