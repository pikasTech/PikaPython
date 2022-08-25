/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __pika_lvgl_indev_t__H
#define __pika_lvgl_indev_t__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_pika_lvgl_indev_t(Args *args);

void pika_lvgl_indev_t_get_vect(PikaObj *self, PikaObj* point);

#endif
