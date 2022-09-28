#include "PikaMath_Operator.h"

int PikaMath_Operator_AND(PikaObj* self, int flag1, int flag2) {
    return flag1 && flag2;
}
int PikaMath_Operator_NOT(PikaObj* self, int flag) {
    return !flag;
}
int PikaMath_Operator_OR(PikaObj* self, int flag1, int flag2) {
    return flag1 || flag2;
}
int PikaMath_Operator_equalFloat(PikaObj* self, pika_float num1, pika_float num2) {
    return num1 == num2;
}
int PikaMath_Operator_equalInt(PikaObj* self, int num1, int num2) {
    return num1 == num2;
}
int PikaMath_Operator_graterThanFloat(PikaObj* self, pika_float num1, pika_float num2) {
    return num1 > num2;
}
int PikaMath_Operator_graterThanInt(PikaObj* self, int num1, int num2) {
    return num1 > num2;
}
int PikaMath_Operator_lessThanFloat(PikaObj* self, pika_float num1, pika_float num2) {
    return num1 < num2;
}
int PikaMath_Operator_lessThanInt(PikaObj* self, int num1, int num2) {
    return num1 < num2;
}
pika_float PikaMath_Operator_minusFloat(PikaObj* self, pika_float num1, pika_float num2) {
    return num1 - num2;
}
int PikaMath_Operator_minusInt(PikaObj* self, int num1, int num2) {
    return num1 - num2;
}

pika_float PikaMath_Operator_plusFloat(PikaObj* self, pika_float num1, pika_float num2) {
    return num1 + num2;
}

int PikaMath_Operator_plusInt(PikaObj* self, int num1, int num2) {
    return num1 + num2;
}

char* PikaMath_Operator___str__(PikaObj *self){
    obj_setStr(self, "__buf", "test");
    return obj_getStr(self, "__buf");
}

void PikaMath_Operator___del__(PikaObj *self){
    __platform_printf("del operator...\r\n");
}


