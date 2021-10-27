#include "PikaParser.h"
#include "BaseObj.h"
#include "PikaObj.h"
#include "dataQueue.h"
#include "dataQueueObj.h"
#include "dataStack.h"
#include "dataStrs.h"

char* strsPopTokenWithSkip(Args* buffs,
                           char* stmts,
                           char sign,
                           char skipStart,
                           char skipEnd) {
    int32_t size = strGetSize(stmts);
    if (0 == size) {
        return NULL;
    }
    char* strOut = args_getBuff(buffs, size);
    int32_t iOut = 0;
    int32_t stmtEnd = 0;
    uint8_t isGetSign = 0;
    int32_t parentheseDeepth = 0;
    for (int32_t i = 0; i < size; i++) {
        if (skipStart == stmts[i]) {
            parentheseDeepth++;
        }
        if (skipEnd == stmts[i]) {
            parentheseDeepth--;
        }
        if (parentheseDeepth == 0) {
            if (sign == stmts[i]) {
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

enum StmtType {
    REF,
    STR,
    NUM,
    METHOD,
    OPERATOR,
    NONE,
};

char* strs_deleteBetween(Args* buffs, char* strIn, char begin, char end) {
    int32_t size = strGetSize(strIn);
    char* strOut = args_getBuff(buffs, size);
    uint8_t deepth = 0;
    uint32_t iOut = 0;
    for (int i = 0; i < size; i++) {
        if (end == strIn[i]) {
            deepth--;
        }
        if (0 == deepth) {
            strOut[iOut] = strIn[i];
            iOut++;
        }
        if (begin == strIn[i]) {
            deepth++;
        }
    }
    strOut[iOut] = 0;
    return strOut;
}

uint8_t checkIsEqu(char* str) {
    uint32_t size = strGetSize(str) + 1;
    for (int i = 0; i + 1 < size; i++) {
        if (str[i] == '=' && str[i + 1] == '=') {
            return 1;
        }
    }
    return 0;
}

static enum StmtType matchStmtType(char* right) {
    Args* buffs = New_strBuff();
    enum StmtType stmtType = NONE;
    char* rightWithoutSubStmt = strs_deleteBetween(buffs, right, '(', ')');
    if (strIsContain(rightWithoutSubStmt, '+') ||
        strIsContain(rightWithoutSubStmt, '-') ||
        strIsContain(rightWithoutSubStmt, '*') ||
        strIsContain(rightWithoutSubStmt, '<') ||
        strIsContain(rightWithoutSubStmt, '>') ||
        checkIsEqu(rightWithoutSubStmt) ||
        strIsContain(rightWithoutSubStmt, '/')) {
        stmtType = OPERATOR;
        goto exit;
    }
    if (strIsContain(rightWithoutSubStmt, '(') ||
        strIsContain(rightWithoutSubStmt, ')')) {
        stmtType = METHOD;
        goto exit;
    }
    if (strIsContain(rightWithoutSubStmt, '\'') ||
        strIsContain(rightWithoutSubStmt, '\"')) {
        stmtType = STR;
        goto exit;
    }
    if (rightWithoutSubStmt[0] >= '0' && rightWithoutSubStmt[0] <= '9') {
        stmtType = NUM;
        goto exit;
    }
    if (!strEqu(rightWithoutSubStmt, "")) {
        stmtType = REF;
        goto exit;
    }
exit:
    args_deinit(buffs);
    return stmtType;
}

uint8_t checkIsDirect(char* str) {
    /* include '0' */
    uint32_t size = strGetSize(str) + 1;
    for (int i = 1; i + 1 < size; i++) {
        if ((str[i - 1] != '=') && (str[i] == '=') && (str[i + 1] != '=')) {
            return 1;
        }
    }
    return 0;
}

AST* AST_parseStmt(AST* ast, char* stmt) {
    Args* buffs = New_strBuff();
    char* assignment = strsGetFirstToken(buffs, stmt, '(');
    char* direct = NULL;
    char* method = NULL;
    char* ref = NULL;
    char* str = NULL;
    char* num = NULL;
    char* right = NULL;
    /* solve direct */
    uint8_t directExist = 0;
    if (checkIsDirect(assignment)) {
        directExist = 1;
    }
    if (directExist) {
        direct = strsGetFirstToken(buffs, assignment, '=');
        obj_setStr(ast, (char*)"direct", direct);
    }
    /* solve right stmt */
    if (directExist) {
        right = strsGetLastToken(buffs, stmt, '=');
    } else {
        right = stmt;
    }
    enum StmtType stmtType = matchStmtType(right);
    /* solve method stmt */
    if (OPERATOR == stmtType) {
        char* rightWithoutSubStmt = strs_deleteBetween(buffs, right, '(', ')');
        char operator[2] = {0};
        if (strIsContain(rightWithoutSubStmt, '*')) {
            operator[0] = '*';
        }
        if (strIsContain(rightWithoutSubStmt, '/')) {
            operator[0] = '/';
        }
        if (strIsContain(rightWithoutSubStmt, '+')) {
            operator[0] = '+';
        }
        if (strIsContain(rightWithoutSubStmt, '-')) {
            operator[0] = '-';
        }
        if (strIsContain(rightWithoutSubStmt, '<')) {
            operator[0] = '<';
        }
        if (strIsContain(rightWithoutSubStmt, '>')) {
            operator[0] = '>';
        }
        if (checkIsEqu(rightWithoutSubStmt)) {
            operator[0] = '=';
            operator[1] = '=';
        }
        obj_setStr(ast, (char*)"operator", operator);
        char* rightBuff = strsCopy(buffs, right);
        char* subStmt1 =
            strsPopTokenWithSkip(buffs, rightBuff, operator[0], '(', ')');
        char* subStmt2 = rightBuff;
        if (operator[1] == '=') {
            subStmt2 = rightBuff + 1;
        }
        queueObj_pushObj(ast, (char*)"stmt");
        AST_parseStmt(queueObj_getCurrentObj(ast), subStmt1);
        queueObj_pushObj(ast, (char*)"stmt");
        AST_parseStmt(queueObj_getCurrentObj(ast), subStmt2);
        goto exit;
    }
    if (METHOD == stmtType) {
        method = strsGetFirstToken(buffs, right, '(');
        obj_setStr(ast, (char*)"method", method);
        char* subStmts = strsCut(buffs, stmt, '(', ')');
        while (1) {
            char* subStmt =
                strsPopTokenWithSkip(buffs, subStmts, ',', '(', ')');
            if (NULL == subStmt) {
                break;
            }
            queueObj_pushObj(ast, (char*)"stmt");
            AST_parseStmt(queueObj_getCurrentObj(ast), subStmt);
        }
        goto exit;
    }
    /* solve reference stmt */
    if (REF == stmtType) {
        ref = right;
        obj_setStr(ast, (char*)"ref", ref);
        goto exit;
    }
    /* solve str stmt */
    if (STR == stmtType) {
        str = right;
        str = strsDeleteChar(buffs, str, '\'');
        str = strsDeleteChar(buffs, str, '\"');
        obj_setStr(ast, (char*)"str", str);
        goto exit;
    }
    /* solve number stmt */
    if (NUM == stmtType) {
        num = right;
        obj_setStr(ast, (char*)"num", num);
        goto exit;
    }
exit:
    args_deinit(buffs);
    return ast;
}

static int32_t getPyLineBlockDeepth(char* line) {
    uint32_t size = strGetSize(line);
    for (int i = 0; i < size; i++) {
        if (line[i] != ' ') {
            uint32_t spaceNum = i;
            if (0 == spaceNum % 4) {
                return spaceNum / 4;
            }
            /* space Num is not 4N, error*/
            return -1;
        }
    }
    return 0;
}

AST* pikaParseLine(char* line, Stack* blockStack) {
    AST* ast = New_queueObj();
    Args* buffs = New_strBuff();
    line = strsDeleteChar(buffs, line, '\r');
    uint8_t blockDeepth = getPyLineBlockDeepth(line);
    uint8_t blockDeepthLast = blockDeepth;
    if (NULL != blockStack) {
        blockDeepthLast = args_getInt(blockStack, "top");
        for (int i = 0; i < blockDeepthLast - blockDeepth; i++) {
            QueueObj* exitBlock = obj_getObj(ast, "exitBlock", 0);
            if (NULL == exitBlock) {
                obj_newObj(ast, "exitBlock", "", New_TinyObj);
                exitBlock = obj_getObj(ast, "exitBlock", 0);
                queueObj_init(exitBlock);
            }
            char buff[10] = {0};
            char* blockType = stack_popStr(blockStack, buff);
            queueObj_pushStr(exitBlock, blockType);
        }
    }
    obj_setInt(ast, "blockDeepth", blockDeepth);
    char* lineStart = line + blockDeepth * 4;
    char* stmt = lineStart;
    if (0 == strncmp(lineStart, (char*)"while ", 6)) {
        stmt = strsCut(buffs, lineStart, ' ', ':');
        obj_setStr(ast, "contralFlow", "while");
        if (NULL != blockStack) {
            stack_pushStr(blockStack, "while");
        }
    }
    if (0 == strncmp(lineStart, (char*)"if ", 3)) {
        stmt = strsCut(buffs, lineStart, ' ', ':');
        obj_setStr(ast, "contralFlow", "if");
        if (NULL != blockStack) {
            stack_pushStr(blockStack, "if");
        }
    }
    stmt = strsGetCleanCmd(buffs, stmt);
    ast = AST_parseStmt(ast, stmt);
    goto exit;
exit:
    args_deinit(buffs);
    return ast;
}

char* pikaParseLineToAsm(Args* buffs, char* line, Stack* blockStack) {
    AST* ast = pikaParseLine(line, blockStack);
    char* pikaAsm = AST_toPikaAsm(ast, buffs);
    AST_deinit(ast);
    return pikaAsm;
}

char* pikaParseMultiLineToAsm(Args* outBuffs, char* multiLine) {
    Stack* blockStack = New_Stack();
    Arg* pikaAsmBuff = arg_setStr(NULL, "", "");
    uint32_t lineOffset = 0;
    uint32_t multiLineSize = strGetSize(multiLine);
    uint8_t isSave = 0;
    if( strCountSign(multiLine, '\n') > 1){
        isSave = 1;
    }
    while (1) {
        Args* singleRunBuffs = New_strBuff();
        char* line = strsGetFirstToken(singleRunBuffs, multiLine + lineOffset, '\n');
        uint32_t lineSize = strGetSize(line);
        lineOffset = lineOffset + lineSize + 1;
        char* singleAsm = pikaParseLineToAsm(singleRunBuffs, line, blockStack);
        char* pikaAsm = arg_getStr(pikaAsmBuff);
        pikaAsm = strsAppend(singleRunBuffs, pikaAsm, singleAsm);
        arg_deinit(pikaAsmBuff);
        pikaAsmBuff = arg_setStr(NULL, "", pikaAsm);
        args_deinit(singleRunBuffs);
        if (lineOffset >= multiLineSize) {
            break;
        }
    }
    char* multiAsm = strsCopy(outBuffs, arg_getStr(pikaAsmBuff));
    arg_deinit(pikaAsmBuff);
    stack_deinit(blockStack);
    return multiAsm;
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
    char* operator= obj_getStr(subAst, "operator");
    char* ref = obj_getStr(subAst, "ref");
    char* direct = obj_getStr(subAst, "direct");
    char* str = obj_getStr(subAst, "str");
    char* num = obj_getStr(subAst, "num");
    if (NULL != ref) {
        char buff[32] = {0};
        sprintf(buff, "%d REF %s\n", deepth, ref);
        pikaAsm = strsAppend(buffs, pikaAsm, buff);
    }
    if (NULL != operator) {
        char buff[32] = {0};
        sprintf(buff, "%d OPT %s\n", deepth, operator);
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
    if (NULL != num) {
        char buff[32] = {0};
        sprintf(buff, "%d NUM %s\n", deepth, num);
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

static char* addBlockDeepth(AST* ast,
                            Args* buffs,
                            char* pikaAsm,
                            uint8_t deepthOffset) {
    pikaAsm = strsAppend(buffs, pikaAsm, (char*)"B");
    char buff[11];
    pikaAsm = strsAppend(
        buffs, pikaAsm,
        fast_itoa(buff, obj_getInt(ast, "blockDeepth") + deepthOffset));
    pikaAsm = strsAppend(buffs, pikaAsm, (char*)"\n");
    return pikaAsm;
}

char* AST_toPikaAsm(AST* ast, Args* buffs) {
    Args* runBuffs = New_strBuff();
    char* pikaAsm = strsCopy(runBuffs, "");
    QueueObj* exitBlock = obj_getObj(ast, "exitBlock", 0);
    if (NULL != exitBlock) {
        while (1) {
            uint8_t blockTypeNum = obj_getInt(exitBlock, "top") -
                                   obj_getInt(exitBlock, "bottom") - 1;
            char* blockType = queueObj_popStr(exitBlock);
            if (NULL == blockType) {
                break;
            }
            /* goto the while start when exit while block */
            if (strEqu(blockType, "while")) {
                pikaAsm = addBlockDeepth(ast, buffs, pikaAsm, blockTypeNum);
                pikaAsm = strsAppend(buffs, pikaAsm, (char*)"0 JMP -1\n");
            }
        }
    }
    pikaAsm = addBlockDeepth(ast, buffs, pikaAsm, 0);
    obj_setInt(ast, "deepth", 0);

    /* parse ast to asm main process */
    pikaAsm = AST_appandPikaAsm(ast, ast, runBuffs, pikaAsm);

    if (strEqu(obj_getStr(ast, "contralFlow"), "while")) {
        pikaAsm = strsAppend(runBuffs, pikaAsm, "0 JEZ 2\n");
    }
    if (strEqu(obj_getStr(ast, "contralFlow"), "if")) {
        pikaAsm = strsAppend(runBuffs, pikaAsm, "0 JEZ 1\n");
    }
    pikaAsm = strsCopy(buffs, pikaAsm);
    args_deinit(runBuffs);
    return pikaAsm;
}

int32_t AST_deinit(AST* ast) {
    return obj_deinit(ast);
}