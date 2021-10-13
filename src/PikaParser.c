#include "PikaParser.h"
#include "PikaObj.h"
#include "dataQueueObj.h"
#include "dataStrs.h"

AST* AST_parseStmt(AST* ast, char* stmt) {
    Args* buffs = New_strBuff();
    char* assignment = strsGetFirstToken(buffs, stmt, '(');
    char* direct = NULL;
    char* method = NULL;
    if (strIsContain(assignment, '=')) {
        direct = strsGetFirstToken(buffs, assignment, '=');
        obj_setStr(ast, "direct", direct);
        method = strsGetLastToken(buffs, assignment, '=');
    }
    method = assignment;
    obj_setStr(ast, "method", method);
    char* subStmts = strsCut(buffs, stmt, '(', ')');

    goto exit;
exit:
    args_deinit(buffs);
    return ast;
}

AST* pikaParse(char* line) {
    AST* ast = New_queueObj();
    Args* buffs = New_strBuff();
    char* stmt = strsGetCleanCmd(buffs, stmt);
    ast = AST_parseStmt(ast, stmt);
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