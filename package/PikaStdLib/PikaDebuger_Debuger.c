#include "PikaVM.h"
#include "dataStrs.h"

extern PikaObj* __pikaMain;
static enum shell_state __obj_shellLineHandler_debuger(PikaObj* self,
                                                       char* input_line) {
    /* continue */
    if (strEqu("c", input_line)) {
        return SHELL_STATE_EXIT;
    }
    /* next */
    if (strEqu("n", input_line)) {
        return SHELL_STATE_EXIT;
    }
    /* launch shell */
    if (strEqu("sh", input_line)) {
        /* exit pika shell */
        pikaScriptShell(__pikaMain);
        return SHELL_STATE_CONTINUE;
    }
    /* quit */
    if (strEqu("q", input_line)) {
        obj_setInt(self, "enable", 0);
        return SHELL_STATE_EXIT;
    }
    /* print */
    if (strIsStartWith(input_line, "p ")) {
        char* path = input_line + 2;
        Arg* asm_buff = arg_setStr(NULL, "", "B0\n1 REF ");
        asm_buff = arg_strAppend(asm_buff, path);
        asm_buff = arg_strAppend(asm_buff, "\n0 RUN print\n");
        pikaVM_runAsm(__pikaMain, arg_getStr(asm_buff));
        arg_deinit(asm_buff);
        return SHELL_STATE_CONTINUE;
    }
    obj_run(__pikaMain, input_line);
    return SHELL_STATE_CONTINUE;
}

void PikaDebug_Debuger___init__(PikaObj* self) {
    /* global enable contral */
    obj_setInt(self, "enable", 1);
}

void PikaDebug_Debuger_set_trace(PikaObj* self) {
    if (!obj_getInt(self, "enable")) {
        return;
    }
    struct shell_config cfg = {
        .prefix = "(pika-debug) ",
    };
    obj_shellLineProcess(self, __obj_shellLineHandler_debuger, &cfg);
}
