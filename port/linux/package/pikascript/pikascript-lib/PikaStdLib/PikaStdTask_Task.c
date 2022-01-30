#include "BaseObj.h"
#include "PikaVM.h"

extern PikaObj* __pikaMain;
void PikaStdTask_Task___init__(PikaObj* self) {
    obj_run(self,
            "calls.__init__()\n"
            "is_period = 0\n");
    obj_setPtr(__pikaMain, "__calls", obj_getPtr(self, "calls"));
}

void PikaStdTask_Task_call_always(PikaObj* self, Arg* fun_todo) {
    obj_setArg(self, "fun_todo", fun_todo);
    obj_run(self,
            "calls.append('always')\n"
            "calls.append(fun_todo)\n");
}

void PikaStdTask_Task_call_when(PikaObj* self, Arg* fun_todo, Arg* fun_when) {
    obj_setArg(self, "fun_todo", fun_todo);
    obj_setArg(self, "fun_when", fun_when);
    obj_run(self,
            "calls.append('when')\n"
            "calls.append(fun_when)\n"
            "calls.append(fun_todo)\n");
}

void PikaStdTask_Task_call_period_ms(PikaObj* self,
                                     Arg* fun_todo,
                                     int period_ms) {
    obj_setArg(self, "fun_todo", fun_todo);
    obj_setInt(self, "period_ms", period_ms);
    obj_run(self,
            "calls.append('period_ms')\n"
            "calls.append(period_ms)\n"
            "calls.append(fun_todo)\n"
            "calls.append(0)\n"
            "is_period = 1\n");
}

void PikaStdTask_Task_run_once(PikaObj* self) {
    /* transfer the tick to pikaMain */
    obj_setInt(__pikaMain, "__tick", obj_getInt(self, "tick"));
    /* Python
    obj_run(__pikaMain,
            "len = __calls.len()\n"
            "mode = 'none'\n"
            "info_index = 0\n"
            "for i in range(0, len):\n"
            "    if len == 0:\n"
            "        break\n"
            "    if info_index == 0:\n"
            "        mode = __calls[i]\n"
            "        info_index = 1\n"
            "    elif info_index == 1:\n"
            "        if mode == 'always':\n"
            "            todo = __calls[i]\n"
            "            todo()\n"
            "            info_index = 0\n"
            "        elif mode == 'when':\n"
            "            when = __calls[i]\n"
            "            info_index = 2\n"
            "        elif mode == 'period_ms':\n"
            "            period_ms = __calls[i]\n"
            "            info_index = 2\n"
            "    elif info_index == 2:\n"
            "        if mode == 'when':\n"
            "            if when():\n"
            "                todo = __calls[i]\n"
            "                todo()\n"
            "            info_index = 0\n"
            "        elif mode == 'period_ms':\n"
            "            todo = __calls[i]\n"
            "            info_index = 3\n"
            "    elif info_index == 3:\n"
            "        if mode == 'period_ms':\n"
            "            if __tick > __calls[i]:\n"
            "                todo()\n"
            "                __calls[i] = __tick + period_ms\n"
            "            info_index = 0\n"
            "\n");
    */
    pikaVM_runAsm(__pikaMain,
                  "B0\n"
                  "0 RUN __calls.len\n"
                  "0 OUT len\n"
                  "B0\n"
                  "0 STR none\n"
                  "0 OUT mode\n"
                  "B0\n"
                  "0 NUM 0\n"
                  "0 OUT info_index\n"
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
                  "1 REF info_index\n"
                  "1 NUM 0\n"
                  "0 OPT ==\n"
                  "0 JEZ 1\n"
                  "B2\n"
                  "1 REF __calls\n"
                  "1 REF i\n"
                  "0 RUN __get__\n"
                  "0 OUT mode\n"
                  "B2\n"
                  "0 NUM 1\n"
                  "0 OUT info_index\n"
                  "B1\n"
                  "0 NEL 1\n"
                  "1 REF info_index\n"
                  "1 NUM 1\n"
                  "0 OPT ==\n"
                  "0 JEZ 1\n"
                  "B2\n"
                  "1 REF mode\n"
                  "1 STR always\n"
                  "0 OPT ==\n"
                  "0 JEZ 1\n"
                  "B3\n"
                  "1 REF __calls\n"
                  "1 REF i\n"
                  "0 RUN __get__\n"
                  "0 OUT todo\n"
                  "B3\n"
                  "0 RUN todo\n"
                  "B3\n"
                  "0 NUM 0\n"
                  "0 OUT info_index\n"
                  "B2\n"
                  "0 NEL 1\n"
                  "1 REF mode\n"
                  "1 STR when\n"
                  "0 OPT ==\n"
                  "0 JEZ 1\n"
                  "B3\n"
                  "1 REF __calls\n"
                  "1 REF i\n"
                  "0 RUN __get__\n"
                  "0 OUT when\n"
                  "B3\n"
                  "0 NUM 2\n"
                  "0 OUT info_index\n"
                  "B2\n"
                  "0 NEL 1\n"
                  "1 REF mode\n"
                  "1 STR period_ms\n"
                  "0 OPT ==\n"
                  "0 JEZ 1\n"
                  "B3\n"
                  "1 REF __calls\n"
                  "1 REF i\n"
                  "0 RUN __get__\n"
                  "0 OUT period_ms\n"
                  "B3\n"
                  "0 NUM 2\n"
                  "0 OUT info_index\n"
                  "B1\n"
                  "0 NEL 1\n"
                  "1 REF info_index\n"
                  "1 NUM 2\n"
                  "0 OPT ==\n"
                  "0 JEZ 1\n"
                  "B2\n"
                  "1 REF mode\n"
                  "1 STR when\n"
                  "0 OPT ==\n"
                  "0 JEZ 1\n"
                  "B3\n"
                  "0 RUN when\n"
                  "0 JEZ 1\n"
                  "B4\n"
                  "1 REF __calls\n"
                  "1 REF i\n"
                  "0 RUN __get__\n"
                  "0 OUT todo\n"
                  "B4\n"
                  "0 RUN todo\n"
                  "B3\n"
                  "0 NUM 0\n"
                  "0 OUT info_index\n"
                  "B2\n"
                  "0 NEL 1\n"
                  "1 REF mode\n"
                  "1 STR period_ms\n"
                  "0 OPT ==\n"
                  "0 JEZ 1\n"
                  "B3\n"
                  "1 REF __calls\n"
                  "1 REF i\n"
                  "0 RUN __get__\n"
                  "0 OUT todo\n"
                  "B3\n"
                  "0 NUM 3\n"
                  "0 OUT info_index\n"
                  "B1\n"
                  "0 NEL 1\n"
                  "1 REF info_index\n"
                  "1 NUM 3\n"
                  "0 OPT ==\n"
                  "0 JEZ 1\n"
                  "B2\n"
                  "1 REF mode\n"
                  "1 STR period_ms\n"
                  "0 OPT ==\n"
                  "0 JEZ 1\n"
                  "B3\n"
                  "1 REF __tick\n"
                  "2 REF __calls\n"
                  "2 REF i\n"
                  "1 RUN __get__\n"
                  "0 OPT >\n"
                  "0 JEZ 1\n"
                  "B4\n"
                  "0 RUN todo\n"
                  "B4\n"
                  "1 REF __calls\n"
                  "1 REF i\n"
                  "2 REF __tick\n"
                  "2 REF period_ms\n"
                  "1 OPT +\n"
                  "1 STR __calls\n"
                  "0 RUN __set__\n"
                  "B3\n"
                  "0 NUM 0\n"
                  "0 OUT info_index\n"
                  "B0\n"
                  "0 JMP -1\n"
                  "B0\n"
                  "0 DEL _l0\n"
                  "B0\n");
}

void __Task_update_tick(PikaObj* self) {
    if (obj_getInt(self, "is_perod")) {
        pikaVM_runAsm(self, "B0\n0 RUN platformGetTick\n");
    }
}

void PikaStdTask_Task_run_forever(PikaObj* self) {
    while (1) {
        __Task_update_tick(self);
        PikaStdTask_Task_run_once(self);
    }
}

void PikaStdTask_Task_run_until_ms(PikaObj* self, int until_ms) {
    while (1) {
        __Task_update_tick(self);
        PikaStdTask_Task_run_once(self);
        if (obj_getInt(self, "tick") > until_ms) {
            return;
        }
    }
}

void PikaStdTask_Task_platformGetTick(PikaObj* self) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}
