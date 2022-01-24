#include "PikaStdTask_Task.h"
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
}

void __Task_update_tick(PikaObj* self) {
    obj_run(self,
            "if is_period:\n"
            "    platformGetTick()\n");
}

void PikaStdTask_Task_run_always(PikaObj* self) {
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
