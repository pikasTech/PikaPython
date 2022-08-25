/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __pika_lvgl_style_t__H
#define __pika_lvgl_style_t__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_pika_lvgl_style_t(Args *args);

void pika_lvgl_style_t___init__(PikaObj *self);
void pika_lvgl_style_t_init(PikaObj *self);
void pika_lvgl_style_t_set_bg_color(PikaObj *self, PikaObj* color);
void pika_lvgl_style_t_set_bg_opa(PikaObj *self, int opa);
void pika_lvgl_style_t_set_outline_color(PikaObj *self, PikaObj* color);
void pika_lvgl_style_t_set_outline_pad(PikaObj *self, int pad);
void pika_lvgl_style_t_set_outline_width(PikaObj *self, int w);
void pika_lvgl_style_t_set_radius(PikaObj *self, int radius);
void pika_lvgl_style_t_set_shadow_color(PikaObj *self, PikaObj* color);
void pika_lvgl_style_t_set_shadow_spread(PikaObj *self, int s);
void pika_lvgl_style_t_set_shadow_width(PikaObj *self, int w);

#endif
