#include "PikaParser.h"
#include "PikaObj.h"
#include "dataQueueObj.h"
#include "dataStrs.h"

AST* pikaParse(char* line) {
    AST* ast = New_queueObj();
    Args* buffs = New_strBuff();
    char* cmd = strsGetCleanCmd(buffs, line);
    char* assignment = strsGetFirstToken(buffs, cmd, '(');
    char* direct = NULL;
    if (strIsContain(assignment, '=')) {
        direct = strsGetFirstToken(buffs, assignment, '=');
    }

    goto exit;
exit:
    args_deinit(buffs);
    return ast;
}

char* AST_toShell(AST* ast, Args* buffs) {
    return NULL;
}

int32_t AST_deinit(AST* ast) {
    return obj_deinit(ast);
}