/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "PikaParser.h"
#include "BaseObj.h"
#include "PikaObj.h"
#include "dataQueue.h"
#include "dataQueueObj.h"
#include "dataStack.h"
#include "dataStrs.h"

/* local head */
char* AST_genAsm_top(AST* ast, Args* outBuffs);
int32_t AST_deinit(AST* ast);

uint8_t TokenStream_isContain(char* tokenStream,
                              enum TokenType token_type,
                              char* pyload);
char* TokenStream_pop(Args* buffs_p, char** tokenStream);
char* parser_lines2Target(Parser* self, char* sPyLines);

/* Cursor preivilage */
void _Cursor_init(struct Cursor* cs);
void _Cursor_parse(struct Cursor* cs, char* stmt);
void _Cursor_beforeIter(struct Cursor* cs);

/* Cursor iter api */
void Cursor_iterStart(struct Cursor* cs);
void Cursor_iterEnd(struct Cursor* cs);
void Cursor_deinit(struct Cursor* cs);

/* Cursor high level api */
char* Cursor_popToken(Args* buffs, char** pStmt, char* devide);
pika_bool Cursor_isContain(char* stmt, TokenType type, char* pyload);
char* Cursor_splitCollect(Args* buffs, char* stmt, char* devide, int index);

char* pika_lines2Asm(Args* outBuffs, char* multiLine);
uint16_t TokenStream_getSize(char* tokenStream) {
    if (strEqu("", tokenStream)) {
        return 0;
    }
    return strCountSign(tokenStream, 0x1F) + 1;
}

char* Cursor_popLastToken(Args* outBuffs, char** pStmt, char* str) {
    char* sStmts = *pStmt;
    uint8_t uDividerIndex = 0;
    Arg* aKeeped = arg_newStr("");
    Arg* aPoped = arg_newStr("");
    Cursor_forEach(cs, sStmts) {
        Cursor_iterStart(&cs);
        if (cs.bracket_deepth == 0) {
            if (strEqu(str, cs.token1.pyload)) {
                uDividerIndex = cs.iter_index;
            }
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    Cursor_forEachExistPs(cs, sStmts) {
        Cursor_iterStart(&cs);
        if (cs.iter_index < uDividerIndex) {
            aPoped = arg_strAppend(aPoped, cs.token1.pyload);
        }
        if (cs.iter_index > uDividerIndex) {
            aKeeped = arg_strAppend(aKeeped, cs.token1.pyload);
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    char* sKeeped = strsCopy(outBuffs, arg_getStr(aKeeped));
    char* sPoped = arg_getStr(aPoped);
    pika_platform_memcpy(sStmts, sPoped, strGetSize(sPoped) + 1);
    arg_deinit(aPoped);
    arg_deinit(aKeeped);
    return sKeeped;
}

char* Cursor_getCleanStmt(Args* outBuffs, char* cmd) {
    pika_assert(cmd != NULL);
    int32_t iSize = strGetSize(cmd);
    /* lexer may generate more chars than input */
    char* sOut = args_getBuff(outBuffs, iSize * 2);
    int32_t iOut = 0;
    Cursor_forEach(cs, cmd) {
        Cursor_iterStart(&cs);
        for (uint16_t k = 0; k < strGetSize(cs.token1.pyload); k++) {
            sOut[iOut] = cs.token1.pyload[k];
            iOut++;
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    /* add \0 */
    sOut[iOut] = 0;
    return sOut;
}

static uint8_t Lexer_isError(char* line) {
    Args buffs = {0};
    uint8_t uRes = 0; /* not error */
    char* sTokenStream = Lexer_getTokenStream(&buffs, line);
    if (NULL == sTokenStream) {
        uRes = 1; /* lex error */
        goto __exit;
    }
    goto __exit;
__exit:
    strsDeinit(&buffs);
    return uRes;
}

static char* Cursor_removeTokensBetween(Args* outBuffs,
                                        char* input,
                                        char* token_pyload1,
                                        char* token_pyload2) {
    Args buffs = {0};
    uint8_t uBlockDeepth = 0;
    char* sOutput = "";
    Cursor_forEach(cs, input) {
        Cursor_iterStart(&cs);
        if (strEqu(token_pyload1, cs.token1.pyload)) {
            if (uBlockDeepth == 0) {
                sOutput = strsAppend(&buffs, sOutput, cs.token1.pyload);
            }
            uBlockDeepth++;
        }
        if (strEqu(token_pyload2, cs.token1.pyload)) {
            uBlockDeepth--;
        }
        if (uBlockDeepth == 0) {
            sOutput = strsAppend(&buffs, sOutput, cs.token1.pyload);
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    sOutput = strsCopy(outBuffs, sOutput);
    strsDeinit(&buffs);
    return sOutput;
}

char* _remove_sub_stmt(Args* outBuffs, char* sStmt) {
    Args buffs = {0};
    sStmt = strsCopy(&buffs, sStmt);
    sStmt = Cursor_removeTokensBetween(&buffs, sStmt, "(", ")");
    sStmt = Cursor_removeTokensBetween(&buffs, sStmt, "[", "]");
    sStmt = Cursor_removeTokensBetween(&buffs, sStmt, "{", "}");
    char* sRes = args_cacheStr(outBuffs, sStmt);
    strsDeinit(&buffs);
    return sRes;
}

static enum StmtType Lexer_matchStmtType(char* right) {
    Args buffs = {0};
    enum StmtType eStmtType = STMT_none;
    char* sTopStmt = _remove_sub_stmt(&buffs, right);

    pika_bool bOperator = pika_false;
    pika_bool bMethod = pika_false;
    pika_bool bString = pika_false;
    pika_bool bBytes = pika_false;
    pika_bool bNumber = pika_false;
    pika_bool bSymbol = pika_false;
    pika_bool bList = pika_false;
    pika_bool bSlice = pika_false;
    pika_bool bDict = pika_false;
    pika_bool bImport = pika_false;
    pika_bool bInhert = pika_false;
    pika_bool bChain = pika_false;
    Cursor_forEach(cs, sTopStmt) {
        Cursor_iterStart(&cs);
        /* collect type */
        if (strEqu(cs.token1.pyload, " import ")) {
            bImport = pika_true;
            goto __iter_continue;
        }
        if (strEqu(cs.token1.pyload, "@inh ")) {
            bInhert = pika_true;
            goto __iter_continue;
        }
        if (strEqu(cs.token2.pyload, "[")) {
            /* (symble | iteral | <]> | <)>) + <[> */
            if (TOKEN_symbol == cs.token1.type ||
                TOKEN_literal == cs.token1.type ||
                strEqu(cs.token1.pyload, "]") ||
                strEqu(cs.token1.pyload, ")")) {
                /* keep the last one of the chain or slice */
                bSlice = pika_true;
                bChain = pika_false;
                goto __iter_continue;
            }
            /* ( <,> | <=> ) + <[> */
            bList = pika_true;
        }
        if (strEqu(cs.token1.pyload, "[") && cs.iter_index == 1) {
            /* VOID + <[> */
            bList = pika_true;
            bMethod = pika_false;
            goto __iter_continue;
        }
        if (strEqu(cs.token1.pyload, "...")) {
            goto __iter_continue;
        }

        if (strEqu(cs.token1.pyload, "pass")) {
            goto __iter_continue;
        }

        if (strIsStartWith(cs.token1.pyload, ".")) {
            if (cs.iter_index != 1) {
                /* keep the last one of the chain or slice */
                bChain = pika_true;
                bSlice = pika_false;
                goto __iter_continue;
            }
        }
        if (strEqu(cs.token1.pyload, "{")) {
            bDict = pika_true;
            goto __iter_continue;
        }
        if (cs.token1.type == TOKEN_operator) {
            bOperator = pika_true;
            goto __iter_continue;
        }
        /* <(> */
        if (strEqu(cs.token1.pyload, "(")) {
            bMethod = pika_true;
            bSlice = pika_false;
            goto __iter_continue;
        }
        if (cs.token1.type == TOKEN_literal) {
            if (cs.token1.pyload[0] == '\'' || cs.token1.pyload[0] == '"') {
                bString = pika_true;
                goto __iter_continue;
            }
            if (cs.token1.pyload[1] == '\'' || cs.token1.pyload[1] == '"') {
                if (cs.token1.pyload[0] == 'b') {
                    bBytes = pika_true;
                    goto __iter_continue;
                }
            }
            bNumber = pika_true;
            goto __iter_continue;
        }
        if (cs.token1.type == TOKEN_symbol) {
            bSymbol = pika_true;
            goto __iter_continue;
        }
    __iter_continue:
        Cursor_iterEnd(&cs);
    }
    if (bInhert) {
        eStmtType = STMT_inhert;
        goto __exit;
    }
    if (bImport) {
        eStmtType = STMT_import;
        goto __exit;
    }
    if (bOperator) {
        eStmtType = STMT_operator;
        goto __exit;
    }
    if (bSlice) {
        eStmtType = STMT_slice;
        goto __exit;
    }
    if (bChain) {
        eStmtType = STMT_chain;
        goto __exit;
    }
    if (bList) {
        eStmtType = STMT_list;
        goto __exit;
    }
    if (bDict) {
        eStmtType = STMT_dict;
        goto __exit;
    }
    if (bMethod) {
        eStmtType = STMT_method;
        goto __exit;
    }
    if (bString) {
        /* support multi assign */
        if (Cursor_isContain(right, TOKEN_devider, ",")) {
            eStmtType = STMT_tuple;
            goto __exit;
        }
        eStmtType = STMT_string;
        goto __exit;
    }
    if (bBytes) {
        /* support multi assign */
        if (Cursor_isContain(right, TOKEN_devider, ",")) {
            eStmtType = STMT_tuple;
            goto __exit;
        }
        eStmtType = STMT_bytes;
        goto __exit;
    }
    if (bNumber) {
        /* support multi assign */
        if (Cursor_isContain(right, TOKEN_devider, ",")) {
            eStmtType = STMT_tuple;
            goto __exit;
        }
        eStmtType = STMT_number;
        goto __exit;
    }
    if (bSymbol) {
        /* support multi assign */
        if (Cursor_isContain(right, TOKEN_devider, ",")) {
            eStmtType = STMT_tuple;
            goto __exit;
        }
        eStmtType = STMT_reference;
        goto __exit;
    }
__exit:
    Cursor_deinit(&cs);
    strsDeinit(&buffs);
    return eStmtType;
}

char* Lexer_printTokenStream(Args* outBuffs, char* tokenStream) {
    pika_assert(tokenStream);
    /* init */
    Args buffs = {0};
    char* sPrintOut = strsCopy(&buffs, "");

    /* process */
    uint16_t uTokenSize = TokenStream_getSize(tokenStream);
    for (uint16_t i = 0; i < uTokenSize; i++) {
        char* sToken = TokenStream_pop(&buffs, &tokenStream);
        if (sToken[0] == TOKEN_operator) {
            sPrintOut = strsAppend(&buffs, sPrintOut, "{opt}");
            sPrintOut = strsAppend(&buffs, sPrintOut, sToken + 1);
        }
        if (sToken[0] == TOKEN_devider) {
            sPrintOut = strsAppend(&buffs, sPrintOut, "{dvd}");
            sPrintOut = strsAppend(&buffs, sPrintOut, sToken + 1);
        }
        if (sToken[0] == TOKEN_symbol) {
            sPrintOut = strsAppend(&buffs, sPrintOut, "{sym}");
            sPrintOut = strsAppend(&buffs, sPrintOut, sToken + 1);
        }
        if (sToken[0] == TOKEN_literal) {
            sPrintOut = strsAppend(&buffs, sPrintOut, "{lit}");
            sPrintOut = strsAppend(&buffs, sPrintOut, sToken + 1);
        }
    }
    /* out put */
    sPrintOut = strsCopy(outBuffs, sPrintOut);
    strsDeinit(&buffs);
    return sPrintOut;
}

uint8_t Parser_checkIsDirect(char* sStr) {
    Args buffs = {0};
    uint8_t uRes = 0;
    pika_assert(NULL != sStr);
    char* sLeft = Cursor_splitCollect(&buffs, sStr, "=", 1);
    if (!strEqu(sLeft, sStr)) {
        uRes = 1;
        goto __exit;
    }
__exit:
    strsDeinit(&buffs);
    return uRes;
}

Arg* Lexer_setToken(Arg* tokenStream_arg,
                    enum TokenType token_type,
                    char*
                    operator) {
    Args buffs = {0};
    char sTokenTypeBuff[3] = {0};
    sTokenTypeBuff[0] = 0x1F;
    sTokenTypeBuff[1] = token_type;
    char* sTokenStream = arg_getStr(tokenStream_arg);
    sTokenStream = strsAppend(&buffs, sTokenStream, sTokenTypeBuff);
    sTokenStream = strsAppend(&buffs, sTokenStream, operator);
    Arg* aNewTokenStream = arg_newStr(sTokenStream);
    arg_deinit(tokenStream_arg);
    strsDeinit(&buffs);
    return aNewTokenStream;
}

Arg* Lexer_setSymbel(Arg* aTokenStream,
                     char* stmt,
                     int32_t i,
                     int32_t* iSymbolStartIndex_p) {
    Args buffs = {0};
    char* sSymbolBuff = NULL;
    if (-1 == *iSymbolStartIndex_p) {
        /* no found symbol start index */
        goto __exit;
    }
    /* nothing to add symbel */
    if (i == *iSymbolStartIndex_p) {
        goto __exit;
    }
    sSymbolBuff = args_getBuff(&buffs, i - *iSymbolStartIndex_p);
    pika_platform_memcpy(sSymbolBuff, stmt + *iSymbolStartIndex_p,
                         i - *iSymbolStartIndex_p);
    /* literal */
    if ((sSymbolBuff[0] == '\'') || (sSymbolBuff[0] == '"')) {
        /* "" or '' */
        aTokenStream = Lexer_setToken(aTokenStream, TOKEN_literal, sSymbolBuff);
        goto __exit;
    }

    if ((sSymbolBuff[0] >= '0') && (sSymbolBuff[0] <= '9')) {
        /* number */
        aTokenStream = Lexer_setToken(aTokenStream, TOKEN_literal, sSymbolBuff);
        goto __exit;
    }

    if ((sSymbolBuff[0] == 'b') &&
        ((sSymbolBuff[1] == '\'') || (sSymbolBuff[1] == '"'))) {
        /* b"" or b'' */
        aTokenStream = Lexer_setToken(aTokenStream, TOKEN_literal, sSymbolBuff);
        goto __exit;
    }

    /* symbol */
    aTokenStream = Lexer_setToken(aTokenStream, TOKEN_symbol, sSymbolBuff);
    goto __exit;
__exit:
    *iSymbolStartIndex_p = -1;
    strsDeinit(&buffs);
    return aTokenStream;
}

/* tokenStream is devided by space */
/* a token is [TOKENTYPE|(CONTENT)] */
char* Lexer_getTokenStream(Args* outBuffs, char* sStmt) {
    /* init */
    Arg* aTokenStream = New_arg(NULL);
    aTokenStream = arg_setStr(aTokenStream, "", "");
    int32_t iSize = strGetSize(sStmt);
    uint8_t uBracketDeepth = 0;
    uint8_t cn2 = 0;
    uint8_t cn1 = 0;
    uint8_t c0 = 0;
    uint8_t c1 = 0;
    uint8_t c2 = 0;
    uint8_t c3 = 0;
    uint8_t c4 = 0;
    uint8_t c5 = 0;
    uint8_t c6 = 0;
    int32_t iSymbolStartIndex = -1;
    pika_bool bInString = 0;
    pika_bool bNumber = 0;
    char* sTokenStream;

    /* process */
    for (int32_t i = 0; i < iSize; i++) {
        /* update char */
        cn2 = 0;
        cn1 = 0;
        c0 = sStmt[i];
        c1 = 0;
        c2 = 0;
        c3 = 0;
        c4 = 0;
        c5 = 0;
        c6 = 0;
        if (i - 2 >= 0) {
            cn2 = sStmt[i - 2];
        }
        if (i - 1 >= 0) {
            cn1 = sStmt[i - 1];
        }
        if (i + 1 < iSize) {
            c1 = sStmt[i + 1];
        }
        if (i + 2 < iSize) {
            c2 = sStmt[i + 2];
        }
        if (i + 3 < iSize) {
            c3 = sStmt[i + 3];
        }
        if (i + 4 < iSize) {
            c4 = sStmt[i + 4];
        }
        if (i + 5 < iSize) {
            c5 = sStmt[i + 5];
        }
        if (i + 6 < iSize) {
            c6 = sStmt[i + 6];
        }
        if (-1 == iSymbolStartIndex) {
            bNumber = 0;
            if ((c0 >= '0') && (c0 <= '9')) {
                bNumber = 1;
            }
            iSymbolStartIndex = i;
        }

        /* solve string */
        if (0 == bInString) {
            if ('\'' == c0) {
                if ('\\' != cn1 || ('\\' == cn1 && '\\' == cn2)) {
                    /* in ' */
                    bInString = 1;
                    continue;
                }
            }
            if ('"' == c0) {
                if ('\\' != cn1 || ('\\' == cn1 && '\\' == cn2)) {
                    /* in "" */
                    bInString = 2;
                    continue;
                }
            }
        }

        if (1 == bInString) {
            if ('\'' == c0 && ('\\' != cn1 || ('\\' == cn1 && '\\' == cn2))) {
                bInString = 0;
                aTokenStream = Lexer_setSymbel(aTokenStream, sStmt, i + 1,
                                               &iSymbolStartIndex);
            }
            continue;
        }
        if (2 == bInString) {
            if ('"' == c0 && ('\\' != cn1 || ('\\' == cn1 && '\\' == cn2))) {
                bInString = 0;
                aTokenStream = Lexer_setSymbel(aTokenStream, sStmt, i + 1,
                                               &iSymbolStartIndex);
            }
            continue;
        }

        /* match annotation */
        if ('#' == c0) {
            break;
        }

        /* match devider*/
        if (('(' == c0) || (')' == c0) || (',' == c0) || ('[' == c0) ||
            (']' == c0) || (':' == c0) || ('{' == c0) || ('}' == c0) ||
            (';' == c0)) {
            aTokenStream =
                Lexer_setSymbel(aTokenStream, sStmt, i, &iSymbolStartIndex);
            char sContent[2] = {0};
            sContent[0] = c0;
            aTokenStream =
                Lexer_setToken(aTokenStream, TOKEN_devider, sContent);
            if (c0 == '(') {
                uBracketDeepth++;
            }
            if (c0 == ')') {
                uBracketDeepth--;
            }
            continue;
        }
        /* match operator */
        if (('>' == c0) || ('<' == c0) || ('*' == c0) || ('/' == c0) ||
            ('+' == c0) || ('-' == c0) || ('!' == c0) || ('=' == c0) ||
            ('%' == c0) || ('&' == c0) || ('|' == c0) || ('^' == c0) ||
            ('~' == c0)) {
            if ('-' == c0 && bNumber) {
                if ((cn1 == 'e') || (cn1 == 'E')) {
                    continue;
                }
            }
            if (('*' == c0) || ('/' == c0)) {
                /*
                    =, **=, //
                */
                if ((c0 == c1) && ('=' == c2)) {
                    char sContent[4] = {0};
                    sContent[0] = c0;
                    sContent[1] = c1;
                    sContent[2] = '=';
                    aTokenStream = Lexer_setSymbel(aTokenStream, sStmt, i,
                                                   &iSymbolStartIndex);
                    aTokenStream =
                        Lexer_setToken(aTokenStream, TOKEN_operator, sContent);
                    i = i + 2;
                    continue;
                }
            }
            /*
                >>, <<, **, //
            */
            if (('>' == c0) || ('<' == c0) || ('*' == c0) || ('/' == c0)) {
                if (c0 == c1) {
                    /* >>=, <<=, **=, //= */
                    if ('=' == c2) {
                        char sContent[4] = {0};
                        sContent[0] = c0;
                        sContent[1] = c1;
                        sContent[2] = '=';
                        aTokenStream = Lexer_setSymbel(aTokenStream, sStmt, i,
                                                       &iSymbolStartIndex);
                        aTokenStream = Lexer_setToken(aTokenStream,
                                                      TOKEN_operator, sContent);
                        i = i + 2;
                        continue;
                    }
                    char content[3] = {0};
                    content[0] = c0;
                    content[1] = c1;
                    aTokenStream = Lexer_setSymbel(aTokenStream, sStmt, i,
                                                   &iSymbolStartIndex);
                    aTokenStream =
                        Lexer_setToken(aTokenStream, TOKEN_operator, content);
                    i = i + 1;
                    continue;
                }
            }
            /*
                >=, <=, *=, /=, +=, -=, !=, ==, %=, |=, ^=, &=
            */
            if (('>' == c0) || ('<' == c0) || ('*' == c0) || ('/' == c0) ||
                ('+' == c0) || ('-' == c0) || ('!' == c0) || ('=' == c0) ||
                ('%' == c0) || ('|' == c0) || ('&' == c0) || ('^' == c0)) {
                if ('=' == c1) {
                    char content[3] = {0};
                    content[0] = c0;
                    content[1] = c1;
                    aTokenStream = Lexer_setSymbel(aTokenStream, sStmt, i,
                                                   &iSymbolStartIndex);
                    aTokenStream =
                        Lexer_setToken(aTokenStream, TOKEN_operator, content);
                    i = i + 1;
                    continue;
                }
            }

            /* single operator */
            /*
                +, -, *, ... /
            */
            char sContent[2] = {0};
            sContent[0] = c0;
            aTokenStream =
                Lexer_setSymbel(aTokenStream, sStmt, i, &iSymbolStartIndex);
            aTokenStream =
                Lexer_setToken(aTokenStream, TOKEN_operator, sContent);
            continue;
        }

        // not the string operator
        if ((cn1 >= 'a' && cn1 <= 'z') || (cn1 >= 'A' && cn1 <= 'Z') ||
            (cn1 >= '0' && cn1 <= '9') || cn1 == '_' || cn1 == '.') {
            goto __after_match_string_operator;
        }
        /* not */
        if ('n' == c0) {
            if (('o' == c1) && ('t' == c2) && (' ' == c3)) {
                aTokenStream =
                    Lexer_setSymbel(aTokenStream, sStmt, i, &iSymbolStartIndex);
                aTokenStream =
                    Lexer_setToken(aTokenStream, TOKEN_operator, " not ");
                i = i + 3;
                continue;
            }
        }
        /* and */
        if ('a' == c0) {
            if (('n' == c1) && ('d' == c2) && (' ' == c3)) {
                aTokenStream =
                    Lexer_setSymbel(aTokenStream, sStmt, i, &iSymbolStartIndex);
                aTokenStream =
                    Lexer_setToken(aTokenStream, TOKEN_operator, " and ");
                i = i + 3;
                continue;
            }
        }
        /* or */
        if ('o' == c0) {
            if (('r' == c1) && (' ' == c2)) {
                aTokenStream =
                    Lexer_setSymbel(aTokenStream, sStmt, i, &iSymbolStartIndex);
                aTokenStream =
                    Lexer_setToken(aTokenStream, TOKEN_operator, " or ");
                i = i + 2;
                continue;
            }
        }
        /* is */
        if ('i' == c0) {
            if (('s' == c1) && (' ' == c2)) {
                aTokenStream =
                    Lexer_setSymbel(aTokenStream, sStmt, i, &iSymbolStartIndex);
                aTokenStream =
                    Lexer_setToken(aTokenStream, TOKEN_operator, " is ");
                i = i + 2;
                continue;
            }
        }
        /* in */
        if ('i' == c0) {
            if (('n' == c1) && (' ' == c2)) {
                aTokenStream =
                    Lexer_setSymbel(aTokenStream, sStmt, i, &iSymbolStartIndex);
                aTokenStream =
                    Lexer_setToken(aTokenStream, TOKEN_operator, " in ");
                i = i + 2;
                continue;
            }
        }
        /* if */
        if ('i' == c0) {
            if (('f' == c1) && (' ' == c2)) {
                aTokenStream =
                    Lexer_setSymbel(aTokenStream, sStmt, i, &iSymbolStartIndex);
                aTokenStream =
                    Lexer_setToken(aTokenStream, TOKEN_keyword, " if ");
                i = i + 2;
                continue;
            }
        }
        /* as */
        if ('a' == c0) {
            if (('s' == c1) && (' ' == c2)) {
                aTokenStream =
                    Lexer_setSymbel(aTokenStream, sStmt, i, &iSymbolStartIndex);
                aTokenStream =
                    Lexer_setToken(aTokenStream, TOKEN_operator, " as ");
                i = i + 2;
                continue;
            }
        }
        /* for */
        if ('f' == c0) {
            if (('o' == c1) && ('r' == c2) && (' ' == c3)) {
                aTokenStream =
                    Lexer_setSymbel(aTokenStream, sStmt, i, &iSymbolStartIndex);
                aTokenStream =
                    Lexer_setToken(aTokenStream, TOKEN_keyword, " for ");
                i = i + 3;
                continue;
            }
        }
        /* import */
        if ('i' == c0) {
            if (('m' == c1) && ('p' == c2) && ('o' == c3) && ('r' == c4) &&
                ('t' == c5) && (' ' == c6)) {
                aTokenStream =
                    Lexer_setSymbel(aTokenStream, sStmt, i, &iSymbolStartIndex);
                aTokenStream =
                    Lexer_setToken(aTokenStream, TOKEN_operator, " import ");
                i = i + 5;
                continue;
            }
        }
        /* @inh */
        if ('@' == c0) {
            if (('i' == c1) && ('n' == c2) && ('h' == c3) && (' ' == c4)) {
                aTokenStream =
                    Lexer_setSymbel(aTokenStream, sStmt, i, &iSymbolStartIndex);
                aTokenStream =
                    Lexer_setToken(aTokenStream, TOKEN_operator, "@inh ");
                i = i + 3;
                continue;
            }
        }
    __after_match_string_operator:
        /* skip spaces */
        if (' ' == c0) {
            /* not get symbal */
            if (i == iSymbolStartIndex) {
                iSymbolStartIndex = -1;
            } else {
                /* already get symbal */
                aTokenStream =
                    Lexer_setSymbel(aTokenStream, sStmt, i, &iSymbolStartIndex);
            }
        }
        if (i == iSize - 1) {
            /* last check symbel */
            // if('\n' == c0){
            //     continue;
            // }
            aTokenStream =
                Lexer_setSymbel(aTokenStream, sStmt, iSize, &iSymbolStartIndex);
        }
    }
    /* output */
    sTokenStream = arg_getStr(aTokenStream);
    sTokenStream = strsCopy(outBuffs, sTokenStream);
    arg_deinit(aTokenStream);
    return sTokenStream;
}

char* TokenStream_pop(Args* buffs, char** sTokenStream_pp) {
    return strsPopToken(buffs, sTokenStream_pp, 0x1F);
}

enum TokenType Token_getType(char* sToken) {
    return (enum TokenType)sToken[0];
}

char* Token_getPyload(char* sToken) {
    return (char*)((uintptr_t)sToken + 1);
}

uint8_t TokenStream_count(char* sTokenStream,
                          enum TokenType eTokenType,
                          char* sPyload) {
    Args buffs = {0};
    char* sTokenStreamBuff = strsCopy(&buffs, sTokenStream);
    uint8_t uRes = 0;
    uint16_t uTokenSize = TokenStream_getSize(sTokenStream);
    for (int i = 0; i < uTokenSize; i++) {
        char* sToken = TokenStream_pop(&buffs, &sTokenStreamBuff);
        if (eTokenType == Token_getType(sToken)) {
            if (strEqu(Token_getPyload(sToken), sPyload)) {
                uRes++;
            }
        }
    }
    strsDeinit(&buffs);
    return uRes;
}

uint8_t TokenStream_isContain(char* sTokenStream,
                              enum TokenType eTokenType,
                              char* sPyload) {
    if (TokenStream_count(sTokenStream, eTokenType, sPyload) > 0) {
        return 1;
    }
    return 0;
}

static char* _solveEqualLevelOperator(Args* buffs,
                                      char* sOperator,
                                      char* sOp1,
                                      char* sOp2,
                                      char* sStmt) {
    if ((strEqu(sOperator, sOp1)) || (strEqu(sOperator, sOp2))) {
        Cursor_forEach(cs, sStmt) {
            Cursor_iterStart(&cs);
            if (strEqu(cs.token1.pyload, sOp1)) {
                sOperator = strsCopy(buffs, sOp1);
            }
            if (strEqu(cs.token1.pyload, sOp2)) {
                sOperator = strsCopy(buffs, sOp2);
            }
            Cursor_iterEnd(&cs);
        }
        Cursor_deinit(&cs);
    }
    return sOperator;
}

static const char operators[][9] = {
    "**",  "~",    "*",     "/",     "%",    "//",      "+",  "-",  ">>",
    "<<",  "&",    "^",     "|",     "<",    "<=",      ">",  ">=", "!=",
    "==",  " is ", " in ",  "%=",    "/=",   "//=",     "-=", "+=", "*=",
    "**=", "^=",   " not ", " and ", " or ", " import "};

char* Lexer_getOperator(Args* outBuffs, char* sStmt) {
    Args buffs = {0};
    char* sOperator = NULL;

    // use parse state foreach to get operator
    for (uint32_t i = 0; i < sizeof(operators) / 9; i++) {
        Cursor_forEach(cs, sStmt) {
            Cursor_iterStart(&cs);
            // get operator
            if (strEqu(cs.token1.pyload, (char*)operators[i])) {
                // solve the iuuse of "~-1"
                sOperator = strsCopy(&buffs, (char*)operators[i]);
                Cursor_iterEnd(&cs);
                break;
            }
            Cursor_iterEnd(&cs);
        };
        Cursor_deinit(&cs);
    }

    /* solve the iuuse of "~-1" */
    if (strEqu(sOperator, "-")) {
        Cursor_forEach(cs, sStmt) {
            Cursor_iterStart(&cs);
            if (strEqu(cs.token2.pyload, "-")) {
                if (cs.token1.type == TOKEN_operator) {
                    sOperator = strsCopy(&buffs, cs.token1.pyload);
                    Cursor_iterEnd(&cs);
                    break;
                }
            }
            Cursor_iterEnd(&cs);
        };
        Cursor_deinit(&cs);
    }

    /* match the last operator in equal level */
    sOperator = _solveEqualLevelOperator(&buffs, sOperator, "+", "-", sStmt);
    sOperator = _solveEqualLevelOperator(&buffs, sOperator, "*", "/", sStmt);
    /* out put */
    if (NULL == sOperator) {
        return NULL;
    }
    sOperator = strsCopy(outBuffs, sOperator);
    strsDeinit(&buffs);
    return sOperator;
}

const char void_str[] = "";

void LexToken_update(struct LexToken* lex_token) {
    lex_token->type = Token_getType(lex_token->tokenStream);
    if (lex_token->type == TOKEN_strEnd) {
        lex_token->pyload = (char*)void_str;
    } else {
        lex_token->pyload = Token_getPyload(lex_token->tokenStream);
    }
}

void Cursor_iterStart(struct Cursor* cs) {
    cs->iter_index++;
    cs->iter_buffs = New_strBuff();
    /* token1 is the last token */
    cs->token1.tokenStream =
        strsCopy(cs->iter_buffs, arg_getStr(cs->last_token));
    /* token2 is the next token */
    cs->token2.tokenStream = TokenStream_pop(cs->iter_buffs, &cs->tokenStream);
    /* store last token */
    arg_deinit(cs->last_token);
    cs->last_token = arg_newStr(cs->token2.tokenStream);

    LexToken_update(&cs->token1);
    LexToken_update(&cs->token2);
    if (strEqu(cs->token1.pyload, "(")) {
        cs->bracket_deepth++;
    }
    if (strEqu(cs->token1.pyload, ")")) {
        cs->bracket_deepth--;
    }
    if (strEqu(cs->token1.pyload, "[")) {
        cs->bracket_deepth++;
    }
    if (strEqu(cs->token1.pyload, "]")) {
        cs->bracket_deepth--;
    }
    if (strEqu(cs->token1.pyload, "{")) {
        cs->bracket_deepth++;
    }
    if (strEqu(cs->token1.pyload, "}")) {
        cs->bracket_deepth--;
    }
}

void LexToken_init(struct LexToken* lt) {
    lt->pyload = NULL;
    lt->tokenStream = NULL;
    lt->type = TOKEN_strEnd;
}

void _Cursor_init(struct Cursor* cs) {
    cs->tokenStream = NULL;
    cs->length = 0;
    cs->iter_index = 0;
    cs->bracket_deepth = 0;
    cs->last_token = NULL;
    cs->iter_buffs = NULL;
    cs->buffs_p = New_strBuff();
    cs->result = PIKA_RES_OK;
    LexToken_init(&cs->token1);
    LexToken_init(&cs->token2);
}

void Cursor_iterEnd(struct Cursor* cs) {
    args_deinit(cs->iter_buffs);
}

void Cursor_deinit(struct Cursor* cs) {
    if (NULL != cs->last_token) {
        arg_deinit(cs->last_token);
    }
    args_deinit(cs->buffs_p);
}

void _Cursor_parse(struct Cursor* cs, char* stmt) {
    if (NULL == stmt) {
        cs->result = PIKA_RES_ERR_SYNTAX_ERROR;
        return;
    }
    cs->tokenStream = Lexer_getTokenStream(cs->buffs_p, stmt);
    if (NULL == cs->tokenStream) {
        cs->result = PIKA_RES_ERR_SYNTAX_ERROR;
        return;
    }
    cs->length = TokenStream_getSize(cs->tokenStream);
}

void _Cursor_beforeIter(struct Cursor* cs) {
    /* clear first token */
    if (cs->result != PIKA_RES_OK) {
        return;
    }
    TokenStream_pop(cs->buffs_p, &cs->tokenStream);
    cs->last_token = arg_newStr(TokenStream_pop(cs->buffs_p, &cs->tokenStream));
}

uint8_t Token_isBranketStart(LexToken* token) {
    if (token->type != TOKEN_devider) {
        return pika_false;
    }
    if (strEqu(token->pyload, "(") || strEqu(token->pyload, "[") ||
        strEqu(token->pyload, "{")) {
        return pika_true;
    }
    return pika_false;
}

uint8_t Token_isBranketEnd(LexToken* token) {
    if (token->type != TOKEN_devider) {
        return pika_false;
    }
    if (strEqu(token->pyload, ")") || strEqu(token->pyload, "]") ||
        strEqu(token->pyload, "}")) {
        return pika_true;
    }
    return pika_false;
}

uint8_t Token_isBranket(LexToken* token) {
    return Token_isBranketStart(token) || Token_isBranketEnd(token);
}

uint8_t _Cursor_count(char* sStmt,
                      TokenType eType,
                      char* sPyload,
                      pika_bool bSkipbracket) {
    /* fast return */
    if (!strstr(sStmt, sPyload)) {
        return pika_false;
    }
    Args buffs = {0};
    uint8_t uRes = 0;
    Cursor_forEach(cs, sStmt) {
        Cursor_iterStart(&cs);
        if (cs.token1.type == eType && (strEqu(cs.token1.pyload, sPyload))) {
            if (bSkipbracket) {
                uint8_t branket_deepth_check = 0;
                if (Token_isBranketStart(&cs.token1)) {
                    branket_deepth_check = 1;
                }
                if (cs.bracket_deepth > branket_deepth_check) {
                    /* skip bracket */
                    Cursor_iterEnd(&cs);
                    continue;
                }
            }
            uRes++;
        }
        Cursor_iterEnd(&cs);
    };
    Cursor_deinit(&cs);
    strsDeinit(&buffs);
    return uRes;
}

uint8_t Cursor_count(char* sStmt, TokenType eType, char* sPyload) {
    return _Cursor_count(sStmt, eType, sPyload, pika_false);
}

pika_bool Cursor_isContain(char* sStmt, TokenType eType, char* sPyload) {
    if (Cursor_count(sStmt, eType, sPyload) > 0) {
        return pika_true;
    }
    return pika_false;
}

char* Cursor_popToken(Args* buffs, char** sStmt_p, char* sDevide) {
    Arg* aOutitem = arg_newStr("");
    Arg* aTokenStreamAfter = arg_newStr("");
    pika_bool bFindDevide = pika_false;
    Cursor_forEach(cs, *sStmt_p) {
        Cursor_iterStart(&cs);
        if (!bFindDevide) {
            if ((cs.bracket_deepth == 0 && strEqu(cs.token1.pyload, sDevide)) ||
                cs.iter_index == cs.length) {
                bFindDevide = pika_true;
                Cursor_iterEnd(&cs);
                continue;
            }
        }
        if (!bFindDevide) {
            aOutitem = arg_strAppend(aOutitem, cs.token1.pyload);
        } else {
            aTokenStreamAfter =
                arg_strAppend(aTokenStreamAfter, cs.token1.pyload);
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    /* cache out item */
    char* sOutItem = strsCopy(buffs, arg_getStr(aOutitem));
    arg_deinit(aOutitem);
    /* cache tokenStream after */
    char* sTokenAfter = strsCopy(buffs, arg_getStr(aTokenStreamAfter));
    arg_deinit(aTokenStreamAfter);
    /* update tokenStream */
    *sStmt_p = sTokenAfter;
    return sOutItem;
}

char* Cursor_splitCollect(Args* buffs, char* sStmt, char* sDevide, int index) {
    Arg* aOut = arg_newStr("");
    int iExpectBracket = 0;
    if (sDevide[0] == '(' || sDevide[0] == '[' || sDevide[0] == '{') {
        iExpectBracket = 1;
    }
    int i = 0;
    Cursor_forEach(cs, sStmt) {
        Cursor_iterStart(&cs);
        if (cs.bracket_deepth == iExpectBracket &&
            strEqu(cs.token1.pyload, sDevide)) {
            i++;
            Cursor_iterEnd(&cs);
            continue;
        }
        if (i == index) {
            aOut = arg_strAppend(aOut, cs.token1.pyload);
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    /* if not found, return origin string */
    if (i == 0) {
        arg_deinit(aOut);
        aOut = arg_newStr(sStmt);
    }
    return strsCacheArg(buffs, aOut);
}

static void Slice_getPars(Args* outBuffs,
                          char* sInner,
                          char** sStart_p,
                          char** sEnd_p,
                          char** sStep_p) {
#if PIKA_NANO_ENABLE
    return;
#endif
    Args buffs = {0};
    *sStart_p = "";
    *sEnd_p = "";
    *sStep_p = "";

    /* slice */
    uint8_t uColonIndex = 0;
    Cursor_forEach(cs, sInner) {
        Cursor_iterStart(&cs);
        if (strEqu(cs.token1.pyload, ":") && cs.bracket_deepth == 0) {
            uColonIndex++;
            goto __iter_continue1;
        }
        if (uColonIndex == 0) {
            *sStart_p = strsAppend(&buffs, *sStart_p, cs.token1.pyload);
        }
        if (uColonIndex == 1) {
            *sEnd_p = strsAppend(&buffs, *sEnd_p, cs.token1.pyload);
        }
        if (uColonIndex == 2) {
            *sStep_p = strsAppend(&buffs, *sStep_p, cs.token1.pyload);
        }
    __iter_continue1:
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    if (uColonIndex == 1) {
        *sStep_p = "1";
        if (strEqu(*sStart_p, "")) {
            *sStart_p = "0";
        }
        if (strEqu(*sEnd_p, "")) {
            *sEnd_p = "-1";
        }
    }
    if (uColonIndex == 0) {
        *sEnd_p = strsAppend(&buffs, *sStart_p, " + 1");
        *sStep_p = "1";
    }

    /* slice with step */

    /* output */
    *sStart_p = strsCopy(outBuffs, *sStart_p);
    *sEnd_p = strsCopy(outBuffs, *sEnd_p);
    *sStep_p = strsCopy(outBuffs, *sStep_p);
    /* clean */
    strsDeinit(&buffs);
}

char* Suger_leftSlice(Args* outBuffs, char* sRight, char** sLeft_p) {
#if !PIKA_SYNTAX_SLICE_ENABLE
    return sRight;
#endif
    /* init objects */
    Args buffs = {0};
    Arg* aRight = arg_newStr("");
    char* sLeft = *sLeft_p;
    pika_bool bInBrancket = 0;
    args_setStr(&buffs, "inner", "");
    pika_bool bMatched = 0;
    char* sRightRes = NULL;
    /* exit when NULL */
    if (NULL == sLeft) {
        arg_deinit(aRight);
        aRight = arg_newStr(sRight);
        goto __exit;
    }
    /* exit when not match
         (symble|iteral)'['
    */
    Cursor_forEach(cs, sLeft) {
        Cursor_iterStart(&cs);
        if (strEqu(cs.token2.pyload, "[")) {
            if (TOKEN_symbol == cs.token1.type ||
                TOKEN_literal == cs.token1.type) {
                bMatched = 1;
                Cursor_iterEnd(&cs);
                break;
            }
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    if (!bMatched) {
        /* not contain '[', return origin */
        arg_deinit(aRight);
        aRight = arg_newStr(sRight);
        goto __exit;
    }

    /* matched [] */
    Cursor_forEachExistPs(cs, sLeft) {
        Cursor_iterStart(&cs);
        /* found '[' */
        if ((TOKEN_devider == cs.token2.type) &&
            (strEqu(cs.token2.pyload, "["))) {
            /* get 'obj' from obj[] */
            args_setStr(&buffs, "obj", cs.token1.pyload);
            bInBrancket = 1;
            /* fond ']' */
        } else if ((TOKEN_devider == cs.token2.type) &&
                   (strEqu(cs.token2.pyload, "]"))) {
            bInBrancket = 0;
            char* sInner = args_getStr(&buffs, "inner");
            Arg* aInner = arg_newStr(sInner);
            aInner = arg_strAppend(aInner, cs.token1.pyload);
            args_setStr(&buffs, "inner", arg_getStr(aInner));
            arg_deinit(aInner);
            /* update inner pointer */
            sInner = args_getStr(&buffs, "inner");
            char* sStart = NULL;
            char* sEnd = NULL;
            char* sStep = NULL;
            Slice_getPars(&buffs, sInner, &sStart, &sEnd, &sStep);
            /* obj = __setitem__(obj, key, val) */
            aRight = arg_strAppend(aRight, "__setitem__(");
            aRight = arg_strAppend(aRight, args_getStr(&buffs, "obj"));
            aRight = arg_strAppend(aRight, ",");
            aRight = arg_strAppend(aRight, sStart);
            aRight = arg_strAppend(aRight, ",");
            aRight = arg_strAppend(aRight, sRight);
            aRight = arg_strAppend(aRight, ")");
            /* clean the inner */
            args_setStr(&buffs, "inner", "");
            /* in brancket and found '[' */
        } else if (bInBrancket && (!strEqu(cs.token1.pyload, "["))) {
            char* sInner = args_getStr(&buffs, "inner");
            Arg* aIndex = arg_newStr(sInner);
            aIndex = arg_strAppend(aIndex, cs.token1.pyload);
            args_setStr(&buffs, "inner", arg_getStr(aIndex));
            arg_deinit(aIndex);
            /* out of brancket and not found ']' */
        } else if (!bInBrancket && (!strEqu(cs.token1.pyload, "]"))) {
            if (TOKEN_strEnd != cs.token1.type) {
                aRight = arg_strAppend(aRight, cs.token1.pyload);
            }
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    /* clean the left */
    for (size_t i = 0; i < strGetSize(sLeft); i++) {
        if (sLeft[i] == '[') {
            sLeft[i] = '\0';
            break;
        }
    }
__exit:
    /* clean and return */
    sRightRes = strsCopy(outBuffs, arg_getStr(aRight));
    arg_deinit(aRight);
    strsDeinit(&buffs);
    return sRightRes;
}

char* Suger_format(Args* outBuffs, char* sRight) {
#if !PIKA_SYNTAX_FORMAT_ENABLE
    return sRight;
#endif
    /* quick skip */
    if (!strIsContain(sRight, '%')) {
        return sRight;
    }

    pika_bool bFormat = pika_false;
    Cursor_forEach(ps1, sRight) {
        Cursor_iterStart(&ps1);
        if (ps1.bracket_deepth == 0 && strEqu(ps1.token1.pyload, "%")) {
            bFormat = pika_true;
        }
        Cursor_iterEnd(&ps1);
    }
    Cursor_deinit(&ps1);
    if (pika_false == bFormat) {
        return sRight;
    }

    char* sRes = sRight;
    Arg* aStrBuf = arg_newStr("");
    Arg* aVarBuf = arg_newStr("");
    pika_bool bInFormat = pika_false;
    pika_bool bTuple = pika_false;
    pika_bool bOutVars = pika_false;
    Args buffs = {0};
    char* sFmt = NULL;
    Cursor_forEach(cs, sRight) {
        char* sItem = "";
        Cursor_iterStart(&cs);
        if (pika_false == bInFormat) {
            if (cs.token1.type != TOKEN_literal) {
                sItem = cs.token1.pyload;
                goto __iter_continue;
            }
            if (cs.token1.pyload[0] != '\'' && cs.token1.pyload[0] != '"') {
                sItem = cs.token1.pyload;
                goto __iter_continue;
            }
            if (!strEqu(cs.token2.pyload, "%")) {
                sItem = cs.token1.pyload;
                goto __iter_continue;
            }
            /* found the format stmt */
            bInFormat = pika_true;
            sFmt = strsCopy(&buffs, cs.token1.pyload);
            goto __iter_continue;
        }
        if (pika_true == bInFormat) {
            /* check the format vars */
            if (strEqu(cs.token1.pyload, "%")) {
                /* is a tuple */
                if (strEqu(cs.token2.pyload, "(")) {
                    bTuple = pika_true;
                } else {
                    aVarBuf = arg_strAppend(aVarBuf, cs.token2.pyload);
                }
                goto __iter_continue;
            }
            /* found the end of tuple */
            if (cs.iter_index == cs.length) {
                bOutVars = pika_true;
                bInFormat = pika_false;
            } else {
                /* push the vars inner the tuple */
                aVarBuf = arg_strAppend(aVarBuf, cs.token2.pyload);
            }
            if (bOutVars) {
                if (bTuple) {
                    aStrBuf = arg_strAppend(aStrBuf, "cformat(");
                    aStrBuf = arg_strAppend(aStrBuf, sFmt);
                    aStrBuf = arg_strAppend(aStrBuf, ",");
                    aStrBuf = arg_strAppend(aStrBuf, arg_getStr(aVarBuf));
                } else {
                    aStrBuf = arg_strAppend(aStrBuf, "cformat(");
                    aStrBuf = arg_strAppend(aStrBuf, sFmt);
                    aStrBuf = arg_strAppend(aStrBuf, ",");
                    aStrBuf = arg_strAppend(aStrBuf, arg_getStr(aVarBuf));
                    aStrBuf = arg_strAppend(aStrBuf, ")");
                }
            }
        }
    __iter_continue:
        if (!bInFormat) {
            aStrBuf = arg_strAppend(aStrBuf, sItem);
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);

    sRes = strsCopy(outBuffs, arg_getStr(aStrBuf));
    arg_deinit(aStrBuf);
    arg_deinit(aVarBuf);
    strsDeinit(&buffs);
    return sRes;
}

#define SELF_OPERATORES_LEN 4
static const char selfOperators[][SELF_OPERATORES_LEN] = {
    "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=", "**=", "//="};

uint8_t Suger_selfOperator(Args* outbuffs,
                           char* sStmt,
                           char** sRight_p,
                           char** sLeft_p) {
    char* sLeftNew = NULL;
    char* sRightNew = NULL;
    Arg* aLeft = arg_newStr("");
    Arg* aRight = arg_newStr("");
    Arg* aRightNew = arg_newStr("");
    pika_bool bLeftExist = 0;

    Args buffs = {0};
    char _sOperator[3] = {0};
    char* sOperator = (char*)_sOperator;
    pika_bool bRight = 0;
    for (uint8_t i = 0; i < sizeof(selfOperators) / SELF_OPERATORES_LEN; i++) {
        if (Cursor_isContain(sStmt, TOKEN_operator, (char*)selfOperators[i])) {
            pika_platform_memcpy(sOperator, selfOperators[i],
                                 strGetSize((char*)selfOperators[i]) - 1);
            break;
        }
    }
    /* not found self operator */
    if (sOperator[0] == 0) {
        goto __exit;
    }
    /* found self operator */
    bLeftExist = 1;
    Cursor_forEach(cs, sStmt) {
        Cursor_iterStart(&cs);
        for (uint8_t i = 0; i < sizeof(selfOperators) / SELF_OPERATORES_LEN;
             i++) {
            if (strEqu(cs.token1.pyload, (char*)selfOperators[i])) {
                bRight = 1;
                goto __iter_continue;
            }
        }
        if (!bRight) {
            aLeft = arg_strAppend(aLeft, cs.token1.pyload);
        } else {
            aRight = arg_strAppend(aRight, cs.token1.pyload);
        }
    __iter_continue:
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    /* connect right */
    aRightNew = arg_strAppend(aRightNew, arg_getStr(aLeft));
    aRightNew = arg_strAppend(aRightNew, sOperator);
    aRightNew = arg_strAppend(aRightNew, "(");
    aRightNew = arg_strAppend(aRightNew, arg_getStr(aRight));
    aRightNew = arg_strAppend(aRightNew, ")");

    /* collect left_new and right_new */
    sLeftNew = arg_getStr(aLeft);
    sRightNew = arg_getStr(aRightNew);

__exit:
    strsDeinit(&buffs);
    if (NULL != sRightNew) {
        *(sRight_p) = strsCopy(outbuffs, sRightNew);
        ;
    }
    if (NULL != sLeftNew) {
        *(sLeft_p) = strsCopy(outbuffs, sLeftNew);
    }
    arg_deinit(aRight);
    arg_deinit(aLeft);
    arg_deinit(aRightNew);
    return bLeftExist;
}

AST* AST_create(void) {
    return New_queueObj();
}

PIKA_RES AST_setNodeAttr(AST* ast, char* sAttrName, char* sAttrVal) {
    return obj_setStr(ast, sAttrName, sAttrVal);
}

char* AST_getNodeAttr(AST* ast, char* sAttrName) {
    return obj_getStr(ast, sAttrName);
}

PIKA_RES AST_setNodeBlock(AST* ast, char* sNodeContent) {
    return AST_setNodeAttr(ast, "block", sNodeContent);
}

char* AST_getThisBlock(AST* ast) {
    return obj_getStr(ast, "block");
}

PIKA_RES AST_parseSubStmt(AST* ast, char* sNodeContent) {
    queueObj_pushObj(ast, (char*)"stmt");
    AST_parseStmt(queueObj_getCurrentObj(ast), sNodeContent);
    return PIKA_RES_OK;
}

char* Parser_popSubStmt(Args* outbuffs, char** sStmt_p, char* sDelimiter) {
    Arg* aSubstmt = arg_newStr("");
    Arg* aNewStmt = arg_newStr("");
    char* sStmt = *sStmt_p;
    pika_bool bIsGetSubstmt = pika_false;
    Args buffs = {0};
    Cursor_forEach(cs, sStmt) {
        Cursor_iterStart(&cs);
        if (bIsGetSubstmt) {
            /* get new stmt */
            aNewStmt = arg_strAppend(aNewStmt, cs.token1.pyload);
            Cursor_iterEnd(&cs);
            continue;
        }
        if (cs.bracket_deepth > 0) {
            /* ignore */
            aSubstmt = arg_strAppend(aSubstmt, cs.token1.pyload);
            Cursor_iterEnd(&cs);
            continue;
        }
        if (strEqu(cs.token1.pyload, sDelimiter)) {
            /* found delimiter */
            bIsGetSubstmt = pika_true;
            Cursor_iterEnd(&cs);
            continue;
        }
        /* collect substmt */
        aSubstmt = arg_strAppend(aSubstmt, cs.token1.pyload);
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);

    strsDeinit(&buffs);

    char* sSubstmt = strsCacheArg(outbuffs, aSubstmt);
    char* sNewstmt = strsCacheArg(outbuffs, aNewStmt);
    *sStmt_p = sNewstmt;
    return sSubstmt;
}

int Parser_getSubStmtNum(char* sSubStmts, char* sDelimiter) {
    if (strEqu(sSubStmts, ",")) {
        return 0;
    }
    return _Cursor_count(sSubStmts, TOKEN_devider, sDelimiter, pika_true);
}

char* _Parser_popLastSubStmt(Args* outbuffs,
                             char** sStmt_p,
                             char* sDelimiter,
                             pika_bool bSkipBracket) {
    uint8_t uLastStmtI = 0;
    char* stmt = *sStmt_p;
    Cursor_forEach(cs, stmt) {
        Cursor_iterStart(&cs);
        if (strIsStartWith(cs.token1.pyload, sDelimiter)) {
            /* found delimiter */

            if (bSkipBracket && cs.bracket_deepth > 0) {
                /* skip bracket */
                Cursor_iterEnd(&cs);
                continue;
            }

            /* for "[" */
            if (cs.bracket_deepth > 1) {
                /* ignore */
                Cursor_iterEnd(&cs);
                continue;
            }

            uLastStmtI = cs.iter_index;
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);

    Arg* aMainStmt = arg_newStr("");
    Arg* aLastStmt = arg_newStr("");
    {
        Cursor_forEach(cs, stmt) {
            Cursor_iterStart(&cs);
            if (cs.iter_index < uLastStmtI) {
                aMainStmt = arg_strAppend(aMainStmt, cs.token1.pyload);
            }
            if (cs.iter_index >= uLastStmtI) {
                aLastStmt = arg_strAppend(aLastStmt, cs.token1.pyload);
            }
            Cursor_iterEnd(&cs);
        }
        Cursor_deinit(&cs);
    }

    *sStmt_p = strsCacheArg(outbuffs, aMainStmt);
    return strsCacheArg(outbuffs, aLastStmt);
}

char* Parser_popLastSubStmt(Args* outbuffs, char** sStmt_p, char* sDelimiter) {
    return _Parser_popLastSubStmt(outbuffs, sStmt_p, sDelimiter, pika_true);
}

static void _AST_parse_list(AST* ast, Args* buffs, char* sStmt) {
#if !PIKA_BUILTIN_STRUCT_ENABLE
    return;
#endif
    AST_setNodeAttr(ast, (char*)"list", "list");
    char* sSubStmts = strsCut(buffs, sStmt, '[', ']');
    sSubStmts = strsAppend(buffs, sSubStmts, ",");
    while (1) {
        char* sSubStmt = Parser_popSubStmt(buffs, &sSubStmts, ",");
        AST_parseSubStmt(ast, sSubStmt);
        if (strEqu(sSubStmts, "")) {
            break;
        }
    }
    return;
}

static void _AST_parse_comprehension(AST* ast, Args* outBuffs, char* sStmt) {
#if PIKA_NANO_ENABLE
    return;
#endif
    /* [ substmt1 for substmt2 in substmt3 ] */
    Args buffs = {0};
    AST_setNodeAttr(ast, (char*)"comprehension", "");
    char* sSubStmts = strsCut(&buffs, sStmt, '[', ']');
    char* sSubStms1 = Cursor_splitCollect(&buffs, sSubStmts, " for ", 0);
    char* sSubStms23 = Cursor_splitCollect(&buffs, sSubStmts, " for ", 1);
    char* sSubStms2 = Cursor_splitCollect(&buffs, sSubStms23, " in ", 0);
    char* sSubStms3 = Cursor_splitCollect(&buffs, sSubStms23, " in ", 1);
    AST_setNodeAttr(ast, (char*)"substmt1", sSubStms1);
    AST_setNodeAttr(ast, (char*)"substmt2", sSubStms2);
    AST_setNodeAttr(ast, (char*)"substmt3", sSubStms3);
    strsDeinit(&buffs);
    return;
}

static void _AST_parse_list_comprehension(AST* ast, Args* buffs, char* sStmt) {
    if (Cursor_isContain(sStmt, TOKEN_keyword, " for ")) {
        _AST_parse_comprehension(ast, buffs, sStmt);
        return;
    }
    _AST_parse_list(ast, buffs, sStmt);
}

static void _AST_parse_dict(AST* ast, Args* buffs, char* sStmt) {
#if !PIKA_BUILTIN_STRUCT_ENABLE
    return;
#endif
    AST_setNodeAttr(ast, (char*)"dict", "dict");
    char* subStmts = strsCut(buffs, sStmt, '{', '}');
    subStmts = strsAppend(buffs, subStmts, ",");
    while (1) {
        char* sSubStmt = Parser_popSubStmt(buffs, &subStmts, ",");
        char* sKey = Parser_popSubStmt(buffs, &sSubStmt, ":");
        char* sValue = sSubStmt;
        AST_parseSubStmt(ast, sKey);
        AST_parseSubStmt(ast, sValue);
        if (strEqu(subStmts, "")) {
            break;
        }
    }
}

static void _AST_parse_slice(AST* ast, Args* buffs, char* sStmt) {
#if !PIKA_SYNTAX_SLICE_ENABLE
    return;
#endif
    AST_setNodeAttr(ast, (char*)"slice", "slice");
    sStmt = strsCopy(buffs, sStmt);
    char* sLaststmt = _Parser_popLastSubStmt(buffs, &sStmt, "[", pika_false);
    AST_parseSubStmt(ast, sStmt);
    char* sSliceList = strsCut(buffs, sLaststmt, '[', ']');
    pika_assert(sSliceList != NULL);
    sSliceList = strsAppend(buffs, sSliceList, ":");
    int iIndex = 0;
    while (1) {
        char* sSlice = Parser_popSubStmt(buffs, &sSliceList, ":");
        if (iIndex == 0 && strEqu(sSlice, "")) {
            AST_parseSubStmt(ast, "0");
        } else if (iIndex == 1 && strEqu(sSlice, "")) {
            AST_parseSubStmt(ast, "-99999");
        } else {
            AST_parseSubStmt(ast, sSlice);
        }
        iIndex++;
        if (strEqu("", sSliceList)) {
            break;
        }
    }
}

char* _Suger_process(Args* out_buffs,
                     char* sLine,
                     char* sToken1,
                     char* sToken2,
                     char* sFormat) {
    char* sRet = sLine;
    char* sStmt1 = "";
    char* sStmt2 = "";
    pika_bool bGotTokens = pika_false;
    pika_bool bSkip = pika_false;
    Args buffs = {0};

    if (1 != Cursor_count(sLine, TOKEN_operator, sToken1)) {
        sRet = sLine;
        goto __exit;
    }
    if (1 != Cursor_count(sLine, TOKEN_operator, sToken2)) {
        sRet = sLine;
        goto __exit;
    }

    Cursor_forEach(cs, sLine) {
        Cursor_iterStart(&cs);
        if (!bGotTokens) {
            if (strEqu(cs.token1.pyload, sToken1) &&
                strEqu(cs.token2.pyload, sToken2)) {
                bGotTokens = pika_true;
                Cursor_iterEnd(&cs);
                continue;
            }
            sStmt1 = strsAppend(&buffs, sStmt1, cs.token1.pyload);
        } else {
            if (!bSkip) {
                bSkip = pika_true;
                Cursor_iterEnd(&cs);
                continue;
            }
            sStmt2 = strsAppend(&buffs, sStmt2, cs.token1.pyload);
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);

    if (!bGotTokens) {
        sRet = sLine;
        goto __exit;
    }

    sRet = strsFormat(out_buffs,
                      strGetSize(sLine) + strlen(sToken1) + strlen(sToken2),
                      sFormat, sStmt1, sStmt2);

__exit:
    strsDeinit(&buffs);
    return sRet;
}

char* Suger_not_in(Args* out_buffs, char* sLine) {
    return _Suger_process(out_buffs, sLine, " not ", " in ", " not %s in %s");
}

char* Suger_is_not(Args* out_buffs, char* sLine) {
    return _Suger_process(out_buffs, sLine, " is ", " not ", " not %s is %s");
}

AST* AST_parseStmt(AST* ast, char* sStmt) {
    Args buffs = {0};
    char* assignment = Cursor_splitCollect(&buffs, sStmt, "(", 0);
    char* sMethod = NULL;
    char* sRef = NULL;
    char* sStr = NULL;
    char* sNum = NULL;
    char* sLeft = NULL;
    char* sRight = NULL;
    char* sImport = NULL;
    char* sInhert = NULL;
    enum StmtType eStmtType = STMT_none;
    PIKA_RES eResult = PIKA_RES_OK;

    sRight = sStmt;
    /* solve check direct */
    pika_bool bLeftExist = 0;
    if (Parser_checkIsDirect(assignment)) {
        bLeftExist = 1;
        sLeft = strsCopy(&buffs, "");
        sRight = strsCopy(&buffs, "");
        pika_bool bMeetEqu = 0;
        Cursor_forEach(cs, sStmt) {
            Cursor_iterStart(&cs);
            if (!bMeetEqu && strEqu(cs.token1.pyload, "=") &&
                cs.token1.type == TOKEN_operator) {
                bMeetEqu = 1;
                Cursor_iterEnd(&cs);
                continue;
            }
            if (0 == bMeetEqu) {
                sLeft = strsAppend(&buffs, sLeft, cs.token1.pyload);
            }
            if (1 == bMeetEqu) {
                sRight = strsAppend(&buffs, sRight, cs.token1.pyload);
            }
            Cursor_iterEnd(&cs);
        }
        Cursor_deinit(&cs);
    }
    /* solve the += -= /= *= stmt */
    if (!bLeftExist) {
        bLeftExist = Suger_selfOperator(&buffs, sStmt, &sRight, &sLeft);
    }

    /* remove hint */
    if (bLeftExist) {
        sLeft = Cursor_splitCollect(&buffs, sLeft, ":", 0);
    }

    /* solve the [] stmt */
    sRight = Suger_leftSlice(&buffs, sRight, &sLeft);
    sRight = Suger_format(&buffs, sRight);

    /* set left */
    if (bLeftExist) {
        if (strEqu(sLeft, "")) {
            eResult = PIKA_RES_ERR_SYNTAX_ERROR;
            goto __exit;
        }
        AST_setNodeAttr(ast, (char*)"left", sLeft);
    }
    /* match statment type */
    eStmtType = Lexer_matchStmtType(sRight);
    if (STMT_tuple == eStmtType) {
        sRight = strsFormat(&buffs, PIKA_LINE_BUFF_SIZE, "(%s)", sRight);
        eStmtType = STMT_method;
    }
    /* solve operator stmt */
    if (STMT_operator == eStmtType) {
        sRight = Suger_not_in(&buffs, sRight);
        sRight = Suger_is_not(&buffs, sRight);
        char* rightWithoutSubStmt = _remove_sub_stmt(&buffs, sRight);
        char* operator= Lexer_getOperator(&buffs, rightWithoutSubStmt);
        if (NULL == operator) {
            eResult = PIKA_RES_ERR_SYNTAX_ERROR;
            goto __exit;
        }
        AST_setNodeAttr(ast, (char*)"operator", operator);
        char* sRightBuff = strsCopy(&buffs, sRight);
        char* sSubStmt2 = Cursor_popLastToken(&buffs, &sRightBuff, operator);
        char* sSubStmt1 = sRightBuff;
        AST_parseSubStmt(ast, sSubStmt1);
        AST_parseSubStmt(ast, sSubStmt2);
        goto __exit;
    }

    /* solve list stmt */
    if (STMT_list == eStmtType) {
        _AST_parse_list_comprehension(ast, &buffs, sRight);
        goto __exit;
    }

    /* solve dict stmt */
    if (STMT_dict == eStmtType) {
        _AST_parse_dict(ast, &buffs, sRight);
        goto __exit;
    }

    /* solve method chain */
    if (STMT_chain == eStmtType) {
        char* sHost = strsCopy(&buffs, sRight);
        char* sMethodStmt = Parser_popLastSubStmt(&buffs, &sHost, ".");
        AST_parseSubStmt(ast, sHost);
        AST_parseStmt(ast, sMethodStmt);
        goto __exit;
    }

    if (STMT_slice == eStmtType) {
        /* solve slice stmt */
        _AST_parse_slice(ast, &buffs, sRight);
        goto __exit;
    }

    /* solve method stmt */
    if (STMT_method == eStmtType) {
        char* sRealType = "method";
        char* sMethodStmt = strsCopy(&buffs, sRight);
        char* sLastStmt = sMethodStmt;
        /* for method()() */
        int iBracketNum =
            _Cursor_count(sMethodStmt, TOKEN_devider, "(", pika_true) +
            _Cursor_count(sMethodStmt, TOKEN_devider, "[", pika_true);
        if (iBracketNum > 1) {
            sLastStmt =
                _Parser_popLastSubStmt(&buffs, &sMethodStmt, "(", pika_false);
            /* for (...) */
            if (_Cursor_count(sLastStmt, TOKEN_devider, "(", pika_false) == 1) {
                char* sMethodCheck = strsGetFirstToken(&buffs, sLastStmt, '(');
                if (strEqu(sMethodCheck, "")) {
                    sLastStmt = strsAppend(&buffs, ".", sLastStmt);
                }
            }
            AST_parseSubStmt(ast, sMethodStmt);
        }
        sMethod = strsGetFirstToken(&buffs, sLastStmt, '(');
        char* sSubStmts = strsCut(&buffs, sLastStmt, '(', ')');
        if (NULL == sSubStmts) {
            eResult = PIKA_RES_ERR_SYNTAX_ERROR;
            goto __exit;
        }
        /* add ',' at the end */
        sSubStmts = strsAppend(&buffs, sSubStmts, ",");
        int iSubStmtsNum = Parser_getSubStmtNum(sSubStmts, ",");
        for (int i = 0; i < iSubStmtsNum; i++) {
            char* substmt = Parser_popSubStmt(&buffs, &sSubStmts, ",");
            AST_parseSubStmt(ast, substmt);
            if (strOnly(sSubStmts, ',')) {
                if (i < iSubStmtsNum - 2) {
                    eResult = PIKA_RES_ERR_SYNTAX_ERROR;
                    goto __exit;
                }
                if (i == iSubStmtsNum - 2 && strEqu(sMethod, "")) {
                    sRealType = "tuple";
                }
                break;
            }
            if (strEqu("", sSubStmts)) {
                if (i != iSubStmtsNum - 1) {
                    eResult = PIKA_RES_ERR_SYNTAX_ERROR;
                    goto __exit;
                }
                break;
            }
        }
        AST_setNodeAttr(ast, (char*)sRealType, sMethod);
        goto __exit;
    }
    /* solve reference stmt */
    if (STMT_reference == eStmtType) {
        sRef = sRight;
        /* filter for type hint */
        sRef = Cursor_splitCollect(&buffs, sRef, ":", 0);
        if (!strEqu(sRef, sRight)) {
            goto __exit;
        }
        AST_setNodeAttr(ast, (char*)"ref", sRef);
        goto __exit;
    }
    /* solve import stmt */
    if (STMT_import == eStmtType) {
        sImport = strsGetLastToken(&buffs, sRight, ' ');
        AST_setNodeAttr(ast, (char*)"import", sImport);
        goto __exit;
    }
    /* solve @inh stmt (from <module> import *) */
    if (STMT_inhert == eStmtType) {
        sInhert = strsGetLastToken(&buffs, sRight, ' ');
        AST_setNodeAttr(ast, (char*)"inhert", sInhert);
        goto __exit;
    }
    /* solve str/bytes stmt */
    if (STMT_string == eStmtType || STMT_bytes == eStmtType) {
        sStr = strsCopy(&buffs, sRight);
        /* remove the first char */
        char firstChar = sStr[0];
        switch (eStmtType) {
            case STMT_string:
                sStr = sStr + 1;
                break;
            case STMT_bytes:
                sStr = sStr + 2;
                break;
            default:
                // never reach
                pika_assert(0);
        }
        /* remove the last char */
        sStr[strGetSize(sStr) - 1] = '\0';
        /* replace */
        if (strIsContain(sStr, '\\')) {
            switch (firstChar) {
                case '\'':
                    sStr = strsReplace(&buffs, sStr, "\\\'", "\'");
                    break;
                case '\"':
                    sStr = strsReplace(&buffs, sStr, "\\\"", "\"");
                    break;
            }
        }
        if (STMT_string == eStmtType) {
            AST_setNodeAttr(ast, (char*)"string", sStr);
        } else if (STMT_bytes == eStmtType) {
            AST_setNodeAttr(ast, (char*)"bytes", sStr);
        }
        goto __exit;
    }
    /* solve number stmt */
    if (STMT_number == eStmtType) {
        sNum = sRight;
        AST_setNodeAttr(ast, (char*)"num", sNum);
        goto __exit;
    }
__exit:
    strsDeinit(&buffs);
    if (eResult != PIKA_RES_OK) {
        AST_deinit(ast);
        return NULL;
    }
    return ast;
}

static int32_t Parser_getPyLineBlockDeepth(char* sLine) {
    int32_t iSpaceNum = strGetIndent(sLine);
    if (0 == iSpaceNum % PIKA_BLOCK_SPACE) {
        return iSpaceNum / PIKA_BLOCK_SPACE;
    }
    /* space Num is not 4N, error*/
    return -1;
}

char* Parser_removeComment(char* sLine) {
    pika_bool bAnnotationExit = 0;
    pika_bool bInSingleQuotes = 0;
    pika_bool bInDoubleQuotesDeepth = 0;
    pika_bool bPrevCharWasBackslash = 0;

    for (uint32_t i = 0; i < strGetSize(sLine); i++) {
        if (bPrevCharWasBackslash) {
            bPrevCharWasBackslash = 0;
            continue;
        }

        if ('\\' == sLine[i]) {
            bPrevCharWasBackslash = 1;
            continue;
        }

        if ('\'' == sLine[i] && !bInDoubleQuotesDeepth) {
            bInSingleQuotes = !bInSingleQuotes;
            continue;
        }
        if ('"' == sLine[i] && !bInSingleQuotes) {
            bInDoubleQuotesDeepth = !bInDoubleQuotesDeepth;
            continue;
        }
        if (!(bInSingleQuotes == 0 && bInDoubleQuotesDeepth == 0)) {
            continue;
        }
        if ('#' == sLine[i]) {
            sLine[i] = 0;
            bAnnotationExit = 1;
            break;
        }
    }

    if (!bAnnotationExit) {
        return sLine;
    }

    for (uint32_t i = 0; i < strGetSize(sLine); i++) {
        if (' ' != sLine[i]) {
            return sLine;
        }
    }

    sLine = "@annotation";
    return sLine;
}

char* _defGetDefault(Args* outBuffs, char** sDeclearOut_p) {
#if PIKA_NANO_ENABLE
    return "";
#endif
    Args buffs = {0};
    char* sDeclear = strsCopy(&buffs, *sDeclearOut_p);
    char* sFnName = strsGetFirstToken(&buffs, sDeclear, '(');
    Arg* aDeclear = arg_strAppend(arg_newStr(sFnName), "(");
    Arg* aDefault = arg_newStr("");
    char* sArgList = strsCut(&buffs, sDeclear, '(', ')');
    char* sDefaultOut = NULL;
    pika_assert(NULL != sArgList);
    int iArgNum = _Cursor_count(sArgList, TOKEN_devider, ",", pika_true) + 1;
    for (int i = 0; i < iArgNum; i++) {
        char* sItem = Cursor_popToken(&buffs, &sArgList, ",");
        char* sDefaultVal = NULL;
        char* sDefaultKey = NULL;
        pika_bool bDefault = 0;
        if (Cursor_isContain(sItem, TOKEN_operator, "=")) {
            /* has default value */
            sDefaultVal = Cursor_splitCollect(&buffs, sItem, "=", 1);
            sDefaultKey = Cursor_splitCollect(&buffs, sItem, "=", 0);
            sDefaultKey = Cursor_splitCollect(&buffs, sDefaultKey, ":", 0);
            aDefault = arg_strAppend(aDefault, sDefaultKey);
            aDefault = arg_strAppend(aDefault, "=");
            aDefault = arg_strAppend(aDefault, sDefaultVal);
            aDefault = arg_strAppend(aDefault, ",");
            bDefault = 1;
        } else {
            sDefaultKey = sItem;
        }
        aDeclear = arg_strAppend(aDeclear, sDefaultKey);
        if (bDefault) {
            aDeclear = arg_strAppend(aDeclear, "=");
        }
        aDeclear = arg_strAppend(aDeclear, ",");
    }
    strPopLastToken(arg_getStr(aDeclear), ',');
    aDeclear = arg_strAppend(aDeclear, ")");
    *sDeclearOut_p = strsCopy(outBuffs, arg_getStr(aDeclear));
    sDefaultOut = strsCopy(outBuffs, arg_getStr(aDefault));
    strPopLastToken(sDefaultOut, ',');
    arg_deinit(aDeclear);
    arg_deinit(aDefault);
    strsDeinit(&buffs);
    return sDefaultOut;
}

static char* Suger_multiReturn(Args* out_buffs, char* sLine) {
#if PIKA_NANO_ENABLE
    return sLine;
#endif
    Cursor_forEach(cs, sLine) {
        Cursor_iterStart(&cs);
        if (cs.bracket_deepth == 0 && strEqu(cs.token1.pyload, ",")) {
            sLine = strsFormat(out_buffs, strGetSize(sLine) + 3, "(%s)", sLine);
            Cursor_iterEnd(&cs);
            break;
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    return sLine;
}

/* match block start keywords */
const char control_keywords[][9] = {"break", "continue"};

/* normal keyward */
const char normal_keywords[][7] = {"while", "if", "elif"};

AST* parser_line2Ast(Parser* self, char* sLine) {
    BlockState* blockState = &self->blockState;
    /* line is not exist */
    if (sLine == NULL) {
        return NULL;
    }

    /* init data */
    AST* oAst = AST_create();
    Args buffs = {0};
    int8_t iBlockDeepthNow, iBlockDeepthLast = -1;
    char *sLineStart, *sStmt;

    /* docsting */
    if (strIsStartWith(sLine, "@docstring")) {
        AST_setNodeAttr(oAst, "docstring", sLine + sizeof("@docstring"));
        sStmt = "";
        goto __block_matched;
    }

    /* get block deepth */
    iBlockDeepthNow = Parser_getPyLineBlockDeepth(sLine);
    if (self->blockDeepthOrigin == _VAL_NEED_INIT) {
        self->blockDeepthOrigin = iBlockDeepthNow;
    }
    /* set block deepth */
    if (iBlockDeepthNow == -1) {
        /* get block_deepth error */
        pika_platform_printf(
            "IndentationError: unexpected indent, only support 4 "
            "spaces\r\n");
        obj_deinit(oAst);
        oAst = NULL;
        goto __exit;
    }
    iBlockDeepthNow += blockState->deepth;
    obj_setInt(oAst, "blockDeepth", iBlockDeepthNow);

    /* check if exit block */
    if (0 != stack_getTop(blockState->stack)) {
        iBlockDeepthLast = stack_getTop(blockState->stack) +
                           blockState->deepth + self->blockDeepthOrigin;
        /* exit each block */
        for (int i = 0; i < iBlockDeepthLast - iBlockDeepthNow; i++) {
            QueueObj* exit_block_queue = obj_getObj(oAst, "exitBlock");
            /* create an exit_block queue */
            if (NULL == exit_block_queue) {
                obj_newObj(oAst, "exitBlock", "", New_TinyObj);
                exit_block_queue = obj_getObj(oAst, "exitBlock");
                queueObj_init(exit_block_queue);
            }
            char buff[10] = {0};
            char* sBlockType = stack_popStr(blockState->stack, buff);
            /* push exit block type to exit_block queue */
            queueObj_pushStr(exit_block_queue, sBlockType);
        }
    }

    sLineStart = sLine + (iBlockDeepthNow - blockState->deepth) * 4;
    sStmt = sLineStart;

    // "while" "if" "elif"
    for (uint32_t i = 0; i < sizeof(normal_keywords) / 7; i++) {
        char* sKeyword = (char*)normal_keywords[i];
        uint8_t sKeywordLen = strGetSize(sKeyword);
        if (strIsStartWith(sLineStart, sKeyword) &&
            (sLineStart[sKeywordLen] == ' ')) {
            sStmt = strsCut(&buffs, sLineStart, ' ', ':');
            AST_setNodeBlock(oAst, sKeyword);
            stack_pushStr(blockState->stack, sKeyword);
            goto __block_matched;
        }
    }

    /* contral keyward */
    /* "break", "continue" */
    for (uint32_t i = 0; i < sizeof(control_keywords) / 8; i++) {
        char* sKeyward = (char*)control_keywords[i];
        uint8_t keyward_size = strGetSize(sKeyward);
        if ((strIsStartWith(sLineStart, sKeyward)) &&
            ((sLineStart[keyward_size] == ' ') ||
             (sLineStart[keyward_size] == 0))) {
            AST_setNodeAttr(oAst, sKeyward, "");
            sStmt = "";
            goto __block_matched;
        }
    }

    /* for */
    if (strIsStartWith(sLineStart, "for ")) {
        Args* list_buffs = New_strBuff();
        char* sLineBuff = strsCopy(list_buffs, sLineStart + 4);
        sLineBuff = Cursor_getCleanStmt(list_buffs, sLineBuff);
        if (strCountSign(sLineBuff, ':') < 1) {
            args_deinit(list_buffs);
            obj_deinit(oAst);
            oAst = NULL;
            goto __exit;
        }
        char* sArgIn = strsPopToken(list_buffs, &sLineBuff, ' ');
        AST_setNodeAttr(oAst, "arg_in", sArgIn);
        strsPopToken(list_buffs, &sLineBuff, ' ');
        char* sListIn = Cursor_splitCollect(list_buffs, sLineBuff, ":", 0);
        sListIn = strsAppend(list_buffs, "iter(", sListIn);
        sListIn = strsAppend(list_buffs, sListIn, ")");
        sListIn = strsCopy(&buffs, sListIn);
        args_deinit(list_buffs);
        AST_setNodeBlock(oAst, "for");
        AST_setNodeAttr(oAst, "list_in", sListIn);
        stack_pushStr(blockState->stack, "for");
        sStmt = sListIn;
        goto __block_matched;
    }

    /* else */
    if (strIsStartWith(sLineStart, "else")) {
        if ((sLineStart[4] == ' ') || (sLineStart[4] == ':')) {
            sStmt = "";
            AST_setNodeBlock(oAst, "else");
            stack_pushStr(blockState->stack, "else");
        }
        goto __block_matched;
    }

#if PIKA_SYNTAX_EXCEPTION_ENABLE
    /* try */
    if (strIsStartWith(sLineStart, "try")) {
        if ((sLineStart[3] == ' ') || (sLineStart[3] == ':')) {
            sStmt = "";
            AST_setNodeBlock(oAst, "try");
            stack_pushStr(blockState->stack, "try");
        }
        goto __block_matched;
    }

    /* except */
    if (strIsStartWith(sLineStart, "except")) {
        if ((sLineStart[6] == ' ') || (sLineStart[6] == ':')) {
            sStmt = "";
            AST_setNodeBlock(oAst, "except");
            stack_pushStr(blockState->stack, "except");
        }
        goto __block_matched;
    }
#endif

    if (strEqu(sLineStart, "return")) {
        AST_setNodeAttr(oAst, "return", "");
        sStmt = "";
        goto __block_matched;
    }
    if (strIsStartWith(sLineStart, "return ")) {
        char* lineBuff = strsCopy(&buffs, sLineStart);
        strsPopToken(&buffs, &lineBuff, ' ');
        sStmt = lineBuff;
        sStmt = Suger_multiReturn(&buffs, sStmt);
        AST_setNodeAttr(oAst, "return", "");
        goto __block_matched;
    }

#if PIKA_SYNTAX_EXCEPTION_ENABLE
    if (strEqu(sLineStart, "raise")) {
        AST_setNodeAttr(oAst, "raise", "");
        sStmt = "RuntimeError";
        goto __block_matched;
    }
    if (strIsStartWith(sLineStart, "raise ")) {
        AST_setNodeAttr(oAst, "raise", "");
        char* sLineBuff = strsCopy(&buffs, sLineStart);
        strsPopToken(&buffs, &sLineBuff, ' ');
        sStmt = sLineBuff;
        if (strEqu("", sStmt)) {
            sStmt = "RuntimeError";
        }
        goto __block_matched;
    }
    /* assert */
    if (strIsStartWith(sLineStart, "assert ")) {
        sStmt = "";
        AST_setNodeAttr(oAst, "assert", "");
        char* sLineBuff = strsCopy(&buffs, sLineStart + 7);
        /* assert expr [, msg] */
        while (1) {
            char* sSubStmt = Parser_popSubStmt(&buffs, &sLineBuff, ",");
            AST_parseSubStmt(oAst, sSubStmt);
            if (strEqu(sLineBuff, "")) {
                break;
            }
        }
        goto __block_matched;
    }
#endif

    if (strIsStartWith(sLineStart, "global ")) {
        sStmt = "";
        char* sGlobalList = sLineStart + 7;
        sGlobalList = Cursor_getCleanStmt(&buffs, sGlobalList);
        AST_setNodeAttr(oAst, "global", sGlobalList);
        goto __block_matched;
    }
    if (strIsStartWith(sLineStart, "del ") ||
        strIsStartWith(sLineStart, "del(")) {
        sStmt = "";
        char* sDelDir = strsCut(&buffs, sLineStart, '(', ')');
        if (!sDelDir) {
            sDelDir = sLineStart + sizeof("del ") - 1;
        }
        sDelDir = Cursor_getCleanStmt(&buffs, sDelDir);
        AST_setNodeAttr(oAst, "del", sDelDir);
        goto __block_matched;
    }
    if (strIsStartWith(sLineStart, (char*)"def ")) {
        sStmt = "";
        char* sDeclare = strsCut(&buffs, sLineStart, ' ', ':');
        if (NULL == sDeclare) {
            obj_deinit(oAst);
            oAst = NULL;
            goto __exit;
        }
        sDeclare = Cursor_getCleanStmt(&buffs, sDeclare);
        AST_setNodeAttr(oAst, "raw", sDeclare);
        if (!strIsContain(sDeclare, '(') || !strIsContain(sDeclare, ')')) {
            obj_deinit(oAst);
            oAst = NULL;
            goto __exit;
        }
        char* sDefaultStmt = _defGetDefault(&buffs, &sDeclare);
        AST_setNodeBlock(oAst, "def");
        AST_setNodeAttr(oAst, "declare", sDeclare);
        if (sDefaultStmt[0] != '\0') {
            AST_setNodeAttr(oAst, "default", sDefaultStmt);
        }
        stack_pushStr(blockState->stack, "def");
        goto __block_matched;
    }
    if (strIsStartWith(sLineStart, (char*)"class ")) {
        sStmt = "";
        char* sDeclare = strsCut(&buffs, sLineStart, ' ', ':');
        if (NULL == sDeclare) {
            obj_deinit(oAst);
            oAst = NULL;
            goto __exit;
        }
        sDeclare = Cursor_getCleanStmt(&buffs, sDeclare);
        AST_setNodeBlock(oAst, "class");
        AST_setNodeAttr(oAst, "declare", sDeclare);
        stack_pushStr(blockState->stack, "class");
        goto __block_matched;
    }

__block_matched:
    if (NULL == sStmt) {
        AST_deinit(oAst);
        oAst = NULL;
        goto __exit;
    }
    sStmt = Cursor_getCleanStmt(&buffs, sStmt);
    oAst = AST_parseStmt(oAst, sStmt);
    goto __exit;
__exit:
    strsDeinit(&buffs);
    return oAst;
}

static AST* line2Ast_withBlockDeepth(char* sLine, int iBlockDeepth) {
    Parser* parser = parser_create();
    parser->blockState.deepth = iBlockDeepth;
    AST* ast = parser_line2Ast(parser, sLine);
    parser_deinit(parser);
    return ast;
}

int AST_getBlockDeepthNow(AST* ast) {
    return obj_getInt(ast, "blockDeepth");
}

AST* line2Ast(char* sLine) {
    return line2Ast_withBlockDeepth(sLine, 0);
}

static char* Suger_import_as(Args* out_buffs, char* sLine) {
#if !PIKA_SYNTAX_IMPORT_EX_ENABLE
    return sLine;
#endif
    Args buffs = {0};
    char* sLineOut = sLine;
    char* sAlias = NULL;
    char* sOrigin = NULL;
    char* sStmt = sLine + 7;

    /* not import, exit */
    if (!strIsStartWith(sLine, "import ")) {
        sLineOut = sLine;
        goto __exit;
    }

    if (!Cursor_isContain(sStmt, TOKEN_operator, " as ")) {
        sLineOut = sLine;
        goto __exit;
    }

    /* {origin} as {alias} */
    sOrigin = Cursor_popToken(&buffs, &sStmt, " as ");
    sAlias = sStmt;

    /* 'import' and 'as' */
    sLineOut = strsFormat(&buffs, PIKA_LINE_BUFF_SIZE, "import %s\n%s = %s",
                          sOrigin, sAlias, sOrigin);
__exit:
    return strsReturnOut(&buffs, out_buffs, sLineOut);
}

static pika_bool _check_is_multi_assign(char* sArgList) {
#if PIKA_NANO_ENABLE
    return pika_false;
#endif
    pika_bool bRes = pika_false;
    Cursor_forEach(cs, sArgList) {
        Cursor_iterStart(&cs);
        if ((cs.bracket_deepth == 0 && strEqu(cs.token1.pyload, ","))) {
            bRes = pika_true;
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    return bRes;
}

Arg* arg_strAddIndent(Arg* aStrIn, int indent) {
    if (0 == indent) {
        return aStrIn;
    }
    /* add space */
    char* sSpaces = pikaMalloc(indent + 1);
    pika_platform_memset(sSpaces, ' ', indent);
    sSpaces[indent] = '\0';
    Arg* aRet = arg_newStr(sSpaces);
    aRet = arg_strAppend(aRet, arg_getStr(aStrIn));
    pikaFree(sSpaces, indent + 1);
    arg_deinit(aStrIn);
    return aRet;
}

Arg* arg_strAddIndentMulti(Arg* aStrInMuti, int indent) {
    if (0 == indent) {
        return aStrInMuti;
    }
    char* sStrInMuti = arg_getStr(aStrInMuti);
    char* sLine = NULL;
    int iLineNum = strGetLineNum(sStrInMuti);
    Arg* aStrOut = arg_newStr("");
    Args buffs = {0};
    for (int i = 0; i < iLineNum; i++) {
        sLine = strsPopLine(&buffs, &sStrInMuti);
        Arg* aLine = arg_newStr(sLine);
        aLine = arg_strAddIndent(aLine, indent);
        sLine = arg_getStr(aLine);
        aStrOut = arg_strAppend(aStrOut, sLine);
        if (i != iLineNum - 1) {
            aStrOut = arg_strAppend(aStrOut, "\n");
        }
        arg_deinit(aLine);
    }
    strsDeinit(&buffs);
    arg_deinit(aStrInMuti);
    return aStrOut;
}

static char* Suger_multiAssign(Args* out_buffs, char* sLine) {
#if PIKA_NANO_ENABLE
    return sLine;
#endif
    if (!strIsContain(sLine, '=') || !strIsContain(sLine, ',')) {
        return sLine;
    }
    Args buffs = {0};
    char* sLineOut = sLine;
    int iIndent = strGetIndent(sLine);
    pika_bool bAssign = pika_false;
    Arg* aStmt = arg_newStr("");
    Arg* aOutList = arg_newStr("");
    Arg* aOutItem = arg_newStr("");
    Arg* aLineOut = arg_newStr("");
    char* sLineItem = NULL;
    char* sOutList = NULL;
    int iOutNum = 0;
    Cursor_forEach(cs, sLine) {
        Cursor_iterStart(&cs);
        if (cs.bracket_deepth == 0 && strEqu(cs.token1.pyload, "=")) {
            bAssign = pika_true;
            Cursor_iterEnd(&cs);
            continue;
        }
        if (bAssign) {
            aStmt = arg_strAppend(aStmt, cs.token1.pyload);
        }
        if (!bAssign) {
            aOutList = arg_strAppend(aOutList, cs.token1.pyload);
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    if (!bAssign) {
        sLineOut = sLine;
        goto __exit;
    }

    if (!_check_is_multi_assign(arg_getStr(aOutList))) {
        sLineOut = sLine;
        goto __exit;
    }

    sLineItem = strsFormat(&buffs, PIKA_LINE_BUFF_SIZE, "$tmp= %s\n",
                           arg_getStr(aStmt));

    /* add space */
    aLineOut = arg_strAppend(aLineOut, sLineItem);

    sOutList = arg_getStr(aOutList);
    while (1) {
        char* item = Cursor_popToken(&buffs, &sOutList, ",");
        if (item[0] == '\0') {
            break;
        }
        char* sLineItem = strsFormat(&buffs, PIKA_LINE_BUFF_SIZE,
                                     "%s = $tmp[%d]\n", item, iOutNum);
        /* add space */
        aLineOut = arg_strAppend(aLineOut, sLineItem);
        iOutNum++;
    }
    /* add space */
    aLineOut = arg_strAppend(aLineOut, "del $tmp");
    aLineOut = arg_strAddIndentMulti(aLineOut, iIndent);
    sLineOut = strsCopy(out_buffs, arg_getStr(aLineOut));
    goto __exit;
__exit:
    arg_deinit(aStmt);
    arg_deinit(aOutList);
    arg_deinit(aOutItem);
    arg_deinit(aLineOut);
    strsDeinit(&buffs);
    return sLineOut;
}

static char* Suger_from_import_as(Args* buffs_p, char* sLine) {
#if !PIKA_SYNTAX_IMPORT_EX_ENABLE
    return sLine;
#endif
    Args buffs = {0};
    Arg* aLineOut = NULL;
    char* sLineOut = sLine;
    char* sClass = NULL;
    char* sModule = NULL;
    char* sAlias = NULL;
    char* sStmt = sLine + 5;
    char* sClassAfter = "";

    if (!strIsStartWith(sLine, "from ")) {
        sLineOut = sLine;
        goto __exit;
    }

    sModule = Cursor_popToken(&buffs, &sStmt, " import ");
    if (!Cursor_isContain(sStmt, TOKEN_operator, " as ")) {
        sClass = sStmt;
    } else {
        sClass = Cursor_popToken(&buffs, &sStmt, " as ");
        sAlias = sStmt;
    }

    if (NULL == sAlias) {
        sAlias = sClass;
    }

    /* skip PikaObj */
    if (strEqu(sModule, "PikaObj")) {
        sLineOut = strsCopy(buffs_p, "");
        goto __exit;
    }

    /* solve from module import * */
    if (strEqu(sClass, "*")) {
        sLineOut =
            strsFormat(&buffs, PIKA_LINE_BUFF_SIZE, "@inh %s\n", sModule);
        sLineOut = strsCopy(buffs_p, sLineOut);
        goto __exit;
    }

    while (1) {
        char* sClassItem = Cursor_popToken(&buffs, &sClass, ",");
        if (sClassItem[0] == '\0') {
            break;
        }
        sClassItem = strsFormat(&buffs, PIKA_LINE_BUFF_SIZE, "%s.%s,", sModule,
                                sClassItem);
        sClassAfter = strsAppend(&buffs, sClassAfter, sClassItem);
    }
    sClassAfter[strGetSize(sClassAfter) - 1] = '\0';
    sClass = sClassAfter;
    aLineOut = arg_newStr("import ");
    aLineOut = arg_strAppend(aLineOut, sClass);
    aLineOut = arg_strAppend(aLineOut, "\n");
    aLineOut = arg_strAppend(aLineOut, sAlias);
    aLineOut = arg_strAppend(aLineOut, " = ");
    aLineOut = arg_strAppend(aLineOut, sClass);
    sLineOut = arg_getStr(aLineOut);
    sLineOut = strsCopy(buffs_p, sLineOut);
__exit:
    if (NULL != aLineOut) {
        arg_deinit(aLineOut);
    }
    strsDeinit(&buffs);
    return sLineOut;
}

static char* Suger_import(Args* outbuffs, char* sLine) {
#if !PIKA_SYNTAX_IMPORT_EX_ENABLE
    return sLine;
#endif
    sLine = Suger_import_as(outbuffs, sLine);
    sLine = Suger_from_import_as(outbuffs, sLine);
    Arg* aLineBuff = arg_newStr("");
    while (1) {
        char* sSingleLine = strPopFirstToken(&sLine, '\n');
        if (sSingleLine[0] == '\0') {
            break;
        }
        if (strIsStartWith(sSingleLine, "import ")) {
            if (strIsContain(sSingleLine, ',')) {
                sSingleLine = sSingleLine + 7;
                while (1) {
                    char* single_import = strPopFirstToken(&sSingleLine, ',');
                    if (single_import[0] == '\0') {
                        break;
                    }
                    aLineBuff = arg_strAppend(aLineBuff, "import ");
                    aLineBuff = arg_strAppend(aLineBuff, single_import);
                    aLineBuff = arg_strAppend(aLineBuff, "\n");
                }
                char* sLineAfter = arg_getStr(aLineBuff);
                sLineAfter[strlen(sLineAfter) - 1] = '\0';
            }
        }
        aLineBuff = arg_strAppend(aLineBuff, sSingleLine);
        aLineBuff = arg_strAppend(aLineBuff, "\n");
    }
    char* sLineAfter = arg_getStr(aLineBuff);
    sLineAfter[strlen(sLineAfter) - 1] = '\0';
    sLineAfter = strsCopy(outbuffs, sLineAfter);
    arg_deinit(aLineBuff);
    return sLineAfter;
}

static char* Suger_semicolon(Args* outbuffs, char* sLine) {
    Args buffs = {0};
    char* sStmt = sLine;
    char* sStmtAfter = "";
    if (Cursor_count(sLine, TOKEN_devider, ";") < 1) {
        return sLine;
    }
    while (1) {
        char* sStmtItem = Cursor_popToken(&buffs, &sStmt, ";");
        if (sStmtItem[0] == '\0') {
            break;
        };
        sStmtItem = strsAppend(&buffs, sStmtItem, "\n");
        sStmtAfter = strsAppend(&buffs, sStmtAfter, sStmtItem);
    }
    sStmtAfter[strGetSize(sStmtAfter) - 1] = '\0';
    sStmtAfter = strsCopy(outbuffs, sStmtAfter);
    strsDeinit(&buffs);
    return sStmtAfter;
}

typedef char* (*Suger_processor)(Args*, char*);
const Suger_processor Suger_processor_list[] = {Suger_import, Suger_semicolon,
                                                Suger_multiAssign};

static char* Parser_sugerProcessOnce(Args* outbuffs, char* sLine) {
    for (uint32_t i = 0; i < sizeof(Suger_processor_list) / sizeof(void*);
         i++) {
        sLine = Suger_processor_list[i](outbuffs, sLine);
        if (strCountSign(sLine, '\n') > 1) {
            break;
        }
    }
    return sLine;
}

static char* Parser_sugerProcess(Args* outbuffs, char* sLine) {
    /* process import */
    int32_t block_deepth = Parser_getPyLineBlockDeepth(sLine);
    if (block_deepth < 0) {
        return NULL;
    }
    char* sLineOrigin = sLine;
    sLine = sLine + block_deepth * PIKA_BLOCK_SPACE;
    sLine = Parser_sugerProcessOnce(outbuffs, sLine);
    if (strEqu(sLineOrigin, sLine)) {
        return sLine;
    }
    /* process multi assign */
    int iLineNum = strCountSign(sLine, '\n') + 1;
    Arg* aLine = arg_newStr("");
    for (int i = 0; i < iLineNum; i++) {
        if (i > 0) {
            aLine = arg_strAppend(aLine, "\n");
        }
        char* sSingleLine = strsPopToken(outbuffs, &sLine, '\n');
        sSingleLine = Parser_sugerProcess(outbuffs, sSingleLine);
        aLine = arg_strAppend(aLine, sSingleLine);
    }
    sLine = strsCopy(outbuffs, arg_getStr(aLine));
    sLine =
        strsAddIndentation(outbuffs, sLine, block_deepth * PIKA_BLOCK_SPACE);
    if (NULL != aLine) {
        arg_deinit(aLine);
    }
    return sLine;
}

static char* Parser_linePreProcess(Args* outbuffs, char* sLine) {
    sLine = Parser_removeComment(sLine);

    /* check syntex error */
    if (Lexer_isError(sLine)) {
        sLine = NULL;
        goto __exit;
    }
    /* process EOL */
    sLine = strsDeleteChar(outbuffs, sLine, '\r');
    /* process syntax sugar */
    sLine = Parser_sugerProcess(outbuffs, sLine);
__exit:
    return sLine;
}

char* parser_line2Target(Parser* self, char* sLine) {
    char* sOut = NULL;
    AST* oAst = NULL;
    uint8_t uLineNum = 0;
    /* docsting */
    if (strIsStartWith(sLine, "@docstring")) {
        oAst = parser_line2Ast(self, sLine);
        char* sBackendCode = self->fn_ast2Target(self, oAst);
        if (NULL == oAst) {
            sOut = "";
            goto __exit;
        }
        AST_deinit(oAst);
        sOut = sBackendCode;
        goto __exit;
    }
    /* pre process */
    sLine = Parser_linePreProcess(&self->lineBuffs, sLine);
    if (NULL == sLine) {
        /* preprocess error */
        goto __exit;
    }
    if (strEqu("@annotation", sLine)) {
        sOut = "";
        goto __exit;
    }
    /*
        solve more lines
        preprocess may generate more lines
    */
    uLineNum = strCountSign(sLine, '\n') + 1;
    for (int i = 0; i < uLineNum; i++) {
        char* sSingleLine = strsPopToken(&self->lineBuffs, &sLine, '\n');
        /* parse line to AST */
        oAst = parser_line2Ast(self, sSingleLine);
        if (NULL == oAst) {
            /* parse error */
            goto __exit;
        }
        /* gen ASM from AST */
        char* sBackendCode = self->fn_ast2Target(self, oAst);
        if (sOut == NULL) {
            sOut = sBackendCode;
        } else {
            sOut = strsAppend(&self->lineBuffs, sOut, sBackendCode);
        }
        if (NULL != oAst) {
            AST_deinit(oAst);
        }
    }
__exit:
    return sOut;
}

static int Parser_isVoidLine(char* sLine) {
    for (uint32_t i = 0; i < strGetSize(sLine); i++) {
        if (sLine[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

static uint8_t Parser_checkIsDocstring(char* line,
                                       Args* outbuffs,
                                       uint8_t bIsInDocstring,
                                       uint8_t* pbIsSingleDocstring,
                                       char** psDocstring) {
    pika_bool bIsDocstring = 0;
    uint32_t i = 0;
    int32_t iDocstringStart = 0;
    int32_t iDocstringEnd = -1;
    uint32_t uLineSize = strGetSize(line);
    char* sDocstring = NULL;
    while (i + 2 < uLineSize) {
        /* not match ' or " */
        if ((line[i] != '\'') && (line[i] != '"')) {
            if (!bIsDocstring && !bIsInDocstring && !charIsBlank(line[i])) {
                break;
            }
            i++;
            continue;
        }
        /* not match ''' or """ */
        if (!((line[i + 1] == line[i]) && (line[i + 2] == line[i]))) {
            i++;
            continue;
        }
        if (bIsDocstring) {
            *pbIsSingleDocstring = 1;
            iDocstringEnd = i;
            break;
        }
        bIsDocstring = 1;
        if (bIsInDocstring) {
            iDocstringEnd = i;
        } else {
            iDocstringStart = i + 3;
        }
        i++;
    }
    if (bIsDocstring) {
        sDocstring = strsCopy(outbuffs, line + iDocstringStart);
        if (iDocstringEnd != -1) {
            sDocstring[iDocstringEnd - iDocstringStart] = '\0';
        }
        *psDocstring = sDocstring;
    }
    return bIsDocstring;
}

char* parser_lines2Target(Parser* self, char* sPyLines) {
    Arg* aBackendCode = arg_newStr("");
    Arg* aLineConnection = arg_newStr("");
    Arg* aDocstring = arg_newStr("");
    uint32_t uLinesOffset = 0;
    uint16_t uLinesNum = strCountSign(sPyLines, '\n') + 1;
    uint16_t uLinesIndex = 0;
    uint8_t bIsInDocstring = 0;
    uint8_t bIsSingleDocstring = 0;
    uint8_t bIsLineConnection = 0;
    uint8_t bIsLineConnectionForBracket = 0;
    char* sOut = NULL;
    char* sBackendCode = NULL;
    char* sDocstring = NULL;
    uint32_t uLineSize = 0;
    /* parse each line */
    while (1) {
        uLinesIndex++;
        char* sLineOrigin = NULL;
        char* sLine = NULL;

        /* add void line to the end */
        if (uLinesIndex >= uLinesNum + 1) {
            sLine = "";
            goto __parse_line;
        }

        /* get single line by pop multiline */
        sLineOrigin =
            strsGetFirstToken(&self->lineBuffs, sPyLines + uLinesOffset, '\n');

        sLine = strsCopy(&self->lineBuffs, sLineOrigin);

        /* line connection */
        if (bIsLineConnection) {
            if (bIsLineConnectionForBracket) {
                sLine = Parser_removeComment(sLine);
                if (strEqu(sLine, "@annotation")) {
                    sLine = "";
                }
            }
            aLineConnection = arg_strAppend(aLineConnection, sLine);
            sLine = strsCopy(&self->lineBuffs, arg_getStr(aLineConnection));
            /* reflash the line_connection_arg */
            arg_deinit(aLineConnection);
            aLineConnection = arg_newStr("");
            bIsLineConnection = 0;
            bIsLineConnectionForBracket = 0;
        }

        /* check connection */
        if ('\\' == sLine[strGetSize(sLine) - 1]) {
            /* remove the '\\' */
            sLine[strGetSize(sLine) - 1] = '\0';
            bIsLineConnection = 1;
            aLineConnection = arg_strAppend(aLineConnection, sLine);
            goto __next_line;
        }

        /* filter for not end \n */
        if (Parser_isVoidLine(sLine)) {
            goto __next_line;
        }

        /* filter for docstring ''' or """ */
        if (Parser_checkIsDocstring(sLine, &self->lineBuffs, bIsInDocstring,
                                    &bIsSingleDocstring, &sDocstring)) {
            bIsInDocstring = ~bIsInDocstring;
            if (sDocstring[0] != '\0') {
                aDocstring = arg_strAppend(aDocstring, sDocstring);
                aDocstring = arg_strAppend(aDocstring, "\n");
            }
            /* one line docstring */
            if (bIsSingleDocstring) {
                bIsInDocstring = 0;
                bIsSingleDocstring = 0;
            }
            if (!bIsInDocstring) {
                /* multi line docstring */
                sLine = strsAppend(&self->lineBuffs, "@docstring\n",
                                   arg_getStr(aDocstring));
                /* reflash the docstring_arg */
                arg_deinit(aDocstring);
                aDocstring = arg_newStr("");
                goto __parse_line;
            }
            goto __next_line;
        }

        /* skip docstring */
        if (bIsInDocstring) {
            aDocstring = arg_strAppend(aDocstring, sLine);
            aDocstring = arg_strAppend(aDocstring, "\n");
            goto __next_line;
        }

        /* support Tab */
        sLine = strsReplace(&self->lineBuffs, sLine, "\t", "    ");
        /* remove \r */
        sLine = strsReplace(&self->lineBuffs, sLine, "\r", "");

        /* check auto connection */
        Cursor_forEach(c, sLine) {
            Cursor_iterStart(&c);
            Cursor_iterEnd(&c);
        }
        Cursor_deinit(&c);
        /* auto connection */
        if (uLinesIndex < uLinesNum) {
            if (c.bracket_deepth > 0) {
                aLineConnection = arg_strAppend(aLineConnection, sLine);
                bIsLineConnection = 1;
                bIsLineConnectionForBracket = 1;
                goto __next_line;
            }
        }

        /* bracket match failed */
        if (c.bracket_deepth != 0) {
            sBackendCode = NULL;
            goto __parse_after;
        }

    __parse_line:
        /* parse single Line to Asm */
        if (strEqu(sLine, "#!label")) {
            self->bytecode_frame->label_pc =
                self->bytecode_frame->instruct_array.size;
            goto __next_line;
        }
        sBackendCode = parser_line2Target(self, sLine);
    __parse_after:
        if (NULL == sBackendCode) {
            sOut = NULL;
            pika_platform_printf(
                "----------[%d]----------\r\n%s\r\n-------------------------"
                "\r\n",
                uLinesIndex, sLine);
            strsDeinit(&self->lineBuffs);
            goto __exit;
        }

        if (self->isGenBytecode) {
            /* store ByteCode */
            byteCodeFrame_appendFromAsm(self->bytecode_frame, sBackendCode);
        } else {
            /* store ASM */
            aBackendCode = arg_strAppend(aBackendCode, sBackendCode);
        }

    __next_line:
        if (uLinesIndex < uLinesNum) {
            uLineSize = strGetSize(sLineOrigin);
            uLinesOffset = uLinesOffset + uLineSize + 1;
        }
        strsDeinit(&self->lineBuffs);

        /* exit when finished */
        if (uLinesIndex >= uLinesNum + 1) {
            break;
        }
    }
    if (self->isGenBytecode) {
        /* generate bytecode success */
        sOut = (char*)1;
    } else {
        /* load stored ASM */
        sOut = strsCopy(&self->genBuffs, arg_getStr(aBackendCode));
    }
    goto __exit;
__exit:
    if (NULL != aBackendCode) {
        arg_deinit(aBackendCode);
    }
    if (NULL != aLineConnection) {
        arg_deinit(aLineConnection);
    }
    if (NULL != aDocstring) {
        arg_deinit(aDocstring);
    }
    return sOut;
};

char* parser_lines2Asm(Parser* self, char* sPyLines) {
    self->fn_ast2Target = parser_ast2Asm;
    return parser_lines2Target(self, sPyLines);
}

PIKA_RES pika_lines2Bytes(ByteCodeFrame* bf, char* py_lines) {
#if PIKA_BYTECODE_ONLY_ENABLE
    pika_platform_printf(
        "Error: In bytecode-only mode, can not parse python script.\r\n");
    pika_platform_printf(
        " Note: Please check PIKA_BYTECODE_ONLY_ENABLE config.\r\n");
    return PIKA_RES_ERR_SYNTAX_ERROR;
#else
    Parser* parser = parser_create();
    parser->isGenBytecode = pika_true;
    parser->bytecode_frame = bf;
    if (1 == (uintptr_t)parser_lines2Target(parser, py_lines)) {
        parser_deinit(parser);
        return PIKA_RES_OK;
    }
    parser_deinit(parser);
    return PIKA_RES_ERR_SYNTAX_ERROR;
#endif
}

char* pika_lines2Asm(Args* outBuffs, char* multi_line) {
    Parser* parser = parser_create();
    parser->isGenBytecode = pika_false;
    char* sAsm = parser_lines2Target(parser, multi_line);
    if (NULL == sAsm) {
        parser_deinit(parser);
        return NULL;
    }
    sAsm = strsCopy(outBuffs, sAsm);
    parser_deinit(parser);
    return sAsm;
}

char* pika_file2Target(Args* outBuffs,
                       char* filename,
                       fn_parser_Lines2Target fn) {
    Args buffs = {0};
    Arg* file_arg = arg_loadFile(NULL, filename);
    pika_assert(NULL != file_arg);
    if (NULL == file_arg) {
        pika_platform_printf("Error: Can not open file: %s\r\n", filename);
        return NULL;
    }
    char* lines = (char*)arg_getBytes(file_arg);
    /* replace the "\r\n" to "\n" */
    lines = strsReplace(&buffs, lines, "\r\n", "\n");
    /* clear the void line */
    lines = strsReplace(&buffs, lines, "\n\n", "\n");
    /* add '\n' at the end */
    lines = strsAppend(&buffs, lines, "\n\n");
    Parser* parser = parser_create();
    char* res = fn(parser, lines);
    if (NULL == res) {
        goto __exit;
    }
    res = strsCopy(outBuffs, res);
__exit:
    parser_deinit(parser);
    arg_deinit(file_arg);
    strsDeinit(&buffs);
    return res;
}

int parser_file2TargetFile(Parser* self,
                           char* sPyFile,
                           char* sDocFile,
                           fn_parser_Lines2Target fn) {
    char* sBackendCode = pika_file2Target(&self->genBuffs, sPyFile, fn);
    FILE* fp = pika_platform_fopen(sDocFile, "wb");
    if (NULL == fp) {
        return -1;
    }
    pika_assert(NULL != sBackendCode);
    pika_platform_fwrite(sBackendCode, 1, strGetSize(sBackendCode), fp);
    pika_platform_fclose(fp);
    return 0;
}

char* pika_file2Asm(Args* outBuffs, char* filename) {
    return pika_file2Target(outBuffs, filename, parser_lines2Asm);
}

char* parser_file2Doc(Parser* self, char* sPyFile) {
    return pika_file2Target(&self->genBuffs, sPyFile, parser_lines2Doc);
}

char* _comprehension2Asm(Args* outBuffs,
                         int iBlockDeepth,
                         char* sSubStmt1,
                         char* sSbuStmt2,
                         char* sSubStmt3) {
    Args buffs = {0};
    /*
     * generate code for comprehension:
     * $tmp = []
     * for <substmt2> in <substmt3>:
     *   $tmp.append(<substmt1>)
     */
    Arg* aLineOut = arg_newStr("$tmp = []\n");
    aLineOut = arg_strAppend(
        aLineOut, strsFormat(&buffs, PIKA_LINE_BUFF_SIZE, "for %s in %s:\n",
                             sSbuStmt2, sSubStmt3));
    aLineOut = arg_strAppend(
        aLineOut, strsFormat(&buffs, PIKA_LINE_BUFF_SIZE,
                             "    $tmp.append(%s)\npass\n", sSubStmt1));
    aLineOut = arg_strAddIndentMulti(aLineOut, 4 * iBlockDeepth);
    char* sLineOut = arg_getStr(aLineOut);
    Parser* parser = parser_create();
    char* sAsmOut = parser_lines2Asm(parser, sLineOut);
    size_t lenAsmOut = strGetSize(sAsmOut);
    /* strip B0 */
    sAsmOut[lenAsmOut - 3] = '\0';
    sAsmOut = strsAppend(&buffs, sAsmOut, "0 REF $tmp\n");
    /* skip B0\n */
    sAsmOut = strsCopy(outBuffs, sAsmOut + 3);
    parser_deinit(parser);
    arg_deinit(aLineOut);
    strsDeinit(&buffs);
    return sAsmOut;
}

char* AST_genAsm(AST* oAST, AST* subAst, Args* outBuffs, char* sPikaAsm) {
    int deepth = obj_getInt(oAST, "deepth");
    Args buffs = {0};
    char* buff = args_getBuff(&buffs, PIKA_SPRINTF_BUFF_SIZE);

    /* comprehension */
    if (NULL != AST_getNodeAttr(subAst, "list")) {
        pika_sprintf(buff, "%d %s \n", deepth, "LST");
        sPikaAsm = strsAppend(&buffs, sPikaAsm, buff);
    }

    /* Solve sub stmt */
    while (1) {
        QueueObj* subStmt = queueObj_popObj(subAst);
        if (NULL == subStmt) {
            break;
        }
        obj_setInt(oAST, "deepth", deepth + 1);
        sPikaAsm = AST_genAsm(oAST, subStmt, &buffs, sPikaAsm);
    }

    /* Byte code generate rules */
    const GenRule rules_after[] = {
        {.ins = "RUN", .type = VAL_DYNAMIC, .ast = "method"},
        {.ins = "OPT", .type = VAL_DYNAMIC, .ast = "operator"},
        {.ins = "BYT", .type = VAL_DYNAMIC, .ast = "bytes"},
        {.ins = "NUM", .type = VAL_DYNAMIC, .ast = "num"},
        {.ins = "IMP", .type = VAL_DYNAMIC, .ast = "import"},
        {.ins = "INH", .type = VAL_DYNAMIC, .ast = "inhert"},
        {.ins = "REF", .type = VAL_DYNAMIC, .ast = "ref"},
        {.ins = "STR", .type = VAL_DYNAMIC, .ast = "string"},
        {.ins = "SLC", .type = VAL_NONEVAL, .ast = "slice"},
        {.ins = "DCT", .type = VAL_NONEVAL, .ast = "dict"},
        {.ins = "TPL", .type = VAL_NONEVAL, .ast = "tuple"},
        {.ins = "NLS", .type = VAL_NONEVAL, .ast = "list"},
        {.ins = "OUT", .type = VAL_DYNAMIC, .ast = "left"}};

    /* comprehension */
    if (NULL != AST_getNodeAttr(oAST, "comprehension")) {
        int iBlockDeepth = AST_getBlockDeepthNow(oAST);
        char* sSubStmt1 = AST_getNodeAttr(oAST, "substmt1");
        char* sSubStmt2 = AST_getNodeAttr(oAST, "substmt2");
        char* sSubStmt3 = AST_getNodeAttr(oAST, "substmt3");
        sPikaAsm =
            strsAppend(&buffs, sPikaAsm,
                       _comprehension2Asm(&buffs, iBlockDeepth, sSubStmt1,
                                          sSubStmt2, sSubStmt3));
    }

    /* append the syntax item */
    for (size_t i = 0; i < sizeof(rules_after) / sizeof(GenRule); i++) {
        GenRule rule = rules_after[i];
        char* sNodeVal = AST_getNodeAttr(subAst, rule.ast);
        if (NULL != sNodeVal) {
            /* e.g. "0 RUN print \n" */
            pika_sprintf(buff, "%d %s ", deepth, rule.ins);
            Arg* aBuff = arg_newStr(buff);
            if (rule.type == VAL_DYNAMIC) {
                aBuff = arg_strAppend(aBuff, sNodeVal);
            }
            aBuff = arg_strAppend(aBuff, "\n");
            sPikaAsm = strsAppend(&buffs, sPikaAsm, arg_getStr(aBuff));
            arg_deinit(aBuff);
        }
    }

    obj_setInt(oAST, "deepth", deepth - 1);
    goto __exit;
__exit:
    sPikaAsm = strsCopy(outBuffs, sPikaAsm);
    strsDeinit(&buffs);
    return sPikaAsm;
}

char* ASM_addBlockDeepth(AST* ast,
                         Args* buffs_p,
                         char* pikaAsm,
                         uint8_t deepthOffset) {
    pikaAsm = strsAppend(buffs_p, pikaAsm, (char*)"B");
    char buff[11];
    pikaAsm =
        strsAppend(buffs_p, pikaAsm,
                   fast_itoa(buff, AST_getBlockDeepthNow(ast) + deepthOffset));
    pikaAsm = strsAppend(buffs_p, pikaAsm, (char*)"\n");
    return pikaAsm;
}

char* GenRule_toAsm(GenRule rule,
                    Args* buffs,
                    AST* ast,
                    char* pikaAsm,
                    int deepth) {
    char* buff = args_getBuff(buffs, PIKA_SPRINTF_BUFF_SIZE);
    /* parse stmt ast */
    pikaAsm = AST_genAsm(ast, ast, buffs, pikaAsm);
    /* e.g. "0 CTN \n" */
    pika_sprintf(buff, "%d %s ", deepth, rule.ins);
    Arg* aBuff = arg_newStr(buff);
    if (rule.type == VAL_DYNAMIC) {
        aBuff = arg_strAppend(aBuff, obj_getStr(ast, rule.ast));
    }
    if (rule.type == VAL_STATIC_) {
        aBuff = arg_strAppend(aBuff, rule.val);
    }
    aBuff = arg_strAppend(aBuff, "\n");
    pikaAsm = strsAppend(buffs, pikaAsm, arg_getStr(aBuff));
    arg_deinit(aBuff);
    return pikaAsm;
}

char* AST_genAsm_top(AST* oAST, Args* outBuffs) {
    const GenRule rules_topAst[] = {
        {.ins = "CTN", .type = VAL_NONEVAL, .ast = "continue"},
        {.ins = "BRK", .type = VAL_NONEVAL, .ast = "break"},
        {.ins = "DEL", .type = VAL_DYNAMIC, .ast = "del"},
        {.ins = "GLB", .type = VAL_DYNAMIC, .ast = "global"},
        {.ins = "RIS", .type = VAL_DYNAMIC, .ast = "raise"},
        {.ins = "ASS", .type = VAL_NONEVAL, .ast = "assert"},
        {.ins = "RET", .type = VAL_NONEVAL, .ast = "return"}};

    /* generate code for block ast */
    const GenRule rules_block[] = {
        {.ins = "TRY", .type = VAL_NONEVAL, .ast = "try"},
        {.ins = "EXP", .type = VAL_NONEVAL, .ast = "except"},
        {.ins = "NEL", .type = VAL_STATIC_, .ast = "else", .val = "1"},
        {.ins = "JEZ", .type = VAL_STATIC_, .ast = "if", .val = "1"},
        {.ins = "JEZ", .type = VAL_STATIC_, .ast = "while", .val = "2"},
    };
    Args buffs = {0};
    char* sPikaAsm = strsCopy(&buffs, "");
    QueueObj* oExitBlock;
    pika_bool bblockMatched = 0;

    if (NULL == oAST) {
        sPikaAsm = NULL;
        goto __exit;
    }

    /* skip for docsting */
    if (NULL != AST_getNodeAttr(oAST, "docstring")) {
        goto __exit;
    }

    oExitBlock = obj_getObj(oAST, "exitBlock");
    /* exiting from block */
    if (oExitBlock != NULL) {
        while (1) {
            uint8_t uDeepthOffset = obj_getInt(oExitBlock, "top") -
                                    obj_getInt(oExitBlock, "bottom") - 1;
            char* sBlockType = queueObj_popStr(oExitBlock);
            if (NULL == sBlockType) {
                break;
            }
            /* goto the while start when exit while block */
            if (strEqu(sBlockType, "while")) {
                sPikaAsm =
                    ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm, uDeepthOffset);
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 JMP -1\n");
            }
#if PIKA_SYNTAX_EXCEPTION_ENABLE
            /* goto the while start when exit while block */
            if (strEqu(sBlockType, "try")) {
                sPikaAsm =
                    ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm, uDeepthOffset);
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 NTR \n");
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 GER \n");
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 JEZ 2\n");
            }
#endif
            /* goto the while start when exit while block */
            if (strEqu(sBlockType, "for")) {
                sPikaAsm =
                    ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm, uDeepthOffset);
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 JMP -1\n");
                /* garbage collect for the list */
                sPikaAsm =
                    ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm, uDeepthOffset);
                char _l_x[] = "$lx";
                char block_deepth_char =
                    AST_getBlockDeepthNow(oAST) + uDeepthOffset + '0';
                _l_x[sizeof(_l_x) - 2] = block_deepth_char;
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 DEL ");
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)_l_x);
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"\n");
            }
            /* return when exit method */
            if (strEqu(sBlockType, "def")) {
                sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm,
                                              uDeepthOffset + 1);
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 RET \n");
            }
            /* return when exit class */
            if (strEqu(sBlockType, "class")) {
                sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm,
                                              uDeepthOffset + 1);
                sPikaAsm =
                    strsAppend(outBuffs, sPikaAsm, (char*)"0 RAS $origin\n");
                sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm, 1);
                sPikaAsm =
                    strsAppend(outBuffs, sPikaAsm, (char*)"0 NEW self\n");
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 RET \n");
            }
        }
    }
    /* add block deepth */
    /* example: B0 */
    sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm, 0);

    /* "deepth" is invoke deepth, not the blockDeepth */
    obj_setInt(oAST, "deepth", 0);

    /* match block */
    bblockMatched = 0;
    if (strEqu(AST_getThisBlock(oAST), "for")) {
        /* for "for" iter */
        char* sArgIn = AST_getNodeAttr(oAST, "arg_in");
#if !PIKA_NANO_ENABLE
        char* sArgInKv = NULL;
#endif
        Arg* aNewAsm = arg_newStr("");
        char _l_x[] = "$lx";
        char sBlockDeepthCHar = '0';
        sBlockDeepthCHar += AST_getBlockDeepthNow(oAST);
        _l_x[sizeof(_l_x) - 2] = sBlockDeepthCHar;
        /* init iter */
        /*     get the iter(_l<x>) */
        sPikaAsm = AST_genAsm(oAST, oAST, &buffs, sPikaAsm);
        aNewAsm = arg_strAppend(aNewAsm, "0 OUT ");
        aNewAsm = arg_strAppend(aNewAsm, _l_x);
        aNewAsm = arg_strAppend(aNewAsm, "\n");
        sPikaAsm = strsAppend(&buffs, sPikaAsm, arg_getStr(aNewAsm));
        arg_deinit(aNewAsm);
        aNewAsm = arg_newStr("");
        /* get next */
        /*     run next(_l<x>) */
        /*     check item is exist */
        /*
            $n = $lx.next()
            EST $n
            k, v = $n
            DEL $n
        */

#if !PIKA_NANO_ENABLE
        if (_check_is_multi_assign(sArgIn)) {
            sArgInKv = sArgIn;
            sArgIn = "$tmp";
        }
#endif

        sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm, 0);
        aNewAsm = arg_strAppend(aNewAsm, "0 RUN ");
        aNewAsm = arg_strAppend(aNewAsm, _l_x);
        aNewAsm = arg_strAppend(aNewAsm,
                                ".__next__\n"
                                "0 OUT ");
        aNewAsm = arg_strAppend(aNewAsm, sArgIn);
        aNewAsm = arg_strAppend(aNewAsm,
                                "\n"
                                "0 EST ");
        aNewAsm = arg_strAppend(aNewAsm, sArgIn);
        aNewAsm = arg_strAppend(aNewAsm, "\n0 JEZ 2\n");
        sPikaAsm = strsAppend(&buffs, sPikaAsm, arg_getStr(aNewAsm));
        arg_deinit(aNewAsm);

#if !PIKA_NANO_ENABLE
        if (NULL != sArgInKv) {
            int out_num = 0;
            while (1) {
                char* item = Cursor_popToken(&buffs, &sArgInKv, ",");
                if (item[0] == '\0') {
                    break;
                }
                char* stmt = strsFormat(&buffs, PIKA_LINE_BUFF_SIZE,
                                        "%s = $tmp[%d]\n", item, out_num);

                AST* ast_this = line2Ast_withBlockDeepth(
                    stmt, AST_getBlockDeepthNow(oAST) + 1);
                sPikaAsm = strsAppend(&buffs, sPikaAsm,
                                      AST_genAsm_top(ast_this, &buffs));
                AST_deinit(ast_this);
                out_num++;
            }
            sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm, 1);
            sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 DEL $tmp\n");
        }
#endif

        bblockMatched = 1;
        goto __exit;
    }
    if (strEqu(AST_getThisBlock(oAST), "elif")) {
        /* skip if __else is 0 */
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 NEL 1\n");
        /* parse stmt ast */
        sPikaAsm = AST_genAsm(oAST, oAST, &buffs, sPikaAsm);
        /* skip if stmt is 0 */
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 JEZ 1\n");
        bblockMatched = 1;
        goto __exit;
    }
    if (strEqu(AST_getThisBlock(oAST), "def")) {
#if !PIKA_NANO_ENABLE
        char* sDefaultStmts = AST_getNodeAttr(oAST, "default");
#endif
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 DEF ");
        sPikaAsm =
            strsAppend(&buffs, sPikaAsm, AST_getNodeAttr(oAST, "declare"));
        sPikaAsm = strsAppend(&buffs, sPikaAsm,
                              "\n"
                              "0 JMP 1\n");

#if !PIKA_NANO_ENABLE
        if (NULL != sDefaultStmts) {
            int iStmtNum =
                _Cursor_count(sDefaultStmts, TOKEN_devider, ",", pika_true) + 1;
            for (int i = 0; i < iStmtNum; i++) {
                char* sStmt = Cursor_popToken(&buffs, &sDefaultStmts, ",");
                char* sArgName = strsGetFirstToken(&buffs, sStmt, '=');
                sPikaAsm = ASM_addBlockDeepth(oAST, &buffs, sPikaAsm, 1);
                sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 EST ");
                sPikaAsm = strsAppend(&buffs, sPikaAsm, sArgName);
                sPikaAsm = strsAppend(&buffs, sPikaAsm, "\n");
                sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 JNZ 2\n");
                AST* ast_this = line2Ast_withBlockDeepth(
                    sStmt, AST_getBlockDeepthNow(oAST) + 1);
                sPikaAsm = strsAppend(&buffs, sPikaAsm,
                                      AST_genAsm_top(ast_this, &buffs));
                AST_deinit(ast_this);
            }
        }
#endif

        bblockMatched = 1;
        goto __exit;
    }

    if (strEqu(AST_getThisBlock(oAST), "class")) {
        char* sDeclare = obj_getStr(oAST, "declare");
        char* sThisClass = NULL;
        char* sSuperClass = NULL;
        if (strIsContain(sDeclare, '(')) {
            sThisClass = strsGetFirstToken(&buffs, sDeclare, '(');
            sSuperClass = strsCut(&buffs, sDeclare, '(', ')');
        } else {
            sThisClass = sDeclare;
            sSuperClass = "";
        }
        if (strEqu("", sSuperClass)) {
            /* default superClass */
            sSuperClass = "TinyObj";
        }
        if (strEqu("TinyObj", sSuperClass)) {
            /* default superClass */
            sSuperClass = "TinyObj";
        }
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 CLS ");
        sPikaAsm = strsAppend(&buffs, sPikaAsm,
                              strsAppend(&buffs, sThisClass,
                                         "()\n"
                                         "0 JMP 1\n"));
        char sBlockDeepth[] = "B0\n";
        /* goto deeper block */
        sBlockDeepth[1] += AST_getBlockDeepthNow(oAST) + 1;
        sPikaAsm = strsAppend(&buffs, sPikaAsm, sBlockDeepth);
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 RUN ");
        sPikaAsm = strsAppend(&buffs, sPikaAsm, sSuperClass);
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "\n");
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 OUT self\n");
        sPikaAsm = strsAppend(&buffs, sPikaAsm, sBlockDeepth);
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 RAS self\n");
        bblockMatched = 1;
        goto __exit;
    }

    for (size_t i = 0; i < sizeof(rules_block) / sizeof(GenRule); i++) {
        GenRule rule = rules_block[i];
        if (strEqu(AST_getThisBlock(oAST), rule.ast)) {
            sPikaAsm = GenRule_toAsm(rule, &buffs, oAST, sPikaAsm, 0);
            bblockMatched = 1;
            goto __exit;
        }
    }

    /* generate code for top level ast */
    for (size_t i = 0; i < sizeof(rules_topAst) / sizeof(rules_topAst[0]);
         i++) {
        GenRule item = rules_topAst[i];
        if (obj_isArgExist(oAST, item.ast)) {
            sPikaAsm = GenRule_toAsm(item, &buffs, oAST, sPikaAsm, 0);
            bblockMatched = 1;
            goto __exit;
        }
    }
__exit:
    if (NULL == sPikaAsm) {
        strsDeinit(&buffs);
        return NULL;
    }
    if (!bblockMatched) {
        /* parse stmt ast */
        sPikaAsm = AST_genAsm(oAST, oAST, &buffs, sPikaAsm);
    }

    /* output pikaAsm */
    sPikaAsm = strsCopy(outBuffs, sPikaAsm);
    strsDeinit(&buffs);
    return sPikaAsm;
}

#define IS_SPACE_OR_TAB(ch) ((ch) == ' ' || (ch) == '\t')
static pika_bool _strCheckCodeBlockFlag(char* sLine) {
    pika_bool bStart = pika_false, bEnd = pika_false;
    char *sStart = sLine, *pEnd = sLine + strlen(sLine) - 1;
    while (sStart <= pEnd && IS_SPACE_OR_TAB(*sStart)) {
        sStart++;
    }
    while (pEnd >= sStart && IS_SPACE_OR_TAB(*pEnd)) {
        pEnd--;
    }
    if (pEnd - sStart < 2) {
        return pika_false;
    }
    if (strncmp(sStart, "```", 3) == 0) {
        bStart = pika_true;
    }
    if (pEnd - sStart >= 5 && strncmp(pEnd - 2, "```", 3) == 0) {
        bEnd = pika_true;
    }
    if (bStart && bEnd) {
        return pika_false;
    }
    if (bStart || bEnd) {
        return pika_true;
    }
    return pika_false;
}

static char* _parser_fixDocStringIndent(Parser* self,
                                        char* sDocString,
                                        int iIndent) {
    Args buffs = {0};
    char* sBuff = strsCopy(&buffs, sDocString);
    Arg* aOut = arg_newStr("");
    char* sOut = NULL;
    uint32_t iLineNum = strCountSign(sBuff, '\n');
    pika_bool bInCodeBlock = pika_false;
    int iIndentCodeBlock = 0;
    for (int i = 0; i < iLineNum; i++) {
        char* sLine = strsPopToken(&buffs, &sBuff, '\n');
        if (strIsBlank(sLine)) {
            continue;
        }
        int iIndentThis = strGetIndent(sLine);
        int iIndentStrip = iIndentThis;
        pika_bool bCodeBlockFlag = _strCheckCodeBlockFlag(sLine);
        if (bCodeBlockFlag) {
            bInCodeBlock = !bInCodeBlock;
            iIndentCodeBlock = iIndentStrip;
        }
        if (bInCodeBlock) {
            iIndentStrip = iIndentCodeBlock;
        }
        if (strGetIndent(sLine) >= iIndentStrip) {
            sLine = sLine + iIndentStrip;
        }
        for (int k = 0; k < iIndent; k++) {
            aOut = arg_strAppend(aOut, " ");
        }
        aOut = arg_strAppend(aOut, sLine);
        aOut = arg_strAppend(aOut, "\n");
        if (!bInCodeBlock) {
            iIndentCodeBlock = 0;
            aOut = arg_strAppend(aOut, "\n");
        }
    }
    sOut = strsCopy(&self->lineBuffs, arg_getStr(aOut));
    strsDeinit(&buffs);
    arg_deinit(aOut);
    return sOut;
}

char* parser_ast2Doc(Parser* self, AST* oAST) {
    if (strEqu(AST_getThisBlock(oAST), "def")) {
        char* sDeclare = AST_getNodeAttr(oAST, "raw");
        int blockDeepth = AST_getBlockDeepthNow(oAST);
        self->thisBlockDeepth = blockDeepth;
        char* sOut =
            strsFormat(&self->lineBuffs, 2048, "def %s:...\r\n", sDeclare);
        for (int i = 0; i < blockDeepth; i++) {
            sOut = strsAppend(&self->lineBuffs, "", sOut);
        }
        sOut =
            strsFormat(&self->lineBuffs, 2048, "``` python\n%s```\n\n", sOut);
        return sOut;
    };
    if (strEqu(AST_getThisBlock(oAST), "class")) {
        char* sDeclare = obj_getStr(oAST, "declare");
        int blockDeepth = AST_getBlockDeepthNow(oAST);
        self->thisBlockDeepth = blockDeepth;
        return strsFormat(&self->lineBuffs, 2048, "### class %s:\r\n",
                          sDeclare);
    };
    char* sDocString = AST_getNodeAttr(oAST, "docstring");
    if (NULL != sDocString) {
        sDocString = _parser_fixDocStringIndent(self, sDocString, 0);
        return strsAppend(&self->lineBuffs, sDocString, "\n");
    }
    return "";
}

int parser_file2DocFile(Parser* self, char* sPyFile, char* sDocFile) {
    return parser_file2TargetFile(self, sPyFile, sDocFile, parser_lines2Doc);
}

char* parser_lines2Doc(Parser* self, char* sPyLines) {
    self->fn_ast2Target = parser_ast2Doc;
    return parser_lines2Target(self, sPyLines);
}

char* parser_ast2Asm(Parser* self, AST* ast) {
    return AST_genAsm_top(ast, &self->lineBuffs);
}

int32_t AST_deinit(AST* ast) {
    return obj_deinit(ast);
}

ByteCodeFrame* byteCodeFrame_appendFromAsm(ByteCodeFrame* self,
                                           char* sPikaAsm) {
    Asmer asmer = {
        .asm_code = sPikaAsm,
        .block_deepth_now = 0,
        .is_new_line = 0,
        .line_pointer = sPikaAsm,
    };
    uint16_t uConstPoolOffset;
    uint16_t uExistOffset;
    for (int i = 0; i < strCountSign(sPikaAsm, '\n'); i++) {
        Args buffs = {0};
        char* sLine = strsGetLine(&buffs, asmer.line_pointer);
        char* sData = NULL;
        char sIns[4] = "";
        char sInvokeDeepth[3] = "";
        uint8_t uSpaceNum = 0;
        uint8_t iInvokeDeepth = 0;
        uint8_t iInsStr = 0;
        Arg* aLineBuff = arg_newStr(sLine);
        strsDeinit(&buffs);
        sLine = arg_getStr(aLineBuff);
        InstructUnit ins_unit = {0};
        /* remove '\r' */
        if (sLine[strGetSize(sLine) - 1] == '\r') {
            sLine[strGetSize(sLine) - 1] = 0;
        }
        /* process block deepth flag*/
        if ('B' == sLine[0]) {
            asmer.block_deepth_now = fast_atoi(sLine + 1);
            asmer.is_new_line = 1;
            goto __next_line;
        }

        /* process each ins */

        /* get constPool offset */
        uConstPoolOffset = 0;

        for (int i = 0; i < (int)strGetSize(sLine); i++) {
            if (uSpaceNum < 2) {
                if (sLine[i] == ' ') {
                    uSpaceNum++;
                    if (uSpaceNum == 2) {
                        sData = sLine + i + 1;
                        break;
                    }
                    continue;
                }
            }
            if (uSpaceNum == 0) {
                sInvokeDeepth[iInvokeDeepth++] = sLine[i];
                continue;
            }
            if (uSpaceNum == 1) {
                sIns[iInsStr++] = sLine[i];
                continue;
            }
        }

        uExistOffset = constPool_getOffsetByData(&(self->const_pool), sData);

        /* get const offset */
        if (strEqu(sData, "")) {
            /* not need const value */
            uConstPoolOffset = 0;
        } else if (65535 == uExistOffset) {
            /* push new const value */
            uConstPoolOffset = constPool_getLastOffset(&(self->const_pool));
            /* load const to const pool buff */
            constPool_append(&(self->const_pool), sData);
        } else {
            /* use exist const value */
            uConstPoolOffset = uExistOffset;
        }

        iInvokeDeepth = fast_atoi(sInvokeDeepth);
        /* load Asm to byte code unit */
        instructUnit_setBlockDeepth(&ins_unit, asmer.block_deepth_now);
        instructUnit_setInvokeDeepth(&ins_unit, iInvokeDeepth);
        instructUnit_setConstPoolIndex(&ins_unit, uConstPoolOffset);
        instructUnit_setInstruct(&ins_unit, pikaVM_getInstructFromAsm(sIns));
        if (asmer.is_new_line) {
            instructUnit_setIsNewLine(&ins_unit, 1);
            asmer.is_new_line = 0;
        }

        /* append instructUnit to instructArray */
        instructArray_append(&(self->instruct_array), &ins_unit);

    __next_line:
        /* point to next line */
        asmer.line_pointer += strGetLineSize(asmer.line_pointer) + 1;
        arg_deinit(aLineBuff);
    }
    return self;
}

char* pika_lines2Array(char* sLines) {
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    pika_lines2Bytes(&bytecode_frame, sLines);
    /* do something */
    byteCodeFrame_print(&bytecode_frame);

    pika_platform_printf("\n\n/* clang-format off */\n");
    pika_platform_printf("PIKA_PYTHON(\n");
    pika_platform_printf("%s\n", sLines);
    pika_platform_printf(")\n");
    pika_platform_printf("/* clang-format on */\n");
    byteCodeFrame_printAsArray(&bytecode_frame);
    /* deinit */
    byteCodeFrame_deinit(&bytecode_frame);
    pika_platform_printf("\n\n");
    return NULL;
}

Parser* parser_create(void) {
    Parser* self = (Parser*)pikaMalloc(sizeof(Parser));
    pika_platform_memset(self, 0, sizeof(Parser));
    self->blockState.stack = pikaMalloc(sizeof(Stack));
    /* generate asm as default */
    self->fn_ast2Target = parser_ast2Asm;
    pika_platform_memset(self->blockState.stack, 0, sizeof(Stack));
    stack_init(self->blockState.stack);
    /* -1 means not inited */
    self->blockDeepthOrigin = _VAL_NEED_INIT;
    return self;
}

int parser_deinit(Parser* self) {
    stack_deinit(self->blockState.stack);
    strsDeinit(&self->genBuffs);
    pikaFree(self->blockState.stack, sizeof(Stack));
    pikaFree(self, sizeof(Parser));
    return 0;
}
