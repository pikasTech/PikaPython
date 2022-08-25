/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __pika_lvgl_dropdown__H
#define __pika_lvgl_dropdown__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_pika_lvgl_dropdown(Args *args);

void pika_lvgl_dropdown___init__(PikaObj *self, PikaObj* parent);
void pika_lvgl_dropdown_add_option(PikaObj *self, char* option, int pos);
void pika_lvgl_dropdown_clear_options(PikaObj *self);
void pika_lvgl_dropdown_close(PikaObj *self);
int pika_lvgl_dropdown_get_dir(PikaObj *self);
int pika_lvgl_dropdown_get_option_cnt(PikaObj *self);
int pika_lvgl_dropdown_get_option_index(PikaObj *self, char* option);
char* pika_lvgl_dropdown_get_options(PikaObj *self);
int pika_lvgl_dropdown_get_selected(PikaObj *self);
int pika_lvgl_dropdown_get_selected_highlight(PikaObj *self);
char* pika_lvgl_dropdown_get_selected_str(PikaObj *self);
int pika_lvgl_dropdown_get_symbol(PikaObj *self);
char* pika_lvgl_dropdown_get_text(PikaObj *self);
int pika_lvgl_dropdown_is_open(PikaObj *self);
void pika_lvgl_dropdown_open(PikaObj *self);
void pika_lvgl_dropdown_set_dir(PikaObj *self, int dir);
void pika_lvgl_dropdown_set_options(PikaObj *self, char* options);
void pika_lvgl_dropdown_set_selected(PikaObj *self, int sel_opt);
void pika_lvgl_dropdown_set_selected_hightlight(PikaObj *self, int en);
void pika_lvgl_dropdown_set_symbol(PikaObj *self, char* symbol);
void pika_lvgl_dropdown_set_text(PikaObj *self, char* txt);

#endif
