/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __pika_lvgl_lv_timer_t__H
#define __pika_lvgl_lv_timer_t__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_pika_lvgl_lv_timer_t(Args *args);

void pika_lvgl_lv_timer_t__del(PikaObj *self);
void pika_lvgl_lv_timer_t_set_cb(PikaObj *self, Arg* cb);
void pika_lvgl_lv_timer_t_set_period(PikaObj *self, int period);

#endif
