/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdTask_Task.h"
#include "TinyObj.h"
#include "PikaStdData_List.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdTask_Task___init__Method(PikaObj *self, Args *args){
    PikaStdTask_Task___init__(self);
}

void PikaStdTask_Task_call_alwaysMethod(PikaObj *self, Args *args){
    Arg* fun_todo = args_getArg(args, "fun_todo");
    PikaStdTask_Task_call_always(self, fun_todo);
}

void PikaStdTask_Task_call_period_msMethod(PikaObj *self, Args *args){
    Arg* fun_todo = args_getArg(args, "fun_todo");
    int period_ms = args_getInt(args, "period_ms");
    PikaStdTask_Task_call_period_ms(self, fun_todo, period_ms);
}

void PikaStdTask_Task_call_whenMethod(PikaObj *self, Args *args){
    Arg* fun_todo = args_getArg(args, "fun_todo");
    Arg* fun_when = args_getArg(args, "fun_when");
    PikaStdTask_Task_call_when(self, fun_todo, fun_when);
}

void PikaStdTask_Task_platformGetTickMethod(PikaObj *self, Args *args){
    PikaStdTask_Task_platformGetTick(self);
}

void PikaStdTask_Task_run_foreverMethod(PikaObj *self, Args *args){
    PikaStdTask_Task_run_forever(self);
}

void PikaStdTask_Task_run_onceMethod(PikaObj *self, Args *args){
    PikaStdTask_Task_run_once(self);
}

void PikaStdTask_Task_run_until_msMethod(PikaObj *self, Args *args){
    int until_ms = args_getInt(args, "until_ms");
    PikaStdTask_Task_run_until_ms(self, until_ms);
}

PikaObj *New_PikaStdTask_Task(Args *args){
    PikaObj *self = New_TinyObj(args);
    obj_newObj(self, "calls", "PikaStdData_List", New_PikaStdData_List);
    class_defineMethod(self, "__init__()", PikaStdTask_Task___init__Method);
    class_defineMethod(self, "call_always(fun_todo:any)", PikaStdTask_Task_call_alwaysMethod);
    class_defineMethod(self, "call_period_ms(fun_todo:any,period_ms:int)", PikaStdTask_Task_call_period_msMethod);
    class_defineMethod(self, "call_when(fun_todo:any,fun_when:any)", PikaStdTask_Task_call_whenMethod);
    class_defineMethod(self, "platformGetTick()", PikaStdTask_Task_platformGetTickMethod);
    class_defineMethod(self, "run_forever()", PikaStdTask_Task_run_foreverMethod);
    class_defineMethod(self, "run_once()", PikaStdTask_Task_run_onceMethod);
    class_defineMethod(self, "run_until_ms(until_ms:int)", PikaStdTask_Task_run_until_msMethod);
    return self;
}

Arg *PikaStdTask_Task(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdTask_Task);
}
