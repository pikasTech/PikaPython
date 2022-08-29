/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __pika_lvgl_lv_event__H
#define __pika_lvgl_lv_event__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_pika_lvgl_lv_event(Args *args);

int pika_lvgl_lv_event_get_code(PikaObj *self);
PikaObj* pika_lvgl_lv_event_get_target(PikaObj *self);

#endif
