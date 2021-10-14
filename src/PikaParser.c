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
    char* str = NULL;
    uint8_t directExist = 0, isMethod = 0, isRef = 0, isStr = 0;
    if (strIsContain(assignment, '=')) {
        directExist = 1;
    }
    if (strIsContain(stmt, '(') || strIsContain(stmt, ')')) {
        isMethod = 1;
        isRef = 0;
        isStr = 0;
    }
    if (!isMethod && (strIsContain(stmt, '\'') || strIsContain(stmt, '\"'))) {
        isMethod = 0;
        isRef = 0;
        isStr = 1;
    }
    if (!isMethod && !isStr) {
        isMethod = 0;
        isRef = 1;
        isStr = 0;
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
        goto exit;
    }
    if (isRef) {
        if (directExist) {
            ref = strsGetLastToken(buffs, assignment, '=');
        } else {
            ref = assignment;
        }
        obj_setStr(ast, (char*)"ref", ref);
        goto exit;
    }
    if (isStr) {
        if (directExist) {
            str = strsGetLastToken(buffs, assignment, '=');
        } else {
            str = assignment;
        }
        str = strsDeleteChar(buffs, str, '\'');
        str = strsDeleteChar(buffs, str, '\"');
        obj_setStr(ast, (char*)"str", str);
        goto exit;
    }
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

char* AST_appandPikaAsm(AST* ast, AST* subAst, Args* buffs, char* pikaAsm) {
    uint32_t deepth = obj_getInt(ast, "deepth");
    while (1) {
        QueueObj* subStmt = queueObj_popObj(subAst);
        if (NULL == subStmt) {
            break;
        }
        obj_setInt(ast, "deepth", deepth + 1);
        pikaAsm = AST_appandPikaAsm(ast, subStmt, buffs, pikaAsm);
    }
    char* method = obj_getStr(subAst, "method");
    char* ref = obj_getStr(subAst, "ref");
    char* direct = obj_getStr(subAst, "direct");
    char* str = obj_getStr(subAst, "str");
    if (NULL != ref) {
        char buff[32] = {0};
        sprintf(buff, "%d REF %s\n", deepth, ref);
        pikaAsm = strsAppend(buffs, pikaAsm, buff);
    }
    if (NULL != method) {
        char buff[32] = {0};
        sprintf(buff, "%d RUN %s\n", deepth, method);
        pikaAsm = strsAppend(buffs, pikaAsm, buff);
    }
    if (NULL != str) {
        char buff[32] = {0};
        sprintf(buff, "%d STR %s\n", deepth, str);
        pikaAsm = strsAppend(buffs, pikaAsm, buff);
    }
    if (NULL != direct) {
        char buff[32] = {0};
        sprintf(buff, "%d OUT %s\n", deepth, direct);
        pikaAsm = strsAppend(buffs, pikaAsm, buff);
    }
    obj_setInt(ast, "deepth", deepth - 1);
    return pikaAsm;
}

char* AST_toPikaAsm(AST* ast, Args* buffs) {
    char* pikaAsm = strsCopy(buffs, "");
    obj_setInt(ast, "deepth", 0);
    return AST_appandPikaAsm(ast, ast, buffs, pikaAsm);
}

int32_t AST_deinit(AST* ast) {
    return obj_deinit(ast);
}