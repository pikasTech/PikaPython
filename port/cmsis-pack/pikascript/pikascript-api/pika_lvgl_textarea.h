/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __pika_lvgl_textarea__H
#define __pika_lvgl_textarea__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_pika_lvgl_textarea(Args *args);

void pika_lvgl_textarea___init__(PikaObj *self, PikaObj* parent);
void pika_lvgl_textarea_set_one_line(PikaObj *self, int en);

#endif
