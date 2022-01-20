#include "PikaStdTask_Task.h"
#include "BaseObj.h"
#include "PikaVM.h"

extern PikaObj* __pikaMain;
void PikaStdTask_Task___init__(PikaObj* self) {
    pikaVM_runAsm(self,
                  "B0\n"
                  "0 RUN task_list_always.__init__\n"
                  "0 RUN task_list_when.__init__\n");
    obj_setPtr(__pikaMain, "__task_list_always",
               obj_getPtr(self, "task_list_always"));
}

void PikaStdTask_Task_call_always(PikaObj* self, Arg* fun_todo) {
    obj_setArg(self, "fun_todo", fun_todo);
    pikaVM_runAsm(self,
                  "B0\n"
                  "1 REF fun_todo\n"
                  "0 RUN task_list_always.append\n");
}

void PikaStdTask_Task_call_when(PikaObj* self, Arg* fun_todo, Arg* fun_when) {}

void PikaStdTask_Task_run_once(PikaObj* self) {
    /* reference the task_list_always in __pikaMain */
    pikaVM_runAsm(__pikaMain,
                  "B0\n"
                  "1 REF __task_list_always\n"
                  "0 RUN iter\n"
                  "0 OUT _l0\n"
                  "B0\n"
                  "0 RUN _l0.__next__\n"
                  "0 OUT fun_todo\n"
                  "0 EST fun_todo\n"
                  "0 JEZ 2\n"
                  "B1\n"
                  "0 RUN fun_todo\n"
                  "B0\n"
                  "0 JMP -1\n"
                  "B0\n"
                  "0 DEL _l0\n"
                  "B0\n");
}

void PikaStdTask_Task_run_always(PikaObj* self) {
    while (1) {
        PikaStdTask_Task_run_once(self);
    }
}
