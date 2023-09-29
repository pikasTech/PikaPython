#include "PikaVM.h"
#include "dataStrs.h"

extern volatile PikaObj* __pikaMain;
static enum shellCTRL __obj_shellLineHandler_debug(PikaObj* self,
                                                   char* input_line,
                                                   struct ShellConfig* config) {
    /* continue */
    if (strEqu("c", input_line)) {
        return SHELL_CTRL_EXIT;
    }
    /* next */
    if (strEqu("n", input_line)) {
        return SHELL_CTRL_EXIT;
    }
    /* launch shell */
    if (strEqu("sh", input_line)) {
        /* exit pika shell */
        pikaScriptShell((PikaObj*)__pikaMain);
        return SHELL_CTRL_CONTINUE;
    }
    /* quit */
    if (strEqu("q", input_line)) {
        obj_setInt(self, "enable", 0);
        return SHELL_CTRL_EXIT;
    }
    /* print */
    if (strIsStartWith(input_line, "p ")) {
        char* path = input_line + 2;
        Arg* asm_buff = arg_newStr("print(");
        asm_buff = arg_strAppend(asm_buff, path);
        asm_buff = arg_strAppend(asm_buff, ")\n");
        pikaVM_run_ex_cfg cfg = {0};
        cfg.globals = config->globals;
        cfg.in_repl = pika_true;
        pikaVM_run_ex(config->locals, arg_getStr(asm_buff), &cfg);
        arg_deinit(asm_buff);
        return SHELL_CTRL_CONTINUE;
    }
    pikaVM_run_ex_cfg cfg = {0};
    cfg.globals = config->globals;
    cfg.in_repl = pika_true;
    pikaVM_run_ex(config->locals, input_line, &cfg);
    return SHELL_CTRL_CONTINUE;
}

void PikaDebug_Debuger___init__(PikaObj* self) {
    /* global enable contral */
    obj_setInt(self, "enable", 1);
}

void PikaDebug_Debuger_set_trace(PikaObj* self) {
    if (!obj_getInt(self, "enable")) {
        return;
    }
    char* name = "stdin";
    pika_assert(NULL != self->vmFrame);
    if (NULL != self->vmFrame->bytecode_frame->name) {
        name = self->vmFrame->bytecode_frame->name;
    }
    pika_platform_printf("%s:%d\n", name, self->vmFrame->pc);
    struct ShellConfig cfg = {
        .prefix = "(Pdb-pika) ",
        .handler = __obj_shellLineHandler_debug,
        .fn_getchar = __platform_getchar,
        .locals = self->vmFrame->locals,
        .globals = self->vmFrame->globals,
    };
    _do_pikaScriptShell(self, &cfg);
    shConfig_deinit(&cfg);
}

void PikaDebug_set_trace(PikaObj* self) {
    PikaDebug_Debuger_set_trace(self);
}

void PikaDebug_set_break(PikaObj* self, char* module, int pc_break) {
    pika_debug_set_break(module, pc_break);
}

void PikaDebug_reset_break(PikaObj* self, char* module, int pc_break) {
    pika_debug_reset_break(module, pc_break);
}
