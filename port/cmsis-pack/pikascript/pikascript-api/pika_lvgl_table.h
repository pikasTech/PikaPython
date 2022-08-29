/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __pika_lvgl_table__H
#define __pika_lvgl_table__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_pika_lvgl_table(Args *args);

void pika_lvgl_table___init__(PikaObj *self, PikaObj* parent);
void pika_lvgl_table_set_cell_value(PikaObj *self, int row, int col, char* txt);

#endif
