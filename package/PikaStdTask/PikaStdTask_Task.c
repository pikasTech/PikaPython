#include "PikaStdTask_Task.h"
#include "BaseObj.h"
#include "PikaVM.h"

extern PikaObj* __pikaMain;
void PikaStdTask_Task___init__(PikaObj* self) {
    pikaVM_runAsm(self,
                  "B0\n"
                  "0 RUN calls_always.__init__\n"
                  "0 RUN calls_when.__init__\n"
                  "0 RUN assert_when.__init__\n"
                  "0 RUN calls_period.__init__\n"
                  "0 RUN assert_period.__init__\n"
                  "0 RUN time_period.__init__\n");
    obj_setPtr(__pikaMain, "__calls_always", obj_getPtr(self, "calls_always"));
    obj_setPtr(__pikaMain, "__calls_when", obj_getPtr(self, "calls_when"));
    obj_setPtr(__pikaMain, "__assert_when", obj_getPtr(self, "assert_when"));
    obj_setPtr(__pikaMain, "__calls_period", obj_getPtr(self, "calls_period"));
    obj_setPtr(__pikaMain, "__assert_period",
               obj_getPtr(self, "assert_period"));
    obj_setPtr(__pikaMain, "__time_period", obj_getPtr(self, "time_period"));
}

void PikaStdTask_Task_call_always(PikaObj* self, Arg* fun_todo) {
    obj_setArg(self, "fun_todo", fun_todo);
    pikaVM_runAsm(self, "B0\n1 REF fun_todo\n0 RUN calls_always.append\n");
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

void PikaStdTask_Task_call_period_ms(PikaObj* self,
                                     Arg* fun_todo,
                                     int period_ms) {
    obj_setArg(self, "fun_todo", fun_todo);
    obj_setInt(self, "period_ms", period_ms);
    pikaVM_runAsm(self,
                  "B0\n"
                  "1 REF fun_todo\n"
                  "0 RUN calls_period.append\n"
                  "B0\n"
                  "1 REF period_ms\n"
                  "0 RUN assert_period.append\n"
                  "B0\n"
                  "1 REF 0\n"
                  "0 RUN time_period.append\n");
}

void PikaStdTask_Task_run_once(PikaObj* self) {
    /* Python
    if calls_period.len() > 0:
        platformGetTick()
    */
    pikaVM_runAsm(self,
                  "B0\n"
                  "1 RUN calls_period.len\n"
                  "1 NUM 0\n"
                  "0 OPT >\n"
                  "0 JEZ 1\n"
                  "B1\n"
                  "0 RUN platformGetTick\n"
                  "B0\n");
    /* transfer the tick to pikaMain */
    obj_setInt(__pikaMain, "__tick", obj_getInt(self, "tick"));
    /* Python
    len = __calls_always.len()
    for i in range(0, len):
        if len == 0:
            break
        todo = __calls_always[i]
        todo()
    */
    pikaVM_runAsm(__pikaMain,
                  "B0\n"
                  "0 RUN __calls_always.len\n"
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
                  "1 REF __calls_always\n"
                  "1 REF i\n"
                  "0 RUN __get__\n"
                  "0 OUT todo\n"
                  "B1\n"
                  "0 RUN todo\n"
                  "B0\n"
                  "0 JMP -1\n"
                  "B0\n"
                  "0 DEL _l0\n"
                  "B0\n");

    /* Python
    __len = __calls_when.len()
    for i in range(0, __len):
        if __len == 0:
            break
        when = __assert_when[i]
        if when():
            todo = __calls_when[i]
            todo()
    */
    pikaVM_runAsm(__pikaMain,
                  "B0\n"
                  "0 RUN __calls_when.len\n"
                  "0 OUT __len\n"
                  "B0\n"
                  "2 NUM 0\n"
                  "2 REF __len\n"
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
                  "1 REF __len\n"
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
    /* Python
    __len = __calls_period.len()
    for i in range(0, __len):
        if __len == 0:
            break
        time = __time_period[i]
        if __tick > __time_period[i]:
            todo = __calls_period[i]
            todo()
            __time_period[i] = __tick + __assert_period[i]
    */
    pikaVM_runAsm(__pikaMain,
                  "B0\n"
                  "0 RUN __calls_period.len\n"
                  "0 OUT __len\n"
                  "B0\n"
                  "2 NUM 0\n"
                  "2 REF __len\n"
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
                  "1 REF __len\n"
                  "1 NUM 0\n"
                  "0 OPT ==\n"
                  "0 JEZ 1\n"
                  "B2\n"
                  "0 BRK\n"
                  "B1\n"
                  "1 REF __time_period\n"
                  "1 REF i\n"
                  "0 RUN __get__\n"
                  "0 OUT time\n"
                  "B1\n"
                  "1 REF __tick\n"
                  "2 REF __time_period\n"
                  "2 REF i\n"
                  "1 RUN __get__\n"
                  "0 OPT >\n"
                  "0 JEZ 1\n"
                  "B2\n"
                  "1 REF __calls_period\n"
                  "1 REF i\n"
                  "0 RUN __get__\n"
                  "0 OUT todo\n"
                  "B2\n"
                  "0 RUN todo\n"
                  "B2\n"
                  "1 REF __time_period\n"
                  "1 REF i\n"
                  "2 REF __tick\n"
                  "3 REF __assert_period\n"
                  "3 REF i\n"
                  "2 RUN __get__\n"
                  "1 OPT +\n"
                  "1 STR __time_period\n"
                  "0 RUN __set__\n"
                  "B0\n"
                  "0 JMP -1\n"
                  "B0\n"
                  "0 DEL _l0\n"
                  "B0 \n");
}

void PikaStdTask_Task_run_always(PikaObj* self) {
    while (1) {
        PikaStdTask_Task_run_once(self);
    }
}

void PikaStdTask_Task_platformGetTick(PikaObj* self) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}
