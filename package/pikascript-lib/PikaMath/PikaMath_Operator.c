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
int PikaMath_Operator_equalFloat(PikaObj* self, float num1, float num2) {
    return num1 == num2;
}
int PikaMath_Operator_equalInt(PikaObj* self, int num1, int num2) {
    return num1 == num2;
}
int PikaMath_Operator_graterThanFloat(PikaObj* self, float num1, float num2) {
    return num1 > num2;
}
int PikaMath_Operator_graterThanInt(PikaObj* self, int num1, int num2) {
    return num1 > num2;
}
int PikaMath_Operator_lessThanFloat(PikaObj* self, float num1, float num2) {
    return num1 < num2;
}
int PikaMath_Operator_lessThanInt(PikaObj* self, int num1, int num2) {
    return num1 < num2;
}
float PikaMath_Operator_minusFloat(PikaObj* self, float num1, float num2) {
    return num1 - num2;
}
int PikaMath_Operator_minusInt(PikaObj* self, int num1, int num2) {
    return num1 - num2;
}
float PikaMath_Operator_plusFloat(PikaObj* self, float num1, float num2) {
    return num1 + num2;
}
int PikaMath_Operator_plusInt(PikaObj* self, int num1, int num2) {
    return num1 + num2;
}