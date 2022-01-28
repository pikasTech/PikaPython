#include "PikaDebug_Debuger.h"

extern PikaObj* __pikaMain;
static enum shell_state __obj_shellLineHandler_obj_run(PikaObj* self,
                                                       char* input_line) {
    /* continue */
    if (strEqu("c", input_line)) {
        /* exit pika shell */
        return SHELL_STATE_EXIT;
    }
    if (strEqu("sh", input_line)) {
        /* exit pika shell */
        pikaScriptShell(__pikaMain);
        return SHELL_STATE_CONTINUE;
    }
}

void PikaDebug_Debuger_set_trace(PikaObj* self) {
    struct shell_config cfg = {
        .prefix = "(pika-debug) ",
    };
    obj_shellLineProcess(__pikaMain, __obj_shellLineHandler_obj_run, &cfg);
}
