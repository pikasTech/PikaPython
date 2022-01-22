#include "PikaStdTask_Task.h"
#include "BaseObj.h"
#include "PikaVM.h"

extern PikaObj* __pikaMain;
void PikaStdTask_Task___init__(PikaObj* self) {
    pikaVM_runAsm(self,
                  "B0\n"
                  "0 RUN calls_always.__init__\n"
                  "0 RUN calls_when.__init__\n"
                  "0 RUN assert_when.__init__\n");
    obj_setPtr(__pikaMain, "__calls_always", obj_getPtr(self, "calls_always"));
    obj_setPtr(__pikaMain, "__calls_when", obj_getPtr(self, "calls_when"));
    obj_setPtr(__pikaMain, "__assert_when", obj_getPtr(self, "assert_when"));
}

void PikaStdTask_Task_call_always(PikaObj* self, Arg* fun_todo) {
    obj_setArg(self, "fun_todo", fun_todo);
    pikaVM_runAsm(self,
                  "B0\n"
                  "1 REF fun_todo\n"
                  "0 RUN calls_always.append\n");
}

void PikaStdTask_Task_call_when(PikaObj* self, Arg* fun_todo, Arg* fun_when) {
    obj_setArg(self, "fun_todo", fun_todo);
    obj_setArg(self, "fun_when", fun_when);
    pikaVM_runAsm(self,
                  "B0\n"
                  "1 REF fun_todo\n"
                  "0 RUN calls_when.append\n"
                  "B0\n"
                  "1 REF fun_when\n"
                  "0 RUN assert_when.append\n");
}

void PikaStdTask_Task_run_once(PikaObj* self) {
    /* reference the calls_always in __pikaMain */
    pikaVM_runAsm(__pikaMain,
                  "B0\n"
                  "1 REF __calls_always\n"
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
                  "B0\n"
                  "0 RUN __calls_when.len\n"
                  "0 OUT len\n"
                  "B0\n"
                  "2 NUM 0\n"
                  "2 REF len\n"
                  "1 RUN range\n"
                  "0 RUN iter\n"
                  "0 OUT _l0\n"
                  "0 REF _r1\n"
                  "0 REF _r2\n"
                  "0 REF _r3\n"
                  "0 OUT _l0.a1\n"
                  "0 OUT _l0.a2\n"
                  "0 OUT _l0.a3\n"
                  "B0\n"
                  "0 RUN _l0.__next__\n"
                  "0 OUT i\n"
                  "0 EST i\n"
                  "0 JEZ 2\n"
                  "B1\n"
                  "1 REF len\n"
                  "1 NUM 0\n"
                  "0 OPT ==\n"
                  "0 JEZ 1\n"
                  "B2\n"
                  "0 BRK\n"
                  "B1\n"
                  "1 REF __assert_when\n"
                  "1 REF i\n"
                  "0 RUN __get__\n"
                  "0 OUT when\n"
                  "B1\n"
                  "0 RUN when\n"
                  "0 JEZ 1\n"
                  "B2\n"
                  "1 REF __calls_when\n"
                  "1 REF i\n"
                  "0 RUN __get__\n"
                  "0 OUT todo\n"
                  "B2\n"
                  "0 RUN todo\n"
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
