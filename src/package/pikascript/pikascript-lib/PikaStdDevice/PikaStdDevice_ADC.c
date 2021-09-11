#include "PikaStdDevice_ADC.h"
#include "BaseObj.h"

void PikaStdDevice_ADC_enable(PikaObj* self) {
  obj_run(self, "platformEnable(pin)");
}

void PikaStdDevice_ADC_init(PikaObj* self) {
  obj_setStr(self, "pin", "PA0");
}

float PikaStdDevice_ADC_read(PikaObj* self) {
  obj_run(self, "val = platformRead(pin)");
  return obj_getFloat(self, "val");
}

void PikaStdDevice_ADC_setPin(PikaObj* self, char* pin) {
  obj_setStr(self, "pin", pin);
}

void PikaStdDevice_ADC_platformEnable(PikaObj* self, char* pin) {
  obj_setErrorCode(self, 1);
  obj_setSysOut(self, "[error] platform method need to be override.");
}

float PikaStdDevice_ADC_platformRead(PikaObj* self, char* pin) {
  obj_setErrorCode(self, 1);
  obj_setSysOut(self, "[error] platform method need to be override.");
  return -1;
}