#include "PikaParser.h"
#include "PikaObj.h"
#include "dataQueueObj.h"
#include "dataStrs.h"

AST* pikaParse(char* line) {
    AST* ast = New_queueObj();
    Args* buffs = New_strBuff();
    char* stmt = strsGetCleanCmd(buffs, line);
    char* assignment = strsGetFirstToken(buffs, stmt, '(');
    char* direct = NULL;
    char* method = NULL;
    if (strIsContain(assignment, '=')) {
        direct = strsGetFirstToken(buffs, assignment, '=');
        method = strGetLastToken(buffs, assignment, '=');
    }
    method = assignment;
    char* subStmts = strsCut(buffs, stmt, '(', ')');

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