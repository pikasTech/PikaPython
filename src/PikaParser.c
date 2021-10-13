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
    char* ref = NULL;
    uint8_t directExist = 0;
    uint8_t isMethod = 0;
    if (strIsContain(assignment, '=')) {
        directExist = 1;
    }
    if (strIsContain(stmt, '(') || strIsContain(stmt, ')')) {
        isMethod = 1;
    }
    if (directExist) {
        direct = strsGetFirstToken(buffs, assignment, '=');
        obj_setStr(ast, (char*)"direct", direct);
    }
    char* subStmts = NULL;
    if (isMethod) {
        if (directExist) {
            method = strsGetLastToken(buffs, assignment, '=');
        } else {
            method = assignment;
        }
        obj_setStr(ast, (char*)"method", method);
        subStmts = strsCut(buffs, stmt, '(', ')');
        while (1) {
            char* subStmt = strsPopStmts(buffs, subStmts);
            if (NULL == subStmt) {
                break;
            }
            queueObj_pushObj(ast, (char*)"stmt");
            AST_parseStmt(queueObj_getCurrentObj(ast), subStmt);
        }
    } else {
        if (directExist) {
            ref = strsGetLastToken(buffs, assignment, '=');
        } else {
            ref = assignment;
        }
        obj_setStr(ast, (char*)"ref", ref);
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
uint32_t deepth;
char* AST_appandShell(AST* ast, Args* buffs, char* sh) {
    while (1) {
        QueueObj* subStmt = queueObj_popObj(ast);
        if (NULL == subStmt) {
            break;
        }
        deepth++;
        sh = AST_appandShell(subStmt, buffs, sh);
    }
    char* method = obj_getStr(ast, "method");
    char* ref = obj_getStr(ast, "ref");
    char* direct = obj_getStr(ast, "direct");
    if (NULL != ref) {
        char buff[32] = {0};
        sprintf(buff, "%d REF %s\n", deepth, ref);
        sh = strsAppend(buffs, sh, buff);
    }
    if (NULL != method) {
        char buff[32] = {0};
        sprintf(buff, "%d RUN %s\n", deepth, method);
        sh = strsAppend(buffs, sh, buff);
    }
    if (NULL != direct) {
        char buff[32] = {0};
        sprintf(buff, "%d OUT %s\n", deepth, direct);
        sh = strsAppend(buffs, sh, buff);
    }
    deepth--;
    return sh;
}

char* AST_toShell(AST* ast, Args* buffs) {
    char* sh = strsCopy(buffs, "");
    deepth = 0;
    return AST_appandShell(ast, buffs, sh);
}

int32_t AST_deinit(AST* ast) {
    return obj_deinit(ast);
}