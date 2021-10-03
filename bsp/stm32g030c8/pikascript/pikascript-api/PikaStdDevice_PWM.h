/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdDevice_PWM__H
#define __PikaStdDevice_PWM__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdDevice_PWM(Args *args);

void PikaStdDevice_PWM_enable(PikaObj *self);
float PikaStdDevice_PWM_getDuty(PikaObj *self);
int PikaStdDevice_PWM_getFrequency(PikaObj *self);
void PikaStdDevice_PWM_init(PikaObj *self);
void PikaStdDevice_PWM_platformEnable(PikaObj *self, float duty, int freq, char * pin);
void PikaStdDevice_PWM_platformSetDuty(PikaObj *self, float duty, char * pin);
void PikaStdDevice_PWM_platformSetFrequency(PikaObj *self, int freq, char * pin);
void PikaStdDevice_PWM_setDuty(PikaObj *self, float duty);
void PikaStdDevice_PWM_setFrequency(PikaObj *self, int freq);
void PikaStdDevice_PWM_setPin(PikaObj *self, char * pin);

#endif
