#include "PikaDebug_Debuger.h"

static enum shell_state __obj_shellLineHandler_obj_run(PikaObj *self, char *input_line){
    /* exit */
    if (strEqu("exit()", input_line)) {
        /* exit pika shell */
        return SHELL_STATE_EXIT;
    }
    /* run single line */
    obj_run(self, input_line);
    return SHELL_STATE_CONTINUE;
}

extern PikaObj* __pikaMain;
void PikaDebug_Debuger_set_trace(PikaObj* self) {
    obj_shellLineProcess(__pikaMain, __obj_shellLineHandler_obj_run);
}
