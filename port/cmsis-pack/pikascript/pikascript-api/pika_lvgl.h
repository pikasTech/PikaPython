/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __pika_lvgl__H
#define __pika_lvgl__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_pika_lvgl(Args *args);

Arg* pika_lvgl_ALIGN(PikaObj *self);
Arg* pika_lvgl_ANIM(PikaObj *self);
Arg* pika_lvgl_EVENT(PikaObj *self);
Arg* pika_lvgl_OPA(PikaObj *self);
Arg* pika_lvgl_PALETTE(PikaObj *self);
Arg* pika_lvgl_STATE(PikaObj *self);
void pika_lvgl___init__(PikaObj *self);
Arg* pika_lvgl_arc(PikaObj *self);
Arg* pika_lvgl_bar(PikaObj *self);
Arg* pika_lvgl_btn(PikaObj *self);
Arg* pika_lvgl_checkbox(PikaObj *self);
Arg* pika_lvgl_dropdown(PikaObj *self);
PikaObj* pika_lvgl_indev_get_act(PikaObj *self);
Arg* pika_lvgl_indev_t(PikaObj *self);
Arg* pika_lvgl_label(PikaObj *self);
Arg* pika_lvgl_lv_color_t(PikaObj *self);
Arg* pika_lvgl_lv_event(PikaObj *self);
Arg* pika_lvgl_lv_obj(PikaObj *self);
Arg* pika_lvgl_lv_timer_t(PikaObj *self);
PikaObj* pika_lvgl_obj(PikaObj *self, PikaObj* parent);
PikaObj* pika_lvgl_palette_lighten(PikaObj *self, int p, int lvl);
PikaObj* pika_lvgl_palette_main(PikaObj *self, int p);
Arg* pika_lvgl_point_t(PikaObj *self);
Arg* pika_lvgl_roller(PikaObj *self);
PikaObj* pika_lvgl_scr_act(PikaObj *self);
Arg* pika_lvgl_slider(PikaObj *self);
Arg* pika_lvgl_style_t(PikaObj *self);
Arg* pika_lvgl_switch(PikaObj *self);
Arg* pika_lvgl_table(PikaObj *self);
Arg* pika_lvgl_textarea(PikaObj *self);
PikaObj* pika_lvgl_timer_create_basic(PikaObj *self);

#endif
