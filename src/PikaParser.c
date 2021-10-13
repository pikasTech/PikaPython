#include "PikaParser.h"
#include "PikaObj.h"
#include "dataQueueObj.h"
#include "dataStrs.h"

char* strsPopStmts(Args* buffs, char* stmts) {
    int32_t size = strGetSize(stmts);
    if (0 == size) {
        return NULL;
    }
    char* strOut = args_getBuff(buffs, size);
    int32_t iOut = 0;
    char delChar = ' ';
    int32_t stmtEnd = 0;
    uint8_t isGetSign = 0;
    int32_t parentheseDeepth = 0;
    for (int32_t i = 0; i < size; i++) {
        if ('(' == stmts[i]) {
            parentheseDeepth++;
        }
        if (')' == stmts[i]) {
            parentheseDeepth--;
        }
        if (parentheseDeepth == 0) {
            if (',' == stmts[i]) {
                stmtEnd = i;
                isGetSign = 1;
                break;
            }
        }
    }
    if (!isGetSign) {
        stmtEnd = size;
    }
    for (int32_t i = 0; i < stmtEnd; i++) {
        strOut[i] = stmts[i];
    }
    memmove(stmts, stmts + stmtEnd + 1, size);
    strOut[stmtEnd] = 0;
    return strOut;
}

AST* AST_parseStmt(AST* ast, char* stmt) {
    Args* buffs = New_strBuff();
    char* assignment = strsGetFirstToken(buffs, stmt, '(');
    char* direct = NULL;
    char* method = NULL;
    if (strIsContain(assignment, '=')) {
        direct = strsGetFirstToken(buffs, assignment, '=');
        obj_setStr(ast, (char*)"direct", direct);
        method = strsGetLastToken(buffs, assignment, '=');
    }
    method = assignment;
    obj_setStr(ast, (char*)"method", method);
    char* subStmts = strsCut(buffs, stmt, '(', ')');
    while (1) {
        char* subStmt = strsPopStmts(buffs, subStmts);
        if (NULL == subStmt) {
            break;
        }
        queueObj_pushObj(ast, (char*)"stmt");
        obj_setStr(queueObj_getCurrentObj(ast), (char*)"nonTerminal", subStmt);
    }

    goto exit;
exit:
    args_deinit(buffs);
    return ast;
}

AST* pikaParse(char* line) {
    AST* ast = New_queueObj();
    Args* buffs = New_strBuff();
    char* stmt = strsGetCleanCmd(buffs, line);
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