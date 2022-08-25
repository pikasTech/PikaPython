/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __pika_lvgl_arc__H
#define __pika_lvgl_arc__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_pika_lvgl_arc(Args *args);

void pika_lvgl_arc___init__(PikaObj *self, PikaObj* parent);
int pika_lvgl_arc_get_angle_end(PikaObj *self);
int pika_lvgl_arc_get_angle_start(PikaObj *self);
int pika_lvgl_arc_get_bg_angle_end(PikaObj *self);
int pika_lvgl_arc_get_bg_angle_start(PikaObj *self);
int pika_lvgl_arc_get_max_value(PikaObj *self);
int pika_lvgl_arc_get_min_value(PikaObj *self);
int pika_lvgl_arc_get_mode(PikaObj *self);
int pika_lvgl_arc_get_value(PikaObj *self);
void pika_lvgl_arc_set_angles(PikaObj *self, int start, int end);
void pika_lvgl_arc_set_bg_angles(PikaObj *self, int start, int end);
void pika_lvgl_arc_set_bg_end_angle(PikaObj *self, int angle);
void pika_lvgl_arc_set_bg_start_angle(PikaObj *self, int start);
void pika_lvgl_arc_set_change_rate(PikaObj *self, int rate);
void pika_lvgl_arc_set_end_angle(PikaObj *self, int angle);
void pika_lvgl_arc_set_mode(PikaObj *self, int mode);
void pika_lvgl_arc_set_range(PikaObj *self, int min, int max);
void pika_lvgl_arc_set_rotation(PikaObj *self, int rotation);
void pika_lvgl_arc_set_start_angle(PikaObj *self, int start);
void pika_lvgl_arc_set_value(PikaObj *self, int value);

#endif
