/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __pika_lvgl_roller__H
#define __pika_lvgl_roller__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_pika_lvgl_roller(Args *args);

void pika_lvgl_roller___init__(PikaObj *self, PikaObj* parent);
void pika_lvgl_roller_set_options(PikaObj *self, char* options, int mode);
void pika_lvgl_roller_set_visible_row_count(PikaObj *self, int row_cnt);

#endif
