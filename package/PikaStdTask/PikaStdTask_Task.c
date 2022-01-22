#include "PikaStdTask_Task.h"
#include "BaseObj.h"
#include "PikaVM.h"

extern PikaObj* __pikaMain;
void PikaStdTask_Task___init__(PikaObj* self) {
    pikaVM_runAsm(self,
                  "B0\n0 RUN calls_always.__init__\n0 RUN "
                  "calls_when.__init__\n0 RUN assert_when.__init__\n");
    obj_setPtr(__pikaMain, "__calls_always", obj_getPtr(self, "calls_always"));
    obj_setPtr(__pikaMain, "__calls_when", obj_getPtr(self, "calls_when"));
    obj_setPtr(__pikaMain, "__assert_when", obj_getPtr(self, "assert_when"));
}

void PikaStdTask_Task_call_always(PikaObj* self, Arg* fun_todo) {
    obj_setArg(self, "fun_todo", fun_todo);
    pikaVM_runAsm(self, "B0\n1 REF fun_todo\n0 RUN calls_always.append\n");
}

void PikaStdTask_Task_call_when(PikaObj* self, Arg* fun_todo, Arg* fun_when) {
    obj_setArg(self, "fun_todo", fun_todo);
    obj_setArg(self, "fun_when", fun_when);
    pikaVM_runAsm(self,
                  "B0\n1 REF fun_todo\n0 RUN calls_when.append\nB0\n1 REF "
                  "fun_when\n0 RUN assert_when.append\n");
}

void PikaStdTask_Task_run_once(PikaObj* self) {
    /* reference the calls_always in __pikaMain */
    pikaVM_runAsm(
        __pikaMain,
        "B0\n1 REF __calls_always\n0 RUN iter\n0 OUT _l0\nB0\n0 RUN "
        "_l0.__next__\n0 OUT fun_todo\n0 EST fun_todo\n0 JEZ 2\nB1\n0 RUN "
        "fun_todo\nB0\n0 JMP -1\nB0\n0 DEL _l0\nB0\n0 RUN __calls_when.len\n0 "
        "OUT len\nB0\n2 NUM 0\n2 REF len\n1 RUN range\n0 RUN iter\n0 OUT "
        "_l0\n0 REF _r1\n0 REF _r2\n0 REF _r3\n0 OUT _l0.a1\n0 OUT _l0.a2\n0 "
        "OUT _l0.a3\nB0\n0 RUN _l0.__next__\n0 OUT i\n0 EST i\n0 JEZ 2\nB1\n1 "
        "REF len\n1 NUM 0\n0 OPT ==\n0 JEZ 1\nB2\n0 BRK\nB1\n1 REF "
        "__assert_when\n1 REF i\n0 RUN __get__\n0 OUT when\nB1\n0 RUN when\n0 "
        "JEZ 1\nB2\n1 REF __calls_when\n1 REF i\n0 RUN __get__\n0 OUT "
        "todo\nB2\n0 RUN todo\nB0\n0 JMP -1\nB0\n0 DEL _l0\nB0\n");
}

void PikaStdTask_Task_run_always(PikaObj* self) {
    while (1) {
        PikaStdTask_Task_run_once(self);
    }
}
