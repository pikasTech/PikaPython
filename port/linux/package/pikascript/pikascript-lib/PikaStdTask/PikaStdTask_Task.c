#include "PikaStdTask_Task.h"
#include "PikaVM.h"

extern PikaObj* __pikaMain;
void PikaStdTask_Task___init__(PikaObj* self) {
    obj_run(self, "task_list.__init__()");
}
void PikaStdTask_Task_call_always(PikaObj* self, Arg* fun_todo) {
    obj_setArg(self, "fun_todo", fun_todo);
    obj_run(self, "task_list.append(fun_todo)");
    // pikaVM_runAsm(__pikaMain, fun_todo_ptr);
}
void PikaStdTask_Task_do_when(PikaObj* self, Arg* fun_todo, Arg* fun_when) {}

void PikaStdTask_Task_run_once(PikaObj* self) {
    /* reference the task_list in __pikaMain */
    obj_setPtr(__pikaMain, "__task_list", obj_getPtr(self, "task_list"));
    obj_run(__pikaMain, 
            "for fun_todo in __task_list :\n"
            "    fun_todo()\n"
            "\n"
    );
}

void PikaStdTask_Task_run_always(PikaObj *self){
    while (1){
        PikaStdTask_Task_run_once(self);
    }
}
