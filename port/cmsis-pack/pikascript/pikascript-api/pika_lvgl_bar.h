/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __pika_lvgl_bar__H
#define __pika_lvgl_bar__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_pika_lvgl_bar(Args *args);

void pika_lvgl_bar___init__(PikaObj *self, PikaObj* parent);
int pika_lvgl_bar_get_max_value(PikaObj *self);
int pika_lvgl_bar_get_min_value(PikaObj *self);
int pika_lvgl_bar_get_mode(PikaObj *self);
int pika_lvgl_bar_get_start_value(PikaObj *self);
int pika_lvgl_bar_get_value(PikaObj *self);
void pika_lvgl_bar_set_mode(PikaObj *self, int mode);
void pika_lvgl_bar_set_range(PikaObj *self, int min, int max);
void pika_lvgl_bar_set_start_value(PikaObj *self, int start_value, int anim);
void pika_lvgl_bar_set_value(PikaObj *self, int value, int anim);

#endif
