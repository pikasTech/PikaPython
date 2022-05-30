/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdTask.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdTask_TaskMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdTask_Task(self);
    method_returnArg(args, res);
}

PikaObj *New_PikaStdTask(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineConstructor(self, "Task()->any", PikaStdTask_TaskMethod);
    return self;
}

