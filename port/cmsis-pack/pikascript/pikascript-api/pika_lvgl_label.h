/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __pika_lvgl_label__H
#define __pika_lvgl_label__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_pika_lvgl_label(Args *args);

void pika_lvgl_label___init__(PikaObj *self, PikaObj* parent);
void pika_lvgl_label_set_long_mode(PikaObj *self, int mode);
void pika_lvgl_label_set_recolor(PikaObj *self, int en);
void pika_lvgl_label_set_style_text_align(PikaObj *self, int value, int selector);
void pika_lvgl_label_set_text(PikaObj *self, char* txt);

#endif
